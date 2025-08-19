// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_ORCHESTRATORS

#include "CoreMinimal.h"

struct ONLINESUBSYSTEMREDPOINTEOS_API FServerOrchestratorPortMapping
{
private:
    FName UnrealEnginePort;
    int16 ExternalPort;

public:
    FServerOrchestratorPortMapping(const FName &InUnrealEnginePort, const int16 &InExternalPort)
        : UnrealEnginePort(InUnrealEnginePort)
        , ExternalPort(InExternalPort){};
    FServerOrchestratorPortMapping(const FServerOrchestratorPortMapping &InOther)
        : UnrealEnginePort(InOther.UnrealEnginePort)
        , ExternalPort(InOther.ExternalPort){};
    FServerOrchestratorPortMapping(FServerOrchestratorPortMapping &&InOther)
        : UnrealEnginePort(InOther.UnrealEnginePort)
        , ExternalPort(InOther.ExternalPort){};
    ~FServerOrchestratorPortMapping(){};

    const FName &GetPortName() const
    {
        return this->UnrealEnginePort;
    }
    const int16 &GetPortValue() const
    {
        return this->ExternalPort;
    }
};

DECLARE_DELEGATE_OneParam(
    FServerOrchestratorGetPortMappingsComplete,
    const TArray<FServerOrchestratorPortMapping> & /* PortMappings */);
DECLARE_DELEGATE(FServerOrchestratorServingTrafficComplete);
DECLARE_DELEGATE(FServerOrchestratorSessionStartedComplete);
DECLARE_DELEGATE(FServerOrchestratorShutdownComplete);

class ONLINESUBSYSTEMREDPOINTEOS_API IServerOrchestrator
{
public:
    /**
     * If this returns true, the dedicated server is running under this orchestrator and should use this implementation.
     */
    virtual bool IsEnabled() const = 0;

    /**
     * Initializes the server orchestrator intergration. This happens at startup and is expected to run for the lifetime
     * of the dedicated server process. This is where you could, e.g. set up health check loops.
     */
    virtual bool Init() = 0;

    /**
     * Fetches the port mappings for the dedicated server. If a mapping for NAME_GamePort is returned, this will be set
     * to __EOS_OverrideAddressBound. For any other ports, they are exposed as Port_<Name> in the session attribute
     * list.
     *
     * Returns true if the callback will be fired.
     */
    virtual bool GetPortMappings(
        const FServerOrchestratorGetPortMappingsComplete &OnComplete =
            FServerOrchestratorGetPortMappingsComplete()) = 0;

    /**
     * Inform the orchestrator that we are now ready to receive game traffic; this is called when CreateSession is
     * called.
     */
    virtual bool ServingTraffic(
        const FName &SessionName,
        const FString &SessionId,
        const FServerOrchestratorServingTrafficComplete &OnComplete = FServerOrchestratorServingTrafficComplete()) = 0;

    /**
     * Inform the orchestrator that the match has started and that this game server process should not be terminated.
     * This is called by the plugin when StartSession is called.
     */
    virtual bool SessionStarted(
        const FServerOrchestratorSessionStartedComplete &OnComplete = FServerOrchestratorSessionStartedComplete()) = 0;

    /**
     * If true, the game should treat an EndSession call as DestroySession. This is the case for orchestrators that
     * will terminate the game server process when Shutdown is called from EndSession, since in these cases you don't
     * want a stale unusable session left in the session list to expire.
     */
    virtual bool ShouldDestroySessionOnEndSession() = 0;

    /**
     * Inform the orchestrator that the game server should be shutdown and deallocated. This is called by the plugin
     * when either EndSession or DestroySession is called.
     */
    virtual bool Shutdown(
        const FServerOrchestratorShutdownComplete &OnComplete = FServerOrchestratorShutdownComplete()) = 0;
};

#endif // #if EOS_HAS_ORCHESTRATORS