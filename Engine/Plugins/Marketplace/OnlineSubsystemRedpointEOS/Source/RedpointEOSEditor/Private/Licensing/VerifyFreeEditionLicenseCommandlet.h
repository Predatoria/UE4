// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "Commandlets/Commandlet.h"
#include "CoreMinimal.h"

#include "VerifyFreeEditionLicenseCommandlet.generated.h"

UCLASS()
class UVerifyFreeEditionLicenseCommandlet : public UCommandlet
{
    GENERATED_UCLASS_BODY()

public:
    virtual int32 Main(const FString &Params) override;
};
