// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "./QueuedEntry.h"
#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Private/EOSControlChannel.h"

class FQueuedBeaconEntry : public IQueuedEntry, public TSharedFromThis<FQueuedBeaconEntry>
{
public:
    FQueuedBeaconEntry(
        const FString &InBeaconType,
        const FUniqueNetIdRepl &InIncomingUser,
        UEOSControlChannel *InControlChannel)
        : BeaconType(InBeaconType)
        , IncomingUser(InIncomingUser)
        , ControlChannel(InControlChannel)
        , Context(nullptr){};
    UE_NONCOPYABLE(FQueuedBeaconEntry);
    virtual ~FQueuedBeaconEntry(){};

    FString BeaconType;
    FUniqueNetIdRepl IncomingUser;
    TSoftObjectPtr<UEOSControlChannel> ControlChannel;
    TSharedPtr<FAuthBeaconPhaseContext> Context;

    virtual void SetContext(const TSharedRef<IAuthPhaseContext> &InContext) override;
    virtual void SendSuccess() override;
    virtual void SendFailure(const FString &Message) override;
    virtual bool Contains() override;
    virtual void Track() override;
    virtual void Release() override;
};