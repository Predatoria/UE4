// Copyright June Rhodes. All Rights Reserved.

#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineLeaderboardsInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlineLeaderboardsInterface_CanQueryGlobalLeaderboardsInParallel,
    "OnlineSubsystemEOS.OnlineLeaderboardsInterface.CanQueryGlobalLeaderboardsInParallel",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_OnlineLeaderboardsInterface_CanQueryGlobalLeaderboardsInParallel::RunAsyncTest(
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

            FOnlineLeaderboardReadRef ReadRef1 = MakeShared<FOnlineLeaderboardRead, ESPMode::ThreadSafe>();
            ReadRef1->LeaderboardName = FName(TEXT("TestScore"));

            FOnlineLeaderboardReadRef ReadRef2 = MakeShared<FOnlineLeaderboardRead, ESPMode::ThreadSafe>();
            ReadRef2->LeaderboardName = FName(TEXT("TestScore2"));

            int *Count = new int(0);

            auto CancelCallback1 = RegisterOSSCallback(
                this,
                Leaderboards,
                &IOnlineLeaderboards::AddOnLeaderboardReadCompleteDelegate_Handle,
                &IOnlineLeaderboards::ClearOnLeaderboardReadCompleteDelegate_Handle,
                // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                std::function<void(bool)>([this, OnDone, Leaderboards, ReadRef1, Count](bool bWasSuccessful) {
                    TestTrue("(1) ReadLeaderboardsAroundRank operation succeeded", bWasSuccessful);

                    (*Count)--;
                    if (*Count == 0)
                    {
                        delete Count;
                        OnDone();
                    }
                }));
            (*Count)++;
            if (!Leaderboards->ReadLeaderboardsAroundRank(0, 100, ReadRef1))
            {
                CancelCallback1();
                TestTrue("(1) ReadLeaderboardsAroundRank operation started", false);
                (*Count)--;
                if (*Count == 0)
                {
                    delete Count;
                    OnDone();
                    return;
                }
            }

            auto CancelCallback2 = RegisterOSSCallback(
                this,
                Leaderboards,
                &IOnlineLeaderboards::AddOnLeaderboardReadCompleteDelegate_Handle,
                &IOnlineLeaderboards::ClearOnLeaderboardReadCompleteDelegate_Handle,
                // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                std::function<void(bool)>([this, OnDone, Leaderboards, ReadRef2, Count](bool bWasSuccessful) {
                    TestTrue("(2) ReadLeaderboardsAroundRank operation succeeded", bWasSuccessful);

                    (*Count)--;
                    if (*Count == 0)
                    {
                        delete Count;
                        OnDone();
                    }
                }));
            (*Count)++;
            if (!Leaderboards->ReadLeaderboardsAroundRank(0, 100, ReadRef2))
            {
                CancelCallback1();
                TestTrue("(2) ReadLeaderboardsAroundRank operation started", false);
                (*Count)--;
                if (*Count == 0)
                {
                    delete Count;
                    OnDone();
                }
            }
        });
}