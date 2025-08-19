// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "Settings/LevelEditorPlaySettings.h"
#include "TestHelpers.h"
#include "Tests/AutomationEditorCommon.h"

DECLARE_LOG_CATEGORY_EXTERN(LogEOSEditorTests, All, All);

enum class EEditorAuthMode : int8
{
    CreateOnDemand,

    RequireEpicGamesAccount,
};

// NOLINTNEXTLINE(performance-unnecessary-value-param)
DEFINE_LATENT_AUTOMATION_COMMAND_FIVE_PARAMETER(
    FStartPlayingEmptyMap,
    FAutomationTestBase *,
    Test,
    EEditorAuthMode,
    AuthMode,
    int,
    NumPlayers,
    EPlayNetMode,
    PlayMode,
    FString,
    AdditionalLaunchParameters);
DEFINE_LATENT_AUTOMATION_COMMAND(FClearAutomationConfig);

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FWaitForStandalonePlayerController, FAutomationTestBase *, Test);
DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FWaitForConnectedPlayerController, FAutomationTestBase *, Test);
DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FWaitForConnectedPlayerControllerOnDedicatedServer, FAutomationTestBase *, Test);
DEFINE_LATENT_AUTOMATION_COMMAND(FSetupAutomationConfig);