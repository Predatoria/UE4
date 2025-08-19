// Copyright June Rhodes. All Rights Reserved.

#include "OnlinePurchaseInterfaceRedpointSteam.h"

#include "./Tasks/OnlineAsyncTaskSteamPurchase.h"
#include "LogRedpointSteam.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "OnlineSubsystemRedpointSteam.h"
#include "SteamConstants.h"

#if EOS_STEAM_ENABLED

EOS_ENABLE_STRICT_WARNINGS

bool FOnlinePurchaseInterfaceRedpointSteam::IsAllowedToPurchase(const FUniqueNetId &UserId)
{
    return true;
}

void FOnlinePurchaseInterfaceRedpointSteam::Checkout(
    const FUniqueNetId &UserId,
    const FPurchaseCheckoutRequest &CheckoutRequest,
    const FOnPurchaseCheckoutComplete &Delegate)
{
    TArray<FPurchaseCheckoutRequest::FPurchaseOfferEntry> FilteredPurchaseOffers;
    for (const auto &Offer : CheckoutRequest.PurchaseOffers)
    {
        if (Offer.Quantity >= 1)
        {
            FilteredPurchaseOffers.Add(Offer);
        }
    }
    if (FilteredPurchaseOffers.Num() == 0)
    {
        Delegate.ExecuteIfBound(
            OnlineRedpointEOS::Errors::InvalidRequest(
                TEXT("FOnlinePurchaseInterfaceRedpointSteam::Checkout"),
                TEXT("You must submit at least one offer for purchasing.")),
            MakeShared<FPurchaseReceipt>());
        return;
    }
    bool bHasItem = false;
    bool bHasDlc = false;
    bool bHasInvalid = false;
    for (const auto &Entry : FilteredPurchaseOffers)
    {
        if (Entry.OfferId.StartsWith(FString::Printf(TEXT("%s:"), STEAM_TYPE_ITEM_DEFINITION)))
        {
            bHasItem = true;
        }
        else if (Entry.OfferId.StartsWith(FString::Printf(TEXT("%s:"), STEAM_TYPE_DLC)))
        {
            bHasDlc = true;
        }
        else
        {
            bHasInvalid = true;
        }
    }
    if (bHasInvalid)
    {
        Delegate.ExecuteIfBound(
            OnlineRedpointEOS::Errors::InvalidRequest(
                TEXT("FOnlinePurchaseInterfaceRedpointSteam::Checkout"),
                TEXT(
                    "One or more offer IDs in the checkout request are invalid. Offer IDs should start with 'itemdef:' "
                    "or 'dlc:', and you can not mix types.")),
            MakeShared<FPurchaseReceipt>());
        return;
    }
    if (bHasDlc && bHasItem)
    {
        Delegate.ExecuteIfBound(
            OnlineRedpointEOS::Errors::InvalidRequest(
                TEXT("FOnlinePurchaseInterfaceRedpointSteam::Checkout"),
                TEXT("You have a mix of item and DLC offers in the checkout request, which is not permitted.")),
            MakeShared<FPurchaseReceipt>());
        return;
    }
    if (bHasDlc && FilteredPurchaseOffers.Num() != 1)
    {
        Delegate.ExecuteIfBound(
            OnlineRedpointEOS::Errors::InvalidRequest(
                TEXT("FOnlinePurchaseInterfaceRedpointSteam::Checkout"),
                TEXT("You can only purchase one DLC at a time.")),
            MakeShared<FPurchaseReceipt>());
        return;
    }

    if (bHasDlc)
    {
        // Just open the DLC purchase page in the Steam Overlay.
        FString DlcId = FilteredPurchaseOffers[0].OfferId.Mid(FString(STEAM_TYPE_DLC).Len() + 1);
        SteamFriends()->ActivateGameOverlayToStore(FCString::Atoi(*DlcId), k_EOverlayToStoreFlag_AddToCartAndShow);
        auto Receipt = MakeShared<FPurchaseReceipt>();
        Receipt->TransactionId = TEXT("");
        Receipt->TransactionState = EPurchaseTransactionState::Deferred;
        for (const auto &Entry : FilteredPurchaseOffers)
        {
            Receipt->ReceiptOffers.Add(
                FPurchaseReceipt::FReceiptOfferEntry(Entry.OfferNamespace, Entry.OfferId, Entry.Quantity));
        }
        Delegate.ExecuteIfBound(OnlineRedpointEOS::Errors::Success(), Receipt);
        return;
    }
    else
    {
        // Start the purchase of the items.
        // NOLINTNEXTLINE(unreal-ionlinesubsystem-get)
        IOnlineSubsystem *OSS = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
        if (OSS == nullptr)
        {
            Delegate.ExecuteIfBound(
                OnlineRedpointEOS::Errors::InvalidRequest(
                    TEXT("FOnlinePurchaseInterfaceRedpointSteam::Checkout"),
                    TEXT("The Steam online subsystem is not available.")),
                MakeShared<FPurchaseReceipt>());
            return;
        }

        FOnlineAsyncTaskSteamPurchase *NewTask =
            new FOnlineAsyncTaskSteamPurchase(CheckoutRequest, Delegate, this->AsShared());
        FOnlineSubsystemSteamProtectedAccessor::GetAsyncTaskRunner((FOnlineSubsystemSteam *)OSS)->AddToInQueue(NewTask);
    }
}

#if defined(UE_5_1_OR_LATER)
void FOnlinePurchaseInterfaceRedpointSteam::Checkout(
    const FUniqueNetId &UserId,
    const FPurchaseCheckoutRequest &CheckoutRequest,
    const FOnPurchaseReceiptlessCheckoutComplete &Delegate)
{
    this->Checkout(
        UserId,
        CheckoutRequest,
        FOnPurchaseCheckoutComplete::CreateLambda(
            [Delegate](const FOnlineError &Result, const TSharedRef<FPurchaseReceipt> &Receipt) {
                Delegate.ExecuteIfBound(Result);
            }));
}
#endif

void FOnlinePurchaseInterfaceRedpointSteam::FinalizePurchase(const FUniqueNetId &UserId, const FString &ReceiptId)
{
}

void FOnlinePurchaseInterfaceRedpointSteam::RedeemCode(
    const FUniqueNetId &UserId,
    const FRedeemCodeRequest &RedeemCodeRequest,
    const FOnPurchaseRedeemCodeComplete &Delegate)
{
    Delegate.ExecuteIfBound(
        OnlineRedpointEOS::Errors::InvalidRequest(
            TEXT("FOnlinePurchaseInterfaceRedpointSteam::RedeemCode"),
            TEXT("RedeemCode is not supported by Steam.")),
        MakeShared<FPurchaseReceipt>());
}

void FOnlinePurchaseInterfaceRedpointSteam::QueryReceipts(
    const FUniqueNetId &UserId,
    bool bRestoreReceipts,
    const FOnQueryReceiptsComplete &Delegate)
{
    Delegate.ExecuteIfBound(OnlineRedpointEOS::Errors::InvalidRequest(
        TEXT("FOnlinePurchaseInterfaceRedpointSteam::QueryReceipts"),
        TEXT("QueryReceipts is not supported by Steam.")));
}

void FOnlinePurchaseInterfaceRedpointSteam::GetReceipts(
    const FUniqueNetId &UserId,
    TArray<FPurchaseReceipt> &OutReceipts) const
{
    UE_LOG(LogRedpointSteam, Error, TEXT("GetReceipts is not supported by Steam."));
}

void FOnlinePurchaseInterfaceRedpointSteam::FinalizeReceiptValidationInfo(
    const FUniqueNetId &UserId,
    FString &InReceiptValidationInfo,
    const FOnFinalizeReceiptValidationInfoComplete &Delegate)
{
    Delegate.ExecuteIfBound(
        OnlineRedpointEOS::Errors::InvalidRequest(
            TEXT("FOnlinePurchaseInterfaceRedpointSteam::FinalizeReceiptValidationInfo"),
            TEXT("FinalizeReceiptValidationInfo is not supported by Steam.")),
        InReceiptValidationInfo);
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_STEAM_ENABLED