// Copyright June Rhodes. All Rights Reserved.

#include "Containers/Ticker.h"
#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "Misc/Guid.h"
#include "RedpointEOSInterfaces/Private/Interfaces/OnlineLobbyInterface.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlineLobbyInterface_CanGetLobbyInterface,
    "OnlineSubsystemEOS.OnlineLobbyInterface.CanGetLobbyInterface",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_OnlineLobbyInterface_CanGetLobbyInterface::RunAsyncTest(const std::function<void()> &OnDone)
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

            auto LobbyInterface = Online::GetLobbyInterface(Subsystem.Get());
            TestTrue("Online subsystem provides IOnlineLobby interface", LobbyInterface.IsValid());

            OnDone();
        });
}