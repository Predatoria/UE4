// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/JumpToNamedNode.h"

#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

EOS_ENABLE_STRICT_WARNINGS

void FJumpToNamedNode::Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone)
{
    TSharedPtr<FAuthenticationGraphNode> Node = this->Graph->__GetNode(this->TargetName);

    UE_LOG(LogEOS, Verbose, TEXT("Authentication graph entering node: %s"), *Node->GetDebugName());
    Node->Execute(State, FAuthenticationGraphNodeOnDone::CreateSP(this, &FJumpToNamedNode::HandleOnDone, Node, OnDone));
}

void FJumpToNamedNode::HandleOnDone(
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