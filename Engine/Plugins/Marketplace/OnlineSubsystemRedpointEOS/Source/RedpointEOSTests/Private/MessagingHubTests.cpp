// Copyright June Rhodes. All Rights Reserved.

#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSMessagingHub.h"
#include "OnlineSubsystemRedpointEOS/Shared/MultiOperation.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_MessagingHubTest,
    "OnlineSubsystemEOS.MessagingHubTest",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_MessagingHubTest::RunAsyncTest(const std::function<void()> &OnDone)
{
    CreateSubsystemsForTest_CreateOnDemand(
        this,
        2,
        OnDone,
        [this](const TArray<FMultiplayerScenarioInstance> &Instances, const FOnDone &OnDone) {
            auto Host = StaticCastSharedPtr<FOnlineSubsystemEOS>(Instances[0].Subsystem.Pin())->MessagingHub;
            auto Client = StaticCastSharedPtr<FOnlineSubsystemEOS>(Instances[1].Subsystem.Pin())->MessagingHub;

            auto Count = MakeShared<int32>(0);

            Client->OnMessageReceived().AddLambda([this, Instances, Count, OnDone](
                                                      const FUniqueNetIdEOS &SenderId,
                                                      const FUniqueNetIdEOS &TargetId,
                                                      const FString &Type,
                                                      const FString &Message) {
                TestEqual("Sender is host", SenderId, *Instances[0].UserId);
                TestEqual("Receiver is host", TargetId, *Instances[1].UserId);
                TestEqual("Type is AutomationTest", Type, "AutomationTest");
                TestEqual("Message is ExpectedValue", Message, "ExpectedValue");
                *Count += 1;
                if (*Count == 2)
                {
                    OnDone();
                }
            });

            Host->SendMessage(
                *Instances[0].UserId,
                *Instances[1].UserId,
                "AutomationTest",
                "ExpectedValue",
                FOnEOSHubMessageSent::CreateLambda([this, Count, OnDone](bool bWasReceived) {
                    TestTrue("Did get marked as received", bWasReceived);
                    if (bWasReceived)
                    {
                        *Count += 1;
                        if (*Count == 2)
                        {
                            OnDone();
                        }
                    }
                    else
                    {
                        // Since we won't get the OnMessageReceived event.
                        OnDone();
                    }
                }));
        });
}