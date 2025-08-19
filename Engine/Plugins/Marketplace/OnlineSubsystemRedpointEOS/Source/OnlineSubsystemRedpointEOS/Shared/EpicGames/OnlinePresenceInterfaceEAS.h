// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineFriendsInterfaceEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineIdentityInterfaceEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineUserPresenceEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineIdentityInterfaceEOS.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FOnlinePresenceInterfaceEAS
    : public IOnlinePresence,
      public TSharedFromThis<FOnlinePresenceInterfaceEAS, ESPMode::ThreadSafe>
{
    friend class FOnlineSubsystemRedpointEAS;
    friend class FOnlineFriendsInterfaceEAS;

private:
    EOS_HPresence EOSPresence;
    EOS_HConnect EOSConnect;
    TSharedPtr<FOnlineIdentityInterfaceEAS, ESPMode::ThreadSafe> Identity;
    TSharedPtr<class FOnlineFriendsInterfaceEAS, ESPMode::ThreadSafe> Friends;
    TSharedPtr<const class FEOSConfig> Config;
    TUserIdMap<TSharedPtr<FOnlineUserPresenceEAS>> PresenceByEpicId;
    TSharedPtr<EOSEventHandle<EOS_Presence_PresenceChangedCallbackInfo>> Unregister_PresenceChanged;

    void ConnectFriendsToPresence();
    void DisconnectFriendsFromPresence();
    void RegisterEvents();

    TSharedPtr<FOnlineUserPresenceEAS> GetOrCreatePresenceInfoForEpicId(
        const TSharedRef<const FUniqueNetIdEAS> &InEpicId);

public:
    FOnlinePresenceInterfaceEAS(
        EOS_HPlatform InPlatform,
        const TSharedRef<FOnlineIdentityInterfaceEAS, ESPMode::ThreadSafe> &InIdentity,
        const TSharedRef<class FOnlineFriendsInterfaceEAS, ESPMode::ThreadSafe> &InFriends,
        const TSharedRef<const class FEOSConfig> &InConfig);
    UE_NONCOPYABLE(FOnlinePresenceInterfaceEAS);

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
        const FOnPresenceTaskCompleteDelegate &Delegate) override;

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

#endif // #if EOS_HAS_AUTHENTICATION