// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "EOSRuntimePlatformDesktopBase.h"

#if PLATFORM_LINUX

class FEOSRuntimePlatformLinux : public FEOSRuntimePlatformDesktopBase
{
public:
    FEOSRuntimePlatformLinux() = default;
    virtual ~FEOSRuntimePlatformLinux() = default;

#if EOS_VERSION_AT_LEAST(1, 15, 0)
#if !defined(UE_SERVER) || !UE_SERVER
    virtual void RegisterLifecycleHandlers(const TWeakPtr<class FEOSLifecycleManager> &InLifecycleManager) override;
    virtual bool ShouldPollLifecycleApplicationStatus() const override;
    virtual bool ShouldPollLifecycleNetworkStatus() const override;
    virtual void PollLifecycleApplicationStatus(EOS_EApplicationStatus &OutApplicationStatus) const override;
    virtual void PollLifecycleNetworkStatus(EOS_ENetworkStatus &OutNetworkStatus) const override;
#else
    virtual void SetLifecycleForNewPlatformInstance(EOS_HPlatform InPlatform) override;
#endif
#endif
};

#endif