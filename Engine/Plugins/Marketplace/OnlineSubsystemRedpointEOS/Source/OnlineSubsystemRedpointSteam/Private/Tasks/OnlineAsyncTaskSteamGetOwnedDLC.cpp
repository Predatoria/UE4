// Copyright June Rhodes. All Rights Reserved.

#include "./OnlineAsyncTaskSteamGetOwnedDLC.h"

#include "../LogRedpointSteam.h"
#include "../OnlineSubsystemRedpointSteam.h"
#include "../SteamConstants.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"

#if EOS_STEAM_ENABLED

EOS_ENABLE_STRICT_WARNINGS

void FOnlineAsyncTaskSteamGetOwnedDLC::Tick()
{
    // This can't fail, and isn't really an async task since
    // we do all our work on the game thread in Finalize...
    this->bIsComplete = true;
    this->bWasSuccessful = true;
}

void FOnlineAsyncTaskSteamGetOwnedDLC::Finalize()
{
    // Convert owned DLC apps into entitlements.
    TMap<FUniqueEntitlementId, TSharedPtr<FOnlineEntitlement>> Entitlements;
    int DlcCount = SteamApps()->GetDLCCount();
    for (int i = 0; i < DlcCount; i++)
    {
        AppId_t DlcId;
        bool bAvailable;
        char NameBuffer[2049];
        FMemory::Memzero(NameBuffer);
        if (SteamApps()->BGetDLCDataByIndex(i, &DlcId, &bAvailable, NameBuffer, 2048))
        {
            // We ignore bAvailable for owned DLC, because that represents whether
            // the DLC is available in the Steam store, and the user might
            // own DLC that is no longer available for sale.
            if (SteamApps()->BIsSubscribedApp(DlcId) || SteamApps()->BIsDlcInstalled(DlcId))
            {
                TSharedRef<FOnlineEntitlement> Entitlement = MakeShared<FOnlineEntitlement>();
                Entitlement->Id = FString::Printf(TEXT("%s:%u"), STEAM_TYPE_DLC, DlcId);

                Entitlement->Name = UTF8_TO_TCHAR(NameBuffer);
                Entitlement->ItemId = FString::Printf(TEXT("%s:%u"), STEAM_TYPE_DLC, DlcId);
                Entitlement->Namespace = STEAM_TYPE_DLC;

                Entitlement->bIsConsumable = false;
                Entitlement->RemainingCount = 1;
                Entitlement->ConsumedCount = 0;

                Entitlement->StartDate = TEXT("");
                Entitlement->EndDate = TEXT("");
                Entitlement->Status = SteamApps()->BIsDlcInstalled(DlcId) ? TEXT("Installed") : TEXT("Owned");

                Entitlements.Add(Entitlement->Id, Entitlement);
            }
        }
    }

    this->Delegate.ExecuteIfBound(OnlineRedpointEOS::Errors::Success(), Entitlements);
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_STEAM_ENABLED