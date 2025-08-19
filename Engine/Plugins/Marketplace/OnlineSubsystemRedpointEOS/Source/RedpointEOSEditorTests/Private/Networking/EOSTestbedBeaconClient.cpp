// Copyright June Rhodes. All Rights Reserved.

#include "EOSTestbedBeaconClient.h"
#include "Net/UnrealNetwork.h"

#include "EOSEditorCommands.h"

AEOSTestbedBeaconClient::AEOSTestbedBeaconClient()
{
    this->bReplicates = true;
}

void AEOSTestbedBeaconClient::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AEOSTestbedBeaconClient, PIEInstance);
}

void AEOSTestbedBeaconClient::OnFailure()
{
    UE_LOG(LogEOSEditorTests, Error, TEXT("Beacon encountered an error."));
    Super::OnFailure();
}

void AEOSTestbedBeaconClient::ClientPing_Implementation()
{
    UE_LOG(LogEOSEditorTests, Verbose, TEXT("Server successfully called ping on client. Destroying beacon client."));
    this->DestroyBeacon();
}