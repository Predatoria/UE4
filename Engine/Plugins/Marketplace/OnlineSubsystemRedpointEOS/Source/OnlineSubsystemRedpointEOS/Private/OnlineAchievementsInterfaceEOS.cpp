// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/OnlineAchievementsInterfaceEOS.h"

#include "Interfaces/OnlineStatsInterface.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

FOnlineAchievementsInterfaceEOS::FOnlineAchievementsInterfaceEOS(EOS_HPlatform InPlatform, IOnlineStatsPtr InStats)
    : EOSAchievements(EOS_Platform_GetAchievementsInterface(InPlatform))
    , Stats(MoveTemp(InStats))
    , CachedAchievementDescriptions()
    , CachedAchievementsByProductUserId()
    , Unregister_AchievementUnlockedReceived()
{
    check(this->EOSAchievements != nullptr);
    check(this->Stats != nullptr);
};

void FOnlineAchievementsInterfaceEOS::RegisterEvents()
{
    EOS_Achievements_AddNotifyAchievementsUnlockedV2Options Opts = {};
    Opts.ApiVersion = EOS_ACHIEVEMENTS_ADDNOTIFYACHIEVEMENTSUNLOCKEDV2_API_LATEST;

    this->Unregister_AchievementUnlockedReceived = EOSRegisterEvent<
        EOS_HAchievements,
        EOS_Achievements_AddNotifyAchievementsUnlockedV2Options,
        EOS_Achievements_OnAchievementsUnlockedCallbackV2Info>(
        this->EOSAchievements,
        &Opts,
        EOS_Achievements_AddNotifyAchievementsUnlockedV2,
        EOS_Achievements_RemoveNotifyAchievementsUnlocked,
        [WeakThis = GetWeakThis(this)](const EOS_Achievements_OnAchievementsUnlockedCallbackV2Info *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                TSharedRef<const FUniqueNetIdEOS> PlayerId = MakeShared<FUniqueNetIdEOS>(Data->UserId);
                This->TriggerOnAchievementUnlockedDelegates(
                    *PlayerId,
                    EOSString_Achievements_AchievementId::FromAnsiString(Data->AchievementId));
            }
        });
}

void FOnlineAchievementsInterfaceEOS::WriteAchievements(
    const FUniqueNetId &PlayerId,
    FOnlineAchievementsWriteRef &WriteObject,
    const FOnAchievementsWrittenDelegate &Delegate)
{
    if (PlayerId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("WriteAchievements: PlayerId was invalid (not an EOS user)"));
        Delegate.ExecuteIfBound(PlayerId, false);
        return;
    }

    TSharedRef<const FUniqueNetIdEOS> LocalUserIdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(PlayerId.AsShared());

    FOnlineStatsUserUpdatedStats UpdatedStat = FOnlineStatsUserUpdatedStats(LocalUserIdEOS);
    for (const auto &Prop : WriteObject->Properties)
    {
        UpdatedStat.Stats.Add(
            Prop.Key.ToString(),
            FOnlineStatUpdate(Prop.Value, FOnlineStatUpdate::EOnlineStatModificationType::Unknown));
    }

    TArray<FOnlineStatsUserUpdatedStats> UpdatedStats;
    UpdatedStats.Add(UpdatedStat);

    WriteObject->WriteState = EOnlineAsyncTaskState::InProgress;

    this->Stats->UpdateStats(
        LocalUserIdEOS,
        UpdatedStats,
        FOnlineStatsUpdateStatsComplete::CreateLambda(
            [Delegate, LocalUserIdEOS, WriteObject](const FOnlineError &ResultState) {
            if (ResultState.bSucceeded)
            {
                WriteObject->WriteState = EOnlineAsyncTaskState::Done;
            }
            else
            {
                WriteObject->WriteState = EOnlineAsyncTaskState::Failed;
            }
            Delegate.ExecuteIfBound(*LocalUserIdEOS, ResultState.bSucceeded);
        }));
}

void FOnlineAchievementsInterfaceEOS::QueryAchievements(
    const FUniqueNetId &PlayerId,
    const FOnQueryAchievementsCompleteDelegate &Delegate)
{
    if (PlayerId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("QueryAchievements: PlayerId was invalid (not an EOS user)"));
        Delegate.ExecuteIfBound(PlayerId, false);
        return;
    }

    TSharedRef<const FUniqueNetIdEOS> LocalUserIdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(PlayerId.AsShared());

    EOS_Achievements_QueryPlayerAchievementsOptions Opts = {};
    Opts.ApiVersion = EOS_ACHIEVEMENTS_QUERYPLAYERACHIEVEMENTS_API_LATEST;
#if EOS_VERSION_AT_LEAST(1, 11, 0)
    Opts.LocalUserId = LocalUserIdEOS->GetProductUserId();
    Opts.TargetUserId = LocalUserIdEOS->GetProductUserId();
#else
    Opts.UserId = LocalUserIdEOS->GetProductUserId();
#endif
    EOSRunOperation<
        EOS_HAchievements,
        EOS_Achievements_QueryPlayerAchievementsOptions,
        EOS_Achievements_OnQueryPlayerAchievementsCompleteCallbackInfo>(
        this->EOSAchievements,
        &Opts,
        EOS_Achievements_QueryPlayerAchievements,
        [WeakThis = GetWeakThis(this), Delegate, LocalUserIdEOS](
            const EOS_Achievements_OnQueryPlayerAchievementsCompleteCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Data->ResultCode != EOS_EResult::EOS_Success)
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("QueryAchievements: Operation failed with result code %s"),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
                    Delegate.ExecuteIfBound(*LocalUserIdEOS, false);
                    return;
                }

                TMap<FAchievementId, FOnlineAchievement> Achievements;

                EOS_Achievements_GetPlayerAchievementCountOptions CountOpts = {};
                CountOpts.ApiVersion = EOS_ACHIEVEMENTS_GETPLAYERACHIEVEMENTCOUNT_API_LATEST;
                CountOpts.UserId = LocalUserIdEOS->GetProductUserId();
                uint32_t Count = EOS_Achievements_GetPlayerAchievementCount(This->EOSAchievements, &CountOpts);

                for (uint32_t i = 0; i < Count; i++)
                {
                    EOS_Achievements_PlayerAchievement *AchievementRaw = nullptr;

                    EOS_Achievements_CopyPlayerAchievementByIndexOptions CopyOpts = {};
                    CopyOpts.ApiVersion = EOS_ACHIEVEMENTS_COPYPLAYERACHIEVEMENTBYINDEX_API_LATEST;
#if EOS_VERSION_AT_LEAST(1, 11, 0)
                    CopyOpts.LocalUserId = LocalUserIdEOS->GetProductUserId();
                    CopyOpts.TargetUserId = LocalUserIdEOS->GetProductUserId();
#else
                    CopyOpts.UserId = LocalUserIdEOS->GetProductUserId();
#endif
                    CopyOpts.AchievementIndex = i;
                    verify(
                        EOS_Achievements_CopyPlayerAchievementByIndex(
                            This->EOSAchievements,
                            &CopyOpts,
                            &AchievementRaw) == EOS_EResult::EOS_Success);

                    FOnlineAchievement Achievement;
                    Achievement.Id =
                        EOSString_Achievements_AchievementId::FromAnsiString(AchievementRaw->AchievementId);
                    Achievement.Progress = AchievementRaw->Progress * 100.0f;

                    EOS_Achievements_PlayerAchievement_Release(AchievementRaw);

                    Achievements.Add(Achievement.Id, Achievement);
                }

                This->CachedAchievementsByProductUserId.Add(*LocalUserIdEOS, Achievements);

                Delegate.ExecuteIfBound(*LocalUserIdEOS, true);
            }
        });
}

void FOnlineAchievementsInterfaceEOS::QueryAchievementDescriptions(
    const FUniqueNetId &PlayerId,
    const FOnQueryAchievementsCompleteDelegate &Delegate)
{
    if (PlayerId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("QueryAchievementDescriptions: PlayerId was invalid (not an EOS user)"));
        Delegate.ExecuteIfBound(PlayerId, false);
        return;
    }

    TSharedRef<const FUniqueNetIdEOS> LocalUserIdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(PlayerId.AsShared());

    EOS_Achievements_QueryDefinitionsOptions Opts = {};
    Opts.ApiVersion = EOS_ACHIEVEMENTS_QUERYDEFINITIONS_API_LATEST;
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    Opts.LocalUserId = LocalUserIdEOS->GetProductUserId();
#else
    Opts.UserId = LocalUserIdEOS->GetProductUserId();
#endif
    EOSRunOperation<
        EOS_HAchievements,
        EOS_Achievements_QueryDefinitionsOptions,
        EOS_Achievements_OnQueryDefinitionsCompleteCallbackInfo>(
        this->EOSAchievements,
        &Opts,
        EOS_Achievements_QueryDefinitions,
        [WeakThis = GetWeakThis(this), Delegate, LocalUserIdEOS](
            const EOS_Achievements_OnQueryDefinitionsCompleteCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                TMap<FAchievementId, FOnlineAchievementDesc> AchievementDescriptions;

                EOS_Achievements_GetAchievementDefinitionCountOptions CountOpts = {};
                CountOpts.ApiVersion = EOS_ACHIEVEMENTS_GETACHIEVEMENTDEFINITIONCOUNT_API_LATEST;
                int32_t Count = EOS_Achievements_GetAchievementDefinitionCount(This->EOSAchievements, &CountOpts);
                for (int32_t i = 0; i < Count; i++)
                {
                    EOS_Achievements_DefinitionV2 *AchievementDefinitionRaw = nullptr;

                    EOS_Achievements_CopyAchievementDefinitionV2ByIndexOptions CopyOpts = {};
                    CopyOpts.ApiVersion = EOS_ACHIEVEMENTS_COPYDEFINITIONV2BYINDEX_API_LATEST;
                    CopyOpts.AchievementIndex = i;
                    verify(
                        EOS_Achievements_CopyAchievementDefinitionV2ByIndex(
                            This->EOSAchievements,
                            &CopyOpts,
                            &AchievementDefinitionRaw) == EOS_EResult::EOS_Success);

                    FAchievementId AchievementId =
                        EOSString_Achievements_AchievementId::FromAnsiString(AchievementDefinitionRaw->AchievementId);
                    FOnlineAchievementDesc AchievementDesc;
                    AchievementDesc.Title =
                        FText::FromString(UTF8_TO_TCHAR(AchievementDefinitionRaw->UnlockedDisplayName));
                    AchievementDesc.LockedDesc =
                        FText::FromString(UTF8_TO_TCHAR(AchievementDefinitionRaw->LockedDescription));
                    AchievementDesc.UnlockedDesc =
                        FText::FromString(UTF8_TO_TCHAR(AchievementDefinitionRaw->UnlockedDescription));
                    AchievementDesc.bIsHidden = AchievementDefinitionRaw->bIsHidden == EOS_TRUE;

                    EOS_Achievements_DefinitionV2_Release(AchievementDefinitionRaw);

                    AchievementDescriptions.Add(AchievementId, AchievementDesc);
                }

                This->CachedAchievementDescriptions = AchievementDescriptions;

                Delegate.ExecuteIfBound(*LocalUserIdEOS, Data->ResultCode == EOS_EResult::EOS_Success);
            }
        });
}

EOnlineCachedResult::Type FOnlineAchievementsInterfaceEOS::GetCachedAchievement(
    const FUniqueNetId &PlayerId,
    const FString &AchievementId,
    FOnlineAchievement &OutAchievement)
{
    if (PlayerId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("GetCachedAchievements: PlayerId was invalid (not an EOS user)"));
        return EOnlineCachedResult::NotFound;
    }

    if (this->CachedAchievementsByProductUserId.Contains(PlayerId))
    {
        if (this->CachedAchievementsByProductUserId[PlayerId].Contains(AchievementId))
        {
            OutAchievement = this->CachedAchievementsByProductUserId[PlayerId][AchievementId];
            return EOnlineCachedResult::Success;
        }
    }

    return EOnlineCachedResult::NotFound;
}

EOnlineCachedResult::Type FOnlineAchievementsInterfaceEOS::GetCachedAchievements(
    const FUniqueNetId &PlayerId,
    TArray<FOnlineAchievement> &OutAchievements)
{
    if (PlayerId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("GetCachedAchievements: PlayerId was invalid (not an EOS user)"));
        return EOnlineCachedResult::NotFound;
    }

    if (this->CachedAchievementsByProductUserId.Contains(PlayerId))
    {
        OutAchievements.Empty();
        for (const auto &KV : this->CachedAchievementsByProductUserId[PlayerId])
        {
            OutAchievements.Add(KV.Value);
        }
        return EOnlineCachedResult::Success;
    }

    return EOnlineCachedResult::NotFound;
}

EOnlineCachedResult::Type FOnlineAchievementsInterfaceEOS::GetCachedAchievementDescription(
    const FString &AchievementId,
    FOnlineAchievementDesc &OutAchievementDesc)
{
    if (this->CachedAchievementDescriptions.Contains(AchievementId))
    {
        OutAchievementDesc = this->CachedAchievementDescriptions[AchievementId];
        return EOnlineCachedResult::Success;
    }

    return EOnlineCachedResult::NotFound;
}

#if !UE_BUILD_SHIPPING
bool FOnlineAchievementsInterfaceEOS::ResetAchievements(const FUniqueNetId &PlayerId)
{
    UE_LOG(LogEOS, Error, TEXT("ResetAchievements is not supported by EOS."));
    return false;
}
#endif

EOS_DISABLE_STRICT_WARNINGS