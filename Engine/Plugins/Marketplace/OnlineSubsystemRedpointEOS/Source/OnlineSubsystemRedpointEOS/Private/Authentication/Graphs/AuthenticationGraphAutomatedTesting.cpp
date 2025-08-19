// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#if !defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Graphs/AuthenticationGraphAutomatedTesting.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraphRegistry.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Graphs/AuthenticationGraphOnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/AuthenticationGraphNodeUntil_Forever.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/AuthenticationGraphNodeUntil_LoginComplete.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/BailIfAlreadyAuthenticatedNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/IssueJwtForAutomatedTestingNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/LoginWithSelectedEOSAccountNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/PerformOpenIdLoginForAutomatedTestingNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/SelectAutomatedTestingEOSAccountNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/SelectCrossPlatformAccountNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/SelectSingleSuccessfulEOSAccountNode.h"

EOS_ENABLE_STRICT_WARNINGS

TSharedRef<FAuthenticationGraphNode> FAuthenticationGraphAutomatedTesting::CreateGraph(
    const TSharedRef<FAuthenticationGraphState> &InitialState)
{
    if (InitialState->AutomatedTestingEmailAddress.StartsWith(TEXT("CreateOnDemand:")))
    {
        return MakeShared<FAuthenticationGraphNodeUntil_Forever>()
            ->Add(MakeShared<FBailIfAlreadyAuthenticatedNode>())
            ->Add(MakeShared<FIssueJwtForAutomatedTestingNode>())
            ->Add(MakeShared<FPerformOpenIdLoginForAutomatedTestingNode>())
            ->Add(MakeShared<FSelectAutomatedTestingEOSAccountNode>())
            ->Add(MakeShared<FLoginWithSelectedEOSAccountNode>());
    }
    else
    {
        checkf(
            InitialState->CrossPlatformAccountProvider.IsValid(),
            TEXT("Cross-platform account provider isn't set, but automated testing node requires the Epic Games "
                 "cross-platform account provider."));

        return MakeShared<FAuthenticationGraphNodeUntil_Forever>()
            ->Add(MakeShared<FBailIfAlreadyAuthenticatedNode>())
            ->Add(MakeShared<FAuthenticationGraphNodeUntil_LoginComplete>()
                      ->Add(InitialState->CrossPlatformAccountProvider->GetAutomatedTestingAuthenticationSequence())
                      ->Add(MakeShared<FSelectCrossPlatformAccountNode>())
                      ->Add(MakeShared<FLoginWithSelectedEOSAccountNode>()));
    }
}

void FAuthenticationGraphAutomatedTesting::Register()
{
    FAuthenticationGraphOnlineSubsystem::RegisterForCustomPlatform(
        EOS_AUTH_GRAPH_AUTOMATED_TESTING_OSS,
        NSLOCTEXT(
            "OnlineSubsystemRedpointEOS",
            "AuthGraph_AutomatedTestingOSS",
            "Automated Testing (Online Subsystem)"),
        NULL_SUBSYSTEM,
        EOS_EExternalCredentialType::EOS_ECT_OPENID_ACCESS_TOKEN,
        TEXT("unused"),
        TEXT("unused"));
}

EOS_DISABLE_STRICT_WARNINGS

#endif // !UE_BUILD_SHIPPING

#endif // #if EOS_HAS_AUTHENTICATION