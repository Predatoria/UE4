// Copyright June Rhodes. All Rights Reserved.

#include "EOSEditorCommands.h"

#include "./EOSTestbedGameMode.h"
#include "Editor/UnrealEdEngine.h"
#include "EditorModeManager.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "LevelEditor.h"
#include "Misc/AutomationTest.h"
#include "OnlineEngineInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOSModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"
#include "UnrealEdGlobals.h"

DEFINE_LOG_CATEGORY(LogEOSEditorTests);

bool FStartPlayingEmptyMap::Update()
{
    if (AuthMode == EEditorAuthMode::RequireEpicGamesAccount)
    {
        TArray<FEpicGamesAutomatedTestingCredential> Credentials;
        if (!Test->TestTrue(TEXT("Credentials were loaded"), LoadEpicGamesAutomatedTestingCredentials(Credentials)))
        {
            return true;
        }

        auto CredentialsPIE = MakeShared<FAutomationLoginCredentialsForPIE>();
        check(this->NumPlayers == 1 || this->NumPlayers == 2);
        if (this->NumPlayers == 1)
        {
            CredentialsPIE->Credentials.Add(
                FOnlineAccountCredentials(TEXT("AUTOMATED_TESTING"), Credentials[0].Username, Credentials[0].Password));
        }
        else
        {
            CredentialsPIE->Credentials.Add(
                FOnlineAccountCredentials(TEXT("AUTOMATED_TESTING"), Credentials[0].Username, Credentials[0].Password));
            CredentialsPIE->Credentials.Add(
                FOnlineAccountCredentials(TEXT("AUTOMATED_TESTING"), Credentials[1].Username, Credentials[1].Password));
        }
        ((UOnlineEngineInterfaceEOS *)UOnlineEngineInterface::Get())->AutomationCredentials = CredentialsPIE;
    }
    else
    {
        auto CredentialsPIE = MakeShared<FAutomationLoginCredentialsForPIE>();
        for (int i = 0; i < this->NumPlayers; i++)
        {
            CredentialsPIE->Credentials.Add(FOnlineAccountCredentials(
                TEXT("AUTOMATED_TESTING"),
                FString::Printf(TEXT("CreateOnDemand:%s"), *Test->GetTestName()),
                FString::Printf(TEXT("%d"), TestHelpers::Port(i))));
        }
        ((UOnlineEngineInterfaceEOS *)UOnlineEngineInterface::Get())->AutomationCredentials = CredentialsPIE;
    }

    UWorld *World = FAutomationEditorCommonUtils::CreateNewMap();

    for (int i = 0; i < this->NumPlayers; i++)
    {
        GEditor->AddActor(World->GetCurrentLevel(), APlayerStart::StaticClass(), FTransform(FVector(100 * i, 0, 0)));
    }

    FLevelEditorModule &LevelEditorModule =
        FModuleManager::Get().GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));

    ULevelEditorPlaySettings *PlaySettings = NewObject<ULevelEditorPlaySettings>();
    PlaySettings->SetRunUnderOneProcess(true);
    PlaySettings->SetPlayNumberOfClients(this->NumPlayers);
    PlaySettings->SetPlayNetMode(this->PlayMode);
    PlaySettings->SetServerPort(TestHelpers::Port(7777));
    PlaySettings->bLaunchSeparateServer = false;

    // Hack our parameters in. The handling code will change /Temp/Fixup into the correct map URL for us.
    ((UEditorEngine *)GEngine)->UserEditedPlayWorldURL =
        ((UEditorEngine *)GEngine)->BuildPlayWorldURL(TEXT("/Temp/Fixup"), false, this->AdditionalLaunchParameters);

    FRequestPlaySessionParams Params;
    Params.DestinationSlateViewport = LevelEditorModule.GetFirstActiveViewport();
    Params.EditorPlaySettings = PlaySettings;
    Params.SessionDestination = EPlaySessionDestinationType::InProcess;
    Params.GameModeOverride = AEOSTestbedGameMode::StaticClass();
    GUnrealEd->RequestPlaySession(Params);

    return true;
}

bool FClearAutomationConfig::Update()
{
    ((UOnlineEngineInterfaceEOS *)UOnlineEngineInterface::Get())->AutomationCredentials = nullptr;
    ((UEditorEngine *)GEngine)->UserEditedPlayWorldURL = TEXT("");
    return true;
}

bool FWaitForStandalonePlayerController::Update()
{
    if (FPlatformTime::Seconds() - this->StartTime > 60)
    {
        // We didn't connect in time. Fail the test.
        for (const FWorldContext &WorldContext : GEngine->GetWorldContexts())
        {
            if (WorldContext.WorldType == EWorldType::PIE && WorldContext.World() != nullptr)
            {
                UE_LOG(
                    LogEOSEditorTests,
                    Verbose,
                    TEXT("PIE world context %d had player controllers:"),
                    WorldContext.PIEInstance);
                for (TActorIterator<APlayerController> It(WorldContext.World()); It; ++It)
                {
                    UE_LOG(LogEOSEditorTests, Verbose, TEXT("- %s (%d)"), *It->GetName(), It->GetLocalRole());
                }
            }
        }

        Test->TestTrue(TEXT("Player controller was initialized within 10 seconds"), false);
        return true;
    }

    for (const FWorldContext &WorldContext : GEngine->GetWorldContexts())
    {
        if (WorldContext.WorldType == EWorldType::PIE && WorldContext.PIEInstance == 0 &&
            WorldContext.World() != nullptr)
        {
            for (TActorIterator<APlayerController> It(WorldContext.World()); It; ++It)
            {
                return true;
            }
        }
    }

    return false;
}

bool FWaitForConnectedPlayerController::Update()
{
    if (FPlatformTime::Seconds() - this->StartTime > 60)
    {
        // We didn't connect in time. Fail the test.
        for (const FWorldContext &WorldContext : GEngine->GetWorldContexts())
        {
            if (WorldContext.WorldType == EWorldType::PIE && WorldContext.World() != nullptr)
            {
                UE_LOG(
                    LogEOSEditorTests,
                    Verbose,
                    TEXT("PIE world context %d had player controllers:"),
                    WorldContext.PIEInstance);
                for (TActorIterator<APlayerController> It(WorldContext.World()); It; ++It)
                {
                    UE_LOG(LogEOSEditorTests, Verbose, TEXT("- %s (%d)"), *It->GetName(), It->GetLocalRole());
                }
            }
        }

        Test->TestTrue(TEXT("Network connection was successfully established within 60 seconds"), false);
        return true;
    }

    bool bFoundAuthorativePC0OnHost = false;
    bool bFoundAuthorativePC1OnHost = false;
    bool bFoundAutonomousProxyPC1OnClient = false;

    for (const FWorldContext &WorldContext : GEngine->GetWorldContexts())
    {
        if (WorldContext.WorldType == EWorldType::PIE && WorldContext.World() != nullptr)
        {
            for (TActorIterator<APlayerController> It(WorldContext.World()); It; ++It)
            {
                if (WorldContext.PIEInstance == 0 && It->GetName() == TEXT("PlayerController_0") &&
                    It->GetLocalRole() == ENetRole::ROLE_Authority)
                {
                    bFoundAuthorativePC0OnHost = true;
                }

                if (WorldContext.PIEInstance == 0 && It->GetName() == TEXT("PlayerController_1") &&
                    It->GetLocalRole() == ENetRole::ROLE_Authority)
                {
                    bFoundAuthorativePC1OnHost = true;
                }

                if (WorldContext.PIEInstance == 1 && It->GetName() == TEXT("PlayerController_1") &&
                    It->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
                {
                    bFoundAutonomousProxyPC1OnClient = true;
                }
            }
        }
    }

    return bFoundAuthorativePC0OnHost && bFoundAuthorativePC1OnHost && bFoundAutonomousProxyPC1OnClient;
}

bool FWaitForConnectedPlayerControllerOnDedicatedServer::Update()
{
    if (FPlatformTime::Seconds() - this->StartTime > 60)
    {
        // We didn't connect in time. Fail the test.
        for (const FWorldContext &WorldContext : GEngine->GetWorldContexts())
        {
            if (WorldContext.WorldType == EWorldType::PIE && WorldContext.World() != nullptr)
            {
                UE_LOG(
                    LogEOSEditorTests,
                    Verbose,
                    TEXT("PIE world context %d had player controllers:"),
                    WorldContext.PIEInstance);
                for (TActorIterator<APlayerController> It(WorldContext.World()); It; ++It)
                {
                    UE_LOG(LogEOSEditorTests, Verbose, TEXT("- %s (%d)"), *It->GetName(), It->GetLocalRole());
                }
            }
        }

        Test->TestTrue(TEXT("Network connection was successfully established within 60 seconds"), false);
        return true;
    }

    bool bFoundAuthorativePC0OnHost = false;

    for (const FWorldContext &WorldContext : GEngine->GetWorldContexts())
    {
        if (WorldContext.WorldType == EWorldType::PIE && WorldContext.World() != nullptr)
        {
            for (TActorIterator<APlayerController> It(WorldContext.World()); It; ++It)
            {
                if (WorldContext.PIEInstance == 0 && It->GetName() == TEXT("PlayerController_0") &&
                    It->GetLocalRole() == ENetRole::ROLE_Authority)
                {
                    bFoundAuthorativePC0OnHost = true;
                }
            }
        }
    }

    return bFoundAuthorativePC0OnHost;
}

bool FSetupAutomationConfig::Update()
{
    FOnlineSubsystemRedpointEOSModule &OnlineSubsystemEOSModule =
        FModuleManager::Get().GetModuleChecked<FOnlineSubsystemRedpointEOSModule>(TEXT("OnlineSubsystemRedpointEOS"));
    OnlineSubsystemEOSModule.AutomationTestingConfigOverride = MakeShared<FEOSConfigEASLogin>();

    // Force set up of AES-GCM encryption for encryption testing.
    GConfig->SetString(
        TEXT("PacketHandlerComponents"),
        TEXT("EncryptionComponent"),
        TEXT("AESGCMHandlerComponent"),
        GEngineIni);

    return true;
}