// Copyright June Rhodes. All Rights Reserved.

#include "CoreEOSConfigurer.h"

#include "../PlatformHelpers.h"

void FCoreEOSConfigurer::InitDefaults(FEOSConfigurerContext &Context, UOnlineSubsystemEOSEditorConfig *Instance)
{
    Instance->ProductName = TEXT("Product Name Not Set");
    Instance->ProductVersion = TEXT("0.0.0");
    Instance->RequireEpicGamesLauncher = false;
    Instance->ApiVersion = (EEOSApiVersion)0;
}

void FCoreEOSConfigurer::Load(
    FEOSConfigurerContext &Context,
    FEOSConfigurationReader &Reader,
    UOnlineSubsystemEOSEditorConfig *Instance)
{
    Reader.GetString(TEXT("ProductName"), Instance->ProductName);
    Reader.GetString(TEXT("ProductVersion"), Instance->ProductVersion);
    Reader.GetString(TEXT("ProductId"), Instance->ProductId);
    Reader.GetString(TEXT("SandboxId"), Instance->SandboxId);
    Reader.GetString(TEXT("DeploymentId"), Instance->DeploymentId);
    Reader.GetString(TEXT("ClientId"), Instance->ClientId);
    Reader.GetString(TEXT("ClientSecret"), Instance->ClientSecret);
    Reader.GetEnum<EEOSApiVersion>(TEXT("ApiVersion"), TEXT("EEOSApiVersion"), Instance->ApiVersion);
    Reader.GetBool(TEXT("RequireEpicGamesLauncher"), Instance->RequireEpicGamesLauncher);
#if defined(EOS_IS_FREE_EDITION)
    if (Instance->GetClass() == UOnlineSubsystemEOSEditorConfigFreeEdition::StaticClass())
    {
        Reader.GetString(
            TEXT("FreeEditionLicenseKey"),
            Cast<UOnlineSubsystemEOSEditorConfigFreeEdition>(Instance)->FreeEditionLicenseKey);
    }
#endif
}

bool FCoreEOSConfigurer::Validate(FEOSConfigurerContext &Context, UOnlineSubsystemEOSEditorConfig *Instance)
{
    if (Context.bAutomaticallyConfigureEngineLevelSettings)
    {
        // Check that the default subsystem is correct.
        FString Value;
        GConfig->GetString(TEXT("OnlineSubsystem"), TEXT("DefaultPlatformService"), Value, GEngineIni);
        if (Value != TEXT("RedpointEOS"))
        {
            EOSSendCustomEditorSignal(TEXT("Configuration"), TEXT("NotConfiguredForEOS"));
            return true;
        }

        // Check that the net driver definitions are correct, and if they aren't
        // force a full save.
        TArray<FString> ExpectedNetDriverDefinitions;
        ExpectedNetDriverDefinitions.Add("(DefName=\"GameNetDriver\",DriverClassName=\"/Script/"
                                         "OnlineSubsystemRedpointEOS.EOSNetDriver\",DriverClassNameFallback=\"/Script/"
                                         "OnlineSubsystemUtils.IpNetDriver\")");
        ExpectedNetDriverDefinitions.Add("(DefName=\"BeaconNetDriver\",DriverClassName=\"/Script/"
                                         "OnlineSubsystemRedpointEOS.EOSNetDriver\",DriverClassNameFallback=\"/Script/"
                                         "OnlineSubsystemUtils.IpNetDriver\")");
        ExpectedNetDriverDefinitions.Add(
            "(DefName=\"DemoNetDriver\",DriverClassName=\"/Script/"
            "Engine.DemoNetDriver\",DriverClassNameFallback=\"/Script/Engine.DemoNetDriver\")");
        TArray<FString> ActualNetDriverDefinitions;
        GConfig->GetArray(
            TEXT("/Script/Engine.Engine"),
            TEXT("NetDriverDefinitions"),
            ActualNetDriverDefinitions,
            GEngineIni);
        if (ActualNetDriverDefinitions.Num() != ExpectedNetDriverDefinitions.Num())
        {
            return true;
        }
        for (const auto &Expected : ExpectedNetDriverDefinitions)
        {
            if (!ActualNetDriverDefinitions.Contains(Expected))
            {
                return true;
            }
        }
        ActualNetDriverDefinitions.Empty();
        GConfig->GetArray(
            TEXT("/Script/Engine.GameEngine"),
            TEXT("NetDriverDefinitions"),
            ActualNetDriverDefinitions,
            GEngineIni);
        if (ActualNetDriverDefinitions.Num() != ExpectedNetDriverDefinitions.Num())
        {
            return true;
        }
        for (const auto &Expected : ExpectedNetDriverDefinitions)
        {
            if (!ActualNetDriverDefinitions.Contains(Expected))
            {
                return true;
            }
        }
    }

    return false;
}

void FCoreEOSConfigurer::Save(
    FEOSConfigurerContext &Context,
    FEOSConfigurationWriter &Writer,
    UOnlineSubsystemEOSEditorConfig *Instance)
{
    if (Context.bAutomaticallyConfigureEngineLevelSettings)
    {
        // Set up network driver array.
        TArray<FString> NetDriverDefinitions;
        NetDriverDefinitions.Add("(DefName=\"GameNetDriver\",DriverClassName=\"/Script/"
                                 "OnlineSubsystemRedpointEOS.EOSNetDriver\",DriverClassNameFallback=\"/Script/"
                                 "OnlineSubsystemUtils.IpNetDriver\")");
        NetDriverDefinitions.Add("(DefName=\"BeaconNetDriver\",DriverClassName=\"/Script/"
                                 "OnlineSubsystemRedpointEOS.EOSNetDriver\",DriverClassNameFallback=\"/Script/"
                                 "OnlineSubsystemUtils.IpNetDriver\")");
        NetDriverDefinitions.Add("(DefName=\"DemoNetDriver\",DriverClassName=\"/Script/"
                                 "Engine.DemoNetDriver\",DriverClassNameFallback=\"/Script/Engine.DemoNetDriver\")");

        // Core settings required for EOS and networking.
        Writer.SetString(
            TEXT("DefaultPlatformService"),
            TEXT("RedpointEOS"),
            TEXT("OnlineSubsystem"),
            EEOSConfigurationFileType::Engine);
        Writer.ReplaceArray(
            TEXT("NetDriverDefinitions"),
            NetDriverDefinitions,
            TEXT("/Script/Engine.Engine"),
            EEOSConfigurationFileType::Engine);
        Writer.ReplaceArray(
            TEXT("NetDriverDefinitions"),
            NetDriverDefinitions,
            TEXT("/Script/Engine.GameEngine"),
            EEOSConfigurationFileType::Engine);
        for (const auto &Platform : GetAllPlatformNames())
        {
            Writer.SetString(
                TEXT("DefaultPlatformService"),
                TEXT("RedpointEOS"),
                TEXT("OnlineSubsystem"),
                EEOSConfigurationFileType::Engine,
                Platform);
            Writer.ReplaceArray(
                TEXT("NetDriverDefinitions"),
                NetDriverDefinitions,
                TEXT("/Script/Engine.Engine"),
                EEOSConfigurationFileType::Engine,
                Platform);
            Writer.ReplaceArray(
                TEXT("NetDriverDefinitions"),
                NetDriverDefinitions,
                TEXT("/Script/Engine.GameEngine"),
                EEOSConfigurationFileType::Engine,
                Platform);
        }

        // IOS required settings.
        Writer.SetString(
            TEXT("MinimumiOSVersion"),
            TEXT("IOS_13"),
            TEXT("/Script/IOSRuntimeSettings.IOSRuntimeSettings"),
            EEOSConfigurationFileType::Engine,
            FName(TEXT("IOS")));
        Writer.SetBool(
            TEXT("bEnableSignInWithAppleSupport"),
            true,
            TEXT("/Script/IOSRuntimeSettings.IOSRuntimeSettings"),
            EEOSConfigurationFileType::Engine,
            FName(TEXT("IOS")));
        Writer.SetBool(
            TEXT("bEnabled"),
            true,
            TEXT("OnlineSubsystemApple"),
            EEOSConfigurationFileType::Engine,
            FName(TEXT("IOS")));

        // Asset manager cook settings.
        TArray<FString> UserWidgetAssetTypes;
        UserWidgetAssetTypes.Add(
            "(PrimaryAssetType=\"UserWidgetRedpointEOS\",AssetBaseClass=/Script/"
            "CoreUObject.Object,bHasBlueprintClasses=True,bIsEditorOnly=False,Directories=((Path=\"/"
            "OnlineSubsystemRedpointEOS\")),SpecificAssets=,Rules=(Priority=-1,ChunkId=-1,"
            "bApplyRecursively=True,CookRule=AlwaysCook))");
        Writer.EnsureArrayElements(
            TEXT("PrimaryAssetTypesToScan"),
            UserWidgetAssetTypes,
            TEXT("/Script/Engine.AssetManagerSettings"),
            EEOSConfigurationFileType::Game);
        Writer.SetBool(
            TEXT("bShouldManagerDetermineTypeAndName"),
            true,
            TEXT("/Script/Engine.AssetManagerSettings"),
            EEOSConfigurationFileType::Game);

        // Blueprint nativization settings.
        TArray<FString> ExcludeFromNativization;
        ExcludeFromNativization.Add("/OnlineSubsystemRedpointEOS/EOSDefaultUserInterface_EnterDevicePinCode");
        ExcludeFromNativization.Add("/OnlineSubsystemRedpointEOS/EOSDefaultUserInterface_SignInOrCreateAccount");
        Writer.EnsureArrayElements(
            TEXT("ExcludedAssets"),
            ExcludeFromNativization,
            TEXT("BlueprintNativizationSettings"),
            EEOSConfigurationFileType::Editor);

        // Steam networking settings.
        Writer.SetBool(
            TEXT("bUseSteamNetworking"),
            false,
            TEXT("OnlineSubsystemSteam"),
            EEOSConfigurationFileType::Engine,
            TEXT("Windows"));
        Writer.SetBool(
            TEXT("bUseSteamNetworking"),
            false,
            TEXT("OnlineSubsystemSteam"),
            EEOSConfigurationFileType::Engine,
            TEXT("Mac"));
        Writer.SetBool(
            TEXT("bUseSteamNetworking"),
            false,
            TEXT("OnlineSubsystemSteam"),
            EEOSConfigurationFileType::Engine,
            TEXT("Linux"));
    }

    // Settings configured through Project Settings.
    Writer.SetString(TEXT("ProductName"), Instance->ProductName);
    Writer.SetString(TEXT("ProductVersion"), Instance->ProductVersion);
    Writer.SetString(TEXT("ProductId"), Instance->ProductId);
    Writer.SetString(TEXT("SandboxId"), Instance->SandboxId);
    Writer.SetString(TEXT("DeploymentId"), Instance->DeploymentId);
    Writer.SetString(TEXT("ClientId"), Instance->ClientId);
    Writer.SetString(TEXT("ClientSecret"), Instance->ClientSecret);
    Writer.SetEnum<EEOSApiVersion>(TEXT("ApiVersion"), TEXT("EEOSApiVersion"), Instance->ApiVersion);
#if defined(EOS_IS_FREE_EDITION)
    if (Instance->GetClass() == UOnlineSubsystemEOSEditorConfigFreeEdition::StaticClass())
    {
        Writer.SetString(
            TEXT("FreeEditionLicenseKey"),
            Cast<UOnlineSubsystemEOSEditorConfigFreeEdition>(Instance)->FreeEditionLicenseKey);
    }
#endif

    // Epic Games Store settings.
    Writer.SetBool(TEXT("RequireEpicGamesLauncher"), Instance->RequireEpicGamesLauncher);
}