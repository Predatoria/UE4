// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "OnlineSubsystemRedpointEOS/Shared/DelegatedSubsystems.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineIdentityInterfaceEOS.h"
#include "RedpointEOSInterfaces/Private/Interfaces/OnlineAvatarInterface.h"

EOS_ENABLE_STRICT_WARNINGS

class FOnlineAvatarInterfaceSynthetic : public IOnlineAvatar,
                                        public TSharedFromThis<FOnlineAvatarInterfaceSynthetic, ESPMode::ThreadSafe>
{
private:
    TMap<FName, FDelegatedInterface<IOnlineAvatar>> DelegatedAvatar;
    TMap<FName, FDelegatedInterface<IOnlineIdentity>> DelegatedIdentity;
    FName InstanceName;
    TSharedPtr<FEOSConfig> Config;
    TSharedPtr<FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> Identity;
    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
    IOnlineFriendsPtr Friends;
    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
    IOnlineUserPtr User;
    bool bInitialized;

    void OnQueryUserInfoComplete(
        int32 LocalUserNum,
        bool bWasSuccessful,
        const TArray<TSharedRef<const FUniqueNetId>> &TargetUserIds,
        const FString &ErrorMessage,
        TSharedRef<FDelegateHandle> AllocatedHandle,
        TSharedRef<const FUniqueNetId> IntendedUserId,
        FOnGetAvatarComplete OnComplete,
        TSoftObjectPtr<UTexture> DefaultTexture);

    void OnDownloadAvatarComplete(
        FHttpRequestPtr Request,
        FHttpResponsePtr Response,
        bool bConnectedSuccessfully,
        TSoftObjectPtr<UTexture> DefaultTexture,
        FOnGetAvatarComplete OnComplete);

public:
    FOnlineAvatarInterfaceSynthetic(
        FName InInstanceName,
        const TSharedRef<FEOSConfig> &InConfig,
        TSharedPtr<FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> InIdentity,
        IOnlineFriendsPtr InFriends,
        IOnlineUserPtr InUser);
    UE_NONCOPYABLE(FOnlineAvatarInterfaceSynthetic);
    virtual ~FOnlineAvatarInterfaceSynthetic(){};

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

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION