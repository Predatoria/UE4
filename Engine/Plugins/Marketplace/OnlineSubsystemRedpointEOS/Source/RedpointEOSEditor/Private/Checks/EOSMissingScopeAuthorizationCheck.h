// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "IEOSCheck.h"

class FEOSMissingScopeAuthorizationCheck : public IEOSCheck
{
public:
    FEOSMissingScopeAuthorizationCheck() = default;
    UE_NONCOPYABLE(FEOSMissingScopeAuthorizationCheck);
    virtual ~FEOSMissingScopeAuthorizationCheck(){};

    virtual const TArray<FEOSCheckEntry> ProcessLogMessage(
        EOS_ELogLevel InLogLevel,
        const FString &Category,
        const FString &Message) const override;

    virtual void HandleAction(const FString &CheckId, const FString &ActionId) const override;
};