// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/OnlineStoreInterfaceV2Synthetic.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineIdentityInterfaceEOS.h"

EOS_ENABLE_STRICT_WARNINGS

static TSharedPtr<IOnlineStoreV2, ESPMode::ThreadSafe> GetStoreV2(IOnlineSubsystem *OSS)
{
    return OSS->GetStoreV2Interface();
}

void FOnlineStoreInterfaceV2Synthetic::QueryCategories(
    const FUniqueNetId &UserId,
    const FOnQueryOnlineStoreCategoriesComplete &Delegate)
{
    TSharedPtr<const FUniqueNetId> UserIdNative;
    TSharedPtr<IOnlineStoreV2, ESPMode::ThreadSafe> StoreV2 =
        this->IdentityEOS->GetNativeInterface<IOnlineStoreV2>(UserId, UserIdNative, GetStoreV2);
    if (StoreV2.IsValid())
    {
        StoreV2->QueryCategories(*UserIdNative, Delegate);
        return;
    }

    UE_LOG(
        LogEOS,
        Error,
        TEXT("%s"),
        *OnlineRedpointEOS::Errors::Missing_Feature(
             TEXT("FOnlineStoreInterfaceV2Synthetic::QueryCategories"),
             TEXT("This platform does not support IOnlineStoreV2."))
             .ToLogString());
    Delegate.ExecuteIfBound(
        false,
        OnlineRedpointEOS::Errors::Missing_Feature(
            TEXT("FOnlineStoreInterfaceV2Synthetic::QueryCategories"),
            TEXT("This platform does not support IOnlineStoreV2."))
            .ToLogString());
}

void FOnlineStoreInterfaceV2Synthetic::GetCategories(TArray<FOnlineStoreCategory> &OutCategories) const
{
    TSharedPtr<const FUniqueNetId> UserIdNative;
    TSharedPtr<IOnlineStoreV2, ESPMode::ThreadSafe> StoreV2 =
        this->IdentityEOS->GetNativeInterface<IOnlineStoreV2>(0, UserIdNative, GetStoreV2);
    if (StoreV2.IsValid())
    {
        StoreV2->GetCategories(OutCategories);
        return;
    }

    UE_LOG(
        LogEOS,
        Error,
        TEXT("%s"),
        *OnlineRedpointEOS::Errors::Missing_Feature(
             TEXT("FOnlineStoreInterfaceV2Synthetic::GetCategories"),
             TEXT("This platform does not support IOnlineStoreV2."))
             .ToLogString());
}

void FOnlineStoreInterfaceV2Synthetic::QueryOffersByFilter(
    const FUniqueNetId &UserId,
    const FOnlineStoreFilter &Filter,
    const FOnQueryOnlineStoreOffersComplete &Delegate)
{
    TSharedPtr<const FUniqueNetId> UserIdNative;
    TSharedPtr<IOnlineStoreV2, ESPMode::ThreadSafe> StoreV2 =
        this->IdentityEOS->GetNativeInterface<IOnlineStoreV2>(UserId, UserIdNative, GetStoreV2);
    if (StoreV2.IsValid())
    {
        StoreV2->QueryOffersByFilter(*UserIdNative, Filter, Delegate);
        return;
    }

    UE_LOG(
        LogEOS,
        Error,
        TEXT("%s"),
        *OnlineRedpointEOS::Errors::Missing_Feature(
             TEXT("FOnlineStoreInterfaceV2Synthetic::QueryOffersByFilter"),
             TEXT("This platform does not support IOnlineStoreV2."))
             .ToLogString());
    Delegate.ExecuteIfBound(
        false,
        TArray<FUniqueOfferId>(),
        OnlineRedpointEOS::Errors::Missing_Feature(
            TEXT("FOnlineStoreInterfaceV2Synthetic::QueryOffersByFilter"),
            TEXT("This platform does not support IOnlineStoreV2."))
            .ToLogString());
}

void FOnlineStoreInterfaceV2Synthetic::QueryOffersById(
    const FUniqueNetId &UserId,
    const TArray<FUniqueOfferId> &OfferIds,
    const FOnQueryOnlineStoreOffersComplete &Delegate)
{
    TSharedPtr<const FUniqueNetId> UserIdNative;
    TSharedPtr<IOnlineStoreV2, ESPMode::ThreadSafe> StoreV2 =
        this->IdentityEOS->GetNativeInterface<IOnlineStoreV2>(UserId, UserIdNative, GetStoreV2);
    if (StoreV2.IsValid())
    {
        StoreV2->QueryOffersById(*UserIdNative, OfferIds, Delegate);
        return;
    }

    UE_LOG(
        LogEOS,
        Error,
        TEXT("%s"),
        *OnlineRedpointEOS::Errors::Missing_Feature(
             TEXT("FOnlineStoreInterfaceV2Synthetic::QueryOffersById"),
             TEXT("This platform does not support IOnlineStoreV2."))
             .ToLogString());
    Delegate.ExecuteIfBound(
        false,
        TArray<FUniqueOfferId>(),
        OnlineRedpointEOS::Errors::Missing_Feature(
            TEXT("FOnlineStoreInterfaceV2Synthetic::QueryOffersById"),
            TEXT("This platform does not support IOnlineStoreV2."))
            .ToLogString());
}

void FOnlineStoreInterfaceV2Synthetic::GetOffers(TArray<FOnlineStoreOfferRef> &OutOffers) const
{
    TSharedPtr<const FUniqueNetId> UserIdNative;
    TSharedPtr<IOnlineStoreV2, ESPMode::ThreadSafe> StoreV2 =
        this->IdentityEOS->GetNativeInterface<IOnlineStoreV2>(0, UserIdNative, GetStoreV2);
    if (StoreV2.IsValid())
    {
        StoreV2->GetOffers(OutOffers);
        return;
    }

    UE_LOG(
        LogEOS,
        Error,
        TEXT("%s"),
        *OnlineRedpointEOS::Errors::Missing_Feature(
             TEXT("FOnlineStoreInterfaceV2Synthetic::GetOffers"),
             TEXT("This platform does not support IOnlineStoreV2."))
             .ToLogString());
}

TSharedPtr<FOnlineStoreOffer> FOnlineStoreInterfaceV2Synthetic::GetOffer(const FUniqueOfferId &OfferId) const
{
    TSharedPtr<const FUniqueNetId> UserIdNative;
    TSharedPtr<IOnlineStoreV2, ESPMode::ThreadSafe> StoreV2 =
        this->IdentityEOS->GetNativeInterface<IOnlineStoreV2>(0, UserIdNative, GetStoreV2);
    if (StoreV2.IsValid())
    {
        return StoreV2->GetOffer(OfferId);
    }

    UE_LOG(
        LogEOS,
        Error,
        TEXT("%s"),
        *OnlineRedpointEOS::Errors::Missing_Feature(
             TEXT("FOnlineStoreInterfaceV2Synthetic::GetOffer"),
             TEXT("This platform does not support IOnlineStoreV2."))
             .ToLogString());
    return nullptr;
}

EOS_DISABLE_STRICT_WARNINGS