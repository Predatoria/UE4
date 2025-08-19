// Copyright June Rhodes. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Diagnostics;

public class RedpointEOSEditorTests : ModuleRules
{
    public RedpointEOSEditorTests(ReadOnlyTargetRules Target) : base(Target)
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

            // 4.27's implementation of the Oculus is buggy and causes UE4Editor-Cmd.exe to crash on shutdown unless we do some workarounds.
#if UE_4_27_OR_LATER && !UE_5_0_OR_LATER
            PrivateDefinitions.Add("EOS_HANDLE_BUGGY_OCULUSEDITOR_MODULE=1");
#endif

            PrivateDependencyModuleNames.AddRange(new string[] {
                "Core",
                "Engine",
                "CoreUObject",
                "OnlineSubsystemRedpointEOS",
                "RedpointEOSUtils",
                "RedpointEOSTests",
                "OnlineSubsystem",
                "OnlineSubsystemUtils",
                "RedpointEOSSDK",
                "Json",

                "UnrealEd",
                "LevelEditor",
            });
        }
        /* PRECOMPILED REMOVE END */
    }
}