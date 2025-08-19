// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IpConnection.h"
#include "UObject/ObjectMacros.h"

#include "EOSIpNetConnection.generated.h"

EOS_ENABLE_STRICT_WARNINGS

UCLASS(transient, config = OnlineSubsystemRedpointEOS)
class UEOSIpNetConnection : public UIpConnection
{
    GENERATED_BODY()

public:
    virtual void InitBase(
        UNetDriver *InDriver,
        FSocket *InSocket,
        const FURL &InURL,
        EConnectionState InState,
        int32 InMaxPacket = 0,
        int32 InPacketOverhead = 0) override;
};

EOS_DISABLE_STRICT_WARNINGS