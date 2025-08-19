// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineBeaconClient.h"
#include "UObject/ObjectMacros.h"

#include "EOSTestbedBeaconClient.generated.h"

UCLASS(transient, notplaceable)
class AEOSTestbedBeaconClient : public AOnlineBeaconClient
{
    GENERATED_BODY()

public:
    AEOSTestbedBeaconClient();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

    UPROPERTY(Replicated)
    int PIEInstance;

    virtual void OnFailure() override;

    UFUNCTION(client, reliable)
    virtual void ClientPing();
};