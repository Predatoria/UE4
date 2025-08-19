// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/UserInterface/EOSUserInterface_SwitchToCrossPlatformAccount_Choice.h"
#include "UObject/Interface.h"

#include "EOSUserInterface_SwitchToCrossPlatformAccount.generated.h"

EOS_ENABLE_STRICT_WARNINGS

DECLARE_DELEGATE_OneParam(
    UEOSUserInterface_SwitchToCrossPlatformAccount_OnChoice,
    EEOSUserInterface_SwitchToCrossPlatformAccount_Choice /* Choice */);

UCLASS(BlueprintType) class UEOSUserInterface_SwitchToCrossPlatformAccount_Context : public UObject
{
    GENERATED_BODY()

private:
    UEOSUserInterface_SwitchToCrossPlatformAccount_OnChoice OnChoice;

public:
    void SetOnChoice(UEOSUserInterface_SwitchToCrossPlatformAccount_OnChoice InOnChoice)
    {
        this->OnChoice = MoveTemp(InOnChoice);
    }

    UFUNCTION(BlueprintCallable, Category = "EOS|User Interface")
    virtual void SelectChoice(EEOSUserInterface_SwitchToCrossPlatformAccount_Choice SelectedChoice)
    {
        this->OnChoice.ExecuteIfBound(SelectedChoice);
    }
};

UINTERFACE(MinimalAPI, Blueprintable)
class UEOSUserInterface_SwitchToCrossPlatformAccount : public UInterface
{
    GENERATED_BODY()
};

class ONLINESUBSYSTEMREDPOINTEOS_API IEOSUserInterface_SwitchToCrossPlatformAccount
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "EOS|User Interface")
    void SetupUserInterface(
        UEOSUserInterface_SwitchToCrossPlatformAccount_Context *Context,
        const FString &EpicAccountName);
};

EOS_DISABLE_STRICT_WARNINGS