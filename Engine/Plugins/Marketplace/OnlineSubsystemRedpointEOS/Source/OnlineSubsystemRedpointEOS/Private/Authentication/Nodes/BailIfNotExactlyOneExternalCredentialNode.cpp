// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/BailIfNotExactlyOneExternalCredentialNode.h"

EOS_ENABLE_STRICT_WARNINGS

void FBailIfNotExactlyOneExternalCredentialNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
#if !defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING
    if (State->Config->IsAutomatedTesting())
    {
        // We are running through authentication unit tests, which are designed to test the logic flow of the OSS
        // authentication graph without actually requiring an external service.
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        return;
    }
#endif

    if (State->AvailableExternalCredentials.Num() != 1)
    {
        State->ErrorMessages.Add(TEXT("Authentication failed because there are no valid credentials"));
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
    }
    else
    {
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
    }
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION