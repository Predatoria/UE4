// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

EOS_ENABLE_STRICT_WARNINGS

void FAuthenticationGraph::__RegisterNode(const FName &InNodeName, const TSharedPtr<FAuthenticationGraphNode> &InNode)
{
    this->RegisteredNodes.Add(InNodeName, InNode);
}

TSharedPtr<FAuthenticationGraphNode> FAuthenticationGraph::__GetNode(const FName &InNodeName)
{
    return this->RegisteredNodes[InNodeName];
}

bool FAuthenticationGraph::Condition_Unauthenticated(const FAuthenticationGraphState &State)
{
    return !State.ExistingUserId.IsValid();
}

bool FAuthenticationGraph::Condition_HasCrossPlatformAccountProvider(const FAuthenticationGraphState &State)
{
    return State.CrossPlatformAccountProvider.IsValid();
}

bool FAuthenticationGraph::Condition_RequireCrossPlatformAccount(const FAuthenticationGraphState &State)
{
    return State.Config->GetRequireCrossPlatformAccount();
}

bool FAuthenticationGraph::Condition_CrossPlatformAccountIsValid(const FAuthenticationGraphState &State)
{
    return State.AuthenticatedCrossPlatformAccountId.IsValid();
}

bool FAuthenticationGraph::Condition_CrossPlatformAccountIsValidWithPUID(const FAuthenticationGraphState &State)
{
    if (!State.AuthenticatedCrossPlatformAccountId.IsValid())
    {
        return false;
    }

    for (const auto &Candidate : State.EOSCandidates)
    {
        if (Candidate.Type == EAuthenticationGraphEOSCandidateType::CrossPlatform &&
            !EOSString_ContinuanceToken::IsNone(Candidate.ContinuanceToken))
        {
            // The cross-platform candidate doesn't have an EOS account (no PUID).
            return false;
        }
    }

    return true;
}

bool FAuthenticationGraph::Condition_OneSuccessfulCandidate(const FAuthenticationGraphState &State)
{
    int SuccessCount = 0;
    for (const auto &Candidate : State.EOSCandidates)
    {
        if (EOSString_ProductUserId::IsValid(Candidate.ProductUserId))
        {
            SuccessCount++;
        }
    }

    // Exactly one success.
    return SuccessCount == 1;
}

bool FAuthenticationGraph::Condition_MoreThanOneSuccessfulCandidate(const FAuthenticationGraphState &State)
{
    int SuccessCount = 0;
    for (const auto &Candidate : State.EOSCandidates)
    {
        if (EOSString_ProductUserId::IsValid(Candidate.ProductUserId))
        {
            SuccessCount++;
        }
    }

    // Multiple success.
    return SuccessCount > 1;
}

bool FAuthenticationGraph::Condition_NoSuccessfulCandidate_AtLeastOneContinuanceToken(
    const FAuthenticationGraphState &State)
{
    int SuccessCount = 0;
    int ContinuanceCount = 0;
    for (const auto &Candidate : State.EOSCandidates)
    {
        if (EOSString_ProductUserId::IsValid(Candidate.ProductUserId))
        {
            SuccessCount++;
        }
        if (!EOSString_ContinuanceToken::IsNone(Candidate.ContinuanceToken))
        {
            ContinuanceCount++;
        }
    }

    // No success, at least one continuation token.
    return SuccessCount == 0 && ContinuanceCount > 0;
}

bool FAuthenticationGraph::Condition_NoSuccessfulCandidate_NoContinuanceToken(const FAuthenticationGraphState &State)
{
    int SuccessCount = 0;
    int ContinuanceCount = 0;
    for (const auto &Candidate : State.EOSCandidates)
    {
        if (EOSString_ProductUserId::IsValid(Candidate.ProductUserId))
        {
            SuccessCount++;
        }
        if (!EOSString_ContinuanceToken::IsNone(Candidate.ContinuanceToken))
        {
            ContinuanceCount++;
        }
    }

    // No success, no continuation token, EAS enabled/available.
    return SuccessCount == 0 && ContinuanceCount == 0;
}

bool FAuthenticationGraph::Condition_CrossPlatformProvidedContinuanceToken(const FAuthenticationGraphState &State)
{
    for (const auto &Candidate : State.EOSCandidates)
    {
        if (Candidate.Type == EAuthenticationGraphEOSCandidateType::CrossPlatform)
        {
            if (!EOSString_ProductUserId::IsValid(Candidate.ProductUserId) &&
                !EOSString_ContinuanceToken::IsNone(Candidate.ContinuanceToken))
            {
                return true;
            }
        }
    }

    return false;
}

bool FAuthenticationGraph::Condition_IsSwitchToCrossPlatformAccount(const FAuthenticationGraphState &State)
{
    return State.LastSwitchChoice == EEOSUserInterface_SwitchToCrossPlatformAccount_Choice::SwitchToThisAccount;
}

bool FAuthenticationGraph::Condition_IsLinkADifferentAccount(const FAuthenticationGraphState &State)
{
    return State.LastSwitchChoice == EEOSUserInterface_SwitchToCrossPlatformAccount_Choice::LinkADifferentAccount;
}

void FAuthenticationGraph::Execute(
    const TSharedRef<FAuthenticationGraphState> &InitialState,
    const FAuthenticationGraphNodeOnDone &OnDone)
{
    this->Start = this->CreateGraph(InitialState);

    UE_LOG(LogEOS, Verbose, TEXT("Authentication graph is starting"));
    UE_LOG(LogEOS, Verbose, TEXT("Authentication graph entering node: %s"), *Start->GetDebugName());
    this->ForwardingOnDone = OnDone;
    this->Start->Execute(
        InitialState,
        FAuthenticationGraphNodeOnDone::CreateSP(this, &FAuthenticationGraph::HandleOnDone, InitialState));
}

void FAuthenticationGraph::HandleOnDone(
    EAuthenticationGraphNodeResult Result,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<FAuthenticationGraphState> ResultingState)
{
    UE_LOG(LogEOS, Verbose, TEXT("Authentication graph exiting node: %s"), *this->Start->GetDebugName());

    if (Result == EAuthenticationGraphNodeResult::Error)
    {
        if (ResultingState->CleanupNodesOnError.Num() > 0)
        {
            auto Node = ResultingState->CleanupNodesOnError[0];
            ResultingState->CleanupNodesOnError.RemoveAt(0);

            UE_LOG(
                LogEOS,
                Verbose,
                TEXT("Authentication graph is returning with an error, need to run cleanup node %s first"),
                *Node->GetDebugName());
            UE_LOG(LogEOS, Verbose, TEXT("Authentication graph entering node: %s"), *Node->GetDebugName());
            Node->Execute(
                ResultingState,
                FAuthenticationGraphNodeOnDone::CreateSP(
                    this,
                    &FAuthenticationGraph::HandleCleanupOnDone,
                    ResultingState,
                    Result));
            return;
        }
    }

    UE_LOG(LogEOS, Verbose, TEXT("Authentication graph is complete"));
    this->ForwardingOnDone.ExecuteIfBound(Result);
}

void FAuthenticationGraph::HandleCleanupOnDone(
    EAuthenticationGraphNodeResult Result,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<FAuthenticationGraphState> ResultingState,
    EAuthenticationGraphNodeResult OriginalResult)
{
    UE_LOG(LogEOS, Verbose, TEXT("Authentication graph exiting node: %s"), *this->Start->GetDebugName());

    if (Result == EAuthenticationGraphNodeResult::Error)
    {
        UE_LOG(LogEOS, Warning, TEXT("Authentication graph cleanup encountered an error, continuing cleanup anyway"));
    }

    if (OriginalResult == EAuthenticationGraphNodeResult::Error)
    {
        if (ResultingState->CleanupNodesOnError.Num() > 0)
        {
            auto Node = ResultingState->CleanupNodesOnError[0];
            ResultingState->CleanupNodesOnError.RemoveAt(0);

            UE_LOG(
                LogEOS,
                Verbose,
                TEXT("Authentication graph is returning with an error, need to run cleanup node %s first"),
                *Node->GetDebugName());
            Node->Execute(
                ResultingState,
                FAuthenticationGraphNodeOnDone::CreateSP(this, &FAuthenticationGraph::HandleOnDone, ResultingState));
            return;
        }
    }

    UE_LOG(LogEOS, Verbose, TEXT("Authentication graph is complete"));
    this->ForwardingOnDone.ExecuteIfBound(OriginalResult);
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION