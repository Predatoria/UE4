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

class FOnlineAsyncTaskSteamGetAllItems : public FOnlineAsyncTaskBasic<class FOnlineSubsystemSteam>
{
private:
    bool bInit;
    SteamInventoryResult_t Result;
    FSteamEntitlementsFetched Delegate;
    FString FailureContext;

public:
    FOnlineAsyncTaskSteamGetAllItems(FSteamEntitlementsFetched InDelegate)
        : bInit(false)
        , Result()
        , Delegate(MoveTemp(InDelegate))
    {
    }

    virtual ~FOnlineAsyncTaskSteamGetAllItems()
    {
    }

    virtual FString ToString() const override
    {
        return TEXT("FOnlineAsyncTaskSteamGetAllItems");
    }

    virtual void Tick() override;
    virtual void Finalize() override;
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_STEAM_ENABLED