// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineUserCloudInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/PDSFriends/SerializedBlockedUser.h"
#include "OnlineSubsystemRedpointEOS/Shared/PDSFriends/SerializedCachedFriend.h"
#include "OnlineSubsystemRedpointEOS/Shared/PDSFriends/SerializedExternalAccount.h"
#include "OnlineSubsystemRedpointEOS/Shared/PDSFriends/SerializedFriend.h"
#include "OnlineSubsystemRedpointEOS/Shared/PDSFriends/SerializedPendingFriendRequest.h"
#include "OnlineSubsystemRedpointEOS/Shared/PDSFriends/SerializedRecentPlayer.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"

EOS_ENABLE_STRICT_WARNINGS

DECLARE_DELEGATE_OneParam(FFriendDatabaseOperationComplete, bool);

class ONLINESUBSYSTEMREDPOINTEOS_API FFriendDatabase : public TSharedFromThis<FFriendDatabase>
{
private:
    TSharedRef<const FUniqueNetIdEOS> LocalUserId;
    TWeakPtr<FOnlineUserCloudInterfaceEOS, ESPMode::ThreadSafe> UserCloudImpl;
    bool bLoading;
    bool bLoaded;
    bool bDirty;
    bool bFlushing;
    FDelegateHandle EnumerateHandle;
    FDelegateHandle LoadHandle;
    FDelegateHandle SaveHandle;
    TArray<FFriendDatabaseOperationComplete> PendingCallbacksForNextFlushBatch;
    FFriendDatabaseOperationComplete NextChainedFlushOperation;
    /**
     * Friends that have been explicitly added by the user. These are never removed unless the user
     * explicitly removes them from their friend list.
     */
    TUserIdMap<FSerializedFriend> Friends;
    /**
     * A copy of the friends from the delegated subsystems is stored in the friend database. This allows
     * us to pull metadata about friends the user has on platforms they aren't signed into at the moment
     * (for example, we can pull information about Steam friends while playing on console). The data
     * from imported friends is then used by the synthetic friends system when unified friends, where it
     * is unable to call the specified subsystem for friend information.
     *
     * When the synthetic friends system is able to query a given subsystem, it fully replaces the subsystem's
     * cached list of friends in the friend database.
     */
    TMap<FName, TMap<FString, FSerializedCachedFriend>> DelegatedSubsystemCachedFriends;
    /**
     * A list of players the user has recently interacted with.
     */
    TUserIdMap<FSerializedRecentPlayer> RecentPlayers;
    /**
     * A list of blocked users.
     */
    TUserIdMap<FSerializedBlockedUser> BlockedUsers;
    /**
     * A list of pending friend requests. This includes friends requests we are yet to send (because
     * the target is offline), friend requests we're waiting to get a response on, and friend responses
     * we're yet to send (because the original requestor is offline).
     */
    TUserIdMap<FSerializedPendingFriendRequest> PendingFriendRequests;
    /**
     * A map of user IDs to friend aliases. The key is <ID type>:<ID string>, because there's no sensible
     * way to serialize/deserialize arbitrary unique net IDs and keep them comparable.
     */
    TMap<FString, FString> FriendAliases;
    /**
     * The expected header for the current version of the friends database.
     */
    TArray<uint8> ExpectedHeader;

    void OnEnumerateFilesComplete(
        bool bWasSuccessful,
        const FUniqueNetId &UserId,
        FFriendDatabaseOperationComplete OnComplete);
    void OnFileReadComplete(
        bool bWasSuccessful,
        const FUniqueNetId &UserId,
        const FString &FileName,
        FFriendDatabaseOperationComplete OnComplete);
    void OnFileWriteComplete(
        bool bWasSuccessful,
        const FUniqueNetId &UserId,
        const FString &FileName,
        TArray<FFriendDatabaseOperationComplete> OnCompleteCallbacks);

    /**
     * Serialize/deserialize state.
     */
    void Archive(FArchive &Ar);

    /**
     * Internal flush implementation.
     */
    void InternalFlush(const TArray<FFriendDatabaseOperationComplete> &OnCompleteCallbacks);

public:
    FFriendDatabase(
        const TSharedRef<const FUniqueNetIdEOS> &InLocalUserId,
        const TSharedRef<FOnlineUserCloudInterfaceEOS, ESPMode::ThreadSafe> &InUserCloudImpl);
    UE_NONCOPYABLE(FFriendDatabase);
    ~FFriendDatabase() = default;

    /**
     * Wait until the friend database has loaded it's state from Player Data Storage. The
     * load only happens once per user per application.
     */
    void WaitUntilLoaded(const FFriendDatabaseOperationComplete &OnComplete);

    /**
     * Flushes the database state to Player Data Storage, if there are any pending changes
     * to write.
     */
    void DirtyAndFlush(const FFriendDatabaseOperationComplete &OnComplete);

    TUserIdMap<FSerializedFriend> &GetFriends()
    {
        return this->Friends;
    }

    TMap<FName, TMap<FString, FSerializedCachedFriend>> &GetDelegatedSubsystemCachedFriends()
    {
        return this->DelegatedSubsystemCachedFriends;
    }

    TUserIdMap<FSerializedRecentPlayer> &GetRecentPlayers()
    {
        return this->RecentPlayers;
    }

    TUserIdMap<FSerializedBlockedUser> &GetBlockedUsers()
    {
        return this->BlockedUsers;
    }

    TUserIdMap<FSerializedPendingFriendRequest> &GetPendingFriendRequests()
    {
        return this->PendingFriendRequests;
    }

    TMap<FString, FString> &GetFriendAliases()
    {
        return this->FriendAliases;
    }
};

EOS_DISABLE_STRICT_WARNINGS