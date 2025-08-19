// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemImpl.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineEntitlementsInterfaceEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineFriendsInterfaceEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineIdentityInterfaceEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlinePresenceInterfaceEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlinePurchaseInterfaceEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineStoreInterfaceV2EAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineUserInterfaceEAS.h"

EOS_ENABLE_STRICT_WARNINGS

class FOnlineSubsystemRedpointEAS : public FOnlineSubsystemImpl,
                                    public TSharedFromThis<FOnlineSubsystemRedpointEAS, ESPMode::ThreadSafe>
{
private:
    TSharedRef<class FOnlineSubsystemEOS, ESPMode::ThreadSafe> ParentEOS;
    TSharedPtr<FOnlineIdentityInterfaceEAS, ESPMode::ThreadSafe> IdentityImpl;
    TSharedPtr<FOnlineFriendsInterfaceEAS, ESPMode::ThreadSafe> FriendsImpl;
    TSharedPtr<FOnlinePresenceInterfaceEAS, ESPMode::ThreadSafe> PresenceImpl;
    TSharedPtr<FOnlineUserInterfaceEAS, ESPMode::ThreadSafe> UserImpl;
    TSharedPtr<FOnlineEntitlementsInterfaceEAS, ESPMode::ThreadSafe> EntitlementsImpl;
    TSharedPtr<FOnlineStoreInterfaceV2EAS, ESPMode::ThreadSafe> StoreV2Impl;
    TSharedPtr<FOnlinePurchaseInterfaceEAS, ESPMode::ThreadSafe> PurchaseImpl;

public:
    UE_NONCOPYABLE(FOnlineSubsystemRedpointEAS);
    FOnlineSubsystemRedpointEAS() = delete;
    FOnlineSubsystemRedpointEAS(
        FName InInstanceName,
        TSharedRef<class FOnlineSubsystemEOS, ESPMode::ThreadSafe> InParentEOS);
    ~FOnlineSubsystemRedpointEAS(){};

    virtual IOnlineFriendsPtr GetFriendsInterface() const override;
    virtual IOnlineIdentityPtr GetIdentityInterface() const override;
    virtual IOnlinePresencePtr GetPresenceInterface() const override;
    virtual IOnlineUserPtr GetUserInterface() const override;
    virtual IOnlineEntitlementsPtr GetEntitlementsInterface() const override;
    virtual IOnlineStoreV2Ptr GetStoreV2Interface() const override;
    virtual IOnlinePurchasePtr GetPurchaseInterface() const override;

    virtual bool Init() override;
    virtual bool Shutdown() override;

    virtual FString GetAppId() const override;

    virtual FText GetOnlineServiceName(void) const override;

    virtual IOnlineSessionPtr GetSessionInterface() const override
    {
        return nullptr;
    }

    virtual IOnlineTurnBasedPtr GetTurnBasedInterface() const override
    {
        return nullptr;
    }

    virtual IOnlineTournamentPtr GetTournamentInterface() const override
    {
        return nullptr;
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION