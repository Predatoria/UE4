// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineStoreInterfaceV2.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FOnlineStoreInterfaceV2Synthetic
    : public IOnlineStoreV2,
      public TSharedFromThis<FOnlineStoreInterfaceV2Synthetic, ESPMode::ThreadSafe>
{
private:
    TSharedRef<class FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> IdentityEOS;

public:
    FOnlineStoreInterfaceV2Synthetic(TSharedRef<class FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> InIdentityEOS)
        : IdentityEOS(MoveTemp(InIdentityEOS)){};
    UE_NONCOPYABLE(FOnlineStoreInterfaceV2Synthetic);
    virtual ~FOnlineStoreInterfaceV2Synthetic() override = default;

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