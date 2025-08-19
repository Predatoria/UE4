// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/AuthenticationGraphNodeConditional.h"

EOS_ENABLE_STRICT_WARNINGS

void FAuthenticationGraphNodeConditional::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
    for (const auto &ConditionalNode : this->Conditions)
    {
        checkf(
            ConditionalNode.Key.IsBound(),
            TEXT("Conditional authentication nodes must have a callback bound to the delegate."));
        if (ConditionalNode.Key.Execute(*State))
        {
            TSharedPtr<FAuthenticationGraphNode> Node = ConditionalNode.Value;
            UE_LOG(LogEOS, Verbose, TEXT("Authentication graph entering node: %s"), *Node->GetDebugName());
            Node->Execute(
                State,
                FAuthenticationGraphNodeOnDone::CreateSP(
                    this,
                    &FAuthenticationGraphNodeConditional::HandleOnDone,
                    Node,
                    OnDone));
            return;
        }
    }

    // No matching condition, treat as failure.
    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
}

void FAuthenticationGraphNodeConditional::HandleOnDone(
    EAuthenticationGraphNodeResult Result,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedPtr<FAuthenticationGraphNode> Node,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FAuthenticationGraphNodeOnDone OnDone)
{
    UE_LOG(LogEOS, Verbose, TEXT("Authentication graph exiting node: %s"), *Node->GetDebugName());

    OnDone.ExecuteIfBound(Result);
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION