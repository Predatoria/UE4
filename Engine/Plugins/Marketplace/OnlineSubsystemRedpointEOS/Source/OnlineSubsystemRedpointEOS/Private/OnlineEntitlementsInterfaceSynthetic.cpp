// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/OnlineEntitlementsInterfaceSynthetic.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineIdentityInterfaceEOS.h"

EOS_ENABLE_STRICT_WARNINGS

static TSharedPtr<IOnlineEntitlements, ESPMode::ThreadSafe> GetEntitlements(IOnlineSubsystem *OSS)
{
    return OSS->GetEntitlementsInterface();
}

void FOnlineEntitlementsInterfaceSynthetic::OnQueryEntitlementsComplete(
    bool bWasSuccessful,
    const FUniqueNetId &UserId,
    const FString &Namespace,
    const FString &Error,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FString RequestedNamespace,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<const FUniqueNetId> RequestedUserId,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<const FUniqueNetId> OriginalNativeUserId)
{
    if (Namespace != RequestedNamespace && UserId != *OriginalNativeUserId)
    {
        // Not for this binding.
        return;
    }

    TSharedPtr<const FUniqueNetId> _;
    TSharedPtr<IOnlineEntitlements, ESPMode::ThreadSafe> Entitlements =
        this->IdentityEOS->GetNativeInterface<IOnlineEntitlements>(*RequestedUserId, _, GetEntitlements);
    if (!Entitlements.IsValid())
    {
        // Entitlements not valid, can't de-register event handler.
        return;
    }

    FString HandlerKey = FString::Printf(TEXT("%s // %s"), *RequestedUserId->ToString(), *Namespace);
    Entitlements->ClearOnQueryEntitlementsCompleteDelegate_Handle(this->EventHandlers[HandlerKey]);
    this->EventHandlers.Remove(HandlerKey);

    // Propagate event, mapping user ID back to the original user ID.
    this->TriggerOnQueryEntitlementsCompleteDelegates(bWasSuccessful, *RequestedUserId, Namespace, Error);
}

TSharedPtr<FOnlineEntitlement> FOnlineEntitlementsInterfaceSynthetic::GetEntitlement(
    const FUniqueNetId &UserId,
    const FUniqueEntitlementId &EntitlementId)
{
    TSharedPtr<const FUniqueNetId> UserIdNative;
    TSharedPtr<IOnlineEntitlements, ESPMode::ThreadSafe> Entitlements =
        this->IdentityEOS->GetNativeInterface<IOnlineEntitlements>(UserId, UserIdNative, GetEntitlements);
    if (Entitlements.IsValid())
    {
        return Entitlements->GetEntitlement(*UserIdNative, EntitlementId);
    }

    UE_LOG(
        LogEOS,
        Error,
        TEXT("%s"),
        *OnlineRedpointEOS::Errors::Missing_Feature(
             TEXT("FOnlineEntitlementsInterfaceSynthetic::GetEntitlement"),
             TEXT("This platform does not support IOnlineEntitlements."))
             .ToLogString());
    return TSharedPtr<FOnlineEntitlement>();
}

TSharedPtr<FOnlineEntitlement> FOnlineEntitlementsInterfaceSynthetic::GetItemEntitlement(
    const FUniqueNetId &UserId,
    const FString &ItemId)
{
    TSharedPtr<const FUniqueNetId> UserIdNative;
    TSharedPtr<IOnlineEntitlements, ESPMode::ThreadSafe> Entitlements =
        this->IdentityEOS->GetNativeInterface<IOnlineEntitlements>(UserId, UserIdNative, GetEntitlements);
    if (Entitlements.IsValid())
    {
        return Entitlements->GetItemEntitlement(*UserIdNative, ItemId);
    }

    UE_LOG(
        LogEOS,
        Error,
        TEXT("%s"),
        *OnlineRedpointEOS::Errors::Missing_Feature(
             TEXT("FOnlineEntitlementsInterfaceSynthetic::GetItemEntitlement"),
             TEXT("This platform does not support IOnlineEntitlements."))
             .ToLogString());
    return TSharedPtr<FOnlineEntitlement>();
}

void FOnlineEntitlementsInterfaceSynthetic::GetAllEntitlements(
    const FUniqueNetId &UserId,
    const FString &Namespace,
    TArray<TSharedRef<FOnlineEntitlement>> &OutUserEntitlements)
{
    TSharedPtr<const FUniqueNetId> UserIdNative;
    TSharedPtr<IOnlineEntitlements, ESPMode::ThreadSafe> Entitlements =
        this->IdentityEOS->GetNativeInterface<IOnlineEntitlements>(UserId, UserIdNative, GetEntitlements);
    if (Entitlements.IsValid())
    {
        return Entitlements->GetAllEntitlements(*UserIdNative, Namespace, OutUserEntitlements);
    }

    UE_LOG(
        LogEOS,
        Error,
        TEXT("%s"),
        *OnlineRedpointEOS::Errors::Missing_Feature(
             TEXT("FOnlineEntitlementsInterfaceSynthetic::GetAllEntitlements"),
             TEXT("This platform does not support IOnlineEntitlements."))
             .ToLogString());
}

bool FOnlineEntitlementsInterfaceSynthetic::QueryEntitlements(
    const FUniqueNetId &UserId,
    const FString &Namespace,
    const FPagedQuery &Page)
{
    TSharedPtr<const FUniqueNetId> UserIdNative;
    TSharedPtr<IOnlineEntitlements, ESPMode::ThreadSafe> Entitlements =
        this->IdentityEOS->GetNativeInterface<IOnlineEntitlements>(UserId, UserIdNative, GetEntitlements);
    if (Entitlements.IsValid())
    {
        FString HandlerKey = FString::Printf(TEXT("%s // %s"), *UserId.ToString(), *Namespace);
        if (!this->EventHandlers.Contains(HandlerKey))
        {
            this->EventHandlers.Add(
                HandlerKey,
                Entitlements->AddOnQueryEntitlementsCompleteDelegate_Handle(
                    FOnQueryEntitlementsCompleteDelegate::CreateThreadSafeSP(
                        this,
                        &FOnlineEntitlementsInterfaceSynthetic::OnQueryEntitlementsComplete,
                        Namespace,
                        UserId.AsShared(),
                        UserIdNative.ToSharedRef())));
            return Entitlements->QueryEntitlements(*UserIdNative, Namespace, Page);
        }
        else
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("%s"),
                *OnlineRedpointEOS::Errors::Missing_Feature(
                     TEXT("FOnlineEntitlementsInterfaceSynthetic::QueryEntitlements"),
                     TEXT("Another QueryEntitlements operation is in progress for the same user ID and namespace."))
                     .ToLogString());
            return false;
        }
    }

    UE_LOG(
        LogEOS,
        Error,
        TEXT("%s"),
        *OnlineRedpointEOS::Errors::Missing_Feature(
             TEXT("FOnlineEntitlementsInterfaceSynthetic::QueryEntitlements"),
             TEXT("This platform does not support IOnlineEntitlements."))
             .ToLogString());
    return false;
}

EOS_DISABLE_STRICT_WARNINGS