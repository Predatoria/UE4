// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/UserInterface/EOSUserInterface_SignInOrCreateAccount_Choice.h"
#include "UObject/Interface.h"

#include "EOSUserInterface_SignInOrCreateAccount.generated.h"

EOS_ENABLE_STRICT_WARNINGS

DECLARE_DELEGATE_OneParam(
    UEOSUserInterface_SignInOrCreateAccount_OnChoice,
    EEOSUserInterface_SignInOrCreateAccount_Choice /* Choice */);

UCLASS(BlueprintType) class UEOSUserInterface_SignInOrCreateAccount_Context : public UObject
{
    GENERATED_BODY()

private:
    UEOSUserInterface_SignInOrCreateAccount_OnChoice OnChoice;

public:
    void SetOnChoice(UEOSUserInterface_SignInOrCreateAccount_OnChoice InOnChoice)
    {
        this->OnChoice = MoveTemp(InOnChoice);
    }

    UFUNCTION(BlueprintCallable, Category = "EOS|User Interface")
    virtual void SelectChoice(EEOSUserInterface_SignInOrCreateAccount_Choice SelectedChoice)
    {
        this->OnChoice.ExecuteIfBound(SelectedChoice);
    }
};

UINTERFACE(MinimalAPI, Blueprintable)
class UEOSUserInterface_SignInOrCreateAccount : public UInterface
{
    GENERATED_BODY()
};

class ONLINESUBSYSTEMREDPOINTEOS_API IEOSUserInterface_SignInOrCreateAccount
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "EOS|User Interface")
    void SetupUserInterface(UEOSUserInterface_SignInOrCreateAccount_Context *Context);
};

EOS_DISABLE_STRICT_WARNINGS