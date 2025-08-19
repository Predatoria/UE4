// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/AuthenticationGraphNodeUntil.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FAuthenticationGraphNodeUntil_Forever : public FAuthenticationGraphNodeUntil
{
private:
    static bool Condition(const FAuthenticationGraphState &State)
    {
        return false;
    }
    virtual bool RequireConditionPass() const override
    {
        return false;
    }

public:
    FAuthenticationGraphNodeUntil_Forever()
        : FAuthenticationGraphNodeUntil(
              FAuthenticationGraphCondition::CreateStatic(&FAuthenticationGraphNodeUntil_Forever::Condition)){};
    UE_NONCOPYABLE(FAuthenticationGraphNodeUntil_Forever);
    virtual ~FAuthenticationGraphNodeUntil_Forever() = default;

    virtual FString GetDebugName() const override
    {
        return TEXT("FAuthenticationGraphNodeUntil_Forever");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION