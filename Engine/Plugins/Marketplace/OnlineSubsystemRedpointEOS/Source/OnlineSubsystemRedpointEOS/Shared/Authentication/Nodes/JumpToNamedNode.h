// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FJumpToNamedNode : public FAuthenticationGraphNode
{
private:
    TSharedRef<FAuthenticationGraph> Graph;
    FName TargetName;

    void HandleOnDone(
        EAuthenticationGraphNodeResult Result,
        TSharedPtr<FAuthenticationGraphNode> Node,
        FAuthenticationGraphNodeOnDone OnDone);

public:
    UE_NONCOPYABLE(FJumpToNamedNode);
    virtual ~FJumpToNamedNode() = default;

    FJumpToNamedNode(TSharedRef<FAuthenticationGraph> InGraph, FName InTargetName)
        : Graph(MoveTemp(InGraph))
        , TargetName(InTargetName){};

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FJumpToNamedNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
