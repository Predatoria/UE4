// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if EOS_DISCORD_ENABLED

#include "DiscordGameSDK.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "OnlineIdentityInterfaceRedpointDiscord.h"
#include "OnlineSubsystemRedpointEOS/Shared/UserIdMap.h"

class FOnlineFriendsInterfaceRedpointDiscord
    : public IOnlineFriends,
      public TSharedFromThis<FOnlineFriendsInterfaceRedpointDiscord, ESPMode::ThreadSafe>
{
    friend class FOnlineSubsystemRedpointDiscord;
    friend class FOnlinePresenceInterfaceRedpointDiscord;

private:
    TSharedRef<discord::Core> Instance;
    discord::Event<>::Token OnRefreshHandle;
    discord::Event<discord::Relationship const &>::Token OnRelationshipUpdateHandle;
    TSharedPtr<FOnlineIdentityInterfaceRedpointDiscord, ESPMode::ThreadSafe> Identity;
    TSharedPtr<class FOnlinePresenceInterfaceRedpointDiscord, ESPMode::ThreadSafe> Presence;
    bool bFriendsLoaded;
    TArray<std::function<void()>> OnFriendsLoaded;

    TArray<TSharedRef<class FOnlineFriendRedpointDiscord>> CachedFriendsArray;
    TUserIdMap<TSharedPtr<class FOnlineFriendRedpointDiscord>> CachedFriendsMap;

    void RegisterEvents();

public:
    FOnlineFriendsInterfaceRedpointDiscord(
        TSharedRef<discord::Core> InInstance,
        TSharedPtr<FOnlineIdentityInterfaceRedpointDiscord, ESPMode::ThreadSafe> InIdentity);
    UE_NONCOPYABLE(FOnlineFriendsInterfaceRedpointDiscord);
    virtual ~FOnlineFriendsInterfaceRedpointDiscord();

    virtual bool ReadFriendsList(
        int32 LocalUserNum,
        const FString &ListName,
        const FOnReadFriendsListComplete &Delegate = FOnReadFriendsListComplete()) override;
    virtual bool DeleteFriendsList(
        int32 LocalUserNum,
        const FString &ListName,
        const FOnDeleteFriendsListComplete &Delegate = FOnDeleteFriendsListComplete()) override;

    virtual bool SendInvite(
        int32 LocalUserNum,
        const FUniqueNetId &FriendId,
        const FString &ListName,
        const FOnSendInviteComplete &Delegate = FOnSendInviteComplete()) override;
    virtual bool AcceptInvite(
        int32 LocalUserNum,
        const FUniqueNetId &FriendId,
        const FString &ListName,
        const FOnAcceptInviteComplete &Delegate = FOnAcceptInviteComplete()) override;
    virtual bool RejectInvite(int32 LocalUserNum, const FUniqueNetId &FriendId, const FString &ListName) override;

    virtual void SetFriendAlias(
        int32 LocalUserNum,
        const FUniqueNetId &FriendId,
        const FString &ListName,
        const FString &Alias,
        const FOnSetFriendAliasComplete &Delegate = FOnSetFriendAliasComplete()) override;
    virtual void DeleteFriendAlias(
        int32 LocalUserNum,
        const FUniqueNetId &FriendId,
        const FString &ListName,
        const FOnDeleteFriendAliasComplete &Delegate = FOnDeleteFriendAliasComplete()) override;
    virtual bool DeleteFriend(int32 LocalUserNum, const FUniqueNetId &FriendId, const FString &ListName) override;

    virtual bool GetFriendsList(
        int32 LocalUserNum,
        const FString &ListName,
        TArray<TSharedRef<FOnlineFriend>> &OutFriends) override;
    virtual TSharedPtr<FOnlineFriend> GetFriend(
        int32 LocalUserNum,
        const FUniqueNetId &FriendId,
        const FString &ListName) override;
    virtual bool IsFriend(int32 LocalUserNum, const FUniqueNetId &FriendId, const FString &ListName) override;

    virtual bool QueryRecentPlayers(const FUniqueNetId &UserId, const FString &Namespace) override;
    virtual bool GetRecentPlayers(
        const FUniqueNetId &UserId,
        const FString &Namespace,
        TArray<TSharedRef<FOnlineRecentPlayer>> &OutRecentPlayers) override;
    virtual void DumpRecentPlayers() const override;

    virtual bool BlockPlayer(int32 LocalUserNum, const FUniqueNetId &PlayerId) override;
    virtual bool UnblockPlayer(int32 LocalUserNum, const FUniqueNetId &PlayerId) override;
    virtual bool QueryBlockedPlayers(const FUniqueNetId &UserId) override;
    virtual bool GetBlockedPlayers(
        const FUniqueNetId &UserId,
        TArray<TSharedRef<FOnlineBlockedPlayer>> &OutBlockedPlayers) override;
    virtual void DumpBlockedPlayers() const override;
};

#endif