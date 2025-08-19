// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "./OnlineAsyncTaskSteamDelegates.h"
#include "OnlineAsyncTaskManager.h"
#include "Interfaces/OnlinePurchaseInterface.h"
#include "../OnlinePurchaseInterfaceRedpointSteam.h"

#if EOS_STEAM_ENABLED

THIRD_PARTY_INCLUDES_START
#include "steam/steam_api.h"
THIRD_PARTY_INCLUDES_END

EOS_ENABLE_STRICT_WARNINGS

class FOnlineAsyncTaskSteamPurchase : public FOnlineAsyncTaskBasic<class FOnlineSubsystemSteam>
{
private:
    bool bInit;
    FPurchaseCheckoutRequest CheckoutRequest;
    FOnPurchaseCheckoutComplete Delegate;
    SteamAPICall_t CallbackHandle;
    SteamInventoryStartPurchaseResult_t CallbackResults;
    TWeakPtr<FOnlinePurchaseInterfaceRedpointSteam, ESPMode::ThreadSafe> InterfaceWk;
    FString FailureContext;
    uint64 OrderId;
    uint64 TransactionId;

public:
    FOnlineAsyncTaskSteamPurchase(
        const FPurchaseCheckoutRequest &InCheckoutRequest,
        const FOnPurchaseCheckoutComplete &InDelegate,
        TWeakPtr<FOnlinePurchaseInterfaceRedpointSteam, ESPMode::ThreadSafe> InInterface)
        : bInit(false)
        , CheckoutRequest(InCheckoutRequest)
        , Delegate(InDelegate)
        , CallbackHandle()
        , CallbackResults()
        , InterfaceWk(MoveTemp(InInterface))
        , FailureContext()
        , OrderId()
        , TransactionId()
    {
    }

    virtual ~FOnlineAsyncTaskSteamPurchase()
    {
    }

    virtual FString ToString() const override
    {
        return TEXT("FOnlineAsyncTaskSteamPurchase");
    }

    virtual void Tick() override;
    virtual void Finalize() override;
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_STEAM_ENABLED