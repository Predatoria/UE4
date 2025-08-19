// Copyright June Rhodes. All Rights Reserved.

#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineStatsInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlineIdentityInterface_IdTokenAvailableIfSdkSupportsIt,
    "OnlineSubsystemEOS.OnlineIdentityInterface.IdTokenAvailableIfSdkSupportsIt",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_OnlineIdentityInterface_IdTokenAvailableIfSdkSupportsIt::RunAsyncTest(const FOnDone &OnDone)
{
#if !EOS_VERSION_AT_LEAST(1, 14, 0)
    OnDone();
    return;
#else
    CreateSubsystemsForTest_CreateOnDemand(
        this,
        1,
        OnDone,
        [this](TArray<FMultiplayerScenarioInstance> Instances, const FOnDone &OnDone) {
            FMultiplayerScenarioInstance Instance = Instances[0];

            auto Identity = Instance.Subsystem.Pin()->GetIdentityInterface();
            TestTrue("Online subsystem provides IOnlineIdentity interface", Identity.IsValid());

            if (!Identity.IsValid())
            {
                OnDone();
                return;
            }

            TestFalse("GetAuthToken must return value", Identity->GetAuthToken(0).IsEmpty());
            TestFalse(
                "GetAccessToken must return value",
                Identity->GetUserAccount(*Identity->GetUniquePlayerId(0))->GetAccessToken().IsEmpty());
            OnDone();
        });
#endif
}
