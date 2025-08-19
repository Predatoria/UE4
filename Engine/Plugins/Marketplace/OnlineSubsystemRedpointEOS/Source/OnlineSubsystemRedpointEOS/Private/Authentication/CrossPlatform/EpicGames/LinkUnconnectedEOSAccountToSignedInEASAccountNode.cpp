// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/LinkUnconnectedEOSAccountToSignedInEASAccountNode.h"

EOS_ENABLE_STRICT_WARNINGS

void FLinkUnconnectedEOSAccountToSignedInEASAccountNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
    // For this node, we expect:
    //
    // - An EOS candidate that is an Epic Games account, which is currently signed in as per the
    // State->AuthenticatedEpicAccountId.
    // - An EOS candidate for the native platform that has a continuance token (and is not a signed in account).
    //

    if (State->EOSCandidates.Num() != 2)
    {
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        return;
    }

    bool bHasEASCandidate = false;
    bool bHasContinuanceCandidate = false;
    EOS_ContinuanceToken ContinuanceToken = {};
    for (const auto &Candidate : State->EOSCandidates)
    {
        if (Candidate.Type == EAuthenticationGraphEOSCandidateType::CrossPlatform &&
            State->GetSelectedEOSCandidate().Type == EAuthenticationGraphEOSCandidateType::CrossPlatform)
        {
            bHasEASCandidate = true;
        }
        else if (
            Candidate.Type == EAuthenticationGraphEOSCandidateType::Generic && Candidate.ContinuanceToken != nullptr)
        {
            bHasContinuanceCandidate = true;
            ContinuanceToken = Candidate.ContinuanceToken;
        }
    }

    if (!bHasEASCandidate || !bHasContinuanceCandidate)
    {
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        return;
    }

    EOS_Connect_LinkAccountOptions Opts = {};
    Opts.ApiVersion = EOS_CONNECT_LINKACCOUNT_API_LATEST;
    Opts.ContinuanceToken = ContinuanceToken;
    Opts.LocalUserId = State->GetSelectedEOSCandidate().ProductUserId;

    EOSRunOperation<EOS_HConnect, EOS_Connect_LinkAccountOptions, EOS_Connect_LinkAccountCallbackInfo>(
        State->EOSConnect,
        &Opts,
        EOS_Connect_LinkAccount,
        [WeakThis = GetWeakThis(this), State, OnDone](const EOS_Connect_LinkAccountCallbackInfo *Info) {
            if (Info->ResultCode != EOS_EResult::EOS_Success)
            {
                UE_LOG(
                    LogEOS,
                    Error,
                    TEXT("LinkUnconnectedEOSAccountToSignedInEASAccountNode: Unable to associate native credential "
                         "with EOS account signed in with Epic Games: %s"),
                    ANSI_TO_TCHAR(EOS_EResult_ToString(Info->ResultCode)));
            }

            OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        });
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION