// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_ORCHESTRATORS

#include "./IServerOrchestrator.h"
#include "CoreMinimal.h"
#include "HttpModule.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"

class ONLINESUBSYSTEMREDPOINTEOS_API FAgonesServerOrchestrator : public IServerOrchestrator,
                                                                 public TSharedFromThis<FAgonesServerOrchestrator>
{
private:
    int16 AgonesHttpServerPort;
    FUTickerDelegateHandle HealthCheckTimerHandle;
    bool bHasSentReady;
    bool bHasSentAllocate;
    bool bHasSentShutdown;

    void HandleHealthCheck(FHttpRequestPtr HttpRequest, const FHttpResponsePtr HttpResponse, const bool bSucceeded);
    bool SendHealthCheck(float DeltaSeconds);

    bool RetryGetPortMappings(float DeltaSeconds, FServerOrchestratorGetPortMappingsComplete OnComplete);
    void HandleGameServer(
        FHttpRequestPtr HttpRequest,
        const FHttpResponsePtr HttpResponse,
        const bool bSucceeded,
        FServerOrchestratorGetPortMappingsComplete OnComplete);
    void HandleLabel(
        FHttpRequestPtr HttpRequest,
        const FHttpResponsePtr HttpResponse,
        const bool bSucceeded,
        FServerOrchestratorServingTrafficComplete OnComplete);
    void HandleReady(
        FHttpRequestPtr HttpRequest,
        const FHttpResponsePtr HttpResponse,
        const bool bSucceeded,
        FServerOrchestratorServingTrafficComplete OnComplete);
    void HandleAllocate(
        FHttpRequestPtr HttpRequest,
        const FHttpResponsePtr HttpResponse,
        const bool bSucceeded,
        FServerOrchestratorSessionStartedComplete OnComplete);
    void HandleShutdown(
        FHttpRequestPtr HttpRequest,
        const FHttpResponsePtr HttpResponse,
        const bool bSucceeded,
        FServerOrchestratorShutdownComplete OnComplete);

public:
    FAgonesServerOrchestrator();
    UE_NONCOPYABLE(FAgonesServerOrchestrator);
    virtual ~FAgonesServerOrchestrator();

    virtual bool IsEnabled() const override;
    virtual bool Init() override;
    virtual bool GetPortMappings(
        const FServerOrchestratorGetPortMappingsComplete &OnComplete =
            FServerOrchestratorGetPortMappingsComplete()) override;
    virtual bool ServingTraffic(
        const FName &SessionName,
        const FString &SessionId,
        const FServerOrchestratorServingTrafficComplete &OnComplete =
            FServerOrchestratorServingTrafficComplete()) override;
    virtual bool SessionStarted(
        const FServerOrchestratorSessionStartedComplete &OnComplete =
            FServerOrchestratorSessionStartedComplete()) override;
    virtual bool ShouldDestroySessionOnEndSession() override;
    virtual bool Shutdown(
        const FServerOrchestratorShutdownComplete &OnComplete = FServerOrchestratorShutdownComplete()) override;
};

#endif // #if EOS_HAS_ORCHESTRATORS