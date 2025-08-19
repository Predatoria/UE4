// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "IEOSCheck.h"

class FEOSNetworkingDuplicateAccountCheck : public IEOSCheck
{
public:
    FEOSNetworkingDuplicateAccountCheck() = default;
    UE_NONCOPYABLE(FEOSNetworkingDuplicateAccountCheck);
    virtual ~FEOSNetworkingDuplicateAccountCheck(){};

    virtual const TArray<FEOSCheckEntry> ProcessLogMessage(
        EOS_ELogLevel InLogLevel,
        const FString &Category,
        const FString &Message) const override;

    virtual void HandleAction(const FString &CheckId, const FString &ActionId) const override;
};