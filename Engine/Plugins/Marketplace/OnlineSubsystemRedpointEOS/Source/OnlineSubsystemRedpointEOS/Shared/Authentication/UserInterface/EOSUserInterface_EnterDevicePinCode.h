// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "EOSUserInterface_EnterDevicePinCode.generated.h"

EOS_ENABLE_STRICT_WARNINGS

DECLARE_DELEGATE(UEOSUserInterface_EnterDevicePinCode_OnCancel);

UCLASS(BlueprintType) class UEOSUserInterface_EnterDevicePinCode_Context : public UObject
{
    GENERATED_BODY()

private:
    UEOSUserInterface_EnterDevicePinCode_OnCancel OnCancel;

public:
    void SetOnCancel(UEOSUserInterface_EnterDevicePinCode_OnCancel InOnCancel)
    {
        this->OnCancel = MoveTemp(InOnCancel);
    }

    UFUNCTION(BlueprintCallable, Category = "EOS|User Interface")
    virtual void CancelLogin()
    {
        this->OnCancel.ExecuteIfBound();
    }
};

UINTERFACE(MinimalAPI, Blueprintable)
class UEOSUserInterface_EnterDevicePinCode : public UInterface
{
    GENERATED_BODY()
};

class ONLINESUBSYSTEMREDPOINTEOS_API IEOSUserInterface_EnterDevicePinCode
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "EOS|User Interface")
    void SetupUserInterface(
        UEOSUserInterface_EnterDevicePinCode_Context *Context,
        const FString &VerificationUrl,
        const FString &PinCode);
};

EOS_DISABLE_STRICT_WARNINGS