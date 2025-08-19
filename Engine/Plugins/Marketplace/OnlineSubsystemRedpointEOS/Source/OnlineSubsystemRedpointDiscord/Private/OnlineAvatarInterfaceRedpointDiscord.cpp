// Copyright June Rhodes. All Rights Reserved.

#include "OnlineAvatarInterfaceRedpointDiscord.h"

#if EOS_DISCORD_ENABLED

#include "HttpModule.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Interfaces/IHttpResponse.h"
#include "LogRedpointDiscord.h"
#include "OnlineSubsystemRedpointDiscordConstants.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSTextureLoader.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "UniqueNetIdRedpointDiscord.h"

void FOnlineAvatarInterfaceRedpointDiscord::OnDownloadAvatarComplete(
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FHttpRequestPtr Request,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FHttpResponsePtr Response,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    bool bConnectedSuccessfully,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSoftObjectPtr<UTexture> DefaultTexture,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FOnGetAvatarComplete OnComplete)
{
    if (!bConnectedSuccessfully || !Response.IsValid() || !EHttpResponseCodes::IsOk(Response->GetResponseCode()))
    {
        UE_LOG(LogRedpointDiscord, Error, TEXT("GetAvatar: HTTP request to download avatar from Discord failed."));
        OnComplete.ExecuteIfBound(false, DefaultTexture);
        return;
    }

    UTexture *LoadedTexture = FEOSTextureLoader::LoadTextureFromHttpResponse(Response);
    if (IsValid(LoadedTexture))
    {
        OnComplete.ExecuteIfBound(true, LoadedTexture);
    }
    else
    {
        OnComplete.ExecuteIfBound(false, DefaultTexture);
    }
}

void FOnlineAvatarInterfaceRedpointDiscord::DownloadAvatar(
    const FString &UserId,
    const FString &AvatarHash,
    TSoftObjectPtr<UTexture> DefaultTexture,
    FOnGetAvatarComplete OnComplete)
{
    auto Request = FHttpModule::Get().CreateRequest();
    Request->SetVerb("GET");
    Request->SetURL(
        FString::Printf(TEXT("https://cdn.discordapp.com/avatars/%s/%s.png?size=256"), *UserId, *AvatarHash));
    Request->OnProcessRequestComplete().BindThreadSafeSP(
        this,
        &FOnlineAvatarInterfaceRedpointDiscord::OnDownloadAvatarComplete,
        MoveTemp(DefaultTexture),
        MoveTemp(OnComplete));
    Request->ProcessRequest();
}

bool FOnlineAvatarInterfaceRedpointDiscord::GetAvatar(
    const FUniqueNetId &LocalUserId,
    const FUniqueNetId &TargetUserId,
    TSoftObjectPtr<UTexture> DefaultTexture,
    FOnGetAvatarComplete OnComplete)
{
    if (TargetUserId.GetType() != REDPOINT_DISCORD_SUBSYSTEM)
    {
        UE_LOG(LogRedpointDiscord, Error, TEXT("GetAvatar: TargetUserId is non-Discord user."));
        OnComplete.ExecuteIfBound(false, DefaultTexture);
        return true;
    }

    TSharedRef<const FUniqueNetIdRedpointDiscord> TargetUserIdDiscord =
        StaticCastSharedRef<const FUniqueNetIdRedpointDiscord>(TargetUserId.AsShared());

    this->Instance->UserManager().GetUser(
        TargetUserIdDiscord->GetUserId(),
        [WeakThis = GetWeakThis(this), DefaultTexture, OnComplete](discord::Result Result, const discord::User &User) {
            if (auto This = PinWeakThis(WeakThis))
            {
                This->DownloadAvatar(
                    FString::Printf(TEXT("%lld"), User.GetId()),
                    ANSI_TO_TCHAR(User.GetAvatar()),
                    DefaultTexture,
                    OnComplete);
            }
        });
    return true;
}

bool FOnlineAvatarInterfaceRedpointDiscord::GetAvatarUrl(
    const FUniqueNetId &LocalUserId,
    const FUniqueNetId &TargetUserId,
    FString DefaultAvatarUrl,
    FOnGetAvatarUrlComplete OnComplete)
{
    if (TargetUserId.GetType() != REDPOINT_DISCORD_SUBSYSTEM)
    {
        UE_LOG(LogRedpointDiscord, Error, TEXT("GetAvatarUrl: TargetUserId is non-Discord user."));
        OnComplete.ExecuteIfBound(false, DefaultAvatarUrl);
        return true;
    }

    TSharedRef<const FUniqueNetIdRedpointDiscord> TargetUserIdDiscord =
        StaticCastSharedRef<const FUniqueNetIdRedpointDiscord>(TargetUserId.AsShared());

    this->Instance->UserManager().GetUser(
        TargetUserIdDiscord->GetUserId(),
        [WeakThis = GetWeakThis(this),
         DefaultAvatarUrl,
         OnComplete](discord::Result Result, const discord::User &User) {
            if (auto This = PinWeakThis(WeakThis))
            {
                const FString AvatarUrl = FString::Printf(
                    TEXT("https://cdn.discordapp.com/avatars/%s/%s.png?size=256"),
                    *FString::Printf(TEXT("%lld"), User.GetId()),
                    ANSI_TO_TCHAR(User.GetAvatar()));
                OnComplete.ExecuteIfBound(true, AvatarUrl);
            }
        });
    return true;
}

#endif