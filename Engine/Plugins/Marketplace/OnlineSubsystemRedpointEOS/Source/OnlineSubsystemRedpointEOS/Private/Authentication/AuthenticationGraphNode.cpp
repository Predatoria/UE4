// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraphNode.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

EOS_ENABLE_STRICT_WARNINGS

TSharedRef<FAuthenticationGraphNode> FAuthenticationGraphNode::RegisterNode(
    class FAuthenticationGraph *InGraph,
    FName InNodeName)
{
    InGraph->__RegisterNode(InNodeName, this->AsShared());
    return this->AsShared();
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION