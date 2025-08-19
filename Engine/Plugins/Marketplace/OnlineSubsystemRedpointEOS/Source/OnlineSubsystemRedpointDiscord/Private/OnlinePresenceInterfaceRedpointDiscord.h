// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if EOS_DISCORD_ENABLED

#include "Interfaces/OnlinePresenceInterface.h"
#include "OnlineIdentityInterfaceRedpointDiscord.h"
#include "OnlineSubsystemRedpointEOS/Shared/UserIdMap.h"
#include "OnlineUserPresenceRedpointDiscord.h"

class FOnlinePresenceInterfaceRedpointDiscord
    : public IOnlinePresence,
      public TSharedFromThis<FOnlinePresenceInterfaceRedpointDiscord, ESPMode::ThreadSafe>
{
    friend class FOnlineSubsystemRedpointDiscord;
    friend class FOnlineFriendsInterfaceRedpointDiscord;

private:
    TSharedPtr<FOnlineIdentityInterfaceRedpointDiscord, ESPMode::ThreadSafe> Identity;
    TSharedPtr<class FOnlineFriendsInterfaceRedpointDiscord, ESPMode::ThreadSafe> Friends;
    TUserIdMap<TSharedPtr<FOnlineUserPresenceRedpointDiscord>> PresenceByDiscordId;

    void ConnectFriendsToPresence();
    void DisconnectFriendsFromPresence();

    TSharedPtr<FOnlineUserPresenceRedpointDiscord> GetOrCreatePresenceInfoForDiscordId(
        const TSharedRef<const FUniqueNetIdRedpointDiscord> &InDiscordId);

public:
    FOnlinePresenceInterfaceRedpointDiscord(
        const TSharedRef<FOnlineIdentityInterfaceRedpointDiscord, ESPMode::ThreadSafe> &InIdentity,
        const TSharedRef<class FOnlineFriendsInterfaceRedpointDiscord, ESPMode::ThreadSafe> &InFriends);
    UE_NONCOPYABLE(FOnlinePresenceInterfaceRedpointDiscord);
    virtual ~FOnlinePresenceInterfaceRedpointDiscord(){};

    virtual void SetPresence(
        const FUniqueNetId &User,
        const FOnlineUserPresenceStatus &Status,
        const FOnPresenceTaskCompleteDelegate &Delegate = FOnPresenceTaskCompleteDelegate()) override;

    virtual void QueryPresence(
        const FUniqueNetId &User,
        const FOnPresenceTaskCompleteDelegate &Delegate = FOnPresenceTaskCompleteDelegate()) override;

    virtual EOnlineCachedResult::Type GetCachedPresence(
        const FUniqueNetId &User,
        TSharedPtr<FOnlineUserPresence> &OutPresence) override;

    virtual EOnlineCachedResult::Type GetCachedPresenceForApp(
        const FUniqueNetId &LocalUserId,
        const FUniqueNetId &User,
        const FString &AppId,
        TSharedPtr<FOnlineUserPresence> &OutPresence) override;
};

#endif