// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "IEOSCheck.h"

class FEOSMissingPermissionForActionCheck : public IEOSCheck
{
public:
    FEOSMissingPermissionForActionCheck() = default;
    UE_NONCOPYABLE(FEOSMissingPermissionForActionCheck);
    virtual ~FEOSMissingPermissionForActionCheck(){};

    virtual const TArray<FEOSCheckEntry> ProcessLogMessage(
        EOS_ELogLevel InLogLevel,
        const FString &Category,
        const FString &Message) const override;

    virtual void HandleAction(const FString &CheckId, const FString &ActionId) const override;
};