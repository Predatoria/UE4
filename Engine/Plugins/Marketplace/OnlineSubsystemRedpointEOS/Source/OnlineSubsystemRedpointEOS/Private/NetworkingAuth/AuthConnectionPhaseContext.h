// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthConnectionPhase.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthPhaseContext.h"

class FAuthConnectionPhaseContext : public FAuthPhaseContext<IAuthConnectionPhase, FAuthConnectionPhaseContext>,
                                    public TSharedFromThis<FAuthConnectionPhaseContext>
{
protected:
    virtual FString GetIdentifier() const override;
    virtual FString GetPhaseGroup() const override;

public:
    FAuthConnectionPhaseContext(UEOSControlChannel *InControlChannel)
        : FAuthPhaseContext(InControlChannel){};
    UE_NONCOPYABLE(FAuthConnectionPhaseContext);
    virtual ~FAuthConnectionPhaseContext(){};

    /** Marks the connection as "trusted" on the client, which allows sending sensitive data on the legacy network
     * authentication path. */
    void MarkConnectionAsTrustedOnClient();
};