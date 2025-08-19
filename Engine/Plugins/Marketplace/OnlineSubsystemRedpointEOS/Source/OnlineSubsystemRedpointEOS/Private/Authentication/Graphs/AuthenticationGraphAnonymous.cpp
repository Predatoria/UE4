// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Graphs/AuthenticationGraphAnonymous.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraphRegistry.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/AuthenticationGraphNodeConditional.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/AuthenticationGraphNodeUntil_Forever.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/AuthenticationGraphNodeUntil_LoginComplete.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/BailIfAlreadyAuthenticatedNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/CreateDeviceIdNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/LoginWithSelectedEOSAccountNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/SelectOnlyEOSCandidateNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/TryDeviceIdAuthenticationNode.h"

EOS_ENABLE_STRICT_WARNINGS

bool AnyCandidates(const FAuthenticationGraphState &State)
{
    return State.EOSCandidates.Num() > 0;
}

TSharedRef<FAuthenticationGraphNode> FAuthenticationGraphAnonymous::CreateGraph(
    const TSharedRef<FAuthenticationGraphState> &InitialState)
{
    return MakeShared<FAuthenticationGraphNodeUntil_Forever>()
        ->Add(MakeShared<FBailIfAlreadyAuthenticatedNode>())
        ->Add(MakeShared<FTryDeviceIdAuthenticationNode>())
        ->Add(MakeShared<FAuthenticationGraphNodeConditional>()
                  ->If(
                      FAuthenticationGraphCondition::CreateStatic(&AnyCandidates),
                      MakeShared<FAuthenticationGraphNodeUntil_LoginComplete>()
                          ->Add(MakeShared<FSelectOnlyEOSCandidateNode>())
                          ->Add(MakeShared<FLoginWithSelectedEOSAccountNode>()))
                  ->Else(MakeShared<FAuthenticationGraphNodeUntil_LoginComplete>()
                             ->Add(MakeShared<FCreateDeviceIdNode>())
                             ->Add(MakeShared<FTryDeviceIdAuthenticationNode>())
                             ->Add(MakeShared<FSelectOnlyEOSCandidateNode>())
                             ->Add(MakeShared<FLoginWithSelectedEOSAccountNode>())));
}

void FAuthenticationGraphAnonymous::Register()
{
    FAuthenticationGraphRegistry::Register(
        EOS_AUTH_GRAPH_ANONYMOUS,
        NSLOCTEXT("OnlineSubsystemRedpointEOS", "AuthGraph_Anonymous", "Anonymous"),
        MakeShared<FAuthenticationGraphAnonymous>());
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION