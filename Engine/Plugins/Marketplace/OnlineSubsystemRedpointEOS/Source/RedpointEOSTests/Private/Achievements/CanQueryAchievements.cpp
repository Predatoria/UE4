// Copyright June Rhodes. All Rights Reserved.

#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineAchievementsInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlineAchievementsInterface_CanQueryAchievements,
    "OnlineSubsystemEOS.OnlineAchievementsInterface.CanQueryAchievements",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_OnlineAchievementsInterface_CanQueryAchievements::RunAsyncTest(
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

            auto Achievements = Subsystem->GetAchievementsInterface();
            TestTrue("Online subsystem provides IOnlineAchievements interface", Achievements.IsValid());

            if (!Achievements.IsValid())
            {
                OnDone();
                return;
            }

            Achievements->QueryAchievements(
                *UserId,
                FOnQueryAchievementsCompleteDelegate::CreateLambda(
                    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                    [this, Achievements, OnDone](const FUniqueNetId &UserId, const bool bWasSuccessful) {
                        TestTrue("QueryAchievements was successful", bWasSuccessful);

                        FOnlineAchievement AchOut;
                        auto GotAch = Achievements->GetCachedAchievement(UserId, TEXT("TestAchievement"), AchOut);
                        TestEqual("GetCachedAchievement found achievement", GotAch, EOnlineCachedResult::Success);
                        if (GotAch == EOnlineCachedResult::Success)
                        {
                            TestEqual("TestAchievement has expected ID", AchOut.Id, TEXT("TestAchievement"));
                        }

                        OnDone();
                    }));
        });
}