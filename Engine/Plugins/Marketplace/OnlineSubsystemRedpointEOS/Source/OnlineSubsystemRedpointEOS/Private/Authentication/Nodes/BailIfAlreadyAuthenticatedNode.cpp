// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/BailIfAlreadyAuthenticatedNode.h"

EOS_ENABLE_STRICT_WARNINGS

void FBailIfAlreadyAuthenticatedNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
    if (State->ExistingUserId.IsValid())
    {
        State->ErrorMessages.Add(TEXT("User is already authenticated"));
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
    }
    else
    {
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
    }
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION