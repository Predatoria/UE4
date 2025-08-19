// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGamesCrossPlatformAccountProvider.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FSignOutEASAccountNode : public FAuthenticationGraphNode
{
private:
    TSharedRef<const FEpicGamesCrossPlatformAccountId> AccountId;

public:
    UE_NONCOPYABLE(FSignOutEASAccountNode);
    FSignOutEASAccountNode(const TSharedRef<const FEpicGamesCrossPlatformAccountId> &InAccountId)
        : AccountId(InAccountId){};
    virtual ~FSignOutEASAccountNode() = default;

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FSignOutEASAccountNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
