// Copyright June Rhodes. All Rights Reserved.

#include "AndroidEOSConfigurer.h"

#include "../PlatformHelpers.h"

void FAndroidEOSConfigurer::InitDefaults(FEOSConfigurerContext &Context, UOnlineSubsystemEOSEditorConfig *Instance)
{
}

void FAndroidEOSConfigurer::Load(
    FEOSConfigurerContext &Context,
    FEOSConfigurationReader &Reader,
    UOnlineSubsystemEOSEditorConfig *Instance)
{
    // This value isn't stored in the instance.
    FString MinSDKVersionString = TEXT("19");
    Reader.GetString(
        TEXT("MinSDKVersion"),
        MinSDKVersionString,
        TEXT("/Script/AndroidRuntimeSettings.AndroidRuntimeSettings"),
        EEOSConfigurationFileType::Engine,
        FName(TEXT("Android")));
    this->MinSDKVersion = FCString::Atoi(*MinSDKVersionString);
}

bool FAndroidEOSConfigurer::Validate(FEOSConfigurerContext &Context, UOnlineSubsystemEOSEditorConfig *Instance)
{
    if (Context.bAutomaticallyConfigureEngineLevelSettings)
    {
        // We must fix up Android settings.
        return this->MinSDKVersion < 23;
    }

    return false;
}

void FAndroidEOSConfigurer::Save(
    FEOSConfigurerContext &Context,
    FEOSConfigurationWriter &Writer,
    UOnlineSubsystemEOSEditorConfig *Instance)
{
    if (Context.bAutomaticallyConfigureEngineLevelSettings)
    {
        // Android required settings.
        Writer.SetString(
            TEXT("MinSDKVersion"),
            TEXT("23"),
            TEXT("/Script/AndroidRuntimeSettings.AndroidRuntimeSettings"),
            EEOSConfigurationFileType::Engine,
            FName(TEXT("Android")));
        // Set in DefaultEngine.ini as well since it's a build-time thing.
        Writer.SetString(
            TEXT("MinSDKVersion"),
            TEXT("23"),
            TEXT("/Script/AndroidRuntimeSettings.AndroidRuntimeSettings"),
            EEOSConfigurationFileType::Engine);
        this->MinSDKVersion = 23;
    }
}