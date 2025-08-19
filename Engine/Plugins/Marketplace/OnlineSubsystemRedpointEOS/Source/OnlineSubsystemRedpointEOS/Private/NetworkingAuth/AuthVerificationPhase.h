// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthPhase.h"

class IAuthVerificationPhase : public IAuthPhase
{
public:
    /**
     * Begin the phase. This is always only called on the server, which ultimately
     * controls the authentication sequence.
     */
    virtual void Start(const TSharedRef<class FAuthVerificationPhaseContext> &Context) = 0;
};