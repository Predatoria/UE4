// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "IEOSCheck.h"

class FEOSRunUnderOneProcessCheck : public IEOSCheck
{
public:
    FEOSRunUnderOneProcessCheck() = default;
    UE_NONCOPYABLE(FEOSRunUnderOneProcessCheck);
    virtual ~FEOSRunUnderOneProcessCheck(){};

    virtual bool ShouldTick() const override
    {
        return true;
    }

    virtual const TArray<FEOSCheckEntry> Tick(float DeltaSeconds) const override;

    virtual void HandleAction(const FString &CheckId, const FString &ActionId) const override;
};