// Copyright June Rhodes. All Rights Reserved.

#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineStatsInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlineStatsInterface_BackfillsStatsThatDoNotExist,
    "OnlineSubsystemEOS.OnlineStatsInterface.BackfillsStatsThatDoNotExist",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_OnlineStatsInterface_BackfillsStatsThatDoNotExist::RunAsyncTest(
    const std::function<void()> &OnDone)
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
            TArray<TSharedRef<const FUniqueNetId>> UsersToQuery;
            UsersToQuery.Add(UserId.ToSharedRef());

            TArray<FString> StatNames;
            StatNames.Add(TEXT("ThisStatIsNeverWrittenTo"));

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
                        TestEqual("QueryStats: One row returned for one stat query", UserStatsResult->Stats.Num(), 1);
                        TestEqual(
                            "QueryStats: Row is associated with correct user",
                            UserStatsResult->Account->ToString(),
                            UserId->ToString());
                        TestTrue(
                            "QueryStats: StatQuery contains ThisStatIsNeverWrittenTo row",
                            UserStatsResult->Stats.Contains(TEXT("ThisStatIsNeverWrittenTo")));
                        if (UserStatsResult->Stats.Contains(TEXT("ThisStatIsNeverWrittenTo")))
                        {
                            TestTrue(
                                "QueryStats: ThisStatIsNeverWrittenTo value is empty",
                                UserStatsResult->Stats[TEXT("ThisStatIsNeverWrittenTo")].GetType() ==
                                    EOnlineKeyValuePairDataType::Empty);
                        }

                        TSharedPtr<const FOnlineStatsUserStats> FetchedStats = Stats->GetStats(UserId.ToSharedRef());
                        TestTrue("GetStats: Can get stats through GetStats after query", FetchedStats.IsValid());
                        if (FetchedStats.IsValid())
                        {
                            TestEqual("GetStats: One row returned for two stat query", FetchedStats->Stats.Num(), 1);
                            TestTrue(
                                "GetStats: StatQuery contains ThisStatIsNeverWrittenTo row",
                                FetchedStats->Stats.Contains(TEXT("ThisStatIsNeverWrittenTo")));
                            if (FetchedStats->Stats.Contains(TEXT("ThisStatIsNeverWrittenTo")))
                            {
                                TestTrue(
                                    "GetStats: ThisStatIsNeverWrittenTo value is empty",
                                    FetchedStats->Stats[TEXT("ThisStatIsNeverWrittenTo")].GetType() ==
                                        EOnlineKeyValuePairDataType::Empty);
                            }
                        }

                        OnDone();
                    }));
        });
}