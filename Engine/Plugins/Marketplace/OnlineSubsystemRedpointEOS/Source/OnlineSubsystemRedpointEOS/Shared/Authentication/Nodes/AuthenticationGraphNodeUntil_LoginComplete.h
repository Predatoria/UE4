// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/AuthenticationGraphNodeUntil.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FAuthenticationGraphNodeUntil_LoginComplete : public FAuthenticationGraphNodeUntil
{
private:
    static bool Condition(const FAuthenticationGraphState &State)
    {
        return State.ResultUserId.IsValid();
    }

public:
    FAuthenticationGraphNodeUntil_LoginComplete(FString InErrorMessage = TEXT(""))
        : FAuthenticationGraphNodeUntil(
              FAuthenticationGraphCondition::CreateStatic(&FAuthenticationGraphNodeUntil_LoginComplete::Condition),
              MoveTemp(InErrorMessage)){};
    UE_NONCOPYABLE(FAuthenticationGraphNodeUntil_LoginComplete);
    virtual ~FAuthenticationGraphNodeUntil_LoginComplete() = default;

    virtual FString GetDebugName() const override
    {
        return TEXT("FAuthenticationGraphNodeUntil_LoginComplete");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
