// Copyright June Rhodes. All Rights Reserved.

#include "ConnectToBeaconsInSequence_Common.h"
#include "EOSEditorCommands.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FConnectToBeaconsInSequence_OverIP,
    "OnlineSubsystemEOS.Networking.ConnectToBeaconsInSequence.OverIP",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);

bool FConnectToBeaconsInSequence_OverIP::RunTest(const FString &InParam)
{
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FSetupAutomationConfig());
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FStartPlayingEmptyMap(
        this,
        EEditorAuthMode::CreateOnDemand,
        3,
        EPlayNetMode::PIE_Standalone,
        TEXT("?NetMode=ForceIP")));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FWaitForBeaconPlayerControllers(this, 3));

    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(
        FMakePIEStartListeningServer(this, 0, TestHelpers::Port(9000), TEXT("NetMode=ForceIP")));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(
        FMakePIEStartListeningServer(this, 1, TestHelpers::Port(9001), TEXT("NetMode=ForceIP")));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FSpawnBeaconOnPIE(this, 0, TestHelpers::Port(9002), TEXT("NetMode=ForceIP")));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FSpawnBeaconOnPIE(this, 1, TestHelpers::Port(9003), TEXT("NetMode=ForceIP")));

    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FConnectClientPIEToHostPIEBeacon(this, 2, 0, TestHelpers::Port(9002)));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FWaitForClientBeaconToDestroyItselfAfterPing(this, 2));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FConnectClientPIEToHostPIEBeacon(this, 2, 1, TestHelpers::Port(9003)));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FWaitForClientBeaconToDestroyItselfAfterPing(this, 2));

    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FEndPlayMapCommand());
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FClearAutomationConfig());

    return true;
}
