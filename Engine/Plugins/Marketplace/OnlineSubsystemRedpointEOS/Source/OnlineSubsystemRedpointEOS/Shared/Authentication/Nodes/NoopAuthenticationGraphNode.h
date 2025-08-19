// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraphNode.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FNoopAuthenticationGraphNode : public FAuthenticationGraphNode
{
public:
    UE_NONCOPYABLE(FNoopAuthenticationGraphNode);
    FNoopAuthenticationGraphNode() = default;
    virtual ~FNoopAuthenticationGraphNode() = default;

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FNoopAuthenticationGraphNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
