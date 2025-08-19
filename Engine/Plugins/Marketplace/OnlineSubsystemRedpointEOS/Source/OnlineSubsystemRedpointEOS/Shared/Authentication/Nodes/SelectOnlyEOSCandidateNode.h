// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FSelectOnlyEOSCandidateNode : public FAuthenticationGraphNode
{
public:
    UE_NONCOPYABLE(FSelectOnlyEOSCandidateNode);
    FSelectOnlyEOSCandidateNode() = default;
    virtual ~FSelectOnlyEOSCandidateNode() = default;

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FSelectOnlyEOSCandidateNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
