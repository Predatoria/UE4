// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthVerificationPhase.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthVerificationPhaseContext.h"

#define AUTH_PHASE_LEGACY_IDENTITY_CHECK FName(TEXT("LegacyIdentityCheck"))

class FLegacyIdentityCheckPhase : public IAuthVerificationPhase, public TSharedFromThis<FLegacyIdentityCheckPhase>
{
private:
    FDelegateHandle UserInfoHandle;

public:
    FLegacyIdentityCheckPhase(){};
    UE_NONCOPYABLE(FLegacyIdentityCheckPhase);
    virtual ~FLegacyIdentityCheckPhase(){};

    virtual FName GetName() const override
    {
        return AUTH_PHASE_LEGACY_IDENTITY_CHECK;
    }

    static void RegisterRoutes(class UEOSControlChannel *ControlChannel);
    virtual void Start(const TSharedRef<FAuthVerificationPhaseContext> &Context) override;

private:
    void OnUserInfoReceived(
        int32 LocalUserNum,
        bool bWasSuccessful,
        const TArray<TSharedRef<const FUniqueNetId>> &UserIds,
        const FString &ErrorString,
        // NOLINTNEXTLINE(performance-unnecessary-value-param)
        TSharedRef<FAuthVerificationPhaseContext> Context);
};