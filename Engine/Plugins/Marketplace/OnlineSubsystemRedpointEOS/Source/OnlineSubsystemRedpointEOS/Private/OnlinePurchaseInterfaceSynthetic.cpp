// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/OnlinePurchaseInterfaceSynthetic.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineIdentityInterfaceEOS.h"

EOS_ENABLE_STRICT_WARNINGS

static TSharedPtr<IOnlinePurchase, ESPMode::ThreadSafe> GetPurchase(IOnlineSubsystem *OSS)
{
    return OSS->GetPurchaseInterface();
}

bool FOnlinePurchaseInterfaceSynthetic::IsAllowedToPurchase(const FUniqueNetId &UserId)
{
    TSharedPtr<const FUniqueNetId> UserIdNative;
    TSharedPtr<IOnlinePurchase, ESPMode::ThreadSafe> Purchase =
        this->IdentityEOS->GetNativeInterface<IOnlinePurchase>(UserId, UserIdNative, GetPurchase);
    if (Purchase.IsValid())
    {
        return Purchase->IsAllowedToPurchase(*UserIdNative);
    }

    UE_LOG(
        LogEOS,
        Error,
        TEXT("%s"),
        *OnlineRedpointEOS::Errors::Missing_Feature(
             TEXT("FOnlinePurchaseInterfaceSynthetic::IsAllowedToPurchase"),
             TEXT("This platform does not support IOnlinePurchase."))
             .ToLogString());
    return false;
}

void FOnlinePurchaseInterfaceSynthetic::Checkout(
    const FUniqueNetId &UserId,
    const FPurchaseCheckoutRequest &CheckoutRequest,
    const FOnPurchaseCheckoutComplete &Delegate)
{
    TSharedPtr<const FUniqueNetId> UserIdNative;
    TSharedPtr<IOnlinePurchase, ESPMode::ThreadSafe> Purchase =
        this->IdentityEOS->GetNativeInterface<IOnlinePurchase>(UserId, UserIdNative, GetPurchase);
    if (Purchase.IsValid())
    {
        return Purchase->Checkout(*UserIdNative, CheckoutRequest, Delegate);
    }

    UE_LOG(
        LogEOS,
        Error,
        TEXT("%s"),
        *OnlineRedpointEOS::Errors::Missing_Feature(
             TEXT("FOnlinePurchaseInterfaceSynthetic::Checkout"),
             TEXT("This platform does not support IOnlinePurchase."))
             .ToLogString());
    Delegate.ExecuteIfBound(
        OnlineRedpointEOS::Errors::Missing_Feature(
            TEXT("FOnlinePurchaseInterfaceSynthetic::Checkout"),
            TEXT("This platform does not support IOnlinePurchase.")),
        MakeShared<FPurchaseReceipt>());
}

#if defined(UE_5_1_OR_LATER)
void FOnlinePurchaseInterfaceSynthetic::Checkout(
    const FUniqueNetId &UserId,
    const FPurchaseCheckoutRequest &CheckoutRequest,
    const FOnPurchaseReceiptlessCheckoutComplete &Delegate)
{
    TSharedPtr<const FUniqueNetId> UserIdNative;
    TSharedPtr<IOnlinePurchase, ESPMode::ThreadSafe> Purchase =
        this->IdentityEOS->GetNativeInterface<IOnlinePurchase>(UserId, UserIdNative, GetPurchase);
    if (Purchase.IsValid())
    {
        return Purchase->Checkout(*UserIdNative, CheckoutRequest, Delegate);
    }

    UE_LOG(
        LogEOS,
        Error,
        TEXT("%s"),
        *OnlineRedpointEOS::Errors::Missing_Feature(
             TEXT("FOnlinePurchaseInterfaceSynthetic::Checkout"),
             TEXT("This platform does not support IOnlinePurchase."))
             .ToLogString());
    Delegate.ExecuteIfBound(OnlineRedpointEOS::Errors::Missing_Feature(
        TEXT("FOnlinePurchaseInterfaceSynthetic::Checkout"),
        TEXT("This platform does not support IOnlinePurchase.")));
}
#endif

void FOnlinePurchaseInterfaceSynthetic::FinalizePurchase(const FUniqueNetId &UserId, const FString &ReceiptId)
{
    TSharedPtr<const FUniqueNetId> UserIdNative;
    TSharedPtr<IOnlinePurchase, ESPMode::ThreadSafe> Purchase =
        this->IdentityEOS->GetNativeInterface<IOnlinePurchase>(UserId, UserIdNative, GetPurchase);
    if (Purchase.IsValid())
    {
        return Purchase->FinalizePurchase(*UserIdNative, ReceiptId);
    }

    UE_LOG(
        LogEOS,
        Error,
        TEXT("%s"),
        *OnlineRedpointEOS::Errors::Missing_Feature(
             TEXT("FOnlinePurchaseInterfaceSynthetic::FinalizePurchase"),
             TEXT("This platform does not support IOnlinePurchase."))
             .ToLogString());
}

void FOnlinePurchaseInterfaceSynthetic::RedeemCode(
    const FUniqueNetId &UserId,
    const FRedeemCodeRequest &RedeemCodeRequest,
    const FOnPurchaseRedeemCodeComplete &Delegate)
{
    TSharedPtr<const FUniqueNetId> UserIdNative;
    TSharedPtr<IOnlinePurchase, ESPMode::ThreadSafe> Purchase =
        this->IdentityEOS->GetNativeInterface<IOnlinePurchase>(UserId, UserIdNative, GetPurchase);
    if (Purchase.IsValid())
    {
        return Purchase->RedeemCode(*UserIdNative, RedeemCodeRequest, Delegate);
    }

    UE_LOG(
        LogEOS,
        Error,
        TEXT("%s"),
        *OnlineRedpointEOS::Errors::Missing_Feature(
             TEXT("FOnlinePurchaseInterfaceSynthetic::RedeemCode"),
             TEXT("This platform does not support IOnlinePurchase."))
             .ToLogString());
    Delegate.ExecuteIfBound(
        OnlineRedpointEOS::Errors::Missing_Feature(
            TEXT("FOnlinePurchaseInterfaceSynthetic::RedeemCode"),
            TEXT("This platform does not support IOnlinePurchase.")),
        MakeShared<FPurchaseReceipt>());
}

void FOnlinePurchaseInterfaceSynthetic::QueryReceipts(
    const FUniqueNetId &UserId,
    bool bRestoreReceipts,
    const FOnQueryReceiptsComplete &Delegate)
{
    TSharedPtr<const FUniqueNetId> UserIdNative;
    TSharedPtr<IOnlinePurchase, ESPMode::ThreadSafe> Purchase =
        this->IdentityEOS->GetNativeInterface<IOnlinePurchase>(UserId, UserIdNative, GetPurchase);
    if (Purchase.IsValid())
    {
        return Purchase->QueryReceipts(*UserIdNative, bRestoreReceipts, Delegate);
    }

    UE_LOG(
        LogEOS,
        Error,
        TEXT("%s"),
        *OnlineRedpointEOS::Errors::Missing_Feature(
             TEXT("FOnlinePurchaseInterfaceSynthetic::QueryReceipts"),
             TEXT("This platform does not support IOnlinePurchase."))
             .ToLogString());
    Delegate.ExecuteIfBound(OnlineRedpointEOS::Errors::Missing_Feature(
        TEXT("FOnlinePurchaseInterfaceSynthetic::QueryReceipts"),
        TEXT("This platform does not support IOnlinePurchase.")));
}

void FOnlinePurchaseInterfaceSynthetic::GetReceipts(const FUniqueNetId &UserId, TArray<FPurchaseReceipt> &OutReceipts)
    const
{
    TSharedPtr<const FUniqueNetId> UserIdNative;
    TSharedPtr<IOnlinePurchase, ESPMode::ThreadSafe> Purchase =
        this->IdentityEOS->GetNativeInterface<IOnlinePurchase>(UserId, UserIdNative, GetPurchase);
    if (Purchase.IsValid())
    {
        return Purchase->GetReceipts(*UserIdNative, OutReceipts);
    }

    UE_LOG(
        LogEOS,
        Error,
        TEXT("%s"),
        *OnlineRedpointEOS::Errors::Missing_Feature(
             TEXT("FOnlinePurchaseInterfaceSynthetic::GetReceipts"),
             TEXT("This platform does not support IOnlinePurchase."))
             .ToLogString());
    OutReceipts.Reset();
}

void FOnlinePurchaseInterfaceSynthetic::FinalizeReceiptValidationInfo(
    const FUniqueNetId &UserId,
    FString &InReceiptValidationInfo,
    const FOnFinalizeReceiptValidationInfoComplete &Delegate)
{
    TSharedPtr<const FUniqueNetId> UserIdNative;
    TSharedPtr<IOnlinePurchase, ESPMode::ThreadSafe> Purchase =
        this->IdentityEOS->GetNativeInterface<IOnlinePurchase>(UserId, UserIdNative, GetPurchase);
    if (Purchase.IsValid())
    {
        return Purchase->FinalizeReceiptValidationInfo(*UserIdNative, InReceiptValidationInfo, Delegate);
    }

    UE_LOG(
        LogEOS,
        Error,
        TEXT("%s"),
        *OnlineRedpointEOS::Errors::Missing_Feature(
             TEXT("FOnlinePurchaseInterfaceSynthetic::FinalizeReceiptValidationInfo"),
             TEXT("This platform does not support IOnlinePurchase."))
             .ToLogString());
    Delegate.ExecuteIfBound(
        OnlineRedpointEOS::Errors::Missing_Feature(
            TEXT("FOnlinePurchaseInterfaceSynthetic::FinalizeReceiptValidationInfo"),
            TEXT("This platform does not support IOnlinePurchase.")),
        InReceiptValidationInfo);
}

EOS_DISABLE_STRICT_WARNINGS