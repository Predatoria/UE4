// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

EOS_ENABLE_STRICT_WARNINGS

enum ESelectCrossPlatformAccountMode
{
    /**
     * The default behaviour; the cross-platform account will be selected, even if there's no
     * EOS account for it.
     */
    PermitContinuanceToken,

    /**
     * If the cross-platform account is defined by a continuance token (no EOS account), then
     * authentication will fail with an error.
     */
    ExistingAccountOnly,
};

class ONLINESUBSYSTEMREDPOINTEOS_API FSelectCrossPlatformAccountNode : public FAuthenticationGraphNode
{
private:
    ESelectCrossPlatformAccountMode Mode;

public:
    UE_NONCOPYABLE(FSelectCrossPlatformAccountNode);
    FSelectCrossPlatformAccountNode(
        ESelectCrossPlatformAccountMode InMode = ESelectCrossPlatformAccountMode::PermitContinuanceToken)
        : Mode(InMode){};
    virtual ~FSelectCrossPlatformAccountNode() = default;

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FSelectCrossPlatformAccountNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
