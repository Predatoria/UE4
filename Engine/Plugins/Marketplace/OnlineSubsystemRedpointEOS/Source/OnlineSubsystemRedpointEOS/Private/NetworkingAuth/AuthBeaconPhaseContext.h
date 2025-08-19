// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthBeaconPhase.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthPhaseContext.h"

class FAuthBeaconPhaseContext : public FAuthPhaseContext<IAuthBeaconPhase, FAuthBeaconPhaseContext>,
                                public TSharedFromThis<FAuthBeaconPhaseContext>
{
private:
    TSharedRef<const FUniqueNetIdEOS> UserId;
    FString BeaconName;

protected:
    virtual FString GetIdentifier() const override;
    virtual FString GetPhaseGroup() const override;

public:
    FAuthBeaconPhaseContext(
        UEOSControlChannel *InControlChannel,
        const TSharedRef<const FUniqueNetIdEOS> &InUserId,
        const FString &InBeaconName)
        : FAuthPhaseContext(InControlChannel)
        , UserId(InUserId)
        , BeaconName(InBeaconName){};
    UE_NONCOPYABLE(FAuthBeaconPhaseContext);
    virtual ~FAuthBeaconPhaseContext(){};

    FORCEINLINE TSharedRef<const FUniqueNetIdEOS> GetUserId() const
    {
        return this->UserId;
    }

    FORCEINLINE FString GetBeaconName() const
    {
        return this->BeaconName;
    }
};