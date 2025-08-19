// Copyright June Rhodes. All Rights Reserved.

#include "ConnectToBeaconsInSequence_Common.h"

#include "./EOSTestbedGameMode.h"
#include "EOSEditorCommands.h"
#include "EOSTestbedBeaconClient.h"
#include "EOSTestbedBeaconControlledClient.h"
#include "EOSTestbedBeaconControlledHost.h"
#include "EOSTestbedBeaconHost.h"
#include "Editor/UnrealEdEngine.h"
#include "EditorModeManager.h"
#include "Engine/NetConnection.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "LevelEditor.h"
#include "Misc/AutomationTest.h"
#include "OnlineBeaconHost.h"
#include "OnlineEngineInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSNetDriver.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOSModule.h"
#include "Sockets.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"
#include "UnrealEdGlobals.h"

bool FWaitForBeaconPlayerControllers::Update()
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

        Test->TestTrue(TEXT("Player controllers in standalone games established within 60 seconds"), false);
        return true;
    }

    TArray<bool> FoundPCs;
    for (int i = 0; i < this->ExpectedInstances; i++)
    {
        FoundPCs.Add(false);
    }

    for (const FWorldContext &WorldContext : GEngine->GetWorldContexts())
    {
        if (WorldContext.WorldType == EWorldType::PIE && WorldContext.World() != nullptr)
        {
            for (TActorIterator<APlayerController> It(WorldContext.World()); It; ++It)
            {
                if (WorldContext.PIEInstance >= 0 && WorldContext.PIEInstance < FoundPCs.Num())
                {
                    FoundPCs[WorldContext.PIEInstance] = true;
                }
            }
        }
    }

    for (int i = 0; i < this->ExpectedInstances; i++)
    {
        if (!FoundPCs[i])
        {
            return false;
        }
    }
    return true;
}

bool FMakePIEStartListeningServer::Update()
{
    if (FPlatformTime::Seconds() - this->StartTime > 10)
    {
        Test->TestTrue(TEXT("Could not move PIE instance into listening mode within timeout"), false);
        return true;
    }

    for (const FWorldContext &WorldContext : GEngine->GetWorldContexts())
    {
        if (WorldContext.WorldType == EWorldType::PIE && WorldContext.World() != nullptr &&
            WorldContext.PIEInstance == this->PIEInstance)
        {
            FURL URL(WorldContext.LastURL);
            URL.Port = this->Port;
            URL.AddOption(TEXT("listen"));
            if (!this->ExtraParams.IsEmpty())
            {
                URL.AddOption(*this->ExtraParams);
            }
            WorldContext.World()->Listen(URL);
            return true;
        }
    }

    return true;
}

bool FSpawnBeaconOnPIE::Update()
{
    if (FPlatformTime::Seconds() - this->StartTime > 10)
    {
        Test->TestTrue(TEXT("Could not spawn host beacon within timeout"), false);
        return true;
    }

    for (const FWorldContext &WorldContext : GEngine->GetWorldContexts())
    {
        if (WorldContext.WorldType == EWorldType::PIE && WorldContext.World() != nullptr &&
            WorldContext.PIEInstance == this->PIEInstance)
        {
            AEOSTestbedBeaconHost *BeaconHost =
                WorldContext.World()->SpawnActor<AEOSTestbedBeaconHost>(AEOSTestbedBeaconHost::StaticClass());
            BeaconHost->ListenPort = this->Port;
            BeaconHost->ExtraParams.Add(this->ExtraParam);
            if (Test->TestTrue(TEXT("Beacon initializes"), BeaconHost != nullptr && BeaconHost->InitHost()))
            {
                AEOSTestbedBeaconHostObject *BeaconHostObject =
                    WorldContext.World()->SpawnActor<AEOSTestbedBeaconHostObject>(
                        AEOSTestbedBeaconHostObject::StaticClass());
                if (Test->TestTrue(TEXT("Beacon host object created"), BeaconHostObject != nullptr))
                {
                    BeaconHostObject->PIEInstance = this->PIEInstance;
                    BeaconHost->RegisterHost(BeaconHostObject);
                }
                BeaconHost->PauseBeaconRequests(false);
            }
            return true;
        }
    }

    return false;
}

bool FConnectClientPIEToHostPIEBeacon::Update()
{
    if (FPlatformTime::Seconds() - this->StartTime > 10)
    {
        Test->TestTrue(TEXT("Did not create client beacon connected to host beacon within timeout"), false);
        return true;
    }

    FString URL = TEXT("");

    for (const FWorldContext &WorldContext : GEngine->GetWorldContexts())
    {
        if (WorldContext.WorldType == EWorldType::PIE && WorldContext.World() != nullptr &&
            WorldContext.PIEInstance == this->HostPIEInstance)
        {
            for (const auto &NetDriver : WorldContext.ActiveNetDrivers)
            {
                if (NetDriver.NetDriverDef->DefName.IsEqual(NAME_BeaconNetDriver))
                {
                    UEOSNetDriver *EOSNetDriver = Cast<UEOSNetDriver>(NetDriver.NetDriver);
                    if (IsValid(EOSNetDriver))
                    {
                        if (EOSNetDriver->IsPerformingIpConnection)
                        {
                            TSharedPtr<FInternetAddr> Addr = EOSNetDriver->GetSocketSubsystem()->CreateInternetAddr();
                            FSocket *Socket = EOSNetDriver->GetSocket();
                            Socket->GetAddress(*Addr);
                            URL = FString::Printf(TEXT("127.0.0.1:%d"), Addr->GetPort());
                        }
                        else if (EOSNetDriver->RegisteredListeningUser)
                        {
                            FString PUID;
                            EOSString_ProductUserId::ToString(EOSNetDriver->RegisteredListeningUser, PUID);
                            URL = FString::Printf(
                                TEXT("%s.default.eosp2p:%d"),
                                *PUID,
                                this->Port % EOS_CHANNEL_ID_MODULO);
                        }
                    }
                    else if (
                        IsValid(NetDriver.NetDriver) && NetDriver.NetDriver->GetClass() == UIpNetDriver::StaticClass())
                    {
                        UE_LOG(
                            LogEOSEditorTests,
                            Error,
                            TEXT("Beacon net driver was IP net driver, not EOS net driver! Make sure the project is "
                                 "set up correctly in order to run this unit test!"));
                        Test->TestTrue(TEXT("Project misconfigured or EOS net driver could not be loaded"), false);
                        return true;
                    }
                    break;
                }
            }
            break;
        }
    }

    if (URL.IsEmpty())
    {
        // Host not ready.
        return false;
    }

    for (const FWorldContext &WorldContext : GEngine->GetWorldContexts())
    {
        if (WorldContext.WorldType == EWorldType::PIE && WorldContext.World() != nullptr &&
            WorldContext.PIEInstance == this->ClientPIEInstance)
        {
            auto Beacon = WorldContext.World()->SpawnActor<AOnlineBeacon>(AEOSTestbedBeaconClient::StaticClass());
            if (Beacon)
            {
                FURL OldURL;
                FURL NewURL(&OldURL, *URL, ETravelType::TRAVEL_Absolute);
                Cast<AEOSTestbedBeaconClient>(Beacon)->InitClient(NewURL);
            }

            return true;
        }
    }

    return false;
}

bool FWaitForClientBeaconToDestroyItselfAfterPing::Update()
{
    if (FPlatformTime::Seconds() - this->StartTime > 60)
    {
        Test->TestTrue(TEXT("Client beacon did not destroy itself within timeout after ping"), false);
        return true;
    }

    for (const FWorldContext &WorldContext : GEngine->GetWorldContexts())
    {
        if (WorldContext.WorldType == EWorldType::PIE && WorldContext.World() != nullptr &&
            WorldContext.PIEInstance == this->ClientPIEInstance)
        {
            for (TActorIterator<AEOSTestbedBeaconClient> It(WorldContext.World()); It; ++It)
            {
                // Beacon still exists.
                return false;
            }

            // Beacon no longer exists.
            return true;
        }
    }

    return false;
}

bool FWaitForever::Update()
{
    return false;
}

bool FConnectClientPIEToHostPIE::Update()
{
    if (FPlatformTime::Seconds() - this->StartTime > 10)
    {
        Test->TestTrue(TEXT("Did not start connection of client PIE to host PIE within timeout"), false);
        return true;
    }

    FString URL = TEXT("");

    for (const FWorldContext &WorldContext : GEngine->GetWorldContexts())
    {
        if (WorldContext.WorldType == EWorldType::PIE && WorldContext.World() != nullptr &&
            WorldContext.PIEInstance == this->HostPIEInstance)
        {
            for (const auto &NetDriver : WorldContext.ActiveNetDrivers)
            {
                if (NetDriver.NetDriverDef->DefName.IsEqual(NAME_GameNetDriver))
                {
                    UEOSNetDriver *EOSNetDriver = Cast<UEOSNetDriver>(NetDriver.NetDriver);
                    if (IsValid(EOSNetDriver))
                    {
                        if (EOSNetDriver->IsPerformingIpConnection)
                        {
                            TSharedPtr<FInternetAddr> Addr = EOSNetDriver->GetSocketSubsystem()->CreateInternetAddr();
                            FSocket *Socket = EOSNetDriver->GetSocket();
                            Socket->GetAddress(*Addr);
                            URL = FString::Printf(TEXT("127.0.0.1:%d"), Addr->GetPort());
                        }
                        else if (EOSNetDriver->RegisteredListeningUser)
                        {
                            FString PUID;
                            EOSString_ProductUserId::ToString(EOSNetDriver->RegisteredListeningUser, PUID);
                            URL = FString::Printf(
                                TEXT("%s.default.eosp2p:%d"),
                                *PUID,
                                this->Port % EOS_CHANNEL_ID_MODULO);
                        }
                    }
                    else if (
                        IsValid(NetDriver.NetDriver) && NetDriver.NetDriver->GetClass() == UIpNetDriver::StaticClass())
                    {
                        UE_LOG(
                            LogEOSEditorTests,
                            Error,
                            TEXT("Beacon net driver was IP net driver, not EOS net driver! Make sure the project is "
                                 "set up correctly in order to run this unit test!"));
                        Test->TestTrue(TEXT("Project misconfigured or EOS net driver could not be loaded"), false);
                        return true;
                    }
                    break;
                }
            }
            break;
        }
    }

    if (URL.IsEmpty())
    {
        // Host not ready.
        return false;
    }

    for (const FWorldContext &WorldContext : GEngine->GetWorldContexts())
    {
        if (WorldContext.WorldType == EWorldType::PIE && WorldContext.World() != nullptr &&
            WorldContext.PIEInstance == this->ClientPIEInstance)
        {
            FURL OldURL;
            FURL NewURL(&OldURL, *URL, ETravelType::TRAVEL_Absolute);
            FString Error;
            if (GEngine->Browse((FWorldContext &)WorldContext, NewURL, Error) == EBrowseReturnVal::Failure)
            {
                Test->TestTrue(FString::Printf(TEXT("Browse failed for client: %s"), *Error), false);
            }
            return true;
        }
    }

    return false;
}

bool FWaitForClientPIEToBeConnectedToHostPIE::Update()
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
                if (WorldContext.PIEInstance == this->HostPIEInstance && It->GetName() == TEXT("PlayerController_0") &&
                    It->GetLocalRole() == ENetRole::ROLE_Authority)
                {
                    bFoundAuthorativePC0OnHost = true;
                }

                if (WorldContext.PIEInstance == this->HostPIEInstance && It->GetName() != TEXT("PlayerController_0") &&
                    It->GetLocalRole() == ENetRole::ROLE_Authority)
                {
                    bFoundAuthorativePC1OnHost = true;
                }

                if (WorldContext.PIEInstance == this->ClientPIEInstance &&
                    It->GetName() == TEXT("PlayerController_1") && It->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
                {
                    bFoundAutonomousProxyPC1OnClient = true;
                }
            }
        }
    }

    return bFoundAuthorativePC0OnHost && bFoundAuthorativePC1OnHost && bFoundAutonomousProxyPC1OnClient;
}

bool FWaitForClientPIEToBeConnectedToHostPIEWithName::Update()
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
                if (WorldContext.PIEInstance == this->HostPIEInstance && It->GetName() == TEXT("PlayerController_0") &&
                    It->GetLocalRole() == ENetRole::ROLE_Authority)
                {
                    bFoundAuthorativePC0OnHost = true;
                }

                if (WorldContext.PIEInstance == this->HostPIEInstance && It->GetName() == ClientPCName &&
                    It->GetLocalRole() == ENetRole::ROLE_Authority)
                {
                    bFoundAuthorativePC1OnHost = true;
                }

                if (WorldContext.PIEInstance == this->ClientPIEInstance &&
                    It->GetName() == TEXT("PlayerController_1") && It->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
                {
                    bFoundAutonomousProxyPC1OnClient = true;
                }
            }
        }
    }

    return bFoundAuthorativePC0OnHost && bFoundAuthorativePC1OnHost && bFoundAutonomousProxyPC1OnClient;
}

bool FSpawnControlledBeaconOnPIE::Update()
{
    if (FPlatformTime::Seconds() - this->StartTime > 10)
    {
        Test->TestTrue(TEXT("Did not spawn controlled beacon within timeout"), false);
        return true;
    }

    for (const FWorldContext &WorldContext : GEngine->GetWorldContexts())
    {
        if (WorldContext.WorldType == EWorldType::PIE && WorldContext.World() != nullptr &&
            WorldContext.PIEInstance == this->PIEInstance)
        {
            AEOSTestbedBeaconControlledHost *BeaconHost =
                WorldContext.World()->SpawnActor<AEOSTestbedBeaconControlledHost>(
                    AEOSTestbedBeaconControlledHost::StaticClass());
            BeaconHost->ListenPort = this->Port;
            BeaconHost->ExtraParams.Add(this->ExtraParam);
            BeaconHost->BeaconInstanceName = this->BeaconInstanceName;
            if (Test->TestTrue(TEXT("Beacon initializes"), BeaconHost != nullptr && BeaconHost->InitHost()))
            {
                AEOSTestbedBeaconControlledHostObject *BeaconHostObject =
                    WorldContext.World()->SpawnActor<AEOSTestbedBeaconControlledHostObject>(
                        AEOSTestbedBeaconControlledHostObject::StaticClass());
                if (Test->TestTrue(TEXT("Beacon host object created"), BeaconHostObject != nullptr))
                {
                    BeaconHostObject->PIEInstance = this->PIEInstance;
                    BeaconHostObject->BeaconInstanceName = this->BeaconInstanceName;
                    BeaconHost->RegisterHost(BeaconHostObject);
                }
                BeaconHost->PauseBeaconRequests(false);
            }
            return true;
        }
    }

    return false;
}

bool FCreateClientControlledBeaconToPIE::Update()
{
    if (FPlatformTime::Seconds() - this->StartTime > 10)
    {
        Test->TestTrue(TEXT("Did not create controlled client beacon within timeout"), false);
        return true;
    }

    FString URL = TEXT("");

    for (const FWorldContext &WorldContext : GEngine->GetWorldContexts())
    {
        if (WorldContext.WorldType == EWorldType::PIE && WorldContext.World() != nullptr &&
            WorldContext.PIEInstance == this->HostPIEInstance)
        {
            for (const auto &NetDriver : WorldContext.ActiveNetDrivers)
            {
                if (NetDriver.NetDriverDef->DefName.IsEqual(NAME_BeaconNetDriver))
                {
                    UEOSNetDriver *EOSNetDriver = Cast<UEOSNetDriver>(NetDriver.NetDriver);
                    if (IsValid(EOSNetDriver))
                    {
                        if (EOSNetDriver->IsPerformingIpConnection)
                        {
                            TSharedPtr<FInternetAddr> Addr = EOSNetDriver->GetSocketSubsystem()->CreateInternetAddr();
                            FSocket *Socket = EOSNetDriver->GetSocket();
                            Socket->GetAddress(*Addr);
                            URL = FString::Printf(TEXT("127.0.0.1:%d"), this->Port);
                        }
                        else if (EOSNetDriver->RegisteredListeningUser)
                        {
                            FString PUID;
                            EOSString_ProductUserId::ToString(EOSNetDriver->RegisteredListeningUser, PUID);
                            URL = FString::Printf(
                                TEXT("%s.default.eosp2p:%d"),
                                *PUID,
                                this->Port % EOS_CHANNEL_ID_MODULO);
                        }
                    }
                    else if (
                        IsValid(NetDriver.NetDriver) && NetDriver.NetDriver->GetClass() == UIpNetDriver::StaticClass())
                    {
                        UE_LOG(
                            LogEOSEditorTests,
                            Error,
                            TEXT("Beacon net driver was IP net driver, not EOS net driver! Make sure the project is "
                                 "set up correctly in order to run this unit test!"));
                        Test->TestTrue(TEXT("Project misconfigured or EOS net driver could not be loaded"), false);
                        return true;
                    }
                    break;
                }
            }
            break;
        }
    }

    if (URL.IsEmpty())
    {
        // Host not ready.
        return false;
    }

    for (const FWorldContext &WorldContext : GEngine->GetWorldContexts())
    {
        if (WorldContext.WorldType == EWorldType::PIE && WorldContext.World() != nullptr &&
            WorldContext.PIEInstance == this->ClientPIEInstance)
        {
            auto Beacon = WorldContext.World()->SpawnActor<AEOSTestbedBeaconControlledClient>(
                AEOSTestbedBeaconControlledClient::StaticClass());
            if (Beacon)
            {
                Beacon->BeaconInstanceName = this->BeaconInstanceName;
                FURL OldURL;
                FURL NewURL(&OldURL, *URL, ETravelType::TRAVEL_Absolute);
                Beacon->InitClient(NewURL);
            }

            return true;
        }
    }

    return false;
}

bool FWaitForClientBeaconReady::Update()
{
    if (FPlatformTime::Seconds() - this->StartTime > 60)
    {
        Test->TestTrue(TEXT("Client beacon took too long to get ready"), false);
        return true;
    }

    for (const FWorldContext &WorldContext : GEngine->GetWorldContexts())
    {
        if (WorldContext.WorldType == EWorldType::PIE && WorldContext.World() != nullptr &&
            WorldContext.PIEInstance == this->PIEInstance)
        {
            for (TActorIterator<AEOSTestbedBeaconControlledClient> It(WorldContext.World()); It; ++It)
            {
                if (It->BeaconInstanceName == this->BeaconInstanceName && It->bIsClientReady)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool FStartClientPing::Update()
{
    if (FPlatformTime::Seconds() - this->StartTime > 10)
    {
        Test->TestTrue(TEXT("Unable to start ping process on client"), false);
        return true;
    }

    for (const FWorldContext &WorldContext : GEngine->GetWorldContexts())
    {
        if (WorldContext.WorldType == EWorldType::PIE && WorldContext.World() != nullptr &&
            WorldContext.PIEInstance == this->PIEInstance)
        {
            for (TActorIterator<AEOSTestbedBeaconControlledClient> It(WorldContext.World()); It; ++It)
            {
                if (It->BeaconInstanceName == this->BeaconInstanceName && It->bIsClientReady)
                {
                    It->bClientGotPong = false;
                    It->ServerPing();
                    return true;
                }
            }
        }
    }

    return false;
}

bool FWaitForClientPing::Update()
{
    if (FPlatformTime::Seconds() - this->StartTime > 10)
    {
        Test->TestTrue(TEXT("Took too long to receive ping on client"), false);
        return true;
    }

    for (const FWorldContext &WorldContext : GEngine->GetWorldContexts())
    {
        if (WorldContext.WorldType == EWorldType::PIE && WorldContext.World() != nullptr &&
            WorldContext.PIEInstance == this->PIEInstance)
        {
            for (TActorIterator<AEOSTestbedBeaconControlledClient> It(WorldContext.World()); It; ++It)
            {
                if (It->BeaconInstanceName == this->BeaconInstanceName && It->bIsClientReady && It->bClientGotPong)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool FDestroyClientControlledBeacon::Update()
{
    if (FPlatformTime::Seconds() - this->StartTime > 10)
    {
        Test->TestTrue(TEXT("Did not destroy controlled client beacon within timeout"), false);
        return true;
    }

    for (const FWorldContext &WorldContext : GEngine->GetWorldContexts())
    {
        if (WorldContext.WorldType == EWorldType::PIE && WorldContext.World() != nullptr &&
            WorldContext.PIEInstance == this->PIEInstance)
        {
            for (TActorIterator<AEOSTestbedBeaconControlledClient> It(WorldContext.World()); It; ++It)
            {
                if (It->BeaconInstanceName == this->BeaconInstanceName)
                {
                    It->DestroyBeacon();
                    return true;
                }
            }
        }
    }

    return false;
}

bool FDestroyClientBeacons::Update()
{
    for (const FWorldContext &WorldContext : GEngine->GetWorldContexts())
    {
        if (WorldContext.WorldType == EWorldType::PIE && WorldContext.World() != nullptr &&
            WorldContext.PIEInstance == this->PIEInstance)
        {
            for (TActorIterator<AEOSTestbedBeaconControlledClient> It(WorldContext.World()); It; ++It)
            {
                if (It->GetNetConnection())
                {
                    It->GetNetConnection()->CleanUp();
                }
                It->DestroyBeacon();
            }

            for (TActorIterator<AEOSTestbedBeaconClient> It(WorldContext.World()); It; ++It)
            {
                if (It->GetNetConnection())
                {
                    It->GetNetConnection()->CleanUp();
                }
                It->DestroyBeacon();
            }
        }
    }

    return true;
}

bool FDestroyHostBeacons::Update()
{
    for (const FWorldContext &WorldContext : GEngine->GetWorldContexts())
    {
        if (WorldContext.WorldType == EWorldType::PIE && WorldContext.World() != nullptr &&
            WorldContext.PIEInstance == this->PIEInstance)
        {
            for (TActorIterator<AEOSTestbedBeaconControlledHostObject> It(WorldContext.World()); It; ++It)
            {
                It->Destroy();
            }

            for (TActorIterator<AEOSTestbedBeaconControlledHost> It(WorldContext.World()); It; ++It)
            {
                if (It->GetNetConnection())
                {
                    It->GetNetConnection()->CleanUp();
                }
                It->DestroyBeacon();
            }

            for (TActorIterator<AEOSTestbedBeaconHostObject> It(WorldContext.World()); It; ++It)
            {
                It->Destroy();
            }

            for (TActorIterator<AEOSTestbedBeaconHost> It(WorldContext.World()); It; ++It)
            {
                if (It->GetNetConnection())
                {
                    It->GetNetConnection()->CleanUp();
                }
                It->DestroyBeacon();
            }
        }
    }

    return true;
}