// Copyright June Rhodes. All Rights Reserved.

#include "OnlineAvatarInterfaceRedpointSteam.h"
#include "Containers/Ticker.h"
#include "Dom/JsonObject.h"
#include "Engine/Texture2D.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "LogRedpointSteam.h"
#include "Misc/ConfigCacheIni.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "OnlineSubsystemRedpointSteam.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include <limits>

#if EOS_STEAM_ENABLED

THIRD_PARTY_INCLUDES_START
#include "steam/steam_api.h"
THIRD_PARTY_INCLUDES_END

UTexture2D *ConvertSteamPictureToAvatarOSS(int Picture)
{
    uint32 Width;
    uint32 Height;
    SteamUtils()->GetImageSize(Picture, &Width, &Height);

    size_t BufferSize = (size_t)Width * Height * 4;
    if (Width > 0 && Height > 0 && BufferSize < (size_t)std::numeric_limits<int>::max())
    {
        uint8 *AvatarRGBA = (uint8 *)FMemory::MallocZeroed(BufferSize);
        SteamUtils()->GetImageRGBA(Picture, AvatarRGBA, BufferSize);
        for (uint32 i = 0; i < (Width * Height * 4); i += 4)
        {
            uint8 Temp = AvatarRGBA[i + 0];
            AvatarRGBA[i + 0] = AvatarRGBA[i + 2];
            AvatarRGBA[i + 2] = Temp;
        }

        UTexture2D *Avatar = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
#if defined(UE_5_0_OR_LATER)
        uint8 *MipData = (uint8 *)Avatar->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
#else
        uint8 *MipData = (uint8 *)Avatar->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
#endif // #if defined(UE_5_0_OR_LATER)
        FMemory::Memcpy(MipData, (void *)AvatarRGBA, BufferSize);
#if defined(UE_5_0_OR_LATER)
        Avatar->GetPlatformData()->Mips[0].BulkData.Unlock();
        Avatar->GetPlatformData()->SetNumSlices(1);
#else
        Avatar->PlatformData->Mips[0].BulkData.Unlock();
        Avatar->PlatformData->SetNumSlices(1);
#endif // #if defined(UE_5_0_OR_LATER)
        Avatar->NeverStream = true;
        Avatar->UpdateResource();

        return Avatar;
    }

    return nullptr;
}

void FOnlineAvatarInterfaceRedpointSteam::OnProcessAvatarUrlRequestComplete(
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FHttpRequestPtr Request,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FHttpResponsePtr Response,
    bool bConnectedSuccessfully,
    FString DefaultAvatarUrl,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FOnGetAvatarUrlComplete OnComplete)
{
    const FString Content = Response->GetContentAsString();

    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Content);

    if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
    {
        TSharedPtr<FJsonObject> JsonResponse = JsonObject->GetObjectField("response");
        if (JsonResponse.IsValid())
        {
            TArray<TSharedPtr<FJsonValue>> JsonPlayers = JsonResponse->GetArrayField("players");
            if (JsonPlayers.Num() > 0)
            {
                TSharedPtr<FJsonObject> JsonPlayer = JsonPlayers[0]->AsObject();
                OnComplete.ExecuteIfBound(true, JsonPlayer->GetStringField("avatarfull"));
                return;
            }
        }
    }

    OnComplete.ExecuteIfBound(false, MoveTemp(DefaultAvatarUrl));
}

bool FOnlineAvatarInterfaceRedpointSteam::GetAvatar(
    const FUniqueNetId &LocalUserId,
    const FUniqueNetId &TargetUserId,
    TSoftObjectPtr<UTexture> DefaultTexture,
    FOnGetAvatarComplete OnComplete)
{
    if (TargetUserId.GetType() != STEAM_SUBSYSTEM)
    {
        UE_LOG(LogRedpointSteam, Error, TEXT("GetAvatar: TargetUserId is non-Steam user."));
        OnComplete.ExecuteIfBound(false, DefaultTexture);
        return true;
    }
    if (!LocalUserId.DoesSharedInstanceExist())
    {
        UE_LOG(LogRedpointSteam, Error, TEXT("GetAvatar: LocalUserId is not a shareable FUniqueNetId."));
        OnComplete.ExecuteIfBound(false, DefaultTexture);
        return true;
    }
    if (!TargetUserId.DoesSharedInstanceExist())
    {
        UE_LOG(LogRedpointSteam, Error, TEXT("GetAvatar: TargetUserId is not a shareable FUniqueNetId."));
        OnComplete.ExecuteIfBound(false, DefaultTexture);
        return true;
    }

    // Cheat. We can't access FUniqueNetIdSteam directly, but we do know it returns
    // the CSteamID as a uint64 from GetBytes :)
    uint64 SteamID = *(uint64 *)TargetUserId.GetBytes();

    int Picture = SteamFriends()->GetLargeFriendAvatar(SteamID);
    if (Picture == 0)
    {
        // User has no avatar set.
        OnComplete.ExecuteIfBound(false, DefaultTexture);
        return true;
    }

    if (Picture != -1)
    {
        auto Avatar = ConvertSteamPictureToAvatarOSS(Picture);
        OnComplete.ExecuteIfBound(IsValid(Avatar), IsValid(Avatar) ? Avatar : DefaultTexture);
        return true;
    }
    else
    {
        // Normally we'd wait for AvatarImageLoaded_t, but the Steam OSS implementation is bad
        // and it only ticks the Steam callbacks if it thinks it's expecting one internally. So
        // instead, we just check occasionally until it's done...
        FUTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateLambda([WeakThis = GetWeakThis(this),
                                           LocalUserIdShared = LocalUserId.AsShared(),
                                           TargetUserIdShared = TargetUserId.AsShared(),
                                           DefaultTexture,
                                           OnComplete](float DeltaTime) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    This->GetAvatar(LocalUserIdShared.Get(), TargetUserIdShared.Get(), DefaultTexture, OnComplete);
                }
                return false;
            }),
            0.2f);
        return true;
    }
}

bool FOnlineAvatarInterfaceRedpointSteam::GetAvatarUrl(
    const FUniqueNetId &LocalUserId,
    const FUniqueNetId &TargetUserId,
    FString DefaultAvatarUrl,
    FOnGetAvatarUrlComplete OnComplete)
{
    if (TargetUserId.GetType() != STEAM_SUBSYSTEM)
    {
        UE_LOG(LogRedpointSteam, Error, TEXT("GetAvatarUrl: TargetUserId is non-Steam user."));
        OnComplete.ExecuteIfBound(false, DefaultAvatarUrl);
        return true;
    }
    if (!LocalUserId.DoesSharedInstanceExist())
    {
        UE_LOG(LogRedpointSteam, Error, TEXT("GetAvatarUrl: LocalUserId is not a shareable FUniqueNetId."));
        OnComplete.ExecuteIfBound(false, DefaultAvatarUrl);
        return true;
    }
    if (!TargetUserId.DoesSharedInstanceExist())
    {
        UE_LOG(LogRedpointSteam, Error, TEXT("GetAvatarUrl: TargetUserId is not a shareable FUniqueNetId."));
        OnComplete.ExecuteIfBound(false, DefaultAvatarUrl);
        return true;
    }

    // Cheat. We can't access FUniqueNetIdSteam directly, but we do know it returns
    // the CSteamID as a uint64 from GetBytes :)
    uint64 SteamID = *(uint64 *)TargetUserId.GetBytes();

    FOnlineSubsystemRedpointSteam *OnlineSubsystemSteam =
        // NOLINTNEXTLINE(unreal-ionlinesubsystem-get)
        static_cast<FOnlineSubsystemRedpointSteam *>(IOnlineSubsystem::Get(REDPOINT_STEAM_SUBSYSTEM));
    if (OnlineSubsystemSteam)
    {
        const FString WebApiKey = OnlineSubsystemSteam->GetWebApiKey();
        if (!WebApiKey.IsEmpty())
        {
            const FString PlayerUrl = FString::Printf(
                TEXT("https://api.steampowered.com/ISteamUser/GetPlayerSummaries/v0002/?key=%s&steamids=%llu"),
                *WebApiKey,
                SteamID);

            auto Request = FHttpModule::Get().CreateRequest();
            Request->SetVerb("GET");
            Request->SetURL(PlayerUrl);
            Request->OnProcessRequestComplete().BindThreadSafeSP(
                AsShared(),
                &FOnlineAvatarInterfaceRedpointSteam::OnProcessAvatarUrlRequestComplete,
                DefaultAvatarUrl,
                OnComplete);

            Request->ProcessRequest();
        }
        else
        {
            UE_LOG(LogRedpointSteam, Error, TEXT("GetAvatarUrl: Web API Key is empty."));
            OnComplete.ExecuteIfBound(false, DefaultAvatarUrl);
        }
    }
    else
    {
        UE_LOG(LogRedpointSteam, Error, TEXT("GetAvatarUrl: FOnlineSubsystemRedpointSteam not valid."));
        OnComplete.ExecuteIfBound(false, DefaultAvatarUrl);
    }

    return true;
}

#endif
