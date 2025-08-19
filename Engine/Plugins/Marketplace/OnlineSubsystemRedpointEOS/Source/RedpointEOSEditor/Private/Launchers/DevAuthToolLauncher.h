// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "Containers/UnrealString.h"
#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"

#if PLATFORM_WINDOWS || PLATFORM_MAC

class FDevAuthToolLauncher
{
public:
    static bool bNeedsToCheckIfDevToolRunning;
    static bool bIsDevToolRunning;
    static FUTickerDelegateHandle ResetCheckHandle;

    static FString GetPathToToolsFolder();
    static bool IsDevAuthToolRunning();

    static bool Launch(bool bInteractive);
};

#endif