// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "IEOSCheck.h"

class FEOSSeparateServerWithListenServerCheck : public IEOSCheck
{
public:
    FEOSSeparateServerWithListenServerCheck() = default;
    UE_NONCOPYABLE(FEOSSeparateServerWithListenServerCheck);
    virtual ~FEOSSeparateServerWithListenServerCheck(){};

    virtual bool ShouldTick() const override
    {
        return true;
    }

    virtual const TArray<FEOSCheckEntry> Tick(float DeltaSeconds) const override;

    virtual void HandleAction(const FString &CheckId, const FString &ActionId) const override;
};