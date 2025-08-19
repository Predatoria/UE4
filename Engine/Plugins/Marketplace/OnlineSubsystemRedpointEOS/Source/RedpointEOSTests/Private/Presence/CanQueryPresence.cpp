// Copyright June Rhodes. All Rights Reserved.

#include "HAL/MemoryMisc.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlinePresenceInterface_CanQueryPresence,
    "OnlineSubsystemEOS.OnlinePresenceInterface.CanQueryPresence",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_OnlinePresenceInterface_CanQueryPresence::RunAsyncTest(const std::function<void()> &OnDone)
{
    CreateSubsystemsForTest_EpicGames(
        this,
        EEpicGamesAccountCount::One,
        OnDone,
        [this](const TArray<FMultiplayerScenarioInstance> &Instances, const FOnDone &OnDone) {
            auto Host = Instances[0];

            auto Presence = Host.Subsystem.Pin()->GetPresenceInterface();
            TestTrue("Online subsystem provides IOnlinePresence interface", Presence.IsValid());

            if (!Presence.IsValid())
            {
                OnDone();
                return;
            }

            Presence->QueryPresence(
                *Host.UserId,
                IOnlinePresence::FOnPresenceTaskCompleteDelegate::CreateLambda(
                    [this, OnDone, Host](const class FUniqueNetId &UserId, const bool bWasSuccessful) {
                        TestTrue(TEXT("Presence was queried"), bWasSuccessful);
                        TestTrue(TEXT("User id matched expected value"), UserId == *Host.UserId);
                        OnDone();
                    }));
        });
}