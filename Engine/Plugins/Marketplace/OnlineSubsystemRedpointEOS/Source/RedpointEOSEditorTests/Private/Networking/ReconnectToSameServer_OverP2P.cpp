// Copyright June Rhodes. All Rights Reserved.

#include "ConnectToBeaconsInSequence_Common.h"
#include "EOSEditorCommands.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FReconnectToSameServer_OverP2P,
    "OnlineSubsystemEOS.Networking.ReconnectToSameServer.OverP2P",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);

bool FReconnectToSameServer_OverP2P::RunTest(const FString &InParam)
{
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FSetupAutomationConfig());
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FStartPlayingEmptyMap(
        this,
        EEditorAuthMode::CreateOnDemand,
        3,
        EPlayNetMode::PIE_Standalone,
        TEXT("?NetMode=ForceP2P")));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FWaitForBeaconPlayerControllers(this, 3));

    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(
        FMakePIEStartListeningServer(this, 0, TestHelpers::Port(9000), TEXT("NetMode=ForceP2P")));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(
        FMakePIEStartListeningServer(this, 1, TestHelpers::Port(9001), TEXT("NetMode=ForceP2P")));

    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FConnectClientPIEToHostPIE(this, 2, 0, TestHelpers::Port(9000)));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FWaitForClientPIEToBeConnectedToHostPIE(this, 2, 0));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FConnectClientPIEToHostPIE(this, 2, 1, TestHelpers::Port(9001)));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FWaitForClientPIEToBeConnectedToHostPIE(this, 2, 1));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FConnectClientPIEToHostPIE(this, 2, 0, TestHelpers::Port(9000)));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FWaitForClientPIEToBeConnectedToHostPIE(this, 2, 0));

    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FEndPlayMapCommand());
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FClearAutomationConfig());

    return true;
}