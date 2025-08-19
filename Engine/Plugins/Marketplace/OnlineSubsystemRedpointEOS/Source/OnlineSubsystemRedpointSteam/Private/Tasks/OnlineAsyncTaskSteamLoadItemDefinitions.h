// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "./OnlineAsyncTaskSteamDelegates.h"
#include "OnlineAsyncTaskManager.h"

#if EOS_STEAM_ENABLED

THIRD_PARTY_INCLUDES_START
#include "steam/steam_api.h"
THIRD_PARTY_INCLUDES_END

EOS_ENABLE_STRICT_WARNINGS

class FOnlineAsyncTaskSteamLoadItemDefinitions : public FOnlineAsyncTaskBasic<class FOnlineSubsystemSteam>
{
private:
    bool bInit;
    SteamAPICall_t LoadCallbackHandle;
    SteamInventoryDefinitionUpdate_t LoadCallbackResults;
    FSteamItemLoadComplete Delegate;
    FString FailureContext;

public:
    FOnlineAsyncTaskSteamLoadItemDefinitions(FSteamItemLoadComplete InDelegate)
        : bInit(false)
        , LoadCallbackHandle()
        , LoadCallbackResults()
        , Delegate(MoveTemp(InDelegate))
        , FailureContext()
    {
    }

    virtual ~FOnlineAsyncTaskSteamLoadItemDefinitions()
    {
    }

    virtual FString ToString() const override
    {
        return TEXT("FOnlineAsyncTaskSteamLoadItemDefinitions");
    }

    virtual void Tick() override;
    virtual void Finalize() override;
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_STEAM_ENABLED