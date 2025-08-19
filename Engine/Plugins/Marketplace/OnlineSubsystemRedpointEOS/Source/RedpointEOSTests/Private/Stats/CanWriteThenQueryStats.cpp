// Copyright June Rhodes. All Rights Reserved.

#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineStatsInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlineStatsInterface_CanWriteThenQueryStats,
    "OnlineSubsystemEOS.OnlineStatsInterface.CanWriteThenQueryStats",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_OnlineStatsInterface_CanWriteThenQueryStats::RunAsyncTest(const std::function<void()> &OnDone)
{
    CreateSingleSubsystemForTest_CreateOnDemand(
        this,
        OnDone,
        [this](
            const IOnlineSubsystemPtr &Subsystem,
            const TSharedPtr<const FUniqueNetIdEOS> &UserId,
            const FOnDone &OnDone) {
            if (!(Subsystem.IsValid() && UserId.IsValid()))
            {
                this->AddError(FString::Printf(TEXT("Unable to init subsystem / authenticate")));
                OnDone();
                return;
            }

            auto Stats = Subsystem->GetStatsInterface();
            TestTrue("Online subsystem provides IOnlineStats interface", Stats.IsValid());

            if (!Stats.IsValid())
            {
                OnDone();
                return;
            }

            FOnlineStatsUserUpdatedStats StatToSend = FOnlineStatsUserUpdatedStats(UserId.ToSharedRef());
            StatToSend.Stats.Add(
                TEXT("TestLatest"),
                FOnlineStatUpdate(20, FOnlineStatUpdate::EOnlineStatModificationType::Set));
            StatToSend.Stats.Add(
                TEXT("TestScore"),
                FOnlineStatUpdate(10, FOnlineStatUpdate::EOnlineStatModificationType::Set));
            StatToSend.Stats.Add(
                TEXT("TestFloatEnc"),
                FOnlineStatUpdate(20.0f, FOnlineStatUpdate::EOnlineStatModificationType::Set));

            TArray<FOnlineStatsUserUpdatedStats> StatsToSend;
            StatsToSend.Add(StatToSend);

            Stats->UpdateStats(
                UserId.ToSharedRef(),
                StatsToSend,
                FOnlineStatsUpdateStatsComplete::CreateLambda(
                    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                    [this, OnDone, Stats, UserId](const FOnlineError &ResultState) {
                        TestTrue("Can write stats update", ResultState.bSucceeded);
                        if (!ResultState.bSucceeded)
                        {
                            OnDone();
                            return;
                        }

                        TArray<TSharedRef<const FUniqueNetId>> UsersToQuery;
                        UsersToQuery.Add(UserId.ToSharedRef());

                        TArray<FString> StatNames;
                        StatNames.Add(TEXT("TestLatest"));
                        StatNames.Add(TEXT("TestScore"));
                        StatNames.Add(TEXT("TestFloatEnc"));

                        Stats->QueryStats(
                            UserId.ToSharedRef(),
                            UsersToQuery,
                            StatNames,
                            FOnlineStatsQueryUsersStatsComplete::CreateLambda(
                                // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                                [this, OnDone, UserId, Stats](
                                    const FOnlineError &ResultState,
                                    const TArray<TSharedRef<const FOnlineStatsUserStats>> &UsersStatsResult) {
                                    TestTrue("QueryStats operation succeeded", ResultState.bSucceeded);
                                    if (!ResultState.bSucceeded)
                                    {
                                        OnDone();
                                        return;
                                    }

                                    TestEqual(
                                        "One record returned for single user query from QueryStats",
                                        UsersStatsResult.Num(),
                                        1);
                                    if (UsersStatsResult.Num() != 1)
                                    {
                                        OnDone();
                                        return;
                                    }

                                    TSharedRef<const FOnlineStatsUserStats> UserStatsResult = UsersStatsResult[0];
                                    TestEqual(
                                        "QueryStats: 3 rows returned for two stat query",
                                        UserStatsResult->Stats.Num(),
                                        3);
                                    TestEqual(
                                        "QueryStats: Row is associated with correct user",
                                        UserStatsResult->Account->ToString(),
                                        UserId->ToString());
                                    TestTrue(
                                        "QueryStats: StatQuery contains TestLatest row",
                                        UserStatsResult->Stats.Contains(TEXT("TestLatest")));
                                    TestTrue(
                                        "QueryStats: StatQuery contains TestScore row",
                                        UserStatsResult->Stats.Contains(TEXT("TestScore")));
                                    TestTrue(
                                        "QueryStats: StatQuery contains TestFloatEnc row",
                                        UserStatsResult->Stats.Contains(TEXT("TestFloatEnc")));

                                    TSharedPtr<const FOnlineStatsUserStats> FetchedStats =
                                        Stats->GetStats(UserId.ToSharedRef());
                                    TestTrue(
                                        "GetStats: Can get stats through GetStats after query",
                                        FetchedStats.IsValid());
                                    if (FetchedStats.IsValid())
                                    {
                                        TestEqual(
                                            "GetStats: 3 rows returned for 3 stat query",
                                            FetchedStats->Stats.Num(),
                                            3);
                                        TestEqual(
                                            "GetStats: Row is associated with correct user",
                                            FetchedStats->Account->ToString(),
                                            UserId->ToString());
                                        TestTrue(
                                            "GetStats: StatQuery contains TestLatest row",
                                            FetchedStats->Stats.Contains(TEXT("TestLatest")));
                                        TestTrue(
                                            "GetStats: StatQuery contains TestScore row",
                                            FetchedStats->Stats.Contains(TEXT("TestScore")));
                                        TestTrue(
                                            "GetStats: StatQuery contains TestFloatEnc row",
                                            FetchedStats->Stats.Contains(TEXT("TestFloatEnc")));
                                    }

                                    // NOTE: We can't test the actual values in the query because the stats interface is
                                    // eventually consistent, and so the values might not have updated yet. All we can
                                    // test is that the EOS plugin correctly returns rows for the queried values.

                                    OnDone();
                                }));
                    }));
        });
}