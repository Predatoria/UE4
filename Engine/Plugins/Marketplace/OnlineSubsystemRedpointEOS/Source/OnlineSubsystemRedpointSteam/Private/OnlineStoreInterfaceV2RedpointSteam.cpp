// Copyright June Rhodes. All Rights Reserved.

#include "OnlineStoreInterfaceV2RedpointSteam.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "OnlineSubsystemRedpointSteam.h"

#include "./Tasks/OnlineAsyncTaskSteamGetAvailableDLC.h"
#include "./Tasks/OnlineAsyncTaskSteamLoadItemDefinitions.h"
#include "./Tasks/OnlineAsyncTaskSteamRequestPrices.h"

#if EOS_STEAM_ENABLED

EOS_ENABLE_STRICT_WARNINGS

void FOnlineStoreInterfaceV2RedpointSteam::OnItemDefinitionLoadComplete(
    const FOnlineError &Error,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FOnQueryOnlineStoreOffersComplete Delegate)
{
    // Check if the item definition load failed.
    if (!Error.WasSuccessful())
    {
        Delegate.ExecuteIfBound(false, TArray<FUniqueOfferId>(), Error.ToLogString());
        return;
    }

    // NOLINTNEXTLINE(unreal-ionlinesubsystem-get)
    IOnlineSubsystem *OSS = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
    if (OSS == nullptr)
    {
        Delegate.ExecuteIfBound(
            false,
            TArray<FUniqueOfferId>(),
            OnlineRedpointEOS::Errors::Missing_Feature(
                TEXT("FOnlineStoreInterfaceV2RedpointSteam::OnItemDefinitionLoadComplete"),
                TEXT("The Steam online subsystem is not available."))
                .ToLogString());
        return;
    }

    // Now request item prices.
    FOnlineAsyncTaskSteamRequestPrices *PricesTask =
        new FOnlineAsyncTaskSteamRequestPrices(FSteamOffersFetched::CreateThreadSafeSP(
            this,
            &FOnlineStoreInterfaceV2RedpointSteam::OnItemPricesRequested,
            Delegate));
    FOnlineSubsystemSteamProtectedAccessor::GetAsyncTaskRunner((FOnlineSubsystemSteam *)OSS)->AddToInQueue(PricesTask);
}

void FOnlineStoreInterfaceV2RedpointSteam::OnItemPricesRequested(
    const FOnlineError &Error,
    const TMap<FUniqueOfferId, TSharedPtr<FOnlineStoreOffer>> &Offers,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FOnQueryOnlineStoreOffersComplete Delegate)
{
    // Check if the item prices load failed.
    if (!Error.WasSuccessful())
    {
        Delegate.ExecuteIfBound(false, TArray<FUniqueOfferId>(), Error.ToLogString());
        return;
    }

    // NOLINTNEXTLINE(unreal-ionlinesubsystem-get)
    IOnlineSubsystem *OSS = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
    if (OSS == nullptr)
    {
        Delegate.ExecuteIfBound(
            false,
            TArray<FUniqueOfferId>(),
            OnlineRedpointEOS::Errors::Missing_Feature(
                TEXT("FOnlineStoreInterfaceV2RedpointSteam::OnItemDefinitionLoadComplete"),
                TEXT("The Steam online subsystem is not available."))
                .ToLogString());
        return;
    }

    // Now request DLC items.
    FOnlineAsyncTaskSteamGetAvailableDLC *DlcTask =
        new FOnlineAsyncTaskSteamGetAvailableDLC(FSteamOffersFetched::CreateThreadSafeSP(
            this,
            &FOnlineStoreInterfaceV2RedpointSteam::OnDlcRequested,
            Offers,
            Delegate));
    FOnlineSubsystemSteamProtectedAccessor::GetAsyncTaskRunner((FOnlineSubsystemSteam *)OSS)->AddToInQueue(DlcTask);
}

void FOnlineStoreInterfaceV2RedpointSteam::OnDlcRequested(
    const FOnlineError &Error,
    const TMap<FUniqueOfferId, TSharedPtr<FOnlineStoreOffer>> &DlcOffers,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TMap<FUniqueOfferId, TSharedPtr<FOnlineStoreOffer>> ItemOffers,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FOnQueryOnlineStoreOffersComplete Delegate)
{
    // Check if the DLC load failed.
    if (!Error.WasSuccessful())
    {
        Delegate.ExecuteIfBound(false, TArray<FUniqueOfferId>(), Error.ToLogString());
        return;
    }

    // Add the DLC items to the ItemOffers copy, and then set it as the current cached offers.
    for (const auto &DlcKV : DlcOffers)
    {
        ItemOffers.Add(DlcKV.Key, DlcKV.Value);
    }
    TArray<FUniqueOfferId> OfferIds;
    for (const auto &OfferKV : ItemOffers)
    {
        OfferIds.Add(OfferKV.Key);
    }
    this->CachedOffers = ItemOffers;

    Delegate.ExecuteIfBound(true, OfferIds, TEXT(""));
}

void FOnlineStoreInterfaceV2RedpointSteam::QueryCategories(
    const FUniqueNetId &UserId,
    const FOnQueryOnlineStoreCategoriesComplete &Delegate)
{
    // Steam e-commerce doesn't support categories, so we just return true. There
    // are no categories for offers.
    Delegate.ExecuteIfBound(true, TEXT(""));
}

void FOnlineStoreInterfaceV2RedpointSteam::GetCategories(TArray<FOnlineStoreCategory> &OutCategories) const
{
    // Steam e-commerce doesn't support categories, so we just return true. There
    // are no categories for offers.
    OutCategories.Reset();
}

void FOnlineStoreInterfaceV2RedpointSteam::QueryOffersByFilter(
    const FUniqueNetId &UserId,
    const FOnlineStoreFilter &Filter,
    const FOnQueryOnlineStoreOffersComplete &Delegate)
{
    // NOLINTNEXTLINE(unreal-ionlinesubsystem-get)
    IOnlineSubsystem *OSS = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
    if (OSS == nullptr)
    {
        Delegate.ExecuteIfBound(
            false,
            TArray<FUniqueOfferId>(),
            OnlineRedpointEOS::Errors::Missing_Feature(
                TEXT("FOnlineStoreInterfaceV2RedpointSteam::QueryOffersByFilter"),
                TEXT("The Steam online subsystem is not available."))
                .ToLogString());
        return;
    }

    // Load item definitions first.
    FOnlineAsyncTaskSteamLoadItemDefinitions *LoadTask =
        new FOnlineAsyncTaskSteamLoadItemDefinitions(FSteamItemLoadComplete::CreateThreadSafeSP(
            this,
            &FOnlineStoreInterfaceV2RedpointSteam::OnItemDefinitionLoadComplete,
            Delegate));
    FOnlineSubsystemSteamProtectedAccessor::GetAsyncTaskRunner((FOnlineSubsystemSteam *)OSS)->AddToInQueue(LoadTask);
}

void FOnlineStoreInterfaceV2RedpointSteam::QueryOffersById(
    const FUniqueNetId &UserId,
    const TArray<FUniqueOfferId> &OfferIds,
    const FOnQueryOnlineStoreOffersComplete &Delegate)
{
    Delegate.ExecuteIfBound(
        false,
        TArray<FUniqueOfferId>(),
        OnlineRedpointEOS::Errors::NotImplemented(
            TEXT("FOnlineStoreInterfaceV2RedpointSteam::QueryOffersById"),
            TEXT("Steam e-commerce does not support querying offers by ID; call QueryOffersByFilter instead "
                 "(which will fetch all offers)."))
            .ToLogString());
}

void FOnlineStoreInterfaceV2RedpointSteam::GetOffers(TArray<FOnlineStoreOfferRef> &OutOffers) const
{
    for (const auto &KV : this->CachedOffers)
    {
        OutOffers.Add(KV.Value.ToSharedRef());
    }
}

TSharedPtr<FOnlineStoreOffer> FOnlineStoreInterfaceV2RedpointSteam::GetOffer(const FUniqueOfferId &OfferId) const
{
    if (this->CachedOffers.Contains(OfferId))
    {
        return this->CachedOffers[OfferId];
    }

    return nullptr;
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_STEAM_ENABLED