// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineFriendsInterfaceEAS.h"

#include "OnlineError.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineFriendEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlinePresenceInterfaceEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/MultiOperation.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineUserEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

void FOnlineFriendsInterfaceEAS::CacheFriends(
    const FUniqueNetIdEAS &UserId,
    const FFriendCacheReady &FriendsReadyDelegate)
{
    EOS_Friends_GetFriendsCountOptions Opts = {};
    Opts.ApiVersion = EOS_FRIENDS_GETFRIENDSCOUNT_API_LATEST;
    Opts.LocalUserId = UserId.GetEpicAccountId();

    TArray<EOS_EpicAccountId> PendingAccountIds;
    auto Count = EOS_Friends_GetFriendsCount(this->EOSFriends, &Opts);
    for (auto Idx = 0; Idx < Count; Idx++)
    {
        EOS_Friends_GetFriendAtIndexOptions GetOpts = {};
        GetOpts.ApiVersion = EOS_FRIENDS_GETFRIENDATINDEX_API_LATEST;
        GetOpts.Index = Idx;
        GetOpts.LocalUserId = UserId.GetEpicAccountId();

        auto FriendEpicId = EOS_Friends_GetFriendAtIndex(this->EOSFriends, &GetOpts);

        EOS_Friends_GetStatusOptions StatusOpts = {};
        StatusOpts.ApiVersion = EOS_FRIENDS_GETSTATUS_API_LATEST;
        StatusOpts.LocalUserId = UserId.GetEpicAccountId();
        StatusOpts.TargetUserId = FriendEpicId;

        auto FriendStatus = EOS_Friends_GetStatus(this->EOSFriends, &StatusOpts);

        if (FriendStatus == EOS_EFriendsStatus::EOS_FS_Friends)
        {
            // We can't act on pending invites or non-friends because of API limitations, so
            // just return the user's actual friends.
            PendingAccountIds.Add(FriendEpicId);
        }
    }

    FMultiOperation<EOS_EpicAccountId, EOS_UserInfo *>::Run(
        PendingAccountIds,
        [WeakThis = GetWeakThis(this), UserId = StaticCastSharedRef<const FUniqueNetIdEAS>(UserId.AsShared())](
            EOS_EpicAccountId EpicAccountId,
            const std::function<void(EOS_UserInfo *)> &OnDone) -> bool {
            if (auto This = PinWeakThis(WeakThis))
            {
                EOS_UserInfo_QueryUserInfoOptions QueryOpts = {};
                QueryOpts.ApiVersion = EOS_USERINFO_QUERYUSERINFO_API_LATEST;
                QueryOpts.LocalUserId = UserId->GetEpicAccountId();
                QueryOpts.TargetUserId = EpicAccountId;

                EOSRunOperation<
                    EOS_HUserInfo,
                    EOS_UserInfo_QueryUserInfoOptions,
                    EOS_UserInfo_QueryUserInfoCallbackInfo>(
                    This->EOSUserInfo,
                    &QueryOpts,
                    &EOS_UserInfo_QueryUserInfo,
                    [WeakThis = GetWeakThis(This), UserId, EpicAccountId, OnDone](
                        const EOS_UserInfo_QueryUserInfoCallbackInfo *Info) {
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            if (Info->ResultCode != EOS_EResult::EOS_Success)
                            {
                                FString EpicAccountIdStr;
                                if (EOSString_EpicAccountId::ToString(EpicAccountId, EpicAccountIdStr) ==
                                    EOS_EResult::EOS_Success)
                                {
                                    UE_LOG(
                                        LogEOSFriends,
                                        Warning,
                                        TEXT("Unable to query user information for friend with Epic account ID '%s'. "
                                             "User will not appear "
                                             "in friends list."),
                                        *EpicAccountIdStr);
                                }
                                OnDone(nullptr);
                                return;
                            }

                            EOS_UserInfo_CopyUserInfoOptions CopyOpts = {};
                            CopyOpts.ApiVersion = EOS_USERINFO_COPYUSERINFO_API_LATEST;
                            CopyOpts.LocalUserId = UserId->GetEpicAccountId();
                            CopyOpts.TargetUserId = EpicAccountId;

                            EOS_UserInfo *UserInfo = nullptr;
                            EOS_EResult CopyResult = EOS_UserInfo_CopyUserInfo(This->EOSUserInfo, &CopyOpts, &UserInfo);
                            if (CopyResult != EOS_EResult::EOS_Success)
                            {
                                FString EpicAccountIdStr;
                                if (EOSString_EpicAccountId::ToString(EpicAccountId, EpicAccountIdStr) ==
                                    EOS_EResult::EOS_Success)
                                {
                                    UE_LOG(
                                        LogEOSFriends,
                                        Warning,
                                        TEXT("Unable to copy user information for friend with Epic account ID '%s'. "
                                             "User will not appear "
                                             "in friends list."),
                                        *EpicAccountIdStr);
                                }
                                OnDone(nullptr);
                                return;
                            }

                            OnDone(UserInfo);
                        }
                    });
                return true;
            }

            return false;
        },
        [WeakThis = GetWeakThis(this),
         UserId = StaticCastSharedRef<const FUniqueNetIdEAS>(UserId.AsShared()),
         FriendsReadyDelegate](const TArray<EOS_UserInfo *> &Results) {
            if (auto This = PinWeakThis(WeakThis))
            {
                TArray<TSharedRef<FOnlineFriend>> ArrayOfFriends;
                TUserIdMap<TSharedPtr<FOnlineFriend>> MapOfFriends;
                for (const auto &UserInfo : Results)
                {
                    if (UserInfo != nullptr)
                    {
                        // FOnlineFriendEAS takes ownership of UserInfo.
                        TSharedRef<const FUniqueNetIdEAS> FriendEASId = MakeShared<FUniqueNetIdEAS>(UserInfo->UserId);
                        TSharedRef<FOnlineFriendEAS> FriendEAS = MakeShared<FOnlineFriendEAS>(
                            FriendEASId,
                            This->Presence->GetOrCreatePresenceInfoForEpicId(FriendEASId).ToSharedRef(),
                            UserInfo);

                        ArrayOfFriends.Add(FriendEAS);
                        MapOfFriends.Add(*FriendEAS->GetUserId(), FriendEAS);
                    }
                }
                This->CachedFriendsArray.Add(*UserId, ArrayOfFriends);
                This->CachedFriendsMap.Add(*UserId, MapOfFriends);
                FriendsReadyDelegate.ExecuteIfBound();
            }
        });
}

FOnlineFriendsInterfaceEAS::FOnlineFriendsInterfaceEAS(
    EOS_HPlatform InPlatform,
    TSharedPtr<FOnlineIdentityInterfaceEAS, ESPMode::ThreadSafe> InIdentity)
    : EOSFriends(EOS_Platform_GetFriendsInterface(InPlatform))
    , EOSUserInfo(EOS_Platform_GetUserInfoInterface(InPlatform))
    , Identity(MoveTemp(InIdentity))
    , OnFriendsUpdateHandle()
    , CachedFriendsArray()
    , CachedFriendsMap()
    , Presence()
{
    check(this->EOSFriends != nullptr);
    check(this->EOSUserInfo != nullptr);
}

void FOnlineFriendsInterfaceEAS::RegisterEvents()
{
    EOS_Friends_AddNotifyFriendsUpdateOptions Opts = {};
    Opts.ApiVersion = EOS_FRIENDS_ADDNOTIFYFRIENDSUPDATE_API_LATEST;

    this->OnFriendsUpdateHandle =
        EOSRegisterEvent<EOS_HFriends, EOS_Friends_AddNotifyFriendsUpdateOptions, EOS_Friends_OnFriendsUpdateInfo>(
            this->EOSFriends,
            &Opts,
            EOS_Friends_AddNotifyFriendsUpdate,
            EOS_Friends_RemoveNotifyFriendsUpdate,
            [WeakThis = GetWeakThis(this)](const EOS_Friends_OnFriendsUpdateInfo *Data) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    for (auto i = 0; i < MAX_LOCAL_PLAYERS; i++)
                    {
                        if (This->Identity->GetLoginStatus(i) == ELoginStatus::LoggedIn &&
                            This->Identity->GetUniquePlayerId(i)->GetType() == REDPOINT_EAS_SUBSYSTEM)
                        {
                            EOS_EpicAccountId EpicAccountId = This->Identity->GetEpicAccountId(i);
                            if (EOSString_EpicAccountId::IsEqual(EpicAccountId, Data->LocalUserId))
                            {
                                This->CacheFriends(
                                    ((const FUniqueNetIdEAS &)This->Identity->GetUniquePlayerId(i).ToSharedRef().Get()),
                                    FFriendCacheReady::CreateLambda([WeakThis = GetWeakThis(This), i]() {
                                        if (auto This = PinWeakThis(WeakThis))
                                        {
                                            This->TriggerOnFriendsChangeDelegates(i);
                                        }
                                    }));
                            }
                        }
                    }
                }
            });
}

FOnlineFriendsInterfaceEAS::~FOnlineFriendsInterfaceEAS()
{
}

bool FOnlineFriendsInterfaceEAS::ReadFriendsList(
    int32 LocalUserNum,
    const FString &ListName,
    const FOnReadFriendsListComplete &Delegate)
{
    if (ListName != TEXT(""))
    {
        Delegate.ExecuteIfBound(
            LocalUserNum,
            false,
            ListName,
            TEXT("Expected empty list name for all friends operations in the EOS subsystem"));
        return true;
    }

    if (this->Identity->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
    {
        Delegate.ExecuteIfBound(LocalUserNum, false, ListName, TEXT("Local user is not logged in"));
        return true;
    }

    auto UserId = StaticCastSharedPtr<const FUniqueNetIdEAS>(this->Identity->GetUniquePlayerId(LocalUserNum));
    if (UserId == nullptr || !UserId->HasValidEpicAccountId())
    {
        this->CachedFriendsArray.Add(*UserId, TArray<TSharedRef<FOnlineFriend>>());
        this->CachedFriendsMap.Add(*UserId, TUserIdMap<TSharedPtr<FOnlineFriend>>());
        Delegate.ExecuteIfBound(LocalUserNum, true, ListName, TEXT(""));
        return true;
    }

    EOS_Friends_QueryFriendsOptions Opts = {};
    Opts.ApiVersion = EOS_FRIENDS_QUERYFRIENDS_API_LATEST;
    Opts.LocalUserId = UserId->GetEpicAccountId();

    EOSRunOperation<EOS_HFriends, EOS_Friends_QueryFriendsOptions, EOS_Friends_QueryFriendsCallbackInfo>(
        this->EOSFriends,
        &Opts,
        EOS_Friends_QueryFriends,
        [WeakThis = GetWeakThis(this), Delegate, LocalUserNum, ListName, UserId](
            const EOS_Friends_QueryFriendsCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Data->ResultCode != EOS_EResult::EOS_Success)
                {
                    Delegate.ExecuteIfBound(LocalUserNum, false, ListName, TEXT("Failed to retrieve online friends"));
                    return;
                }

                This->CacheFriends(
                    *UserId,
                    FFriendCacheReady::CreateLambda([WeakThis = GetWeakThis(This), LocalUserNum, Delegate, ListName]() {
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            Delegate.ExecuteIfBound(LocalUserNum, true, ListName, TEXT(""));
                        }
                    }));
            }
        });
    return true;
}

bool FOnlineFriendsInterfaceEAS::DeleteFriendsList(
    int32 LocalUserNum,
    const FString &ListName,
    const FOnDeleteFriendsListComplete &Delegate)
{
    UE_LOG(LogEOSFriends, Error, TEXT("IOnlineFriends::DeleteFriendsList not supported for Epic Online Services"));
    return false;
}

bool FOnlineFriendsInterfaceEAS::SendInvite(
    int32 LocalUserNum,
    const FUniqueNetId &FriendId,
    const FString &ListName,
    const FOnSendInviteComplete &Delegate)
{
    UE_LOG(LogEOSFriends, Error, TEXT("IOnlineFriends::SendInvite not supported for Epic Online Services"));
    return false;
}

bool FOnlineFriendsInterfaceEAS::AcceptInvite(
    int32 LocalUserNum,
    const FUniqueNetId &FriendId,
    const FString &ListName,
    const FOnAcceptInviteComplete &Delegate)
{
    UE_LOG(LogEOSFriends, Error, TEXT("IOnlineFriends::AcceptInvite not supported for Epic Online Services"));
    return false;
}

bool FOnlineFriendsInterfaceEAS::RejectInvite(int32 LocalUserNum, const FUniqueNetId &FriendId, const FString &ListName)
{
    UE_LOG(LogEOSFriends, Error, TEXT("IOnlineFriends::RejectInvite not supported for Epic Online Services"));
    return false;
}

void FOnlineFriendsInterfaceEAS::SetFriendAlias(
    int32 LocalUserNum,
    const FUniqueNetId &FriendId,
    const FString &ListName,
    const FString &Alias,
    const FOnSetFriendAliasComplete &Delegate)
{
    // Not supported.
    Delegate.ExecuteIfBound(LocalUserNum, FriendId, ListName, OnlineRedpointEOS::Errors::NotImplemented());
}

void FOnlineFriendsInterfaceEAS::DeleteFriendAlias(
    int32 LocalUserNum,
    const FUniqueNetId &FriendId,
    const FString &ListName,
    const FOnDeleteFriendAliasComplete &Delegate)
{
    // Not supported.
    Delegate.ExecuteIfBound(LocalUserNum, FriendId, ListName, OnlineRedpointEOS::Errors::NotImplemented());
}

bool FOnlineFriendsInterfaceEAS::DeleteFriend(int32 LocalUserNum, const FUniqueNetId &FriendId, const FString &ListName)
{
    UE_LOG(LogEOSFriends, Error, TEXT("IOnlineFriends::DeleteFriend not supported for Epic Online Services"));
    return false;
}

bool FOnlineFriendsInterfaceEAS::GetFriendsList(
    int32 LocalUserNum,
    const FString &ListName,
    TArray<TSharedRef<FOnlineFriend>> &OutFriends)
{
    if (ListName != TEXT(""))
    {
        UE_LOG(LogEOSFriends, Error, TEXT("Expected empty list name for all friends operations in the EOS subsystem"));
        return false;
    }

    if (this->Identity->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
    {
        UE_LOG(LogEOSFriends, Error, TEXT("Local user is not logged in"));
        return false;
    }

    auto UserId = this->Identity->GetUniquePlayerId(LocalUserNum);
    if (UserId->GetType() != REDPOINT_EAS_SUBSYSTEM)
    {
        UE_LOG(LogEOSFriends, Error, TEXT("Local user is not using the Epic subsystem"));
        return false;
    }

    if (this->CachedFriendsArray.Contains(*UserId))
    {
        OutFriends = this->CachedFriendsArray[*UserId];
        return true;
    }
    else
    {
        UE_LOG(LogEOSFriends, Error, TEXT("Friends have not yet been cached for this user, call ReadFriendsList"));
        return false;
    }
}

TSharedPtr<FOnlineFriend> FOnlineFriendsInterfaceEAS::GetFriend(
    int32 LocalUserNum,
    const FUniqueNetId &FriendId,
    const FString &ListName)
{
    if (ListName != TEXT(""))
    {
        UE_LOG(LogEOSFriends, Error, TEXT("Expected empty list name for all friends operations in the EOS subsystem"));
        return nullptr;
    }

    if (this->Identity->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
    {
        UE_LOG(LogEOSFriends, Error, TEXT("Local user is not logged in"));
        return nullptr;
    }

    auto UserId = this->Identity->GetUniquePlayerId(LocalUserNum);
    if (UserId->GetType() != REDPOINT_EAS_SUBSYSTEM)
    {
        UE_LOG(LogEOSFriends, Error, TEXT("Local user is not using the Epic subsystem"));
        return nullptr;
    }

    if (this->CachedFriendsMap.Contains(*UserId))
    {
        if (this->CachedFriendsMap[*UserId].Contains(FriendId))
        {
            return this->CachedFriendsMap[*UserId][FriendId];
        }

        return nullptr;
    }
    else
    {
        UE_LOG(LogEOSFriends, Error, TEXT("Friends have not yet been cached for this user, call ReadFriendsList"));
        return nullptr;
    }
}

bool FOnlineFriendsInterfaceEAS::IsFriend(int32 LocalUserNum, const FUniqueNetId &FriendId, const FString &ListName)
{
    if (ListName != TEXT(""))
    {
        UE_LOG(LogEOSFriends, Error, TEXT("Expected empty list name for all friends operations in the EOS subsystem"));
        return false;
    }

    if (this->Identity->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
    {
        UE_LOG(LogEOSFriends, Error, TEXT("Local user is not logged in"));
        return false;
    }

    auto UserId = this->Identity->GetUniquePlayerId(LocalUserNum);
    if (UserId->GetType() != REDPOINT_EAS_SUBSYSTEM)
    {
        UE_LOG(LogEOSFriends, Error, TEXT("Local user is not using the Epic subsystem"));
        return false;
    }

    if (this->CachedFriendsMap.Contains(*UserId))
    {
        if (this->CachedFriendsMap[*UserId].Contains(FriendId))
        {
            return true;
        }

        return false;
    }
    else
    {
        UE_LOG(LogEOSFriends, Error, TEXT("Friends have not yet been cached for this user, call ReadFriendsList"));
        return false;
    }
}

bool FOnlineFriendsInterfaceEAS::QueryRecentPlayers(const FUniqueNetId &UserId, const FString &Namespace)
{
    UE_LOG(LogEOSFriends, Error, TEXT("IOnlineFriends::QueryRecentPlayers not supported for Epic Online Services"));
    return false;
}

bool FOnlineFriendsInterfaceEAS::GetRecentPlayers(
    const FUniqueNetId &UserId,
    const FString &Namespace,
    TArray<TSharedRef<FOnlineRecentPlayer>> &OutRecentPlayers)
{
    UE_LOG(LogEOSFriends, Error, TEXT("IOnlineFriends::GetRecentPlayers not supported for Epic Online Services"));
    return false;
}

void FOnlineFriendsInterfaceEAS::DumpRecentPlayers() const
{
    UE_LOG(LogEOSFriends, Error, TEXT("IOnlineFriends::DumpRecentPlayers not supported for Epic Online Services"));
}

bool FOnlineFriendsInterfaceEAS::BlockPlayer(int32 LocalUserNum, const FUniqueNetId &PlayerId)
{
    UE_LOG(LogEOSFriends, Error, TEXT("IOnlineFriends::BlockPlayer not supported for Epic Online Services"));
    return false;
}

bool FOnlineFriendsInterfaceEAS::UnblockPlayer(int32 LocalUserNum, const FUniqueNetId &PlayerId)
{
    UE_LOG(LogEOSFriends, Error, TEXT("IOnlineFriends::UnblockPlayer not supported for Epic Online Services"));
    return false;
}

bool FOnlineFriendsInterfaceEAS::QueryBlockedPlayers(const FUniqueNetId &UserId)
{
    UE_LOG(LogEOSFriends, Error, TEXT("IOnlineFriends::QueryBlockedPlayers not supported for Epic Online Services"));
    return false;
}

bool FOnlineFriendsInterfaceEAS::GetBlockedPlayers(
    const FUniqueNetId &UserId,
    TArray<TSharedRef<FOnlineBlockedPlayer>> &OutBlockedPlayers)
{
    UE_LOG(LogEOSFriends, Error, TEXT("IOnlineFriends::GetBlockedPlayers not supported for Epic Online Services"));
    return false;
}

void FOnlineFriendsInterfaceEAS::DumpBlockedPlayers() const
{
    UE_LOG(LogEOSFriends, Error, TEXT("IOnlineFriends::DumpBlockedPlayers not supported for Epic Online Services"));
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION