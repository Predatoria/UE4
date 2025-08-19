// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthLoginPhase.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthLoginPhaseContext.h"

#define AUTH_PHASE_ANTI_CHEAT_INTEGRITY FName(TEXT("AntiCheatIntegrity"))

#if EOS_VERSION_AT_LEAST(1, 12, 0)

class FAntiCheatIntegrityPhase : public IAuthLoginPhase
{
private:
    bool bIsStarted;
    bool bIsVerified;

public:
    FAntiCheatIntegrityPhase()
        : bIsStarted(false)
        , bIsVerified(false){};
    virtual ~FAntiCheatIntegrityPhase(){};

    virtual FName GetName() const override
    {
        return AUTH_PHASE_ANTI_CHEAT_INTEGRITY;
    }

    static void RegisterRoutes(class UEOSControlChannel *ControlChannel);
    virtual void Start(const TSharedRef<FAuthLoginPhaseContext> &Context) override;

    virtual void OnAntiCheatPlayerAuthStatusChanged(
        const TSharedRef<FAuthLoginPhaseContext> &Context,
        EOS_EAntiCheatCommonClientAuthStatus NewAuthStatus) override;
    virtual void OnAntiCheatPlayerActionRequired(
        const TSharedRef<FAuthLoginPhaseContext> &Context,
        EOS_EAntiCheatCommonClientAction ClientAction,
        EOS_EAntiCheatCommonClientActionReason ActionReasonCode,
        const FString &ActionReasonDetailsString) override;
};

#endif // #if EOS_VERSION_AT_LEAST(1, 12, 0)