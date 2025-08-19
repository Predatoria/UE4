// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FGetEASContinuanceTokenOrAccountForExistingExternalCredentialsNode
    : public FAuthenticationGraphNode
{
private:
    void OnResultCallback(
        const EOS_Auth_LoginCallbackInfo *Data,
        TSharedRef<FAuthenticationGraphState> State,
        FAuthenticationGraphNodeOnDone OnDone);

public:
    UE_NONCOPYABLE(FGetEASContinuanceTokenOrAccountForExistingExternalCredentialsNode);
    FGetEASContinuanceTokenOrAccountForExistingExternalCredentialsNode() = default;
    virtual ~FGetEASContinuanceTokenOrAccountForExistingExternalCredentialsNode() = default;

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FGetEASContinuanceTokenOrAccountForExistingExternalCredentialsNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
