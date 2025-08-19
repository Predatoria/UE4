// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION && EOS_OCULUS_ENABLED

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystemOculus.h"

EOS_ENABLE_STRICT_WARNINGS

class FGetExternalCredentialsFromOculusNode : public FAuthenticationGraphNode
{
private:
    void OnUserProofReceived(
        ovrMessageHandle InHandle,
        bool bIsError,
        ovrID InUserId,
        FString InDisplayName,
        TSharedRef<FAuthenticationGraphState> InState,
        FAuthenticationGraphNodeOnDone InOnDone);

    void OnUserDetailsReceived(
        ovrMessageHandle InHandle,
        bool bIsError,
        ovrID InUserId,
        TSharedRef<FAuthenticationGraphState> InState,
        FAuthenticationGraphNodeOnDone InOnDone);

public:
    FGetExternalCredentialsFromOculusNode() = default;
    UE_NONCOPYABLE(FGetExternalCredentialsFromOculusNode);

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FGetExternalCredentialsFromOculusNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION && EOS_OCULUS_ENABLED