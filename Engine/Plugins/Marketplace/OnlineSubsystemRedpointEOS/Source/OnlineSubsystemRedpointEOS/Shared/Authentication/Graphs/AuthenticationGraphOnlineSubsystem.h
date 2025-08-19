// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

EOS_ENABLE_STRICT_WARNINGS

/**
 * This authentication graph leverages the Online Subsystem APIs to authenticate a user.
 */
class ONLINESUBSYSTEMREDPOINTEOS_API FAuthenticationGraphOnlineSubsystem : public FAuthenticationGraph
{
private:
    FName SubsystemName;
    EOS_EExternalCredentialType CredentialType;
    FString AuthenticatedWithValue;
    FString TokenAuthAttributeName;
    TSharedPtr<class FAuthenticationGraphNode> OverrideGetCredentialsNode;

protected:
    virtual TSharedRef<FAuthenticationGraphNode> CreateGraph(
        const TSharedRef<FAuthenticationGraphState> &InitialState) override;

public:
    UE_NONCOPYABLE(FAuthenticationGraphOnlineSubsystem);

    FAuthenticationGraphOnlineSubsystem(
        FName InSubsystemName,
        EOS_EExternalCredentialType InCredentialType,
        const FString &InAuthenticatedWithValue,
        const FString &InTokenAuthAttributeName,
        const TSharedPtr<class FAuthenticationGraphNode> &InOverrideGetCredentialsNode)
        : SubsystemName(InSubsystemName)
        , CredentialType(InCredentialType)
        , AuthenticatedWithValue(InAuthenticatedWithValue)
        , TokenAuthAttributeName(InTokenAuthAttributeName)
        , OverrideGetCredentialsNode(InOverrideGetCredentialsNode)
    {
    }

    virtual ~FAuthenticationGraphOnlineSubsystem(){};

    static void RegisterForCustomPlatform(
        FName InAuthenticationGraphName,
        const FText &GeneralDescription,
        FName InSubsystemName,
        EOS_EExternalCredentialType InCredentialType,
        const FString &InAuthenticatedWithValue,
        const FString &InTokenAuthAttributeName,
        const TSharedPtr<class FAuthenticationGraphNode> &InOverrideGetCredentialsNode = nullptr);
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
