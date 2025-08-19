// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineEntitlementsInterface.h"
#include "SteamInventory.h"
#include "OnlineError.h"

#if EOS_STEAM_ENABLED

class FOnlineEntitlementsInterfaceRedpointSteam
    : public IOnlineEntitlements,
      public TSharedFromThis<FOnlineEntitlementsInterfaceRedpointSteam, ESPMode::ThreadSafe>
{
    friend class FInventorySteamCallbacks;

private:
    TMap<FUniqueEntitlementId, TSharedPtr<FOnlineEntitlement>> CachedItems;
    TMap<FUniqueEntitlementId, TSharedPtr<FOnlineEntitlement>> CachedDlcs;

    void OnItemDefinitionsLoaded(const FOnlineError &Error, TSharedRef<const FUniqueNetId> UserId, FString Namespace);
    void OnGetAllItemsLoaded(
        const FOnlineError &Error,
        const TMap<FUniqueEntitlementId, TSharedPtr<FOnlineEntitlement>> &Items,
        TSharedRef<const FUniqueNetId> UserId,
        FString Namespace);
    void OnDlcRequested(
        const FOnlineError &Error,
        const TMap<FUniqueEntitlementId, TSharedPtr<FOnlineEntitlement>> &DlcEntitlements,
        TSharedRef<const FUniqueNetId> UserId,
        FString Namespace);

public:
    FOnlineEntitlementsInterfaceRedpointSteam();
    UE_NONCOPYABLE(FOnlineEntitlementsInterfaceRedpointSteam);
    virtual ~FOnlineEntitlementsInterfaceRedpointSteam() override = default;

    virtual TSharedPtr<FOnlineEntitlement> GetEntitlement(
        const FUniqueNetId &UserId,
        const FUniqueEntitlementId &EntitlementId) override;
    virtual TSharedPtr<FOnlineEntitlement> GetItemEntitlement(const FUniqueNetId &UserId, const FString &ItemId)
        override;
    virtual void GetAllEntitlements(
        const FUniqueNetId &UserId,
        const FString &Namespace,
        TArray<TSharedRef<FOnlineEntitlement>> &OutUserEntitlements) override;
    virtual bool QueryEntitlements(const FUniqueNetId &UserId, const FString &Namespace, const FPagedQuery &Page)
        override;
};

#endif