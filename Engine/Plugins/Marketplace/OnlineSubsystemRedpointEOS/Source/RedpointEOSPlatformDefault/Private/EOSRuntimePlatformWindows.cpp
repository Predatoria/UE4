// Copyright June Rhodes. All Rights Reserved.

#include "EOSRuntimePlatformWindows.h"

#if PLATFORM_WINDOWS
#if EOS_VERSION_AT_LEAST(1, 15, 0)

#include "HAL/PlatformApplicationMisc.h"
#include "Misc/App.h"

#if !defined(UE_SERVER) || !UE_SERVER

void FEOSRuntimePlatformWindows::RegisterLifecycleHandlers(
    const TWeakPtr<class FEOSLifecycleManager> &InLifecycleManager)
{
}

bool FEOSRuntimePlatformWindows::ShouldPollLifecycleApplicationStatus() const
{

    return true;
}

bool FEOSRuntimePlatformWindows::ShouldPollLifecycleNetworkStatus() const
{
    return false;
}

void FEOSRuntimePlatformWindows::PollLifecycleApplicationStatus(EOS_EApplicationStatus &OutApplicationStatus) const
{
    if (FPlatformApplicationMisc::IsThisApplicationForeground() || !FApp::CanEverRender())
    {
        // Application is either in foreground, or intentionally run as headless (in which case
        // we treat it as foreground).
        OutApplicationStatus = EOS_EApplicationStatus::EOS_AS_Foreground;
    }
    else
    {
        // Windows does not constrain background applications.
        OutApplicationStatus = EOS_EApplicationStatus::EOS_AS_BackgroundUnconstrained;
    }
}

void FEOSRuntimePlatformWindows::PollLifecycleNetworkStatus(EOS_ENetworkStatus &OutNetworkStatus) const
{
}

#else

void FEOSRuntimePlatformWindows::SetLifecycleForNewPlatformInstance(EOS_HPlatform InPlatform)
{
    UE_LOG(
        LogEOS,
        Verbose,
        TEXT("FEOSRuntimePlatformWindows::SetLifecycleForNewPlatformInstance: Platform instance %p application initial "
             "status set to foreground."),
        InPlatform);
    EOS_Platform_SetApplicationStatus(InPlatform, EOS_EApplicationStatus::EOS_AS_Foreground);
    EOS_Platform_SetNetworkStatus(InPlatform, EOS_ENetworkStatus::EOS_NS_Online);
}

#endif

#endif
#endif