// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "IEOSCheck.h"

class FEOSEssentialSettingsCheck : public IEOSCheck
{
private:
    TSharedRef<class FEOSConfigIni> Config;

public:
    FEOSEssentialSettingsCheck();
    UE_NONCOPYABLE(FEOSEssentialSettingsCheck);
    virtual ~FEOSEssentialSettingsCheck(){};

    virtual bool ShouldTick() const override
    {
        return true;
    }

    virtual const TArray<FEOSCheckEntry> Tick(float DeltaSeconds) const override;

    virtual void HandleAction(const FString &CheckId, const FString &ActionId) const override;
};