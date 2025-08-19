// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Graphs/AuthenticationGraphAlwaysFail.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraphRegistry.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/FailAuthenticationNode.h"

EOS_ENABLE_STRICT_WARNINGS

TSharedRef<FAuthenticationGraphNode> FAuthenticationGraphAlwaysFail::CreateGraph(
    const TSharedRef<FAuthenticationGraphState> &InitialState)
{
    return MakeShared<FFailAuthenticationNode>(TEXT("There is no usable authentication mechanism on this platform."));
}

void FAuthenticationGraphAlwaysFail::Register()
{
    FAuthenticationGraphRegistry::Register(
        EOS_AUTH_GRAPH_ALWAYS_FAIL,
        NSLOCTEXT("OnlineSubsystemRedpointEOS", "AuthGraph_AlwaysFail", "Always Fail"),
        MakeShared<FAuthenticationGraphAlwaysFail>());
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION