// Copyright June Rhodes. All Rights Reserved.

#include "./OnlineAsyncTaskSteamRequestPrices.h"

#include "../LogRedpointSteam.h"
#include "../OnlineSubsystemRedpointSteam.h"
#include "../SteamConstants.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "SteamInventory.h"

#if EOS_STEAM_ENABLED

EOS_ENABLE_STRICT_WARNINGS

void FOnlineAsyncTaskSteamRequestPrices::Tick()
{
    ISteamUtils *SteamUtilsPtr = SteamUtils();
    check(SteamUtilsPtr);

    if (!this->bInit)
    {
        this->PriceCallbackHandle = SteamInventory()->RequestPrices();
        this->bInit = true;
    }

    if (this->PriceCallbackHandle != k_uAPICallInvalid)
    {
        bool bFailedCall = false;
        this->bIsComplete = SteamUtilsPtr->IsAPICallCompleted(this->PriceCallbackHandle, &bFailedCall) ? true : false;
        if (this->bIsComplete)
        {
            bool bFailedResult;
            // Retrieve the callback data from the request
            bool bSuccessCallResult = SteamUtilsPtr->GetAPICallResult(
                this->PriceCallbackHandle,
                &this->PriceCallbackResults,
                sizeof(this->PriceCallbackResults),
                this->PriceCallbackResults.k_iCallback,
                &bFailedResult);
            this->bWasSuccessful = (bSuccessCallResult ? true : false) && (!bFailedCall ? true : false) &&
                                   (!bFailedResult ? true : false) &&
                                   ((this->PriceCallbackResults.m_result == k_EResultOK) ? true : false);
            if (!this->bWasSuccessful)
            {
                this->FailureContext = FString::Printf(
                    TEXT("RequestPrices failed (%d, %d)"),
                    SteamUtils()->GetAPICallFailureReason(this->PriceCallbackHandle),
                    this->PriceCallbackResults.m_result);
            }
            else
            {
                this->CurrencyCode = ANSI_TO_TCHAR(this->PriceCallbackResults.m_rgchCurrency);
            }
        }
    }
    else
    {
        // Invalid API call
        this->FailureContext = TEXT("RequestPrices API call invalid");
        this->bIsComplete = true;
        this->bWasSuccessful = false;
    }
}

void FOnlineAsyncTaskSteamRequestPrices::Finalize()
{
    if (!this->bWasSuccessful)
    {
        this->Delegate.ExecuteIfBound(
            OnlineRedpointEOS::Errors::UnexpectedError(
                TEXT("FOnlineAsyncTaskSteamRequestPrices::Finalize"),
                FString::Printf(
                    TEXT("Failed to retrieve item definitions from Steam for e-commerce (%s)."),
                    *this->FailureContext)),
            TMap<FUniqueOfferId, TSharedPtr<FOnlineStoreOffer>>());
        return;
    }

    uint32 NumItems = SteamInventory()->GetNumItemsWithPrices();
    SteamItemDef_t *Items = (SteamItemDef_t *)FMemory::Malloc(sizeof(SteamItemDef_t) * NumItems);
    uint64 *CurrentPrices = (uint64 *)FMemory::Malloc(sizeof(uint64) * NumItems);
    uint64 *BasePrices = (uint64 *)FMemory::Malloc(sizeof(uint64) * NumItems);
    if (!SteamInventory()->GetItemsWithPrices(Items, CurrentPrices, BasePrices, NumItems))
    {
        FMemory::Free(Items);
        FMemory::Free(CurrentPrices);
        FMemory::Free(BasePrices);
        this->Delegate.ExecuteIfBound(
            OnlineRedpointEOS::Errors::UnexpectedError(
                TEXT("FOnlineAsyncTaskSteamRequestPrices::Finalize"),
                TEXT("GetItemsWithPrices call failed.")),
            TMap<FUniqueOfferId, TSharedPtr<FOnlineStoreOffer>>());
        return;
    }

    TArray<FSteamItemDefinition> ItemDefinitions =
        OnlineRedpointSteam::ParseItemDefinitions(Items, CurrentPrices, BasePrices, NumItems);

    FMemory::Free(Items);
    FMemory::Free(CurrentPrices);
    FMemory::Free(BasePrices);

    // Convert the loaded item definitions into offers.
    TMap<FUniqueOfferId, TSharedPtr<FOnlineStoreOffer>> Offers;
    for (const auto &ItemDef : ItemDefinitions)
    {
        TSharedRef<FOnlineStoreOffer> Offer = MakeShared<FOnlineStoreOffer>();
        Offer->OfferId = FString::Printf(TEXT("%s:%d"), STEAM_TYPE_ITEM_DEFINITION, ItemDef.SteamItemID);

        Offer->Title = ItemDef.Properties.Contains(TEXT("name")) ? FText::FromString(ItemDef.Properties[TEXT("name")])
                                                                 : FText::GetEmpty();
        Offer->Description = ItemDef.Properties.Contains(TEXT("description"))
                                 ? FText::FromString(ItemDef.Properties[TEXT("description")])
                                 : FText::GetEmpty();
        Offer->LongDescription = ItemDef.Properties.Contains(TEXT("long_description"))
                                     ? FText::FromString(ItemDef.Properties[TEXT("long_description")])
                                     : FText::GetEmpty();

        Offer->RegularPriceText = FText::GetEmpty();
#if defined(UE_4_27_OR_LATER)
        Offer->RegularPrice = ItemDef.BasePrice;
#else
        Offer->RegularPrice = (int32)ItemDef.BasePrice;
#endif

        Offer->PriceText = FText::GetEmpty();
#if defined(UE_4_27_OR_LATER)
        Offer->NumericPrice = ItemDef.CurrentPrice;
#else
        Offer->NumericPrice = (int32)ItemDef.CurrentPrice;
#endif

        Offer->CurrencyCode = this->CurrencyCode;

        if (ItemDef.Properties.Contains(TEXT("date_created")))
        {
            // Parse the date value which comes in the form "20221008T103044Z".
            FString DateString = ItemDef.Properties[TEXT("date_created")];
            int32 Year = FCString::Atoi(*DateString.Mid(0, 4));
            int32 Month = FCString::Atoi(*DateString.Mid(4, 2));
            int32 Day = FCString::Atoi(*DateString.Mid(6, 2));
            int32 Hour = FCString::Atoi(*DateString.Mid(9, 2));
            int32 Minute = FCString::Atoi(*DateString.Mid(11, 2));
            int32 Second = FCString::Atoi(*DateString.Mid(13, 2));
            Offer->ReleaseDate = FDateTime(Year, Month, Day, Hour, Minute, Second);
        }
        else
        {
            Offer->ReleaseDate = FDateTime::MinValue();
        }
        Offer->ExpirationDate = FDateTime::MaxValue();
        Offer->DiscountType = Offer->RegularPrice != Offer->NumericPrice ? EOnlineStoreOfferDiscountType::Percentage
                                                                         : EOnlineStoreOfferDiscountType::NotOnSale;

        Offer->DynamicFields = ItemDef.Properties;

        Offers.Add(Offer->OfferId, Offer);
    }

    this->Delegate.ExecuteIfBound(OnlineRedpointEOS::Errors::Success(), Offers);
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_STEAM_ENABLED