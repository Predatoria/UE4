// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/AuthenticationGraphNodeUntil.h"

EOS_ENABLE_STRICT_WARNINGS

void FAuthenticationGraphNodeUntil::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
    if (this->Sequence.Num() == 0)
    {
        // No steps configured.
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    check(!this->CurrentState.IsValid());
    this->CurrentSequenceNum = 0;
    this->CurrentState = State;
    this->CurrentSequenceDone = OnDone;

    TSharedPtr<FAuthenticationGraphNode> Node = this->Sequence[this->CurrentSequenceNum];
    UE_LOG(LogEOS, Verbose, TEXT("Authentication graph entering node: %s"), *Node->GetDebugName());
    Node->Execute(
        this->CurrentState.ToSharedRef(),
        FAuthenticationGraphNodeOnDone::CreateSP(this, &FAuthenticationGraphNodeUntil::HandleOnDone, Node));
}

void FAuthenticationGraphNodeUntil::Continue(EAuthenticationGraphNodeResult Result)
{
    checkf(
        this->UntilCondition.IsBound(),
        TEXT("'Until' authentication nodes must have a callback bound to the until condition delegate."));
    if (Result == EAuthenticationGraphNodeResult::Error || this->UntilCondition.Execute(*this->CurrentState) ||
        this->CurrentSequenceNum >= this->Sequence.Num() - 1)
    {
        // Copy state and done handler out first, then reset them in case the OnDone
        // re-enters this sequence.
        TSharedPtr<FAuthenticationGraphState> CachedState = this->CurrentState;
        FAuthenticationGraphNodeOnDone CachedDone = this->CurrentSequenceDone;
        this->CurrentSequenceNum = -1;
        this->CurrentState = nullptr;
        this->CurrentSequenceDone = FAuthenticationGraphNodeOnDone();
        if (Result == EAuthenticationGraphNodeResult::Continue && this->RequireConditionPass())
        {
            if (!this->UntilCondition.Execute(*CachedState))
            {
                if (this->ErrorMessage.IsEmpty())
                {
                    CachedState->ErrorMessages.Add(
                        FString::Printf(TEXT("Condition was not met in sequence node: %s"), *this->GetDebugName()));
                }
                else
                {
                    CachedState->ErrorMessages.Add(this->ErrorMessage);
                }
#if WITH_EDITOR
                if (!this->EditorSignalContext.IsEmpty())
                {
                    EOSSendCustomEditorSignal(this->EditorSignalContext, this->EditorSignalId);
                }
#endif
                CachedDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
            }
            else
            {
                CachedDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
            }
        }
        else
        {
            CachedDone.ExecuteIfBound(Result);
        }
        return;
    }

    this->CurrentSequenceNum++;
    TSharedPtr<FAuthenticationGraphNode> Node = this->Sequence[this->CurrentSequenceNum];
    UE_LOG(LogEOS, Verbose, TEXT("Authentication graph entering node: %s"), *Node->GetDebugName());
    Node->Execute(
        this->CurrentState.ToSharedRef(),
        FAuthenticationGraphNodeOnDone::CreateSP(this, &FAuthenticationGraphNodeUntil::HandleOnDone, Node));
}

void FAuthenticationGraphNodeUntil::HandleOnDone(
    EAuthenticationGraphNodeResult Result,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedPtr<FAuthenticationGraphNode> Node)
{
    UE_LOG(LogEOS, Verbose, TEXT("Authentication graph exiting node: %s"), *Node->GetDebugName());

    this->Continue(Result);
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION