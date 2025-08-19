// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSessionInterfaceRedpointDiscord.h"

#if EOS_DISCORD_ENABLED

#include "Containers/StringConv.h"
#include "LogRedpointDiscord.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/ScopeLock.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "UniqueNetIdRedpointDiscord.h"

#define DISCORD_CONFIG_SECTION TEXT("OnlineSubsystemRedpointDiscord")

class FOnlineSessionInfoRedpointDiscord : public FOnlineSessionInfo
{
private:
    TSharedRef<const FUniqueNetId> SessionId;

public:
    FOnlineSessionInfoRedpointDiscord(TSharedRef<const FUniqueNetId> InSessionId)
        : FOnlineSessionInfo()
        , SessionId(MoveTemp(InSessionId)){};

    virtual const FUniqueNetId &GetSessionId() const override
    {
        return *SessionId;
    }
    virtual const uint8 *GetBytes() const override
    {
        return SessionId->GetBytes();
    }
    virtual int32 GetSize() const override
    {
        return SessionId->GetSize();
    }
    virtual bool IsValid() const override
    {
        return true;
    }
    virtual FString ToString() const override
    {
        return SessionId->ToString();
    }
    virtual FString ToDebugString() const override
    {
        return SessionId->ToDebugString();
    }
};

void FOnlineSessionInterfaceRedpointDiscord::RegisterEvents()
{
    this->OnActivityJoinRequestHandle = this->Instance->ActivityManager().OnActivityJoinRequest.Connect(
        [WeakThis = GetWeakThis(this)](discord::User const &InUser) {
            if (auto This = PinWeakThis(WeakThis))
            {
                // We always allow join requests and then leave it up to the game to handle at a higher level.
                This->Instance->ActivityManager().SendRequestReply(
                    InUser.GetId(),
                    discord::ActivityJoinRequestReply::Yes,
                    [](discord::Result Result) {
                        // Ignored for now.
                    });
            }
        });
    this->OnActivityJoinHandle =
        this->Instance->ActivityManager().OnActivityJoin.Connect([WeakThis = GetWeakThis(this)](const char *InSecret) {
            if (auto This = PinWeakThis(WeakThis))
            {
                FOnlineSessionSearchResult Result;

                FString InSecretStr = ANSI_TO_TCHAR(InSecret);
                FString TargetId = InSecretStr.Mid(0, InSecretStr.Len() - 4);

                if (InSecretStr.EndsWith("_psc"))
                {
                    UE_LOG(LogRedpointDiscord, Verbose, TEXT("Got activity join with EOS party ID: %s"), *TargetId);
                    Result.Session.SessionSettings.Set(
                        FName(TEXT("EOS_RealPartyId")),
                        TargetId,
                        EOnlineDataAdvertisementType::ViaOnlineService);
                }
                else if (InSecretStr.EndsWith("_ssc"))
                {
                    UE_LOG(LogRedpointDiscord, Verbose, TEXT("Got activity join with EOS session ID: %s"), *TargetId);
                    Result.Session.SessionSettings.Set(
                        FName(TEXT("EOS_RealSessionId")),
                        TargetId,
                        EOnlineDataAdvertisementType::ViaOnlineService);
                }
                else
                {
                    return;
                }

                Result.Session.OwningUserId = MakeShared<FUniqueNetIdRedpointDiscord>(1);
                Result.Session.SessionInfo =
                    MakeShared<FOnlineSessionInfoRedpointDiscord>(MakeShared<FUniqueNetIdRedpointDiscord>(1));
                This->TriggerOnSessionUserInviteAcceptedDelegates(
                    true,
                    0,
                    MakeShared<FUniqueNetIdRedpointDiscord>(1),
                    Result);
            }
        });
}

FOnlineSessionInterfaceRedpointDiscord::~FOnlineSessionInterfaceRedpointDiscord()
{
    this->Instance->ActivityManager().OnActivityJoinRequest.Disconnect(this->OnActivityJoinRequestHandle);
    this->Instance->ActivityManager().OnActivityJoin.Disconnect(this->OnActivityJoinHandle);
}

class FNamedOnlineSession *FOnlineSessionInterfaceRedpointDiscord::AddNamedSession(
    FName SessionName,
    const FOnlineSessionSettings &SessionSettings)
{
    FScopeLock ScopeLock(&this->SessionLock);
    return new (this->Sessions) FNamedOnlineSession(SessionName, SessionSettings);
}

class FNamedOnlineSession *FOnlineSessionInterfaceRedpointDiscord::AddNamedSession(
    FName SessionName,
    const FOnlineSession &Session)
{
    FScopeLock ScopeLock(&this->SessionLock);
    return new (this->Sessions) FNamedOnlineSession(SessionName, Session);
}

class FNamedOnlineSession *FOnlineSessionInterfaceRedpointDiscord::GetNamedSession(FName SessionName)
{
    FScopeLock ScopeLock(&this->SessionLock);
    for (int32 SearchIndex = 0; SearchIndex < this->Sessions.Num(); SearchIndex++)
    {
        if (this->Sessions[SearchIndex].SessionName == SessionName)
        {
            return &this->Sessions[SearchIndex];
        }
    }

    return nullptr;
}

void FOnlineSessionInterfaceRedpointDiscord::RemoveNamedSession(FName SessionName)
{
    FScopeLock ScopeLock(&this->SessionLock);
    for (int32 SearchIndex = 0; SearchIndex < this->Sessions.Num(); SearchIndex++)
    {
        if (this->Sessions[SearchIndex].SessionName == SessionName)
        {
            this->Sessions.RemoveAtSwap(SearchIndex);
            return;
        }
    }
}

bool FOnlineSessionInterfaceRedpointDiscord::CreateSession(
    const FUniqueNetId &HostingPlayerId,
    FName SessionName,
    const FOnlineSessionSettings &NewSessionSettings)
{
    checkf(
        SessionName.ToString().StartsWith(TEXT("SyntheticParty_")) ||
            SessionName.ToString().StartsWith(TEXT("SyntheticSession_")),
        TEXT("The Discord session implementation is not for general use. It is only implemented to support synthetic "
             "parties."));

    FNamedOnlineSession *Session = this->GetNamedSession(SessionName);
    if (Session == nullptr)
    {
        Session = AddNamedSession(SessionName, NewSessionSettings);
        Session->SessionState = EOnlineSessionState::Creating;

        FString GameName;
        if (!GConfig->GetString(DISCORD_CONFIG_SECTION, TEXT("ApplicationName"), GameName, GEngineIni))
        {
            GameName = TEXT("Game Name Not Set");
        }
        FTCHARToUTF8 GameNameUTF8 = FTCHARToUTF8(*GameName);

        FString RealId, IdSuffix, SecretSuffix, State;
        if (NewSessionSettings.Settings.Contains(TEXT("EOS_RealPartyId")))
        {
            NewSessionSettings.Settings[TEXT("EOS_RealPartyId")].Data.GetValue(RealId);
            IdSuffix = "pid";
            SecretSuffix = "psc";
            State = "In Party";
        }
        else
        {
            NewSessionSettings.Settings[TEXT("EOS_RealSessionId")].Data.GetValue(RealId);
            IdSuffix = "sid";
            SecretSuffix = "ssc";
            State = "In Game";
        }
        auto IdStr = StringCast<ANSICHAR>(*FString::Printf(TEXT("%s_%s"), *RealId, *IdSuffix));
        auto JoinSecretStr = StringCast<ANSICHAR>(*FString::Printf(TEXT("%s_%s"), *RealId, *SecretSuffix));
        auto StateStr = StringCast<ANSICHAR>(*State);

        discord::Activity Activity = {};
        Activity.SetType(discord::ActivityType::Playing);
        Activity.SetName(GameNameUTF8.Get());
        Activity.SetState(StateStr.Get());
        Activity.SetDetails("");
        Activity.GetParty().SetId(IdStr.Get());
        Activity.GetParty().GetSize().SetCurrentSize(1);
        Activity.GetParty().GetSize().SetMaxSize(4);
        Activity.GetSecrets().SetJoin(JoinSecretStr.Get());
        Activity.GetSecrets().SetMatch("");
        Activity.GetSecrets().SetSpectate("");
        Activity.GetAssets().SetLargeImage("");
        Activity.GetAssets().SetLargeText(StateStr.Get());
        Activity.GetAssets().SetSmallImage("");
        Activity.GetAssets().SetSmallText(StateStr.Get());
        Activity.GetTimestamps().SetStart(0);
        Activity.GetTimestamps().SetEnd(0);

        this->Instance->ActivityManager().UpdateActivity(
            Activity,
            [WeakThis = GetWeakThis(this), SessionName, Session](discord::Result Result) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    if (Result != discord::Result::Ok)
                    {
                        This->RemoveNamedSession(SessionName);
                    }
                    else
                    {
                        Session->SessionState = EOnlineSessionState::Pending;
                    }

                    This->TriggerOnCreateSessionCompleteDelegates(SessionName, Result == discord::Result::Ok);
                }
            });
        return true;
    }
    else
    {
        UE_LOG(
            LogRedpointDiscord,
            Error,
            TEXT("CreateSession: Failed because a session with the name %s already exists."),
            *SessionName.ToString());
        return false;
    }
}

bool FOnlineSessionInterfaceRedpointDiscord::DestroySession(
    FName SessionName,
    const FOnDestroySessionCompleteDelegate &CompletionDelegate)
{
    checkf(
        SessionName.ToString().StartsWith(TEXT("SyntheticParty_")) ||
            SessionName.ToString().StartsWith(TEXT("SyntheticSession_")),
        TEXT("The Discord session implementation is not for general use. It is only implemented to support synthetic "
             "parties."));

    FNamedOnlineSession *Session = this->GetNamedSession(SessionName);
    if (Session == nullptr)
    {
        UE_LOG(LogRedpointDiscord, Error, TEXT("DestroySession: Called with non-existant session."));
        return false;
    }

    FString GameName;
    if (!GConfig->GetString(DISCORD_CONFIG_SECTION, TEXT("ApplicationName"), GameName, GEngineIni))
    {
        GameName = TEXT("Game Name Not Set");
    }
    FTCHARToUTF8 GameNameUTF8 = FTCHARToUTF8(*GameName);

    discord::Activity Activity = {};
    Activity.SetType(discord::ActivityType::Playing);
    Activity.SetName(GameNameUTF8.Get());
    Activity.SetState("In Game");
    Activity.SetDetails("");
    Activity.GetParty().SetId("");
    Activity.GetParty().GetSize().SetCurrentSize(0);
    Activity.GetParty().GetSize().SetMaxSize(0);
    Activity.GetSecrets().SetJoin("");
    Activity.GetSecrets().SetMatch("");
    Activity.GetSecrets().SetSpectate("");
    Activity.GetAssets().SetLargeImage("");
    Activity.GetAssets().SetLargeText("In Game");
    Activity.GetAssets().SetSmallImage("");
    Activity.GetAssets().SetSmallText("In Game");
    Activity.GetTimestamps().SetStart(0);
    Activity.GetTimestamps().SetEnd(0);

    this->Instance->ActivityManager().UpdateActivity(
        Activity,
        [WeakThis = GetWeakThis(this), SessionName, CompletionDelegate](discord::Result Result) {
            if (auto This = PinWeakThis(WeakThis))
            {
                This->RemoveNamedSession(SessionName);
                This->TriggerOnDestroySessionCompleteDelegates(SessionName, Result == discord::Result::Ok);
                CompletionDelegate.ExecuteIfBound(SessionName, Result == discord::Result::Ok);
            }
        });
    return true;
}

bool FOnlineSessionInterfaceRedpointDiscord::SendSessionInviteToFriend(
    const FUniqueNetId &LocalUserId,
    FName SessionName,
    const FUniqueNetId &Friend)
{
    checkf(
        SessionName.ToString().StartsWith(TEXT("SyntheticParty_")) ||
            SessionName.ToString().StartsWith(TEXT("SyntheticSession_")),
        TEXT("The Discord session implementation is not for general use. It is only implemented to support synthetic "
             "parties."));

    FNamedOnlineSession *Session = this->GetNamedSession(SessionName);
    if (Session == nullptr)
    {
        UE_LOG(LogRedpointDiscord, Error, TEXT("SendSessionInviteToFriend: Called with non-existant session."));
        return false;
    }

    TSharedRef<const FUniqueNetIdRedpointDiscord> FriendDiscord =
        StaticCastSharedRef<const FUniqueNetIdRedpointDiscord>(Friend.AsShared());

    FString InvitationText;
    if (!GConfig->GetString(DISCORD_CONFIG_SECTION, TEXT("InvitationText"), InvitationText, GEngineIni))
    {
        InvitationText = TEXT("Come play!");
    }
    FTCHARToUTF8 InvitationTextUTF8 = FTCHARToUTF8(*InvitationText);

    this->Instance->ActivityManager().SendInvite(
        FriendDiscord->GetUserId(),
        discord::ActivityActionType::Join,
        InvitationTextUTF8.Get(),
        [WeakThis = GetWeakThis(this), SessionName](discord::Result Result) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Result != discord::Result::Ok)
                {
                    UE_LOG(
                        LogRedpointDiscord,
                        Warning,
                        TEXT("SendSessionInviteToFriend: Unable to send invite across Discord."));
                }
            }
        });
    return true;
}

/***** Nothing below this line is implemented. *****/

TSharedPtr<const FUniqueNetId> FOnlineSessionInterfaceRedpointDiscord::CreateSessionIdFromString(
    const FString &SessionIdStr)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::CreateSessionIdFromString is not implemented."));
    return nullptr;
}

bool FOnlineSessionInterfaceRedpointDiscord::HasPresenceSession()
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::HasPresenceSession is not implemented."));
    return false;
}

EOnlineSessionState::Type FOnlineSessionInterfaceRedpointDiscord::GetSessionState(FName SessionName) const
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::GetSessionState is not implemented."));
    return EOnlineSessionState::NoSession;
}

bool FOnlineSessionInterfaceRedpointDiscord::CreateSession(
    int32 HostingPlayerNum,
    FName SessionName,
    const FOnlineSessionSettings &NewSessionSettings)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::CreateSession is not implemented (HostingPlayerNum)."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::StartSession(FName SessionName)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::StartSession is not implemented."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::UpdateSession(
    FName SessionName,
    FOnlineSessionSettings &UpdatedSessionSettings,
    bool bShouldRefreshOnlineData)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::UpdateSession is not implemented."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::EndSession(FName SessionName)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::EndSession is not implemented."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::IsPlayerInSession(FName SessionName, const FUniqueNetId &UniqueId)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::IsPlayerInSession is not implemented."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::StartMatchmaking(
    const TArray<TSharedRef<const FUniqueNetId>> &LocalPlayers,
    FName SessionName,
    const FOnlineSessionSettings &NewSessionSettings,
    TSharedRef<FOnlineSessionSearch> &SearchSettings)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::StartMatchmaking is not implemented."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::CancelMatchmaking(int32 SearchingPlayerNum, FName SessionName)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::CancelMatchmaking is not implemented."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::CancelMatchmaking(const FUniqueNetId &SearchingPlayerId, FName SessionName)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::CancelMatchmaking is not implemented."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::FindSessions(
    int32 SearchingPlayerNum,
    const TSharedRef<FOnlineSessionSearch> &SearchSettings)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::FindSessions is not implemented."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::FindSessions(
    const FUniqueNetId &SearchingPlayerId,
    const TSharedRef<FOnlineSessionSearch> &SearchSettings)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::FindSessions is not implemented."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::FindSessionById(
    const FUniqueNetId &SearchingUserId,
    const FUniqueNetId &SessionId,
    const FUniqueNetId &FriendId,
    const FOnSingleSessionResultCompleteDelegate &CompletionDelegate)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::FindSessionById is not implemented."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::CancelFindSessions()
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::CancelFindSessions is not implemented."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::PingSearchResults(const FOnlineSessionSearchResult &SearchResult)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::PingSearchResults is not implemented."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::JoinSession(
    int32 LocalUserNum,
    FName SessionName,
    const FOnlineSessionSearchResult &DesiredSession)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::JoinSession is not implemented."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::JoinSession(
    const FUniqueNetId &LocalUserId,
    FName SessionName,
    const FOnlineSessionSearchResult &DesiredSession)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::JoinSession is not implemented."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::FindFriendSession(int32 LocalUserNum, const FUniqueNetId &Friend)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::FindFriendSession is not implemented."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::FindFriendSession(
    const FUniqueNetId &LocalUserId,
    const FUniqueNetId &Friend)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::FindFriendSession is not implemented."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::FindFriendSession(
    const FUniqueNetId &LocalUserId,
    const TArray<TSharedRef<const FUniqueNetId>> &FriendList)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::FindFriendSession is not implemented."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::SendSessionInviteToFriend(
    int32 LocalUserNum,
    FName SessionName,
    const FUniqueNetId &Friend)
{
    checkf(
        false,
        TEXT("FOnlineSessionInterfaceRedpointDiscord::SendSessionInviteToFriend is not implemented (LocalUserNum)."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::SendSessionInviteToFriends(
    int32 LocalUserNum,
    FName SessionName,
    const TArray<TSharedRef<const FUniqueNetId>> &Friends)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::SendSessionInviteToFriends is not implemented."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::SendSessionInviteToFriends(
    const FUniqueNetId &LocalUserId,
    FName SessionName,
    const TArray<TSharedRef<const FUniqueNetId>> &Friends)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::SendSessionInviteToFriends is not implemented."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::GetResolvedConnectString(
    FName SessionName,
    FString &ConnectInfo,
    FName PortType)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::GetResolvedConnectString is not implemented."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::GetResolvedConnectString(
    const class FOnlineSessionSearchResult &SearchResult,
    FName PortType,
    FString &ConnectInfo)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::GetResolvedConnectString is not implemented."));
    return false;
}

FOnlineSessionSettings *FOnlineSessionInterfaceRedpointDiscord::GetSessionSettings(FName SessionName)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::GetSessionSettings is not implemented."));
    return nullptr;
}

bool FOnlineSessionInterfaceRedpointDiscord::RegisterPlayer(
    FName SessionName,
    const FUniqueNetId &PlayerId,
    bool bWasInvited)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::RegisterPlayer is not implemented."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::RegisterPlayers(
    FName SessionName,
    const TArray<TSharedRef<const FUniqueNetId>> &Players,
    bool bWasInvited)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::RegisterPlayers is not implemented."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::UnregisterPlayer(FName SessionName, const FUniqueNetId &PlayerId)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::UnregisterPlayer is not implemented."));
    return false;
}

bool FOnlineSessionInterfaceRedpointDiscord::UnregisterPlayers(
    FName SessionName,
    const TArray<TSharedRef<const FUniqueNetId>> &Players)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::UnregisterPlayers is not implemented."));
    return false;
}

void FOnlineSessionInterfaceRedpointDiscord::RegisterLocalPlayer(
    const FUniqueNetId &PlayerId,
    FName SessionName,
    const FOnRegisterLocalPlayerCompleteDelegate &Delegate)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::RegisterLocalPlayer is not implemented."));
}

void FOnlineSessionInterfaceRedpointDiscord::UnregisterLocalPlayer(
    const FUniqueNetId &PlayerId,
    FName SessionName,
    const FOnUnregisterLocalPlayerCompleteDelegate &Delegate)
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::UnregisterLocalPlayer is not implemented."));
}

int32 FOnlineSessionInterfaceRedpointDiscord::GetNumSessions()
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::GetNumSessions is not implemented."));
    return 0;
}

void FOnlineSessionInterfaceRedpointDiscord::DumpSessionState()
{
    checkf(false, TEXT("FOnlineSessionInterfaceRedpointDiscord::DumpSessionState is not implemented."));
}

#endif