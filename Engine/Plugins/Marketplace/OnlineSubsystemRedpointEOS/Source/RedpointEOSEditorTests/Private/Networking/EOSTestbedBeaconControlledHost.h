// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineBeaconHost.h"
#include "OnlineBeaconHostObject.h"
#include "UObject/ObjectMacros.h"

#include "EOSTestbedBeaconControlledHost.generated.h"

UCLASS(transient, notplaceable)
class AEOSTestbedBeaconControlledHostObject : public AOnlineBeaconHostObject
{
    GENERATED_BODY()

public:
    AEOSTestbedBeaconControlledHostObject();

    UPROPERTY()
    int PIEInstance;

    UPROPERTY()
    FString BeaconInstanceName;

    virtual AOnlineBeaconClient *SpawnBeaconActor(class UNetConnection *ClientConnection) override;
    virtual void OnClientConnected(class AOnlineBeaconClient *NewClientActor, class UNetConnection *ClientConnection)
        override;

    virtual bool Init();
};

UCLASS(transient, notplaceable)
class AEOSTestbedBeaconControlledHost : public AOnlineBeaconHost
{
    GENERATED_BODY()

public:
    UPROPERTY()
    TArray<FString> ExtraParams;

    UPROPERTY()
    FString BeaconInstanceName;

    virtual bool InitHost() override;
};