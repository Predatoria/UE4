// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EOSConfiguration.h"
#include "EOSConfigurationReader.h"
#include "EOSConfigurationWriter.h"
#include "IEOSConfigurer.h"
#include "OnlineSubsystemEOSEditorConfig.h"

class FEOSConfigurerRegistry
{
private:
    TArray<TSharedRef<IEOSConfigurer>> Configurers;
    static TSharedPtr<FEOSConfigurerRegistry> RegistryInstance;

public:
    FEOSConfigurerRegistry();

    static void InitDefaults(FEOSConfigurerContext &Context, UOnlineSubsystemEOSEditorConfig *Instance);
    static void Load(
        FEOSConfigurerContext &Context,
        FEOSConfigurationReader &Reader,
        UOnlineSubsystemEOSEditorConfig *Instance);
    static bool Validate(FEOSConfigurerContext &Context, UOnlineSubsystemEOSEditorConfig *Instance);
    static void Save(
        FEOSConfigurerContext &Context,
        FEOSConfigurationWriter &Writer,
        UOnlineSubsystemEOSEditorConfig *Instance);
};