// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/SelectSingleSuccessfulEOSAccountNode.h"

#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"

EOS_ENABLE_STRICT_WARNINGS

void FSelectSingleSuccessfulEOSAccountNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
    for (const auto &Candidate : State->EOSCandidates)
    {
        if (EOSString_ProductUserId::IsValid(Candidate.ProductUserId))
        {
            if (State->HasSelectedEOSCandidate())
            {
                State->ErrorMessages.Add(
                    TEXT("SelectSingleSuccessfulEOSAccountNode hit with multiple valid candidates"));
                OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
                return;
            }
            State->SelectEOSCandidate(Candidate);
        }
    }

    if (!State->HasSelectedEOSCandidate())
    {
        State->ErrorMessages.Add(TEXT("SelectSingleSuccessfulEOSAccountNode hit with no valid candidates"));
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION