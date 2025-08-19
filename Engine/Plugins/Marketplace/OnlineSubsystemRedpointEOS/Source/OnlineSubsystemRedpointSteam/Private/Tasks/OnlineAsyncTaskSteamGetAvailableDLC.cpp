// Copyright June Rhodes. All Rights Reserved.

#include "./OnlineAsyncTaskSteamGetAvailableDLC.h"

#include "../LogRedpointSteam.h"
#include "../OnlineSubsystemRedpointSteam.h"
#include "../SteamConstants.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"

#if EOS_STEAM_ENABLED

EOS_ENABLE_STRICT_WARNINGS

void FOnlineAsyncTaskSteamGetAvailableDLC::Tick()
{
    // This can't fail, and isn't really an async task since
    // we do all our work on the game thread in Finalize...
    this->bIsComplete = true;
    this->bWasSuccessful = true;
}

void FOnlineAsyncTaskSteamGetAvailableDLC::Finalize()
{
    // Convert DLC apps into offers.
    TMap<FUniqueOfferId, TSharedPtr<FOnlineStoreOffer>> Offers;
    int DlcCount = SteamApps()->GetDLCCount();
    for (int i = 0; i < DlcCount; i++)
    {
        AppId_t DlcId;
        bool bAvailable;
        char NameBuffer[2049];
        FMemory::Memzero(NameBuffer);
        if (SteamApps()->BGetDLCDataByIndex(i, &DlcId, &bAvailable, NameBuffer, 2048))
        {
            // We only add available DLC to the list. Only limited information is
            // available for DLC items.
#if defined(UE_BUILD_SHIPPING) && UE_BUILD_SHIPPING
            if (bAvailable)
#endif // #if defined(UE_BUILD_SHIPPING) && UE_BUILD_SHIPPING
            {
                TSharedRef<FOnlineStoreOffer> Offer = MakeShared<FOnlineStoreOffer>();
                Offer->OfferId = FString::Printf(TEXT("%s:%u"), STEAM_TYPE_DLC, DlcId);

                Offer->Title = FText::FromString(UTF8_TO_TCHAR(NameBuffer));
                Offer->Description = FText::GetEmpty();
                Offer->LongDescription = FText::GetEmpty();

                Offer->RegularPriceText = NSLOCTEXT("OnlineSubsystemRedpointSteam", "DLCNoPriceAvailable", "N/A");
                Offer->RegularPrice = -1;

                Offer->PriceText = NSLOCTEXT("OnlineSubsystemRedpointSteam", "DLCNoPriceAvailable", "N/A");
                Offer->NumericPrice = -1;

                Offer->CurrencyCode = TEXT("");

                Offer->ReleaseDate = FDateTime::MinValue();
                Offer->ExpirationDate = FDateTime::MaxValue();
                Offer->DiscountType = EOnlineStoreOfferDiscountType::NotOnSale;

                Offer->DynamicFields = TMap<FString, FString>();

                Offers.Add(Offer->OfferId, Offer);
            }
        }
    }

    this->Delegate.ExecuteIfBound(OnlineRedpointEOS::Errors::Success(), Offers);
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_STEAM_ENABLED