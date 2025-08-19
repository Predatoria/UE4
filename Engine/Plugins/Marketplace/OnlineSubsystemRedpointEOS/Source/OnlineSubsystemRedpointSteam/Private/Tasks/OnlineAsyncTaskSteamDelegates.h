// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "Containers/Map.h"
#include "Containers/UnrealString.h"
#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "Interfaces/OnlineEntitlementsInterface.h"
#include "Interfaces/OnlineStoreInterfaceV2.h"
#include "OnlineError.h"
#include "Templates/SharedPointer.h"

#if EOS_STEAM_ENABLED

DECLARE_DELEGATE_OneParam(FSteamItemLoadComplete, const FOnlineError &Error);

typedef TMap<FUniqueOfferId, TSharedPtr<FOnlineStoreOffer>> FSteamOfferMap;
DECLARE_DELEGATE_TwoParams(FSteamOffersFetched, const FOnlineError &, const FSteamOfferMap &);

typedef TMap<FUniqueEntitlementId, TSharedPtr<FOnlineEntitlement>> FSteamEntitlementMap;
DECLARE_DELEGATE_TwoParams(FSteamEntitlementsFetched, const FOnlineError &, const FSteamEntitlementMap &);

#endif // #if EOS_STEAM_ENABLED