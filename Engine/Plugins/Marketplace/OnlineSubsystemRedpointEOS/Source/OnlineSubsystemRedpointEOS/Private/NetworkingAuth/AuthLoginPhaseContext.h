// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthLoginPhase.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthPhaseContext.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/UserVerificationStatus.h"

class FAuthLoginPhaseContext : public FAuthPhaseContext<IAuthLoginPhase, FAuthLoginPhaseContext>,
                               public TSharedFromThis<FAuthLoginPhaseContext>
{
private:
    TSharedRef<const FUniqueNetIdEOS> UserId;

protected:
    virtual FString GetIdentifier() const override;
    virtual FString GetPhaseGroup() const override;

public:
    FAuthLoginPhaseContext(UEOSControlChannel *InControlChannel, const TSharedRef<const FUniqueNetIdEOS> &InUserId)
        : FAuthPhaseContext(InControlChannel)
        , UserId(InUserId){};
    UE_NONCOPYABLE(FAuthLoginPhaseContext);
    virtual ~FAuthLoginPhaseContext(){};

    /** Marks the user as registered for Anti-Cheat, which ensures the control channel will unregister the player when
     * the connection closes. */
    void MarkAsRegisteredForAntiCheat();

    // Used by Anti-Cheat phases, even though this should probably be removed...
    void SetVerificationStatus(EUserVerificationStatus InStatus);
    EUserVerificationStatus GetVerificationStatus() const;

    FORCEINLINE TSharedRef<const FUniqueNetIdEOS> GetUserId() const
    {
        return this->UserId;
    }
};