// Copyright June Rhodes. All Rights Reserved.

#include "SteamInventory.h"

#include "LogRedpointSteam.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"

#if EOS_STEAM_ENABLED

EOS_ENABLE_STRICT_WARNINGS

TArray<FSteamItemDefinition> OnlineRedpointSteam::ParseItemDefinitions(
    SteamItemDef_t *Items,
    uint64 *CurrentPrices,
    uint64 *BasePrices,
    uint32 NumItems)
{
    TArray<FSteamItemDefinition> ItemDefinitions;
    for (uint32 i = 0; i < NumItems; i++)
    {
        // Get the full list of available properties for this item.
        uint32 BufferSizeRequired = 0;
        FString PropertyList;
        {
            if (!SteamInventory()->GetItemDefinitionProperty(Items[i], nullptr, nullptr, &BufferSizeRequired))
            {
                UE_LOG(
                    LogRedpointSteam,
                    Error,
                    TEXT("%s"),
                    *OnlineRedpointEOS::Errors::UnexpectedError(
                         TEXT("FOnlineAsyncTaskSteamRequestPrices::Finalize"),
                         TEXT("GetItemDefinitionProperty call failed (0x1)."))
                         .ToLogString())
                continue;
            }
            char *PropertyListBuffer = (char *)FMemory::MallocZeroed(sizeof(char) * (BufferSizeRequired + 1));
            if (!SteamInventory()
                     ->GetItemDefinitionProperty(Items[i], nullptr, PropertyListBuffer, &BufferSizeRequired))
            {
                FMemory::Free(PropertyListBuffer);
                UE_LOG(
                    LogRedpointSteam,
                    Error,
                    TEXT("%s"),
                    *OnlineRedpointEOS::Errors::UnexpectedError(
                         TEXT("FOnlineAsyncTaskSteamRequestPrices::Finalize"),
                         TEXT("GetItemDefinitionProperty call failed (0x2)."))
                         .ToLogString())
                continue;
            }
            PropertyList = ANSI_TO_TCHAR(PropertyListBuffer);
            FMemory::Free(PropertyListBuffer);
        }

        // Iterate through all of the properties, retrieving all of their values.
        TMap<FString, FString> Properties;
        {
            TArray<FString> PropertyNames;
            PropertyList.ParseIntoArray(PropertyNames, TEXT(","));
            for (const auto &PropertyName : PropertyNames)
            {
                auto SteamPropertyName = StringCast<ANSICHAR>(*PropertyName);
                BufferSizeRequired = 0;
                if (!SteamInventory()
                         ->GetItemDefinitionProperty(Items[i], SteamPropertyName.Get(), nullptr, &BufferSizeRequired))
                {
                    UE_LOG(
                        LogRedpointSteam,
                        Error,
                        TEXT("%s"),
                        *OnlineRedpointEOS::Errors::UnexpectedError(
                             TEXT("FOnlineAsyncTaskSteamRequestPrices::Finalize"),
                             TEXT("GetItemDefinitionProperty call failed (0x3)."))
                             .ToLogString())
                    continue;
                }
                char *PropertyValueBuffer = (char *)FMemory::MallocZeroed(sizeof(char) * (BufferSizeRequired + 1));
                if (!SteamInventory()->GetItemDefinitionProperty(
                        Items[i],
                        SteamPropertyName.Get(),
                        PropertyValueBuffer,
                        &BufferSizeRequired))
                {
                    FMemory::Free(PropertyValueBuffer);
                    UE_LOG(
                        LogRedpointSteam,
                        Error,
                        TEXT("%s"),
                        *OnlineRedpointEOS::Errors::UnexpectedError(
                             TEXT("FOnlineAsyncTaskSteamRequestPrices::Finalize"),
                             TEXT("GetItemDefinitionProperty call failed (0x4)."))
                             .ToLogString())
                    continue;
                }
                FString PropertyValue = UTF8_TO_TCHAR(PropertyValueBuffer);
                FMemory::Free(PropertyValueBuffer);
                Properties.Add(PropertyName, PropertyValue);
            }
        }

        ItemDefinitions.Add(FSteamItemDefinition{
            Items[i],
            CurrentPrices == nullptr ? 0 : CurrentPrices[i],
            BasePrices == nullptr ? 0 : BasePrices[i],
            Properties});
    }
    return ItemDefinitions;
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_STEAM_ENABLED