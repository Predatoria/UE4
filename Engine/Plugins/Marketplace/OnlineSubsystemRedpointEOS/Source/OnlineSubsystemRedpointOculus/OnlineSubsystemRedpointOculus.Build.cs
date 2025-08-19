// Copyright June Rhodes. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Diagnostics;

public class OnlineSubsystemRedpointOculus : ModuleRules
{
    public OnlineSubsystemRedpointOculus(ReadOnlyTargetRules Target) : base(Target)
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

            if (Target.Platform == UnrealTargetPlatform.Win64 ||
                Target.Platform == UnrealTargetPlatform.Android)
            {
                PrivateDependencyModuleNames.AddRange(
                    new string[]
                    {
                        "Core",
                    });

                if (OnlineSubsystemRedpointEOSConfig.GetBool(Target, "ENABLE_OCULUS", true))
                {
                    PrivateDefinitions.Add("EOS_OCULUS_ENABLED=1");

                    PrivateDependencyModuleNames.AddRange(
                        new string[]
                        {
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

                    PublicDependencyModuleNames.AddRange(
                        new string[]
                        {
                            "OVRPlugin",
                            "LibOVRPlatform",
                            "OnlineSubsystemOculus",
                        }
                    );
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
        /* PRECOMPILED REMOVE END */
    }
}