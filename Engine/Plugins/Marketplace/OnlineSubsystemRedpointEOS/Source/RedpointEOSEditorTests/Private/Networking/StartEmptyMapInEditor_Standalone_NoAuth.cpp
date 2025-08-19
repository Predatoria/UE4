// Copyright June Rhodes. All Rights Reserved.

#include "EOSEditorCommands.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FStartEmptyMapInEditor_Standalone_NoAuth,
    "OnlineSubsystemEOS.Networking.StartEmptyMapInEditor.Standalone_NoAuth",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);

bool FStartEmptyMapInEditor_Standalone_NoAuth::RunTest(const FString &InParam)
{
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FSetupAutomationConfig());
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FStartPlayingEmptyMap(
        this,
        EEditorAuthMode::CreateOnDemand,
        1,
        EPlayNetMode::PIE_Standalone,
        TEXT("?NetMode=ForceIP")));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FWaitForStandalonePlayerController(this));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FEndPlayMapCommand());
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FClearAutomationConfig());

    return true;
}