// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthPhaseContext.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthVerificationPhase.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/UserVerificationStatus.h"

class FAuthVerificationPhaseContext : public FAuthPhaseContext<IAuthVerificationPhase, FAuthVerificationPhaseContext>,
                                      public TSharedFromThis<FAuthVerificationPhaseContext>
{
private:
    TSharedRef<const FUniqueNetIdEOS> UserId;

protected:
    virtual FString GetIdentifier() const override;
    virtual FString GetPhaseGroup() const override;

public:
    FAuthVerificationPhaseContext(
        UEOSControlChannel *InControlChannel,
        const TSharedRef<const FUniqueNetIdEOS> &InUserId)
        : FAuthPhaseContext(InControlChannel)
        , UserId(InUserId){};
    UE_NONCOPYABLE(FAuthVerificationPhaseContext);
    virtual ~FAuthVerificationPhaseContext(){};

    void SetVerificationStatus(EUserVerificationStatus InStatus);
    EUserVerificationStatus GetVerificationStatus() const;

    /** Returns if the client trusts the dedicated server. Only used for legacy authentication. */
    bool IsConnectionAsTrustedOnClient() const;

    FORCEINLINE TSharedRef<const FUniqueNetIdEOS> GetUserId() const
    {
        return this->UserId;
    }
};