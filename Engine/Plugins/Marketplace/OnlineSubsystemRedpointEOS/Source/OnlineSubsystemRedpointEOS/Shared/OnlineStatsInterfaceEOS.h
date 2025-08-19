// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EOSCommon.h"
#include "Interfaces/OnlineStatsInterface.h"
#include "Internationalization/Regex.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "UniqueNetIdEOS.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FOnlineStatsInterfaceEOS
    : public IOnlineStats,
      public TSharedFromThis<FOnlineStatsInterfaceEOS, ESPMode::ThreadSafe>
{
private:
    EOS_HStats EOSStats;
    TUserIdMap<TSharedPtr<const FOnlineStatsUserStats>> StatsCache;
    TArray<TTuple<FRegexPattern, TPair<FString, EStatTypingRule>>> CachedStatRules;
    TMap<FString, EStatTypingRule> CachedStatTypes;
    FName InstanceName;
    TSharedRef<class FEOSConfig> Config;

    EStatTypingRule GetTypeForStat(const FString &StatName);

public:
    FOnlineStatsInterfaceEOS(
        EOS_HPlatform InPlatform,
        const TSharedRef<FEOSConfig> &InConfig,
        const FName &InInstanceName);
    UE_NONCOPYABLE(FOnlineStatsInterfaceEOS);

    virtual void QueryStats(
        const TSharedRef<const FUniqueNetId> LocalUserId,
        const TSharedRef<const FUniqueNetId> StatsUser,
        const FOnlineStatsQueryUserStatsComplete &Delegate) override;
    virtual void QueryStats(
        const TSharedRef<const FUniqueNetId> LocalUserId,
        const TArray<TSharedRef<const FUniqueNetId>> &StatUsers,
        const TArray<FString> &StatNames,
        const FOnlineStatsQueryUsersStatsComplete &Delegate) override;
    virtual TSharedPtr<const FOnlineStatsUserStats> GetStats(
        const TSharedRef<const FUniqueNetId> StatsUserId) const override;
    virtual void UpdateStats(
        const TSharedRef<const FUniqueNetId> LocalUserId,
        const TArray<FOnlineStatsUserUpdatedStats> &UpdatedUserStats,
        const FOnlineStatsUpdateStatsComplete &Delegate) override;

#if !UE_BUILD_SHIPPING
    virtual void ResetStats(const TSharedRef<const FUniqueNetId> StatsUserId) override;
#endif
};

EOS_DISABLE_STRICT_WARNINGS