// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

#include "Interfaces/OnlineIdentityInterface.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FGetExternalCredentialsFromOnlineSubsystemNode : public FAuthenticationGraphNode
{
private:
    FName SubsystemName;
    EOS_EExternalCredentialType ObtainedCredentialType;
    FString AuthenticatedWithValue;
    FString TokenAuthAttributeName;

public:
    UE_NONCOPYABLE(FGetExternalCredentialsFromOnlineSubsystemNode);
    virtual ~FGetExternalCredentialsFromOnlineSubsystemNode() = default;

    FGetExternalCredentialsFromOnlineSubsystemNode(
        FName InSubsystemName,
        EOS_EExternalCredentialType InObtainedCredentialType,
        const FString &InAuthenticatedWithValue,
        const FString &InTokenAuthAttributeName)
        : SubsystemName(InSubsystemName)
        , ObtainedCredentialType(InObtainedCredentialType)
        , AuthenticatedWithValue(InAuthenticatedWithValue)
        , TokenAuthAttributeName(InTokenAuthAttributeName)
    {
    }

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FGetExternalCredentialsFromOnlineSubsystemNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
