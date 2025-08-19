// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthVerificationPhase.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthVerificationPhaseContext.h"

#define AUTH_PHASE_LEGACY_CREDENTIAL_AUTH FName(TEXT("LegacyCredentialAuth"))

class FLegacyCredentialAuthPhase : public IAuthVerificationPhase, public TSharedFromThis<FLegacyCredentialAuthPhase>
{
public:
    FLegacyCredentialAuthPhase(){};
    UE_NONCOPYABLE(FLegacyCredentialAuthPhase);
    virtual ~FLegacyCredentialAuthPhase(){};

    virtual FName GetName() const override
    {
        return AUTH_PHASE_LEGACY_CREDENTIAL_AUTH;
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