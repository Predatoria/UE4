// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "Interfaces/OnlineEntitlementsInterface.h"

EOS_ENABLE_STRICT_WARNINGS

class FOnlineEntitlementsInterfaceEAS : public IOnlineEntitlements,
                                        public TSharedFromThis<FOnlineEntitlementsInterfaceEAS, ESPMode::ThreadSafe>
{
public:
    FOnlineEntitlementsInterfaceEAS() = default;
    UE_NONCOPYABLE(FOnlineEntitlementsInterfaceEAS);
    virtual ~FOnlineEntitlementsInterfaceEAS() override = default;

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

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION