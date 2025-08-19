// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if PLATFORM_IOS

#include "OnlineSubsystemRedpointEOS/Shared/EOSRuntimePlatform.h"

class FEOSRuntimePlatformIOS : public IEOSRuntimePlatform
{
private:
    TArray<TSharedPtr<IEOSRuntimePlatformIntegration>> Integrations;
#if EOS_VERSION_AT_LEAST(1, 13, 0)
    EOS_Platform_RTCOptions RTCOptions;
#endif

public:
    virtual ~FEOSRuntimePlatformIOS(){};

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