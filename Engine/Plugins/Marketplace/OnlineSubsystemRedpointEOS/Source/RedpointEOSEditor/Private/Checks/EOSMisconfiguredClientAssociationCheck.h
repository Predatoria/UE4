// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "IEOSCheck.h"

class FEOSMisconfiguredClientAssociationCheck : public IEOSCheck
{
public:
    FEOSMisconfiguredClientAssociationCheck() = default;
    UE_NONCOPYABLE(FEOSMisconfiguredClientAssociationCheck);
    virtual ~FEOSMisconfiguredClientAssociationCheck(){};

    virtual const TArray<FEOSCheckEntry> ProcessLogMessage(
        EOS_ELogLevel InLogLevel,
        const FString &Category,
        const FString &Message) const override;

    virtual void HandleAction(const FString &CheckId, const FString &ActionId) const override;
};