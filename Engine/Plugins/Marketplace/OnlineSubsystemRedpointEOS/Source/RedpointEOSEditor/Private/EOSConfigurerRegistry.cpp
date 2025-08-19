// Copyright June Rhodes. All Rights Reserved.

#include "EOSConfigurerRegistry.h"

#include "./Configurers/AndroidEOSConfigurer.h"
#include "./Configurers/AuthenticationEOSConfigurer.h"
#include "./Configurers/CoreEOSConfigurer.h"
#include "./Configurers/CrossPlatformEOSConfigurer.h"
#include "./Configurers/NetworkingEOSConfigurer.h"
#include "./Configurers/PresenceEOSConfigurer.h"
#include "./Configurers/StatsEOSConfigurer.h"
#include "./Configurers/StorageEOSConfigurer.h"
#include "./Configurers/VoiceChatEOSConfigurer.h"

TSharedPtr<FEOSConfigurerRegistry> FEOSConfigurerRegistry::RegistryInstance = nullptr;

FEOSConfigurerRegistry::FEOSConfigurerRegistry()
{
    this->Configurers.Add(MakeShared<FCoreEOSConfigurer>());
    this->Configurers.Add(MakeShared<FAndroidEOSConfigurer>());
    this->Configurers.Add(MakeShared<FAuthenticationEOSConfigurer>());
    this->Configurers.Add(MakeShared<FNetworkingEOSConfigurer>());
    this->Configurers.Add(MakeShared<FPresenceEOSConfigurer>());
    this->Configurers.Add(MakeShared<FStatsEOSConfigurer>());
    this->Configurers.Add(MakeShared<FStorageEOSConfigurer>());
    this->Configurers.Add(MakeShared<FVoiceChatEOSConfigurer>());
    this->Configurers.Add(MakeShared<FCrossPlatformEOSConfigurer>());
}

void FEOSConfigurerRegistry::InitDefaults(FEOSConfigurerContext &Context, UOnlineSubsystemEOSEditorConfig *Instance)
{
    if (!FEOSConfigurerRegistry::RegistryInstance.IsValid())
    {
        FEOSConfigurerRegistry::RegistryInstance = MakeShared<FEOSConfigurerRegistry>();
    }

    for (const auto &Configurer : FEOSConfigurerRegistry::RegistryInstance->Configurers)
    {
        Configurer->InitDefaults(Context, Instance);
    }
}

void FEOSConfigurerRegistry::Load(
    FEOSConfigurerContext &Context,
    FEOSConfigurationReader &Reader,
    UOnlineSubsystemEOSEditorConfig *Instance)
{
    if (!FEOSConfigurerRegistry::RegistryInstance.IsValid())
    {
        FEOSConfigurerRegistry::RegistryInstance = MakeShared<FEOSConfigurerRegistry>();
    }

    for (const auto &Configurer : FEOSConfigurerRegistry::RegistryInstance->Configurers)
    {
        Configurer->Load(Context, Reader, Instance);
    }

    EOSSendCustomEditorSignal(TEXT("Configuration"), TEXT("Load"));
}

bool FEOSConfigurerRegistry::Validate(FEOSConfigurerContext &Context, UOnlineSubsystemEOSEditorConfig *Instance)
{
    if (!FEOSConfigurerRegistry::RegistryInstance.IsValid())
    {
        FEOSConfigurerRegistry::RegistryInstance = MakeShared<FEOSConfigurerRegistry>();
    }

    bool bDidModify = false;
    for (const auto &Configurer : FEOSConfigurerRegistry::RegistryInstance->Configurers)
    {
        if (Configurer->Validate(Context, Instance))
        {
            bDidModify = true;
        }
    }
    return bDidModify;
}

void FEOSConfigurerRegistry::Save(
    FEOSConfigurerContext &Context,
    FEOSConfigurationWriter &Writer,
    UOnlineSubsystemEOSEditorConfig *Instance)
{
    if (!FEOSConfigurerRegistry::RegistryInstance.IsValid())
    {
        FEOSConfigurerRegistry::RegistryInstance = MakeShared<FEOSConfigurerRegistry>();
    }

    for (const auto &Configurer : FEOSConfigurerRegistry::RegistryInstance->Configurers)
    {
        Configurer->Save(Context, Writer, Instance);
    }

    EOSSendCustomEditorSignal(TEXT("Configuration"), TEXT("Save"));
}