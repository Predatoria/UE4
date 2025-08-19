// Copyright June Rhodes. All Rights Reserved.

#include "OnlineEntitlementsInterfaceRedpointSteam.h"

#include "LogRedpointSteam.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "OnlineSubsystemRedpointSteam.h"
#include "SteamInventory.h"

#if EOS_STEAM_ENABLED

#include "./Tasks/OnlineAsyncTaskSteamGetAllItems.h"
#include "./Tasks/OnlineAsyncTaskSteamGetOwnedDLC.h"
#include "./Tasks/OnlineAsyncTaskSteamLoadItemDefinitions.h"

EOS_ENABLE_STRICT_WARNINGS

FOnlineEntitlementsInterfaceRedpointSteam::FOnlineEntitlementsInterfaceRedpointSteam()
{
}

void FOnlineEntitlementsInterfaceRedpointSteam::OnDlcRequested(
    const FOnlineError &Error,
    const FSteamEntitlementMap &DlcEntitlements,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<const FUniqueNetId> UserId,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FString Namespace)
{
    // Check if the DLC load failed.
    if (!Error.WasSuccessful())
    {
        this->TriggerOnQueryEntitlementsCompleteDelegates(false, *UserId, Namespace, Error.ToLogString());
        return;
    }

    this->CachedDlcs = DlcEntitlements;

    this->TriggerOnQueryEntitlementsCompleteDelegates(true, *UserId, Namespace, TEXT(""));
}

void FOnlineEntitlementsInterfaceRedpointSteam::OnGetAllItemsLoaded(
    const FOnlineError &Error,
    const FSteamEntitlementMap &Items,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<const FUniqueNetId> UserId,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FString Namespace)
{
    // Check if the item definitions were loaded.
    if (!Error.WasSuccessful())
    {
        this->TriggerOnQueryEntitlementsCompleteDelegates(false, *UserId, Namespace, Error.ToLogString());
        return;
    }

    this->CachedItems = Items;

    // NOLINTNEXTLINE(unreal-ionlinesubsystem-get)
    IOnlineSubsystem *OSS = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
    if (OSS == nullptr)
    {
        this->TriggerOnQueryEntitlementsCompleteDelegates(
            false,
            *UserId,
            Namespace,
            OnlineRedpointEOS::Errors::Missing_Feature(
                TEXT("FOnlineStoreInterfaceV2RedpointSteam::OnGetAllItemsLoaded"),
                TEXT("The Steam online subsystem is not available."))
                .ToLogString());
        return;
    }

    // Request owned DLC.
    FOnlineAsyncTaskSteamGetOwnedDLC *DlcTask =
        new FOnlineAsyncTaskSteamGetOwnedDLC(FSteamEntitlementsFetched::CreateThreadSafeSP(
            this,
            &FOnlineEntitlementsInterfaceRedpointSteam::OnDlcRequested,
            UserId,
            Namespace));
    FOnlineSubsystemSteamProtectedAccessor::GetAsyncTaskRunner((FOnlineSubsystemSteam *)OSS)->AddToInQueue(DlcTask);
}

void FOnlineEntitlementsInterfaceRedpointSteam::OnItemDefinitionsLoaded(
    const FOnlineError &Error,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<const FUniqueNetId> UserId,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FString Namespace)
{
    // Check if the item definitions were loaded.
    if (!Error.WasSuccessful())
    {
        this->TriggerOnQueryEntitlementsCompleteDelegates(false, *UserId, Namespace, Error.ToLogString());
        return;
    }

    // NOLINTNEXTLINE(unreal-ionlinesubsystem-get)
    IOnlineSubsystem *OSS = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
    if (OSS == nullptr)
    {
        this->TriggerOnQueryEntitlementsCompleteDelegates(
            false,
            *UserId,
            Namespace,
            OnlineRedpointEOS::Errors::Missing_Feature(
                TEXT("FOnlineStoreInterfaceV2RedpointSteam::OnItemDefinitionLoadComplete"),
                TEXT("The Steam online subsystem is not available."))
                .ToLogString());
        return;
    }

    // Now request item prices.
    FOnlineAsyncTaskSteamGetAllItems *ItemsTask =
        new FOnlineAsyncTaskSteamGetAllItems(FSteamEntitlementsFetched::CreateThreadSafeSP(
            this,
            &FOnlineEntitlementsInterfaceRedpointSteam::OnGetAllItemsLoaded,
            UserId,
            Namespace));
    FOnlineSubsystemSteamProtectedAccessor::GetAsyncTaskRunner((FOnlineSubsystemSteam *)OSS)->AddToInQueue(ItemsTask);
}

TSharedPtr<FOnlineEntitlement> FOnlineEntitlementsInterfaceRedpointSteam::GetEntitlement(
    const FUniqueNetId &UserId,
    const FUniqueEntitlementId &EntitlementId)
{
    if (this->CachedDlcs.Contains(EntitlementId))
    {
        return this->CachedDlcs[EntitlementId];
    }
    if (this->CachedItems.Contains(EntitlementId))
    {
        return this->CachedItems[EntitlementId];
    }

    return nullptr;
}

TSharedPtr<FOnlineEntitlement> FOnlineEntitlementsInterfaceRedpointSteam::GetItemEntitlement(
    const FUniqueNetId &UserId,
    const FString &ItemId)
{
    UE_LOG(
        LogRedpointSteam,
        Warning,
        TEXT("IOnlineEntitlement::GetItemEntitlement should not be used on Steam, as there may be multiple "
             "entitlements (inventory items) with the same item ID (the same item type)."));

    for (const auto &Entitlement : this->CachedDlcs)
    {
        if (Entitlement.Value->ItemId == ItemId)
        {
            return Entitlement.Value;
        }
    }
    for (const auto &Entitlement : this->CachedItems)
    {
        if (Entitlement.Value->ItemId == ItemId)
        {
            return Entitlement.Value;
        }
    }

    return nullptr;
}

void FOnlineEntitlementsInterfaceRedpointSteam::GetAllEntitlements(
    const FUniqueNetId &UserId,
    const FString &Namespace,
    TArray<TSharedRef<FOnlineEntitlement>> &OutUserEntitlements)
{
    for (const auto &KV : this->CachedDlcs)
    {
        // Include if the requested namespace is empty string (global), or if
        // the namespace matches.
        if (Namespace.IsEmpty() || Namespace == KV.Value->Namespace)
        {
            OutUserEntitlements.Add(KV.Value.ToSharedRef());
        }
    }
    for (const auto &KV : this->CachedItems)
    {
        // Include if the requested namespace is empty string (global), or if
        // the namespace matches.
        if (Namespace.IsEmpty() || Namespace == KV.Value->Namespace)
        {
            OutUserEntitlements.Add(KV.Value.ToSharedRef());
        }
    }
}

bool FOnlineEntitlementsInterfaceRedpointSteam::QueryEntitlements(
    const FUniqueNetId &UserId,
    const FString &Namespace,
    const FPagedQuery &Page)
{
    // NOLINTNEXTLINE(unreal-ionlinesubsystem-get)
    IOnlineSubsystem *OSS = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
    if (OSS == nullptr)
    {
        this->TriggerOnQueryEntitlementsCompleteDelegates(
            false,
            UserId,
            Namespace,
            OnlineRedpointEOS::Errors::Missing_Feature(
                TEXT("FOnlineStoreInterfaceV2RedpointSteam::QueryEntitlements"),
                TEXT("The Steam online subsystem is not available."))
                .ToLogString());
        return true;
    }

    // Load definitions.
    FOnlineAsyncTaskSteamLoadItemDefinitions *LoadTask =
        new FOnlineAsyncTaskSteamLoadItemDefinitions(FSteamItemLoadComplete::CreateThreadSafeSP(
            this,
            &FOnlineEntitlementsInterfaceRedpointSteam::OnItemDefinitionsLoaded,
            UserId.AsShared(),
            Namespace));
    FOnlineSubsystemSteamProtectedAccessor::GetAsyncTaskRunner((FOnlineSubsystemSteam *)OSS)->AddToInQueue(LoadTask);
    return true;
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_STEAM_ENABLED