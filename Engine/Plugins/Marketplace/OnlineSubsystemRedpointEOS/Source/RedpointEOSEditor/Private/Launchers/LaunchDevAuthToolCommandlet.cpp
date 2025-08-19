// Copyright June Rhodes. All Rights Reserved.

#include "LaunchDevAuthToolCommandlet.h"

#include "DevAuthToolLauncher.h"

ULaunchDevAuthToolCommandlet::ULaunchDevAuthToolCommandlet(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;

    LogToConsole = true;
    ShowErrorCount = true;
    ShowProgress = true;
}

int32 ULaunchDevAuthToolCommandlet::Main(const FString &Params)
{
    return FDevAuthToolLauncher::Launch(false) ? 0 : 1;
}