// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DiscordGameSDK.h"
#include "OnlineSubsystemRedpointDiscordConstants.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemImplBase.h"

#if EOS_DISCORD_ENABLED

class ONLINESUBSYSTEMREDPOINTDISCORD_API FOnlineSubsystemRedpointDiscord
    : public FOnlineSubsystemImplBase,
      public TSharedFromThis<FOnlineSubsystemRedpointDiscord, ESPMode::ThreadSafe>
{
private:
    bool bInitialized;
    TSharedPtr<discord::Core> Instance;

    TSharedPtr<class FOnlineIdentityInterfaceRedpointDiscord, ESPMode::ThreadSafe> IdentityImpl;
    TSharedPtr<class FOnlineExternalUIInterfaceRedpointDiscord, ESPMode::ThreadSafe> ExternalUIImpl;
    TSharedPtr<class FOnlineAvatarInterfaceRedpointDiscord, ESPMode::ThreadSafe> AvatarImpl;
    TSharedPtr<class FOnlineFriendsInterfaceRedpointDiscord, ESPMode::ThreadSafe> FriendsImpl;
    TSharedPtr<class FOnlinePresenceInterfaceRedpointDiscord, ESPMode::ThreadSafe> PresenceImpl;
    TSharedPtr<class FOnlineSessionInterfaceRedpointDiscord, ESPMode::ThreadSafe> SessionImpl;

public:
    UE_NONCOPYABLE(FOnlineSubsystemRedpointDiscord);
    FOnlineSubsystemRedpointDiscord() = delete;
    FOnlineSubsystemRedpointDiscord(FName InInstanceName);
    ~FOnlineSubsystemRedpointDiscord();

    virtual bool IsEnabled() const override;

    virtual IOnlineIdentityPtr GetIdentityInterface() const override;
    virtual IOnlineFriendsPtr GetFriendsInterface(void) const override;
    virtual IOnlinePresencePtr GetPresenceInterface(void) const override;
    virtual IOnlineSessionPtr GetSessionInterface(void) const override;
    virtual IOnlineExternalUIPtr GetExternalUIInterface() const override;

    // Our custom interfaces. Note that even those these APIs return a UObject*, we return
    // non-UObjects that are TSharedFromThis.
    virtual class UObject *GetNamedInterface(FName InterfaceName) override;
    virtual void SetNamedInterface(FName InterfaceName, class UObject *NewInterface) override
    {
        checkf(false, TEXT("FOnlineSubsystemRedpointDiscord::SetNamedInterface is not supported"));
    };

    virtual bool Init() override;
    virtual bool Tick(float DeltaTime) override;
    virtual bool Shutdown() override;

    virtual FString GetAppId() const override;
    virtual FText GetOnlineServiceName() const override;
};

#endif