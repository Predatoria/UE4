// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "Settings/LevelEditorPlaySettings.h"
#include "Tests/AutomationEditorCommon.h"

DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(
    FWaitForBeaconPlayerControllers,
    FAutomationTestBase *,
    Test,
    int,
    ExpectedInstances);
// NOLINTNEXTLINE(performance-unnecessary-value-param)
DEFINE_LATENT_AUTOMATION_COMMAND_FOUR_PARAMETER(
    FMakePIEStartListeningServer,
    FAutomationTestBase *,
    Test,
    int,
    PIEInstance,
    uint16,
    Port,
    FString,
    ExtraParams);
// NOLINTNEXTLINE(performance-unnecessary-value-param)
DEFINE_LATENT_AUTOMATION_COMMAND_FOUR_PARAMETER(
    FSpawnBeaconOnPIE,
    FAutomationTestBase *,
    Test,
    int,
    PIEInstance,
    uint16,
    Port,
    FString,
    ExtraParam);
DEFINE_LATENT_AUTOMATION_COMMAND(FWaitForever);

DEFINE_LATENT_AUTOMATION_COMMAND_FOUR_PARAMETER(
    FConnectClientPIEToHostPIEBeacon,
    FAutomationTestBase *,
    Test,
    int,
    ClientPIEInstance,
    int,
    HostPIEInstance,
    uint16,
    Port);
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(
    FWaitForClientBeaconToDestroyItselfAfterPing,
    FAutomationTestBase *,
    Test,
    int,
    ClientPIEInstance);
DEFINE_LATENT_AUTOMATION_COMMAND_FOUR_PARAMETER(
    FConnectClientPIEToHostPIE,
    FAutomationTestBase *,
    Test,
    int,
    ClientPIEInstance,
    int,
    HostPIEInstance,
    uint16,
    Port);
DEFINE_LATENT_AUTOMATION_COMMAND_THREE_PARAMETER(
    FWaitForClientPIEToBeConnectedToHostPIE,
    FAutomationTestBase *,
    Test,
    int,
    ClientPIEInstance,
    int,
    HostPIEInstance);
// NOLINTNEXTLINE(performance-unnecessary-value-param)
DEFINE_LATENT_AUTOMATION_COMMAND_FOUR_PARAMETER(
    FWaitForClientPIEToBeConnectedToHostPIEWithName,
    FAutomationTestBase *,
    Test,
    int,
    ClientPIEInstance,
    int,
    HostPIEInstance,
    FString,
    ClientPCName);

// NOLINTNEXTLINE(performance-unnecessary-value-param)
DEFINE_LATENT_AUTOMATION_COMMAND_FIVE_PARAMETER(
    FSpawnControlledBeaconOnPIE,
    FAutomationTestBase *,
    Test,
    int,
    PIEInstance,
    uint16,
    Port,
    FString,
    BeaconInstanceName,
    FString,
    ExtraParam);
// NOLINTNEXTLINE(performance-unnecessary-value-param)
DEFINE_LATENT_AUTOMATION_COMMAND_FIVE_PARAMETER(
    FCreateClientControlledBeaconToPIE,
    FAutomationTestBase *,
    Test,
    int,
    ClientPIEInstance,
    int,
    HostPIEInstance,
    uint16,
    Port,
    FString,
    BeaconInstanceName);
// NOLINTNEXTLINE(performance-unnecessary-value-param)
DEFINE_LATENT_AUTOMATION_COMMAND_THREE_PARAMETER(
    FWaitForClientBeaconReady,
    FAutomationTestBase *,
    Test,
    int,
    PIEInstance,
    FString,
    BeaconInstanceName);
// NOLINTNEXTLINE(performance-unnecessary-value-param)
DEFINE_LATENT_AUTOMATION_COMMAND_THREE_PARAMETER(
    FStartClientPing,
    FAutomationTestBase *,
    Test,
    int,
    PIEInstance,
    FString,
    BeaconInstanceName);
// NOLINTNEXTLINE(performance-unnecessary-value-param)
DEFINE_LATENT_AUTOMATION_COMMAND_THREE_PARAMETER(
    FWaitForClientPing,
    FAutomationTestBase *,
    Test,
    int,
    PIEInstance,
    FString,
    BeaconInstanceName);
// NOLINTNEXTLINE(performance-unnecessary-value-param)
DEFINE_LATENT_AUTOMATION_COMMAND_THREE_PARAMETER(
    FDestroyClientControlledBeacon,
    FAutomationTestBase *,
    Test,
    int,
    PIEInstance,
    FString,
    BeaconInstanceName);
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FDestroyClientBeacons, FAutomationTestBase *, Test, int, PIEInstance);
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FDestroyHostBeacons, FAutomationTestBase *, Test, int, PIEInstance);