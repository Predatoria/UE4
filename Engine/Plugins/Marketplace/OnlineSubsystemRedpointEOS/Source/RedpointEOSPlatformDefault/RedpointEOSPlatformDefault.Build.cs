// Copyright June Rhodes. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Diagnostics;
#if UE_5_0_OR_LATER
using EpicGames.Core;
#else
using Tools.DotNETCommon;
#endif

public class RedpointEOSPlatformDefault : ModuleRules
{
    public RedpointEOSPlatformDefault(ReadOnlyTargetRules Target) : base(Target)
    {
        DefaultBuildSettings = BuildSettingsVersion.V2;
        bUsePrecompiled = false;

        OnlineSubsystemRedpointEOS.ApplyEngineVersionDefinesToModule(this);

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Projects",
                "Engine",
                "OnlineSubsystem",
                "OnlineSubsystemRedpointEOS",
                "OnlineSubsystemUtils",
                "RedpointEOSSDK",
                "ApplicationCore",
            }
        );

        if (Target.Type != TargetType.Server)
        {
            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "UMG",
                }
            );
        }

        /* PRECOMPILED REMOVE BEGIN */
        if (!bUsePrecompiled)
        {
#if UE_5_0_OR_LATER
            PrivateDefinitions.Add("EOS_HAS_UNREAL_ANDROID_NAMESPACE=1");
#else
            PrivateDefinitions.Add("EOS_HAS_UNREAL_ANDROID_NAMESPACE=0");
#endif

            // This can't use OnlineSubsystemRedpointEOSConfig.GetBool because the environment variable comes
            // from the standard build scripts.
            if (Environment.GetEnvironmentVariable("BUILDING_FOR_REDISTRIBUTION") == "true")
            {
                bTreatAsEngineModule = true;
                bPrecompile = true;

                // Force the module to be treated as an engine module for UHT, to ensure UPROPERTY compliance.
#if UE_5_0_OR_LATER
                object ContextObj = this.GetType().GetProperty("Context", BindingFlags.Instance | BindingFlags.NonPublic).GetValue(this);
#else
                object ContextObj = this.GetType().GetField("Context", BindingFlags.Instance | BindingFlags.NonPublic).GetValue(this);
#endif
                ContextObj.GetType().GetField("bClassifyAsGameModuleForUHT", BindingFlags.Instance | BindingFlags.Public).SetValue(ContextObj, false);
            }

            ConfigureSteam();
            ConfigureDiscord();
            ConfigureGOG();
            ConfigureItchIo();
            ConfigureOculus();
            ConfigureGoogle();
            ConfigureApple();
        }
        /* PRECOMPILED REMOVE END */
    }

    /* PRECOMPILED REMOVE BEGIN */

    void ConfigureSteam()
    {
        OnlineSubsystemRedpointSteam.AddSteamLibraries(Target, this);
    }

    void ConfigureDiscord()
    {
        if (Target.Platform == UnrealTargetPlatform.Win64 ||
#if !UE_5_0_OR_LATER
                Target.Platform == UnrealTargetPlatform.Win32 ||
#endif
                Target.Platform == UnrealTargetPlatform.Mac ||
                Target.Platform == UnrealTargetPlatform.Linux)
        {
            // RedpointDiscordGameSDK defines EOS_DISCORD_ENABLED based on whether the SDK was found.
            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "OnlineSubsystemRedpointDiscord",
                    "RedpointDiscordGameSDK",
                    "RedpointDiscordGameSDKRuntime",
                }
            );
        }
        else
        {
            PrivateDefinitions.Add("EOS_DISCORD_ENABLED=0");
        }
    }

    void ConfigureGOG()
    {
        if (Target.Platform == UnrealTargetPlatform.Win64 ||
#if !UE_5_0_OR_LATER
                Target.Platform == UnrealTargetPlatform.Win32 ||
#endif
                Target.Platform == UnrealTargetPlatform.Mac)
        {
            if (OnlineSubsystemRedpointEOSConfig.GetBool(Target, "ENABLE_GOG", false))
            {
                PrivateDefinitions.Add("EOS_GOG_ENABLED=1");

                PrivateDependencyModuleNames.AddRange(
                    new string[]
                    {
                        "OnlineSubsystemGOG",
                        "GalaxySDK",
                    }
                );
            }
            else
            {
                PrivateDefinitions.Add("EOS_GOG_ENABLED=0");
            }
        }
        else
        {
            PrivateDefinitions.Add("EOS_GOG_ENABLED=0");
        }
    }

    void ConfigureItchIo()
    {
        if (Target.Platform == UnrealTargetPlatform.Win64 ||
#if !UE_5_0_OR_LATER
                Target.Platform == UnrealTargetPlatform.Win32 ||
#endif
                Target.Platform == UnrealTargetPlatform.Mac ||
                Target.Platform == UnrealTargetPlatform.Linux)
        {
            // OnlineSubsystemRedpointItchIo defines EOS_ITCH_IO_ENABLED based on ProjectDefinitions.
            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "OnlineSubsystemRedpointItchIo",
                }
            );
        }
        else
        {
            PrivateDefinitions.Add("EOS_ITCH_IO_ENABLED=0");
        }
    }

    void ConfigureOculus()
    {
        if (Target.Platform == UnrealTargetPlatform.Win64 ||
                Target.Platform == UnrealTargetPlatform.Android)
        {
            if (OnlineSubsystemRedpointEOSConfig.GetBool(Target, "ENABLE_OCULUS", true))
            {
                PrivateDefinitions.Add("EOS_OCULUS_ENABLED=1");

                PrivateDependencyModuleNames.AddRange(
                    new string[]
                    {
                        "LibOVRPlatform",
                        "OnlineSubsystemOculus",
                    });
            }
            else
            {
                PrivateDefinitions.Add("EOS_OCULUS_ENABLED=0");
            }
        }
        else
        {
            PrivateDefinitions.Add("EOS_OCULUS_ENABLED=0");
        }
    }

    void ConfigureGoogle()
    {
        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            if (OnlineSubsystemRedpointEOSConfig.GetBool(Target, "ENABLE_GOOGLE", false))
            {
                PrivateDependencyModuleNames.AddRange(
                    new string[]
                    {
                        "OnlineSubsystemGoogle",
                    }
                );

                PrivateDefinitions.Add("EOS_GOOGLE_ENABLED=1");
            }
            else
            {
                PrivateDefinitions.Add("EOS_GOOGLE_ENABLED=0");
            }
        }
        else
        {
            PrivateDefinitions.Add("EOS_GOOGLE_ENABLED=0");
        }
    }

    void ConfigureApple()
    {
        if (Target.Platform == UnrealTargetPlatform.IOS)
        {
            bool bSignInWithAppleSupported;
            ConfigHierarchy PlatformGameConfig = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, DirectoryReference.FromFile(Target.ProjectFile), UnrealTargetPlatform.IOS);
            PlatformGameConfig.GetBool("/Script/IOSRuntimeSettings.IOSRuntimeSettings", "bEnableSignInWithAppleSupport", out bSignInWithAppleSupported);

            if (bSignInWithAppleSupported)
            {
                PrivateDependencyModuleNames.AddRange(
                    new string[]
                    {
                        "OnlineSubsystemApple",
                    }
                );

                PrivateDefinitions.Add("EOS_APPLE_ENABLED=1");
            }
            else
            {
                PrivateDefinitions.Add("EOS_APPLE_ENABLED=0");
            }
        }
        else
        {
            PrivateDefinitions.Add("EOS_APPLE_ENABLED=0");
        }
    }

    /* PRECOMPILED REMOVE END */
}