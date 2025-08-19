// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraphNode.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FAuthenticationGraphNodeConditional : public FAuthenticationGraphNode
{
private:
    TArray<TTuple<FAuthenticationGraphCondition, TSharedPtr<FAuthenticationGraphNode>>> Conditions;

    void HandleOnDone(
        EAuthenticationGraphNodeResult Result,
        TSharedPtr<FAuthenticationGraphNode> Node,
        FAuthenticationGraphNodeOnDone OnDone);

    static bool AlwaysTrue(const FAuthenticationGraphState &State)
    {
        return true;
    }

protected:
    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) final;

public:
    UE_NONCOPYABLE(FAuthenticationGraphNodeConditional);
    FAuthenticationGraphNodeConditional() = default;
    virtual ~FAuthenticationGraphNodeConditional() = default;

    TSharedRef<FAuthenticationGraphNodeConditional> If(
        FAuthenticationGraphCondition Condition,
        TSharedRef<FAuthenticationGraphNode> Node)
    {
        this->Conditions.Add(
            TTuple<FAuthenticationGraphCondition, TSharedPtr<FAuthenticationGraphNode>>(Condition, Node));
        return StaticCastSharedRef<FAuthenticationGraphNodeConditional>(this->AsShared());
    }

    TSharedRef<FAuthenticationGraphNodeConditional> Else(TSharedRef<FAuthenticationGraphNode> Node)
    {
        this->Conditions.Add(TTuple<FAuthenticationGraphCondition, TSharedPtr<FAuthenticationGraphNode>>(
            FAuthenticationGraphCondition::CreateStatic(&FAuthenticationGraphNodeConditional::AlwaysTrue),
            Node));
        return StaticCastSharedRef<FAuthenticationGraphNodeConditional>(this->AsShared());
    }

    virtual FString GetDebugName() const override
    {
        return TEXT("FAuthenticationGraphNodeConditional");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
