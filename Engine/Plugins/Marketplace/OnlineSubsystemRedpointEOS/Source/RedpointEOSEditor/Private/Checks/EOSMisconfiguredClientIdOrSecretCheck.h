// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "IEOSCheck.h"

class FEOSMisconfiguredClientIdOrSecretCheck : public IEOSCheck
{
public:
    FEOSMisconfiguredClientIdOrSecretCheck() = default;
    UE_NONCOPYABLE(FEOSMisconfiguredClientIdOrSecretCheck);
    virtual ~FEOSMisconfiguredClientIdOrSecretCheck(){};

    virtual const TArray<FEOSCheckEntry> ProcessLogMessage(
        EOS_ELogLevel InLogLevel,
        const FString &Category,
        const FString &Message) const override;

    virtual void HandleAction(const FString &CheckId, const FString &ActionId) const override;
};