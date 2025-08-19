// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "./QueuedEntry.h"
#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Private/EOSControlChannel.h"

class FQueuedLoginEntry : public IQueuedEntry, public TSharedFromThis<FQueuedLoginEntry>
{
public:
    FQueuedLoginEntry(
        const FString &InClientResponse,
        const FString &InURLString,
        const FUniqueNetIdRepl &InIncomingUser,
        const FString &InOnlinePlatformNameString,
        UEOSControlChannel *InControlChannel)
        : ClientResponse(InClientResponse)
        , URLString(InURLString)
        , IncomingUser(InIncomingUser)
        , OnlinePlatformNameString(InOnlinePlatformNameString)
        , ControlChannel(InControlChannel)
        , Context(nullptr){};
    UE_NONCOPYABLE(FQueuedLoginEntry);
    virtual ~FQueuedLoginEntry(){};

    FString ClientResponse;
    FString URLString;
    FUniqueNetIdRepl IncomingUser;
    FString OnlinePlatformNameString;
    TSoftObjectPtr<UEOSControlChannel> ControlChannel;
    TSharedPtr<FAuthLoginPhaseContext> Context;

    virtual void SetContext(const TSharedRef<IAuthPhaseContext> &InContext) override;
    virtual void SendSuccess() override;
    virtual void SendFailure(const FString &Message) override;
    virtual bool Contains() override;
    virtual void Track() override;
    virtual void Release() override;
};