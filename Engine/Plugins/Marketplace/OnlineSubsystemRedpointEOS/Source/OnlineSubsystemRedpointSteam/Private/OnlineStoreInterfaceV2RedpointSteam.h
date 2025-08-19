// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineStoreInterfaceV2.h"
#include "OnlineError.h"

#if EOS_STEAM_ENABLED

class FOnlineStoreInterfaceV2RedpointSteam
    : public IOnlineStoreV2,
      public TSharedFromThis<FOnlineStoreInterfaceV2RedpointSteam, ESPMode::ThreadSafe>
{
    friend class FOnlineAsyncTaskSteamRequestPrices;

private:
    TMap<FUniqueOfferId, TSharedPtr<FOnlineStoreOffer>> CachedOffers;

    void OnItemDefinitionLoadComplete(const FOnlineError &Error, FOnQueryOnlineStoreOffersComplete Delegate);
    void OnItemPricesRequested(
        const FOnlineError &Error,
        const TMap<FUniqueOfferId, TSharedPtr<FOnlineStoreOffer>> &Offers,
        FOnQueryOnlineStoreOffersComplete Delegate);
    void OnDlcRequested(
        const FOnlineError &Error,
        const TMap<FUniqueOfferId, TSharedPtr<FOnlineStoreOffer>> &DlcOffers,
        TMap<FUniqueOfferId, TSharedPtr<FOnlineStoreOffer>> ItemOffers,
        FOnQueryOnlineStoreOffersComplete Delegate);

public:
    FOnlineStoreInterfaceV2RedpointSteam() = default;
    UE_NONCOPYABLE(FOnlineStoreInterfaceV2RedpointSteam);
    virtual ~FOnlineStoreInterfaceV2RedpointSteam() override = default;

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

#endif