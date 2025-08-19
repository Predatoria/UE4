// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/AuthenticationGraphNodeUntil.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FAuthenticationGraphNodeUntil_CrossPlatformAccountPresent
    : public FAuthenticationGraphNodeUntil
{
private:
    bool AllowFailureFlag;

    static bool Condition(const FAuthenticationGraphState &State)
    {
        return State.AuthenticatedCrossPlatformAccountId.IsValid();
    }
    virtual bool RequireConditionPass() const override
    {
        return !this->AllowFailureFlag;
    }

public:
    FAuthenticationGraphNodeUntil_CrossPlatformAccountPresent(
        FString InErrorMessage = TEXT(""),
        FString InEditorSignalContext = TEXT(""),
        FString InEditorSignalId = TEXT(""))
        : FAuthenticationGraphNodeUntil(
              FAuthenticationGraphCondition::CreateStatic(
                  &FAuthenticationGraphNodeUntil_CrossPlatformAccountPresent::Condition),
              MoveTemp(InErrorMessage),
              MoveTemp(InEditorSignalContext),
              MoveTemp(InEditorSignalId))
        , AllowFailureFlag(false){};
    UE_NONCOPYABLE(FAuthenticationGraphNodeUntil_CrossPlatformAccountPresent);
    virtual ~FAuthenticationGraphNodeUntil_CrossPlatformAccountPresent() = default;

    virtual TSharedRef<FAuthenticationGraphNodeUntil_CrossPlatformAccountPresent> AllowFailure(bool Allow)
    {
        this->AllowFailureFlag = Allow;
        return StaticCastSharedRef<FAuthenticationGraphNodeUntil_CrossPlatformAccountPresent>(this->AsShared());
    }

    virtual FString GetDebugName() const override
    {
        return TEXT("FAuthenticationGraphNodeUntil_CrossPlatformAccountPresent");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
