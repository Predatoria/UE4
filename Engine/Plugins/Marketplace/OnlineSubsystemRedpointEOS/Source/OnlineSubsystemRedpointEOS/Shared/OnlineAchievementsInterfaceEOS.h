// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EOSCommon.h"
#include "Interfaces/OnlineAchievementsInterface.h"
#include "OnlineSubsystem.h"
#include "UniqueNetIdEOS.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FOnlineAchievementsInterfaceEOS
    : public IOnlineAchievements,
      public TSharedFromThis<FOnlineAchievementsInterfaceEOS, ESPMode::ThreadSafe>
{
    friend class FOnlineSubsystemEOS;

private:
    typedef FString FAchievementId;

    EOS_HAchievements EOSAchievements;
    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
    IOnlineStatsPtr Stats;
    TMap<FAchievementId, FOnlineAchievementDesc> CachedAchievementDescriptions;
    TUserIdMap<TMap<FAchievementId, FOnlineAchievement>> CachedAchievementsByProductUserId;
    TSharedPtr<EOSEventHandle<EOS_Achievements_OnAchievementsUnlockedCallbackV2Info>>
        Unregister_AchievementUnlockedReceived;

    void RegisterEvents();

public:
    FOnlineAchievementsInterfaceEOS(EOS_HPlatform InPlatform, IOnlineStatsPtr InStats);
    UE_NONCOPYABLE(FOnlineAchievementsInterfaceEOS);
    virtual ~FOnlineAchievementsInterfaceEOS(){};

    virtual void WriteAchievements(
        const FUniqueNetId &PlayerId,
        FOnlineAchievementsWriteRef &WriteObject,
        const FOnAchievementsWrittenDelegate &Delegate = FOnAchievementsWrittenDelegate()) override;
    virtual void QueryAchievements(
        const FUniqueNetId &PlayerId,
        const FOnQueryAchievementsCompleteDelegate &Delegate = FOnQueryAchievementsCompleteDelegate()) override;
    virtual void QueryAchievementDescriptions(
        const FUniqueNetId &PlayerId,
        const FOnQueryAchievementsCompleteDelegate &Delegate = FOnQueryAchievementsCompleteDelegate()) override;
    virtual EOnlineCachedResult::Type GetCachedAchievement(
        const FUniqueNetId &PlayerId,
        const FString &AchievementId,
        FOnlineAchievement &OutAchievement) override;
    virtual EOnlineCachedResult::Type GetCachedAchievements(
        const FUniqueNetId &PlayerId,
        TArray<FOnlineAchievement> &OutAchievements) override;
    virtual EOnlineCachedResult::Type GetCachedAchievementDescription(
        const FString &AchievementId,
        FOnlineAchievementDesc &OutAchievementDesc) override;

#if !UE_BUILD_SHIPPING
    virtual bool ResetAchievements(const FUniqueNetId &PlayerId) override;
#endif
};

EOS_DISABLE_STRICT_WARNINGS