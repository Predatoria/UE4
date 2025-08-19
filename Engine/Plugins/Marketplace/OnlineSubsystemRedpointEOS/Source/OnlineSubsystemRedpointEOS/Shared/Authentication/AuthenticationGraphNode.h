// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraphState.h"

EOS_ENABLE_STRICT_WARNINGS

enum class EAuthenticationGraphNodeResult : int8
{
    /* Continue the authentication process. */
    Continue,

    /* The authentication process encountered a permanent error, fail authentication. */
    Error,
};

DECLARE_DELEGATE_OneParam(FAuthenticationGraphNodeOnDone, EAuthenticationGraphNodeResult /* Result */);

DECLARE_DELEGATE_RetVal_OneParam(bool, FAuthenticationGraphCondition, const FAuthenticationGraphState & /*State*/);

class ONLINESUBSYSTEMREDPOINTEOS_API FAuthenticationGraphNode : public TSharedFromThis<FAuthenticationGraphNode>
{
public:
    UE_NONCOPYABLE(FAuthenticationGraphNode);
    FAuthenticationGraphNode() = default;
    virtual ~FAuthenticationGraphNode() = default;

    TSharedRef<FAuthenticationGraphNode> RegisterNode(class FAuthenticationGraph *InGraph, FName InNodeName);

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) = 0;
    virtual FString GetDebugName() const = 0;
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION