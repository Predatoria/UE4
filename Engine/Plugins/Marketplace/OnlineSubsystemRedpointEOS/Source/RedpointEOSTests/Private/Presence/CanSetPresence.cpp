// Copyright June Rhodes. All Rights Reserved.

#include "HAL/MemoryMisc.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlinePresenceInterface_CanSetPresence,
    "OnlineSubsystemEOS.OnlinePresenceInterface.CanSetPresence",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_OnlinePresenceInterface_CanSetPresence::RunAsyncTest(const std::function<void()> &OnDone)
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

            FOnlineUserPresenceStatus Status;
            Status.State = EOnlinePresenceState::Online;
            Status.StatusStr = TEXT("This is a status update from an automated test");

            Presence->SetPresence(
                *Host.UserId,
                Status,
                IOnlinePresence::FOnPresenceTaskCompleteDelegate::CreateLambda(
                    [this, OnDone, Host](const class FUniqueNetId &UserId, const bool bWasSuccessful) {
                        TestTrue(TEXT("Status was updated"), bWasSuccessful);
                        TestTrue(TEXT("User id matched expected value"), UserId == *Host.UserId);
                        OnDone();
                    }));
        });
}