// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if EOS_DISCORD_ENABLED

#include "DiscordGameSDK.h"
#include "Interfaces/IHttpRequest.h"
#include "RedpointEOSInterfaces/Private/Interfaces/OnlineAvatarInterface.h"

class FOnlineAvatarInterfaceRedpointDiscord
    : public IOnlineAvatar,
      public TSharedFromThis<FOnlineAvatarInterfaceRedpointDiscord, ESPMode::ThreadSafe>
{
private:
    TSharedRef<discord::Core> Instance;

    void OnDownloadAvatarComplete(
        FHttpRequestPtr Request,
        FHttpResponsePtr Response,
        bool bConnectedSuccessfully,
        TSoftObjectPtr<UTexture> DefaultTexture,
        FOnGetAvatarComplete OnComplete);

    void DownloadAvatar(
        const FString &UserId,
        const FString &AvatarHash,
        TSoftObjectPtr<UTexture> DefaultTexture,
        FOnGetAvatarComplete OnComplete);

public:
    FOnlineAvatarInterfaceRedpointDiscord(TSharedRef<discord::Core> InInstance)
        : Instance(MoveTemp(InInstance)){};
    UE_NONCOPYABLE(FOnlineAvatarInterfaceRedpointDiscord);
    virtual ~FOnlineAvatarInterfaceRedpointDiscord(){};

    bool GetAvatar(
        const FUniqueNetId &LocalUserId,
        const FUniqueNetId &TargetUserId,
        TSoftObjectPtr<UTexture> DefaultTexture,
        FOnGetAvatarComplete OnComplete) override;

    bool GetAvatarUrl(
        const FUniqueNetId &LocalUserId,
        const FUniqueNetId &TargetUserId,
        FString DefaultAvatarUrl,
        FOnGetAvatarUrlComplete OnComplete) override;
};

#endif