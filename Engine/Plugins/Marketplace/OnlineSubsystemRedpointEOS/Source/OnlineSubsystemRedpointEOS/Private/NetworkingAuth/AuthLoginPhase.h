// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthPhase.h"
#include "OnlineSubsystemRedpointEOS/Shared/AntiCheat/AntiCheat.h"

class IAuthLoginPhase : public IAuthPhase
{
public:
    /**
     * Begin the phase. This is always only called on the server, which ultimately
     * controls the authentication sequence.
     */
    virtual void Start(const TSharedRef<class FAuthLoginPhaseContext> &Context) = 0;

    /**
     * Handle Anti-Cheat authentication status change. This is optional to implement.
     */
    virtual void OnAntiCheatPlayerAuthStatusChanged(
        const TSharedRef<class FAuthLoginPhaseContext> &Context,
        EOS_EAntiCheatCommonClientAuthStatus NewAuthStatus){};

    /**
     * Handle Anti-Cheat action required. This is optional to implement.
     */
    virtual void OnAntiCheatPlayerActionRequired(
        const TSharedRef<class FAuthLoginPhaseContext> &Context,
        EOS_EAntiCheatCommonClientAction ClientAction,
        EOS_EAntiCheatCommonClientActionReason ActionReasonCode,
        const FString &ActionReasonDetailsString){};
};