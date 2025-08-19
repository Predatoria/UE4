// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "IEOSCheck.h"

class FEOSEditorRequiresRestartForOSSCheck : public IEOSCheck
{
public:
    FEOSEditorRequiresRestartForOSSCheck() = default;
    UE_NONCOPYABLE(FEOSEditorRequiresRestartForOSSCheck);
    virtual ~FEOSEditorRequiresRestartForOSSCheck(){};

    virtual const TArray<FEOSCheckEntry> ProcessCustomSignal(const FString &Context, const FString &SignalId)
        const override;

    virtual void HandleAction(const FString &CheckId, const FString &ActionId) const override;
};