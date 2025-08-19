// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EOSCommon.h"
#include "Interfaces/OnlineLeaderboardInterface.h"
#include "OnlineSubsystem.h"
#include "UniqueNetIdEOS.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FOnlineLeaderboardsInterfaceEOS
    : public IOnlineLeaderboards,
      public TSharedFromThis<FOnlineLeaderboardsInterfaceEOS, ESPMode::ThreadSafe>
{
private:
    EOS_HLeaderboards EOSLeaderboards;
    EOS_HConnect EOSConnect;
    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
    IOnlineStatsPtr Stats;
    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
    IOnlineIdentityPtr Identity;
#if EOS_HAS_AUTHENTICATION
    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
    IOnlineFriendsPtr Friends;
#endif // #if EOS_HAS_AUTHENTICATION

    // The EOS SDK does not cache leaderboard rank results per request,
    // so when developers request multiple global leaderboards, we have
    // to queue up the requests sequentially.
    struct FDeferredQueryLeaderboardRanksRequest
    {
        FDeferredQueryLeaderboardRanksRequest(
            int32 InRank,
            uint32 InRange,
            const FOnlineLeaderboardReadRef &InReadObject)
            : Rank(InRank)
            , Range(InRange)
            , ReadObject(InReadObject){};
        int32 Rank;
        uint32 Range;
        FOnlineLeaderboardReadRef ReadObject;
    };
    TArray<FDeferredQueryLeaderboardRanksRequest> DeferredRequests;
    bool bIsExecutingQueryLeaderboardRanks;
    void ProcessNextDeferredRequest();

    bool ReadLeaderboardsWithLocalUserContext(
        const TSharedRef<const FUniqueNetIdEOS> &QueryingUserId,
        const TArray<TSharedRef<const FUniqueNetId>> &Players,
        FOnlineLeaderboardReadRef &ReadObject);

public:
    FOnlineLeaderboardsInterfaceEOS(
        EOS_HPlatform InPlatform,
        IOnlineStatsPtr InStats,
        IOnlineIdentityPtr InIdentity
#if EOS_HAS_AUTHENTICATION
        ,
        IOnlineFriendsPtr InFriends
#endif // #if EOS_HAS_AUTHENTICATION
    );
    UE_NONCOPYABLE(FOnlineLeaderboardsInterfaceEOS);
    virtual ~FOnlineLeaderboardsInterfaceEOS(){};

    virtual bool ReadLeaderboards(
        const TArray<TSharedRef<const FUniqueNetId>> &Players,
        FOnlineLeaderboardReadRef &ReadObject) override;
    virtual bool ReadLeaderboardsForFriends(int32 LocalUserNum, FOnlineLeaderboardReadRef &ReadObject) override;
    virtual bool ReadLeaderboardsAroundRank(int32 Rank, uint32 Range, FOnlineLeaderboardReadRef &ReadObject) override;
    virtual bool ReadLeaderboardsAroundUser(
        TSharedRef<const FUniqueNetId> Player,
        uint32 Range,
        FOnlineLeaderboardReadRef &ReadObject) override;
    virtual void FreeStats(FOnlineLeaderboardRead &ReadObject) override;
    virtual bool WriteLeaderboards(
        const FName &SessionName,
        const FUniqueNetId &Player,
        FOnlineLeaderboardWrite &WriteObject) override;
    virtual bool FlushLeaderboards(const FName &SessionName) override;
    virtual bool WriteOnlinePlayerRatings(
        const FName &SessionName,
        int32 LeaderboardId,
        const TArray<FOnlinePlayerScore> &PlayerScores) override;
};

EOS_DISABLE_STRICT_WARNINGS