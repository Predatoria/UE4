// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthVerificationPhase.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthVerificationPhaseContext.h"

#define AUTH_PHASE_P2P_ADDRESS_CHECK FName(TEXT("P2PAddressCheck"))

class FP2PAddressCheckPhase : public IAuthVerificationPhase, public TSharedFromThis<FP2PAddressCheckPhase>
{
public:
    FP2PAddressCheckPhase(){};
    UE_NONCOPYABLE(FP2PAddressCheckPhase);
    virtual ~FP2PAddressCheckPhase(){};

    virtual FName GetName() const override
    {
        return AUTH_PHASE_P2P_ADDRESS_CHECK;
    }

    static void RegisterRoutes(class UEOSControlChannel *ControlChannel);
    virtual void Start(const TSharedRef<FAuthVerificationPhaseContext> &Context) override;

private:
    void On_NMT_EOS_RequestClientToken(const TSharedRef<FAuthVerificationPhaseContext> &Context);
    void On_NMT_EOS_DeliverClientToken(
        const TSharedRef<FAuthVerificationPhaseContext> &Context,
        const FString &ClientTokenType,
        const FString &ClientDisplayName,
        const FString &ClientToken);
};