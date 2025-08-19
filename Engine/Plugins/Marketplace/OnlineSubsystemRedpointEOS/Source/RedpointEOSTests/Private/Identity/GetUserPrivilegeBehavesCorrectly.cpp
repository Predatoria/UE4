// Copyright June Rhodes. All Rights Reserved.

#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineStatsInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlineIdentityInterface_GetUserPrivilegeBehavesCorrectly,
    "OnlineSubsystemEOS.OnlineIdentityInterface.GetUserPrivilegeBehavesCorrectly",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_OnlineIdentityInterface_GetUserPrivilegeBehavesCorrectly::RunAsyncTest(
    const std::function<void()> &OnDone)
{
    CreateSubsystemsForTest_CreateOnDemand(
        this,
        1,
        OnDone,
        [this](const TArray<FMultiplayerScenarioInstance> &Instances, const FOnDone &OnDone) {
            FMultiplayerScenarioInstance Instance = Instances[0];

            auto Identity = Instance.Subsystem.Pin()->GetIdentityInterface();
            TestTrue("Online subsystem provides IOnlineIdentity interface", Identity.IsValid());

            if (!Identity.IsValid())
            {
                OnDone();
                return;
            }

            int bDelegateInvokeCount = 0;
            Identity->GetUserPrivilege(
                *Instance.UserId,
                EUserPrivileges::Type::CanPlay,
                IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate::CreateLambda([&bDelegateInvokeCount](
                                                                                       const FUniqueNetId &LocalUserId,
                                                                                       EUserPrivileges::Type Privilege,
                                                                                       uint32 PrivilegeResult) {
                    bDelegateInvokeCount++;
                }));
            TestEqual("Delegate invocation count is 1", bDelegateInvokeCount, 1);

            if (bDelegateInvokeCount == 1)
            {
                Identity->GetUserPrivilege(
                    *Instance.UserId,
                    EUserPrivileges::Type::CanPlay,
                    IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate::CreateLambda(
                        [this](
                            const FUniqueNetId &LocalUserId,
                            EUserPrivileges::Type Privilege,
                            uint32 PrivilegeResult) {
                            this->TestEqual(
                                "Returns correct privilege in delegate",
                                Privilege,
                                EUserPrivileges::Type::CanPlay);
                            this->TestEqual(
                                "CanPlay privilege returns success",
                                PrivilegeResult,
                                (uint32)IOnlineIdentity::EPrivilegeResults::NoFailures);
                        }));
                Identity->GetUserPrivilege(
                    *Instance.UserId,
                    (EUserPrivileges::Type)1024,
                    IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate::CreateLambda(
                        [this](
                            const FUniqueNetId &LocalUserId,
                            EUserPrivileges::Type Privilege,
                            uint32 PrivilegeResult) {
                            this->TestEqual(
                                "Returns correct privilege in delegate",
                                Privilege,
                                (EUserPrivileges::Type)1024);
                            this->TestEqual(
                                "Unknown privilege returns failure",
                                PrivilegeResult,
                                (uint32)IOnlineIdentity::EPrivilegeResults::GenericFailure);
                        }));
            }

            OnDone();
        });
}