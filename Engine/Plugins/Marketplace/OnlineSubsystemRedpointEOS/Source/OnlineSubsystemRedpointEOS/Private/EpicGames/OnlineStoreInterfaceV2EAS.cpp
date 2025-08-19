// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineStoreInterfaceV2EAS.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSErrorConv.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/UniqueNetIdEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

class FOnlineStoreOfferEAS : public FOnlineStoreOffer, public TSharedFromThis<FOnlineStoreOfferEAS>
{
public:
    FOnlineStoreOfferEAS() = default;
    UE_NONCOPYABLE(FOnlineStoreOfferEAS);
    virtual ~FOnlineStoreOfferEAS() = default;

    bool bAvailableForPurchase;
    virtual bool IsPurchaseable() const override
    {
        return this->bAvailableForPurchase;
    }
};

void FOnlineStoreInterfaceV2EAS::QueryCategories(
    const FUniqueNetId &UserId,
    const FOnQueryOnlineStoreCategoriesComplete &Delegate)
{
    // Epic Games e-commerce doesn't support categories, so we just return true. There
    // are no categories for offers.
    Delegate.ExecuteIfBound(true, TEXT(""));
}

void FOnlineStoreInterfaceV2EAS::GetCategories(TArray<FOnlineStoreCategory> &OutCategories) const
{
    // Epic Games e-commerce doesn't support categories, so we just return true. There
    // are no categories for offers.
    OutCategories.Reset();
}

void FOnlineStoreInterfaceV2EAS::QueryOffersByFilter(
    const FUniqueNetId &UserId,
    const FOnlineStoreFilter &Filter,
    const FOnQueryOnlineStoreOffersComplete &Delegate)
{
    EOS_EpicAccountId AccountId = StaticCastSharedRef<const FUniqueNetIdEAS>(UserId.AsShared())->GetEpicAccountId();

    EOS_Ecom_QueryOffersOptions Opts = {};
    Opts.ApiVersion = EOS_ECOM_QUERYOFFERS_API_LATEST;
    Opts.LocalUserId = AccountId;
    Opts.OverrideCatalogNamespace = nullptr;

    EOSRunOperation<EOS_HEcom, EOS_Ecom_QueryOffersOptions, EOS_Ecom_QueryOffersCallbackInfo>(
        this->Ecom,
        &Opts,
        &EOS_Ecom_QueryOffers,
        [WeakThis = GetWeakThis(this), AccountId, Delegate](const EOS_Ecom_QueryOffersCallbackInfo *Info) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Info->ResultCode != EOS_EResult::EOS_Success)
                {
                    Delegate.ExecuteIfBound(
                        false,
                        TArray<FUniqueOfferId>(),
                        ConvertError(
                            TEXT("FOnlineStoreInterfaceV2EAS::QueryOffersByFilter"),
                            TEXT("Failed to query store categories."),
                            Info->ResultCode)
                            .ToLogString());
                    return;
                }

                EOS_Ecom_GetOfferCountOptions CountOpts = {};
                CountOpts.ApiVersion = EOS_ECOM_GETOFFERCOUNT_API_LATEST;
                CountOpts.LocalUserId = AccountId;
                uint32_t OfferCount = EOS_Ecom_GetOfferCount(This->Ecom, &CountOpts);
                for (uint32_t i = 0; i < OfferCount; i++)
                {
                    EOS_Ecom_CopyOfferByIndexOptions CopyOpts = {};
                    CopyOpts.ApiVersion = EOS_ECOM_COPYOFFERBYINDEX_API_LATEST;
                    CopyOpts.LocalUserId = AccountId;
                    CopyOpts.OfferIndex = i;
                    EOS_Ecom_CatalogOffer *OfferData = nullptr;
                    EOS_EResult CopyResult = EOS_Ecom_CopyOfferByIndex(This->Ecom, &CopyOpts, &OfferData);
                    if (CopyResult != EOS_EResult::EOS_Success)
                    {
                        UE_LOG(
                            LogEOS,
                            Error,
                            TEXT("%s"),
                            *ConvertError(
                                 TEXT("FOnlineStoreInterfaceV2EAS::QueryOffersByFilter"),
                                 FString::Printf(TEXT("Failed to copy offer at index %u."), i),
                                 CopyResult)
                                 .ToLogString());
                        continue;
                    }

                    TSharedRef<FOnlineStoreOfferEAS> Offer = MakeShared<FOnlineStoreOfferEAS>();
                    Offer->OfferId = EOSString_Ecom_CatalogOfferId::FromAnsiString(OfferData->Id);

                    Offer->Title = FText::FromString(EOSString_Utf8Unlimited::FromUtf8String(OfferData->TitleText));
                    Offer->Description =
                        FText::FromString(EOSString_Utf8Unlimited::FromUtf8String(OfferData->DescriptionText));
                    Offer->LongDescription =
                        FText::FromString(EOSString_Utf8Unlimited::FromUtf8String(OfferData->LongDescriptionText));

                    if (OfferData->PriceResult == EOS_EResult::EOS_Success)
                    {
                        Offer->RegularPriceText = FText::GetEmpty();
#if !defined(UE_4_27_OR_LATER)
                        Offer->RegularPrice = (int32)OfferData->OriginalPrice64;
#else
                        Offer->RegularPrice = OfferData->OriginalPrice64;
#endif

                        Offer->PriceText = FText::GetEmpty();
#if !defined(UE_4_27_OR_LATER)
                        Offer->NumericPrice = (int32)OfferData->CurrentPrice64;
#else
                        Offer->NumericPrice = OfferData->CurrentPrice64;
#endif

                        Offer->bAvailableForPurchase = OfferData->bAvailableForPurchase == EOS_TRUE;
                    }
                    else
                    {
                        // Could not retrieve pricing data.
                        Offer->bAvailableForPurchase = false;
                    }

                    Offer->CurrencyCode = EOSString_Utf8Unlimited::FromUtf8String(OfferData->CurrencyCode);

                    // Epic Games doesn't surface the release date of e-commerce items.
                    Offer->ReleaseDate = FDateTime::MinValue();
                    Offer->ExpirationDate = OfferData->ExpirationTimestamp == -1
                                                ? FDateTime::MaxValue()
                                                : FDateTime::FromUnixTimestamp(OfferData->ExpirationTimestamp);
                    Offer->DiscountType = Offer->RegularPrice == Offer->NumericPrice
                                              ? EOnlineStoreOfferDiscountType::NotOnSale
                                              : EOnlineStoreOfferDiscountType::Percentage;

                    Offer->DynamicFields.Reset();
                    Offer->DynamicFields.Add(
                        TEXT("CatalogNamespace"),
                        EOSString_Ecom_CatalogNamespace::FromAnsiString(OfferData->CatalogNamespace));
                    Offer->DynamicFields.Add(
                        TEXT("DiscountPercentage"),
                        FString::Printf(TEXT("%u"), OfferData->DiscountPercentage));
#if EOS_VERSION_AT_MOST(1, 15, 1)
                    Offer->DynamicFields.Add(
                        TEXT("PurchasedCount"),
                        FString::Printf(TEXT("%u"), OfferData->PurchasedCount));
#endif
                    Offer->DynamicFields.Add(
                        TEXT("PurchaseLimit"),
                        FString::Printf(TEXT("%d"), OfferData->PurchaseLimit));
                    Offer->DynamicFields.Add(
                        TEXT("DecimalPoint"),
                        FString::Printf(TEXT("%u"), OfferData->DecimalPoint));

                    EOS_Ecom_CatalogOffer_Release(OfferData);

                    This->CachedOffers.Add(Offer->OfferId, Offer);
                }

                TArray<FUniqueOfferId> Ids;
                for (const auto &KV : This->CachedOffers)
                {
                    Ids.Add(KV.Key);
                }
                Delegate.ExecuteIfBound(true, Ids, TEXT(""));
            }
        });
}

void FOnlineStoreInterfaceV2EAS::QueryOffersById(
    const FUniqueNetId &UserId,
    const TArray<FUniqueOfferId> &OfferIds,
    const FOnQueryOnlineStoreOffersComplete &Delegate)
{
    Delegate.ExecuteIfBound(
        false,
        TArray<FUniqueOfferId>(),
        OnlineRedpointEOS::Errors::NotImplemented(
            TEXT("FOnlineStoreInterfaceV2EAS::QueryOffersById"),
            TEXT("Epic Games e-commerce does not support querying offers by ID; call QueryOffersByFilter instead "
                 "(which will fetch all offers)."))
            .ToLogString());
}

void FOnlineStoreInterfaceV2EAS::GetOffers(TArray<FOnlineStoreOfferRef> &OutOffers) const
{
    for (const auto &KV : this->CachedOffers)
    {
        OutOffers.Add(KV.Value.ToSharedRef());
    }
}

TSharedPtr<FOnlineStoreOffer> FOnlineStoreInterfaceV2EAS::GetOffer(const FUniqueOfferId &OfferId) const
{
    if (this->CachedOffers.Contains(OfferId))
    {
        return this->CachedOffers[OfferId];
    }

    return nullptr;
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION