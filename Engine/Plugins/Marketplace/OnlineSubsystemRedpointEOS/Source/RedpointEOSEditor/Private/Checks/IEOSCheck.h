// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "Containers/UnrealString.h"
#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

class FEOSCheckAction
{
private:
    FString ActionId;
    FString ActionDisplayName;

public:
    FEOSCheckAction(const FString &InActionId, const FString &InActionDisplayName)
        : ActionId(InActionId)
        , ActionDisplayName(InActionDisplayName){};

    const FString &GetActionId() const
    {
        return this->ActionId;
    }
    const FString &GetActionDisplayName() const
    {
        return this->ActionDisplayName;
    }
};

class FEOSCheckEntry
{
private:
    FString CheckId;
    FString CheckMessage;
    TArray<FEOSCheckAction> CheckActions;

public:
    FEOSCheckEntry(
        const FString &InCheckId,
        const FString &InCheckMessage,
        const TArray<FEOSCheckAction> &InCheckActions)
        : CheckId(InCheckId)
        , CheckMessage(InCheckMessage)
        , CheckActions(InCheckActions){};

    const FString &GetCheckId() const
    {
        return this->CheckId;
    }
    const FString &GetCheckMessage() const
    {
        return this->CheckMessage;
    }
    const TArray<FEOSCheckAction> &GetCheckActions() const
    {
        return this->CheckActions;
    }
};

class IEOSCheck
{
protected:
    static const TArray<FEOSCheckEntry> EmptyEntries;

public:
    IEOSCheck() = default;
    UE_NONCOPYABLE(IEOSCheck);
    virtual ~IEOSCheck(){};

    virtual bool ShouldTick() const
    {
        return false;
    }

    virtual const TArray<FEOSCheckEntry> Tick(float DeltaSeconds) const
    {
        return IEOSCheck::EmptyEntries;
    }

    virtual const TArray<FEOSCheckEntry> ProcessCustomSignal(const FString &Context, const FString &SignalId) const
    {
        return IEOSCheck::EmptyEntries;
    }

    virtual const TArray<FEOSCheckEntry> ProcessLogMessage(
        EOS_ELogLevel InLogLevel,
        const FString &Category,
        const FString &Message) const
    {
        return IEOSCheck::EmptyEntries;
    }

    virtual void HandleAction(const FString &CheckId, const FString &ActionId) const
    {
    }
};