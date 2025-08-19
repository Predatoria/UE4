// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineExternalUIInterface.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSDefines.h"

EOS_ENABLE_STRICT_WARNINGS

#if EOS_HAS_AUTHENTICATION

class ONLINESUBSYSTEMREDPOINTEOS_API FOnlineExternalUIInterfaceEOS
    : public IOnlineExternalUI,
      public TSharedFromThis<FOnlineExternalUIInterfaceEOS, ESPMode::ThreadSafe>
{
private:
    EOS_HUI EOSUI;
    TSharedRef<class FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> IdentityEOS;
    TSharedRef<class FOnlineSessionInterfaceEOS, ESPMode::ThreadSafe> SessionEOS;
    TWeakPtr<class FSyntheticPartySessionManager> SyntheticPartySessionManager;
    TSharedPtr<EOSEventHandle<EOS_UI_OnDisplaySettingsUpdatedCallbackInfo>> Unregister_DisplaySettingsUpdated;

public:
    FOnlineExternalUIInterfaceEOS(
        EOS_HPlatform InPlatform,
        TSharedRef<class FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> InIdentityEOS,
        TSharedRef<class FOnlineSessionInterfaceEOS, ESPMode::ThreadSafe> InSessionEOS,
        const TSharedPtr<class FSyntheticPartySessionManager> &InSyntheticPartySessionManager);
    UE_NONCOPYABLE(FOnlineExternalUIInterfaceEOS);
    virtual ~FOnlineExternalUIInterfaceEOS(){};

    void RegisterEvents();

    virtual bool ShowLoginUI(
        const int ControllerIndex,
        bool bShowOnlineOnly,
        bool bShowSkipButton,
        const FOnLoginUIClosedDelegate &Delegate = FOnLoginUIClosedDelegate()) override;
    virtual bool ShowAccountCreationUI(
        const int ControllerIndex,
        const FOnAccountCreationUIClosedDelegate &Delegate = FOnAccountCreationUIClosedDelegate()) override;
    virtual bool ShowFriendsUI(int32 LocalUserNum) override;
    virtual bool ShowInviteUI(int32 LocalUserNum, FName SessionName = NAME_GameSession) override;
    virtual bool ShowAchievementsUI(int32 LocalUserNum) override;
    virtual bool ShowLeaderboardUI(const FString &LeaderboardName) override;
    virtual bool ShowWebURL(
        const FString &Url,
        const FShowWebUrlParams &ShowParams,
        const FOnShowWebUrlClosedDelegate &Delegate = FOnShowWebUrlClosedDelegate()) override;
    virtual bool CloseWebURL() override;
    virtual bool ShowProfileUI(
        const FUniqueNetId &Requestor,
        const FUniqueNetId &Requestee,
        const FOnProfileUIClosedDelegate &Delegate = FOnProfileUIClosedDelegate()) override;
    virtual bool ShowAccountUpgradeUI(const FUniqueNetId &UniqueId) override;
    virtual bool ShowStoreUI(
        int32 LocalUserNum,
        const FShowStoreParams &ShowParams,
        const FOnShowStoreUIClosedDelegate &Delegate = FOnShowStoreUIClosedDelegate()) override;
    virtual bool ShowSendMessageUI(
        int32 LocalUserNum,
        const FShowSendMessageParams &ShowParams,
        const FOnShowSendMessageUIClosedDelegate &Delegate = FOnShowSendMessageUIClosedDelegate()) override;
    virtual bool ShowSendMessageToUserUI(
        int32 LocalUserNum,
        const FUniqueNetId &Recipient,
        const FShowSendMessageParams &ShowParams,
        const FOnShowSendMessageUIClosedDelegate &Delegate = FOnShowSendMessageUIClosedDelegate()) override;
    virtual bool ShowPlatformMessageBox(const FUniqueNetId &UserId, EPlatformMessageType MessageType) override;
    virtual void ReportEnterInGameStoreUI() override;
    virtual void ReportExitInGameStoreUI() override;
};

#endif // #if EOS_HAS_AUTHENTICATION

EOS_DISABLE_STRICT_WARNINGS