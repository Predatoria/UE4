// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FFailAuthenticationNode : public FAuthenticationGraphNode
{
private:
    FString ErrorMessage;

public:
    UE_NONCOPYABLE(FFailAuthenticationNode);
    FFailAuthenticationNode(FString InErrorMessage)
        : ErrorMessage(MoveTemp(InErrorMessage)){};
    virtual ~FFailAuthenticationNode() = default;

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FFailAuthenticationNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
