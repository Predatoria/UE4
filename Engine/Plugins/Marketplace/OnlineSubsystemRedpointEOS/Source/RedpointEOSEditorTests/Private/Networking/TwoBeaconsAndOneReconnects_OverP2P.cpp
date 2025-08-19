// Copyright June Rhodes. All Rights Reserved.

#include "ConnectToBeaconsInSequence_Common.h"
#include "EOSEditorCommands.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTwoBeaconsAndOneReconnects_OverP2P,
    "OnlineSubsystemEOS.Networking.TwoBeaconsAndOneReconnects.OverP2P",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);

bool FTwoBeaconsAndOneReconnects_OverP2P::RunTest(const FString &InParam)
{
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FSetupAutomationConfig());
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FStartPlayingEmptyMap(
        this,
        EEditorAuthMode::CreateOnDemand,
        2,
        EPlayNetMode::PIE_Standalone,
        TEXT("?NetMode=ForceP2P")));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FWaitForBeaconPlayerControllers(this, 2));

    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(
        FSpawnControlledBeaconOnPIE(this, 0, TestHelpers::Port(9000), TEXT("A"), TEXT("NetMode=ForceP2P")));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(
        FSpawnControlledBeaconOnPIE(this, 0, TestHelpers::Port(9001), TEXT("B"), TEXT("NetMode=ForceP2P")));

    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(
        FCreateClientControlledBeaconToPIE(this, 1, 0, TestHelpers::Port(9000), TEXT("A")));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(
        FCreateClientControlledBeaconToPIE(this, 1, 0, TestHelpers::Port(9001), TEXT("B")));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FWaitForClientBeaconReady(this, 1, TEXT("A")));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FWaitForClientBeaconReady(this, 1, TEXT("B")));

    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FStartClientPing(this, 1, TEXT("A")));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FWaitForClientPing(this, 1, TEXT("A")));

    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FStartClientPing(this, 1, TEXT("B")));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FWaitForClientPing(this, 1, TEXT("B")));

    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FDestroyClientControlledBeacon(this, 1, TEXT("B")));

    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(
        FCreateClientControlledBeaconToPIE(this, 1, 0, TestHelpers::Port(9001), TEXT("B")));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FWaitForClientBeaconReady(this, 1, TEXT("B")));

    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FStartClientPing(this, 1, TEXT("B")));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FWaitForClientPing(this, 1, TEXT("B")));

    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FStartClientPing(this, 1, TEXT("A")));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FWaitForClientPing(this, 1, TEXT("A")));

    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FDestroyClientBeacons(this, 1));

    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FDestroyHostBeacons(this, 0));

    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FEndPlayMapCommand());
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FClearAutomationConfig());

    return true;
}