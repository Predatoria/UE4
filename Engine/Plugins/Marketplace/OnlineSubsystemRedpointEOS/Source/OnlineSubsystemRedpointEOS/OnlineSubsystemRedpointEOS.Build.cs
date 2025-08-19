// Copyright June Rhodes. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Diagnostics;

public static class OnlineSubsystemRedpointEOSConfig
{
    public static bool GetBool(ReadOnlyTargetRules Target, string Name, bool bDefaultValue)
    {
        bool bExplicitlySet;
        return GetBool(Target, Name, bDefaultValue, out bExplicitlySet);
    }

    public static bool GetBool(ReadOnlyTargetRules Target, string Name, bool bDefaultValue, out bool bExplicitlySet)
    {
        // Prepend the ONLINE_SUBSYSTEM_EOS_ prefix.
        Name = "ONLINE_SUBSYSTEM_EOS_" + Name;

        // Try to read from the environment variables first.
        var envValue = Environment.GetEnvironmentVariable(Name);
        if (envValue != null)
        {
            if (Name != "ONLINE_SUBSYSTEM_EOS_FORCE_SDK_USAGE_FROM_PLUGIN" &&
                Name != "ONLINE_SUBSYSTEM_EOS_BUILDING_FREE_EDITION")
            {
                Console.WriteLine("WARNING: The configuration value " + Name + " is being read from an environment variable. You should remove the environment variable from your system and migrate to using 'ProjectDefinitions.Add(\"" + Name + "=1\");' in your project targets instead.");
            }

            bExplicitlySet = true;
            return envValue == "true" || envValue == "1";
        }

        // Check to see if the setting is in the target's definitions.
        foreach (string definition in Target.ProjectDefinitions)
        {
            if (definition.StartsWith(Name + "="))
            {
                bExplicitlySet = true;
                return definition == Name + "=1";
            }
        }

        // Otherwise fallback to default.
        bExplicitlySet = false;
        return bDefaultValue;
    }

    public static string GetString(ReadOnlyTargetRules Target, string Name, string DefaultValue)
    {
        // Prepend the ONLINE_SUBSYSTEM_EOS_ prefix.
        Name = "ONLINE_SUBSYSTEM_EOS_" + Name;

        // For permitted settings, try to read from the environment variables first.
        if (Name == "ONLINE_SUBSYSTEM_EOS_FORCE_SDK_VERSION" ||
            Name.StartsWith("ONLINE_SUBSYSTEM_EOS_FORCE_SDK_VERSION_"))
        {
            var envValue = Environment.GetEnvironmentVariable(Name);
            if (!string.IsNullOrWhiteSpace(envValue))
            {
                return envValue;
            }
        }

        // Check to see if the setting is in the target's definitions.
        foreach (string definition in Target.ProjectDefinitions)
        {
            if (definition.StartsWith(Name + "="))
            {
                return definition.Split('=')[1];
            }
        }

        // Otherwise fallback to default.
        return DefaultValue;
    }
}

public class OnlineSubsystemRedpointEOS : ModuleRules
{
    public static void ApplyEngineVersionDefinesToModule(ModuleRules Module)
    {
#if UE_5_1_OR_LATER
        Module.PublicDefinitions.Add("UE_5_1_OR_LATER=1");
#endif
#if UE_5_0_OR_LATER
        Module.PublicDefinitions.Add("UE_5_0_OR_LATER=1");
#endif
#if UE_4_27_OR_LATER
        Module.PublicDefinitions.Add("UE_4_27_OR_LATER=1");
#endif
    }

    public OnlineSubsystemRedpointEOS(ReadOnlyTargetRules Target) : base(Target)
    {
        DefaultBuildSettings = BuildSettingsVersion.V2;
        bUsePrecompiled = false;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "OnlineSubsystemUtils",
                "Projects",
                "Json",

                // Now necessary for the EOSError.h header in Public.
                "OnlineSubsystem",

                // Now necessary as UEOSSubsystem is a UObject which depends on UUserWidget (and because it is a UObject, it can't be excluded for server-only builds).
                "UMG",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "RedpointEOSSDK",
            }
        );

        ApplyEngineVersionDefinesToModule(this);

        // Prevent implicit narrowing of types on Windows. We have to use these macros around any 
        // external headers because the engine is not compliant with these requirements, and turning 
        // them on globally causes the engine headers to fail the build.
#if UE_5_0_OR_LATER
        if (Target.Platform == UnrealTargetPlatform.Win64)
#else
        if (Target.Platform == UnrealTargetPlatform.Win32 ||
        Target.Platform == UnrealTargetPlatform.Win64)
#endif
        {
            PublicDefinitions.Add("EOS_ENABLE_STRICT_WARNINGS=__pragma(warning(push))__pragma(warning(error:4244))__pragma(warning(error:4838))");
            PublicDefinitions.Add("EOS_DISABLE_STRICT_WARNINGS=__pragma(warning(pop))");
        }
        else
        {
            PublicDefinitions.Add("EOS_ENABLE_STRICT_WARNINGS=");
            PublicDefinitions.Add("EOS_DISABLE_STRICT_WARNINGS=");
        }

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

            if (Target.bBuildDeveloperTools || (Target.Configuration != UnrealTargetConfiguration.Shipping && Target.Configuration != UnrealTargetConfiguration.Test))
            {
                PrivateDependencyModuleNames.Add("GameplayDebugger");
                PrivateDefinitions.Add("WITH_GAMEPLAY_DEBUGGER=1");
            }
            else
            {
                PrivateDefinitions.Add("WITH_GAMEPLAY_DEBUGGER=0");
            }

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "CoreUObject",
                    "Engine",
                    "Sockets",
                    "PacketHandler",
                    "NetCore",
#if UE_5_0_OR_LATER
                    "HTTP",
#else
                    "Http",
#endif
                    "RedpointEOSInterfaces",
                    "RedpointLibHydrogen",
                    "AESGCMHandlerComponent",
                    "ImageWrapper",
                    "VoiceChat",
#if UE_5_0_OR_LATER
                    "CoreOnline",
#endif
                }
            );

            if (Target.Type == TargetType.Server ||
                Target.Type == TargetType.Editor)
            {
                PublicDefinitions.Add("EOS_HAS_ORCHESTRATORS=1");
            }
            else
            {
                PublicDefinitions.Add("EOS_HAS_ORCHESTRATORS=0");
            }

            if (Target.Type != TargetType.Server)
            {
                PublicDefinitions.Add("EOS_HAS_AUTHENTICATION=1");
                PublicDefinitions.Add("EOS_HAS_IMAGE_DECODING=1");

                PrivateDependencyModuleNames.AddRange(
                    new string[]
                    {
                        // Required for image decoding.
                        "ImageWrapper",
                        "RHI",
                        "RenderCore",
                    }
                );
            }
            else
            {
                PublicDefinitions.Add("EOS_HAS_AUTHENTICATION=0");
                PublicDefinitions.Add("EOS_HAS_IMAGE_DECODING=0");
            }

            PrivateDefinitions.Add("ONLINESUBSYSTEMEOS_PACKAGE=1");

            if (OnlineSubsystemRedpointEOSConfig.GetBool(Target, "BUILDING_FREE_EDITION", false))
            {
                PrivateDefinitions.Add("EOS_IS_FREE_EDITION=1");
            }

            // Turn this on if you want socket-level network tracing emitted
            // to the logs. This data is *extremely* verbose and should not be used
            // unless you're trying to figure out some hard-to-diagnose networking
            // issue.
            if (OnlineSubsystemRedpointEOSConfig.GetBool(Target, "ENABLE_NETWORK_TRACING", false))
            {
                PrivateDefinitions.Add("EOS_ENABLE_SOCKET_LEVEL_NETWORK_TRACING=1");
            }

            if (!string.IsNullOrWhiteSpace(Environment.GetEnvironmentVariable("BUILD_VERSION_NAME")))
            {
                PrivateDefinitions.Add("EOS_BUILD_VERSION_NAME=\"" + Environment.GetEnvironmentVariable("BUILD_VERSION_NAME") + "\"");
            }

            // Enable trace counters and stats by default in non-shipping builds.
            if (OnlineSubsystemRedpointEOSConfig.GetBool(Target, "ENABLE_TRACE", Target.Configuration != UnrealTargetConfiguration.Shipping))
            {
                PrivateDefinitions.Add("EOS_ENABLE_TRACE=1");
            }

            if (OnlineSubsystemRedpointEOSConfig.GetBool(Target, "ENABLE_STATE_DIAGNOSTICS", false))
            {
                PrivateDefinitions.Add("EOS_ENABLE_STATE_DIAGNOSTICS=1");
            }
        }
        /* PRECOMPILED REMOVE END */
    }
}
