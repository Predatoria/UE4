// Copyright June Rhodes. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Diagnostics;

public class OnlineSubsystemRedpointSteam : ModuleRules
{
    public OnlineSubsystemRedpointSteam(ReadOnlyTargetRules Target) : base(Target)
    {
        DefaultBuildSettings = BuildSettingsVersion.V2;
        bUsePrecompiled = false;

        OnlineSubsystemRedpointEOS.ApplyEngineVersionDefinesToModule(this);

        /* PRECOMPILED REMOVE BEGIN */
        if (!bUsePrecompiled)
        {
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

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "Projects",
                    "Json",
#if UE_5_0_OR_LATER
                    "HTTP",
#else
                    "Http",
#endif
                    "Engine",
                    "OnlineSubsystem",
                    "OnlineSubsystemUtils",
                    "RedpointEOSInterfaces",

                    // Pull in WeakPtrHelpers.
                    "OnlineSubsystemRedpointEOS",
                }
            );

            AddSteamLibraries(Target, this);
        }
        /* PRECOMPILED REMOVE END */
    }

    /* PRECOMPILED REMOVE BEGIN */
    public static void AddSteamLibraries(ReadOnlyTargetRules Target, ModuleRules Module)
    {
        if (Target.Platform == UnrealTargetPlatform.Win64 ||
#if !UE_5_0_OR_LATER
            Target.Platform == UnrealTargetPlatform.Win32 ||
#endif
            Target.Platform == UnrealTargetPlatform.Mac ||
            Target.Platform == UnrealTargetPlatform.Linux)
        {
            bool bExplicitlySetByConfig;
            var bEnableByConfig = OnlineSubsystemRedpointEOSConfig.GetBool(Target, "ENABLE_STEAM", false, out bExplicitlySetByConfig);
#if UE_5_1_OR_LATER
            if ((Target.bIsEngineInstalled && Target.Platform != UnrealTargetPlatform.Mac) || bEnableByConfig)
#else
            if (Target.bIsEngineInstalled || bEnableByConfig)
#endif
            {
                Module.PrivateDefinitions.Add("EOS_STEAM_ENABLED=1");

#if UE_4_27_OR_LATER || UE_5_0_OR_LATER
                Module.PrivateDefinitions.Add("EOS_STEAM_HAS_NEWER_APP_TICKET_INTERFACE=1");
#else
                Module.PrivateDefinitions.Add("EOS_STEAM_HAS_NEWER_APP_TICKET_INTERFACE=0");
#endif

                Module.PrivateDependencyModuleNames.AddRange(
                    new string[]
                    {
                        // For encrypted app ticket authentication.
                        "OnlineSubsystemSteam",
                        "SteamShared",
                    }
                );

                Module.AddEngineThirdPartyPrivateStaticDependencies(Target, "Steamworks");
            }
            else
            {
                // NOTE: By default, we don't include Steam if the developer isn't using an
                // installed version from the Epic Games launcher. That is because as
                // far as I can tell, source builds don't have OnlineSubsystemSteam
                // available by default, which will make the plugin fail to load when launching
                // the editor.
                //
                // As of Unreal Engine 5.1, we also don't enable Steam by default on macOS
                // as installed engine builds are missing the required Steamworks libraries
                // to compile Steam support in the engine.
                //
                // If you want to enable support, add the following to your .Target.cs file:
                //
                // ProjectDefinitions.Add("ONLINE_SUBSYSTEM_EOS_ENABLE_STEAM=1");
                //
                if (!bExplicitlySetByConfig)
                {
#if UE_5_1_OR_LATER
                    Console.WriteLine("WARNING: Steam authentication for EOS will not be enabled by default, because you are using a source version of the engine or targeting macOS on Unreal Engine 5.1 or later. Read the instructions in OnlineSubsystemRedpointSteam.Build.cs on how to enable support or hide this warning.");
#else
                    Console.WriteLine("WARNING: Steam authentication for EOS will not be enabled by default, because you are using a source version of the engine. Read the instructions in OnlineSubsystemRedpointSteam.Build.cs on how to enable support or hide this warning.");
#endif
                }
                Module.PrivateDefinitions.Add("EOS_STEAM_ENABLED=0");
            }
        }
        else
        {
            Module.PrivateDefinitions.Add("EOS_STEAM_ENABLED=0");
        }
    }
    /* PRECOMPILED REMOVE END */
}