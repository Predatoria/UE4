// Copyright June Rhodes. All Rights Reserved.

#include "EOSTestbedBeaconControlledHost.h"

#include "EOSEditorCommands.h"
#include "EOSTestbedBeaconControlledClient.h"

AEOSTestbedBeaconControlledHostObject::AEOSTestbedBeaconControlledHostObject()
{
    ClientBeaconActorClass = AEOSTestbedBeaconControlledClient::StaticClass();
    BeaconTypeName = ClientBeaconActorClass->GetName();
}

bool AEOSTestbedBeaconControlledHostObject::Init()
{
    UE_LOG(LogEOSEditorTests, Verbose, TEXT("Beacon is now listening on server."));
    return true;
}

void AEOSTestbedBeaconControlledHostObject::OnClientConnected(
    AOnlineBeaconClient *NewClientActor,
    UNetConnection *ClientConnection)
{
    Super::OnClientConnected(NewClientActor, ClientConnection);

    AEOSTestbedBeaconControlledClient *BeaconClient = Cast<AEOSTestbedBeaconControlledClient>(NewClientActor);
    if (BeaconClient != NULL)
    {
        BeaconClient->ClientMarkAsReady();
    }
}

AOnlineBeaconClient *AEOSTestbedBeaconControlledHostObject::SpawnBeaconActor(UNetConnection *ClientConnection)
{
    AEOSTestbedBeaconControlledClient *BeaconClient =
        Cast<AEOSTestbedBeaconControlledClient>(Super::SpawnBeaconActor(ClientConnection));
    BeaconClient->PIEInstance = this->PIEInstance;
    BeaconClient->BeaconInstanceName = this->BeaconInstanceName;
    return BeaconClient;
}

bool AEOSTestbedBeaconControlledHost::InitHost()
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