// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Graphs/AuthenticationGraphCrossPlatformOnly.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraphRegistry.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/AuthenticationGraphNodeUntil_Forever.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/BailIfAlreadyAuthenticatedNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/FailAuthenticationNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/LoginWithSelectedEOSAccountNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/SelectCrossPlatformAccountNode.h"

EOS_ENABLE_STRICT_WARNINGS

TSharedRef<FAuthenticationGraphNode> FAuthenticationGraphCrossPlatformOnly::CreateGraph(
    const TSharedRef<FAuthenticationGraphState> &InitialState)
{
    if (InitialState->CrossPlatformAccountProvider.IsValid())
    {
        return MakeShared<FAuthenticationGraphNodeUntil_Forever>()
            ->Add(MakeShared<FBailIfAlreadyAuthenticatedNode>())
            ->Add(InitialState->CrossPlatformAccountProvider->GetInteractiveAuthenticationSequence())
            ->Add(MakeShared<FSelectCrossPlatformAccountNode>())
            ->Add(MakeShared<FLoginWithSelectedEOSAccountNode>());
    }
    else
    {
        return MakeShared<FFailAuthenticationNode>(TEXT("There is no cross-platform account provider configured."));
    }
}

void FAuthenticationGraphCrossPlatformOnly::Register()
{
    FAuthenticationGraphRegistry::Register(
        EOS_AUTH_GRAPH_CROSS_PLATFORM_ONLY,
        NSLOCTEXT("OnlineSubsystemRedpointEOS", "AuthGraph_CrossPlatformOnly", "Cross-Platform Only"),
        MakeShared<FAuthenticationGraphCrossPlatformOnly>());
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION