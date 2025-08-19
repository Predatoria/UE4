// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FLinkUnconnectedEOSAccountToSignedInEASAccountNode
    : public FAuthenticationGraphNode
{
public:
    UE_NONCOPYABLE(FLinkUnconnectedEOSAccountToSignedInEASAccountNode);
    FLinkUnconnectedEOSAccountToSignedInEASAccountNode() = default;
    virtual ~FLinkUnconnectedEOSAccountToSignedInEASAccountNode() = default;

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FLinkUnconnectedEOSAccountToSignedInEASAccountNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
