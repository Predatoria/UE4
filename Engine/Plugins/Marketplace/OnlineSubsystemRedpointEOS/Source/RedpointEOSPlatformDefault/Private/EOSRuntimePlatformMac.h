// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "EOSRuntimePlatformDesktopBase.h"

#if PLATFORM_MAC

class FEOSRuntimePlatformMac : public FEOSRuntimePlatformDesktopBase
{
public:
    FEOSRuntimePlatformMac() = default;
    virtual ~FEOSRuntimePlatformMac() = default;

#if EOS_VERSION_AT_LEAST(1, 15, 0)
    virtual void RegisterLifecycleHandlers(const TWeakPtr<class FEOSLifecycleManager> &InLifecycleManager) override;
    virtual bool ShouldPollLifecycleApplicationStatus() const override;
    virtual bool ShouldPollLifecycleNetworkStatus() const override;
    virtual void PollLifecycleApplicationStatus(EOS_EApplicationStatus &OutApplicationStatus) const override;
    virtual void PollLifecycleNetworkStatus(EOS_ENetworkStatus &OutNetworkStatus) const override;
#endif
};

#endif