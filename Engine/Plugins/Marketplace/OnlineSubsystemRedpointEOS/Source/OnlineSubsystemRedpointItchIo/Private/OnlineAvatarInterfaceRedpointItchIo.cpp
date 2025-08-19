// Copyright June Rhodes. All Rights Reserved.

#include "OnlineAvatarInterfaceRedpointItchIo.h"

#if EOS_ITCH_IO_ENABLED

#include "HttpModule.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Interfaces/IHttpResponse.h"
#include "LogRedpointItchIo.h"
#include "OnlineIdentityInterfaceRedpointItchIo.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSTextureLoader.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "OnlineSubsystemRedpointItchIoConstants.h"
#include "UniqueNetIdRedpointItchIo.h"

void FOnlineAvatarInterfaceRedpointItchIo::OnDownloadAvatarComplete(
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FHttpRequestPtr Request,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FHttpResponsePtr Response,
    bool bConnectedSuccessfully,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSoftObjectPtr<UTexture> DefaultTexture,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FOnGetAvatarComplete OnComplete)
{
    if (!bConnectedSuccessfully || !Response.IsValid() || !EHttpResponseCodes::IsOk(Response->GetResponseCode()))
    {
        UE_LOG(LogRedpointItchIo, Error, TEXT("GetAvatar: HTTP request to download avatar from itch.io failed."));
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

bool FOnlineAvatarInterfaceRedpointItchIo::GetAvatar(
    const FUniqueNetId &LocalUserId,
    const FUniqueNetId &TargetUserId,
    TSoftObjectPtr<UTexture> DefaultTexture,
    FOnGetAvatarComplete OnComplete)
{
    if (TargetUserId.GetType() != REDPOINT_ITCH_IO_SUBSYSTEM)
    {
        UE_LOG(LogRedpointItchIo, Error, TEXT("GetAvatar: TargetUserId is non-itch.io user."));
        OnComplete.ExecuteIfBound(false, DefaultTexture);
        return true;
    }

    TSharedRef<const FUniqueNetIdRedpointItchIo> TargetUserIdItchIo =
        StaticCastSharedRef<const FUniqueNetIdRedpointItchIo>(TargetUserId.AsShared());

    if (this->Identity->GetLoginStatus(TargetUserId) != ELoginStatus::LoggedIn)
    {
        OnComplete.ExecuteIfBound(false, DefaultTexture);
        return true;
    }

    TSharedPtr<FUserOnlineAccount> Account = this->Identity->GetUserAccount(TargetUserId);
    checkf(Account.IsValid(), TEXT("Expected itch.io account to be valid."));
    FString CoverUrl;
    if (!Account->GetUserAttribute(TEXT("coverUrl"), CoverUrl))
    {
        OnComplete.ExecuteIfBound(false, DefaultTexture);
        return true;
    }

    auto Request = FHttpModule::Get().CreateRequest();
    Request->SetVerb("GET");
    Request->SetURL(CoverUrl);
    Request->OnProcessRequestComplete().BindThreadSafeSP(
        this,
        &FOnlineAvatarInterfaceRedpointItchIo::OnDownloadAvatarComplete,
        DefaultTexture,
        OnComplete);
    Request->ProcessRequest();
    return true;
}

bool FOnlineAvatarInterfaceRedpointItchIo::GetAvatarUrl(
    const FUniqueNetId &LocalUserId,
    const FUniqueNetId &TargetUserId,
    FString DefaultAvatarUrl,
    FOnGetAvatarUrlComplete OnComplete)
{
    if (TargetUserId.GetType() != REDPOINT_ITCH_IO_SUBSYSTEM)
    {
        UE_LOG(LogRedpointItchIo, Error, TEXT("GetAvatar: TargetUserId is non-itch.io user."));
        OnComplete.ExecuteIfBound(false, DefaultAvatarUrl);
        return true;
    }

    TSharedRef<const FUniqueNetIdRedpointItchIo> TargetUserIdItchIo =
        StaticCastSharedRef<const FUniqueNetIdRedpointItchIo>(TargetUserId.AsShared());

    if (this->Identity->GetLoginStatus(TargetUserId) != ELoginStatus::LoggedIn)
    {
        OnComplete.ExecuteIfBound(false, DefaultAvatarUrl);
        return true;
    }

    TSharedPtr<FUserOnlineAccount> Account = this->Identity->GetUserAccount(TargetUserId);
    checkf(Account.IsValid(), TEXT("Expected itch.io account to be valid."));
    FString CoverUrl;
    if (Account->GetUserAttribute(TEXT("coverUrl"), CoverUrl))
    {
        OnComplete.ExecuteIfBound(false, CoverUrl);
    }

    OnComplete.ExecuteIfBound(false, DefaultAvatarUrl);
    return true;
}

#endif
