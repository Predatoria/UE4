// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineEntitlementsInterfaceEAS.h"

EOS_ENABLE_STRICT_WARNINGS

TSharedPtr<FOnlineEntitlement> FOnlineEntitlementsInterfaceEAS::GetEntitlement(
    const FUniqueNetId &UserId,
    const FUniqueEntitlementId &EntitlementId)
{
    return TSharedPtr<FOnlineEntitlement>();
}

TSharedPtr<FOnlineEntitlement> FOnlineEntitlementsInterfaceEAS::GetItemEntitlement(
    const FUniqueNetId &UserId,
    const FString &ItemId)
{
    return TSharedPtr<FOnlineEntitlement>();
}

void FOnlineEntitlementsInterfaceEAS::GetAllEntitlements(
    const FUniqueNetId &UserId,
    const FString &Namespace,
    TArray<TSharedRef<FOnlineEntitlement>> &OutUserEntitlements)
{
}

bool FOnlineEntitlementsInterfaceEAS::QueryEntitlements(
    const FUniqueNetId &UserId,
    const FString &Namespace,
    const FPagedQuery &Page)
{
    return false;
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION