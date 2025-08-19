// Copyright June Rhodes. All Rights Reserved.

#include "EOSRuntimePlatformMac.h"

#if PLATFORM_MAC
#if EOS_VERSION_AT_LEAST(1, 15, 0)

#include "HAL/PlatformApplicationMisc.h"

void FEOSRuntimePlatformMac::RegisterLifecycleHandlers(const TWeakPtr<class FEOSLifecycleManager> &InLifecycleManager)
{
}

bool FEOSRuntimePlatformMac::ShouldPollLifecycleApplicationStatus() const
{
    return true;
}

bool FEOSRuntimePlatformMac::ShouldPollLifecycleNetworkStatus() const
{
    return false;
}

void FEOSRuntimePlatformMac::PollLifecycleApplicationStatus(EOS_EApplicationStatus &OutApplicationStatus) const
{
    if (FPlatformApplicationMisc::IsThisApplicationForeground() || !FApp::CanEverRender())
    {
        // Application is either in foreground, or intentionally run as headless (in which case
        // we treat it as foreground).
        OutApplicationStatus = EOS_EApplicationStatus::EOS_AS_Foreground;
    }
    else
    {
        // macOS does not constrain background applications.
        OutApplicationStatus = EOS_EApplicationStatus::EOS_AS_BackgroundUnconstrained;
    }
}

void FEOSRuntimePlatformMac::PollLifecycleNetworkStatus(EOS_ENetworkStatus &OutNetworkStatus) const
{
}

#endif
#endif