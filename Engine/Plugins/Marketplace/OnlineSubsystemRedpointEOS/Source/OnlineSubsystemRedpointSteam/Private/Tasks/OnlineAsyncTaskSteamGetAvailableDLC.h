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

class FOnlineAsyncTaskSteamGetAvailableDLC : public FOnlineAsyncTaskBasic<class FOnlineSubsystemSteam>
{
private:
    bool bInit;
    FSteamOffersFetched Delegate;
    FString FailureContext;

public:
    FOnlineAsyncTaskSteamGetAvailableDLC(FSteamOffersFetched InDelegate)
        : bInit(false)
        , Delegate(MoveTemp(InDelegate))
        , FailureContext()
    {
    }

    virtual ~FOnlineAsyncTaskSteamGetAvailableDLC()
    {
    }

    virtual FString ToString() const override
    {
        return TEXT("FOnlineAsyncTaskSteamGetAvailableDLC");
    }

    virtual void Tick() override;
    virtual void Finalize() override;
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_STEAM_ENABLED