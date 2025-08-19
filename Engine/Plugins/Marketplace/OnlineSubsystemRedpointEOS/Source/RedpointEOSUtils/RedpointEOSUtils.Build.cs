// Copyright June Rhodes. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Diagnostics;

public class RedpointEOSUtils : ModuleRules
{
    public RedpointEOSUtils(ReadOnlyTargetRules Target) : base(Target)
    {
        DefaultBuildSettings = BuildSettingsVersion.V2;
        bUsePrecompiled = false;

        OnlineSubsystemRedpointEOS.ApplyEngineVersionDefinesToModule(this);

        /* PRECOMPILED REMOVE BEGIN */
        if (!bUsePrecompiled)
        {
            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "Engine",
                    "CoreUObject",
                    "OnlineSubsystem",
                    "OnlineSubsystemUtils",
                    "OnlineSubsystemRedpointEOS",
                }
            );
        }
        /* PRECOMPILED REMOVE END */
    }
}