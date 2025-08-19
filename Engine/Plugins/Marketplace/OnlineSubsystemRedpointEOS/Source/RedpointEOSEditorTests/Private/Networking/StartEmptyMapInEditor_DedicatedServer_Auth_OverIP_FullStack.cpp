// Copyright June Rhodes. All Rights Reserved.

#include "EOSEditorCommands.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FStartEmptyMapInEditor_DedicatedServer_Auth_OverIP_FullStack,
    "OnlineSubsystemEOS.Networking.StartEmptyMapInEditor.DedicatedServer_Auth_OverIP_FullStack",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);

bool FStartEmptyMapInEditor_DedicatedServer_Auth_OverIP_FullStack::RunTest(const FString &InParam)
{
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FSetupAutomationConfig());
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FStartPlayingEmptyMap(
        this,
        EEditorAuthMode::CreateOnDemand,
        1,
        EPlayNetMode::PIE_Client,
        TEXT("?NetMode=ForceIP")));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FWaitForConnectedPlayerControllerOnDedicatedServer(this));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FEndPlayMapCommand());
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FClearAutomationConfig());

    return true;
}