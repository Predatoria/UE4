// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "Containers/UnrealString.h"
#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

#if PLATFORM_WINDOWS && EOS_VERSION_AT_LEAST(1, 15, 0)

class FAuthorizeLauncher
{
private:
    static bool bInProgress;

public:
    static bool CanLaunch();
    static bool Launch(bool bInteractive);
};

#endif