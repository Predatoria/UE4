// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX

#include "OnlineSubsystemRedpointEOS/Shared/EOSRuntimePlatform.h"
#if PLATFORM_WINDOWS && EOS_VERSION_AT_LEAST(1, 13, 0)
#include "Windows/eos_Windows.h"
#endif

class FEOSRuntimePlatformDesktopBase : public IEOSRuntimePlatform
{
private:
    void *DynamicLibraryHandle;
    TArray<TSharedPtr<IEOSRuntimePlatformIntegration>> Integrations;
#if EOS_VERSION_AT_LEAST(1, 13, 0)
    EOS_Platform_RTCOptions RTCOptions;
#if PLATFORM_WINDOWS
    EOS_Windows_RTCOptions WindowsRTCOptions;
    const char *XAudio29DllPathAllocated;
#endif
#endif

protected:
    FEOSRuntimePlatformDesktopBase() = default;

public:
    virtual ~FEOSRuntimePlatformDesktopBase() = default;

    virtual void Load() override;
    virtual void Unload() override;
    virtual bool IsValid() override;
    virtual void *GetSystemInitializeOptions() override;
#if EOS_VERSION_AT_LEAST(1, 13, 0)
    virtual EOS_Platform_RTCOptions *GetRTCOptions() override;
#endif
    virtual FString GetCacheDirectory() override;
    virtual void ClearStoredEASRefreshToken(int32 LocalUserNum) override{};
#if !UE_BUILD_SHIPPING
    virtual FString GetPathToEASAutomatedTestingCredentials() override;
#endif
    virtual const TArray<TSharedPtr<IEOSRuntimePlatformIntegration>> &GetIntegrations() const override;
    virtual bool UseCrossPlatformFriendStorage() const override;
};

#endif