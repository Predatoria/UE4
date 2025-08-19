// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineBeaconClient.h"
#include "UObject/ObjectMacros.h"

#include "EOSTestbedBeaconControlledClient.generated.h"

UCLASS(transient, notplaceable)
class AEOSTestbedBeaconControlledClient : public AOnlineBeaconClient
{
    GENERATED_BODY()

public:
    AEOSTestbedBeaconControlledClient();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

    UPROPERTY()
    bool bIsClientReady;

    UPROPERTY()
    bool bClientGotPong;

    UPROPERTY(Replicated)
    int PIEInstance;

    UPROPERTY()
    FString BeaconInstanceName;

    virtual void OnFailure() override;

    UFUNCTION(server, reliable)
    virtual void ServerPing();

    UFUNCTION(client, reliable)
    virtual void ClientPong();

    UFUNCTION(client, reliable)
    virtual void ClientMarkAsReady();
};