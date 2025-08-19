// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "Containers/Map.h"
#include "CoreMinimal.h"
#include "Misc/CoreMiscDefines.h"
#include "Misc/Optional.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"
#include "Templates/Tuple.h"
#if defined(UE_5_0_OR_LATER)
#include "Online/CoreOnline.h"
#else
#include "UObject/CoreOnline.h"
#endif

class ONLINESUBSYSTEMREDPOINTEOS_API FEOSPlatformUserIdManager
{
private:
    static FPlatformUserId NextPlatformId;
    static TMap<
        FPlatformUserId,
        TTuple<TWeakPtr<class FOnlineSubsystemEOS, ESPMode::ThreadSafe>, TSharedRef<const FUniqueNetIdEOS>>>
        AllocatedIds;

public:
    static FPlatformUserId AllocatePlatformId(
        TSharedRef<class FOnlineSubsystemEOS, ESPMode::ThreadSafe> InEOS,
        TSharedRef<const FUniqueNetIdEOS> InUserId);
    static void DeallocatePlatformId(FPlatformUserId InPlatformId);

    static TOptional<
        TTuple<TSharedRef<class FOnlineSubsystemEOS, ESPMode::ThreadSafe>, TSharedRef<const FUniqueNetIdEOS>>>
    FindByPlatformId(FPlatformUserId InPlatformId);
};