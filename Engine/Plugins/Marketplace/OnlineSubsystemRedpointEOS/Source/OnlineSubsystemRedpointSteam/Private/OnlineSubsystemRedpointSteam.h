// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineAsyncTaskManager.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemImplBase.h"
#include "OnlineSubsystemRedpointSteamConstants.h"

#if EOS_STEAM_ENABLED

#include "OnlineSubsystemSteam.h"

THIRD_PARTY_INCLUDES_START
#include "steam/steam_api.h"
THIRD_PARTY_INCLUDES_END

class FOnlineSubsystemSteamProtectedAccessor : public FOnlineSubsystemSteam
{
public:
    static FOnlineAsyncTaskManager *GetAsyncTaskRunner(FOnlineSubsystemSteam *Steam);
};

class FOnlineSubsystemRedpointSteam : public FOnlineSubsystemImplBase,
                                      public TSharedFromThis<FOnlineSubsystemRedpointSteam, ESPMode::ThreadSafe>
{
private:
    TSharedPtr<class FOnlineAvatarInterfaceRedpointSteam, ESPMode::ThreadSafe> AvatarImpl;
    TSharedPtr<class FOnlineEntitlementsInterfaceRedpointSteam, ESPMode::ThreadSafe> EntitlementsImpl;
    TSharedPtr<class FOnlineStoreInterfaceV2RedpointSteam, ESPMode::ThreadSafe> StoreV2Impl;
    TSharedPtr<class FOnlinePurchaseInterfaceRedpointSteam, ESPMode::ThreadSafe> PurchaseImpl;
    FString WebApiKey;

public:
    UE_NONCOPYABLE(FOnlineSubsystemRedpointSteam);
    FOnlineSubsystemRedpointSteam() = delete;
    FOnlineSubsystemRedpointSteam(FName InInstanceName);
    ~FOnlineSubsystemRedpointSteam();

    virtual bool IsEnabled() const override;

    virtual IOnlineEntitlementsPtr GetEntitlementsInterface() const override;
    virtual IOnlineStoreV2Ptr GetStoreV2Interface() const override;
    virtual IOnlinePurchasePtr GetPurchaseInterface() const override;

    // Our custom interfaces. Note that even those these APIs return a UObject*, we return
    // non-UObjects that are TSharedFromThis.
    virtual class UObject *GetNamedInterface(FName InterfaceName) override;
    virtual void SetNamedInterface(FName InterfaceName, class UObject *NewInterface) override
    {
        checkf(false, TEXT("FOnlineSubsystemEOS::SetNamedInterface is not supported"));
    };

    virtual bool Init() override;
    virtual bool Shutdown() override;

    virtual FString GetAppId() const override;
    virtual FText GetOnlineServiceName() const override;

    FString GetWebApiKey() const;
};

#endif