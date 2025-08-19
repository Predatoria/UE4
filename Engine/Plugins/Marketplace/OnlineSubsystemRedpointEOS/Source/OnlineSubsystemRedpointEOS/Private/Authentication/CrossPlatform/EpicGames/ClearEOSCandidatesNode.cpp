// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/ClearEOSCandidatesNode.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraphState.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/CrossPlatformAccountId.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"

EOS_ENABLE_STRICT_WARNINGS

void FClearEOSCandidatesNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
    State->EOSCandidates.Empty();
    State->AuthenticatedCrossPlatformAccountId.Reset();
    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION