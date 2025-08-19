// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DiscordGameSDK.h"
#include "Interfaces/OnlineExternalUIInterface.h"

#if EOS_DISCORD_ENABLED

class FOnlineExternalUIInterfaceRedpointDiscord
    : public IOnlineExternalUI,
      public TSharedFromThis<FOnlineExternalUIInterfaceRedpointDiscord, ESPMode::ThreadSafe>
{
    friend class FOnlineIdentityInterfaceRedpointDiscord;

private:
    TSharedRef<discord::Core> Instance;
    discord::Event<>::Token OnCurrentUserUpdateHandle;
    TSharedPtr<discord::User> CurrentUser;
    TSharedPtr<discord::OAuth2Token> OAuth2Token;
    TArray<FOnLoginUIClosedDelegate> PendingLoginCallbacks;
    bool bAuthenticated;

public:
    FOnlineExternalUIInterfaceRedpointDiscord(TSharedRef<discord::Core> InInstance);
    UE_NONCOPYABLE(FOnlineExternalUIInterfaceRedpointDiscord);
    virtual ~FOnlineExternalUIInterfaceRedpointDiscord();

    void RegisterEvents();

    virtual bool ShowLoginUI(
        const int ControllerIndex,
        bool bShowOnlineOnly,
        bool bShowSkipButton,
        const FOnLoginUIClosedDelegate &Delegate) override;
    virtual bool ShowAccountCreationUI(const int ControllerIndex, const FOnAccountCreationUIClosedDelegate &Delegate)
        override;
    virtual bool ShowFriendsUI(int32 LocalUserNum) override;
    virtual bool ShowInviteUI(int32 LocalUserNum, FName SessionName) override;
    virtual bool ShowAchievementsUI(int32 LocalUserNum) override;
    virtual bool ShowLeaderboardUI(const FString &LeaderboardName) override;
    virtual bool ShowWebURL(
        const FString &Url,
        const FShowWebUrlParams &ShowParams,
        const FOnShowWebUrlClosedDelegate &Delegate) override;
    virtual bool CloseWebURL() override;
    virtual bool ShowProfileUI(
        const FUniqueNetId &Requestor,
        const FUniqueNetId &Requestee,
        const FOnProfileUIClosedDelegate &Delegate) override;
    virtual bool ShowAccountUpgradeUI(const FUniqueNetId &UniqueId) override;
    virtual bool ShowStoreUI(
        int32 LocalUserNum,
        const FShowStoreParams &ShowParams,
        const FOnShowStoreUIClosedDelegate &Delegate) override;
    virtual bool ShowSendMessageUI(
        int32 LocalUserNum,
        const FShowSendMessageParams &ShowParams,
        const FOnShowSendMessageUIClosedDelegate &Delegate) override;
};

#endif