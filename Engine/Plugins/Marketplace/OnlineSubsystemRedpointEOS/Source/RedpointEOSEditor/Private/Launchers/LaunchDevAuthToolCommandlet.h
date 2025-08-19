// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "Commandlets/Commandlet.h"
#include "CoreMinimal.h"

#include "LaunchDevAuthToolCommandlet.generated.h"

UCLASS()
class ULaunchDevAuthToolCommandlet : public UCommandlet
{
    GENERATED_UCLASS_BODY()

public:
    virtual int32 Main(const FString &Params) override;
};
