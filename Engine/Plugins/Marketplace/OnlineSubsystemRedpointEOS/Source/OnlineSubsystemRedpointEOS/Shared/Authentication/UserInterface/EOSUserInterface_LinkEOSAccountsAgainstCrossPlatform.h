// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EOSUserInterface_CandidateEOSAccount.h"
#include "UObject/Interface.h"

#include "EOSUserInterface_LinkEOSAccountsAgainstCrossPlatform.generated.h"

EOS_ENABLE_STRICT_WARNINGS

DECLARE_DELEGATE_OneParam(
    FEOSUserInterface_LinkEOSAccountsAgainstCrossPlatform_OnSelectedCandidates,
    TArray<FEOSUserInterface_CandidateEOSAccount> /* SelectedCandidates */);

UCLASS(BlueprintType) class UEOSUserInterface_LinkEOSAccountsAgainstCrossPlatform_Context : public UObject
{
    GENERATED_BODY()

private:
    FEOSUserInterface_LinkEOSAccountsAgainstCrossPlatform_OnSelectedCandidates OnSelectedCandidates;

public:
    void SetOnSelectedCandidates(
        FEOSUserInterface_LinkEOSAccountsAgainstCrossPlatform_OnSelectedCandidates InOnSelectedCandidates)
    {
        this->OnSelectedCandidates = MoveTemp(InOnSelectedCandidates);
    }

    UFUNCTION(BlueprintCallable, Category = "EOS|User Interface")
    virtual void SelectedCandidates(TArray<FEOSUserInterface_CandidateEOSAccount> SelectedCandidates)
    {
        this->OnSelectedCandidates.ExecuteIfBound(MoveTemp(SelectedCandidates));
    }
};

UINTERFACE(MinimalAPI, Blueprintable)
class UEOSUserInterface_LinkEOSAccountsAgainstCrossPlatform : public UInterface
{
    GENERATED_BODY()
};

class ONLINESUBSYSTEMREDPOINTEOS_API IEOSUserInterface_LinkEOSAccountsAgainstCrossPlatform
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "EOS|User Interface")
    void SetupUserInterface(
        UEOSUserInterface_LinkEOSAccountsAgainstCrossPlatform_Context *Context,
        const TArray<FEOSUserInterface_CandidateEOSAccount> &AvailableCandidates);
};

EOS_DISABLE_STRICT_WARNINGS