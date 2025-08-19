// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/OnlineFriendsInterfaceSynthetic.h"

#include "OnlineError.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/DelegatedSubsystems.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSErrorConv.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSMessagingHub.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/MultiOperation.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineFriendSynthetic.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineIdentityInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineUserCloudInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineUserEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/PDSFriends/FriendDatabase.h"
#include "OnlineSubsystemRedpointEOS/Shared/PDSFriends/OnlineFriendCached.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

FOnlineFriendsInterfaceSynthetic::FLocalUserFriendState::FLocalUserFriendState(
    const TSharedRef<const FUniqueNetIdEOS> &InLocalUserId,
    const TSharedRef<FOnlineUserCloudInterfaceEOS, ESPMode::ThreadSafe> &InUserCloudImpl)
    : FriendDatabase(MakeShared<FFriendDatabase>(InLocalUserId, InUserCloudImpl))
    , FriendCacheById()
    , FriendCacheByIndex()
    , RecentPlayerCache()
    , bDidQueryRecentPlayers(false)
    , BlockedPlayersCache()
    , bDidQueryBlockedPlayers(false)
{
}

FString FOnlineFriendsInterfaceSynthetic::MutateListName(const FWrappedFriendsInterface &Friends, FString ListName)
{
    for (const auto &Integration : this->RuntimePlatform->GetIntegrations())
    {
        Integration->MutateFriendsListNameIfRequired(Friends.SubsystemName, ListName);
    }
    return ListName;
}

/** Operation to call ReadFriendsList on each of the delegated subsystems */
bool FOnlineFriendsInterfaceSynthetic::Op_ReadFriendsList(
    int32 LocalUserNum,
    const FString &ListName,
    FName InSubsystemName,
    const TWeakPtr<IOnlineIdentity, ESPMode::ThreadSafe> &InIdentity,
    const TWeakPtr<IOnlineFriends, ESPMode::ThreadSafe> &InFriends,
    const std::function<void(bool OutValue)> &OnDone)
{
    if (auto Friends = InFriends.Pin())
    {
        if (auto DelegatedIdentity = InIdentity.Pin())
        {
            if (DelegatedIdentity->GetLoginStatus(LocalUserNum) == ELoginStatus::LoggedIn)
            {
                return Friends->ReadFriendsList(
                    LocalUserNum,
                    ListName,
                    FOnReadFriendsListComplete::CreateLambda([OnDone, InSubsystemName](
                                                                 int32 LocalUserNum,
                                                                 bool bWasSuccessful,
                                                                 const FString &ListName,
                                                                 const FString &ErrorStr) {
                        if (!bWasSuccessful)
                        {
                            FOnlineError Err = OnlineRedpointEOS::Errors::UnexpectedError(
                                TEXT("FOnlineFriendsInterfaceSynthetic::Op_ReadFriendsList"),
                                FString::Printf(
                                    TEXT("(%s) Error from delegated subsystem while performing ReadFriends operation: "
                                         "%s"),
                                    *InSubsystemName.ToString(),
                                    *ErrorStr));
                            UE_LOG(LogEOSFriends, Error, TEXT("%s"), *Err.ToLogString());
                        }
                        OnDone(bWasSuccessful);
                    }));
            }
            else
            {
                // We don't actually make the ReadFriendsList call if the local user isn't signed in, but we don't
                // want this to be reported as "the delegated subsystem had an error". So just pretend like we were
                // successful and retrieved no friends.
                OnDone(true);
                return true;
            }
        }
    }

    return false;
}

/** Once we've done calling ReadFriendsList on all subsystems, proceed */
void FOnlineFriendsInterfaceSynthetic::Dn_ReadFriendsList(
    int32 LocalUserNum,
    const FString &ListName,
    const FOnReadFriendsListComplete &Delegate,
    const TArray<bool> &OutValues)
{
    // If none of the subsystems returned successfully, report failure.
    bool AnySuccess = false;
    for (auto Result : OutValues)
    {
        if (Result)
        {
            AnySuccess = true;
            break;
        }
    }
    if (!AnySuccess)
    {
        Delegate.ExecuteIfBound(
            LocalUserNum,
            false,
            ListName,
            FString(TEXT("None of the delegated subsystems returned successfully")));
        return;
    }

    // Now that we have called ReadFriendsList on all the subsystems, we're going to start
    // aggregating all of their friends lists together.
    FMultiOperation<FWrappedFriendsInterface, TArray<TSharedRef<FOnlineFriend>>>::Run(
        this->OnlineFriends,
        [WeakThis = GetWeakThis(this), LocalUserNum, ListName](
            const FWrappedFriendsInterface &InFriends,
            const std::function<void(TArray<TSharedRef<FOnlineFriend>> OutValue)> &OnDone) {
            if (auto This = PinWeakThis(WeakThis))
            {
                return This->Op_GetFriendsList(
                    LocalUserNum,
                    This->MutateListName(InFriends, ListName),
                    InFriends.OnlineIdentity,
                    InFriends.OnlineFriends,
                    OnDone);
            }
            return false;
        },
        [WeakThis = GetWeakThis(this), LocalUserNum, ListName, Delegate](
            TArray<TArray<TSharedRef<FOnlineFriend>>> OutValues) {
            if (auto This = PinWeakThis(WeakThis))
            {
                This->Dn_GetFriendsList(LocalUserNum, ListName, Delegate, MoveTemp(OutValues));
            }
        });
}

/** Call GetFriendsList on each of the delegated subsystems */
bool FOnlineFriendsInterfaceSynthetic::Op_GetFriendsList(
    int32 LocalUserNum,
    const FString &ListName,
    const TWeakPtr<IOnlineIdentity, ESPMode::ThreadSafe> &InIdentity,
    const TWeakPtr<IOnlineFriends, ESPMode::ThreadSafe> &InFriends,
    const std::function<void(TArray<TSharedRef<FOnlineFriend>> OutValue)> &OnDone)
{
    if (auto Friends = InFriends.Pin())
    {
        if (auto DelegatedIdentity = InIdentity.Pin())
        {
            if (DelegatedIdentity->GetLoginStatus(LocalUserNum) == ELoginStatus::LoggedIn)
            {
                TArray<TSharedRef<FOnlineFriend>> SubFriends;
                bool Outcome = Friends->GetFriendsList(LocalUserNum, ListName, SubFriends);
                if (Outcome)
                {
                    OnDone(SubFriends);
                }
                return Outcome;
            }
            else
            {
                // We don't actually make the GetFriendsList call if the local user isn't signed in, but we don't
                // want this to be reported as "the delegated subsystem had an error". So just pretend like we were
                // successful and retrieved no friends.
                TArray<TSharedRef<FOnlineFriend>> EmptyFriends;
                OnDone(EmptyFriends);
                return true;
            }
        }
    }

    return false;
}

/** We've got all the friends from all the delegated subsystems */
void FOnlineFriendsInterfaceSynthetic::Dn_GetFriendsList(
    int32 LocalUserNum,
    const FString &ListName,
    const FOnReadFriendsListComplete &Delegate,
    TArray<TArray<TSharedRef<FOnlineFriend>>> OutValues)
{
    // Get external account information for all of the friends, using the native platform APIs.
    TArray<TSharedPtr<FFriendInfo>> FriendInfos;
    TUserIdMap<TSharedPtr<FFriendInfo>> UserIdToFriendInfo;
    for (auto i = 0; i < OutValues.Num(); i++)
    {
        auto List = OutValues[i];
        for (const auto &FriendRef : List)
        {
            auto FriendInfo = MakeShared<FFriendInfo>();
            FriendInfo->SubsystemName = this->OnlineFriends[i].SubsystemName;
            FriendInfo->FriendRef = FriendRef;
            FriendInfo->HasExternalInfo = false;
            FriendInfo->IsCachedFromFriendDatabase = false;
            FriendInfo->SubsystemIndex = i;

            if (FriendRef->GetUserId()->GetType() == REDPOINT_EAS_SUBSYSTEM)
            {
                FriendInfo->ExternalInfo.AccountId = FriendRef->GetUserId()->ToString();
                FriendInfo->ExternalInfo.AccountIdType = EOS_EExternalAccountType::EOS_EAT_EPIC;
                FriendInfo->HasExternalInfo = true;
            }
            else
            {
                for (const auto &Integration : this->RuntimePlatform->GetIntegrations())
                {
                    if (Integration->CanProvideExternalAccountId(FriendRef->GetUserId().Get()))
                    {
                        FriendInfo->ExternalInfo = Integration->GetExternalAccountId(FriendRef->GetUserId().Get());
                        FriendInfo->HasExternalInfo = true;
                        break;
                    }
                }
            }

            FriendInfos.Add(FriendInfo);
            UserIdToFriendInfo.Add(*FriendRef->GetUserId(), FriendInfo);
        }
    }

    if (this->RuntimePlatform->UseCrossPlatformFriendStorage())
    {
        // For all of the friend infos that we got, fetch the avatar URLs of every friend. We'll store a copy of
        // the avatar URL in our friend database so that we can display friend avatars even when not on the
        // platform the friend is from.
        FMultiOperation<TSharedPtr<FFriendInfo>, bool>::Run(
            FriendInfos,
            [WeakThis = GetWeakThis(this),
             LocalUserNum](const TSharedPtr<FFriendInfo> &Entry, const std::function<void(bool)> &OnDone) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    return This->Op_ResolveAvatarForFriends(LocalUserNum, Entry, OnDone);
                }
                return false;
            },
            [WeakThis = GetWeakThis(this),
             LocalUserNum,
             ListName,
             Delegate,
             OutValues,
             FriendInfos,
             UserIdToFriendInfo](const TArray<bool> &Results) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    This->Dn_ResolveAvatarForFriends(
                        LocalUserNum,
                        ListName,
                        Delegate,
                        OutValues,
                        FriendInfos,
                        UserIdToFriendInfo);
                }
            });
    }
    else
    {
        this->Dn_FriendDatabaseFlushed(true, LocalUserNum, ListName, Delegate, OutValues, FriendInfos);
    }
}

/** Try to get the avatar URL of the friend */
bool FOnlineFriendsInterfaceSynthetic::Op_ResolveAvatarForFriends(
    int32 LocalUserNum,
    const TSharedPtr<FFriendInfo> &Friend,
    const std::function<void(bool)> &OnDone)
{
    checkf(
        this->RuntimePlatform->UseCrossPlatformFriendStorage(),
        TEXT("Should not call Op_ResolveAvatarForFriends if cross-platform friend storage is not permitted!"));

    auto LocalIdentity = this->OnlineFriends[Friend->SubsystemIndex].OnlineIdentity.Pin();
    auto LocalAvatar = this->OnlineFriends[Friend->SubsystemIndex].OnlineAvatar.Pin();
    if (!LocalIdentity.IsValid() || !LocalAvatar.IsValid())
    {
        return false;
    }

    TSharedPtr<const FUniqueNetId> LocalUserId = LocalIdentity->GetUniquePlayerId(LocalUserNum);
    if (!LocalUserId.IsValid())
    {
        return false;
    }

    return LocalAvatar->GetAvatarUrl(
        *LocalUserId,
        *Friend->FriendRef->GetUserId(),
        TEXT(""),
        FOnGetAvatarUrlComplete::CreateLambda(
            [Friend, OnDone](const bool &bWasSuccessful, const FString &ResultAvatarUrl) {
                if (bWasSuccessful)
                {
                    Friend->FetchedAvatarUrl = ResultAvatarUrl;
                }
                OnDone(bWasSuccessful);
            }));
}

/** We've retrieved all the avatar URLs we can, sync to / import from friend database */
void FOnlineFriendsInterfaceSynthetic::Dn_ResolveAvatarForFriends(
    int32 LocalUserNum,
    const FString &ListName,
    const FOnReadFriendsListComplete &Delegate,
    const TArray<TArray<TSharedRef<FOnlineFriend>>> &OutValues,
    TArray<TSharedPtr<FFriendInfo>> FriendInfos,
    const TUserIdMap<TSharedPtr<FFriendInfo>> &UserIdToFriendInfo)
{
    // For subsystems that were present, sync the list of friends back to the friends database. For
    // subsystems that were not present, pull the friends from the friends database.
    checkf(
        this->RuntimePlatform->UseCrossPlatformFriendStorage(),
        TEXT("Should not call Dn_ResolveAvatarForFriends if cross-platform friend storage is not permitted!"));
    if (this->RuntimePlatform->UseCrossPlatformFriendStorage())
    {
        auto &DbCache = this->FriendState[LocalUserNum]->FriendDatabase->GetDelegatedSubsystemCachedFriends();
        TSet<FName> SubsystemsPresent;
        bool bDbModified = false;
        for (auto i = 0; i < OutValues.Num(); i++)
        {
            SubsystemsPresent.Add(this->OnlineFriends[i].SubsystemName);

            TMap<FString, FSerializedCachedFriend> NewCacheData;
            for (const auto &FriendRef : OutValues[i])
            {
                FSerializedCachedFriend CachedFriend = {};
                CachedFriend.AccountId = FriendRef->GetUserId()->ToString();
                CachedFriend.AccountIdBytes =
                    TArray<uint8>(FriendRef->GetUserId()->GetBytes(), FriendRef->GetUserId()->GetSize());
                CachedFriend.AccountIdTypeHash = GetTypeHash(FriendRef->GetUserId());
                CachedFriend.AccountDisplayName = FriendRef->GetDisplayName();
                CachedFriend.AccountRealName = FriendRef->GetRealName();
                if (UserIdToFriendInfo.Contains(*FriendRef->GetUserId()))
                {
                    CachedFriend.AccountAvatarUrl = UserIdToFriendInfo[*FriendRef->GetUserId()]->FetchedAvatarUrl;
                }

                if (FriendRef->GetUserId()->GetType() == REDPOINT_EAS_SUBSYSTEM)
                {
                    CachedFriend.ExternalAccountId = FriendRef->GetUserId()->ToString();
                    CachedFriend.ExternalAccountIdType = (int32)EOS_EExternalAccountType::EOS_EAT_EPIC;
                }
                else
                {
                    for (const auto &Integration : this->RuntimePlatform->GetIntegrations())
                    {
                        if (Integration->CanProvideExternalAccountId(FriendRef->GetUserId().Get()))
                        {
                            auto ExternalInfo = Integration->GetExternalAccountId(FriendRef->GetUserId().Get());
                            CachedFriend.ExternalAccountId = ExternalInfo.AccountId;
                            CachedFriend.ExternalAccountIdType = (int32)ExternalInfo.AccountIdType;
                            break;
                        }
                    }
                }

                NewCacheData.Add(FriendRef->GetUserId()->ToString(), CachedFriend);
            }
            DbCache.Add(this->OnlineFriends[i].SubsystemName, NewCacheData);
            bDbModified = true;
        }
        for (const auto &KV : DbCache)
        {
            if (SubsystemsPresent.Contains(KV.Key))
            {
                continue;
            }

            for (const auto &FV : KV.Value)
            {
                auto FriendInfo = MakeShared<FFriendInfo>();
                FriendInfo->SubsystemName = KV.Key;
                FriendInfo->FriendRef = MakeShared<FOnlineFriendCached>(KV.Key, FV.Value);
                FriendInfo->HasExternalInfo = !FV.Value.ExternalAccountId.IsEmpty();
                FriendInfo->IsCachedFromFriendDatabase = true;
                if (FriendInfo->HasExternalInfo)
                {
                    FriendInfo->ExternalInfo.AccountId = FV.Value.ExternalAccountId;
                    FriendInfo->ExternalInfo.AccountIdType = (EOS_EExternalAccountType)FV.Value.ExternalAccountIdType;
                }
                FriendInfos.Add(FriendInfo);
            }
        }
        if (bDbModified)
        {
            this->FriendState[LocalUserNum]->FriendDatabase->DirtyAndFlush(
                FFriendDatabaseOperationComplete::CreateThreadSafeSP(
                    this,
                    &FOnlineFriendsInterfaceSynthetic::Dn_FriendDatabaseFlushed,
                    LocalUserNum,
                    ListName,
                    Delegate,
                    OutValues,
                    FriendInfos));
            return;
        }
    }

    this->Dn_FriendDatabaseFlushed(true, LocalUserNum, ListName, Delegate, OutValues, FriendInfos);
}

/** We've flushed the friend database to Player Data Storage, do the unification */
void FOnlineFriendsInterfaceSynthetic::Dn_FriendDatabaseFlushed(
    bool bWasSuccessful,
    int32 LocalUserNum,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FString ListName,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FOnReadFriendsListComplete Delegate,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TArray<TArray<TSharedRef<FOnlineFriend>>> OutValues,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TArray<TSharedPtr<FFriendInfo>> FriendInfos)
{
    // Wrangle the friends so that they're grouped by the external account type, so that
    // we can batch the operations together.
    TMap<EOS_EExternalAccountType, TArray<TSharedPtr<FFriendInfo>>> FriendsByEOSExternalAccountType =
        GroupBy<EOS_EExternalAccountType, TSharedPtr<FFriendInfo>>(
            MoveTemp(FriendInfos),
            [](const TSharedPtr<FFriendInfo> &Info) -> EOS_EExternalAccountType {
                if (Info->HasExternalInfo)
                {
                    return (EOS_EExternalAccountType)Info->ExternalInfo.AccountIdType;
                }
                return (EOS_EExternalAccountType)-1;
            });
    TArray<FUnresolvedFriendEntries> FriendsByEOSExternalAccountTypeUnpacked =
        UnpackMap(FriendsByEOSExternalAccountType);

    // Resolve friends lists by type.
    FMultiOperation<FUnresolvedFriendEntries, TResolvedFriendEntries>::Run(
        FriendsByEOSExternalAccountTypeUnpacked,
        [WeakThis = GetWeakThis(this), LocalUserNum](
            const FUnresolvedFriendEntries &EntriesByType,
            const std::function<void(TResolvedFriendEntries)> &OnDone) {
            if (auto This = PinWeakThis(WeakThis))
            {
                return This->Op_ResolveFriendsListByType(LocalUserNum, EntriesByType, OnDone);
            }
            return false;
        },
        [WeakThis = GetWeakThis(this), LocalUserNum, ListName, Delegate](
            const TArray<TResolvedFriendEntries> &Results) {
            if (auto This = PinWeakThis(WeakThis))
            {
                This->Dn_ResolveFriendsListByType(LocalUserNum, ListName, Delegate, Results);
            }
        });
}

/** For all the friends, grouped by their EOS external account ID type, try to resolve their EOS product ID in batches
 */
bool FOnlineFriendsInterfaceSynthetic::Op_ResolveFriendsListByType(
    int32 LocalUserNum,
    const FUnresolvedFriendEntries &EntriesByType,
    const std::function<void(TResolvedFriendEntries)> &OnDone)
{
    if (EntriesByType.Key == (EOS_EExternalAccountType)-1)
    {
        // Return as-is.
        TArray<TResolvedFriendEntry> AutoFailResults;
        for (auto FriendEntry : EntriesByType.Value)
        {
            AutoFailResults.Add(TResolvedFriendEntry(nullptr, FriendEntry));
        }
        OnDone(TResolvedFriendEntries(EntriesByType.Key, AutoFailResults));
        return true;
    }

    auto EOSUser = StaticCastSharedPtr<const FUniqueNetIdEOS>(this->Identity->GetUniquePlayerId(LocalUserNum));
    if (!EOSUser.IsValid() || !EOSUser->HasValidProductUserId())
    {
        // Can't do this when the user isn't logged in or isn't valid.
        TArray<TResolvedFriendEntry> AutoFailResults;
        for (auto FriendEntry : EntriesByType.Value)
        {
            AutoFailResults.Add(TResolvedFriendEntry(nullptr, FriendEntry));
        }
        OnDone(TResolvedFriendEntries(EntriesByType.Key, AutoFailResults));
        return true;
    }

    FMultiOperation<FUnresolvedFriendEntry, TResolvedFriendEntry>::RunBatched(
        EntriesByType.Value,
        EOS_CONNECT_QUERYEXTERNALACCOUNTMAPPINGS_MAX_ACCOUNT_IDS,
        [WeakThis = GetWeakThis(this), EntriesByType, EOSUser](
            TArray<FUnresolvedFriendEntry> &Batch,
            const std::function<void(TArray<TResolvedFriendEntry> OutBatch)> &OnDone) -> bool {
            if (auto This = PinWeakThis(WeakThis))
            {
                EOS_Connect_QueryExternalAccountMappingsOptions Opts = {};
                Opts.ApiVersion = EOS_CONNECT_QUERYEXTERNALACCOUNTMAPPINGS_API_LATEST;
                Opts.AccountIdType = EntriesByType.Key;
                Opts.LocalUserId = EOSUser->GetProductUserId();
                EOSString_Connect_ExternalAccountId::AllocateToCharListViaAccessor<FUnresolvedFriendEntry>(
                    Batch,
                    [](const FUnresolvedFriendEntry &Entry) {
                        return Entry->ExternalInfo.AccountId;
                    },
                    Opts.ExternalAccountIdCount,
                    Opts.ExternalAccountIds);

                EOSRunOperation<
                    EOS_HConnect,
                    EOS_Connect_QueryExternalAccountMappingsOptions,
                    EOS_Connect_QueryExternalAccountMappingsCallbackInfo>(
                    This->EOSConnect,
                    &Opts,
                    EOS_Connect_QueryExternalAccountMappings,
                    [WeakThis = GetWeakThis(This), Opts, Batch, OnDone, EOSUser, EntriesByType](
                        const EOS_Connect_QueryExternalAccountMappingsCallbackInfo *Info) {
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            TArray<TResolvedFriendEntry> Results;

                            auto FreeMem = [Opts]() {
                                EOSString_Connect_ExternalAccountId::FreeFromCharListConst(
                                    Opts.ExternalAccountIdCount,
                                    Opts.ExternalAccountIds);
                            };

                            if (Info->ResultCode != EOS_EResult::EOS_Success)
                            {
                                // Not successful, return this batch with no EOS product user IDs.
                                for (auto i = 0; i < Batch.Num(); i++)
                                {
                                    Results.Add(TResolvedFriendEntry(nullptr, Batch[i]));
                                }
                                FreeMem();
                                OnDone(Results);
                                return;
                            }

                            // Otherwise copy product IDs out.
                            for (auto i = 0; i < Batch.Num(); i++)
                            {
                                EOS_Connect_GetExternalAccountMappingsOptions GetOpts = {};
                                GetOpts.ApiVersion = EOS_CONNECT_GETEXTERNALACCOUNTMAPPINGS_API_LATEST;
                                GetOpts.LocalUserId = EOSUser->GetProductUserId();
                                GetOpts.AccountIdType = EntriesByType.Key;
                                GetOpts.TargetExternalUserId = Opts.ExternalAccountIds[i];
                                auto ProductUserId = EOS_Connect_GetExternalAccountMapping(This->EOSConnect, &GetOpts);
                                if (EOSString_ProductUserId::IsValid(ProductUserId))
                                {
                                    Results.Add(TResolvedFriendEntry(ProductUserId, Batch[i]));
                                }
                                else
                                {
                                    Results.Add(TResolvedFriendEntry(nullptr, Batch[i]));
                                }
                            }

                            FreeMem();
                            OnDone(Results);
                            return;
                        }
                    });
                return true;
            }
            else
            {
                return false;
            }
        },
        [OnDone, EntriesByType](TArray<TResolvedFriendEntry> OutResults) {
            OnDone(TResolvedFriendEntries(EntriesByType.Key, OutResults));
        });
    return true;
}

/* We have all the resolved results, now merge friends based on their EOS product user ID if present */
void FOnlineFriendsInterfaceSynthetic::Dn_ResolveFriendsListByType(
    int32 LocalUserNum,
    const FString &ListName,
    const FOnReadFriendsListComplete &Delegate,
    const TArray<TResolvedFriendEntries> &Results)
{
    // Flatten nested arrays into a single array of friend refs.
    TArray<TResolvedFriendEntry> ResolvedFriendRefs;
    for (const auto &KV : Results)
    {
        for (const auto &ResolvedFriendRef : KV.Value)
        {
            ResolvedFriendRefs.Add(ResolvedFriendRef);
        }
    }

    // Group by the EOS product user IDs.
    auto FriendRefsByExternalId =
        GroupBy<EOS_ProductUserId, TResolvedFriendEntry>(ResolvedFriendRefs, [](const TResolvedFriendEntry &Ref) {
            return Ref.Key;
        });

    // For friends with EOS account IDs, we now need to call FOnlineFriendEOS::NewAndWaitUntilReady to get
    // the EOS account, and then we'll use the EOS account as the first wrapped friend to the synthetic entry.
    TArray<TSharedRef<const FUniqueNetIdEOS>> UniqueIdsToResolve;
    TUserIdMap<bool> UniqueIdsInList;
    for (const auto &KV : FriendRefsByExternalId)
    {
        if (EOSString_ProductUserId::IsNone(KV.Key))
        {
            // These are non-EOS friends.
            continue;
        }

        auto Id = MakeShared<FUniqueNetIdEOS>(KV.Key);
        if (!UniqueIdsInList.Contains(*Id))
        {
            UniqueIdsToResolve.Add(Id);
            UniqueIdsInList.Add(*Id, true);
        }
    }

    // Add EOS accounts from the user's friend database, including friends that are pending invites.
    TArray<TSharedRef<const FUniqueNetIdEOS>> FriendsFromDb;
    for (const auto &Entry : this->FriendState[LocalUserNum]->FriendDatabase->GetFriends())
    {
        EOS_ProductUserId PUID;
        if (EOSString_ProductUserId::FromString(Entry.Key->ToString(), PUID) == EOS_EResult::EOS_Success)
        {
            auto Id = StaticCastSharedRef<const FUniqueNetIdEOS>(Entry.Key);
            if (!UniqueIdsInList.Contains(*Id))
            {
                UniqueIdsToResolve.Add(Id);
                UniqueIdsInList.Add(*Id, true);
            }
            if (!FriendRefsByExternalId.Contains(PUID))
            {
                FriendsFromDb.Add(Id);
            }
        }
    }
    for (const auto &Entry : this->FriendState[LocalUserNum]->FriendDatabase->GetPendingFriendRequests())
    {
        if (Entry.Value.Mode == ESerializedPendingFriendRequestMode::InboundPendingSendRejection ||
            Entry.Value.Mode == ESerializedPendingFriendRequestMode::OutboundPendingSendDeletion)
        {
            // Should not appear on the friends list. Only exists in the database because we still have
            // to send the message to the target user.
            continue;
        }

        if (Entry.Value.Mode == ESerializedPendingFriendRequestMode::InboundPendingSendAcceptance)
        {
            // There will already be an entry from the friends list.
            continue;
        }

        EOS_ProductUserId PUID;
        if (EOSString_ProductUserId::FromString(Entry.Key->ToString(), PUID) == EOS_EResult::EOS_Success)
        {
            auto Id = StaticCastSharedRef<const FUniqueNetIdEOS>(Entry.Key);
            if (!UniqueIdsInList.Contains(*Id))
            {
                UniqueIdsToResolve.Add(Id);
                UniqueIdsInList.Add(*Id, true);
            }
            if (!FriendRefsByExternalId.Contains(PUID))
            {
                // The FOnlineFriendSynthetic class looks up the invite status if the EOS user
                // ID is in the friend's database. This keeps it's invite status immediately
                // up to date without having to call ReadFriendsList again.
                FriendsFromDb.Add(Id);
            }
        }
    }

    // Determine the querying user.
    auto QueryingUserIdEOS =
        StaticCastSharedPtr<const FUniqueNetIdEOS>(this->Identity->GetUniquePlayerId(LocalUserNum));
    if (!QueryingUserIdEOS.IsValid() || !QueryingUserIdEOS->HasValidProductUserId())
    {
        // Can't do this when the user isn't logged in or isn't valid.
        this->Dn_ResolveEOSAccount(
            LocalUserNum,
            ListName,
            Delegate,
            FriendRefsByExternalId,
            TArray<TSharedPtr<FOnlineUserEOS>>(),
            TArray<TSharedRef<const FUniqueNetIdEOS>>());
    }
    else
    {
        // Resolve EOS accounts.
        FOnlineUserEOS::Get(
            this->UserFactory.ToSharedRef(),
            QueryingUserIdEOS.ToSharedRef(),
            UniqueIdsToResolve,
            [WeakThis = GetWeakThis(this), LocalUserNum, ListName, Delegate, FriendRefsByExternalId, FriendsFromDb](
                const TUserIdMap<TSharedPtr<FOnlineUserEOS>> &ResolvedEOSAccountsMap) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    TArray<TSharedPtr<FOnlineUserEOS>> Array;
                    for (const auto &KV : ResolvedEOSAccountsMap)
                    {
                        Array.Add(KV.Value);
                    }
                    This->Dn_ResolveEOSAccount(
                        LocalUserNum,
                        ListName,
                        Delegate,
                        FriendRefsByExternalId,
                        Array,
                        FriendsFromDb);
                }
            });
    }
}

/* We have now resolved all of the EOS product user IDs into FOnlineFriends to be used as the first entry for each
 * synthetic entry */
void FOnlineFriendsInterfaceSynthetic::Dn_ResolveEOSAccount(
    int32 LocalUserNum,
    const FString &ListName,
    const FOnReadFriendsListComplete &Delegate,
    TMap<EOS_ProductUserId, TArray<TResolvedFriendEntry>> FriendRefsByExternalId,
    const TArray<TSharedPtr<FOnlineUserEOS>> &ResolvedEOSAccounts,
    const TArray<TSharedRef<const FUniqueNetIdEOS>> &FriendsFromDb)
{
    // Make a dictionary of FUniqueNetId -> ResolvedEOSAccount so that
    // we can quickly look it up.
    TUserIdMap<TSharedPtr<FOnlineUserEOS>> ResolvedEOSAccountsById;
    for (const auto &Account : ResolvedEOSAccounts)
    {
        if (Account.IsValid())
        {
            ResolvedEOSAccountsById.Add(*Account->GetUserId(), Account);
        }
    }

    // If we don't have a state for this local user; this is happening after the user logged out,
    // so nothing is valid any more.
    if (!this->FriendState.Contains(LocalUserNum))
    {
        Delegate.ExecuteIfBound(
            LocalUserNum,
            false,
            ListName,
            OnlineRedpointEOS::Errors::InvalidUser(
                TEXT("FOnlineFriendsInterfaceSynthetic::Dn_ResolveEOSAccount"),
                TEXT("The user had logged out by the time the ReadFriendsList operation completed."))
                .ToLogString());
    }

    // Populate the friend cache.
    this->FriendState[LocalUserNum]->FriendCacheById.Empty();
    this->FriendState[LocalUserNum]->FriendCacheByIndex.Empty();
    int32_t FriendsUnifiedCount = 0;
    int32_t FriendsNotUnifiedCount = 0;
    int32_t FriendsFromDbCount = 0;
    for (const auto &UserId : FriendsFromDb)
    {
        if (ResolvedEOSAccountsById.Contains(*UserId))
        {
            TMap<FName, TSharedPtr<FOnlineFriend>> FriendEntries;
            auto SyntheticFriend = MakeShared<FOnlineFriendSynthetic>(
                ResolvedEOSAccountsById[*UserId],
                FriendEntries,
                this->FriendState[LocalUserNum]->FriendDatabase);
            this->FriendState[LocalUserNum]->FriendCacheByIndex.Add(SyntheticFriend);
            this->FriendState[LocalUserNum]->FriendCacheById.Add(*SyntheticFriend->GetUserId(), SyntheticFriend);
            FriendsUnifiedCount++;
        }
    }
    for (const auto &KV : FriendRefsByExternalId)
    {
        if (EOSString_ProductUserId::IsNone(KV.Key))
        {
            // These are non-EOS friends.
            continue;
        }

        TSharedRef<const FUniqueNetIdEOS> UserIdEOS = MakeShared<FUniqueNetIdEOS>(KV.Key);
        TMap<FName, TSharedPtr<FOnlineFriend>> FriendEntries;
        TSharedPtr<FOnlineUserEOS> UserEOS = nullptr;
        if (ResolvedEOSAccountsById.Contains(*UserIdEOS) && ResolvedEOSAccountsById[*UserIdEOS].IsValid())
        {
            UserEOS = ResolvedEOSAccountsById[*UserIdEOS];
        }
        for (const auto &EOSFriendRef : KV.Value)
        {
            FriendEntries.Add(EOSFriendRef.Value->SubsystemName, EOSFriendRef.Value->FriendRef);
        }
        auto SyntheticFriend =
            MakeShared<FOnlineFriendSynthetic>(UserEOS, FriendEntries, this->FriendState[LocalUserNum]->FriendDatabase);
        this->FriendState[LocalUserNum]->FriendCacheByIndex.Add(SyntheticFriend);
        this->FriendState[LocalUserNum]->FriendCacheById.Add(*SyntheticFriend->GetUserId(), SyntheticFriend);
        FriendsUnifiedCount++;
    }
    if (FriendRefsByExternalId.Contains(nullptr))
    {
        for (const auto &NonEOSFriend : FriendRefsByExternalId[nullptr])
        {
            TMap<FName, TSharedPtr<FOnlineFriend>> SingleFriendEntry;
            SingleFriendEntry.Add(NonEOSFriend.Value->SubsystemName, NonEOSFriend.Value->FriendRef);
            auto SyntheticFriend = MakeShared<FOnlineFriendSynthetic>(
                nullptr,
                SingleFriendEntry,
                this->FriendState[LocalUserNum]->FriendDatabase);
            this->FriendState[LocalUserNum]->FriendCacheByIndex.Add(SyntheticFriend);
            this->FriendState[LocalUserNum]->FriendCacheById.Add(
                *NonEOSFriend.Value->FriendRef->GetUserId(),
                SyntheticFriend);
            FriendsNotUnifiedCount++;
        }
    }

    UE_LOG(
        LogEOSFriends,
        Verbose,
        TEXT("ReadFriendsList complete, %d unified friends, %d non-unified friends, %d database friends"),
        FriendsUnifiedCount,
        FriendsNotUnifiedCount,
        FriendsFromDbCount);

    // We are done.
    Delegate.ExecuteIfBound(LocalUserNum, true, ListName, FString(TEXT("")));
    return;
}

void FOnlineFriendsInterfaceSynthetic::InitIfNeeded()
{
    if (this->bInitialized)
    {
        return;
    }

    if (this->SubsystemEAS != nullptr)
    {
        FWrappedFriendsInterface WrappedInterface = {
            REDPOINT_EAS_SUBSYSTEM,
            this->SubsystemEAS->GetIdentityInterface(),
            this->SubsystemEAS->GetFriendsInterface(),
            nullptr,
        };
        this->OnlineFriends.Add(WrappedInterface);
    }
    struct W
    {
        TWeakPtr<IOnlineIdentity, ESPMode::ThreadSafe> Identity;
        TWeakPtr<IOnlineFriends, ESPMode::ThreadSafe> Friends;
        TWeakPtr<IOnlineAvatar, ESPMode::ThreadSafe> Avatar;
    };
    for (const auto &KV : FDelegatedSubsystems::GetDelegatedSubsystems<W>(
             this->Config,
             this->InstanceName,
             [](FDelegatedSubsystems::ISubsystemContext &Ctx) -> TSharedPtr<W> {
                 auto Friends = Ctx.GetDelegatedInterface<IOnlineFriends>([](IOnlineSubsystem *OSS) {
                     return OSS->GetFriendsInterface();
                 });
                 auto IdentityDelegated = Ctx.GetDelegatedInterface<IOnlineIdentity>([](IOnlineSubsystem *OSS) {
                     return OSS->GetIdentityInterface();
                 });
                 auto Avatar = Ctx.GetDelegatedInterface<IOnlineAvatar>([](IOnlineSubsystem *OSS) {
                     return Online::GetAvatarInterface(OSS);
                 });
                 if (Friends.IsValid() && IdentityDelegated.IsValid())
                 {
                     return MakeShared<W>(W{IdentityDelegated, Friends, Avatar});
                 }
                 return nullptr;
             }))
    {
        this->OnlineFriends.Add({KV.Key, KV.Value->Identity, KV.Value->Friends, KV.Value->Avatar});
    }

    for (const auto &FriendsWeak : this->OnlineFriends)
    {
        if (auto Friends = FriendsWeak.OnlineFriends.Pin())
        {
            for (auto i = 0; i < MAX_LOCAL_PLAYERS; i++)
            {
                this->OnFriendsChangeDelegates.Add(
                    TTuple<TWeakPtr<IOnlineFriends, ESPMode::ThreadSafe>, int, FDelegateHandle>(
                        Friends,
                        i,
                        Friends->AddOnFriendsChangeDelegate_Handle(
                            i,
                            FOnFriendsChangeDelegate::CreateLambda([WeakThis = GetWeakThis(this), i]() {
                                if (auto This = PinWeakThis(WeakThis))
                                {
                                    if (This->PendingReadFriendsListFromPropagation.Contains(i))
                                    {
                                        UE_LOG(
                                            LogEOSFriends,
                                            Verbose,
                                            TEXT(
                                                "FOnlineFriendsInterfaceSynthetic: Received OnFriendsChange event from "
                                                "delegated subsystem for user %d, but there is already a "
                                                "ReadFriendsList operation in progress. Scheduling a second call after "
                                                "the first one finishes."),
                                            i);
                                        This->PendingReadFriendsListFromPropagation[i] = true;
                                    }
                                    else
                                    {
                                        UE_LOG(
                                            LogEOSFriends,
                                            Verbose,
                                            TEXT(
                                                "FOnlineFriendsInterfaceSynthetic: Received OnFriendsChange event from "
                                                "delegated subsystem for user %d, calling ReadFriendsList before "
                                                "propagating event..."),
                                            i);
                                        This->PendingReadFriendsListFromPropagation.Add(i, false);
                                        This->RunReadFriendsListForPropagation(i);
                                    }
                                }
                            }))));
            }
        }
    }

    this->bInitialized = true;
}

void FOnlineFriendsInterfaceSynthetic::OnMessageReceived(
    const FUniqueNetIdEOS &SenderId,
    const FUniqueNetIdEOS &ReceiverId,
    const FString &MessageType,
    const FString &MessageData)
{
    this->InitIfNeeded();

    // Determine the local user number.
    int32 LocalUserNum;
    if (!this->Identity->GetLocalUserNum(ReceiverId, LocalUserNum))
    {
        UE_LOG(
            LogEOSFriends,
            Error,
            TEXT("%s"),
            *OnlineRedpointEOS::Errors::UnexpectedError(
                 TEXT("FOnlineFriendsInterfaceSynthetic::OnMessageReceived"),
                 TEXT("Received a message for a user that is not locally signed in."))
                 .ToLogString());
        return;
    }

    // If we don't have a local state for this user, we need to make one immediately.
    if (!this->FriendState.Contains(LocalUserNum))
    {
        this->FriendState.Add(
            LocalUserNum,
            MakeShared<FLocalUserFriendState>(
                StaticCastSharedRef<const FUniqueNetIdEOS>(
                    this->Identity->GetUniquePlayerId(LocalUserNum).ToSharedRef()),
                this->UserCloud));
    }

    this->FriendState[LocalUserNum]->FriendDatabase->WaitUntilLoaded(FFriendDatabaseOperationComplete::CreateLambda(
        [WeakThis = GetWeakThis(this),
         LocalUserNum,
         SenderId = StaticCastSharedRef<const FUniqueNetIdEOS>(SenderId.AsShared()),
         ReceiverId = StaticCastSharedRef<const FUniqueNetIdEOS>(ReceiverId.AsShared()),
         MessageType](bool bWasSuccessful) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (!bWasSuccessful)
                {
                    UE_LOG(
                        LogEOSFriends,
                        Error,
                        TEXT("%s"),
                        *OnlineRedpointEOS::Errors::UnexpectedError(
                             TEXT("FOnlineFriendsInterfaceSynthetic::OnMessageReceived"),
                             TEXT("Unable to load the friend database for a user when receiving a message; the "
                                  "sender "
                                  "will be in an inconsistent state."))
                             .ToLogString());
                    return;
                }

                auto &Db = This->FriendState[LocalUserNum]->FriendDatabase;

                if (MessageType == TEXT("FriendRequest"))
                {
                    if (Db->GetPendingFriendRequests().Contains(*SenderId))
                    {
                        switch (Db->GetPendingFriendRequests()[*SenderId].Mode)
                        {
                        case ESerializedPendingFriendRequestMode::OutboundPendingSend:
                        case ESerializedPendingFriendRequestMode::OutboundPendingReceive:
                        case ESerializedPendingFriendRequestMode::OutboundPendingResponse:
                        case ESerializedPendingFriendRequestMode::InboundPendingResponse:
                        case ESerializedPendingFriendRequestMode::InboundPendingSendAcceptance:
                        default:
                            UE_LOG(
                                LogEOSFriends,
                                Warning,
                                TEXT("Ignoring invalid FriendRequest message for friend that was not in the "
                                     "correct state. Sent by %s to %s."),
                                *SenderId->ToString(),
                                *ReceiverId->ToString());
                            break;
                        }
                    }
                    else
                    {
                        // Store the incoming request in our pending users array.
                        FSerializedPendingFriendRequest FriendRequest = {};
                        FriendRequest.Mode = ESerializedPendingFriendRequestMode::InboundPendingResponse;
                        FriendRequest.ProductUserId = SenderId->GetProductUserIdString();
                        Db->GetPendingFriendRequests().Add(*SenderId, FriendRequest);

                        // Flush the database, then tell the other user we got their request.
                        Db->DirtyAndFlush(FFriendDatabaseOperationComplete::CreateLambda(
                            [WeakThis = GetWeakThis(This), SenderId, ReceiverId, LocalUserNum](bool bWasSuccessful) {
                                if (auto This = PinWeakThis(WeakThis))
                                {
                                    if (!bWasSuccessful)
                                    {
                                        UE_LOG(
                                            LogEOSFriends,
                                            Error,
                                            TEXT("%s"),
                                            *OnlineRedpointEOS::Errors::UnexpectedError(
                                                 TEXT("FOnlineFriendsInterfaceSynthetic::OnMessageReceived"),
                                                 TEXT("Unable to save the friend database for a user when receiving a "
                                                      "message; the sender will be in an inconsistent state."))
                                                 .ToLogString());
                                        return;
                                    }

                                    This->MessagingHub->SendMessage(
                                        *ReceiverId,
                                        *SenderId,
                                        TEXT("FriendRequestReceived"),
                                        TEXT(""),
                                        FOnEOSHubMessageSent::CreateLambda([WeakThis = GetWeakThis(This),
                                                                            SenderId,
                                                                            ReceiverId,
                                                                            LocalUserNum](bool bWasReceived) {
                                            if (auto This = PinWeakThis(WeakThis))
                                            {
                                                if (!bWasReceived)
                                                {
                                                    UE_LOG(
                                                        LogEOSFriends,
                                                        Error,
                                                        TEXT("%s"),
                                                        *OnlineRedpointEOS::Errors::UnexpectedError(
                                                             TEXT(
                                                                 "FOnlineFriendsInterfaceSynthetic::OnMessageReceived"),
                                                             TEXT("Unable to notify the sender that we received their "
                                                                  "request."))
                                                             .ToLogString());
                                                    return;
                                                }

                                                UE_LOG(
                                                    LogEOSFriends,
                                                    Verbose,
                                                    TEXT("Successfully processed incoming friend request sent by %s to "
                                                         "%s."),
                                                    *SenderId->ToString(),
                                                    *ReceiverId->ToString());
                                                This->ReadFriendsList(
                                                    LocalUserNum,
                                                    TEXT(""),
                                                    FOnReadFriendsListComplete::CreateLambda(
                                                        [WeakThis = GetWeakThis(This), SenderId, ReceiverId](
                                                            int32 LocalUserNum,
                                                            bool bWasSuccessful,
                                                            const FString &ListName,
                                                            const FString &ErrorStr) {
                                                            if (auto This = PinWeakThis(WeakThis))
                                                            {
                                                                This->TriggerOnFriendsChangeDelegates(LocalUserNum);
                                                                This->TriggerOnInviteReceivedDelegates(
                                                                    *ReceiverId,
                                                                    *SenderId);
                                                            }
                                                        }));
                                            }
                                        }));
                                }
                            }));
                    }
                }
                else if (MessageType == TEXT("FriendRequestReceived"))
                {
                    if (!Db->GetPendingFriendRequests().Contains(*SenderId))
                    {
                        UE_LOG(
                            LogEOSFriends,
                            Error,
                            TEXT("%s"),
                            *OnlineRedpointEOS::Errors::UnexpectedError(
                                 TEXT("FOnlineFriendsInterfaceSynthetic::OnMessageReceived"),
                                 TEXT("Sender notified us they have recieved our friend request, but we don't have "
                                      "a "
                                      "friend request pending. Ignoring the message."))
                                 .ToLogString());
                        return;
                    }

                    switch (Db->GetPendingFriendRequests()[*SenderId].Mode)
                    {
                    case ESerializedPendingFriendRequestMode::OutboundPendingSend:
                    case ESerializedPendingFriendRequestMode::OutboundPendingReceive:
                        // The user has received our request. Move to pending response.
                        Db->GetPendingFriendRequests()[*SenderId].Mode =
                            ESerializedPendingFriendRequestMode::OutboundPendingResponse;
                        Db->DirtyAndFlush(FFriendDatabaseOperationComplete::CreateLambda(
                            [WeakThis = GetWeakThis(This), SenderId, ReceiverId](bool bWasSuccessful) {
                                if (!bWasSuccessful)
                                {
                                    UE_LOG(
                                        LogEOSFriends,
                                        Error,
                                        TEXT("%s"),
                                        *OnlineRedpointEOS::Errors::UnexpectedError(
                                             TEXT("FOnlineFriendsInterfaceSynthetic::OnMessageReceived"),
                                             TEXT("Unable to save the friend database for a user when receiving a "
                                                  "message; the sender will be in an inconsistent state."))
                                             .ToLogString());
                                    return;
                                }

                                UE_LOG(
                                    LogEOSFriends,
                                    Verbose,
                                    TEXT("Successfully received confirmation of outgoing friend request sent by %s "
                                         "to %s."),
                                    *SenderId->ToString(),
                                    *ReceiverId->ToString());
                            }));
                        break;
                    case ESerializedPendingFriendRequestMode::OutboundPendingResponse:
                    case ESerializedPendingFriendRequestMode::InboundPendingResponse:
                    case ESerializedPendingFriendRequestMode::InboundPendingSendAcceptance:
                    default:
                        UE_LOG(
                            LogEOSFriends,
                            Warning,
                            TEXT("Ignoring invalid FriendRequestReceived message for friend that was not in the "
                                 "correct "
                                 "state. Sent by %s to %s."),
                            *SenderId->ToString(),
                            *ReceiverId->ToString());
                        break;
                    }
                }
                else if (MessageType == TEXT("FriendRequestReject"))
                {
                    if (!Db->GetPendingFriendRequests().Contains(*SenderId))
                    {
                        UE_LOG(
                            LogEOSFriends,
                            Error,
                            TEXT("%s"),
                            *OnlineRedpointEOS::Errors::UnexpectedError(
                                 TEXT("FOnlineFriendsInterfaceSynthetic::OnMessageReceived"),
                                 TEXT("Sender notified us they have rejected our friend request, but we don't have "
                                      "a "
                                      "friend request pending. Ignoring the message."))
                                 .ToLogString());
                        return;
                    }

                    switch (Db->GetPendingFriendRequests()[*SenderId].Mode)
                    {
                    case ESerializedPendingFriendRequestMode::OutboundPendingSend:
                    case ESerializedPendingFriendRequestMode::OutboundPendingReceive:
                    case ESerializedPendingFriendRequestMode::OutboundPendingResponse:
                        // The user has rejected our request.
                        Db->GetPendingFriendRequests().Remove(*SenderId);
                        Db->DirtyAndFlush(FFriendDatabaseOperationComplete::CreateLambda(
                            [WeakThis = GetWeakThis(This), SenderId, ReceiverId, LocalUserNum](bool bWasSuccessful) {
                                if (auto This = PinWeakThis(WeakThis))
                                {
                                    if (!bWasSuccessful)
                                    {
                                        UE_LOG(
                                            LogEOSFriends,
                                            Error,
                                            TEXT("%s"),
                                            *OnlineRedpointEOS::Errors::UnexpectedError(
                                                 TEXT("FOnlineFriendsInterfaceSynthetic::OnMessageReceived"),
                                                 TEXT("Unable to save the friend database for a user when receiving a "
                                                      "message; the sender will be in an inconsistent state."))
                                                 .ToLogString());
                                        return;
                                    }

                                    UE_LOG(
                                        LogEOSFriends,
                                        Verbose,
                                        TEXT("Successfully received rejection of outgoing friend request sent by %s "
                                             "to %s."),
                                        *SenderId->ToString(),
                                        *ReceiverId->ToString());
                                    This->ReadFriendsList(
                                        LocalUserNum,
                                        TEXT(""),
                                        FOnReadFriendsListComplete::CreateLambda(
                                            [WeakThis = GetWeakThis(This), SenderId, ReceiverId](
                                                int32 LocalUserNum,
                                                bool bWasSuccessful,
                                                const FString &ListName,
                                                const FString &ErrorStr) {
                                                if (auto This = PinWeakThis(WeakThis))
                                                {
                                                    This->TriggerOnFriendsChangeDelegates(LocalUserNum);
                                                    This->TriggerOnInviteRejectedDelegates(*ReceiverId, *SenderId);
                                                }
                                            }));
                                }
                            }));
                        break;
                    case ESerializedPendingFriendRequestMode::InboundPendingResponse:
                    case ESerializedPendingFriendRequestMode::InboundPendingSendAcceptance:
                    default:
                        UE_LOG(
                            LogEOSFriends,
                            Warning,
                            TEXT("Ignoring invalid FriendRequestAccept message for friend that was not in the "
                                 "correct "
                                 "state. Sent "
                                 "by %s to %s."),
                            *SenderId->ToString(),
                            *ReceiverId->ToString());
                        break;
                    }
                }
                else if (MessageType == TEXT("FriendRequestAccept"))
                {
                    if (!Db->GetPendingFriendRequests().Contains(*SenderId))
                    {
                        UE_LOG(
                            LogEOSFriends,
                            Error,
                            TEXT("%s"),
                            *OnlineRedpointEOS::Errors::UnexpectedError(
                                 TEXT("FOnlineFriendsInterfaceSynthetic::OnMessageReceived"),
                                 TEXT("Sender notified us they have accepted our friend request, but we don't have "
                                      "a "
                                      "friend request pending. Ignoring the message."))
                                 .ToLogString());
                        return;
                    }

                    switch (Db->GetPendingFriendRequests()[*SenderId].Mode)
                    {
                    case ESerializedPendingFriendRequestMode::OutboundPendingSend:
                    case ESerializedPendingFriendRequestMode::OutboundPendingReceive:
                    case ESerializedPendingFriendRequestMode::OutboundPendingResponse:
                        // The user has accepted our request. Move to friend list.
                        {
                            FSerializedFriend NewFriend = {};
                            NewFriend.ProductUserId = SenderId->ToString();
                            Db->GetFriends().Add(*SenderId, NewFriend);
                        }
                        Db->GetPendingFriendRequests().Remove(*SenderId);
                        Db->DirtyAndFlush(FFriendDatabaseOperationComplete::CreateLambda(
                            [WeakThis = GetWeakThis(This), SenderId, ReceiverId, LocalUserNum](bool bWasSuccessful) {
                                if (auto This = PinWeakThis(WeakThis))
                                {
                                    if (!bWasSuccessful)
                                    {
                                        UE_LOG(
                                            LogEOSFriends,
                                            Error,
                                            TEXT("%s"),
                                            *OnlineRedpointEOS::Errors::UnexpectedError(
                                                 TEXT("FOnlineFriendsInterfaceSynthetic::OnMessageReceived"),
                                                 TEXT("Unable to save the friend database for a user when receiving a "
                                                      "message; the sender will be in an inconsistent state."))
                                                 .ToLogString());
                                        return;
                                    }

                                    UE_LOG(
                                        LogEOSFriends,
                                        Verbose,
                                        TEXT("Successfully received acceptance of outgoing friend request sent by %s "
                                             "to %s."),
                                        *SenderId->ToString(),
                                        *ReceiverId->ToString());
                                    This->ReadFriendsList(
                                        LocalUserNum,
                                        TEXT(""),
                                        FOnReadFriendsListComplete::CreateLambda(
                                            [WeakThis = GetWeakThis(This), SenderId, ReceiverId](
                                                int32 LocalUserNum,
                                                bool bWasSuccessful,
                                                const FString &ListName,
                                                const FString &ErrorStr) {
                                                if (auto This = PinWeakThis(WeakThis))
                                                {
                                                    This->TriggerOnFriendsChangeDelegates(LocalUserNum);
                                                    This->TriggerOnInviteAcceptedDelegates(*ReceiverId, *SenderId);
                                                }
                                            }));
                                }
                            }));
                        break;
                    case ESerializedPendingFriendRequestMode::InboundPendingResponse:
                    case ESerializedPendingFriendRequestMode::InboundPendingSendAcceptance:
                    default:
                        UE_LOG(
                            LogEOSFriends,
                            Warning,
                            TEXT("Ignoring invalid FriendRequestAccept message for friend that was not in the "
                                 "correct "
                                 "state. Sent "
                                 "by %s to %s."),
                            *SenderId->ToString(),
                            *ReceiverId->ToString());
                        break;
                    }
                }
                else if (MessageType == TEXT("FriendDelete"))
                {
                    bool bNeedsFlush = false;
                    bool bHadFriend = false;
                    if (Db->GetPendingFriendRequests().Contains(*SenderId))
                    {
                        // Remove any pending friend requests (pending accept, pending response, etc.)
                        bHadFriend = Db->GetPendingFriendRequests()[*SenderId].Mode ==
                                     ESerializedPendingFriendRequestMode::InboundPendingSendAcceptance;
                        Db->GetPendingFriendRequests().Remove(*SenderId);
                        bNeedsFlush = true;
                    }
                    if (Db->GetFriends().Contains(*SenderId))
                    {
                        // Delete the friend.
                        Db->GetFriends().Remove(*SenderId);
                        bNeedsFlush = true;
                        bHadFriend = true;
                    }
                    if (bNeedsFlush)
                    {
                        // Flush the database.
                        Db->DirtyAndFlush(FFriendDatabaseOperationComplete::CreateLambda(
                            [WeakThis = GetWeakThis(This), SenderId, ReceiverId, LocalUserNum, bHadFriend](
                                bool bWasSuccessful) {
                                if (auto This = PinWeakThis(WeakThis))
                                {
                                    if (!bWasSuccessful)
                                    {
                                        UE_LOG(
                                            LogEOSFriends,
                                            Error,
                                            TEXT("%s"),
                                            *OnlineRedpointEOS::Errors::UnexpectedError(
                                                 TEXT("FOnlineFriendsInterfaceSynthetic::OnMessageReceived"),
                                                 TEXT("Unable to save the friend database for a user when receiving a "
                                                      "message; the sender will be in an inconsistent state."))
                                                 .ToLogString());
                                        return;
                                    }

                                    UE_LOG(
                                        LogEOSFriends,
                                        Verbose,
                                        TEXT("Successfully processed incoming friend deletion sent by %s to "
                                             "%s."),
                                        *SenderId->ToString(),
                                        *ReceiverId->ToString());
                                    This->ReadFriendsList(
                                        LocalUserNum,
                                        TEXT(""),
                                        FOnReadFriendsListComplete::CreateLambda(
                                            [WeakThis = GetWeakThis(This), SenderId, ReceiverId, bHadFriend](
                                                int32 LocalUserNum,
                                                bool bWasSuccessful,
                                                const FString &ListName,
                                                const FString &ErrorStr) {
                                                if (auto This = PinWeakThis(WeakThis))
                                                {
                                                    This->TriggerOnFriendsChangeDelegates(LocalUserNum);
                                                    if (bHadFriend)
                                                    {
                                                        This->TriggerOnFriendRemovedDelegates(*ReceiverId, *SenderId);
                                                    }
                                                }
                                            }));
                                }
                            }));
                    }
                    else
                    {
                        UE_LOG(
                            LogEOSFriends,
                            Warning,
                            TEXT("Ignoring invalid FriendDelete message for friend that was not on our friends "
                                 "list. "
                                 "Sent by %s to %s."),
                            *SenderId->ToString(),
                            *ReceiverId->ToString());
                    }
                }
            }
        }));
}

bool FOnlineFriendsInterfaceSynthetic::Tick(float DeltaSeconds)
{
    FDateTime CurrentTime = FDateTime::UtcNow();

    for (const auto &KV : this->FriendState)
    {
        TSharedPtr<const FUniqueNetIdEOS> SenderId =
            StaticCastSharedPtr<const FUniqueNetIdEOS>(this->Identity->GetUniquePlayerId(KV.Key));
        if (!SenderId.IsValid())
        {
            continue;
        }

        auto &Db = KV.Value->FriendDatabase;
        for (const auto &PendingRequest : Db->GetPendingFriendRequests())
        {
            TSharedRef<const FUniqueNetIdEOS> TargetId = StaticCastSharedRef<const FUniqueNetIdEOS>(PendingRequest.Key);

            if (PendingRequest.Value.NextAttempt < CurrentTime)
            {
                // Ensure we don't fire again on the next tick, but don't flush the database (leave it to the send
                // message callback to do the appropriate flush after it knows the result).
                Db->GetPendingFriendRequests()[*TargetId].NextAttempt =
                    FDateTime::UtcNow() + FTimespan::FromMinutes(OnlineRedpointEOS::Friends::RetryMinutes);

                switch (PendingRequest.Value.Mode)
                {
                case ESerializedPendingFriendRequestMode::InboundPendingSendAcceptance: {
                    this->MessagingHub->SendMessage(
                        *SenderId,
                        *TargetId,
                        TEXT("FriendRequestAccept"),
                        TEXT(""),
                        FOnEOSHubMessageSent::CreateLambda(
                            [WeakThis = GetWeakThis(this), Db, SenderId, TargetId](bool bWasReceived) {
                                if (auto This = PinWeakThis(WeakThis))
                                {
                                    if (bWasReceived)
                                    {
                                        Db->GetPendingFriendRequests().Remove(*TargetId);
                                    }
                                    else
                                    {
                                        Db->GetPendingFriendRequests()[*TargetId].NextAttempt =
                                            FDateTime::UtcNow() +
                                            FTimespan::FromMinutes(OnlineRedpointEOS::Friends::RetryMinutes);
                                    }

                                    Db->DirtyAndFlush(FFriendDatabaseOperationComplete());
                                }
                            }));
                    break;
                }
                case ESerializedPendingFriendRequestMode::InboundPendingSendRejection: {
                    this->MessagingHub->SendMessage(
                        *SenderId,
                        *TargetId,
                        TEXT("FriendRequestReject"),
                        TEXT(""),
                        FOnEOSHubMessageSent::CreateLambda(
                            [WeakThis = GetWeakThis(this), Db, SenderId, TargetId](bool bWasReceived) {
                                if (auto This = PinWeakThis(WeakThis))
                                {
                                    if (bWasReceived)
                                    {
                                        Db->GetPendingFriendRequests().Remove(*TargetId);
                                    }
                                    else
                                    {
                                        Db->GetPendingFriendRequests()[*TargetId].NextAttempt =
                                            FDateTime::UtcNow() +
                                            FTimespan::FromMinutes(OnlineRedpointEOS::Friends::RetryMinutes);
                                    }

                                    Db->DirtyAndFlush(FFriendDatabaseOperationComplete());
                                }
                            }));
                    break;
                }
                case ESerializedPendingFriendRequestMode::OutboundPendingSendDeletion: {
                    this->MessagingHub->SendMessage(
                        *SenderId,
                        *TargetId,
                        TEXT("FriendDelete"),
                        TEXT(""),
                        FOnEOSHubMessageSent::CreateLambda(
                            [WeakThis = GetWeakThis(this), Db, SenderId, TargetId](bool bWasReceived) {
                                if (auto This = PinWeakThis(WeakThis))
                                {
                                    if (bWasReceived)
                                    {
                                        Db->GetPendingFriendRequests().Remove(*TargetId);
                                    }
                                    else
                                    {
                                        Db->GetPendingFriendRequests()[*TargetId].NextAttempt =
                                            FDateTime::UtcNow() +
                                            FTimespan::FromMinutes(OnlineRedpointEOS::Friends::RetryMinutes);
                                    }

                                    Db->DirtyAndFlush(FFriendDatabaseOperationComplete());
                                }
                            }));
                    break;
                }
                case ESerializedPendingFriendRequestMode::OutboundPendingSend: {
                    Db->GetPendingFriendRequests()[*TargetId].Mode =
                        ESerializedPendingFriendRequestMode::OutboundPendingReceive;
                    this->MessagingHub->SendMessage(
                        *SenderId,
                        *TargetId,
                        TEXT("FriendRequest"),
                        TEXT(""),
                        FOnEOSHubMessageSent::CreateLambda(
                            [WeakThis = GetWeakThis(this), Db, SenderId, TargetId](bool bWasReceived) {
                                if (auto This = PinWeakThis(WeakThis))
                                {
                                    if (!bWasReceived)
                                    {
                                        Db->GetPendingFriendRequests()[*TargetId].Mode =
                                            ESerializedPendingFriendRequestMode::OutboundPendingSend;
                                        Db->GetPendingFriendRequests()[*TargetId].NextAttempt =
                                            FDateTime::UtcNow() +
                                            FTimespan::FromMinutes(OnlineRedpointEOS::Friends::RetryMinutes);
                                        Db->DirtyAndFlush(FFriendDatabaseOperationComplete());
                                    }
                                    else if (
                                        Db->GetPendingFriendRequests()[*TargetId].Mode ==
                                        ESerializedPendingFriendRequestMode::OutboundPendingReceive)
                                    {
                                        // Set it back to PendingSend in case the other user doesn't send us
                                        // the expected message. We don't flush because we didn't flush for
                                        // OutboundPendingReceive.
                                        Db->GetPendingFriendRequests()[*TargetId].Mode =
                                            ESerializedPendingFriendRequestMode::OutboundPendingSend;
                                        Db->GetPendingFriendRequests()[*TargetId].NextAttempt =
                                            FDateTime::UtcNow() +
                                            FTimespan::FromMinutes(OnlineRedpointEOS::Friends::RetryMinutes);
                                    }
                                }
                            }));
                    break;
                }
                }
            }
        }
    }

    return true;
}

void FOnlineFriendsInterfaceSynthetic::RunReadFriendsListForPropagation(int32 LocalUserNum)
{
    this->ReadFriendsList(
        LocalUserNum,
        TEXT(""),
        FOnReadFriendsListComplete::CreateLambda(
            [WeakThis = GetWeakThis(this),
             LocalUserNum](int32 _, bool bWasSuccessful, const FString &ListName, const FString &ErrorStr) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    bool bScheduleAgain = false;
                    if (This->PendingReadFriendsListFromPropagation.Contains(LocalUserNum))
                    {
                        if (This->PendingReadFriendsListFromPropagation[LocalUserNum])
                        {
                            // Need to schedule again.
                            bScheduleAgain = true;
                            This->PendingReadFriendsListFromPropagation[LocalUserNum] = false;
                        }
                        else
                        {
                            This->PendingReadFriendsListFromPropagation.Remove(LocalUserNum);
                        }
                    }

                    UE_LOG(
                        LogEOSFriends,
                        Verbose,
                        TEXT("FOnlineFriendsInterfaceSynthetic: Firing OnFriendsChange event after a "
                             "ReadFriendsList call caused by a delegated subsystem's OnFriendsChange event."));
                    This->TriggerOnFriendsChangeDelegates(LocalUserNum);

                    if (bScheduleAgain)
                    {
                        UE_LOG(
                            LogEOSFriends,
                            Verbose,
                            TEXT("FOnlineFriendsInterfaceSynthetic: Calling ReadFriendsList again because another "
                                 "delegated subsystem's OnFriendsChange event fired while in the process of reading "
                                 "friends."));
                        This->RunReadFriendsListForPropagation(LocalUserNum);
                    }
                }
            }));
}

FOnlineFriendsInterfaceSynthetic::FOnlineFriendsInterfaceSynthetic(
    FName InInstanceName,
    EOS_HPlatform InPlatform,
    const TSharedRef<FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> &InIdentity,
    const TSharedRef<FOnlineUserCloudInterfaceEOS, ESPMode::ThreadSafe> &InUserCloudImpl,
    const TSharedRef<class FEOSMessagingHub> &InMessagingHub,
    const IOnlineSubsystemPtr &InSubsystemEAS,
    const TSharedRef<class IEOSRuntimePlatform> &InRuntimePlatform,
    const TSharedRef<FEOSConfig> &InConfig,
    const TSharedRef<class FEOSUserFactory, ESPMode::ThreadSafe> &InUserFactory)
    : EOSPlatform(InPlatform)
    , EOSConnect(EOS_Platform_GetConnectInterface(InPlatform))
    , RuntimePlatform(InRuntimePlatform)
    , Identity(InIdentity)
    , UserCloud(InUserCloudImpl)
    , MessagingHub(InMessagingHub)
    , UserFactory(InUserFactory)
    , OnlineFriends()
    , OnFriendsChangeDelegates()
    // It's not safe to initialize in the constructor, because we need to get the avatar interfaces, and that can't be
    // done until UObjects are initialized.
    , bInitialized(false)
    , SubsystemEAS(InSubsystemEAS)
    , Config(InConfig)
    , InstanceName(InInstanceName)
    , TickHandle()
    , MessagingHubHandle()
    , PendingReadFriendsListFromPropagation()
    , FriendState()
{
    check(InPlatform != nullptr);
    check(this->EOSConnect != nullptr);
}

FOnlineFriendsInterfaceSynthetic::~FOnlineFriendsInterfaceSynthetic()
{
    for (auto V : this->OnFriendsChangeDelegates)
    {
        if (auto Friends = V.Get<0>().Pin())
        {
            Friends->ClearOnFriendsChangeDelegate_Handle(V.Get<1>(), V.Get<2>());
        }
    }
    this->OnFriendsChangeDelegates.Empty();
    FUTicker::GetCoreTicker().RemoveTicker(this->TickHandle);
    this->MessagingHub->OnMessageReceived().Remove(this->MessagingHubHandle);
}

void FOnlineFriendsInterfaceSynthetic::RegisterEvents()
{
    for (int32 i = 0; i < MAX_LOCAL_PLAYERS; i++)
    {
        this->Identity->AddOnLogoutCompleteDelegate_Handle(
            i,
            FOnLogoutCompleteDelegate::CreateLambda(
                [WeakThis = GetWeakThis(this)](int32 LocalUserNum, bool bWasSuccessful) {
                    if (auto This = PinWeakThis(WeakThis))
                    {
                        if (This->FriendState.Contains(LocalUserNum))
                        {
                            This->FriendState.Remove(LocalUserNum);
                        }
                    }
                }));
    }

    this->MessagingHubHandle = this->MessagingHub->OnMessageReceived().AddThreadSafeSP(
        this,
        &FOnlineFriendsInterfaceSynthetic::OnMessageReceived);
    this->TickHandle = FUTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateThreadSafeSP(this, &FOnlineFriendsInterfaceSynthetic::Tick));
}

TSharedPtr<const FUniqueNetIdEOS> FOnlineFriendsInterfaceSynthetic::TryResolveNativeIdToEOS(
    const FUniqueNetId &UserId) const
{
    for (const auto &State : this->FriendState)
    {
        // @todo: This could be optimized by generating a mapping when the friends list is populated, but it's only
        // used by presence events at the moment, which aren't expected to be too frequent.
        for (const auto &Friend : State.Value->FriendCacheById)
        {
            if (*Friend.Key == UserId && !Friend.Key->GetType().IsEqual(REDPOINT_EOS_SUBSYSTEM))
            {
                // This is a non-unified friend (because it is keyed on the native ID). Return nullptr.
                return nullptr;
            }

            if (Friend.Key->GetType().IsEqual(REDPOINT_EOS_SUBSYSTEM))
            {
                for (const auto &WrappedFriend : Friend.Value->GetWrappedFriends())
                {
                    if (*WrappedFriend.Value->GetUserId() == UserId)
                    {
                        return StaticCastSharedRef<const FUniqueNetIdEOS>(Friend.Key);
                    }
                }
            }
        }
    }
    return nullptr;
}

bool FOnlineFriendsInterfaceSynthetic::ReadFriendsList(
    int32 LocalUserNum,
    const FString &ListName,
    const FOnReadFriendsListComplete &Delegate)
{
    this->InitIfNeeded();

    if (this->Identity->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
    {
        Delegate.ExecuteIfBound(
            LocalUserNum,
            false,
            ListName,
            OnlineRedpointEOS::Errors::InvalidUser(
                TEXT("FOnlineFriendsInterfaceSynthetic::ReadFriendsList"),
                TEXT("The specified local user isn't signed in."))
                .ToLogString());
        return true;
    }

    if (!this->FriendState.Contains(LocalUserNum))
    {
        this->FriendState.Add(
            LocalUserNum,
            MakeShared<FLocalUserFriendState>(
                StaticCastSharedRef<const FUniqueNetIdEOS>(
                    this->Identity->GetUniquePlayerId(LocalUserNum).ToSharedRef()),
                this->UserCloud));
    }

    this->FriendState[LocalUserNum]->FriendDatabase->WaitUntilLoaded(FFriendDatabaseOperationComplete::CreateLambda(
        [WeakThis = GetWeakThis(this), Delegate, LocalUserNum, ListName](bool bWasSuccessful) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (!bWasSuccessful)
                {
                    Delegate.ExecuteIfBound(
                        LocalUserNum,
                        false,
                        ListName,
                        OnlineRedpointEOS::Errors::ServiceFailure(
                            TEXT("FOnlineFriendsInterfaceSynthetic::ReadFriendsList"),
                            TEXT("Unable to read friend database from Player Data Storage. Ensure the client has "
                                 "access to Player Data Storage under the client policies for this client."))
                            .ToLogString());
                    return;
                }

                FMultiOperation<FWrappedFriendsInterface, bool>::Run(
                    This->OnlineFriends,
                    [WeakThis = GetWeakThis(This),
                     LocalUserNum,
                     ListName](const FWrappedFriendsInterface &InFriends, std::function<void(bool OutValue)> OnDone) {
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            return This->Op_ReadFriendsList(
                                LocalUserNum,
                                This->MutateListName(InFriends, ListName),
                                InFriends.SubsystemName,
                                InFriends.OnlineIdentity,
                                InFriends.OnlineFriends,
                                MoveTemp(OnDone));
                        }
                        return false;
                    },
                    [WeakThis = GetWeakThis(This), LocalUserNum, ListName, Delegate](TArray<bool> OutValues) {
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            This->Dn_ReadFriendsList(LocalUserNum, ListName, Delegate, MoveTemp(OutValues));
                        }
                    });
            }
        }));
    return true;
}

bool FOnlineFriendsInterfaceSynthetic::DeleteFriendsList(
    int32 LocalUserNum,
    const FString &ListName,
    const FOnDeleteFriendsListComplete &Delegate)
{
    this->InitIfNeeded();

    // Not supported.
    return false;
}

bool FOnlineFriendsInterfaceSynthetic::SendInvite(
    int32 LocalUserNum,
    const FUniqueNetId &FriendId,
    const FString &ListName,
    const FOnSendInviteComplete &Delegate)
{
    this->InitIfNeeded();

    if (this->Identity->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
    {
        Delegate.ExecuteIfBound(
            LocalUserNum,
            false,
            FriendId,
            ListName,
            OnlineRedpointEOS::Errors::InvalidUser(
                TEXT("FOnlineFriendsInterfaceSynthetic::SendInvite"),
                TEXT("The specified local user isn't signed in."))
                .ToLogString());
        return true;
    }

    if (!FriendId.GetType().IsEqual(REDPOINT_EOS_SUBSYSTEM))
    {
        Delegate.ExecuteIfBound(
            LocalUserNum,
            false,
            FriendId,
            ListName,
            OnlineRedpointEOS::Errors::InvalidParameters(
                TEXT("FOnlineFriendsInterfaceSynthetic::SendInvite"),
                TEXT("You can only invite EOS users to your friends list."))
                .ToLogString());
        return true;
    }

    if (!this->FriendState.Contains(LocalUserNum))
    {
        Delegate.ExecuteIfBound(
            LocalUserNum,
            false,
            FriendId,
            ListName,
            OnlineRedpointEOS::Errors::InvalidState(
                TEXT("FOnlineFriendsInterfaceSynthetic::SendInvite"),
                TEXT("You must call ReadFriendsList before sending an invite."))
                .ToLogString());
        return true;
    }

    auto UserIdEOS =
        StaticCastSharedRef<const FUniqueNetIdEOS>(this->Identity->GetUniquePlayerId(LocalUserNum).ToSharedRef());
    auto FriendIdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(FriendId.AsShared());

    auto &Db = this->FriendState[LocalUserNum]->FriendDatabase;

    if (Db->GetPendingFriendRequests().Contains(FriendId))
    {
        if (Db->GetPendingFriendRequests()[FriendId].Mode ==
            ESerializedPendingFriendRequestMode::InboundPendingResponse)
        {
            // If we're sending an invite to a user who has already sent an invite to us, that's an accept
            // operation.
            return this->AcceptInvite(
                LocalUserNum,
                FriendId,
                ListName,
                FOnAcceptInviteComplete::CreateLambda([WeakThis = GetWeakThis(this), Delegate](
                                                          int32 LocalUserNum,
                                                          bool bWasSuccessful,
                                                          const FUniqueNetId &FriendId,
                                                          const FString &ListName,
                                                          const FString &ErrorStr) {
                    if (auto This = PinWeakThis(WeakThis))
                    {
                        Delegate.ExecuteIfBound(LocalUserNum, bWasSuccessful, FriendId, ListName, ErrorStr);
                    }
                }));
        }
        else
        {
            // Otherwise we already have an outbound request pending.
            Delegate.ExecuteIfBound(
                LocalUserNum,
                false,
                FriendId,
                ListName,
                OnlineRedpointEOS::Errors::InvalidParameters(
                    TEXT("FOnlineFriendsInterfaceSynthetic::SendInvite"),
                    TEXT("You already have a friend request to this user pending."))
                    .ToLogString());
            return true;
        }
    }

    {
        FSerializedPendingFriendRequest FriendRequest = {};
        FriendRequest.Mode = ESerializedPendingFriendRequestMode::OutboundPendingReceive;
        FriendRequest.ProductUserId = FriendIdEOS->GetProductUserIdString();
        Db->GetPendingFriendRequests().Add(FriendId, FriendRequest);
    }

    this->MessagingHub->SendMessage(
        *UserIdEOS,
        *FriendIdEOS,
        TEXT("FriendRequest"),
        TEXT(""),
        FOnEOSHubMessageSent::CreateLambda(
            [WeakThis = GetWeakThis(this), Db, FriendIdEOS, LocalUserNum, Delegate, ListName](bool bWasReceived) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    if (Db->GetPendingFriendRequests()[*FriendIdEOS].Mode ==
                        ESerializedPendingFriendRequestMode::OutboundPendingResponse)
                    {
                        // By the time our callback happened, the other user already notified us that they
                        // received our friend request. If we're in this state, the message
                        // received handler would have also called DirtyAndFlush on the database.
                        Delegate.ExecuteIfBound(LocalUserNum, true, *FriendIdEOS, ListName, TEXT(""));
                        This->ReadFriendsList(
                            LocalUserNum,
                            ListName,
                            FOnReadFriendsListComplete::CreateLambda([WeakThis = GetWeakThis(This)](
                                                                         int32 LocalUserNum,
                                                                         bool bWasSuccessful,
                                                                         const FString &ListName,
                                                                         const FString &ErrorStr) {
                                if (auto This = PinWeakThis(WeakThis))
                                {
                                    This->TriggerOnFriendsChangeDelegates(LocalUserNum);
                                }
                            }));
                        return;
                    }
                    else if (!bWasReceived)
                    {
                        // We couldn't reach the target user. Move to PendingSend to reschedule another attempt
                        // later.
                        Db->GetPendingFriendRequests()[*FriendIdEOS].Mode =
                            ESerializedPendingFriendRequestMode::OutboundPendingSend;
                        Db->GetPendingFriendRequests()[*FriendIdEOS].NextAttempt =
                            FDateTime::UtcNow() + FTimespan::FromMinutes(OnlineRedpointEOS::Friends::RetryMinutes);
                        Db->DirtyAndFlush(FFriendDatabaseOperationComplete::CreateLambda(
                            [WeakThis = GetWeakThis(This), FriendIdEOS, LocalUserNum, Delegate, ListName](
                                bool bWasSuccessful) {
                                if (auto This = PinWeakThis(WeakThis))
                                {
                                    Delegate.ExecuteIfBound(LocalUserNum, true, *FriendIdEOS, ListName, TEXT(""));
                                    This->ReadFriendsList(
                                        LocalUserNum,
                                        ListName,
                                        FOnReadFriendsListComplete::CreateLambda([WeakThis = GetWeakThis(This)](
                                                                                     int32 LocalUserNum,
                                                                                     bool bWasSuccessful,
                                                                                     const FString &ListName,
                                                                                     const FString &ErrorStr) {
                                            if (auto This = PinWeakThis(WeakThis))
                                            {
                                                This->TriggerOnFriendsChangeDelegates(LocalUserNum);
                                            }
                                        }));
                                }
                            }));
                        return;
                    }
                    else
                    {
                        // They received our message, but they haven't sent the received notification back to us
                        // yet. We set our state temporarily back to PendingSend so that if they don't end up
                        // sending the notification we'll retry.
                        Db->GetPendingFriendRequests()[*FriendIdEOS].Mode =
                            ESerializedPendingFriendRequestMode::OutboundPendingSend;
                        Db->GetPendingFriendRequests()[*FriendIdEOS].NextAttempt =
                            FDateTime::UtcNow() + FTimespan::FromMinutes(OnlineRedpointEOS::Friends::RetryMinutes);
                        Db->DirtyAndFlush(FFriendDatabaseOperationComplete::CreateLambda(
                            [WeakThis = GetWeakThis(This), FriendIdEOS, LocalUserNum, Delegate, ListName](
                                bool bWasSuccessful) {
                                if (auto This = PinWeakThis(WeakThis))
                                {
                                    Delegate.ExecuteIfBound(LocalUserNum, true, *FriendIdEOS, ListName, TEXT(""));
                                    This->ReadFriendsList(
                                        LocalUserNum,
                                        ListName,
                                        FOnReadFriendsListComplete::CreateLambda([WeakThis = GetWeakThis(This)](
                                                                                     int32 LocalUserNum,
                                                                                     bool bWasSuccessful,
                                                                                     const FString &ListName,
                                                                                     const FString &ErrorStr) {
                                            if (auto This = PinWeakThis(WeakThis))
                                            {
                                                This->TriggerOnFriendsChangeDelegates(LocalUserNum);
                                            }
                                        }));
                                }
                            }));
                        return;
                    }
                }
            }));
    return true;
}

bool FOnlineFriendsInterfaceSynthetic::AcceptInvite(
    int32 LocalUserNum,
    const FUniqueNetId &FriendId,
    const FString &ListName,
    const FOnAcceptInviteComplete &Delegate)
{
    this->InitIfNeeded();

    if (this->Identity->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
    {
        Delegate.ExecuteIfBound(
            LocalUserNum,
            false,
            FriendId,
            ListName,
            OnlineRedpointEOS::Errors::InvalidUser(
                TEXT("FOnlineFriendsInterfaceSynthetic::AcceptInvite"),
                TEXT("The specified local user isn't signed in."))
                .ToLogString());
        return true;
    }

    if (!FriendId.GetType().IsEqual(REDPOINT_EOS_SUBSYSTEM))
    {
        Delegate.ExecuteIfBound(
            LocalUserNum,
            false,
            FriendId,
            ListName,
            OnlineRedpointEOS::Errors::InvalidParameters(
                TEXT("FOnlineFriendsInterfaceSynthetic::AcceptInvite"),
                TEXT("You can only accept EOS users to your friends list."))
                .ToLogString());
        return true;
    }

    if (!this->FriendState.Contains(LocalUserNum))
    {
        Delegate.ExecuteIfBound(
            LocalUserNum,
            false,
            FriendId,
            ListName,
            OnlineRedpointEOS::Errors::InvalidState(
                TEXT("FOnlineFriendsInterfaceSynthetic::AcceptInvite"),
                TEXT("You must call ReadFriendsList before accepting an invite."))
                .ToLogString());
        return true;
    }

    auto UserIdEOS =
        StaticCastSharedRef<const FUniqueNetIdEOS>(this->Identity->GetUniquePlayerId(LocalUserNum).ToSharedRef());
    auto FriendIdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(FriendId.AsShared());

    auto &Db = this->FriendState[LocalUserNum]->FriendDatabase;

    if (!Db->GetPendingFriendRequests().Contains(FriendId) ||
        Db->GetPendingFriendRequests()[FriendId].Mode != ESerializedPendingFriendRequestMode::InboundPendingResponse)
    {
        Delegate.ExecuteIfBound(
            LocalUserNum,
            false,
            FriendId,
            ListName,
            OnlineRedpointEOS::Errors::NotFound(
                TEXT("FOnlineFriendsInterfaceSynthetic::AcceptInvite"),
                TEXT("There is no pending invite for that user."))
                .ToLogString());
        return true;
    }

    this->MessagingHub->SendMessage(
        *UserIdEOS,
        *FriendIdEOS,
        TEXT("FriendRequestAccept"),
        TEXT(""),
        FOnEOSHubMessageSent::CreateLambda(
            [WeakThis = GetWeakThis(this), Db, FriendIdEOS, LocalUserNum, Delegate, ListName](bool bWasReceived) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    FSerializedFriend Friend = {};
                    Friend.ProductUserId = FriendIdEOS->ToString();
                    Db->GetFriends().Add(*FriendIdEOS, Friend);

                    if (bWasReceived)
                    {
                        Db->GetPendingFriendRequests().Remove(*FriendIdEOS);
                    }
                    else
                    {
                        Db->GetPendingFriendRequests()[*FriendIdEOS].Mode =
                            ESerializedPendingFriendRequestMode::InboundPendingSendAcceptance;
                        Db->GetPendingFriendRequests()[*FriendIdEOS].NextAttempt =
                            FDateTime::UtcNow() + FTimespan::FromMinutes(OnlineRedpointEOS::Friends::RetryMinutes);
                    }

                    Db->DirtyAndFlush(FFriendDatabaseOperationComplete::CreateLambda(
                        [WeakThis = GetWeakThis(This), FriendIdEOS, LocalUserNum, Delegate, ListName](
                            bool bWasSuccessful) {
                            if (auto This = PinWeakThis(WeakThis))
                            {
                                Delegate.ExecuteIfBound(LocalUserNum, true, *FriendIdEOS, ListName, TEXT(""));

                                This->ReadFriendsList(
                                    LocalUserNum,
                                    ListName,
                                    FOnReadFriendsListComplete::CreateLambda([WeakThis = GetWeakThis(This)](
                                                                                 int32 LocalUserNum,
                                                                                 bool bWasSuccessful,
                                                                                 const FString &ListName,
                                                                                 const FString &ErrorStr) {
                                        if (auto This = PinWeakThis(WeakThis))
                                        {
                                            This->TriggerOnFriendsChangeDelegates(LocalUserNum);
                                        }
                                    }));
                            }
                        }));
                }
            }));
    return true;
}

bool FOnlineFriendsInterfaceSynthetic::RejectInvite(
    int32 LocalUserNum,
    const FUniqueNetId &FriendId,
    const FString &ListName)
{
    this->InitIfNeeded();

    if (this->Identity->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
    {
        this->TriggerOnRejectInviteCompleteDelegates(
            LocalUserNum,
            false,
            FriendId,
            ListName,
            OnlineRedpointEOS::Errors::InvalidUser(
                TEXT("FOnlineFriendsInterfaceSynthetic::RejectInvite"),
                TEXT("The specified local user isn't signed in."))
                .ToLogString());
        return true;
    }

    if (!FriendId.GetType().IsEqual(REDPOINT_EOS_SUBSYSTEM))
    {
        this->TriggerOnRejectInviteCompleteDelegates(
            LocalUserNum,
            false,
            FriendId,
            ListName,
            OnlineRedpointEOS::Errors::InvalidParameters(
                TEXT("FOnlineFriendsInterfaceSynthetic::RejectInvite"),
                TEXT("You can only reject EOS users to your friends list."))
                .ToLogString());
        return true;
    }

    if (!this->FriendState.Contains(LocalUserNum))
    {
        this->TriggerOnRejectInviteCompleteDelegates(
            LocalUserNum,
            false,
            FriendId,
            ListName,
            OnlineRedpointEOS::Errors::InvalidState(
                TEXT("FOnlineFriendsInterfaceSynthetic::RejectInvite"),
                TEXT("You must call ReadFriendsList before accepting an invite."))
                .ToLogString());
        return true;
    }

    auto UserIdEOS =
        StaticCastSharedRef<const FUniqueNetIdEOS>(this->Identity->GetUniquePlayerId(LocalUserNum).ToSharedRef());
    auto FriendIdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(FriendId.AsShared());

    auto &Db = this->FriendState[LocalUserNum]->FriendDatabase;

    if (!Db->GetPendingFriendRequests().Contains(FriendId) ||
        Db->GetPendingFriendRequests()[FriendId].Mode != ESerializedPendingFriendRequestMode::InboundPendingResponse)
    {
        this->TriggerOnRejectInviteCompleteDelegates(
            LocalUserNum,
            false,
            FriendId,
            ListName,
            OnlineRedpointEOS::Errors::NotFound(
                TEXT("FOnlineFriendsInterfaceSynthetic::RejectInvite"),
                TEXT("There is no pending invite for that user."))
                .ToLogString());
        return true;
    }

    this->MessagingHub->SendMessage(
        *UserIdEOS,
        *FriendIdEOS,
        TEXT("FriendRequestReject"),
        TEXT(""),
        FOnEOSHubMessageSent::CreateLambda(
            [WeakThis = GetWeakThis(this), Db, FriendIdEOS, LocalUserNum, ListName](bool bWasReceived) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    if (bWasReceived)
                    {
                        Db->GetPendingFriendRequests().Remove(*FriendIdEOS);
                    }
                    else
                    {
                        Db->GetPendingFriendRequests()[*FriendIdEOS].Mode =
                            ESerializedPendingFriendRequestMode::InboundPendingSendRejection;
                        Db->GetPendingFriendRequests()[*FriendIdEOS].NextAttempt =
                            FDateTime::UtcNow() + FTimespan::FromMinutes(OnlineRedpointEOS::Friends::RetryMinutes);
                    }

                    Db->DirtyAndFlush(FFriendDatabaseOperationComplete::CreateLambda(
                        [WeakThis = GetWeakThis(This), FriendIdEOS, LocalUserNum, ListName](bool bWasSuccessful) {
                            if (auto This = PinWeakThis(WeakThis))
                            {
                                This->TriggerOnRejectInviteCompleteDelegates(
                                    LocalUserNum,
                                    true,
                                    *FriendIdEOS,
                                    ListName,
                                    TEXT(""));

                                This->ReadFriendsList(
                                    LocalUserNum,
                                    ListName,
                                    FOnReadFriendsListComplete::CreateLambda([WeakThis = GetWeakThis(This)](
                                                                                 int32 LocalUserNum,
                                                                                 bool bWasSuccessful,
                                                                                 const FString &ListName,
                                                                                 const FString &ErrorStr) {
                                        if (auto This = PinWeakThis(WeakThis))
                                        {
                                            This->TriggerOnFriendsChangeDelegates(LocalUserNum);
                                        }
                                    }));
                            }
                        }));
                }
            }));
    return true;
}

void FOnlineFriendsInterfaceSynthetic::SetFriendAlias(
    int32 LocalUserNum,
    const FUniqueNetId &FriendId,
    const FString &ListName,
    const FString &Alias,
    const FOnSetFriendAliasComplete &Delegate)
{
    this->InitIfNeeded();

    if (this->Identity->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
    {
        Delegate.ExecuteIfBound(
            LocalUserNum,
            FriendId,
            ListName,
            OnlineRedpointEOS::Errors::InvalidUser(
                TEXT("FOnlineFriendsInterfaceSynthetic::SetFriendAlias"),
                TEXT("The specified local user isn't signed in.")));
        return;
    }

    if (!this->FriendState.Contains(LocalUserNum))
    {
        this->FriendState.Add(
            LocalUserNum,
            MakeShared<FLocalUserFriendState>(
                StaticCastSharedRef<const FUniqueNetIdEOS>(
                    this->Identity->GetUniquePlayerId(LocalUserNum).ToSharedRef()),
                this->UserCloud));
    }

    if (!FriendId.DoesSharedInstanceExist())
    {
        Delegate.ExecuteIfBound(
            LocalUserNum,
            FriendId,
            ListName,
            OnlineRedpointEOS::Errors::InvalidParameters(
                TEXT("FOnlineFriendsInterfaceSynthetic::SetFriendAlias"),
                TEXT("The provided friend ID doesn't have a shared instance. Please report this issue to "
                     "support.")));
        return;
    }

    this->FriendState[LocalUserNum]->FriendDatabase->WaitUntilLoaded(FFriendDatabaseOperationComplete::CreateLambda(
        [WeakThis = GetWeakThis(this), Delegate, LocalUserNum, ListName, Alias, FriendId = FriendId.AsShared()](
            bool bWasSuccessful) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (!bWasSuccessful)
                {
                    Delegate.ExecuteIfBound(
                        LocalUserNum,
                        *FriendId,
                        ListName,
                        OnlineRedpointEOS::Errors::ServiceFailure(
                            TEXT("FOnlineFriendsInterfaceSynthetic::SetFriendAlias"),
                            TEXT("Unable to read friend database from Player Data Storage. Ensure the client has "
                                 "access to Player Data Storage under the client policies for this client.")));
                    return;
                }

                FString Key = FString::Printf(TEXT("%s:%s"), *FriendId->GetType().ToString(), *FriendId->ToString());
                This->FriendState[LocalUserNum]->FriendDatabase->GetFriendAliases().Add(Key, Alias);

                This->FriendState[LocalUserNum]->FriendDatabase->DirtyAndFlush(
                    FFriendDatabaseOperationComplete::CreateLambda(
                        [WeakThis = GetWeakThis(This), Delegate, LocalUserNum, ListName, Alias, FriendId = FriendId](
                            bool bWasSuccessful) {
                            if (auto This = PinWeakThis(WeakThis))
                            {
                                Delegate.ExecuteIfBound(
                                    LocalUserNum,
                                    *FriendId,
                                    ListName,
                                    OnlineRedpointEOS::Errors::Success());
                            }
                        }));
            }
        }));
}

void FOnlineFriendsInterfaceSynthetic::DeleteFriendAlias(
    int32 LocalUserNum,
    const FUniqueNetId &FriendId,
    const FString &ListName,
    const FOnDeleteFriendAliasComplete &Delegate)
{
    this->InitIfNeeded();

    if (this->Identity->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
    {
        Delegate.ExecuteIfBound(
            LocalUserNum,
            FriendId,
            ListName,
            OnlineRedpointEOS::Errors::InvalidUser(
                TEXT("FOnlineFriendsInterfaceSynthetic::SetFriendAlias"),
                TEXT("The specified local user isn't signed in.")));
        return;
    }

    if (!this->FriendState.Contains(LocalUserNum))
    {
        this->FriendState.Add(
            LocalUserNum,
            MakeShared<FLocalUserFriendState>(
                StaticCastSharedRef<const FUniqueNetIdEOS>(
                    this->Identity->GetUniquePlayerId(LocalUserNum).ToSharedRef()),
                this->UserCloud));
    }

    if (!FriendId.DoesSharedInstanceExist())
    {
        Delegate.ExecuteIfBound(
            LocalUserNum,
            FriendId,
            ListName,
            OnlineRedpointEOS::Errors::InvalidParameters(
                TEXT("FOnlineFriendsInterfaceSynthetic::SetFriendAlias"),
                TEXT("The provided friend ID doesn't have a shared instance. Please report this issue to "
                     "support.")));
        return;
    }

    this->FriendState[LocalUserNum]->FriendDatabase->WaitUntilLoaded(FFriendDatabaseOperationComplete::CreateLambda(
        [WeakThis = GetWeakThis(this), Delegate, LocalUserNum, ListName, FriendId = FriendId.AsShared()](
            bool bWasSuccessful) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (!bWasSuccessful)
                {
                    Delegate.ExecuteIfBound(
                        LocalUserNum,
                        *FriendId,
                        ListName,
                        OnlineRedpointEOS::Errors::ServiceFailure(
                            TEXT("FOnlineFriendsInterfaceSynthetic::SetFriendAlias"),
                            TEXT("Unable to read friend database from Player Data Storage. Ensure the client has "
                                 "access to Player Data Storage under the client policies for this client.")));
                    return;
                }

                TSet<FString> IdsToCheck;
                IdsToCheck.Add(FString::Printf(TEXT("%s:%s"), *FriendId->GetType().ToString(), *FriendId->ToString()));
                auto Friend = This->GetFriend(LocalUserNum, *FriendId, ListName);
                if (Friend.IsValid())
                {
                    IdsToCheck.Add(FString::Printf(
                        TEXT("%s:%s"),
                        *Friend->GetUserId()->GetType().ToString(),
                        *Friend->GetUserId()->ToString()));
                    for (const auto &WrappedFriend :
                         StaticCastSharedPtr<FOnlineFriendSynthetic>(Friend)->GetWrappedFriends())
                    {
                        IdsToCheck.Add(FString::Printf(
                            TEXT("%s:%s"),
                            *WrappedFriend.Value->GetUserId()->GetType().ToString(),
                            *WrappedFriend.Value->GetUserId()->ToString()));
                    }
                }
                auto &FriendAliases = This->FriendState[LocalUserNum]->FriendDatabase->GetFriendAliases();
                for (const auto &Id : IdsToCheck)
                {
                    if (FriendAliases.Contains(Id))
                    {
                        FriendAliases.Remove(Id);
                    }
                }

                This->FriendState[LocalUserNum]->FriendDatabase->DirtyAndFlush(
                    FFriendDatabaseOperationComplete::CreateLambda(
                        [WeakThis = GetWeakThis(This), Delegate, LocalUserNum, ListName, FriendId = FriendId](
                            bool bWasSuccessful) {
                            if (auto This = PinWeakThis(WeakThis))
                            {
                                Delegate.ExecuteIfBound(
                                    LocalUserNum,
                                    *FriendId,
                                    ListName,
                                    OnlineRedpointEOS::Errors::Success());
                            }
                        }));
            }
        }));
}

bool FOnlineFriendsInterfaceSynthetic::DeleteFriend(
    int32 LocalUserNum,
    const FUniqueNetId &FriendId,
    const FString &ListName)
{
    this->InitIfNeeded();

    if (this->Identity->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
    {
        this->TriggerOnDeleteFriendCompleteDelegates(
            LocalUserNum,
            false,
            FriendId,
            ListName,
            OnlineRedpointEOS::Errors::InvalidUser(
                TEXT("FOnlineFriendsInterfaceSynthetic::DeleteFriend"),
                TEXT("The specified local user isn't signed in."))
                .ToLogString());
        return true;
    }

    if (!FriendId.GetType().IsEqual(REDPOINT_EOS_SUBSYSTEM))
    {
        this->TriggerOnDeleteFriendCompleteDelegates(
            LocalUserNum,
            false,
            FriendId,
            ListName,
            OnlineRedpointEOS::Errors::InvalidParameters(
                TEXT("FOnlineFriendsInterfaceSynthetic::DeleteFriend"),
                TEXT("You can only delete EOS users on your friends list."))
                .ToLogString());
        return true;
    }

    if (!this->FriendState.Contains(LocalUserNum))
    {
        this->TriggerOnDeleteFriendCompleteDelegates(
            LocalUserNum,
            false,
            FriendId,
            ListName,
            OnlineRedpointEOS::Errors::InvalidState(
                TEXT("FOnlineFriendsInterfaceSynthetic::DeleteFriend"),
                TEXT("You must call ReadFriendsList before deleting a friend."))
                .ToLogString());
        return true;
    }

    auto UserIdEOS =
        StaticCastSharedRef<const FUniqueNetIdEOS>(this->Identity->GetUniquePlayerId(LocalUserNum).ToSharedRef());
    auto FriendIdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(FriendId.AsShared());

    auto &Db = this->FriendState[LocalUserNum]->FriendDatabase;

    if (Db->GetPendingFriendRequests().Contains(FriendId))
    {
        switch (Db->GetPendingFriendRequests()[FriendId].Mode)
        {
        case ESerializedPendingFriendRequestMode::InboundPendingResponse:
            // This is the same as calling RejectInvite.
            {
                this->MessagingHub->SendMessage(
                    *UserIdEOS,
                    *FriendIdEOS,
                    TEXT("FriendRequestReject"),
                    TEXT(""),
                    FOnEOSHubMessageSent::CreateLambda(
                        [WeakThis = GetWeakThis(this), Db, FriendIdEOS, LocalUserNum, ListName](bool bWasReceived) {
                            if (auto This = PinWeakThis(WeakThis))
                            {
                                if (bWasReceived)
                                {
                                    Db->GetPendingFriendRequests().Remove(*FriendIdEOS);
                                }
                                else
                                {
                                    Db->GetPendingFriendRequests()[*FriendIdEOS].Mode =
                                        ESerializedPendingFriendRequestMode::InboundPendingSendRejection;
                                    Db->GetPendingFriendRequests()[*FriendIdEOS].NextAttempt =
                                        FDateTime::UtcNow() +
                                        FTimespan::FromMinutes(OnlineRedpointEOS::Friends::RetryMinutes);
                                }

                                Db->DirtyAndFlush(FFriendDatabaseOperationComplete::CreateLambda(
                                    [WeakThis = GetWeakThis(This), FriendIdEOS, LocalUserNum, ListName](
                                        bool bWasSuccessful) {
                                        if (auto This = PinWeakThis(WeakThis))
                                        {
                                            This->TriggerOnDeleteFriendCompleteDelegates(
                                                LocalUserNum,
                                                true,
                                                *FriendIdEOS,
                                                ListName,
                                                TEXT(""));

                                            This->ReadFriendsList(
                                                LocalUserNum,
                                                ListName,
                                                FOnReadFriendsListComplete::CreateLambda([WeakThis = GetWeakThis(This)](
                                                                                             int32 LocalUserNum,
                                                                                             bool bWasSuccessful,
                                                                                             const FString &ListName,
                                                                                             const FString &ErrorStr) {
                                                    if (auto This = PinWeakThis(WeakThis))
                                                    {
                                                        This->TriggerOnFriendsChangeDelegates(LocalUserNum);
                                                    }
                                                }));
                                        }
                                    }));
                            }
                        }));
                return true;
            }
        case ESerializedPendingFriendRequestMode::InboundPendingSendRejection:
        case ESerializedPendingFriendRequestMode::OutboundPendingSendDeletion:
            // We've already rejected or deleted locally, so they're technically not on our friends list any more.
            {
                this->TriggerOnDeleteFriendCompleteDelegates(
                    LocalUserNum,
                    false,
                    FriendId,
                    ListName,
                    OnlineRedpointEOS::Errors::NotFound(
                        TEXT("FOnlineFriendsInterfaceSynthetic::DeleteFriend"),
                        TEXT("That user isn't on your friend list."))
                        .ToLogString());
                return true;
            }
        case ESerializedPendingFriendRequestMode::OutboundPendingSend:
            // We've got a pending send for this friend invite, since we haven't been able to send it
            // yet. Just delete the local copy so we don't send it in the future.
            {
                Db->GetPendingFriendRequests().Remove(FriendId);
                Db->DirtyAndFlush(FFriendDatabaseOperationComplete::CreateLambda(
                    [WeakThis = GetWeakThis(this), FriendIdEOS, LocalUserNum, ListName](bool bWasSuccessful) {
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            This->TriggerOnDeleteFriendCompleteDelegates(
                                LocalUserNum,
                                true,
                                *FriendIdEOS,
                                ListName,
                                TEXT(""));
                        }
                    }));
                return true;
            }
        // NOLINTNEXTLINE(bugprone-branch-clone)
        case ESerializedPendingFriendRequestMode::OutboundPendingResponse:
            // Need to do a normal delete, to let the other person know we've cancelled
            // the request.
            break;
        case ESerializedPendingFriendRequestMode::OutboundPendingReceive:
            // We might be successful in sending this invite, so we also have to send
            // the deletion as normal. The networking layer enforces reliable ordered, so
            // the normal deletion message will always happen after the send started.
            break;
        case ESerializedPendingFriendRequestMode::InboundPendingSendAcceptance:
            // We've already accepted locally. Fallthrough to deletion logic.
            break;
        }
    }
    else if (!Db->GetFriends().Contains(FriendId))
    {
        this->TriggerOnDeleteFriendCompleteDelegates(
            LocalUserNum,
            false,
            FriendId,
            ListName,
            OnlineRedpointEOS::Errors::NotFound(
                TEXT("FOnlineFriendsInterfaceSynthetic::DeleteFriend"),
                TEXT("That user isn't on your friend list."))
                .ToLogString());
        return true;
    }

    this->MessagingHub->SendMessage(
        *UserIdEOS,
        *FriendIdEOS,
        TEXT("FriendDelete"),
        TEXT(""),
        FOnEOSHubMessageSent::CreateLambda(
            [WeakThis = GetWeakThis(this), Db, FriendIdEOS, LocalUserNum, ListName](bool bWasReceived) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    Db->GetFriends().Remove(*FriendIdEOS);
                    if (bWasReceived)
                    {
                        Db->GetPendingFriendRequests().Remove(*FriendIdEOS);
                    }
                    else if (Db->GetPendingFriendRequests().Contains(*FriendIdEOS))
                    {
                        Db->GetPendingFriendRequests()[*FriendIdEOS].Mode =
                            ESerializedPendingFriendRequestMode::OutboundPendingSendDeletion;
                        Db->GetPendingFriendRequests()[*FriendIdEOS].NextAttempt =
                            FDateTime::UtcNow() + FTimespan::FromMinutes(OnlineRedpointEOS::Friends::RetryMinutes);
                    }
                    else
                    {
                        FSerializedPendingFriendRequest FriendRequest = {};
                        FriendRequest.Mode = ESerializedPendingFriendRequestMode::OutboundPendingSendDeletion;
                        FriendRequest.ProductUserId = FriendIdEOS->GetProductUserIdString();
                        FriendRequest.NextAttempt =
                            FDateTime::UtcNow() + FTimespan::FromMinutes(OnlineRedpointEOS::Friends::RetryMinutes);
                        Db->GetPendingFriendRequests().Add(*FriendIdEOS, FriendRequest);
                    }

                    Db->DirtyAndFlush(FFriendDatabaseOperationComplete::CreateLambda(
                        [WeakThis = GetWeakThis(This), FriendIdEOS, LocalUserNum, ListName](bool bWasSuccessful) {
                            if (auto This = PinWeakThis(WeakThis))
                            {
                                This->TriggerOnDeleteFriendCompleteDelegates(
                                    LocalUserNum,
                                    true,
                                    *FriendIdEOS,
                                    ListName,
                                    TEXT(""));

                                This->ReadFriendsList(
                                    LocalUserNum,
                                    ListName,
                                    FOnReadFriendsListComplete::CreateLambda([WeakThis = GetWeakThis(This)](
                                                                                 int32 LocalUserNum,
                                                                                 bool bWasSuccessful,
                                                                                 const FString &ListName,
                                                                                 const FString &ErrorStr) {
                                        if (auto This = PinWeakThis(WeakThis))
                                        {
                                            This->TriggerOnFriendsChangeDelegates(LocalUserNum);
                                        }
                                    }));
                            }
                        }));
                }
            }));
    return true;
}

bool FOnlineFriendsInterfaceSynthetic::GetFriendsList(
    int32 LocalUserNum,
    const FString &ListName,
    TArray<TSharedRef<FOnlineFriend>> &OutFriends)
{
    this->InitIfNeeded();

    if (!this->FriendState.Contains(LocalUserNum))
    {
        OutFriends = TArray<TSharedRef<FOnlineFriend>>();
        return true;
    }

    OutFriends = this->FriendState[LocalUserNum]->FriendCacheByIndex;
    return true;
}

TSharedPtr<FOnlineFriend> FOnlineFriendsInterfaceSynthetic::GetFriend(
    int32 LocalUserNum,
    const FUniqueNetId &FriendId,
    const FString &ListName)
{
    this->InitIfNeeded();

    if (!this->FriendState.Contains(LocalUserNum))
    {
        return nullptr;
    }

    if (this->FriendState[LocalUserNum]->FriendCacheById.Contains(FriendId))
    {
        return this->FriendState[LocalUserNum]->FriendCacheById[FriendId];
    }

    // Friend not found.
    return nullptr;
}

bool FOnlineFriendsInterfaceSynthetic::IsFriend(
    int32 LocalUserNum,
    const FUniqueNetId &FriendId,
    const FString &ListName)
{
    this->InitIfNeeded();

    if (!this->FriendState.Contains(LocalUserNum))
    {
        return false;
    }

    return this->FriendState[LocalUserNum]->FriendCacheById.Contains(FriendId);
}

void FOnlineFriendsInterfaceSynthetic::AddRecentPlayers(
    const FUniqueNetId &UserId,
    const TArray<FReportPlayedWithUser> &InRecentPlayers,
    const FString &ListName,
    const FOnAddRecentPlayersComplete &InCompletionDelegate)
{
    this->InitIfNeeded();

    int32 LocalUserNum;
    if (this->Identity->GetLoginStatus(UserId) != ELoginStatus::LoggedIn ||
        !this->Identity->GetLocalUserNum(UserId, LocalUserNum))
    {
        InCompletionDelegate.ExecuteIfBound(
            UserId,
            OnlineRedpointEOS::Errors::InvalidUser(
                TEXT("FOnlineFriendsInterfaceSynthetic::AddRecentPlayers"),
                TEXT("The specified local user isn't signed in.")));
        return;
    }

    if (!this->FriendState.Contains(LocalUserNum))
    {
        this->FriendState.Add(
            LocalUserNum,
            MakeShared<FLocalUserFriendState>(
                StaticCastSharedRef<const FUniqueNetIdEOS>(
                    this->Identity->GetUniquePlayerId(LocalUserNum).ToSharedRef()),
                this->UserCloud));
    }

    this->FriendState[LocalUserNum]->FriendDatabase->WaitUntilLoaded(
        FFriendDatabaseOperationComplete::CreateLambda([WeakThis = GetWeakThis(this),
                                                        UserId = UserId.AsShared(),
                                                        LocalUserNum,
                                                        Db = this->FriendState[LocalUserNum]->FriendDatabase,
                                                        InCompletionDelegate,
                                                        InRecentPlayers](bool bWasSuccessful) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (!bWasSuccessful)
                {
                    InCompletionDelegate.ExecuteIfBound(
                        *UserId,
                        OnlineRedpointEOS::Errors::InvalidUser(
                            TEXT("FOnlineFriendsInterfaceSynthetic::AddRecentPlayers"),
                            TEXT("Unable to read friend database from Player Data Storage. Ensure the client has "
                                 "access to Player Data Storage under the client policies for this client.")));
                    return;
                }

                TArray<TSharedRef<const FUniqueNetIdEOS>> UserIdsToFetch;
                TUserIdMap<bool> NewUserIds;

                bool bDidAddRecentPlayers = false;
                for (const auto &InPlayer : InRecentPlayers)
                {
                    if (!InPlayer.UserId->GetType().IsEqual(REDPOINT_EOS_SUBSYSTEM))
                    {
                        continue;
                    }

                    auto InPlayerEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(InPlayer.UserId);

                    if (Db->GetRecentPlayers().Contains(*InPlayerEOS))
                    {
                        Db->GetRecentPlayers()[*InPlayerEOS].LastSeen = FDateTime::UtcNow();
                    }
                    else
                    {
                        FSerializedRecentPlayer RecentPlayer = {};
                        RecentPlayer.ProductUserId = InPlayerEOS->GetProductUserIdString();
                        RecentPlayer.LastSeen = FDateTime::UtcNow();
                        UserIdsToFetch.Add(InPlayerEOS);
                        NewUserIds.Add(*InPlayerEOS, true);
                        Db->GetRecentPlayers().Add(*InPlayerEOS, RecentPlayer);
                    }
                    bDidAddRecentPlayers = true;
                }

                if (!bDidAddRecentPlayers)
                {
                    InCompletionDelegate.ExecuteIfBound(
                        *UserId,
                        OnlineRedpointEOS::Errors::InvalidRequest(
                            TEXT("FOnlineFriendsInterfaceSynthetic::AddRecentPlayers"),
                            TEXT("Either there were no players to add to recent players, or they did not have EOS "
                                 "user "
                                 "IDs.")));
                    return;
                }

                if (!This->FriendState[LocalUserNum]->bDidQueryRecentPlayers)
                {
                    for (const auto &RecentPlayer : This->FriendState[LocalUserNum]->FriendDatabase->GetRecentPlayers())
                    {
                        UserIdsToFetch.Add(StaticCastSharedRef<const FUniqueNetIdEOS>(
                            This->Identity->CreateUniquePlayerId(RecentPlayer.Value.ProductUserId).ToSharedRef()));
                    }
                }

                FOnlineRecentPlayerEOS::Get(
                    This->UserFactory.ToSharedRef(),
                    StaticCastSharedRef<const FUniqueNetIdEOS>(UserId),
                    UserIdsToFetch,
                    [WeakThis = GetWeakThis(This), LocalUserNum, InCompletionDelegate, UserId, NewUserIds](
                        const TUserIdMap<TSharedPtr<FOnlineRecentPlayerEOS>> &ResolvedUsers) {
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            if (This->FriendState.Contains(LocalUserNum))
                            {
                                This->FriendState[LocalUserNum]->FriendDatabase->DirtyAndFlush(
                                    FFriendDatabaseOperationComplete::CreateLambda(
                                        [WeakThis = GetWeakThis(This),
                                         LocalUserNum,
                                         ResolvedUsers,
                                         NewUserIds,
                                         UserId,
                                         InCompletionDelegate](bool bWasSuccessful) {
                                            if (auto This = PinWeakThis(WeakThis))
                                            {
                                                if (!This->FriendState[LocalUserNum]->bDidQueryRecentPlayers)
                                                {
                                                    TArray<TSharedRef<FOnlineRecentPlayer>> RecentPlayers;
                                                    for (const auto &KV : ResolvedUsers)
                                                    {
                                                        if (KV.Value.IsValid())
                                                        {
                                                            RecentPlayers.Add(KV.Value.ToSharedRef());
                                                        }
                                                    }
                                                    This->FriendState[LocalUserNum]->RecentPlayerCache = RecentPlayers;
                                                    This->FriendState[LocalUserNum]->bDidQueryRecentPlayers = true;
                                                }
                                                else
                                                {
                                                    for (const auto &KV : ResolvedUsers)
                                                    {
                                                        if (KV.Value.IsValid())
                                                        {
                                                            This->FriendState[LocalUserNum]->RecentPlayerCache.Add(
                                                                KV.Value.ToSharedRef());
                                                        }
                                                    }
                                                }

                                                for (const auto &RecentPlayer :
                                                     This->FriendState[LocalUserNum]->RecentPlayerCache)
                                                {
                                                    if (This->FriendState[LocalUserNum]
                                                            ->FriendDatabase->GetRecentPlayers()
                                                            .Contains(*RecentPlayer->GetUserId()))
                                                    {
                                                        StaticCastSharedRef<FOnlineRecentPlayerEOS>(RecentPlayer)
                                                            ->LastSeen =
                                                            This->FriendState[LocalUserNum]
                                                                ->FriendDatabase
                                                                ->GetRecentPlayers()[*RecentPlayer->GetUserId()]
                                                                .LastSeen;
                                                    }
                                                }

                                                TArray<TSharedRef<FOnlineRecentPlayer>> PlayersAdded;
                                                for (const auto &KV : ResolvedUsers)
                                                {
                                                    if (NewUserIds.Contains(*KV.Key))
                                                    {
                                                        PlayersAdded.Add(KV.Value.ToSharedRef());
                                                    }
                                                }
                                                if (PlayersAdded.Num() > 0)
                                                {
                                                    This->TriggerOnRecentPlayersAddedDelegates(*UserId, PlayersAdded);
                                                }
                                                InCompletionDelegate.ExecuteIfBound(
                                                    *UserId,
                                                    OnlineRedpointEOS::Errors::Success());
                                                return;
                                            }
                                        }));
                            }
                        }
                    });
            }
        }));
}

bool FOnlineFriendsInterfaceSynthetic::QueryRecentPlayers(const FUniqueNetId &UserId, const FString &Namespace)
{
    this->InitIfNeeded();

    int32 LocalUserNum;
    if (this->Identity->GetLoginStatus(UserId) != ELoginStatus::LoggedIn ||
        !this->Identity->GetLocalUserNum(UserId, LocalUserNum))
    {
        this->TriggerOnQueryRecentPlayersCompleteDelegates(
            UserId,
            Namespace,
            false,
            OnlineRedpointEOS::Errors::InvalidUser(
                TEXT("FOnlineFriendsInterfaceSynthetic::QueryRecentPlayers"),
                TEXT("The specified local user isn't signed in."))
                .ToLogString());
        return true;
    }

    if (!this->FriendState.Contains(LocalUserNum))
    {
        this->FriendState.Add(
            LocalUserNum,
            MakeShared<FLocalUserFriendState>(
                StaticCastSharedRef<const FUniqueNetIdEOS>(
                    this->Identity->GetUniquePlayerId(LocalUserNum).ToSharedRef()),
                this->UserCloud));
    }

    this->FriendState[LocalUserNum]->FriendDatabase->WaitUntilLoaded(FFriendDatabaseOperationComplete::CreateLambda(
        [WeakThis = GetWeakThis(this),
         UserId = UserId.AsShared(),
         Namespace,
         LocalUserNum,
         Db = this->FriendState[LocalUserNum]->FriendDatabase](bool bWasSuccessful) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (!bWasSuccessful)
                {
                    This->TriggerOnQueryRecentPlayersCompleteDelegates(
                        *UserId,
                        Namespace,
                        false,
                        OnlineRedpointEOS::Errors::InvalidUser(
                            TEXT("FOnlineFriendsInterfaceSynthetic::QueryRecentPlayers"),
                            TEXT("Unable to read friend database from Player Data Storage. Ensure the client has "
                                 "access to Player Data Storage under the client policies for this client."))
                            .ToLogString());
                    return;
                }

                TArray<TSharedRef<const FUniqueNetIdEOS>> UserIds;
                TUserIdMap<FDateTime> UserIdToLastSeen;
                for (const auto &RecentPlayer : Db->GetRecentPlayers())
                {
                    auto RecentPlayerId = StaticCastSharedRef<const FUniqueNetIdEOS>(
                        This->Identity->CreateUniquePlayerId(RecentPlayer.Value.ProductUserId).ToSharedRef());
                    UserIds.Add(RecentPlayerId);
                    UserIdToLastSeen.Add(*RecentPlayerId, RecentPlayer.Value.LastSeen);
                }

                FOnlineRecentPlayerEOS::Get(
                    This->UserFactory.ToSharedRef(),
                    StaticCastSharedRef<const FUniqueNetIdEOS>(UserId),
                    UserIds,
                    [WeakThis = GetWeakThis(This), LocalUserNum, UserIdToLastSeen, UserId, Namespace](
                        const TUserIdMap<TSharedPtr<FOnlineRecentPlayerEOS>> &ResolvedUsers) {
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            if (This->FriendState.Contains(LocalUserNum))
                            {
                                TArray<TSharedRef<FOnlineRecentPlayer>> RecentPlayers;
                                for (const auto &KV : ResolvedUsers)
                                {
                                    if (KV.Value.IsValid())
                                    {
                                        if (UserIdToLastSeen.Contains(*KV.Key))
                                        {
                                            KV.Value->LastSeen = UserIdToLastSeen[*KV.Key];
                                        }

                                        RecentPlayers.Add(KV.Value.ToSharedRef());
                                    }
                                }
                                This->FriendState[LocalUserNum]->RecentPlayerCache = RecentPlayers;
                                This->FriendState[LocalUserNum]->bDidQueryRecentPlayers = true;

                                This->TriggerOnQueryRecentPlayersCompleteDelegates(*UserId, Namespace, true, TEXT(""));
                            }
                        }
                    });
            }
        }));
    return true;
}

bool FOnlineFriendsInterfaceSynthetic::GetRecentPlayers(
    const FUniqueNetId &UserId,
    const FString &Namespace,
    TArray<TSharedRef<FOnlineRecentPlayer>> &OutRecentPlayers)
{
    this->InitIfNeeded();

    int32 LocalUserNum;
    if (this->Identity->GetLoginStatus(UserId) != ELoginStatus::LoggedIn ||
        !this->Identity->GetLocalUserNum(UserId, LocalUserNum))
    {
        UE_LOG(
            LogEOSFriends,
            Error,
            TEXT("%s"),
            *OnlineRedpointEOS::Errors::InvalidUser(
                 TEXT("FOnlineFriendsInterfaceSynthetic::QueryRecentPlayers"),
                 TEXT("The specified local user isn't signed in."))
                 .ToLogString());
        return false;
    }

    if (!this->FriendState.Contains(LocalUserNum) || !this->FriendState[LocalUserNum]->bDidQueryRecentPlayers)
    {
        UE_LOG(
            LogEOSFriends,
            Error,
            TEXT("%s"),
            *OnlineRedpointEOS::Errors::InvalidUser(
                 TEXT("FOnlineFriendsInterfaceSynthetic::QueryRecentPlayers"),
                 TEXT("You must call QueryRecentPlayers for this local user before calling GetRecentPlayers."))
                 .ToLogString());
        return false;
    }

    OutRecentPlayers.Empty();
    for (const auto &RecentPlayer : this->FriendState[LocalUserNum]->RecentPlayerCache)
    {
        OutRecentPlayers.Add(RecentPlayer);
    }
    return true;
}

void FOnlineFriendsInterfaceSynthetic::DumpRecentPlayers() const
{
    for (const auto &KV : this->FriendState)
    {
        if (KV.Value->bDidQueryRecentPlayers)
        {
            UE_LOG(
                LogEOSFriends,
                Verbose,
                TEXT("DumpRecentPlayers: %d: %d recent players"),
                KV.Key,
                KV.Value->RecentPlayerCache.Num());
            for (const auto &RecentPlayer : KV.Value->RecentPlayerCache)
            {
                UE_LOG(
                    LogEOSFriends,
                    Verbose,
                    TEXT("DumpRecentPlayers: %d: %s (%s)"),
                    KV.Key,
                    *RecentPlayer->GetUserId()->ToString(),
                    *RecentPlayer->GetDisplayName());
            }
        }
        else
        {
            UE_LOG(
                LogEOSFriends,
                Verbose,
                TEXT("DumpRecentPlayers: %d: have not called QueryRecentPlayers yet"),
                KV.Key);
        }
    }
}

bool FOnlineFriendsInterfaceSynthetic::BlockPlayer(int32 LocalUserNum, const FUniqueNetId &PlayerId)
{
    this->InitIfNeeded();

    if (this->Identity->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
    {
        this->TriggerOnBlockedPlayerCompleteDelegates(
            LocalUserNum,
            false,
            PlayerId,
            TEXT(""),
            OnlineRedpointEOS::Errors::InvalidUser(
                TEXT("FOnlineFriendsInterfaceSynthetic::BlockPlayer"),
                TEXT("The specified local user isn't signed in."))
                .ToLogString());
        return true;
    }

    if (!this->FriendState.Contains(LocalUserNum))
    {
        this->FriendState.Add(
            LocalUserNum,
            MakeShared<FLocalUserFriendState>(
                StaticCastSharedRef<const FUniqueNetIdEOS>(
                    this->Identity->GetUniquePlayerId(LocalUserNum).ToSharedRef()),
                this->UserCloud));
    }

    auto UserIdEOS =
        StaticCastSharedRef<const FUniqueNetIdEOS>(this->Identity->GetUniquePlayerId(LocalUserNum).ToSharedRef());
    auto PlayerIdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(PlayerId.AsShared());

    this->FriendState[LocalUserNum]->FriendDatabase->WaitUntilLoaded(FFriendDatabaseOperationComplete::CreateLambda(
        [WeakThis = GetWeakThis(this), LocalUserNum, UserIdEOS, PlayerIdEOS](bool bWasSuccessful) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (This->FriendState.Contains(LocalUserNum))
                {
                    if (!bWasSuccessful)
                    {
                        This->TriggerOnBlockedPlayerCompleteDelegates(
                            LocalUserNum,
                            false,
                            *PlayerIdEOS,
                            TEXT(""),
                            OnlineRedpointEOS::Errors::InvalidUser(
                                TEXT("FOnlineFriendsInterfaceSynthetic::BlockPlayer"),
                                TEXT("Unable to read friend database from Player Data Storage. Ensure the client "
                                     "has "
                                     "access to Player Data Storage under the client policies for this client."))
                                .ToLogString());
                        return;
                    }

                    TArray<TSharedRef<const FUniqueNetIdEOS>> UserIds;
                    UserIds.Add(PlayerIdEOS);
                    if (!This->FriendState[LocalUserNum]->bDidQueryBlockedPlayers)
                    {
                        for (const auto &BlockedUser :
                             This->FriendState[LocalUserNum]->FriendDatabase->GetBlockedUsers())
                        {
                            UserIds.Add(StaticCastSharedRef<const FUniqueNetIdEOS>(
                                This->Identity->CreateUniquePlayerId(BlockedUser.Value.ProductUserId).ToSharedRef()));
                        }
                    }
                    FOnlineBlockedPlayerEOS::Get(
                        This->UserFactory.ToSharedRef(),
                        UserIdEOS,
                        UserIds,
                        [WeakThis = GetWeakThis(This), LocalUserNum, UserIdEOS, PlayerIdEOS](
                            const TUserIdMap<TSharedPtr<FOnlineBlockedPlayerEOS>> &ResolvedUsers) {
                            if (auto This = PinWeakThis(WeakThis))
                            {
                                if (This->FriendState.Contains(LocalUserNum))
                                {
                                    for (const auto &Blocked :
                                         This->FriendState[LocalUserNum]->FriendDatabase->GetBlockedUsers())
                                    {
                                        if (Blocked.Value.ProductUserId == PlayerIdEOS->ToString())
                                        {
                                            This->TriggerOnBlockedPlayerCompleteDelegates(
                                                LocalUserNum,
                                                false,
                                                *PlayerIdEOS,
                                                TEXT(""),
                                                OnlineRedpointEOS::Errors::InvalidRequest(
                                                    TEXT("FOnlineFriendsInterfaceSynthetic::BlockPlayer"),
                                                    TEXT("The specified user to block is already blocked."))
                                                    .ToLogString());
                                            return;
                                        }
                                    }

                                    auto BlockedUser = ResolvedUsers[*PlayerIdEOS].ToSharedRef();

                                    if (!This->FriendState[LocalUserNum]->bDidQueryBlockedPlayers)
                                    {
                                        TArray<TSharedRef<FOnlineBlockedPlayer>> BlockedPlayers;
                                        for (const auto &KV : ResolvedUsers)
                                        {
                                            if (KV.Value.IsValid() && *KV.Key != *PlayerIdEOS)
                                            {
                                                BlockedPlayers.Add(KV.Value.ToSharedRef());
                                            }
                                        }
                                        This->FriendState[LocalUserNum]->BlockedPlayersCache = BlockedPlayers;
                                        This->FriendState[LocalUserNum]->bDidQueryBlockedPlayers = true;
                                    }

                                    FSerializedBlockedUser SerializedBlockedUser = {};
                                    SerializedBlockedUser.ProductUserId = PlayerIdEOS->ToString();
                                    This->FriendState[LocalUserNum]->FriendDatabase->GetBlockedUsers().Add(
                                        *PlayerIdEOS,
                                        SerializedBlockedUser);
                                    This->FriendState[LocalUserNum]->FriendDatabase->DirtyAndFlush(
                                        FFriendDatabaseOperationComplete::CreateLambda(
                                            [WeakThis = GetWeakThis(This),
                                             LocalUserNum,
                                             UserIdEOS,
                                             PlayerIdEOS,
                                             BlockedUser](bool bWasSuccessful) {
                                                if (auto This = PinWeakThis(WeakThis))
                                                {
                                                    auto &Cache = This->FriendState[LocalUserNum]->BlockedPlayersCache;
                                                    for (auto i = Cache.Num() - 1; i >= 0; i--)
                                                    {
                                                        if (*Cache[i]->GetUserId() == *PlayerIdEOS)
                                                        {
                                                            Cache.RemoveAt(i);
                                                        }
                                                    };
                                                    Cache.Add(BlockedUser);

                                                    This->TriggerOnBlockListChangeDelegates(LocalUserNum, TEXT(""));
                                                    This->TriggerOnBlockedPlayerCompleteDelegates(
                                                        LocalUserNum,
                                                        true,
                                                        *PlayerIdEOS,
                                                        TEXT(""),
                                                        TEXT(""));
                                                }
                                            }));
                                }
                            }
                        });
                }
            }
        }));
    return true;
}

bool FOnlineFriendsInterfaceSynthetic::UnblockPlayer(int32 LocalUserNum, const FUniqueNetId &PlayerId)
{
    this->InitIfNeeded();

    if (this->Identity->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
    {
        this->TriggerOnUnblockedPlayerCompleteDelegates(
            LocalUserNum,
            false,
            PlayerId,
            TEXT(""),
            OnlineRedpointEOS::Errors::InvalidUser(
                TEXT("FOnlineFriendsInterfaceSynthetic::UnblockPlayer"),
                TEXT("The specified local user isn't signed in."))
                .ToLogString());
        return true;
    }

    if (!this->FriendState.Contains(LocalUserNum))
    {
        this->FriendState.Add(
            LocalUserNum,
            MakeShared<FLocalUserFriendState>(
                StaticCastSharedRef<const FUniqueNetIdEOS>(
                    this->Identity->GetUniquePlayerId(LocalUserNum).ToSharedRef()),
                this->UserCloud));
    }

    auto UserIdEOS =
        StaticCastSharedRef<const FUniqueNetIdEOS>(this->Identity->GetUniquePlayerId(LocalUserNum).ToSharedRef());
    auto PlayerIdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(PlayerId.AsShared());

    this->FriendState[LocalUserNum]->FriendDatabase->WaitUntilLoaded(FFriendDatabaseOperationComplete::CreateLambda(
        [WeakThis = GetWeakThis(this), LocalUserNum, UserIdEOS, PlayerIdEOS](bool bWasSuccessful) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (This->FriendState.Contains(LocalUserNum))
                {
                    if (!bWasSuccessful)
                    {
                        This->TriggerOnUnblockedPlayerCompleteDelegates(
                            LocalUserNum,
                            false,
                            *PlayerIdEOS,
                            TEXT(""),
                            OnlineRedpointEOS::Errors::InvalidUser(
                                TEXT("FOnlineFriendsInterfaceSynthetic::UnblockPlayer"),
                                TEXT("Unable to read friend database from Player Data Storage. Ensure the client "
                                     "has "
                                     "access to Player Data Storage under the client policies for this client."))
                                .ToLogString());
                        return;
                    }

                    TArray<TSharedRef<const FUniqueNetIdEOS>> UserIds;
                    UserIds.Add(PlayerIdEOS);
                    if (!This->FriendState[LocalUserNum]->bDidQueryBlockedPlayers)
                    {
                        for (const auto &BlockedUser :
                             This->FriendState[LocalUserNum]->FriendDatabase->GetBlockedUsers())
                        {
                            UserIds.Add(StaticCastSharedRef<const FUniqueNetIdEOS>(
                                This->Identity->CreateUniquePlayerId(BlockedUser.Value.ProductUserId).ToSharedRef()));
                        }
                    }
                    FOnlineBlockedPlayerEOS::Get(
                        This->UserFactory.ToSharedRef(),
                        UserIdEOS,
                        UserIds,
                        [WeakThis = GetWeakThis(This), LocalUserNum, UserIdEOS, PlayerIdEOS](
                            const TUserIdMap<TSharedPtr<FOnlineBlockedPlayerEOS>> &ResolvedUsers) {
                            if (auto This = PinWeakThis(WeakThis))
                            {
                                if (This->FriendState.Contains(LocalUserNum))
                                {
                                    bool bIsBlocked = false;
                                    for (const auto &Blocked :
                                         This->FriendState[LocalUserNum]->FriendDatabase->GetBlockedUsers())
                                    {
                                        if (Blocked.Value.ProductUserId == PlayerIdEOS->ToString())
                                        {
                                            bIsBlocked = true;
                                            break;
                                        }
                                    }
                                    if (!bIsBlocked)
                                    {
                                        This->TriggerOnUnblockedPlayerCompleteDelegates(
                                            LocalUserNum,
                                            false,
                                            *PlayerIdEOS,
                                            TEXT(""),
                                            OnlineRedpointEOS::Errors::InvalidRequest(
                                                TEXT("FOnlineFriendsInterfaceSynthetic::UnblockPlayer"),
                                                TEXT("The specified user to unblock is not blocked."))
                                                .ToLogString());
                                        return;
                                    }

                                    auto UnblockedUser = ResolvedUsers[*PlayerIdEOS].ToSharedRef();

                                    if (!This->FriendState[LocalUserNum]->bDidQueryBlockedPlayers)
                                    {
                                        TArray<TSharedRef<FOnlineBlockedPlayer>> BlockedPlayers;
                                        for (const auto &KV : ResolvedUsers)
                                        {
                                            if (KV.Value.IsValid() && *KV.Key != *PlayerIdEOS)
                                            {
                                                BlockedPlayers.Add(KV.Value.ToSharedRef());
                                            }
                                        }
                                        This->FriendState[LocalUserNum]->BlockedPlayersCache = BlockedPlayers;
                                        This->FriendState[LocalUserNum]->bDidQueryBlockedPlayers = true;
                                    }

                                    This->FriendState[LocalUserNum]->FriendDatabase->GetBlockedUsers().Remove(
                                        *PlayerIdEOS);
                                    This->FriendState[LocalUserNum]->FriendDatabase->DirtyAndFlush(
                                        FFriendDatabaseOperationComplete::CreateLambda(
                                            [WeakThis = GetWeakThis(This), LocalUserNum, UserIdEOS, PlayerIdEOS](
                                                bool bWasSuccessful) {
                                                if (auto This = PinWeakThis(WeakThis))
                                                {
                                                    auto &Cache = This->FriendState[LocalUserNum]->BlockedPlayersCache;
                                                    for (auto i = Cache.Num() - 1; i >= 0; i--)
                                                    {
                                                        if (*Cache[i]->GetUserId() == *PlayerIdEOS)
                                                        {
                                                            Cache.RemoveAt(i);
                                                        }
                                                    };

                                                    This->TriggerOnBlockListChangeDelegates(LocalUserNum, TEXT(""));
                                                    This->TriggerOnUnblockedPlayerCompleteDelegates(
                                                        LocalUserNum,
                                                        true,
                                                        *PlayerIdEOS,
                                                        TEXT(""),
                                                        TEXT(""));
                                                }
                                            }));
                                }
                            }
                        });
                }
            }
        }));
    return true;
}

bool FOnlineFriendsInterfaceSynthetic::QueryBlockedPlayers(const FUniqueNetId &UserId)
{
    this->InitIfNeeded();

    int32 LocalUserNum;
    if (this->Identity->GetLoginStatus(UserId) != ELoginStatus::LoggedIn ||
        !this->Identity->GetLocalUserNum(UserId, LocalUserNum))
    {
        this->TriggerOnQueryBlockedPlayersCompleteDelegates(
            UserId,
            false,
            OnlineRedpointEOS::Errors::InvalidUser(
                TEXT("FOnlineFriendsInterfaceSynthetic::QueryBlockedPlayers"),
                TEXT("The specified local user isn't signed in."))
                .ToLogString());
        return true;
    }

    if (!this->FriendState.Contains(LocalUserNum))
    {
        this->FriendState.Add(
            LocalUserNum,
            MakeShared<FLocalUserFriendState>(
                StaticCastSharedRef<const FUniqueNetIdEOS>(
                    this->Identity->GetUniquePlayerId(LocalUserNum).ToSharedRef()),
                this->UserCloud));
    }

    this->FriendState[LocalUserNum]->FriendDatabase->WaitUntilLoaded(FFriendDatabaseOperationComplete::CreateLambda(
        [WeakThis = GetWeakThis(this),
         UserId = UserId.AsShared(),
         LocalUserNum,
         Db = this->FriendState[LocalUserNum]->FriendDatabase](bool bWasSuccessful) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (!bWasSuccessful)
                {
                    This->TriggerOnQueryBlockedPlayersCompleteDelegates(
                        *UserId,
                        false,
                        OnlineRedpointEOS::Errors::InvalidUser(
                            TEXT("FOnlineFriendsInterfaceSynthetic::QueryBlockedPlayers"),
                            TEXT("Unable to read friend database from Player Data Storage. Ensure the client has "
                                 "access to Player Data Storage under the client policies for this client."))
                            .ToLogString());
                    return;
                }

                TArray<TSharedRef<const FUniqueNetIdEOS>> UserIds;
                for (const auto &BlockedUser : Db->GetBlockedUsers())
                {
                    UserIds.Add(StaticCastSharedRef<const FUniqueNetIdEOS>(
                        This->Identity->CreateUniquePlayerId(BlockedUser.Value.ProductUserId).ToSharedRef()));
                }

                FOnlineBlockedPlayerEOS::Get(
                    This->UserFactory.ToSharedRef(),
                    StaticCastSharedRef<const FUniqueNetIdEOS>(UserId),
                    UserIds,
                    [WeakThis = GetWeakThis(This), LocalUserNum, UserId](
                        const TUserIdMap<TSharedPtr<FOnlineBlockedPlayerEOS>> &ResolvedUsers) {
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            if (This->FriendState.Contains(LocalUserNum))
                            {
                                TArray<TSharedRef<FOnlineBlockedPlayer>> BlockedPlayers;
                                for (const auto &KV : ResolvedUsers)
                                {
                                    if (KV.Value.IsValid())
                                    {
                                        BlockedPlayers.Add(KV.Value.ToSharedRef());
                                    }
                                }
                                This->FriendState[LocalUserNum]->BlockedPlayersCache = BlockedPlayers;
                                This->FriendState[LocalUserNum]->bDidQueryBlockedPlayers = true;

                                This->TriggerOnQueryBlockedPlayersCompleteDelegates(*UserId, true, TEXT(""));
                            }
                        }
                    });
            }
        }));
    return true;
}

bool FOnlineFriendsInterfaceSynthetic::GetBlockedPlayers(
    const FUniqueNetId &UserId,
    TArray<TSharedRef<FOnlineBlockedPlayer>> &OutBlockedPlayers)
{
    this->InitIfNeeded();

    int32 LocalUserNum;
    if (this->Identity->GetLoginStatus(UserId) != ELoginStatus::LoggedIn ||
        !this->Identity->GetLocalUserNum(UserId, LocalUserNum))
    {
        UE_LOG(
            LogEOSFriends,
            Error,
            TEXT("%s"),
            *OnlineRedpointEOS::Errors::InvalidUser(
                 TEXT("FOnlineFriendsInterfaceSynthetic::GetBlockedPlayers"),
                 TEXT("The specified local user isn't signed in."))
                 .ToLogString());
        return false;
    }

    if (!this->FriendState.Contains(LocalUserNum) || !this->FriendState[LocalUserNum]->bDidQueryBlockedPlayers)
    {
        UE_LOG(
            LogEOSFriends,
            Error,
            TEXT("%s"),
            *OnlineRedpointEOS::Errors::InvalidUser(
                 TEXT("FOnlineFriendsInterfaceSynthetic::GetBlockedPlayers"),
                 TEXT("You must call QueryBlockedPlayers for this local user before calling GetBlockedPlayers."))
                 .ToLogString());
        return false;
    }

    OutBlockedPlayers.Empty();
    for (const auto &BlockedPlayer : this->FriendState[LocalUserNum]->BlockedPlayersCache)
    {
        OutBlockedPlayers.Add(BlockedPlayer);
    }
    return true;
}

void FOnlineFriendsInterfaceSynthetic::DumpBlockedPlayers() const
{
    for (const auto &KV : this->FriendState)
    {
        if (KV.Value->bDidQueryRecentPlayers)
        {
            UE_LOG(
                LogEOSFriends,
                Verbose,
                TEXT("DumpBlockedPlayers: %d: %d blocked players"),
                KV.Key,
                KV.Value->BlockedPlayersCache.Num());
            for (const auto &RecentPlayer : KV.Value->BlockedPlayersCache)
            {
                UE_LOG(
                    LogEOSFriends,
                    Verbose,
                    TEXT("DumpBlockedPlayers: %d: %s (%s)"),
                    KV.Key,
                    *RecentPlayer->GetUserId()->ToString(),
                    *RecentPlayer->GetDisplayName());
            }
        }
        else
        {
            UE_LOG(
                LogEOSFriends,
                Verbose,
                TEXT("DumpBlockedPlayers: %d: have not called QueryBlockedPlayers yet"),
                KV.Key);
        }
    }
}

void FOnlineFriendsInterfaceSynthetic::QueryFriendSettings(
    const FUniqueNetId &LocalUserId,
    FOnSettingsOperationComplete Delegate)
{
    this->InitIfNeeded();

    FFriendSettings Settings;
    Delegate.ExecuteIfBound(
        LocalUserId,
        false,
        false,
        Settings,
        OnlineRedpointEOS::Errors::NotImplemented(
            TEXT("FOnlineFriendsInterfaceSynthetic::QueryFriendSettings"),
            TEXT("QueryFriendSettings is not implemented."))
            .ToLogString());
}

#if !defined(UE_5_0_OR_LATER)
void FOnlineFriendsInterfaceSynthetic::UpdateFriendSettings(
    const FUniqueNetId &LocalUserId,
    const FFriendSettings &NewSettings,
    FOnSettingsOperationComplete Delegate)
{
    this->InitIfNeeded();

    FFriendSettings Settings;
    Delegate.ExecuteIfBound(
        LocalUserId,
        false,
        false,
        Settings,
        OnlineRedpointEOS::Errors::NotImplemented(
            TEXT("FOnlineFriendsInterfaceSynthetic::UpdateFriendSettings"),
            TEXT("QueryFriendSettings is not implemented."))
            .ToLogString());
}
#endif // #if !defined(UE_5_0_OR_LATER)

bool FOnlineFriendsInterfaceSynthetic::QueryFriendSettings(
    const FUniqueNetId &UserId,
    const FString &Source,
    const FOnQueryFriendSettingsComplete &Delegate)
{
    this->InitIfNeeded();

    FFriendSettings Settings;
    Delegate.ExecuteIfBound(
        UserId,
        false,
        OnlineRedpointEOS::Errors::NotImplemented(
            TEXT("FOnlineFriendsInterfaceSynthetic::UpdateFriendSettings"),
            TEXT("QueryFriendSettings is not implemented."))
            .ToLogString());
    return true;
}

bool FOnlineFriendsInterfaceSynthetic::GetFriendSettings(
    const FUniqueNetId &UserId,
    TMap<FString, TSharedRef<FOnlineFriendSettingsSourceData>> &OutSettings)
{
    this->InitIfNeeded();

    UE_LOG(
        LogEOSFriends,
        Error,
        TEXT("%s"),
        *OnlineRedpointEOS::Errors::NotImplemented(
             TEXT("FOnlineFriendsInterfaceSynthetic::GetFriendSettings"),
             TEXT("GetFriendSettings is not implemented."))
             .ToLogString());
    return false;
}

bool FOnlineFriendsInterfaceSynthetic::SetFriendSettings(
    const FUniqueNetId &UserId,
    const FString &Source,
    bool bNeverShowAgain,
    const FOnSetFriendSettingsComplete &Delegate)
{
    this->InitIfNeeded();

    FFriendSettings Settings;
    Delegate.ExecuteIfBound(
        UserId,
        false,
        OnlineRedpointEOS::Errors::NotImplemented(
            TEXT("FOnlineFriendsInterfaceSynthetic::SetFriendSettings"),
            TEXT("SetFriendSettings is not implemented."))
            .ToLogString());
    return true;
}

EOS_DISABLE_STRICT_WARNINGS
