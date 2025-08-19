// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraphNode.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FAuthenticationGraphNodeUntil : public FAuthenticationGraphNode
{
private:
    TArray<TSharedRef<FAuthenticationGraphNode>> Sequence;
    TSharedPtr<FAuthenticationGraphState> CurrentState;
    int CurrentSequenceNum;
    FAuthenticationGraphNodeOnDone CurrentSequenceDone;
    FAuthenticationGraphCondition UntilCondition;
    FString ErrorMessage;
#if WITH_EDITOR
    FString EditorSignalContext;
    FString EditorSignalId;
#endif

    void Continue(EAuthenticationGraphNodeResult Result);
    void HandleOnDone(EAuthenticationGraphNodeResult Result, TSharedPtr<FAuthenticationGraphNode> Node);

protected:
    FAuthenticationGraphNodeUntil() = delete;
    UE_NONCOPYABLE(FAuthenticationGraphNodeUntil);
    FAuthenticationGraphNodeUntil(
        FAuthenticationGraphCondition InUntilCondition,
        FString InErrorMessage = TEXT(""),
        // NOLINTNEXTLINE(performance-unnecessary-value-param)
        FString InEditorSignalContext = TEXT(""),
        // NOLINTNEXTLINE(performance-unnecessary-value-param)
        FString InEditorSignalId = TEXT(""))
        : CurrentState(nullptr)
        , CurrentSequenceNum(0)
        , UntilCondition(MoveTemp(InUntilCondition))
        , ErrorMessage(MoveTemp(InErrorMessage))
#if WITH_EDITOR
        , EditorSignalContext(MoveTemp(InEditorSignalContext))
        , EditorSignalId(MoveTemp(InEditorSignalId))
#endif
              {};

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) final;
    virtual bool RequireConditionPass() const
    {
        return true;
    }

public:
    virtual ~FAuthenticationGraphNodeUntil() = default;

    TSharedRef<FAuthenticationGraphNodeUntil> Add(const TSharedRef<FAuthenticationGraphNode> &Node)
    {
        this->Sequence.Add(Node);
        return StaticCastSharedRef<FAuthenticationGraphNodeUntil>(this->AsShared());
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
