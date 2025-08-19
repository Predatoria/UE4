// Copyright June Rhodes. All Rights Reserved.

#include "EOSEditorCommands.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FStartEmptyMapInEditor_ListenServer_Auth_OverIP_FullStack,
    "OnlineSubsystemEOS.Networking.StartEmptyMapInEditor.ListenServer_Auth_OverIP_FullStack",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);

bool FStartEmptyMapInEditor_ListenServer_Auth_OverIP_FullStack::RunTest(const FString &InParam)
{
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FSetupAutomationConfig());
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FStartPlayingEmptyMap(
        this,
        EEditorAuthMode::CreateOnDemand,
        2,
        EPlayNetMode::PIE_ListenServer,
        TEXT("?NetMode=ForceIP")));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FWaitForConnectedPlayerController(this));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FEndPlayMapCommand());
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FClearAutomationConfig());

    return true;
}