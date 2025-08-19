// Copyright June Rhodes. All Rights Reserved.

#include "./OnlineAsyncTaskSteamPurchase.h"

#include "../LogRedpointSteam.h"
#include "../OnlineSubsystemRedpointSteam.h"
#include "../SteamConstants.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "SteamInventory.h"

#if EOS_STEAM_ENABLED

EOS_ENABLE_STRICT_WARNINGS

void FOnlineAsyncTaskSteamPurchase::Tick()
{
    ISteamUtils *SteamUtilsPtr = SteamUtils();
    check(SteamUtilsPtr);

    if (!this->bInit)
    {
        TArray<FPurchaseCheckoutRequest::FPurchaseOfferEntry> ItemOffers;
        for (const auto &Entry : this->CheckoutRequest.PurchaseOffers)
        {
            if (Entry.OfferId.StartsWith(FString::Printf(TEXT("%s:"), STEAM_TYPE_ITEM_DEFINITION)))
            {
                if (Entry.Quantity >= 1)
                {
                    ItemOffers.Add(Entry);
                }
            }
        }

        SteamItemDef_t *Items = (SteamItemDef_t *)FMemory::MallocZeroed(sizeof(SteamItemDef_t) * ItemOffers.Num());
        uint32 *Quantities = (uint32 *)FMemory::MallocZeroed(sizeof(uint32) * ItemOffers.Num());

        for (int i = 0; i < ItemOffers.Num(); i++)
        {
            Items[i] = FCString::Atoi(*ItemOffers[i].OfferId.Mid(FString(STEAM_TYPE_ITEM_DEFINITION).Len() + 1));
            Quantities[i] = FMath::Max(1, ItemOffers[i].Quantity);
        }

        this->CallbackHandle = SteamInventory()->StartPurchase(Items, Quantities, ItemOffers.Num());
        this->bInit = true;

        FMemory::Free(Items);
        FMemory::Free(Quantities);
    }

    if (this->CallbackHandle != k_uAPICallInvalid)
    {
        bool bFailedCall = false;
        this->bIsComplete = SteamUtilsPtr->IsAPICallCompleted(this->CallbackHandle, &bFailedCall) ? true : false;
        if (this->bIsComplete)
        {
            bool bFailedResult;
            // Retrieve the callback data from the request
            bool bSuccessCallResult = SteamUtilsPtr->GetAPICallResult(
                this->CallbackHandle,
                &this->CallbackResults,
                sizeof(this->CallbackResults),
                this->CallbackResults.k_iCallback,
                &bFailedResult);
            this->bWasSuccessful = (bSuccessCallResult ? true : false) && (!bFailedCall ? true : false) &&
                                   (!bFailedResult ? true : false) &&
                                   ((this->CallbackResults.m_result == k_EResultOK) ? true : false);
            if (!this->bWasSuccessful)
            {
                this->FailureContext = FString::Printf(
                    TEXT("StartPurchase failed (%d, %d)"),
                    SteamUtils()->GetAPICallFailureReason(this->CallbackHandle),
                    this->CallbackResults.m_result);
            }
            else
            {
                this->OrderId = this->CallbackResults.m_ulOrderID;
                this->TransactionId = this->CallbackResults.m_ulTransID;
            }
        }
    }
    else
    {
        // Invalid API call
        this->FailureContext = TEXT("StartPurchase API call invalid");
        this->bIsComplete = true;
        this->bWasSuccessful = false;
    }
}

void FOnlineAsyncTaskSteamPurchase::Finalize()
{
    if (!this->bWasSuccessful)
    {
        this->Delegate.ExecuteIfBound(
            OnlineRedpointEOS::Errors::UnexpectedError(
                TEXT("FOnlineAsyncTaskSteamRequestPrices::Finalize"),
                FString::Printf(
                    TEXT("Failed to retrieve item definitions from Steam for e-commerce (%s)."),
                    *this->FailureContext)),
            MakeShared<FPurchaseReceipt>());
    }
    else
    {
        auto Receipt = MakeShared<FPurchaseReceipt>();
        Receipt->TransactionId = FString::Printf(TEXT("%llu"), this->TransactionId);
        // Steam doesn't actually tell us when
        // this particular purchase is complete!
        Receipt->TransactionState = EPurchaseTransactionState::Processing;
        for (const auto &Entry : this->CheckoutRequest.PurchaseOffers)
        {
            if (Entry.Quantity >= 1)
            {
                Receipt->ReceiptOffers.Add(
                    FPurchaseReceipt::FReceiptOfferEntry(Entry.OfferNamespace, Entry.OfferId, Entry.Quantity));
            }
        }
        this->Delegate.ExecuteIfBound(OnlineRedpointEOS::Errors::Success(), Receipt);
    }
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_STEAM_ENABLED