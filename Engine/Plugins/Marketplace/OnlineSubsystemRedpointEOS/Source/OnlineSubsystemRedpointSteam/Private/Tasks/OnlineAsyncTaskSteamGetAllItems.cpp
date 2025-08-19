// Copyright June Rhodes. All Rights Reserved.

#include "./OnlineAsyncTaskSteamGetAllItems.h"

#include "../LogRedpointSteam.h"
#include "../OnlineSubsystemRedpointSteam.h"
#include "../SteamConstants.h"
#include "../SteamInventory.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"

#if EOS_STEAM_ENABLED

EOS_ENABLE_STRICT_WARNINGS

void FOnlineAsyncTaskSteamGetAllItems::Tick()
{
    if (!this->bInit)
    {
        if (!SteamInventory()->GetAllItems(&this->Result))
        {
            this->bIsComplete = true;
            this->bWasSuccessful = false;
            return;
        }

        this->bInit = true;
    }

    EResult SteamResult = SteamInventory()->GetResultStatus(this->Result);
    switch (SteamResult)
    {
    case k_EResultPending:
        // We are waiting on the operation to complete.
        return;
    case k_EResultOK:
    case k_EResultExpired:
        // This is complete.
        this->bIsComplete = true;
        this->bWasSuccessful = true;
        break;
    case k_EResultInvalidParam:
        this->FailureContext = TEXT("GetAllItems failed (invalid param)");
        this->bIsComplete = true;
        this->bWasSuccessful = false;
        break;
    case k_EResultServiceUnavailable:
        this->FailureContext = TEXT("GetAllItems failed (unavailable)");
        this->bIsComplete = true;
        this->bWasSuccessful = false;
        break;
    case k_EResultLimitExceeded:
        this->FailureContext = TEXT("GetAllItems failed (limit exceeded)");
        this->bIsComplete = true;
        this->bWasSuccessful = false;
        break;
    case k_EResultFail:
        this->FailureContext = TEXT("GetAllItems failed (fail)");
        this->bIsComplete = true;
        this->bWasSuccessful = false;
        break;
    default:
        this->FailureContext = TEXT("GetAllItems failed (unknown)");
        this->bIsComplete = true;
        this->bWasSuccessful = false;
        break;
    }
}

void FOnlineAsyncTaskSteamGetAllItems::Finalize()
{
    if (!this->bWasSuccessful)
    {
        if (this->bInit)
        {
            SteamInventory()->DestroyResult(this->Result);
        }

        this->Delegate.ExecuteIfBound(
            OnlineRedpointEOS::Errors::UnexpectedError(
                TEXT("FOnlineAsyncTaskSteamGetAllItems::Finalize"),
                *this->FailureContext),
            FSteamEntitlementMap());
        return;
    }

    // Get the item definition IDs.
    uint32 ItemDefinitionCount;
    if (!SteamInventory()->GetItemDefinitionIDs(nullptr, &ItemDefinitionCount))
    {
        this->Delegate.ExecuteIfBound(
            OnlineRedpointEOS::Errors::UnexpectedError(
                TEXT("FOnlineAsyncTaskSteamGetAllItems::Finalize"),
                TEXT("GetItemDefinitionIDs failed")),
            FSteamEntitlementMap());
        return;
    }
    if (ItemDefinitionCount == 0)
    {
        this->Delegate.ExecuteIfBound(OnlineRedpointEOS::Errors::Success(), FSteamEntitlementMap());
        return;
    }
    SteamItemDef_t *ItemDefinitions = (SteamItemDef_t *)FMemory::Malloc(sizeof(SteamItemDef_t) * ItemDefinitionCount);
    if (!SteamInventory()->GetItemDefinitionIDs(ItemDefinitions, &ItemDefinitionCount))
    {
        this->Delegate.ExecuteIfBound(
            OnlineRedpointEOS::Errors::UnexpectedError(
                TEXT("FOnlineAsyncTaskSteamGetAllItems::Finalize"),
                TEXT("GetItemDefinitionIDs failed")),
            FSteamEntitlementMap());
        return;
    }

    // Parse the item definitions and generate the map.
    TArray<FSteamItemDefinition> ItemDefinitionsArray =
        OnlineRedpointSteam::ParseItemDefinitions(ItemDefinitions, nullptr, nullptr, ItemDefinitionCount);
    TMap<int32, FSteamItemDefinition> ItemDefinitionsMap;
    for (const auto &Entry : ItemDefinitionsArray)
    {
        ItemDefinitionsMap.Add(Entry.SteamItemID, Entry);
    }

    // Get the items from the result.
    uint32 ItemCount;
    if (!SteamInventory()->GetResultItems(this->Result, nullptr, &ItemCount))
    {
        this->Delegate.ExecuteIfBound(
            OnlineRedpointEOS::Errors::UnexpectedError(
                TEXT("FOnlineAsyncTaskSteamGetAllItems::Finalize"),
                TEXT("GetResultItems failed")),
            FSteamEntitlementMap());
        return;
    }
    if (ItemCount == 0)
    {
        this->Delegate.ExecuteIfBound(OnlineRedpointEOS::Errors::Success(), FSteamEntitlementMap());
        return;
    }
    SteamItemDetails_t *Items = (SteamItemDetails_t *)FMemory::Malloc(sizeof(SteamItemDetails_t) * ItemCount);
    if (!SteamInventory()->GetResultItems(this->Result, Items, &ItemCount))
    {
        this->Delegate.ExecuteIfBound(
            OnlineRedpointEOS::Errors::UnexpectedError(
                TEXT("FOnlineAsyncTaskSteamGetAllItems::Finalize"),
                TEXT("GetResultItems failed")),
            FSteamEntitlementMap());
        return;
    }

    // Parse the items from the result.
    TMap<FUniqueEntitlementId, TSharedPtr<FOnlineEntitlement>> Entitlements;
    for (uint32 i = 0; i < ItemCount; i++)
    {
        TSharedRef<FOnlineEntitlement> Entitlement = MakeShared<FOnlineEntitlement>();
        Entitlement->Id = FString::Printf(TEXT("%s:%llu"), STEAM_TYPE_ITEM, Items[i].m_itemId);

        const FSteamItemDefinition &ItemDef = ItemDefinitionsMap[Items[i].m_iDefinition];

        Entitlement->Name = ItemDef.Properties.Contains(TEXT("name")) ? ItemDef.Properties[TEXT("name")] : TEXT("");

        Entitlement->ItemId = FString::Printf(TEXT("%s:%d"), STEAM_TYPE_ITEM_DEFINITION, Items[i].m_iDefinition);

        Entitlement->Namespace = STEAM_TYPE_ITEM;

        Entitlement->bIsConsumable = ItemDef.Properties.Contains(TEXT("consumable"))
                                         ? ItemDef.Properties[TEXT("consumable")] == TEXT("true")
                                         : false;
        Entitlement->RemainingCount = Items[i].m_unQuantity;
        Entitlement->ConsumedCount = 0;

        Entitlement->StartDate = TEXT("");
        Entitlement->EndDate = TEXT("");

        Entitlement->Status = (Items[i].m_unFlags & k_ESteamItemNoTrade) != 0 ? TEXT("NotTradeable") : TEXT("Owned");

        Entitlements.Add(Entitlement->Id, Entitlement);
    }

    // All done.
    SteamInventory()->DestroyResult(this->Result);
    this->Delegate.ExecuteIfBound(OnlineRedpointEOS::Errors::Success(), Entitlements);
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_STEAM_ENABLED