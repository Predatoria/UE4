// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthLoginPhase.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthLoginPhaseContext.h"

#define AUTH_PHASE_ANTI_CHEAT_PROOF FName(TEXT("AntiCheatProof"))

#if EOS_VERSION_AT_LEAST(1, 12, 0)

class FAntiCheatProofPhase : public IAuthLoginPhase
{
private:
    FString PendingNonceCheck;

public:
    virtual ~FAntiCheatProofPhase(){};

    virtual FName GetName() const override
    {
        return AUTH_PHASE_ANTI_CHEAT_PROOF;
    }

    static void RegisterRoutes(class UEOSControlChannel *ControlChannel);
    virtual void Start(const TSharedRef<FAuthLoginPhaseContext> &Context) override;

private:
    void On_NMT_EOS_RequestTrustedClientProof(
        const TSharedRef<FAuthLoginPhaseContext> &Context,
        const FString &EncodedNonce);
    void On_NMT_EOS_DeliverTrustedClientProof(
        const TSharedRef<FAuthLoginPhaseContext> &Context,
        bool bCanProvideProof,
        const FString &EncodedProof,
        const FString &PlatformString);
};

#endif // #if EOS_VERSION_AT_LEAST(1, 12, 0)