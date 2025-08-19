// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/OnlineExternalUIInterfaceEOS.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/CrossPlatformAccountId.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGamesCrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSErrorConv.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineIdentityInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSessionInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/SyntheticPartySessionManager.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

#if EOS_HAS_AUTHENTICATION

FOnlineExternalUIInterfaceEOS::FOnlineExternalUIInterfaceEOS(
    EOS_HPlatform InPlatform,
    TSharedRef<FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> InIdentityEOS,
    TSharedRef<FOnlineSessionInterfaceEOS, ESPMode::ThreadSafe> InSessionEOS,
    const TSharedPtr<class FSyntheticPartySessionManager> &InSyntheticPartySessionManager)
    : EOSUI(EOS_Platform_GetUIInterface(InPlatform))
    , IdentityEOS(MoveTemp(InIdentityEOS))
    , SessionEOS(MoveTemp(InSessionEOS))
    , SyntheticPartySessionManager(InSyntheticPartySessionManager)
{
}

void FOnlineExternalUIInterfaceEOS::RegisterEvents()
{
    EOS_UI_AddNotifyDisplaySettingsUpdatedOptions Opts = {};
    Opts.ApiVersion = EOS_UI_ADDNOTIFYDISPLAYSETTINGSUPDATED_API_LATEST;

    this->Unregister_DisplaySettingsUpdated = EOSRegisterEvent<
        EOS_HUI,
        EOS_UI_AddNotifyDisplaySettingsUpdatedOptions,
        EOS_UI_OnDisplaySettingsUpdatedCallbackInfo>(
        this->EOSUI,
        &Opts,
        EOS_UI_AddNotifyDisplaySettingsUpdated,
        EOS_UI_RemoveNotifyDisplaySettingsUpdated,
        [WeakThis = GetWeakThis(this)](const EOS_UI_OnDisplaySettingsUpdatedCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                This->TriggerOnExternalUIChangeDelegates(Data->bIsVisible == EOS_TRUE);
            }
        });
}

bool FOnlineExternalUIInterfaceEOS::ShowLoginUI(
    const int ControllerIndex,
    bool bShowOnlineOnly,
    bool bShowSkipButton,
    const FOnLoginUIClosedDelegate &Delegate)
{
    FOnlineError Err = OnlineRedpointEOS::Errors::NotImplemented(
        TEXT("FOnlineExternalUIInterfaceEOS::ShowLoginUI"),
        TEXT("ShowLoginUI is not supported for EOS; call AutoLogin instead."));
    UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
    return false;
}

bool FOnlineExternalUIInterfaceEOS::ShowAccountCreationUI(
    const int ControllerIndex,
    const FOnAccountCreationUIClosedDelegate &Delegate)
{
    FOnlineError Err = OnlineRedpointEOS::Errors::NotImplemented(
        TEXT("FOnlineExternalUIInterfaceEOS::ShowAccountCreationUI"),
        TEXT("ShowAccountCreationUI is not supported for EOS; call AutoLogin instead."));
    UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
    return false;
}

bool FOnlineExternalUIInterfaceEOS::ShowFriendsUI(int32 LocalUserNum)
{
    TSharedPtr<const FUniqueNetId> UserId = this->IdentityEOS->GetUniquePlayerId(LocalUserNum);
    if (!UserId.IsValid())
    {
        FOnlineError Err = OnlineRedpointEOS::Errors::InvalidUser(
            TEXT("FOnlineExternalUIInterfaceEOS::ShowFriendsUI"),
            FString::Printf(TEXT("Can't show friends UI because local user %d isn't signed in."), LocalUserNum));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
        return false;
    }

    TSharedPtr<const FCrossPlatformAccountId> CrossPlatformId = this->IdentityEOS->GetCrossPlatformAccountId(*UserId);
    if (!CrossPlatformId.IsValid() || !CrossPlatformId->GetType().IsEqual(EPIC_GAMES_ACCOUNT_ID))
    {
        // Defer to native subsystem, if possible.
        IOnlineSubsystem *NativeSubsystem = this->IdentityEOS->GetNativeSubsystem(LocalUserNum);
        if (NativeSubsystem != nullptr)
        {
            IOnlineExternalUIPtr NativeUI = NativeSubsystem->GetExternalUIInterface();
            if (NativeUI.IsValid())
            {
                return NativeUI->ShowFriendsUI(LocalUserNum);
            }
        }
        FOnlineError Err = OnlineRedpointEOS::Errors::NotImplemented(
            TEXT("FOnlineExternalUIInterfaceEOS::ShowFriendsUI"),
            TEXT("This platform's native subsystem does not support ShowFriendsUI."));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
        return false;
    }

    EOS_UI_ShowFriendsOptions Opts = {};
    Opts.ApiVersion = EOS_UI_SHOWFRIENDS_API_LATEST;
    Opts.LocalUserId = StaticCastSharedPtr<const FEpicGamesCrossPlatformAccountId>(CrossPlatformId)->GetEpicAccountId();

    EOSRunOperation<EOS_HUI, EOS_UI_ShowFriendsOptions, EOS_UI_ShowFriendsCallbackInfo>(
        this->EOSUI,
        &Opts,
        EOS_UI_ShowFriends,
        [WeakThis = GetWeakThis(this)](const EOS_UI_ShowFriendsCallbackInfo *Info) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Info->ResultCode != EOS_EResult::EOS_Success)
                {
                    FOnlineError Err = ConvertError(
                        TEXT("FOnlineExternalUIInterfaceEOS::ShowFriendsUI"),
                        TEXT("Unable to open the friend UI."),
                        Info->ResultCode);
                    UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
                }
            }
        });
    return true;
}

bool FOnlineExternalUIInterfaceEOS::ShowInviteUI(int32 LocalUserNum, FName SessionName)
{
    IOnlineSubsystem *NativeSubsystem = this->IdentityEOS->GetNativeSubsystem(LocalUserNum);
    if (NativeSubsystem == nullptr)
    {
        FOnlineError Err = OnlineRedpointEOS::Errors::NotImplemented(
            TEXT("FOnlineExternalUIInterfaceEOS::ShowInviteUI"),
            TEXT("This platform does not have a native subsystem."));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
        return false;
    }

    IOnlineExternalUIPtr NativeUI = NativeSubsystem->GetExternalUIInterface();
    if (!NativeUI.IsValid())
    {
        FOnlineError Err = OnlineRedpointEOS::Errors::NotImplemented(
            TEXT("FOnlineExternalUIInterfaceEOS::ShowInviteUI"),
            TEXT("This platform's native subsystem does not support ShowInviteUI."));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
        return false;
    }

    FNamedOnlineSession *Session = this->SessionEOS->GetNamedSession(SessionName);
    if (Session == nullptr)
    {
        FOnlineError Err = OnlineRedpointEOS::Errors::NotFound(
            TEXT("FOnlineExternalUIInterfaceEOS::ShowInviteUI"),
            FString::Printf(TEXT("No session with the name '%s' exists."), *SessionName.ToString()));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
        return false;
    }

    auto SessionId = StaticCastSharedRef<const FUniqueNetIdEOSSession>(Session->SessionInfo->GetSessionId().AsShared());
    auto SPM = this->SyntheticPartySessionManager.Pin();
    if (!SPM.IsValid())
    {
        FOnlineError Err = OnlineRedpointEOS::Errors::Sessions::InviteFailed(
            TEXT("FOnlineExternalUIInterfaceEOS::ShowInviteUI"),
            FString::Printf(
                TEXT("The session with name '%s' does not have a synthetic session for platform '%s', so we can not "
                     "show the invite UI for it."),
                *SessionName.ToString(),
                *NativeSubsystem->GetSubsystemName().ToString()));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
        return false;
    }

    if (!SPM->HasSyntheticSession(NativeSubsystem->GetSubsystemName(), SessionId))
    {
        FOnlineError Err = OnlineRedpointEOS::Errors::Sessions::InviteFailed(
            TEXT("FOnlineExternalUIInterfaceEOS::ShowInviteUI"),
            FString::Printf(
                TEXT("The session with name '%s' does not have a synthetic session for platform '%s', so we can not "
                     "show the invite UI for it."),
                *SessionName.ToString(),
                *NativeSubsystem->GetSubsystemName().ToString()));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
        return false;
    }

    return NativeUI->ShowInviteUI(
        LocalUserNum,
        SPM->GetSyntheticSessionNativeSessionName(NativeSubsystem->GetSubsystemName(), SessionId));
}

bool FOnlineExternalUIInterfaceEOS::ShowAchievementsUI(int32 LocalUserNum)
{
    IOnlineSubsystem *NativeSubsystem = this->IdentityEOS->GetNativeSubsystem(LocalUserNum);
    if (NativeSubsystem != nullptr)
    {
        IOnlineExternalUIPtr NativeUI = NativeSubsystem->GetExternalUIInterface();
        if (NativeUI.IsValid())
        {
            return NativeUI->ShowAchievementsUI(LocalUserNum);
        }
    }

    FOnlineError Err = OnlineRedpointEOS::Errors::NotImplemented(
        TEXT("FOnlineExternalUIInterfaceEOS::ShowAchievementsUI"),
        TEXT("This platform's native subsystem does not support ShowAchievementsUI."));
    UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
    return false;
}

bool FOnlineExternalUIInterfaceEOS::ShowLeaderboardUI(const FString &LeaderboardName)
{
    IOnlineSubsystem *NativeSubsystem = this->IdentityEOS->GetNativeSubsystem(0);
    if (NativeSubsystem != nullptr)
    {
        IOnlineExternalUIPtr NativeUI = NativeSubsystem->GetExternalUIInterface();
        if (NativeUI.IsValid())
        {
            return NativeUI->ShowLeaderboardUI(LeaderboardName);
        }
    }

    FOnlineError Err = OnlineRedpointEOS::Errors::NotImplemented(
        TEXT("FOnlineExternalUIInterfaceEOS::ShowLeaderboardUI"),
        TEXT("This platform's native subsystem does not support ShowLeaderboardUI."));
    UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
    return false;
}

bool FOnlineExternalUIInterfaceEOS::ShowWebURL(
    const FString &Url,
    const FShowWebUrlParams &ShowParams,
    const FOnShowWebUrlClosedDelegate &Delegate)
{
    IOnlineSubsystem *NativeSubsystem = this->IdentityEOS->GetNativeSubsystem(0);
    if (NativeSubsystem != nullptr)
    {
        IOnlineExternalUIPtr NativeUI = NativeSubsystem->GetExternalUIInterface();
        if (NativeUI.IsValid())
        {
            return NativeUI->ShowWebURL(Url, ShowParams, Delegate);
        }
    }

    FOnlineError Err = OnlineRedpointEOS::Errors::NotImplemented(
        TEXT("FOnlineExternalUIInterfaceEOS::ShowWebURL"),
        TEXT("This platform's native subsystem does not support ShowWebURL."));
    UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
    return false;
}

bool FOnlineExternalUIInterfaceEOS::CloseWebURL()
{
    IOnlineSubsystem *NativeSubsystem = this->IdentityEOS->GetNativeSubsystem(0);
    if (NativeSubsystem != nullptr)
    {
        IOnlineExternalUIPtr NativeUI = NativeSubsystem->GetExternalUIInterface();
        if (NativeUI.IsValid())
        {
            return NativeUI->CloseWebURL();
        }
    }

    FOnlineError Err = OnlineRedpointEOS::Errors::NotImplemented(
        TEXT("FOnlineExternalUIInterfaceEOS::CloseWebURL"),
        TEXT("This platform's native subsystem does not support CloseWebURL."));
    UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
    return false;
}

bool FOnlineExternalUIInterfaceEOS::ShowProfileUI(
    const FUniqueNetId &Requestor,
    const FUniqueNetId &Requestee,
    const FOnProfileUIClosedDelegate &Delegate)
{
    IOnlineSubsystem *NativeSubsystem = this->IdentityEOS->GetNativeSubsystem(0);
    if (NativeSubsystem != nullptr)
    {
        IOnlineExternalUIPtr NativeUI = NativeSubsystem->GetExternalUIInterface();
        if (NativeUI.IsValid())
        {
            return NativeUI->ShowProfileUI(Requestor, Requestee, Delegate);
        }
    }

    FOnlineError Err = OnlineRedpointEOS::Errors::NotImplemented(
        TEXT("FOnlineExternalUIInterfaceEOS::ShowProfileUI"),
        TEXT("This platform's native subsystem does not support ShowProfileUI."));
    UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
    return false;
}

bool FOnlineExternalUIInterfaceEOS::ShowAccountUpgradeUI(const FUniqueNetId &UniqueId)
{
    IOnlineSubsystem *NativeSubsystem = this->IdentityEOS->GetNativeSubsystem(0);
    if (NativeSubsystem != nullptr)
    {
        IOnlineExternalUIPtr NativeUI = NativeSubsystem->GetExternalUIInterface();
        if (NativeUI.IsValid())
        {
            return NativeUI->ShowAccountUpgradeUI(UniqueId);
        }
    }

    FOnlineError Err = OnlineRedpointEOS::Errors::NotImplemented(
        TEXT("FOnlineExternalUIInterfaceEOS::ShowAccountUpgradeUI"),
        TEXT("This platform's native subsystem does not support ShowAccountUpgradeUI."));
    UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
    return false;
}

bool FOnlineExternalUIInterfaceEOS::ShowStoreUI(
    int32 LocalUserNum,
    const FShowStoreParams &ShowParams,
    const FOnShowStoreUIClosedDelegate &Delegate)
{
    IOnlineSubsystem *NativeSubsystem = this->IdentityEOS->GetNativeSubsystem(LocalUserNum);
    if (NativeSubsystem != nullptr)
    {
        IOnlineExternalUIPtr NativeUI = NativeSubsystem->GetExternalUIInterface();
        if (NativeUI.IsValid())
        {
            return NativeUI->ShowStoreUI(LocalUserNum, ShowParams, Delegate);
        }
    }

    FOnlineError Err = OnlineRedpointEOS::Errors::NotImplemented(
        TEXT("FOnlineExternalUIInterfaceEOS::ShowStoreUI"),
        TEXT("This platform's native subsystem does not support ShowStoreUI."));
    UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
    return false;
}

bool FOnlineExternalUIInterfaceEOS::ShowSendMessageUI(
    int32 LocalUserNum,
    const FShowSendMessageParams &ShowParams,
    const FOnShowSendMessageUIClosedDelegate &Delegate)
{
    IOnlineSubsystem *NativeSubsystem = this->IdentityEOS->GetNativeSubsystem(LocalUserNum);
    if (NativeSubsystem != nullptr)
    {
        IOnlineExternalUIPtr NativeUI = NativeSubsystem->GetExternalUIInterface();
        if (NativeUI.IsValid())
        {
            return NativeUI->ShowSendMessageUI(LocalUserNum, ShowParams, Delegate);
        }
    }

    FOnlineError Err = OnlineRedpointEOS::Errors::NotImplemented(
        TEXT("FOnlineExternalUIInterfaceEOS::ShowSendMessageUI"),
        TEXT("This platform's native subsystem does not support ShowSendMessageUI."));
    UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
    return false;
}

bool FOnlineExternalUIInterfaceEOS::ShowSendMessageToUserUI(
    int32 LocalUserNum,
    const FUniqueNetId &Recipient,
    const FShowSendMessageParams &ShowParams,
    const FOnShowSendMessageUIClosedDelegate &Delegate)
{
    IOnlineSubsystem *NativeSubsystem = this->IdentityEOS->GetNativeSubsystem(LocalUserNum);
    if (NativeSubsystem != nullptr)
    {
        IOnlineExternalUIPtr NativeUI = NativeSubsystem->GetExternalUIInterface();
        if (NativeUI.IsValid())
        {
            return NativeUI->ShowSendMessageToUserUI(LocalUserNum, Recipient, ShowParams, Delegate);
        }
    }

    FOnlineError Err = OnlineRedpointEOS::Errors::NotImplemented(
        TEXT("FOnlineExternalUIInterfaceEOS::ShowSendMessageToUserUI"),
        TEXT("This platform's native subsystem does not support ShowSendMessageToUserUI."));
    UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
    return false;
}

bool FOnlineExternalUIInterfaceEOS::ShowPlatformMessageBox(const FUniqueNetId &UserId, EPlatformMessageType MessageType)
{
    IOnlineSubsystem *NativeSubsystem = this->IdentityEOS->GetNativeSubsystem(0);
    if (NativeSubsystem != nullptr)
    {
        IOnlineExternalUIPtr NativeUI = NativeSubsystem->GetExternalUIInterface();
        if (NativeUI.IsValid())
        {
            return NativeUI->ShowPlatformMessageBox(UserId, MessageType);
        }
    }

    FOnlineError Err = OnlineRedpointEOS::Errors::NotImplemented(
        TEXT("FOnlineExternalUIInterfaceEOS::ShowPlatformMessageBox"),
        TEXT("This platform's native subsystem does not support ShowPlatformMessageBox."));
    UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
    return false;
}

void FOnlineExternalUIInterfaceEOS::ReportEnterInGameStoreUI()
{
    IOnlineSubsystem *NativeSubsystem = this->IdentityEOS->GetNativeSubsystem(0);
    if (NativeSubsystem != nullptr)
    {
        IOnlineExternalUIPtr NativeUI = NativeSubsystem->GetExternalUIInterface();
        if (NativeUI.IsValid())
        {
            return NativeUI->ReportEnterInGameStoreUI();
        }
    }
}

void FOnlineExternalUIInterfaceEOS::ReportExitInGameStoreUI()
{
    IOnlineSubsystem *NativeSubsystem = this->IdentityEOS->GetNativeSubsystem(0);
    if (NativeSubsystem != nullptr)
    {
        IOnlineExternalUIPtr NativeUI = NativeSubsystem->GetExternalUIInterface();
        if (NativeUI.IsValid())
        {
            return NativeUI->ReportExitInGameStoreUI();
        }
    }
}

#endif // #if EOS_HAS_AUTHENTICATION

EOS_DISABLE_STRICT_WARNINGS