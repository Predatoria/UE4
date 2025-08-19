// Copyright June Rhodes. All Rights Reserved.

#include "EOSTestbedBeaconControlledClient.h"
#include "Net/UnrealNetwork.h"

#include "EOSEditorCommands.h"

AEOSTestbedBeaconControlledClient::AEOSTestbedBeaconControlledClient()
{
    this->bReplicates = true;
    this->bIsClientReady = false;
}

void AEOSTestbedBeaconControlledClient::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AEOSTestbedBeaconControlledClient, PIEInstance);
}

void AEOSTestbedBeaconControlledClient::OnFailure()
{
    UE_LOG(LogEOSEditorTests, Error, TEXT("Beacon %s encountered an error."), *this->BeaconInstanceName);
    Super::OnFailure();
}

void AEOSTestbedBeaconControlledClient::ServerPing_Implementation()
{
    UE_LOG(LogEOSEditorTests, Verbose, TEXT("Server %s received ping, sending pong back."), *this->BeaconInstanceName);
    this->ClientPong();
}

void AEOSTestbedBeaconControlledClient::ClientPong_Implementation()
{
    UE_LOG(
        LogEOSEditorTests,
        Verbose,
        TEXT("Client %s received pong, marking as received."),
        *this->BeaconInstanceName);
    this->bClientGotPong = true;
}

void AEOSTestbedBeaconControlledClient::ClientMarkAsReady_Implementation()
{
    UE_LOG(
        LogEOSEditorTests,
        Verbose,
        TEXT("Client %s successfully established connection."),
        *this->BeaconInstanceName);
    this->bIsClientReady = true;
}