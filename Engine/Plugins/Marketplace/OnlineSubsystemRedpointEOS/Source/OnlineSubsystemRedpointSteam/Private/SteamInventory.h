// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Map.h"
#include "Containers/UnrealString.h"

#if EOS_STEAM_ENABLED

THIRD_PARTY_INCLUDES_START
#include "steam/steam_api.h"
THIRD_PARTY_INCLUDES_END

EOS_ENABLE_STRICT_WARNINGS

struct FSteamItemDefinition
{
public:
    int32 SteamItemID;
    uint64 CurrentPrice;
    uint64 BasePrice;
    TMap<FString, FString> Properties;
};

namespace OnlineRedpointSteam
{
TArray<FSteamItemDefinition> ParseItemDefinitions(
    SteamItemDef_t *ItemDefinitions,
    uint64 *CurrentPrices,
    uint64 *BasePrices,
    uint32 ItemDefinitionCount);
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_STEAM_ENABLED