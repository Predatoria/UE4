// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "Containers/UnrealString.h"
#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthPhaseContext.h"

class IQueuedEntry
{
public:
    virtual ~IQueuedEntry(){};
    virtual void SetContext(const TSharedRef<IAuthPhaseContext> &InContext) = 0;
    virtual void SendSuccess() = 0;
    virtual void SendFailure(const FString &Message) = 0;
    virtual bool Contains() = 0;
    virtual void Track() = 0;
    virtual void Release() = 0;
};