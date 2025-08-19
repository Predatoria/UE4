// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#if !defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FSelectAutomatedTestingEOSAccountNode : public FAuthenticationGraphNode
{
public:
    UE_NONCOPYABLE(FSelectAutomatedTestingEOSAccountNode);
    FSelectAutomatedTestingEOSAccountNode() = default;
    virtual ~FSelectAutomatedTestingEOSAccountNode() = default;

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FSelectAutomatedTestingEOSAccountNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif

#endif // #if EOS_HAS_AUTHENTICATION
