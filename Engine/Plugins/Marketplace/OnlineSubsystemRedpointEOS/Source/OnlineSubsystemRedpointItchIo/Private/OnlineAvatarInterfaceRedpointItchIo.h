// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if EOS_ITCH_IO_ENABLED

#include "Interfaces/IHttpRequest.h"
#include "RedpointEOSInterfaces/Private/Interfaces/OnlineAvatarInterface.h"

class FOnlineAvatarInterfaceRedpointItchIo
    : public IOnlineAvatar,
      public TSharedFromThis<FOnlineAvatarInterfaceRedpointItchIo, ESPMode::ThreadSafe>
{
private:
    TSharedRef<class FOnlineIdentityInterfaceRedpointItchIo, ESPMode::ThreadSafe> Identity;

    void OnDownloadAvatarComplete(
        FHttpRequestPtr Request,
        FHttpResponsePtr Response,
        bool bConnectedSuccessfully,
        TSoftObjectPtr<UTexture> DefaultTexture,
        FOnGetAvatarComplete OnComplete);

public:
    FOnlineAvatarInterfaceRedpointItchIo(
        TSharedRef<class FOnlineIdentityInterfaceRedpointItchIo, ESPMode::ThreadSafe> InIdentity)
        : Identity(MoveTemp(InIdentity)){};
    virtual ~FOnlineAvatarInterfaceRedpointItchIo(){};
    UE_NONCOPYABLE(FOnlineAvatarInterfaceRedpointItchIo);

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