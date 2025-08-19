// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineBeaconHost.h"
#include "OnlineBeaconHostObject.h"
#include "UObject/ObjectMacros.h"

#include "EOSTestbedBeaconHost.generated.h"

UCLASS(transient, notplaceable)
class AEOSTestbedBeaconHostObject : public AOnlineBeaconHostObject
{
    GENERATED_BODY()

public:
    AEOSTestbedBeaconHostObject();

    UPROPERTY()
    int PIEInstance;

    virtual AOnlineBeaconClient *SpawnBeaconActor(class UNetConnection *ClientConnection) override;
    virtual void OnClientConnected(class AOnlineBeaconClient *NewClientActor, class UNetConnection *ClientConnection)
        override;

    virtual bool Init();
};

UCLASS(transient, notplaceable)
class AEOSTestbedBeaconHost : public AOnlineBeaconHost
{
    GENERATED_BODY()

public:
    UPROPERTY()
    TArray<FString> ExtraParams;

    virtual bool InitHost() override;
};