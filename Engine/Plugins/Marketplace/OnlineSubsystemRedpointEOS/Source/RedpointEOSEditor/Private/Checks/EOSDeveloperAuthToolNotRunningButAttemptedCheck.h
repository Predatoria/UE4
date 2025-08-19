// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "IEOSCheck.h"

class FEOSDeveloperAuthToolNotRunningButAttemptedCheck : public IEOSCheck
{
public:
    FEOSDeveloperAuthToolNotRunningButAttemptedCheck() = default;
    UE_NONCOPYABLE(FEOSDeveloperAuthToolNotRunningButAttemptedCheck);
    virtual ~FEOSDeveloperAuthToolNotRunningButAttemptedCheck(){};

    virtual const TArray<FEOSCheckEntry> ProcessCustomSignal(const FString &Context, const FString &SignalId)
        const override;

    virtual void HandleAction(const FString &CheckId, const FString &ActionId) const override;
};