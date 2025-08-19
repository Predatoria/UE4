// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "../IEOSConfigurer.h"
#include "CoreMinimal.h"

class FStorageEOSConfigurer : public IEOSConfigurer
{
public:
    virtual void InitDefaults(FEOSConfigurerContext &Context, UOnlineSubsystemEOSEditorConfig *Instance) override;
    virtual void Load(
        FEOSConfigurerContext &Context,
        FEOSConfigurationReader &Reader,
        UOnlineSubsystemEOSEditorConfig *Instance) override;
    virtual bool Validate(FEOSConfigurerContext &Context, UOnlineSubsystemEOSEditorConfig *Instance) override;
    virtual void Save(
        FEOSConfigurerContext &Context,
        FEOSConfigurationWriter &Writer,
        UOnlineSubsystemEOSEditorConfig *Instance) override;
};