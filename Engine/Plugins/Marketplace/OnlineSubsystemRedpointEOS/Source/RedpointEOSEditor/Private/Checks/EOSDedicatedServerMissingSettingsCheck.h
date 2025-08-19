// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "IEOSCheck.h"

class FEOSDedicatedServerMissingSettingsCheck : public IEOSCheck
{
private:
    TSharedRef<class FEOSConfigIni> Config;

public:
    FEOSDedicatedServerMissingSettingsCheck();
    UE_NONCOPYABLE(FEOSDedicatedServerMissingSettingsCheck);
    virtual ~FEOSDedicatedServerMissingSettingsCheck(){};

    virtual bool ShouldTick() const override
    {
        return true;
    }

    virtual const TArray<FEOSCheckEntry> Tick(float DeltaSeconds) const override;

    virtual void HandleAction(const FString &CheckId, const FString &ActionId) const override;
};