// Copyright June Rhodes. All Rights Reserved.

#include "EOSTestbedBeaconHost.h"

#include "EOSEditorCommands.h"
#include "EOSTestbedBeaconClient.h"

AEOSTestbedBeaconHostObject::AEOSTestbedBeaconHostObject()
{
    ClientBeaconActorClass = AEOSTestbedBeaconClient::StaticClass();
    BeaconTypeName = ClientBeaconActorClass->GetName();
}

bool AEOSTestbedBeaconHostObject::Init()
{
    UE_LOG(LogEOSEditorTests, Verbose, TEXT("Beacon is now listening on server."));
    return true;
}

void AEOSTestbedBeaconHostObject::OnClientConnected(
    AOnlineBeaconClient *NewClientActor,
    UNetConnection *ClientConnection)
{
    Super::OnClientConnected(NewClientActor, ClientConnection);

    AEOSTestbedBeaconClient *BeaconClient = Cast<AEOSTestbedBeaconClient>(NewClientActor);
    if (BeaconClient != NULL)
    {
        BeaconClient->ClientPing();
    }
}

AOnlineBeaconClient *AEOSTestbedBeaconHostObject::SpawnBeaconActor(UNetConnection *ClientConnection)
{
    AEOSTestbedBeaconClient *BeaconClient = Cast<AEOSTestbedBeaconClient>(Super::SpawnBeaconActor(ClientConnection));
    BeaconClient->PIEInstance = this->PIEInstance;
    return BeaconClient;
}

bool AEOSTestbedBeaconHost::InitHost()
{
    FURL URL(nullptr, TEXT(""), TRAVEL_Absolute);

    for (const auto &Param : this->ExtraParams)
    {
        URL.AddOption(*Param);
    }

    URL.Port = this->ListenPort;
    if (URL.Valid)
    {
        if (this->InitBase() && this->NetDriver)
        {
            FString Error;
            if (this->NetDriver->InitListen(this, URL, false, Error))
            {
                this->ListenPort = URL.Port;
                NetDriver->SetWorld(this->GetWorld());
                NetDriver->Notify = this;
                NetDriver->InitialConnectTimeout = this->BeaconConnectionInitialTimeout;
                NetDriver->ConnectionTimeout = this->BeaconConnectionTimeout;
                return true;
            }
            else
            {
                // error initializing the network stack...
                UE_LOG(LogBeacon, Log, TEXT("AOnlineBeaconHost::InitHost failed"));
                this->OnFailure();
            }
        }
    }

    return false;
}