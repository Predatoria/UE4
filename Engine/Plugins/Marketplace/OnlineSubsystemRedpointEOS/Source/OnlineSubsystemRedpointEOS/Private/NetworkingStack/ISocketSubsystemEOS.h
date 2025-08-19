// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "../../Shared/EOSCommon.h"
#include "./ISocketEOS.h"
#include "Engine/EngineBaseTypes.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/ISocketEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "SocketSubsystem.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API ISocketSubsystemEOS : public ISocketSubsystem
{
public:
    virtual ~ISocketSubsystemEOS(){};

    virtual EOS_ProductUserId GetBindingProductUserId_P2POnly() const = 0;
    virtual bool GetBindingProductUserId_P2POrDedicatedServer(EOS_ProductUserId &OutPUID) const = 0;

    virtual FString GetSocketName(bool bListening, FURL InURL) const = 0;

    virtual void SuspendReceive(){};
    virtual void ResumeReceive(){};
};

EOS_DISABLE_STRICT_WARNINGS