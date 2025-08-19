// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EOSCommon.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "OnlineSubsystem.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FOnlinePresenceInterfaceSynthetic
    : public IOnlinePresence,
      public TSharedFromThis<FOnlinePresenceInterfaceSynthetic, ESPMode::ThreadSafe>
{
    friend class FOnlineSubsystemEOS;

private:
    struct FWrappedSubsystem
    {
        FName SubsystemName;
        TWeakPtr<IOnlineIdentity, ESPMode::ThreadSafe> Identity;
        TWeakPtr<IOnlinePresence, ESPMode::ThreadSafe> Presence;
    };

    TSharedPtr<class FEOSConfig> Config;
    TSharedPtr<class FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> IdentityEOS;
    TSharedPtr<class FOnlineFriendsInterfaceSynthetic, ESPMode::ThreadSafe> FriendsSynthetic;
    TArray<FWrappedSubsystem> WrappedSubsystems;
    TArray<TTuple<FWrappedSubsystem, FDelegateHandle>> OnPresenceReceivedDelegates;
    TArray<TTuple<FWrappedSubsystem, FDelegateHandle>> OnPresenceArrayUpdatedDelegates;

    void RegisterEvents();

public:
    FOnlinePresenceInterfaceSynthetic(
        FName InInstanceName,
        TSharedPtr<class FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> InIdentity,
        const TSharedRef<class FOnlineFriendsInterfaceSynthetic, ESPMode::ThreadSafe> &InFriends,
        const IOnlineSubsystemPtr &InSubsystemEAS,
        const TSharedRef<class FEOSConfig> &InConfig);
    UE_NONCOPYABLE(FOnlinePresenceInterfaceSynthetic);
    ~FOnlinePresenceInterfaceSynthetic();

    virtual void SetPresence(
        const FUniqueNetId &User,
        const FOnlineUserPresenceStatus &Status,
        const FOnPresenceTaskCompleteDelegate &Delegate = FOnPresenceTaskCompleteDelegate()) override;

    virtual void QueryPresence(
        const FUniqueNetId &User,
        const FOnPresenceTaskCompleteDelegate &Delegate = FOnPresenceTaskCompleteDelegate()) override;

    virtual void QueryPresence(
        const FUniqueNetId &LocalUserId,
        const TArray<TSharedRef<const FUniqueNetId>> &UserIds,
        const FOnPresenceTaskCompleteDelegate &Delegate)
        override;

    virtual EOnlineCachedResult::Type GetCachedPresence(
        const FUniqueNetId &User,
        TSharedPtr<FOnlineUserPresence> &OutPresence) override;

    virtual EOnlineCachedResult::Type GetCachedPresenceForApp(
        const FUniqueNetId &LocalUserId,
        const FUniqueNetId &User,
        const FString &AppId,
        TSharedPtr<FOnlineUserPresence> &OutPresence) override;
};

EOS_DISABLE_STRICT_WARNINGS