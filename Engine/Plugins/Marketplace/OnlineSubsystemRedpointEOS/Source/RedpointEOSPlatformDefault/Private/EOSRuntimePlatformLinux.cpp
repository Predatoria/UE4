// Copyright June Rhodes. All Rights Reserved.

#include "EOSRuntimePlatformLinux.h"

#if PLATFORM_LINUX
#if EOS_VERSION_AT_LEAST(1, 15, 0)

#include "HAL/PlatformApplicationMisc.h"

#if !defined(UE_SERVER) || !UE_SERVER

void FEOSRuntimePlatformLinux::RegisterLifecycleHandlers(const TWeakPtr<class FEOSLifecycleManager> &InLifecycleManager)
{
}

bool FEOSRuntimePlatformLinux::ShouldPollLifecycleApplicationStatus() const
{
    return true;
}

bool FEOSRuntimePlatformLinux::ShouldPollLifecycleNetworkStatus() const
{
    return false;
}

void FEOSRuntimePlatformLinux::PollLifecycleApplicationStatus(EOS_EApplicationStatus &OutApplicationStatus) const
{
    if (FPlatformApplicationMisc::IsThisApplicationForeground() || !FApp::CanEverRender())
    {
        // Application is either in foreground, or intentionally run as headless (in which case
        // we treat it as foreground).
        OutApplicationStatus = EOS_EApplicationStatus::EOS_AS_Foreground;
    }
    else
    {
        // Linux does not constrain background applications.
        OutApplicationStatus = EOS_EApplicationStatus::EOS_AS_BackgroundUnconstrained;
    }
}

void FEOSRuntimePlatformLinux::PollLifecycleNetworkStatus(EOS_ENetworkStatus &OutNetworkStatus) const
{
}

#else

void FEOSRuntimePlatformLinux::SetLifecycleForNewPlatformInstance(EOS_HPlatform InPlatform)
{
    EOS_Platform_SetApplicationStatus(InPlatform, EOS_EApplicationStatus::EOS_AS_Foreground);
    EOS_Platform_SetNetworkStatus(InPlatform, EOS_ENetworkStatus::EOS_NS_Online);
}

#endif

#endif
#endif