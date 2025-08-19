// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FCreateDeviceIdNode : public FAuthenticationGraphNode
{
public:
    UE_NONCOPYABLE(FCreateDeviceIdNode);
    FCreateDeviceIdNode() = default;
    virtual ~FCreateDeviceIdNode() = default;

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FCreateDeviceIdNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
