// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlinePurchaseInterface.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FOnlinePurchaseInterfaceSynthetic
    : public IOnlinePurchase,
      public TSharedFromThis<FOnlinePurchaseInterfaceSynthetic, ESPMode::ThreadSafe>
{
private:
    TSharedRef<class FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> IdentityEOS;

public:
    FOnlinePurchaseInterfaceSynthetic(TSharedRef<class FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> InIdentityEOS)
        : IdentityEOS(MoveTemp(InIdentityEOS)){};
    UE_NONCOPYABLE(FOnlinePurchaseInterfaceSynthetic);
    virtual ~FOnlinePurchaseInterfaceSynthetic() override = default;

    virtual bool IsAllowedToPurchase(const FUniqueNetId &UserId) override;
    virtual void Checkout(
        const FUniqueNetId &UserId,
        const FPurchaseCheckoutRequest &CheckoutRequest,
        const FOnPurchaseCheckoutComplete &Delegate) override;
    virtual void FinalizePurchase(const FUniqueNetId &UserId, const FString &ReceiptId) override;
#if defined(UE_5_1_OR_LATER)
    virtual void Checkout(
        const FUniqueNetId &UserId,
        const FPurchaseCheckoutRequest &CheckoutRequest,
        const FOnPurchaseReceiptlessCheckoutComplete &Delegate) override;
#endif
    virtual void RedeemCode(
        const FUniqueNetId &UserId,
        const FRedeemCodeRequest &RedeemCodeRequest,
        const FOnPurchaseRedeemCodeComplete &Delegate) override;
    virtual void QueryReceipts(
        const FUniqueNetId &UserId,
        bool bRestoreReceipts,
        const FOnQueryReceiptsComplete &Delegate) override;
    virtual void GetReceipts(const FUniqueNetId &UserId, TArray<FPurchaseReceipt> &OutReceipts) const override;
    virtual void FinalizeReceiptValidationInfo(
        const FUniqueNetId &UserId,
        FString &InReceiptValidationInfo,
        const FOnFinalizeReceiptValidationInfoComplete &Delegate) override;
};

EOS_DISABLE_STRICT_WARNINGS