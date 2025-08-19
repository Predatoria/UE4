// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/EOSPlatformUserIdManager.h"

#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"

#if defined(UE_5_0_OR_LATER)
FPlatformUserId FEOSPlatformUserIdManager::NextPlatformId = FPlatformUserId::CreateFromInternalId(8000);
#else
FPlatformUserId FEOSPlatformUserIdManager::NextPlatformId = 8000;
#endif // #if defined(UE_5_0_OR_LATER)
TMap<
    FPlatformUserId,
    TTuple<TWeakPtr<class FOnlineSubsystemEOS, ESPMode::ThreadSafe>, TSharedRef<const FUniqueNetIdEOS>>>
    FEOSPlatformUserIdManager::AllocatedIds = TMap<
        FPlatformUserId,
        TTuple<TWeakPtr<class FOnlineSubsystemEOS, ESPMode::ThreadSafe>, TSharedRef<const FUniqueNetIdEOS>>>();

FPlatformUserId FEOSPlatformUserIdManager::AllocatePlatformId(
    TSharedRef<FOnlineSubsystemEOS, ESPMode::ThreadSafe> InEOS,
    TSharedRef<const FUniqueNetIdEOS> InUserId)
{
#if defined(UE_5_0_OR_LATER)
    FPlatformUserId AllocatedId = FEOSPlatformUserIdManager::NextPlatformId;
    FEOSPlatformUserIdManager::NextPlatformId =
        FPlatformUserId::CreateFromInternalId(FEOSPlatformUserIdManager::NextPlatformId.GetInternalId() + 1);
#else
    FPlatformUserId AllocatedId = FEOSPlatformUserIdManager::NextPlatformId++;
#endif // #if defined(UE_5_0_OR_LATER)
    FEOSPlatformUserIdManager::AllocatedIds.Add(
        AllocatedId,
        TTuple<TWeakPtr<FOnlineSubsystemEOS, ESPMode::ThreadSafe>, TSharedRef<const FUniqueNetIdEOS>>(InEOS, InUserId));
    UE_LOG(
        LogEOS,
        Verbose,
        TEXT("Allocated %s in OSS %s to platform ID %d"),
        *InUserId->ToString(),
        *InEOS->GetInstanceName().ToString(),
        AllocatedId);
    return AllocatedId;
}

void FEOSPlatformUserIdManager::DeallocatePlatformId(FPlatformUserId InPlatformId)
{
    FEOSPlatformUserIdManager::AllocatedIds.Remove(InPlatformId);
}

TOptional<TTuple<TSharedRef<FOnlineSubsystemEOS, ESPMode::ThreadSafe>, TSharedRef<const FUniqueNetIdEOS>>>
FEOSPlatformUserIdManager::FindByPlatformId(FPlatformUserId InPlatformId)
{
    if (FEOSPlatformUserIdManager::AllocatedIds.Contains(InPlatformId))
    {
        const TTuple<TWeakPtr<class FOnlineSubsystemEOS, ESPMode::ThreadSafe>, TSharedRef<const FUniqueNetIdEOS>>
            &Value = FEOSPlatformUserIdManager::AllocatedIds[InPlatformId];
        TSharedPtr<class FOnlineSubsystemEOS, ESPMode::ThreadSafe> OSS = Value.Key.Pin();
        if (OSS.IsValid())
        {
            return TTuple<TSharedRef<FOnlineSubsystemEOS, ESPMode::ThreadSafe>, TSharedRef<const FUniqueNetIdEOS>>(
                OSS.ToSharedRef(),
                Value.Value);
        }
    }
    return TOptional<TTuple<TSharedRef<FOnlineSubsystemEOS, ESPMode::ThreadSafe>, TSharedRef<const FUniqueNetIdEOS>>>();
}