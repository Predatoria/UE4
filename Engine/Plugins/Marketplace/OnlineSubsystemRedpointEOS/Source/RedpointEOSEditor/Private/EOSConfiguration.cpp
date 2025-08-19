// Copyright June Rhodes. All Rights Reserved.

#include "EOSConfiguration.h"
#include "IEOSConfigurer.h"
#include "OnlineSubsystemEOSEditorPrefs.h"
#include "PlatformHelpers.h"

FEOSConfigurerContext::FEOSConfigurerContext()
    : bAutomaticallyConfigureEngineLevelSettings(false)
{
    const UOnlineSubsystemEOSEditorPrefs *CDO = GetDefault<UOnlineSubsystemEOSEditorPrefs>();
    this->bAutomaticallyConfigureEngineLevelSettings = CDO->bAutomaticallyConfigureProjectForEOS;
}

TMap<FString, FString> EOSConfiguration::FilePathCache;

FString EOSConfiguration::GetFilePath(EEOSConfigurationFileType File, FName Platform)
{
    FString CacheKey;
    switch (File)
    {
    case EEOSConfigurationFileType::Engine:
        CacheKey = FString::Printf(TEXT("Engine-%s"), *Platform.ToString());
        break;
    case EEOSConfigurationFileType::Game:
        CacheKey = FString::Printf(TEXT("Game-%s"), *Platform.ToString());
        break;
    case EEOSConfigurationFileType::Editor:
        CacheKey = FString::Printf(TEXT("Editor-%s"), *Platform.ToString());
        break;
    case EEOSConfigurationFileType::DedicatedServer:
        CacheKey = FString::Printf(TEXT("DedicatedServer-%s"), *Platform.ToString());
        break;
    case EEOSConfigurationFileType::TrustedClient:
        CacheKey = FString::Printf(TEXT("TrustedClient-%s"), *Platform.ToString());
        break;
    }
    if (FilePathCache.Contains(CacheKey))
    {
        return FilePathCache[CacheKey];
    }

    FString SourceConfigDir = FPaths::SourceConfigDir();
    checkf(!SourceConfigDir.IsEmpty(), TEXT("FPaths::SourceConfigDir must not be empty!"));

    if (File == EEOSConfigurationFileType::DedicatedServer)
    {
        FString Result = FString::Printf(TEXT("%sDedicatedServerEngine.ini"), *SourceConfigDir);
        FilePathCache.Add(CacheKey, Result);
        return Result;
    }
    if (File == EEOSConfigurationFileType::TrustedClient)
    {
        FString Result = FPaths::Combine(
            SourceConfigDir,
            FString(".."),
            FString("Build"),
            FString("NoRedist"),
            FString("TrustedEOSClient.ini"));
        FilePathCache.Add(CacheKey, Result);
        return Result;
    }

    FString FileName;
    switch (File)
    {
    case EEOSConfigurationFileType::Engine:
        FileName = TEXT("Engine");
        break;
    case EEOSConfigurationFileType::Game:
        FileName = TEXT("Game");
        break;
    case EEOSConfigurationFileType::Editor:
        FileName = TEXT("Editor");
        break;
    }

    if (Platform.IsNone())
    {
        FString Result = FPaths::Combine(SourceConfigDir, FString::Printf(TEXT("Default%s.ini"), *FileName));
        FilePathCache.Add(CacheKey, Result);
        return Result;
    }
    else if (!GetConfidentialPlatformNames().Contains(Platform))
    {
        FString Result = FPaths::Combine(
            SourceConfigDir,
            Platform.ToString(),
            FString::Printf(TEXT("%s%s.ini"), *Platform.ToString(), *FileName));
        FilePathCache.Add(CacheKey, Result);
        return Result;
    }
    else
    {
        FString ProjectPlatformExtensionsDir = FPaths::ProjectPlatformExtensionsDir();
        checkf(
            !ProjectPlatformExtensionsDir.IsEmpty(),
            TEXT("FPaths::ProjectPlatformExtensionsDir must not be empty!"));
        FString Result = FPaths::Combine(
            ProjectPlatformExtensionsDir,
            Platform.ToString(),
            FString("Config"),
            FString::Printf(TEXT("%s%s.ini"), *Platform.ToString(), *FileName));
        FilePathCache.Add(CacheKey, Result);
        return Result;
    }
}