// Copyright June Rhodes. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Diagnostics;

public class RedpointLibHydrogen : ModuleRules
{
    public RedpointLibHydrogen(ReadOnlyTargetRules Target) : base(Target)
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

                    // We use OpenSSL to implement random.h, because Unreal provides it on all platforms (including consoles).
                    "OpenSSL",
                }
            );

            // Due to the random.h header magic, this module must be
            // compiled locally.
            bBuildLocallyWithSNDBS = true;

            PrivateDefinitions.Add("RANDOM_HEADER=openssl");
            PrivateDefinitions.Add("RedpointLibHydrogen_ENABLE_CLANG_TIDY=0");
        }
        /* PRECOMPILED REMOVE END */
    }
}