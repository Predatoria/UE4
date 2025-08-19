// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthVerificationPhase.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthVerificationPhaseContext.h"

#define AUTH_PHASE_ID_TOKEN_AUTH FName(TEXT("IdTokenAuth"))

class FIdTokenAuthPhase : public IAuthVerificationPhase, public TSharedFromThis<FIdTokenAuthPhase>
{
public:
    FIdTokenAuthPhase(){};
    UE_NONCOPYABLE(FIdTokenAuthPhase);
    virtual ~FIdTokenAuthPhase(){};

    virtual FName GetName() const override
    {
        return AUTH_PHASE_ID_TOKEN_AUTH;
    }

    static void RegisterRoutes(class UEOSControlChannel *ControlChannel);
    virtual void Start(const TSharedRef<FAuthVerificationPhaseContext> &Context) override;

private:
    void On_NMT_EOS_RequestIdToken(const TSharedRef<FAuthVerificationPhaseContext> &Context);
    void On_NMT_EOS_DeliverIdToken(
        const TSharedRef<FAuthVerificationPhaseContext> &Context,
        const FString &ClientToken);
};