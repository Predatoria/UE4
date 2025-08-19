// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FFailAuthenticationDueToConflictingAccountsNode : public FAuthenticationGraphNode
{
public:
    UE_NONCOPYABLE(FFailAuthenticationDueToConflictingAccountsNode);
    FFailAuthenticationDueToConflictingAccountsNode() = default;
    virtual ~FFailAuthenticationDueToConflictingAccountsNode() = default;

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FFailAuthenticationDueToConflictingAccountsNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
