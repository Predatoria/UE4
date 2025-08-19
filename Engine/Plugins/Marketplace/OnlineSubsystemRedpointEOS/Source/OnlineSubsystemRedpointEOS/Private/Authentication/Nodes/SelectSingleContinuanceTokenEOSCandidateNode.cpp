// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/SelectSingleContinuanceTokenEOSCandidateNode.h"

#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"

EOS_ENABLE_STRICT_WARNINGS

void FSelectSingleContinuanceTokenEOSCandidateNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
    for (const auto &Candidate : State->EOSCandidates)
    {
        if (!EOSString_ContinuanceToken::IsNone(Candidate.ContinuanceToken))
        {
            if (State->HasSelectedEOSCandidate())
            {
                State->ErrorMessages.Add(
                    TEXT("SelectSingleContinuanceTokenEOSCandidateNode hit with multiple valid candidates"));
                OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
                return;
            }
            State->SelectEOSCandidate(Candidate);
        }
    }

    if (!State->HasSelectedEOSCandidate())
    {
        State->ErrorMessages.Add(TEXT("SelectSingleContinuanceTokenEOSCandidateNode hit with no valid candidates"));
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION