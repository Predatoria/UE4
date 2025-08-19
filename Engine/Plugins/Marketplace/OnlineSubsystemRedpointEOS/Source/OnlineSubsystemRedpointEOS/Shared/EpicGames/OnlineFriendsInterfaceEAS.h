// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSDefines.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineIdentityInterfaceEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/UniqueNetIdEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineUserEOS.h"

EOS_ENABLE_STRICT_WARNINGS

DECLARE_DELEGATE(FFriendCacheReady);

class ONLINESUBSYSTEMREDPOINTEOS_API FOnlineFriendsInterfaceEAS
    : public IOnlineFriends,
      public TSharedFromThis<FOnlineFriendsInterfaceEAS, ESPMode::ThreadSafe>
{
    friend class FOnlineSubsystemRedpointEAS;
    friend class FOnlinePresenceInterfaceEAS;

private:
    EOS_HFriends EOSFriends;
    EOS_HUserInfo EOSUserInfo;

    TSharedPtr<FOnlineIdentityInterfaceEAS, ESPMode::ThreadSafe> Identity;
    TSharedPtr<EOSEventHandle<EOS_Friends_OnFriendsUpdateInfo>> OnFriendsUpdateHandle;
    TUserIdMap<TArray<TSharedRef<FOnlineFriend>>> CachedFriendsArray;
    TUserIdMap<TUserIdMap<TSharedPtr<FOnlineFriend>>> CachedFriendsMap;
    TSharedPtr<class FOnlinePresenceInterfaceEAS, ESPMode::ThreadSafe> Presence;

    void CacheFriends(const FUniqueNetIdEAS &UserId, const FFriendCacheReady &FriendsReadyDelegate);
    void RegisterEvents();

public:
    FOnlineFriendsInterfaceEAS(
        EOS_HPlatform InPlatform,
        TSharedPtr<FOnlineIdentityInterfaceEAS, ESPMode::ThreadSafe> InIdentity);
    UE_NONCOPYABLE(FOnlineFriendsInterfaceEAS);
    ~FOnlineFriendsInterfaceEAS();

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

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION