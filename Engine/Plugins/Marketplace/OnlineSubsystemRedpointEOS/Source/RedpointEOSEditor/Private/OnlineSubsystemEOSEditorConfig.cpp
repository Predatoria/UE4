// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemEOSEditorConfig.h"

#include "EOSConfiguration.h"
#include "EOSConfigurationReader.h"
#include "EOSConfigurationWriter.h"
#include "EOSConfigurerRegistry.h"
#include "ISettingsContainer.h"
#include "ISettingsModule.h"
#include "Misc/Base64.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSDefines.h"
#include "PlatformHelpers.h"
#include "UObject/UObjectGlobals.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "OnlineSubsystemEOSEditorModule"

#define EOS_SECTION TEXT("EpicOnlineServices")

#if defined(UE_5_0_OR_LATER)
#define FIND_CONFIG_FILE(Filename) GConfig->Find(Filename)
#else
#define FIND_CONFIG_FILE(Filename) GConfig->Find(Filename, true)
#endif

UOnlineSubsystemEOSEditorConfig::UOnlineSubsystemEOSEditorConfig()
{
    FEOSConfigurerContext Context;
    FEOSConfigurerRegistry::InitDefaults(Context, this);

    FEOSConfigurationReader Reader;
    FEOSConfigurerRegistry::Load(Context, Reader, this);
}

void UOnlineSubsystemEOSEditorConfig::LoadEOSConfig()
{
    FEOSConfigurerContext Context;
    FEOSConfigurationReader Reader;
    FEOSConfigurerRegistry::Load(Context, Reader, this);

    if (FEOSConfigurerRegistry::Validate(Context, this))
    {
        FEOSConfigurationWriter Writer;
        FEOSConfigurerRegistry::Save(Context, Writer, this);

        Writer.FlushChanges();
    }
}

void UOnlineSubsystemEOSEditorConfig::SaveEOSConfig()
{
    FEOSConfigurerContext Context;
    FEOSConfigurerRegistry::Validate(Context, this);

    FEOSConfigurationWriter Writer;
    FEOSConfigurerRegistry::Save(Context, Writer, this);

    Writer.FlushChanges();
}

const FName UOnlineSubsystemEOSEditorConfig::GetEditorAuthenticationGraphPropertyName()
{
    static const FName PropertyName =
        GET_MEMBER_NAME_CHECKED(UOnlineSubsystemEOSEditorConfig, EditorAuthenticationGraph);
    return PropertyName;
}

const FName UOnlineSubsystemEOSEditorConfig::GetAuthenticationGraphPropertyName()
{
    static const FName PropertyName = GET_MEMBER_NAME_CHECKED(UOnlineSubsystemEOSEditorConfig, AuthenticationGraph);
    return PropertyName;
}

const FName UOnlineSubsystemEOSEditorConfig::GetCrossPlatformAccountProviderPropertyName()
{
    static const FName PropertyName =
        GET_MEMBER_NAME_CHECKED(UOnlineSubsystemEOSEditorConfig, CrossPlatformAccountProvider);
    return PropertyName;
}

bool FOnlineSubsystemEOSEditorConfig::HandleSettingsSaved()
{
    this->Config->SaveEOSConfig();

    // Prevent the section that gets automatically added by the config serialization system from being added now.
    return false;
}

void FOnlineSubsystemEOSEditorConfig::RegisterSettings()
{
    if (ISettingsModule *SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        // Make the instanced version of the config (required so OverridePerObjectConfigSection will be called).
#if defined(EOS_IS_FREE_EDITION)
        this->Config = NewObject<UOnlineSubsystemEOSEditorConfigFreeEdition>();
#else
        this->Config = NewObject<UOnlineSubsystemEOSEditorConfig>();
#endif
        this->Config->AddToRoot();

        this->Config->LoadEOSConfig();

        // Register the settings
        this->SettingsSection = SettingsModule->RegisterSettings(
            "Project",
            "Game",
            "Epic Online Services",
            LOCTEXT("EOSSettingsName", "Epic Online Services"),
            LOCTEXT("EOSSettingsDescription", "Configure Epic Online Services in your game."),
            this->Config);

        if (SettingsSection.IsValid())
        {
            SettingsSection->OnModified().BindRaw(this, &FOnlineSubsystemEOSEditorConfig::HandleSettingsSaved);
        }
    }
}

void FOnlineSubsystemEOSEditorConfig::UnregisterSettings()
{
    if (ISettingsModule *SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        SettingsModule->UnregisterSettings("Project", "Game", "Epic Online Services");
    }
}

#undef LOCTEXT_NAMESPACE