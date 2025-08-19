// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Public/OnlineSubsystemRedpointEOSModule.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

EOS_ENABLE_STRICT_WARNINGS

#if EOS_VERSION_AT_LEAST(1, 15, 0)

class ONLINESUBSYSTEMREDPOINTEOS_API FEOSLifecycleManager : public TSharedFromThis<FEOSLifecycleManager>
{
private:
    FOnlineSubsystemRedpointEOSModule *Owner;
    TSharedRef<class IEOSRuntimePlatform> RuntimePlatform;
    FUTickerDelegateHandle TickerHandle;
    bool bShouldPollForApplicationStatus;
    bool bShouldPollForNetworkStatus;    

    bool Tick(float DeltaSeconds);

public:
    FEOSLifecycleManager(
        FOnlineSubsystemRedpointEOSModule *InOwner,
        const TSharedRef<class IEOSRuntimePlatform> &InRuntimePlatform);
    UE_NONCOPYABLE(FEOSLifecycleManager);
    ~FEOSLifecycleManager(){};

    void Init();
    void Shutdown();

    void UpdateApplicationStatus(const EOS_EApplicationStatus &InNewStatus);
    void UpdateNetworkStatus(const EOS_ENetworkStatus &InNewStatus);
};

#endif

EOS_DISABLE_STRICT_WARNINGS