// Copyright June Rhodes. All Rights Reserved.

#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineLeaderboardsInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlineLeaderboardsInterface_CanQueryGlobalLeaderboards,
    "OnlineSubsystemEOS.OnlineLeaderboardsInterface.CanQueryGlobalLeaderboards",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_OnlineLeaderboardsInterface_CanQueryGlobalLeaderboards::RunAsyncTest(
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

            auto Leaderboards = Subsystem->GetLeaderboardsInterface();
            TestTrue("Online subsystem provides IOnlineLeaderboards interface", Leaderboards.IsValid());

            if (!Leaderboards.IsValid())
            {
                OnDone();
                return;
            }

            FOnlineLeaderboardReadRef ReadRef = MakeShared<FOnlineLeaderboardRead, ESPMode::ThreadSafe>();
            ReadRef->LeaderboardName = FName(TEXT("TestScore"));

            auto CancelCallback = RegisterOSSCallback(
                this,
                Leaderboards,
                &IOnlineLeaderboards::AddOnLeaderboardReadCompleteDelegate_Handle,
                &IOnlineLeaderboards::ClearOnLeaderboardReadCompleteDelegate_Handle,
                // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                std::function<void(bool)>([this, OnDone, Leaderboards, ReadRef](bool bWasSuccessful) {
                    TestTrue("ReadLeaderboardsAroundRank operation succeeded", bWasSuccessful);

                    // We *SHOULD* have at least one row in the global leaderboard, given that we've
                    // pre-populated it with data.
                    TestTrue("Global leaderboard has at least one row", ReadRef->Rows.Num() > 0);

                    if (ReadRef->Rows.Num() > 0)
                    {
                        auto FirstRow = ReadRef->Rows[0];

                        // If we have at least one row, test that it has the expected columns.
                        TestEqual("First row has rank 1", FirstRow.Rank, 1);
                        TestTrue("First row has player ID", FirstRow.PlayerId.IsValid());
                        TestTrue("First row has score column", FirstRow.Columns.Contains(TEXT("Score")));
                    }

                    OnDone();
                }));
            if (!Leaderboards->ReadLeaderboardsAroundRank(0, 100, ReadRef))
            {
                CancelCallback();
                TestTrue("ReadLeaderboardsAroundRank operation started", false);
                OnDone();
            }
        });
}