// Copyright June Rhodes. All Rights Reserved.

#include "OnlineFriendsInterfaceRedpointDiscord.h"

#if EOS_DISCORD_ENABLED

#include "LogRedpointDiscord.h"
#include "OnlineFriendRedpointDiscord.h"
#include "OnlinePresenceInterfaceRedpointDiscord.h"
#include "OnlineSubsystemRedpointDiscordConstants.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

void FOnlineFriendsInterfaceRedpointDiscord::RegisterEvents()
{
    this->OnRefreshHandle = this->Instance->RelationshipManager().OnRefresh.Connect([WeakThis = GetWeakThis(this)]() {
        if (auto This = PinWeakThis(WeakThis))
        {
            This->Instance->RelationshipManager().Filter([](const discord::Relationship &InRelationship) -> bool {
                // Don't include suggested relationships - we just want friends.
                return (
                    InRelationship.GetType() != discord::RelationshipType::Implicit &&
                    InRelationship.GetType() != discord::RelationshipType::None);
            });

            int32_t Count;
            if (This->Instance->RelationshipManager().Count(&Count) != discord::Result::Ok)
            {
                UE_LOG(
                    LogRedpointDiscord,
                    Error,
                    TEXT("Unable to get relationship count during OnRefresh callback. Friends will not be available."));
                return;
            }

            TArray<TSharedRef<FOnlineFriendRedpointDiscord>> FriendsArray;
            TUserIdMap<TSharedPtr<FOnlineFriendRedpointDiscord>> FriendsMap;
            for (int32_t i = 0; i < Count; i++)
            {
                discord::Relationship Relationship = {};
                if (This->Instance->RelationshipManager().GetAt(i, &Relationship) == discord::Result::Ok)
                {
                    TSharedRef<const FUniqueNetIdRedpointDiscord> FriendId =
                        MakeShared<FUniqueNetIdRedpointDiscord>(Relationship.GetUser().GetId());
                    TSharedRef<FOnlineFriendRedpointDiscord> Friend = MakeShared<FOnlineFriendRedpointDiscord>(
                        FriendId,
                        This->Presence->GetOrCreatePresenceInfoForDiscordId(FriendId).ToSharedRef(),
                        Relationship);
                    FriendsArray.Add(Friend);
                    FriendsMap.Add(*FriendId, Friend);
                }
            }
            This->CachedFriendsArray = FriendsArray;
            This->CachedFriendsMap = FriendsMap;
            This->bFriendsLoaded = true;
            for (const auto &Cb : This->OnFriendsLoaded)
            {
                Cb();
            }
            This->OnFriendsLoaded.Empty();

            This->TriggerOnFriendsChangeDelegates(0);
        }
    });
    this->OnRelationshipUpdateHandle = this->Instance->RelationshipManager().OnRelationshipUpdate.Connect(
        [WeakThis = GetWeakThis(this)](discord::Relationship const &InRelationship) {
            if (auto This = PinWeakThis(WeakThis))
            {
                TSharedRef<const FUniqueNetIdRedpointDiscord> FriendId =
                    MakeShared<FUniqueNetIdRedpointDiscord>(InRelationship.GetUser().GetId());
                if (This->CachedFriendsMap.Contains(*FriendId))
                {
                    This->CachedFriendsMap[*FriendId]->UpdateFromRelationship(InRelationship);
                    This->TriggerOnFriendsChangeDelegates(0);
                    This->Presence->TriggerOnPresenceReceivedDelegates(
                        *FriendId,
                        MakeShared<FOnlineUserPresence>(This->CachedFriendsMap[*FriendId]->GetPresence()));
                }
            }
        });
}

FOnlineFriendsInterfaceRedpointDiscord::FOnlineFriendsInterfaceRedpointDiscord(
    TSharedRef<discord::Core> InInstance,
    TSharedPtr<FOnlineIdentityInterfaceRedpointDiscord, ESPMode::ThreadSafe> InIdentity)
    : Instance(MoveTemp(InInstance))
    , OnRefreshHandle()
    , OnRelationshipUpdateHandle()
    , Identity(MoveTemp(InIdentity))
    , Presence(nullptr)
    , bFriendsLoaded(false)
    , CachedFriendsArray()
    , CachedFriendsMap(){};

FOnlineFriendsInterfaceRedpointDiscord::~FOnlineFriendsInterfaceRedpointDiscord()
{
    this->Instance->RelationshipManager().OnRefresh.Disconnect(this->OnRefreshHandle);
    this->Instance->RelationshipManager().OnRelationshipUpdate.Disconnect(this->OnRelationshipUpdateHandle);
}

bool FOnlineFriendsInterfaceRedpointDiscord::ReadFriendsList(
    int32 LocalUserNum,
    const FString &ListName,
    const FOnReadFriendsListComplete &Delegate)
{
    if (ListName != TEXT(""))
    {
        UE_LOG(
            LogRedpointDiscord,
            Error,
            TEXT("Expected empty list name for all friends operations in the Discord subsystem"));
        return false;
    }

    if (this->Identity->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
    {
        UE_LOG(LogRedpointDiscord, Error, TEXT("Local user is not logged in"));
        return false;
    }

    auto UserId = this->Identity->GetUniquePlayerId(LocalUserNum);
    if (UserId->GetType() != REDPOINT_DISCORD_SUBSYSTEM)
    {
        UE_LOG(LogRedpointDiscord, Error, TEXT("Local user is not using the Discord subsystem"));
        return false;
    }

    if (this->bFriendsLoaded)
    {
        Delegate.ExecuteIfBound(LocalUserNum, true, ListName, TEXT(""));
        return true;
    }

    OnFriendsLoaded.Add([WeakThis = GetWeakThis(this), LocalUserNum, ListName, Delegate]() {
        if (auto This = PinWeakThis(WeakThis))
        {
            Delegate.ExecuteIfBound(LocalUserNum, true, ListName, TEXT(""));
        }
    });
    return true;
}

bool FOnlineFriendsInterfaceRedpointDiscord::DeleteFriendsList(
    int32 LocalUserNum,
    const FString &ListName,
    const FOnDeleteFriendsListComplete &Delegate)
{
    UE_LOG(LogRedpointDiscord, Error, TEXT("FOnlineFriendsInterfaceRedpointDiscord::DeleteFriendsList not supported."));
    return false;
}

bool FOnlineFriendsInterfaceRedpointDiscord::SendInvite(
    int32 LocalUserNum,
    const FUniqueNetId &FriendId,
    const FString &ListName,
    const FOnSendInviteComplete &Delegate)
{
    UE_LOG(LogRedpointDiscord, Error, TEXT("FOnlineFriendsInterfaceRedpointDiscord::SendInvite not supported."));
    return false;
}

bool FOnlineFriendsInterfaceRedpointDiscord::AcceptInvite(
    int32 LocalUserNum,
    const FUniqueNetId &FriendId,
    const FString &ListName,
    const FOnAcceptInviteComplete &Delegate)
{
    UE_LOG(LogRedpointDiscord, Error, TEXT("FOnlineFriendsInterfaceRedpointDiscord::AcceptInvite not supported."));
    return false;
}

bool FOnlineFriendsInterfaceRedpointDiscord::RejectInvite(
    int32 LocalUserNum,
    const FUniqueNetId &FriendId,
    const FString &ListName)
{
    UE_LOG(LogRedpointDiscord, Error, TEXT("FOnlineFriendsInterfaceRedpointDiscord::RejectInvite not supported."));
    return false;
}

void FOnlineFriendsInterfaceRedpointDiscord::SetFriendAlias(
    int32 LocalUserNum,
    const FUniqueNetId &FriendId,
    const FString &ListName,
    const FString &Alias,
    const FOnSetFriendAliasComplete &Delegate)
{
    UE_LOG(LogRedpointDiscord, Error, TEXT("FOnlineFriendsInterfaceRedpointDiscord::SetFriendAlias not supported."));
    Delegate.ExecuteIfBound(LocalUserNum, FriendId, ListName, FOnlineError(false));
}

void FOnlineFriendsInterfaceRedpointDiscord::DeleteFriendAlias(
    int32 LocalUserNum,
    const FUniqueNetId &FriendId,
    const FString &ListName,
    const FOnDeleteFriendAliasComplete &Delegate)
{
    UE_LOG(LogRedpointDiscord, Error, TEXT("FOnlineFriendsInterfaceRedpointDiscord::DeleteFriendAlias not supported."));
    Delegate.ExecuteIfBound(LocalUserNum, FriendId, ListName, FOnlineError(false));
}

bool FOnlineFriendsInterfaceRedpointDiscord::DeleteFriend(
    int32 LocalUserNum,
    const FUniqueNetId &FriendId,
    const FString &ListName)
{
    UE_LOG(LogRedpointDiscord, Error, TEXT("FOnlineFriendsInterfaceRedpointDiscord::DeleteFriend not supported."));
    return false;
}

bool FOnlineFriendsInterfaceRedpointDiscord::GetFriendsList(
    int32 LocalUserNum,
    const FString &ListName,
    TArray<TSharedRef<FOnlineFriend>> &OutFriends)
{
    if (ListName != TEXT(""))
    {
        UE_LOG(
            LogRedpointDiscord,
            Error,
            TEXT("Expected empty list name for all friends operations in the Discord subsystem"));
        return false;
    }

    if (this->Identity->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
    {
        UE_LOG(LogRedpointDiscord, Error, TEXT("Local user is not logged in"));
        return false;
    }

    auto UserId = this->Identity->GetUniquePlayerId(LocalUserNum);
    if (UserId->GetType() != REDPOINT_DISCORD_SUBSYSTEM)
    {
        UE_LOG(LogRedpointDiscord, Error, TEXT("Local user is not using the Discord subsystem"));
        return false;
    }

    if (!this->bFriendsLoaded)
    {
        UE_LOG(LogRedpointDiscord, Error, TEXT("Discord friends have not been loaded yet; call ReadFriendsList"));
        return false;
    }

    OutFriends.Empty();
    for (const auto &Ref : this->CachedFriendsArray)
    {
        OutFriends.Add(Ref);
    }
    return true;
}

TSharedPtr<FOnlineFriend> FOnlineFriendsInterfaceRedpointDiscord::GetFriend(
    int32 LocalUserNum,
    const FUniqueNetId &FriendId,
    const FString &ListName)
{
    if (ListName != TEXT(""))
    {
        UE_LOG(
            LogRedpointDiscord,
            Error,
            TEXT("Expected empty list name for all friends operations in the Discord subsystem"));
        return nullptr;
    }

    if (this->Identity->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
    {
        UE_LOG(LogRedpointDiscord, Error, TEXT("Local user is not logged in"));
        return nullptr;
    }

    auto UserId = this->Identity->GetUniquePlayerId(LocalUserNum);
    if (UserId->GetType() != REDPOINT_DISCORD_SUBSYSTEM)
    {
        UE_LOG(LogRedpointDiscord, Error, TEXT("Local user is not using the Discord subsystem"));
        return nullptr;
    }

    if (!this->bFriendsLoaded)
    {
        UE_LOG(LogRedpointDiscord, Error, TEXT("Discord friends have not been loaded yet; call ReadFriendsList"));
        return nullptr;
    }

    if (this->CachedFriendsMap.Contains(FriendId))
    {
        return this->CachedFriendsMap[FriendId];
    }

    return nullptr;
}

bool FOnlineFriendsInterfaceRedpointDiscord::IsFriend(
    int32 LocalUserNum,
    const FUniqueNetId &FriendId,
    const FString &ListName)
{
    if (ListName != TEXT(""))
    {
        UE_LOG(
            LogRedpointDiscord,
            Error,
            TEXT("Expected empty list name for all friends operations in the Discord subsystem"));
        return false;
    }

    if (this->Identity->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
    {
        UE_LOG(LogRedpointDiscord, Error, TEXT("Local user is not logged in"));
        return false;
    }

    auto UserId = this->Identity->GetUniquePlayerId(LocalUserNum);
    if (UserId->GetType() != REDPOINT_DISCORD_SUBSYSTEM)
    {
        UE_LOG(LogRedpointDiscord, Error, TEXT("Local user is not using the Discord subsystem"));
        return false;
    }

    if (!this->bFriendsLoaded)
    {
        UE_LOG(LogRedpointDiscord, Error, TEXT("Discord friends have not been loaded yet; call ReadFriendsList"));
        return false;
    }

    if (this->CachedFriendsMap.Contains(FriendId))
    {
        return this->CachedFriendsMap[FriendId]->GetInviteStatus() == EInviteStatus::Accepted;
    }

    return false;
}

bool FOnlineFriendsInterfaceRedpointDiscord::QueryRecentPlayers(const FUniqueNetId &UserId, const FString &Namespace)
{
    UE_LOG(
        LogRedpointDiscord,
        Error,
        TEXT("FOnlineFriendsInterfaceRedpointDiscord::QueryRecentPlayers not supported."));
    return false;
}

bool FOnlineFriendsInterfaceRedpointDiscord::GetRecentPlayers(
    const FUniqueNetId &UserId,
    const FString &Namespace,
    TArray<TSharedRef<FOnlineRecentPlayer>> &OutRecentPlayers)
{
    UE_LOG(LogRedpointDiscord, Error, TEXT("FOnlineFriendsInterfaceRedpointDiscord::GetRecentPlayers not supported."));
    return false;
}

void FOnlineFriendsInterfaceRedpointDiscord::DumpRecentPlayers() const
{
}

bool FOnlineFriendsInterfaceRedpointDiscord::BlockPlayer(int32 LocalUserNum, const FUniqueNetId &PlayerId)
{
    UE_LOG(LogRedpointDiscord, Error, TEXT("FOnlineFriendsInterfaceRedpointDiscord::BlockPlayer not supported."));
    return false;
}

bool FOnlineFriendsInterfaceRedpointDiscord::UnblockPlayer(int32 LocalUserNum, const FUniqueNetId &PlayerId)
{
    UE_LOG(LogRedpointDiscord, Error, TEXT("FOnlineFriendsInterfaceRedpointDiscord::UnblockPlayer not supported."));
    return false;
}

bool FOnlineFriendsInterfaceRedpointDiscord::QueryBlockedPlayers(const FUniqueNetId &UserId)
{
    UE_LOG(
        LogRedpointDiscord,
        Error,
        TEXT("FOnlineFriendsInterfaceRedpointDiscord::QueryBlockedPlayers not supported."));
    return false;
}

bool FOnlineFriendsInterfaceRedpointDiscord::GetBlockedPlayers(
    const FUniqueNetId &UserId,
    TArray<TSharedRef<FOnlineBlockedPlayer>> &OutBlockedPlayers)
{
    UE_LOG(LogRedpointDiscord, Error, TEXT("FOnlineFriendsInterfaceRedpointDiscord::GetBlockedPlayers not supported."));
    return false;
}

void FOnlineFriendsInterfaceRedpointDiscord::DumpBlockedPlayers() const
{
}

#endif