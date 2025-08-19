// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EOSUserInterface_CandidateEOSAccount.h"
#include "UObject/Interface.h"

#include "EOSUserInterface_SelectEOSAccount.generated.h"

EOS_ENABLE_STRICT_WARNINGS

DECLARE_DELEGATE_OneParam(
    FEOSUserInterface_SelectEOSAccount_OnSelectCandidate,
    FEOSUserInterface_CandidateEOSAccount /* SelectedCandidate */);

UCLASS(BlueprintType) class UEOSUserInterface_SelectEOSAccount_Context : public UObject
{
    GENERATED_BODY()

private:
    FEOSUserInterface_SelectEOSAccount_OnSelectCandidate OnSelectCandidate;

public:
    void SetOnSelectCandidate(FEOSUserInterface_SelectEOSAccount_OnSelectCandidate InOnSelectCandidate)
    {
        this->OnSelectCandidate = MoveTemp(InOnSelectCandidate);
    }

    UFUNCTION(BlueprintCallable, Category = "EOS|User Interface")
    virtual void SelectCandidate(FEOSUserInterface_CandidateEOSAccount SelectedCandidate)
    {
        this->OnSelectCandidate.ExecuteIfBound(MoveTemp(SelectedCandidate));
    }
};

UINTERFACE(MinimalAPI, Blueprintable)
class UEOSUserInterface_SelectEOSAccount : public UInterface
{
    GENERATED_BODY()
};

class ONLINESUBSYSTEMREDPOINTEOS_API IEOSUserInterface_SelectEOSAccount
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "EOS|User Interface")
    void SetupUserInterface(
        UEOSUserInterface_SelectEOSAccount_Context *Context,
        const TArray<FEOSUserInterface_CandidateEOSAccount> &AvailableCandidates);
};

EOS_DISABLE_STRICT_WARNINGS