// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlinePurchaseInterfaceEAS.h"

EOS_ENABLE_STRICT_WARNINGS

bool FOnlinePurchaseInterfaceEAS::IsAllowedToPurchase(const FUniqueNetId &UserId)
{
    return true;
}

void FOnlinePurchaseInterfaceEAS::Checkout(
    const FUniqueNetId &UserId,
    const FPurchaseCheckoutRequest &CheckoutRequest,
    const FOnPurchaseCheckoutComplete &Delegate)
{
    Delegate.ExecuteIfBound(
        OnlineRedpointEOS::Errors::NotImplemented(
            TEXT("FOnlinePurchaseInterfaceEAS::Checkout"),
            TEXT("Epic Games Store support for purchases is not yet implemented.")),
        MakeShared<FPurchaseReceipt>());
}

#if defined(UE_5_1_OR_LATER)
void FOnlinePurchaseInterfaceEAS::Checkout(
    const FUniqueNetId &UserId,
    const FPurchaseCheckoutRequest &CheckoutRequest,
    const FOnPurchaseReceiptlessCheckoutComplete &Delegate)
{
    Delegate.ExecuteIfBound(OnlineRedpointEOS::Errors::NotImplemented(
        TEXT("FOnlinePurchaseInterfaceEAS::Checkout"),
        TEXT("Epic Games Store support for purchases is not yet implemented.")));
}
#endif

void FOnlinePurchaseInterfaceEAS::FinalizePurchase(const FUniqueNetId &UserId, const FString &ReceiptId)
{
}

void FOnlinePurchaseInterfaceEAS::RedeemCode(
    const FUniqueNetId &UserId,
    const FRedeemCodeRequest &RedeemCodeRequest,
    const FOnPurchaseRedeemCodeComplete &Delegate)
{
    Delegate.ExecuteIfBound(
        OnlineRedpointEOS::Errors::NotImplemented(
            TEXT("FOnlinePurchaseInterfaceEAS::RedeemCode"),
            TEXT("Epic Games Store support for purchases is not yet implemented.")),
        MakeShared<FPurchaseReceipt>());
}

void FOnlinePurchaseInterfaceEAS::QueryReceipts(
    const FUniqueNetId &UserId,
    bool bRestoreReceipts,
    const FOnQueryReceiptsComplete &Delegate)
{
    Delegate.ExecuteIfBound(OnlineRedpointEOS::Errors::NotImplemented(
        TEXT("FOnlinePurchaseInterfaceEAS::QueryReceipts"),
        TEXT("Epic Games Store support for purchases is not yet implemented.")));
}

void FOnlinePurchaseInterfaceEAS::GetReceipts(const FUniqueNetId &UserId, TArray<FPurchaseReceipt> &OutReceipts) const
{
}

void FOnlinePurchaseInterfaceEAS::FinalizeReceiptValidationInfo(
    const FUniqueNetId &UserId,
    FString &InReceiptValidationInfo,
    const FOnFinalizeReceiptValidationInfoComplete &Delegate)
{
    Delegate.ExecuteIfBound(
        OnlineRedpointEOS::Errors::NotImplemented(
            TEXT("FOnlinePurchaseInterfaceEAS::FinalizeReceiptValidationInfo"),
            TEXT("Epic Games Store support for purchases is not yet implemented.")),
        TEXT(""));
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION