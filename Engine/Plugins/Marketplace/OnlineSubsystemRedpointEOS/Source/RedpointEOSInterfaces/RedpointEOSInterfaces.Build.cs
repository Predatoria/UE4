// Copyright June Rhodes. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Diagnostics;

public class RedpointEOSInterfaces : ModuleRules
{
    public RedpointEOSInterfaces(ReadOnlyTargetRules Target) : base(Target)
    {
        DefaultBuildSettings = BuildSettingsVersion.V2;
        bUsePrecompiled = false;
        Type = ModuleType.External;

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

            if (!File.Exists(Path.Combine(ModuleDirectory, "Private", "Interfaces", "README.md")))
            {
                throw new Exception("error: You have not cloned the plugin correctly with Git. You must use 'git clone --recursive git@src.redpoint.games:redpointgames/eos-online-subsystem.git' to ensure all dependencies are fetched. Do not try to download the ZIP archive from GitLab - it does not include all of the required files.");
            }
        }
        /* PRECOMPILED REMOVE END */
    }
}