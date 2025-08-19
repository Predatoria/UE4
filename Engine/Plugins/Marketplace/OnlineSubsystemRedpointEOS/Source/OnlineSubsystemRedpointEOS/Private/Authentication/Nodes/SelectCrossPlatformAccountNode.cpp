// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/SelectCrossPlatformAccountNode.h"

#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"

EOS_ENABLE_STRICT_WARNINGS

void FSelectCrossPlatformAccountNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
    check(!State->HasSelectedEOSCandidate());

    for (const auto &Candidate : State->EOSCandidates)
    {
        if (Candidate.Type == EAuthenticationGraphEOSCandidateType::CrossPlatform &&
            (EOSString_ProductUserId::IsValid(Candidate.ProductUserId) ||
             !EOSString_ContinuanceToken::IsNone(Candidate.ContinuanceToken)))
        {
            if (this->Mode == ESelectCrossPlatformAccountMode::ExistingAccountOnly)
            {
                if (!EOSString_ContinuanceToken::IsNone(Candidate.ContinuanceToken))
                {
                    // The cross-platform account doesn't have an EOS account. Prevent implicit
                    // EOS account creation.
                    State->ErrorMessages.Add(
                        TEXT("The chosen cross-platform account hasn't signed in to this game before. If you've "
                             "previously played this game on another device, log in there and connect your account to "
                             "your cross-platform account first, and then try signing in again."));
                    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
                    return;
                }
            }

            State->SelectEOSCandidate(Candidate);
            OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
            return;
        }
    }

    State->ErrorMessages.Add(
        TEXT("SelectCrossPlatformAccountNode ran but there were no cross-platform candidates available"));
    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION