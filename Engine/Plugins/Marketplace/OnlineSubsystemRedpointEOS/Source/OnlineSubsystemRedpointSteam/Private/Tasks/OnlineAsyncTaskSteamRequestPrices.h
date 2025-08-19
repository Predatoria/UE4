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

class FOnlineAsyncTaskSteamRequestPrices : public FOnlineAsyncTaskBasic<class FOnlineSubsystemSteam>
{
private:
    bool bInit;
    SteamAPICall_t PriceCallbackHandle;
    SteamInventoryRequestPricesResult_t PriceCallbackResults;
    FSteamOffersFetched Delegate;
    FString FailureContext;
    FString CurrencyCode;

public:
    FOnlineAsyncTaskSteamRequestPrices(FSteamOffersFetched InDelegate)
        : bInit(false)
        , PriceCallbackHandle()
        , PriceCallbackResults()
        , Delegate(MoveTemp(InDelegate))
        , FailureContext(){};
    virtual ~FOnlineAsyncTaskSteamRequestPrices(){};

    virtual FString ToString() const override
    {
        return TEXT("FOnlineAsyncTaskSteamRequestPrices");
    }

    virtual void Tick() override;
    virtual void Finalize() override;
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_STEAM_ENABLED