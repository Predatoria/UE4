// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "Interfaces/OnlineStoreInterfaceV2.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

EOS_ENABLE_STRICT_WARNINGS

class FOnlineStoreInterfaceV2EAS : public IOnlineStoreV2,
                                   public TSharedFromThis<FOnlineStoreInterfaceV2EAS, ESPMode::ThreadSafe>
{
private:
    EOS_HEcom Ecom;
    TMap<FUniqueOfferId, TSharedPtr<FOnlineStoreOffer>> CachedOffers;

public:
    FOnlineStoreInterfaceV2EAS(EOS_HEcom InEcom)
        : Ecom(InEcom){};
    UE_NONCOPYABLE(FOnlineStoreInterfaceV2EAS);
    virtual ~FOnlineStoreInterfaceV2EAS() override = default;

    virtual void QueryCategories(
        const FUniqueNetId &UserId,
        const FOnQueryOnlineStoreCategoriesComplete &Delegate = FOnQueryOnlineStoreCategoriesComplete()) override;
    virtual void GetCategories(TArray<FOnlineStoreCategory> &OutCategories) const override;
    virtual void QueryOffersByFilter(
        const FUniqueNetId &UserId,
        const FOnlineStoreFilter &Filter,
        const FOnQueryOnlineStoreOffersComplete &Delegate = FOnQueryOnlineStoreOffersComplete()) override;
    virtual void QueryOffersById(
        const FUniqueNetId &UserId,
        const TArray<FUniqueOfferId> &OfferIds,
        const FOnQueryOnlineStoreOffersComplete &Delegate = FOnQueryOnlineStoreOffersComplete()) override;
    virtual void GetOffers(TArray<FOnlineStoreOfferRef> &OutOffers) const override;
    virtual TSharedPtr<FOnlineStoreOffer> GetOffer(const FUniqueOfferId &OfferId) const override;
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION