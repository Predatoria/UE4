// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Graphs/AuthenticationGraphDevAuthTool.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraphRegistry.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/ChainEASResultToEOSNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/TryDeveloperAuthenticationEASCredentialsNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGamesCrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/AuthenticationGraphNodeUntil_CrossPlatformAccountPresent.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/AuthenticationGraphNodeUntil_Forever.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/AuthenticationGraphNodeUntil_LoginComplete.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/BailIfAlreadyAuthenticatedNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/LoginWithSelectedEOSAccountNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/SelectCrossPlatformAccountNode.h"

EOS_ENABLE_STRICT_WARNINGS

TSharedRef<FAuthenticationGraphNode> FAuthenticationGraphDevAuthTool::CreateGraph(
    const TSharedRef<FAuthenticationGraphState> &InitialState)
{
    // Force the cross-platform provider to be Epic Games if we are using this provider.
    InitialState->CrossPlatformAccountProvider = MakeShared<FEpicGamesCrossPlatformAccountProvider>();

    return MakeShared<FAuthenticationGraphNodeUntil_Forever>()
        ->Add(MakeShared<FBailIfAlreadyAuthenticatedNode>())
        ->Add(MakeShared<FAuthenticationGraphNodeUntil_LoginComplete>(
                  TEXT("The credential provided by the Developer Authentication Tool could not be used. Credentials in "
                       "the Developer Authentication Tool expire after about 4 hours, so please restart the Developer "
                       "Authentication Tool and try again."))
                  ->Add(MakeShared<FAuthenticationGraphNodeUntil_CrossPlatformAccountPresent>(
                            TEXT("Unable to authenticate with Developer Authentication Tool. Ensure you have started "
                                 "it using "
                                 "the EOS dropdown in the toolbar."),
                            TEXT("Authentication"),
                            TEXT("UnableToAuthenticateWithDevTool"))
                            ->Add(MakeShared<FTryPIEDeveloperAuthenticationEASCredentialsNode>())
                            ->Add(MakeShared<FTryDefaultDeveloperAuthenticationEASCredentialsNode>()))
                  ->Add(MakeShared<FChainEASResultToEOSNode>())
                  ->Add(MakeShared<FSelectCrossPlatformAccountNode>())
                  ->Add(MakeShared<FLoginWithSelectedEOSAccountNode>()));
}

void FAuthenticationGraphDevAuthTool::Register()
{
    FAuthenticationGraphRegistry::Register(
        EOS_AUTH_GRAPH_DEV_AUTH_TOOL,
        NSLOCTEXT("OnlineSubsystemRedpointEOS", "AuthGraph_DevAuthTool", "Developer Authentication Tool Only"),
        MakeShared<FAuthenticationGraphDevAuthTool>());
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION