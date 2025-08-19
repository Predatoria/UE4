// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#if !defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/AutomatedTesting/EmitLogForAutomatedTestingNode.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

EOS_ENABLE_STRICT_WARNINGS

FEmitLogForAutomatedTestingNode::FEmitLogForAutomatedTestingNode(const FString &InLog)
{
    this->Log = InLog;
}

void FEmitLogForAutomatedTestingNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
    // These are errors so that unexpected logs in the authentication graph flow will cause the unit tests to fail.
    UE_LOG(LogEOS, Error, TEXT("%s"), *this->Log);
    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if !UE_BUILD_SHIPPING

#endif // #if EOS_HAS_AUTHENTICATION