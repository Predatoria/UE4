// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/OnlineStatsInterfaceEOS.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Net/DataChannel.h"
#include "OnlineSubsystemRedpointEOS/Private/EOSControlChannel.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSControlStats.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSErrorConv.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/MultiOperation.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/WorldResolution.h"
#include "UObject/Object.h"
#include "Misc/AutomationTest.h"

// This needs to be wrapped in an EOS_VERSION_AT_MOST check once
// the bug is fixed in the EOS SDK.
#define EOS_HAS_WRITE_STAT_BUG 1

EOS_ENABLE_STRICT_WARNINGS

constexpr float StatFloatDivisor = 10000000.0f;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FOnlineSubsystemEOS_StatTypeRegexTests,
    "OnlineSubsystemEOS.StatTypeRegexTests",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

static FString GetStatTypeName(const EStatTypingRule& Value)
{
    switch (Value)
    {
    case EStatTypingRule::Int32:
        return TEXT("Int32");
    case EStatTypingRule::Bool:
        return TEXT("Bool");
    case EStatTypingRule::DoubleEncoded:
        return TEXT("Double Encoded");
    case EStatTypingRule::FloatEncoded:
        return TEXT("Float Encoded");
    case EStatTypingRule::FloatTruncated:
        return TEXT("Float Truncated");
    default:
        return TEXT("Unknown");
    }
}

static TTuple<FRegexPattern, TPair<FString, EStatTypingRule>> ConvertRuleToRegex(const TPair<FString, EStatTypingRule>& KV)
{
    return TTuple<FRegexPattern, TPair<FString, EStatTypingRule>>(
        FRegexPattern(FString::Printf(TEXT("^%s$"), *KV.Key.Replace(TEXT("*"), TEXT(".+")))),
        KV);
}

static EStatTypingRule EvaluateRules(
    const FString &StatName,
    const TArray<TTuple<FRegexPattern, TPair<FString, EStatTypingRule>>>&Rules)
{
    EStatTypingRule Type = EStatTypingRule::Int32;
    bool bDidMatchAny = false;
    for (const auto &Rule : Rules)
    {
        UE_LOG(
            LogEOSStat,
            Verbose,
            TEXT("Stat '%s': Evaluating against rule '%s'..."),
            *StatName,
            *Rule.Value.Key);
        if (FRegexMatcher(Rule.Key, StatName).FindNext())
        {
            Type = Rule.Value.Value;
            UE_LOG(
                LogEOSStat,
                Verbose,
                TEXT("Stat '%s': Matched rule '%s' resulting in type %s."),
                *StatName,
                *Rule.Value.Key,
                *GetStatTypeName(Type));
            bDidMatchAny = true;
            break;
        }
    }
    if (!bDidMatchAny)
    {
        UE_LOG(
            LogEOSStat,
            Warning,
            TEXT("Stat '%s': Did not match any rule in Project Settings, defaulting to Int32."),
            *StatName);
    }
    return Type;
}

bool FOnlineSubsystemEOS_StatTypeRegexTests::RunTest(const FString &Parameters)
{
    TArray<TTuple<FRegexPattern, TPair<FString, EStatTypingRule>>> Rules;
    Rules.Add(ConvertRuleToRegex(TPair<FString, EStatTypingRule>("Item", EStatTypingRule::Int32)));
    Rules.Add(ConvertRuleToRegex(TPair<FString, EStatTypingRule>("Other", EStatTypingRule::Int32)));
    Rules.Add(ConvertRuleToRegex(TPair<FString, EStatTypingRule>("mmr1v1_*", EStatTypingRule::FloatEncoded)));
    Rules.Add(ConvertRuleToRegex(TPair<FString, EStatTypingRule>("mmr2v2_*", EStatTypingRule::FloatEncoded)));

    this->TestEqual("mmr1v1_mu is Float Encoded", EvaluateRules("mmr1v1_mu", Rules), EStatTypingRule::FloatEncoded);
    this->TestEqual("mmr1v1_sigma is Float Encoded", EvaluateRules("mmr1v1_sigma", Rules), EStatTypingRule::FloatEncoded);

    return true;
}

FOnlineStatsInterfaceEOS::FOnlineStatsInterfaceEOS(
    EOS_HPlatform InPlatform,
    const TSharedRef<FEOSConfig> &InConfig,
    const FName &InInstanceName)
    : EOSStats(EOS_Platform_GetStatsInterface(InPlatform))
    , StatsCache()
    , CachedStatRules()
    , CachedStatTypes()
    , InstanceName(InInstanceName)
    , Config(InConfig)
{
    check(this->EOSStats != nullptr);

    int32 RuleCount = 0;
    for (const auto &KV : InConfig->GetStatTypingRules())
    {
        UE_LOG(LogEOSStat, Verbose, TEXT("Loaded rule '%s' which is set to type %s"), *KV.Key, *GetStatTypeName(KV.Value));
        this->CachedStatRules.Add(ConvertRuleToRegex(KV));
        RuleCount++;
    }
    UE_LOG(LogEOSStat, Verbose, TEXT("Loaded %d stat typing rules from configuration."), RuleCount);
}

EStatTypingRule FOnlineStatsInterfaceEOS::GetTypeForStat(const FString &StatName)
{
    if (this->CachedStatTypes.Contains(StatName))
    {
        UE_LOG(
            LogEOSStat,
            Verbose,
            TEXT("Stat '%s': Has cached type %d in CachedStatTypes."),
            *StatName,
            this->CachedStatTypes[StatName]);
        return this->CachedStatTypes[StatName];
    }
    EStatTypingRule Type = EvaluateRules(StatName, this->CachedStatRules);
    this->CachedStatTypes.Add(StatName, Type);
    return Type;
}

void FOnlineStatsInterfaceEOS::QueryStats(
    const TSharedRef<const FUniqueNetId> LocalUserId,
    const TSharedRef<const FUniqueNetId> StatsUser,
    const FOnlineStatsQueryUserStatsComplete &Delegate)
{
    if (LocalUserId->GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("QueryStats: LocalUserId was invalid (not an EOS user)"));
        Delegate.ExecuteIfBound(OnlineRedpointEOS::Errors::InvalidUser(), nullptr);
        return;
    }

    if (StatsUser->GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("QueryStats: StatsUser was invalid (not an EOS user)"));
        Delegate.ExecuteIfBound(OnlineRedpointEOS::Errors::InvalidUser(), nullptr);
        return;
    }

    TArray<TSharedRef<const FUniqueNetId>> StatsUsers;
    StatsUsers.Add(StatsUser);

    TArray<FString> StatNames;

    this->QueryStats(
        LocalUserId,
        StatsUsers,
        StatNames,
        FOnlineStatsQueryUsersStatsComplete::CreateLambda(
            [Delegate](
                const FOnlineError &ResultState,
                const TArray<TSharedRef<const FOnlineStatsUserStats>> &UsersStatsResult) {
                if (UsersStatsResult[0]->Stats.Num() == 0)
                {
                    UE_LOG(
                        LogEOS,
                        Warning,
                        TEXT(
                            "No stats were returned by EOS for the QueryStats operation. EOS will not return empty "
                            "stats (stats you have never sent data for), and if you don't provide StatNames in your "
                            "QueryStats call, the EOS plugin can not backfill those stats with empty values for you."));
                }

                Delegate.ExecuteIfBound(ResultState, UsersStatsResult[0]);
            }));
}

void FOnlineStatsInterfaceEOS::QueryStats(
    const TSharedRef<const FUniqueNetId> LocalUserId,
    const TArray<TSharedRef<const FUniqueNetId>> &StatUsers,
    const TArray<FString> &StatNames,
    const FOnlineStatsQueryUsersStatsComplete &Delegate)
{
    if (LocalUserId->GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("QueryStats: LocalUserId was invalid (not an EOS user)"));
        Delegate.ExecuteIfBound(
            OnlineRedpointEOS::Errors::InvalidUser(),
            TArray<TSharedRef<const FOnlineStatsUserStats>>());
        return;
    }

    TSharedRef<const FUniqueNetIdEOS> LocalUserIdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(LocalUserId);

    FMultiOperation<TSharedRef<const FUniqueNetId>, TSharedRef<const FOnlineStatsUserStats>>::Run(
        StatUsers,
        [WeakThis = GetWeakThis(this), LocalUserIdEOS, StatNames](
            const TSharedRef<const FUniqueNetId> &TargetUserId,
            const std::function<void(const TSharedRef<const FOnlineStatsUserStats> OutValue)> &OnDone) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (TargetUserId->GetType() != REDPOINT_EOS_SUBSYSTEM)
                {
                    UE_LOG(LogEOS, Warning, TEXT("QueryStats: StatUser was invalid (not an EOS user)"));
                    return false;
                }

                TSharedRef<const FUniqueNetIdEOS> TargetUserIdEOS =
                    StaticCastSharedRef<const FUniqueNetIdEOS>(TargetUserId);

                EOS_Stats_QueryStatsOptions Opts = {};
                Opts.ApiVersion = EOS_STATS_QUERYSTATS_API_LATEST;
#if EOS_VERSION_AT_LEAST(1, 8, 0)
                Opts.LocalUserId = LocalUserIdEOS->GetProductUserId();
                Opts.TargetUserId = TargetUserIdEOS->GetProductUserId();
#else
                Opts.UserId = TargetUserIdEOS->GetProductUserId();
#endif
                Opts.StartTime = EOS_STATS_TIME_UNDEFINED;
                Opts.EndTime = EOS_STATS_TIME_UNDEFINED;
                TArray<FString> GeneratedStatNames;
                for (const auto &StatName : StatNames)
                {
                    switch (This->GetTypeForStat(StatName))
                    {
                    case EStatTypingRule::DoubleEncoded:
                        GeneratedStatNames.Add(FString::Printf(TEXT("%s_u"), *StatName));
                        GeneratedStatNames.Add(FString::Printf(TEXT("%s_l"), *StatName));
                        break;
                    default:
                        GeneratedStatNames.Add(StatName);
                        break;
                    }
                }
                for (const auto& Name : GeneratedStatNames)
                {
                    UE_LOG(
                        LogEOSStat,
                        Verbose,
                        TEXT("Stat '%s': User '%s': Preparing to query."),
                        *Name,
                        *TargetUserIdEOS->ToString());
                }
                EOSString_Stats_StatName::AllocateToCharList(GeneratedStatNames, Opts.StatNamesCount, Opts.StatNames);

                EOSRunOperation<EOS_HStats, EOS_Stats_QueryStatsOptions, EOS_Stats_OnQueryStatsCompleteCallbackInfo>(
                    This->EOSStats,
                    &Opts,
                    EOS_Stats_QueryStats,
                    [WeakThis = GetWeakThis(This), TargetUserIdEOS, OnDone, StatNames, Opts](
                        const EOS_Stats_OnQueryStatsCompleteCallbackInfo *Data) {
                        EOSString_Stats_StatName::FreeFromCharListConst(Opts.StatNamesCount, Opts.StatNames);

                        if (auto This = PinWeakThis(WeakThis))
                        {
                            TSharedRef<FOnlineStatsUserStats> Result =
                                MakeShared<FOnlineStatsUserStats>(TargetUserIdEOS);

                            if (Data->ResultCode != EOS_EResult::EOS_Success)
                            {
                                OnDone(Result);
                                return;
                            }

                            EOS_Stats_GetStatCountOptions CountOpts = {};
                            CountOpts.ApiVersion = EOS_STATS_GETSTATCOUNT_API_LATEST;
#if EOS_VERSION_AT_LEAST(1, 8, 0)
                            CountOpts.TargetUserId = TargetUserIdEOS->GetProductUserId();
#else
                            CountOpts.UserId = TargetUserIdEOS->GetProductUserId();
#endif
                            uint32_t Count = EOS_Stats_GetStatsCount(This->EOSStats, &CountOpts);

                            TMap<FString, int32> RawStatValues;
                            for (uint32_t i = 0; i < Count; i++)
                            {
                                EOS_Stats_CopyStatByIndexOptions CopyOpts = {};
                                CopyOpts.ApiVersion = EOS_STATS_COPYSTATBYINDEX_API_LATEST;
                                CopyOpts.StatIndex = i;
#if EOS_VERSION_AT_LEAST(1, 8, 0)
                                CopyOpts.TargetUserId = TargetUserIdEOS->GetProductUserId();
#else
                                CopyOpts.UserId = TargetUserIdEOS->GetProductUserId();
#endif

                                EOS_Stats_Stat *Stat = nullptr;
                                EOS_EResult CopyResult = EOS_Stats_CopyStatByIndex(This->EOSStats, &CopyOpts, &Stat);
                                if (CopyResult != EOS_EResult::EOS_Success)
                                {
                                    UE_LOG(
                                        LogEOS,
                                        Warning,
                                        TEXT("Unable to copy stat at index %d, got error %s"),
                                        i,
                                        ANSI_TO_TCHAR(EOS_EResult_ToString(CopyResult)));
                                }

                                FString StatName = EOSString_Stats_StatName::FromAnsiString(Stat->Name);
                                RawStatValues.Add(StatName, Stat->Value);
                                EOS_Stats_Stat_Release(Stat);
                            }

                            for (const auto &StatName : StatNames)
                            {
                                EStatTypingRule StatType = This->GetTypeForStat(StatName);

                                FOnlineStatValue Val;
                                switch (StatType)
                                {
                                case EStatTypingRule::Int32:
                                    if (!RawStatValues.Contains(StatName))
                                    {
                                        continue;
                                    }
                                    UE_LOG(
                                        LogEOSStat,
                                        Verbose,
                                        TEXT("Stat '%s': User '%s': Reading '%d' as Int32 value '%d'"),
                                        *StatName,
                                        *TargetUserIdEOS->ToString(),
                                        RawStatValues[StatName],
                                        RawStatValues[StatName]);
                                    Val.SetValue(RawStatValues[StatName]);
                                    break;
                                case EStatTypingRule::Bool:
                                    if (!RawStatValues.Contains(StatName))
                                    {
                                        continue;
                                    }
                                    UE_LOG(
                                        LogEOSStat,
                                        Verbose,
                                        TEXT("Stat '%s': User '%s': Reading '%d' as Bool value '%s'"),
                                        *StatName,
                                        *TargetUserIdEOS->ToString(),
                                        RawStatValues[StatName],
                                        RawStatValues[StatName] == 1 ? TEXT("true") : TEXT("false"));
                                    Val.SetValue(RawStatValues[StatName] == 1);
                                    break;
                                case EStatTypingRule::FloatTruncated:
                                    if (!RawStatValues.Contains(StatName))
                                    {
                                        continue;
                                    }
                                    UE_LOG(
                                        LogEOSStat,
                                        Verbose,
                                        TEXT("Stat '%s': User '%s': Reading '%d' as Float Truncated value '%f'"),
                                        *StatName,
                                        *TargetUserIdEOS->ToString(),
                                        RawStatValues[StatName],
                                        RawStatValues[StatName] / StatFloatDivisor);
                                    Val.SetValue(RawStatValues[StatName] / StatFloatDivisor);
                                    break;
                                case EStatTypingRule::FloatEncoded:
                                    if (!RawStatValues.Contains(StatName))
                                    {
                                        continue;
                                    }
                                    {
                                        float FloatEncoded = *((float *)&RawStatValues[StatName]);
                                        UE_LOG(
                                            LogEOSStat,
                                            Verbose,
                                            TEXT("Stat '%s': User '%s': Reading '%d' as Float Encoded value '%f'"),
                                            *StatName,
                                            *TargetUserIdEOS->ToString(),
                                            RawStatValues[StatName],
                                            FloatEncoded);
                                        Val.SetValue(FloatEncoded);
                                    }
                                    break;
                                case EStatTypingRule::DoubleEncoded:
                                    if (!RawStatValues.Contains(FString::Printf(TEXT("%s_u"), *StatName)) ||
                                        !RawStatValues.Contains(FString::Printf(TEXT("%s_l"), *StatName)))
                                    {
                                        continue;
                                    }
                                    uint64 DblUpper =
                                        *((uint32 *)&RawStatValues[FString::Printf(TEXT("%s_u"), *StatName)]);
                                    uint64 DblLower =
                                        *((uint32 *)&RawStatValues[FString::Printf(TEXT("%s_l"), *StatName)]);
                                    uint64 DblCombined = (DblUpper << 32) | DblLower;
                                    UE_LOG(
                                        LogEOSStat,
                                        Verbose,
                                        TEXT("Stat '%s': User '%s': Reading '%d' (upper bits from '%s_u') as Double Encoded value '%f'"),
                                        *StatName,
                                        *TargetUserIdEOS->ToString(),
                                        RawStatValues[FString::Printf(TEXT("%s_u"), *StatName)],
                                        *StatName,
                                        *((double *)&DblCombined));
                                    UE_LOG(
                                        LogEOSStat,
                                        Verbose,
                                        TEXT("Stat '%s': User '%s': Reading '%d' (lower bits from '%s_l') as Double Encoded value "
                                             "'%f'"),
                                        *StatName,
                                        *TargetUserIdEOS->ToString(),
                                        RawStatValues[FString::Printf(TEXT("%s_l"), *StatName)],
                                        *StatName,
                                        *((double *)&DblCombined));
                                    Val.SetValue(*((double *)&DblCombined));
                                    break;
                                }
                                Result->Stats.Add(StatName, Val);
                            }

                            // For all of the stats that EOS didn't return, give them an empty entry. EOS only returns
                            // stats for which data has previously been sent.
                            for (const auto &StatName : StatNames)
                            {
                                if (!Result->Stats.Contains(StatName))
                                {
                                    Result->Stats.Add(StatName, FOnlineStatValue());
                                }
                            }

                            This->StatsCache.Add(*TargetUserIdEOS, Result);

                            OnDone(Result);
                        }
                    });
                return true;
            }

            return false;
        },
        [WeakThis = GetWeakThis(this), Delegate](const TArray<TSharedRef<const FOnlineStatsUserStats>> &OutValues) {
            Delegate.ExecuteIfBound(OnlineRedpointEOS::Errors::Success(), OutValues);
        },
        MakeShared<FOnlineStatsUserStats>(LocalUserIdEOS));
}

TSharedPtr<const FOnlineStatsUserStats> FOnlineStatsInterfaceEOS::GetStats(
    const TSharedRef<const FUniqueNetId> StatsUserId) const
{
    if (this->StatsCache.Contains(*StatsUserId))
    {
        return this->StatsCache[*StatsUserId];
    }
    return nullptr;
}

void FOnlineStatsInterfaceEOS::UpdateStats(
    const TSharedRef<const FUniqueNetId> LocalUserId,
    const TArray<FOnlineStatsUserUpdatedStats> &UpdatedUserStats,
    const FOnlineStatsUpdateStatsComplete &Delegate)
{
    if (LocalUserId->GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("UpdateStats: LocalUserId was invalid (not an EOS user)"));
        Delegate.ExecuteIfBound(OnlineRedpointEOS::Errors::InvalidUser());
        return;
    }

    TSharedRef<const FUniqueNetIdEOS> LocalUserIdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(LocalUserId);

    FMultiOperation<FOnlineStatsUserUpdatedStats, bool>::Run(
        UpdatedUserStats,
        [WeakThis = GetWeakThis(this),
         LocalUserIdEOS](FOnlineStatsUserUpdatedStats StatToUpdate, const std::function<void(bool OutValue)> &OnDone) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (StatToUpdate.Account->GetType() != REDPOINT_EOS_SUBSYSTEM)
                {
                    UE_LOG(LogEOS, Warning, TEXT("UpdateStats: Target user ID was invalid (not an EOS user)"));
                    return false;
                }

                TSharedRef<const FUniqueNetIdEOS> TargetUserIdEOS =
                    StaticCastSharedRef<const FUniqueNetIdEOS>(StatToUpdate.Account);

#if EOS_HAS_WRITE_STAT_BUG
                if (*TargetUserIdEOS != *LocalUserIdEOS && This->Config->GetAcceptStatWriteRequestsFromServers())
                {
                    // We can't write other users' stats, even with the ingestStatForAnyUser policy
                    // enabled, due to a bug in the EOS SDK. To workaround this bug, we forward
                    // the UpdateStats request if we're a listen server with connected clients.
                    bool bSentNMTToPlayer = false;
                    TSoftObjectPtr<UWorld> World = FWorldResolution::GetWorld(This->InstanceName, true);
                    if (World.IsValid())
                    {
                        if (World->GetNetMode() == NM_ListenServer)
                        {
                            for (auto It = World->GetPlayerControllerIterator(); It; ++It)
                            {
                                if (It->IsValid())
                                {
                                    APlayerController *PC = It->Get();
                                    APlayerState *PS = PC->GetPlayerState<APlayerState>();
                                    if (IsValid(PS))
                                    {
                                        FUniqueNetIdRepl NetIdRepl = PS->GetUniqueId();
                                        if (NetIdRepl.IsValid())
                                        {
                                            if (TargetUserIdEOS->ToString() == NetIdRepl->ToString())
                                            {
                                                TArray<uint8> SerializedStats = SerializeStats(StatToUpdate);
                                                FNetControlMessage<NMT_EOS_WriteStat>::Send(
                                                    PC->GetNetConnection(),
                                                    SerializedStats);
                                                bSentNMTToPlayer = true;
                                                break;
                                            }
                                        }
                                    }
                                }
                            }

                            if (bSentNMTToPlayer)
                            {
                                OnDone(true);
                            }
                            else
                            {
                                UE_LOG(
                                    LogEOS,
                                    Error,
                                    TEXT("Stats can't be written for player '%s'. The EOS SDK currently has a bug that "
                                         "prevents the use of the `ingestStatForAnyUser` policy. As a workaround, the "
                                         "EOS Online Subsystem plugin supports updating stats for other players, only "
                                         "when the writer is a listen server and the target player is currently "
                                         "connected to the listen server. The player '%s' wasn't currently connected, "
                                         "so the stat can't be written."),
                                    *TargetUserIdEOS->ToString(),
                                    *TargetUserIdEOS->ToString());
                                OnDone(false);
                            }
                            return true;
                        }
                    }
                }
#endif

                TArray<FString> StatKeys;
                StatToUpdate.Stats.GetKeys(StatKeys);

                if (StatKeys.Num() == 0)
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("Error while submitting stats for %s: You submitted an empty array of stats, which is not "
                             "permitted. You should skip calling the UpdateStats function if you have no stats to "
                             "update."),
                        *TargetUserIdEOS->ToString());
                    OnDone(false);
                    return true;
                }

                TMap<FString, int32> GeneratedStats;
                for (const auto &StatName : StatKeys)
                {
                    const auto &StatVal = StatToUpdate.Stats[StatName].GetValue();
                    EStatTypingRule StatType = This->GetTypeForStat(StatName);
                    if (StatType == EStatTypingRule::Int32)
                    {
                        if (StatVal.GetType() == EOnlineKeyValuePairDataType::Int32)
                        {
                            int32 Value;
                            StatVal.GetValue(Value);
                            GeneratedStats.Add(StatName, Value);
                            UE_LOG(
                                LogEOSStat,
                                Verbose,
                                TEXT("Stat '%s': User '%s': Writing Int32 value '%d' as '%d'"),
                                *StatName,
                                *TargetUserIdEOS->ToString(),
                                Value,
                                Value);
                        }
                        else
                        {
                            UE_LOG(
                                LogEOS,
                                Error,
                                TEXT("Stat '%s' is configured as Int32 in Project Settings, but non-int32 "
                                     "value provided during UpdateStats call. This stat will be ignored."),
                                *StatName);
                            continue;
                        }
                    }
                    else if (StatType == EStatTypingRule::Bool)
                    {
                        if (StatVal.GetType() == EOnlineKeyValuePairDataType::Bool)
                        {
                            bool bValue;
                            StatVal.GetValue(bValue);
                            int32 Value = bValue ? 1 : 0;
                            GeneratedStats.Add(StatName, Value);
                            UE_LOG(
                                LogEOSStat,
                                Verbose,
                                TEXT("Stat '%s': User '%s': Writing Bool value '%s' as '%d'"),
                                *StatName,
                                *TargetUserIdEOS->ToString(),
                                bValue ? TEXT("true") : TEXT("false"),
                                Value);
                        }
                        else
                        {
                            UE_LOG(
                                LogEOS,
                                Error,
                                TEXT("Stat '%s' is configured as Bool in Project Settings, but non-boolean "
                                     "value provided during UpdateStats call. This stat will be ignored."),
                                *StatName);
                            continue;
                        }
                    }
                    else if (StatType == EStatTypingRule::FloatTruncated)
                    {
                        if (StatVal.GetType() == EOnlineKeyValuePairDataType::Float)
                        {
                            float FloatTrunc;
                            StatVal.GetValue(FloatTrunc);
                            int32 Value = FMath::TruncToInt(FloatTrunc * StatFloatDivisor);
                            GeneratedStats.Add(StatName, Value);
                            UE_LOG(
                                LogEOSStat,
                                Verbose,
                                TEXT("Stat '%s': User '%s': Writing Float Truncated value '%f' as '%d'"),
                                *StatName,
                                *TargetUserIdEOS->ToString(),
                                FloatTrunc,
                                Value);
                        }
                        else if (StatVal.GetType() == EOnlineKeyValuePairDataType::Double)
                        {
                            double DoubleTrunc;
                            StatVal.GetValue(DoubleTrunc);
                            int32 Value = FMath::TruncToInt((float)DoubleTrunc * StatFloatDivisor);
                            GeneratedStats.Add(StatName, Value);
                            UE_LOG(
                                LogEOSStat,
                                Verbose,
                                TEXT("Stat '%s': User '%s': Writing Float Truncated value '%f' as '%d'"),
                                *StatName,
                                *TargetUserIdEOS->ToString(),
                                DoubleTrunc,
                                Value);
                        }
                        else
                        {
                            UE_LOG(
                                LogEOS,
                                Error,
                                TEXT("Stat '%s' is configured as FloatTruncated in Project Settings, but non-float "
                                     "value provided during UpdateStats call. This stat will be ignored."),
                                *StatName);
                            continue;
                        }
                    }
                    else if (StatType == EStatTypingRule::FloatEncoded)
                    {
                        float FloatEnc;
                        if (StatVal.GetType() == EOnlineKeyValuePairDataType::Float)
                        {
                            StatVal.GetValue(FloatEnc);
                        }
                        else if (StatVal.GetType() == EOnlineKeyValuePairDataType::Double)
                        {
                            double Temp;
                            StatVal.GetValue(Temp);
                            FloatEnc = (float)Temp;
                        }
                        else
                        {
                            UE_LOG(
                                LogEOS,
                                Error,
                                TEXT("Stat '%s' is configured as FloatEncoded in Project Settings, but non-float "
                                     "value provided during UpdateStats call. This stat will be ignored."),
                                *StatName);
                            continue;
                        }
                        int32 Value = *(int32 *)&FloatEnc;
                        GeneratedStats.Add(StatName, Value);
                        UE_LOG(
                            LogEOSStat,
                            Verbose,
                            TEXT("Stat '%s': User '%s': Writing Float Encoded value '%f' as '%d'"),
                            *StatName,
                            *TargetUserIdEOS->ToString(),
                            FloatEnc,
                            Value);
                    }
                    else if (StatType == EStatTypingRule::DoubleEncoded)
                    {
                        double DoubleEnc;
                        if (StatVal.GetType() == EOnlineKeyValuePairDataType::Float)
                        {
                            float Temp;
                            StatVal.GetValue(Temp);
                            DoubleEnc = Temp;
                        }
                        else if (StatVal.GetType() == EOnlineKeyValuePairDataType::Double)
                        {
                            StatVal.GetValue(DoubleEnc);
                        }
                        else
                        {
                            UE_LOG(
                                LogEOS,
                                Error,
                                TEXT("Stat '%s' is configured as DoubleEncoded in Project Settings, but non-float "
                                     "value provided during UpdateStats call. This stat will be ignored."),
                                *StatName);
                            continue;
                        }

                        uint64 RawVal = *(uint64 *)&DoubleEnc;
                        int32 Upper = (RawVal >> 32) & 0xFFFFFFFF;
                        int32 Lower = RawVal & 0xFFFFFFFF;
                        GeneratedStats.Add(FString::Printf(TEXT("%s_u"), *StatName), Upper);
                        GeneratedStats.Add(FString::Printf(TEXT("%s_l"), *StatName), Lower);
                        UE_LOG(
                            LogEOSStat,
                            Verbose,
                            TEXT("Stat '%s': User '%s': Writing Double Encoded value '%f' as '%d' (upper bits as '%s_u')"),
                            *StatName,
                            *TargetUserIdEOS->ToString(),
                            DoubleEnc,
                            Upper,
                            *StatName);
                        UE_LOG(
                            LogEOSStat,
                            Verbose,
                            TEXT("Stat '%s': User '%s': Writing Double Encoded value '%f' as '%d' (lower bits as '%s_l')"),
                            *StatName,
                            *TargetUserIdEOS->ToString(),
                            DoubleEnc,
                            Lower,
                            *StatName);
                    }
                    else
                    {
                        UE_LOG(
                            LogEOS,
                            Error,
                            TEXT("Stat '%s' has an unknown type in Project Settings. It will be ignored."),
                            *StatName);
                        continue;
                    }
                }

                TArray<FString> GeneratedStatsKeys;
                GeneratedStats.GetKeys(GeneratedStatsKeys);

                if (GeneratedStatsKeys.Num() == 0)
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("Error while submitting stats for %s: None of the stats you passed could be converted to "
                             "EOS stats based on the stat type configuration in Project Settings. This is treated as "
                             "an error, as submitting an array of invalid stats is not correct behaviour."),
                        *TargetUserIdEOS->ToString());
                    OnDone(false);
                    return true;
                }

                EOS_Stats_IngestStatOptions Opts = {};
                Opts.ApiVersion = EOS_STATS_INGESTSTAT_API_LATEST;
#if EOS_VERSION_AT_LEAST(1, 8, 0)
                Opts.LocalUserId = LocalUserIdEOS->GetProductUserId();
                Opts.TargetUserId = TargetUserIdEOS->GetProductUserId();
#else
                Opts.UserId = TargetUserIdEOS->GetProductUserId();
#endif
                Opts.StatsCount = GeneratedStatsKeys.Num();
                EOS_Stats_IngestData *ModifiableStats = (EOS_Stats_IngestData *)FMemory::MallocZeroed(
                    sizeof(EOS_Stats_IngestData) * GeneratedStatsKeys.Num());
                Opts.Stats = ModifiableStats;

                for (int32 i = 0; i < GeneratedStatsKeys.Num(); i++)
                {
                    FString Key = GeneratedStatsKeys[i];
                    int32_t Amount = GeneratedStats[Key];

                    ModifiableStats[i].ApiVersion = EOS_STATS_INGESTDATA_API_LATEST;
                    ModifiableStats[i].IngestAmount = Amount;
                    verify(
                        EOSString_Stats_StatName::AllocateToCharBuffer(Key, ModifiableStats[i].StatName) ==
                        EOS_EResult::EOS_Success);
                }

                EOSRunOperation<EOS_HStats, EOS_Stats_IngestStatOptions, EOS_Stats_IngestStatCompleteCallbackInfo>(
                    This->EOSStats,
                    &Opts,
                    EOS_Stats_IngestStat,
                    [WeakThis = GetWeakThis(This), OnDone, Opts, TargetUserIdEOS](
                        const EOS_Stats_IngestStatCompleteCallbackInfo *Data) {
                        for (uint32_t i = 0; i < Opts.StatsCount; i++)
                        {
                            const char *BufRef = Opts.Stats[i].StatName;
                            EOSString_Stats_StatName::FreeFromCharBuffer(BufRef);
                        }
                        FMemory::Free((void *)Opts.Stats);

                        if (auto This = PinWeakThis(WeakThis))
                        {
                            if (Data->ResultCode != EOS_EResult::EOS_Success)
                            {
                                UE_LOG(
                                    LogEOS,
                                    Error,
                                    TEXT("Error while submitting stats for %s: %s"),
                                    *TargetUserIdEOS->ToString(),
                                    *ConvertError(Data->ResultCode).ToLogString())
                            }

                            OnDone(Data->ResultCode == EOS_EResult::EOS_Success);
                        }
                    });
                return true;
            }

            return false;
        },
        [WeakThis = GetWeakThis(this), Delegate](const TArray<bool> &OutValues) {
            bool bAllSuccess = true;
            for (bool bSuccess : OutValues)
            {
                if (!bSuccess)
                {
                    bAllSuccess = false;
                    break;
                }
            }
            if (bAllSuccess)
            {
                Delegate.ExecuteIfBound(OnlineRedpointEOS::Errors::Success());
            }
            else
            {
                Delegate.ExecuteIfBound(OnlineRedpointEOS::Errors::InvalidRequest());
            }
        });
}

#if !UE_BUILD_SHIPPING
void FOnlineStatsInterfaceEOS::ResetStats(const TSharedRef<const FUniqueNetId> StatsUserId)
{
    UE_LOG(LogEOS, Error, TEXT("ResetStats is not supported in EOS."));
}
#endif

EOS_DISABLE_STRICT_WARNINGS
