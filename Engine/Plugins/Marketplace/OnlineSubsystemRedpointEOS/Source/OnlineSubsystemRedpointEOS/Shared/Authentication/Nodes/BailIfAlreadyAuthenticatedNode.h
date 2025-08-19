// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FBailIfAlreadyAuthenticatedNode : public FAuthenticationGraphNode
{
public:
    UE_NONCOPYABLE(FBailIfAlreadyAuthenticatedNode);
    FBailIfAlreadyAuthenticatedNode() = default;
    virtual ~FBailIfAlreadyAuthenticatedNode() = default;

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FBailIfAlreadyAuthenticatedNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
