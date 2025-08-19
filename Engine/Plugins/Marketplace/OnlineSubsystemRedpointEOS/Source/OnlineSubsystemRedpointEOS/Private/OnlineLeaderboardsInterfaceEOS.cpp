// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/OnlineLeaderboardsInterfaceEOS.h"

#if EOS_HAS_AUTHENTICATION
#include "Interfaces/OnlineFriendsInterface.h"
#endif // #if EOS_HAS_AUTHENTICATION
#include "Interfaces/OnlineStatsInterface.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineUserEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

FOnlineLeaderboardsInterfaceEOS::FOnlineLeaderboardsInterfaceEOS(
    EOS_HPlatform InPlatform,
    IOnlineStatsPtr InStats,
    IOnlineIdentityPtr InIdentity
#if EOS_HAS_AUTHENTICATION
    ,
    IOnlineFriendsPtr InFriends
#endif // #if EOS_HAS_AUTHENTICATION
    )
    : EOSLeaderboards(EOS_Platform_GetLeaderboardsInterface(InPlatform))
    , EOSConnect(EOS_Platform_GetConnectInterface(InPlatform))
    , Stats(MoveTemp(InStats))
    , Identity(MoveTemp(InIdentity))
#if EOS_HAS_AUTHENTICATION
    , Friends(MoveTemp(InFriends))
#endif // #if EOS_HAS_AUTHENTICATION
    , bIsExecutingQueryLeaderboardRanks(false)
{
    check(this->EOSLeaderboards != nullptr);
    check(this->EOSConnect != nullptr);
    check(this->Stats != nullptr);
    check(this->Identity != nullptr);
};

bool FOnlineLeaderboardsInterfaceEOS::ReadLeaderboards(
    const TArray<TSharedRef<const FUniqueNetId>> &Players,
    FOnlineLeaderboardReadRef &ReadObject)
{
    auto AuthedUsers = this->Identity->GetAllUserAccounts();
    if (AuthedUsers.Num() == 0)
    {
        // No locally authenticated user.
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Resolving display names of users in a leaderboards list requires a locally authenticated user."));
        ReadObject->ReadState = EOnlineAsyncTaskState::Failed;
        this->TriggerOnLeaderboardReadCompleteDelegates(false);
        return true;
    }

    TSharedRef<const FUniqueNetIdEOS> IdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(AuthedUsers[0]->GetUserId());

    return this->ReadLeaderboardsWithLocalUserContext(IdEOS, Players, ReadObject);
}

bool FOnlineLeaderboardsInterfaceEOS::ReadLeaderboardsWithLocalUserContext(
    const TSharedRef<const FUniqueNetIdEOS> &QueryingUserId,
    const TArray<TSharedRef<const FUniqueNetId>> &Players,
    FOnlineLeaderboardReadRef &ReadObject)
{
    if (ReadObject->ColumnMetadata.Num() == 0)
    {
        // No columns.
        ReadObject->ReadState = EOnlineAsyncTaskState::Done;
        this->TriggerOnLeaderboardReadCompleteDelegates(true);
        return true;
    }

    TArray<EOS_ProductUserId> ProductUserIds;
    for (const auto &Id : Players)
    {
        if (Id->GetType() == REDPOINT_EOS_SUBSYSTEM)
        {
            TSharedRef<const FUniqueNetIdEOS> IdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(Id);
            ProductUserIds.Add(IdEOS->GetProductUserId());
        }
        else
        {
            UE_LOG(
                LogEOS,
                Warning,
                TEXT("User ID passed to ReadLeaderboards is not a valid EOS user ID '%s'; it will be ignored."),
                *Id->ToString());
        }
    }

    if (ProductUserIds.Num() == 0)
    {
        ReadObject->ReadState = EOnlineAsyncTaskState::Done;
        this->TriggerOnLeaderboardReadCompleteDelegates(true);
        return true;
    }

    EOS_Leaderboards_UserScoresQueryStatInfo *StatInfo = (EOS_Leaderboards_UserScoresQueryStatInfo *)FMemory::Malloc(
        sizeof(EOS_Leaderboards_UserScoresQueryStatInfo) * ReadObject->ColumnMetadata.Num());
    uint32_t StatInfoCount = ReadObject->ColumnMetadata.Num();
    for (uint32_t i = 0; i < StatInfoCount; i++)
    {
        auto Column = ReadObject->ColumnMetadata[i];
        StatInfo[i].StatName = nullptr;
        verify(
            EOSString_Stats_StatName::AllocateToCharBuffer(Column.ColumnName.ToString(), StatInfo[i].StatName) ==
            EOS_EResult::EOS_Success);
        StatInfo[i].Aggregation = EOS_ELeaderboardAggregation::EOS_LA_Latest;
    }

    ReadObject->ReadState = EOnlineAsyncTaskState::InProgress;

    EOS_Leaderboards_QueryLeaderboardUserScoresOptions QueryOpts = {};
    QueryOpts.ApiVersion = EOS_LEADERBOARDS_QUERYLEADERBOARDUSERSCORES_API_LATEST;
    QueryOpts.StartTime = EOS_STATS_TIME_UNDEFINED;
    QueryOpts.EndTime = EOS_STATS_TIME_UNDEFINED;
    QueryOpts.UserIds = nullptr;
    QueryOpts.UserIdsCount = 0;
    EOSString_ProductUserId::AllocateToIdList(
        ProductUserIds,
        QueryOpts.UserIdsCount,
        (EOS_ProductUserId *&)QueryOpts.UserIds);
    QueryOpts.StatInfo = StatInfo;
    QueryOpts.StatInfoCount = StatInfoCount;
#if EOS_VERSION_AT_LEAST(1, 10, 2)
    if (QueryingUserId->IsDedicatedServer())
    {
        QueryOpts.LocalUserId = nullptr;
    }
    else
    {
        QueryOpts.LocalUserId = QueryingUserId->GetProductUserId();
    }
#endif
    EOSRunOperation<
        EOS_HLeaderboards,
        EOS_Leaderboards_QueryLeaderboardUserScoresOptions,
        EOS_Leaderboards_OnQueryLeaderboardUserScoresCompleteCallbackInfo>(
        this->EOSLeaderboards,
        &QueryOpts,
        EOS_Leaderboards_QueryLeaderboardUserScores,
        [WeakThis = GetWeakThis(this), StatInfo, StatInfoCount, QueryOpts, ReadObject, ProductUserIds, QueryingUserId](
            const EOS_Leaderboards_OnQueryLeaderboardUserScoresCompleteCallbackInfo *Data) {
            EOSString_ProductUserId::FreeFromIdListConst(
                QueryOpts.UserIdsCount,
                (EOS_ProductUserId *)QueryOpts.UserIds);
            for (uint32_t i = 0; i < StatInfoCount; i++)
            {
                FMemory::Free((void *)StatInfo[i].StatName);
            }
            FMemory::Free(StatInfo);

            if (auto This = PinWeakThis(WeakThis))
            {
                if (Data->ResultCode != EOS_EResult::EOS_Success)
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("ReadLeaderboards operation failed with error code %s"),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
                    ReadObject->ReadState = EOnlineAsyncTaskState::Failed;
                    This->TriggerOnLeaderboardReadCompleteDelegates(false);
                    return;
                }

                // For each product user ID, fetch all of the stats and add them to a row.
                for (auto ProductUserId : ProductUserIds)
                {
                    FOnlineStatsRow Row = FOnlineStatsRow(TEXT(""), MakeShared<FUniqueNetIdEOS>(ProductUserId));

                    for (auto Column : ReadObject->ColumnMetadata)
                    {
                        EOS_Leaderboards_LeaderboardUserScore *UserScore = nullptr;

                        EOS_Leaderboards_CopyLeaderboardUserScoreByUserIdOptions CopyOpts = {};
                        CopyOpts.ApiVersion = EOS_LEADERBOARDS_COPYLEADERBOARDUSERSCOREBYUSERID_API_LATEST;
                        CopyOpts.StatName = nullptr;
                        if (EOSString_Stats_StatName::AllocateToCharBuffer(
                                Column.ColumnName.ToString(),
                                CopyOpts.StatName) != EOS_EResult::EOS_Success)
                        {
                            UE_LOG(
                                LogEOS,
                                Warning,
                                TEXT("Unable to copy stat named %s to row"),
                                *Column.ColumnName.ToString());
                            continue;
                        }
                        CopyOpts.UserId = ProductUserId;

                        if (EOS_Leaderboards_CopyLeaderboardUserScoreByUserId(
                                This->EOSLeaderboards,
                                &CopyOpts,
                                &UserScore) != EOS_EResult::EOS_Success)
                        {
                            UE_LOG(
                                LogEOS,
                                Warning,
                                TEXT("Unable to copy data for stat %s to row"),
                                *Column.ColumnName.ToString());
                            continue;
                        }

                        Row.Columns.Add(Column.ColumnName, UserScore->Score);

                        EOS_Leaderboards_LeaderboardUserScore_Release(UserScore);
                    }

                    ReadObject->Rows.Add(Row);
                }

                // Now we sort client side by the sorting column.
                if (ReadObject->SortedColumn.IsNone())
                {
                    UE_LOG(
                        LogEOS,
                        Warning,
                        TEXT("You must set SortedColumn when reading leaderboards by anything other than around a "
                             "rank, since there is no implicit sort on the leaderboard. Defaulting to the first column "
                             "requested."));
                    ReadObject->SortedColumn = ReadObject->ColumnMetadata[0].ColumnName;
                }
                ReadObject->Rows.RemoveAll([ReadObject](const FOnlineStatsRow &Candidate) {
                    return !Candidate.Columns.Contains(ReadObject->SortedColumn);
                });
                ReadObject->Rows.Sort([ReadObject](const FOnlineStatsRow &A, const FOnlineStatsRow &B) {
                    int32_t AValue, BValue;
                    A.Columns[ReadObject->SortedColumn].GetValue(AValue);
                    B.Columns[ReadObject->SortedColumn].GetValue(BValue);
                    return AValue > BValue;
                });
                for (int32_t i = 0; i < ReadObject->Rows.Num(); i++)
                {
                    ReadObject->Rows[i].Rank = i + 1;
                }

                // Now resolve all of the nicknames of the player rows using the first authenticated user
                // as the querying user.
                EOS_Connect_QueryProductUserIdMappingsOptions NickOpts = {};
                NickOpts.ApiVersion = EOS_CONNECT_QUERYPRODUCTUSERIDMAPPINGS_API_LATEST;
                NickOpts.LocalUserId = QueryingUserId->GetProductUserId();
                NickOpts.ProductUserIds = nullptr;
                NickOpts.ProductUserIdCount = 0;
                EOSString_ProductUserId::AllocateToIdListViaAccessor<FOnlineStatsRow>(
                    ReadObject->Rows,
                    [](const FOnlineStatsRow &Row) {
                        return StaticCastSharedRef<const FUniqueNetIdEOS>(Row.PlayerId.ToSharedRef())
                            ->GetProductUserId();
                    },
                    NickOpts.ProductUserIdCount,
                    NickOpts.ProductUserIds);
                EOSRunOperation<
                    EOS_HConnect,
                    EOS_Connect_QueryProductUserIdMappingsOptions,
                    EOS_Connect_QueryProductUserIdMappingsCallbackInfo>(
                    This->EOSConnect,
                    &NickOpts,
                    EOS_Connect_QueryProductUserIdMappings,
                    [WeakThis = GetWeakThis(This),
                     ReadObject](const EOS_Connect_QueryProductUserIdMappingsCallbackInfo *Info) {
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            TArray<FOnlineStatsRow> NewRows;

                            for (int i = 0; i < ReadObject->Rows.Num(); i++)
                            {
                                auto Row = ReadObject->Rows[i];

                                if (Row.NickName.IsEmpty())
                                {
                                    // Try to backfill this row's nickname data from the query.
                                    EOS_Connect_ExternalAccountInfo *ExternalAccountInfo = nullptr;

                                    EOS_Connect_CopyProductUserInfoOptions CopyOpts = {};
                                    CopyOpts.ApiVersion = EOS_CONNECT_COPYPRODUCTUSERINFO_API_LATEST;
                                    CopyOpts.TargetUserId =
                                        StaticCastSharedRef<const FUniqueNetIdEOS>(Row.PlayerId.ToSharedRef())
                                            ->GetProductUserId();

                                    auto Result = EOS_Connect_CopyProductUserInfo(
                                        This->EOSConnect,
                                        &CopyOpts,
                                        &ExternalAccountInfo);
                                    if (Result != EOS_EResult::EOS_Success)
                                    {
                                        UE_LOG(
                                            LogEOS,
                                            Error,
                                            TEXT("Unable to retrieve external user info: %s"),
                                            ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
                                        NewRows.Add(Row);
                                        continue;
                                    }

                                    FString Nickname = EOSString_UserInfo_DisplayName::FromUtf8String(
                                        ExternalAccountInfo->DisplayName);
                                    FOnlineStatsRow NewRow = FOnlineStatsRow(Nickname, Row.PlayerId.ToSharedRef());
                                    NewRow.Columns = Row.Columns;
                                    NewRow.Rank = Row.Rank;
                                    NewRows.Add(NewRow);

                                    EOS_Connect_ExternalAccountInfo_Release(ExternalAccountInfo);
                                }
                                else
                                {
                                    NewRows.Add(Row);
                                }
                            }

                            ReadObject->Rows = NewRows;
                            ReadObject->ReadState = EOnlineAsyncTaskState::Done;
                            This->TriggerOnLeaderboardReadCompleteDelegates(true);
                        }
                    });
            }
        });
    return true;
}

bool FOnlineLeaderboardsInterfaceEOS::ReadLeaderboardsForFriends(
    int32 LocalUserNum,
    FOnlineLeaderboardReadRef &ReadObject)
{
#if EOS_HAS_AUTHENTICATION
    if (!this->Friends.IsValid())
    {
        UE_LOG(LogEOS, Error, TEXT("ReadLeaderboardsForFriends can not be called on a dedicated server."));
        return false;
    }

    auto LocalPlayerId = this->Identity->GetUniquePlayerId(LocalUserNum);
    if (!LocalPlayerId.IsValid())
    {
        UE_LOG(LogEOS, Error, TEXT("Local user is not signed in."));
        return false;
    }

    this->Friends->ReadFriendsList(
        LocalUserNum,
        TEXT(""),
        FOnReadFriendsListComplete::CreateLambda(
            [WeakThis = GetWeakThis(this),
             LocalPlayerId,
             ReadObject](int32 LocalUserNum, bool bWasSuccessful, const FString &ListName, const FString &ErrorStr) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    if (!bWasSuccessful)
                    {
                        UE_LOG(
                            LogEOS,
                            Error,
                            TEXT("Unable to read friends list, so can not fetch leaderboards for friends."));
                        ReadObject->ReadState = EOnlineAsyncTaskState::Failed;
                        This->TriggerOnLeaderboardReadCompleteDelegates(false);
                        return;
                    }

                    TArray<TSharedRef<FOnlineFriend>> FriendsList;
                    if (!This->Friends->GetFriendsList(LocalUserNum, TEXT(""), FriendsList))
                    {
                        UE_LOG(
                            LogEOS,
                            Error,
                            TEXT("Unable to read friends list, so can not fetch leaderboards for friends."));
                        ReadObject->ReadState = EOnlineAsyncTaskState::Failed;
                        This->TriggerOnLeaderboardReadCompleteDelegates(false);
                        return;
                    }

                    TArray<TSharedRef<const FUniqueNetId>> FilteredFriends;
                    FilteredFriends.Add(LocalPlayerId.ToSharedRef());
                    for (const auto &Friend : FriendsList)
                    {
                        auto Id = Friend->GetUserId();
                        if (Id->GetType() == REDPOINT_EOS_SUBSYSTEM)
                        {
                            FilteredFriends.Add(Id);
                        }
                    }

                    TSharedRef<const FUniqueNetIdEOS> IdEOS =
                        StaticCastSharedRef<const FUniqueNetIdEOS>(LocalPlayerId.ToSharedRef());

                    // If this returns true, it will manage firing the callback.
                    FOnlineLeaderboardReadRef ReadCopy = ReadObject;
                    if (!This->ReadLeaderboardsWithLocalUserContext(IdEOS, FilteredFriends, ReadCopy))
                    {
                        ReadObject->ReadState = EOnlineAsyncTaskState::Failed;
                        This->TriggerOnLeaderboardReadCompleteDelegates(false);
                        return;
                    }
                }
            }));
    return true;
#else
    UE_LOG(LogEOS, Error, TEXT("ReadLeaderboardsForFriends can not be called on a dedicated server."));
    return false;
#endif // #if EOS_HAS_AUTHENTICATION
}

void FOnlineLeaderboardsInterfaceEOS::ProcessNextDeferredRequest()
{
    checkf(
        !this->bIsExecutingQueryLeaderboardRanks,
        TEXT("FOnlineLeaderboardsInterfaceEOS::ProcessNextDeferredRequest called while "
             "bIsExecutingQueryLeaderboardRanks is true; this should not happen!"));

    if (this->DeferredRequests.Num() == 0)
    {
        // Nothing to start execution on.
        return;
    }

    auto NextRequest = this->DeferredRequests[0];
    this->DeferredRequests.RemoveAt(0);

    if (!this->ReadLeaderboardsAroundRank(NextRequest.Rank, NextRequest.Range, NextRequest.ReadObject))
    {
        // We have to notify via global callback that the operation failed,
        // since we always return true to indicate a callback will fire
        // when we're deferring, even if the error is otherwise an immediate
        // failure.
        this->TriggerOnLeaderboardReadCompleteDelegates(false);

        // NOTE: We don't have to call ProcessNextDeferredRequest here, because
        // all code paths inside ReadLeaderboardsAroundRank eventually call it.
    }
}

bool FOnlineLeaderboardsInterfaceEOS::ReadLeaderboardsAroundRank(
    int32 Rank,
    uint32 Range,
    FOnlineLeaderboardReadRef &ReadObject)
{
#if EOS_VERSION_AT_LEAST(1, 10, 2)
    if (this->Identity->GetLoginStatus(0) != ELoginStatus::LoggedIn)
    {
        ReadObject->ReadState = EOnlineAsyncTaskState::Failed;
        UE_LOG(
            LogEOS,
            Error,
            TEXT("LocalUserNum 0 must be signed into EOS in order to use ReadLeaderboardsAroundRank."));
        this->ProcessNextDeferredRequest();
        return false;
    }

    auto LocalUserId = StaticCastSharedPtr<const FUniqueNetIdEOS>(this->Identity->GetUniquePlayerId(0));
#endif

    if (this->bIsExecutingQueryLeaderboardRanks)
    {
        // Defer the request.
        this->DeferredRequests.Add(FDeferredQueryLeaderboardRanksRequest(Rank, Range, ReadObject));
        return true;
    }

    this->bIsExecutingQueryLeaderboardRanks = true;

    EOS_Leaderboards_QueryLeaderboardRanksOptions Opts = {};
    Opts.ApiVersion = EOS_LEADERBOARDS_QUERYLEADERBOARDRANKS_API_LATEST;
    Opts.LeaderboardId = nullptr;
#if EOS_VERSION_AT_LEAST(1, 10, 2)
    if (LocalUserId->IsDedicatedServer())
    {
        Opts.LocalUserId = nullptr;
    }
    else
    {
        Opts.LocalUserId = LocalUserId->GetProductUserId();
    }
#endif
    EOS_EResult AllocateResult = EOSString_Leaderboards_LeaderboardId::AllocateToCharBuffer(
        ReadObject->LeaderboardName.ToString(),
        Opts.LeaderboardId);
    if (AllocateResult != EOS_EResult::EOS_Success)
    {
        ReadObject->ReadState = EOnlineAsyncTaskState::Failed;
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Can not pass leaderboard ID '%s' to operation, got result %s"),
            *ReadObject->LeaderboardName.ToString(),
            ANSI_TO_TCHAR(EOS_EResult_ToString(AllocateResult)));
        this->bIsExecutingQueryLeaderboardRanks = false;
        this->ProcessNextDeferredRequest();
        return false;
    }

    uint32_t LowerRankInclusive = ((int32)(Rank - Range) < 0) ? 0 : (uint32_t)(Rank - Range);
    uint32_t UpperRankInclusive = Rank + Range;

    ReadObject->ReadState = EOnlineAsyncTaskState::InProgress;

    EOSRunOperation<
        EOS_HLeaderboards,
        EOS_Leaderboards_QueryLeaderboardRanksOptions,
        EOS_Leaderboards_OnQueryLeaderboardRanksCompleteCallbackInfo>(
        this->EOSLeaderboards,
        &Opts,
        EOS_Leaderboards_QueryLeaderboardRanks,
        [WeakThis = GetWeakThis(this), Opts, LowerRankInclusive, UpperRankInclusive, ReadObject](
            const EOS_Leaderboards_OnQueryLeaderboardRanksCompleteCallbackInfo *Data) {
            EOSString_Leaderboards_LeaderboardId::FreeFromCharBufferConst(Opts.LeaderboardId);

            if (auto This = PinWeakThis(WeakThis))
            {
                if (Data->ResultCode != EOS_EResult::EOS_Success
#if defined(HAS_DEVICE_ID_RESOLVE_BUG)
                    // This API is also impacted by the "device ID resolve" bug; see OnlineUserEOS.h for more
                    // information.
                    && Data->ResultCode != EOS_EResult::EOS_UnrecognizedResponse
#endif
                )
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("ReadLeaderboards operation failed with error code %s"),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
                    ReadObject->ReadState = EOnlineAsyncTaskState::Failed;
                    This->TriggerOnLeaderboardReadCompleteDelegates(false);
                    This->bIsExecutingQueryLeaderboardRanks = false;
                    This->ProcessNextDeferredRequest();
                    return;
                }

                EOS_Leaderboards_GetLeaderboardRecordCountOptions CountOpts = {};
                CountOpts.ApiVersion = EOS_LEADERBOARDS_GETLEADERBOARDRECORDCOUNT_API_LATEST;
                uint32_t RecordCount = EOS_Leaderboards_GetLeaderboardRecordCount(This->EOSLeaderboards, &CountOpts);

                if (RecordCount < LowerRankInclusive)
                {
                    // No relevant data.
                    ReadObject->ReadState = EOnlineAsyncTaskState::Done;
                    This->TriggerOnLeaderboardReadCompleteDelegates(true);
                    This->bIsExecutingQueryLeaderboardRanks = false;
                    This->ProcessNextDeferredRequest();
                    return;
                }

                for (uint32_t i = LowerRankInclusive; i < RecordCount && i <= UpperRankInclusive; i++)
                {
                    EOS_Leaderboards_LeaderboardRecord *Record = nullptr;

                    EOS_Leaderboards_CopyLeaderboardRecordByIndexOptions CopyOpts = {};
                    CopyOpts.ApiVersion = EOS_LEADERBOARDS_GETLEADERBOARDRECORDCOUNT_API_LATEST;
                    CopyOpts.LeaderboardRecordIndex = i;
                    if (EOS_Leaderboards_CopyLeaderboardRecordByIndex(This->EOSLeaderboards, &CopyOpts, &Record) !=
                        EOS_EResult::EOS_Success)
                    {
                        UE_LOG(
                            LogEOS,
                            Warning,
                            TEXT("EOS_Leaderboards_CopyLeaderboardRecordByIndex failed to copy for index %d"),
                            i);
                        continue;
                    }

                    FString Nickname = EOSString_UserInfo_DisplayName::FromUtf8String(Record->UserDisplayName);
                    TSharedRef<const FUniqueNetIdEOS> UserId = MakeShared<FUniqueNetIdEOS>(Record->UserId);

                    FOnlineStatsRow Row = FOnlineStatsRow(Nickname, UserId);
                    Row.Rank = i + 1;
                    Row.Columns.Add(FName(TEXT("Score")), Record->Score);
                    ReadObject->Rows.Add(Row);

                    EOS_Leaderboards_LeaderboardRecord_Release(Record);
                }

                ReadObject->ReadState = EOnlineAsyncTaskState::Done;
                This->TriggerOnLeaderboardReadCompleteDelegates(true);
                This->bIsExecutingQueryLeaderboardRanks = false;
                This->ProcessNextDeferredRequest();
            }
        });
    return true;
}

bool FOnlineLeaderboardsInterfaceEOS::ReadLeaderboardsAroundUser(
    TSharedRef<const FUniqueNetId> Player,
    uint32 Range,
    FOnlineLeaderboardReadRef &ReadObject)
{
    if (Range != 0)
    {
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("EOS does not support reading a range of scores around a user ID. Will assume range of 0."));
    }

    TArray<TSharedRef<const FUniqueNetId>> PlayerList;
    PlayerList.Add(Player);

    return this->ReadLeaderboards(PlayerList, ReadObject);
}

void FOnlineLeaderboardsInterfaceEOS::FreeStats(FOnlineLeaderboardRead &ReadObject)
{
}

bool FOnlineLeaderboardsInterfaceEOS::WriteLeaderboards(
    const FName &SessionName,
    const FUniqueNetId &Player,
    FOnlineLeaderboardWrite &WriteObject)
{
    if (Player.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("WriteLeaderboards: Player was invalid (not an EOS user)"));
        return false;
    }

    TSharedRef<const FUniqueNetIdEOS> LocalUserIdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(Player.AsShared());

    FOnlineStatsUserUpdatedStats UpdatedStat = FOnlineStatsUserUpdatedStats(LocalUserIdEOS);
    for (const auto &Prop : WriteObject.Properties)
    {
        UpdatedStat.Stats.Add(
            Prop.Key.ToString(),
            FOnlineStatUpdate(Prop.Value, FOnlineStatUpdate::EOnlineStatModificationType::Unknown));
    }

    TArray<FOnlineStatsUserUpdatedStats> UpdatedStats;
    UpdatedStats.Add(UpdatedStat);

    this->Stats->UpdateStats(
        LocalUserIdEOS,
        UpdatedStats,
        FOnlineStatsUpdateStatsComplete::CreateLambda([LocalUserIdEOS](const FOnlineError &ResultState) {
            if (!ResultState.bSucceeded)
            {
                UE_LOG(
                    LogEOS,
                    Error,
                    TEXT("WriteLeaderboards operation failed (you should use IOnlineStatsPtr directly to be able to "
                         "catch errors)"));
            }
            else
            {
                UE_LOG(
                    LogEOS,
                    Verbose,
                    TEXT("WriteLeaderboards operation succeeded (though you should use IOnlineStatsPtr directly to be "
                         "able to handle the result)"));
            }
        }));
    return true;
}

bool FOnlineLeaderboardsInterfaceEOS::FlushLeaderboards(const FName &SessionName)
{
    this->TriggerOnLeaderboardFlushCompleteDelegates(SessionName, true);
    return true;
}

bool FOnlineLeaderboardsInterfaceEOS::WriteOnlinePlayerRatings(
    const FName &SessionName,
    int32 LeaderboardId,
    const TArray<FOnlinePlayerScore> &PlayerScores)
{
    UE_LOG(LogEOS, Error, TEXT("EOS does not support the WriteOnlinePlayerRatings operation."));
    return false;
}

EOS_DISABLE_STRICT_WARNINGS