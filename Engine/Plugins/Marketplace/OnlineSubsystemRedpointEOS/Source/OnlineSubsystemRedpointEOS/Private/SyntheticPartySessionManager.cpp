// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/SyntheticPartySessionManager.h"

#include "Containers/Ticker.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "OnlineSubsystemRedpointEOS/Shared/DelegatedSubsystems.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSDefines.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSErrorConv.h"
#include "OnlineSubsystemRedpointEOS/Shared/MultiOperation.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineIdentityInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlinePartyInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSessionInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "Templates/SharedPointer.h"

EOS_ENABLE_STRICT_WARNINGS

FSyntheticPartySessionManager::FSyntheticPartySessionManager(
    FName InInstanceName,
    TWeakPtr<FOnlinePartySystemEOS, ESPMode::ThreadSafe> InEOSPartySystem,
    TWeakPtr<FOnlineSessionInterfaceEOS, ESPMode::ThreadSafe> InEOSSession,
    TWeakPtr<FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> InEOSIdentitySystem,
    const TSharedRef<class IEOSRuntimePlatform> &InRuntimePlatform,
    const TSharedRef<class FEOSConfig> &InConfig)
    : RuntimePlatform(InRuntimePlatform)
    , Config(InConfig)
    , EOSPartySystem(MoveTemp(InEOSPartySystem))
    , EOSSession(MoveTemp(InEOSSession))
    , EOSIdentitySystem(MoveTemp(InEOSIdentitySystem))
    , WrappedSubsystems()
    , SessionUserInviteAcceptedDelegateHandles()
    , HandlesPendingRemoval()
{
    struct W
    {
        TWeakPtr<IOnlineIdentity, ESPMode::ThreadSafe> Identity;
        TWeakPtr<IOnlineSession, ESPMode::ThreadSafe> Session;
    };
    for (const auto &KV : FDelegatedSubsystems::GetDelegatedSubsystems<W>(
             InConfig,
             InInstanceName,
             [](FDelegatedSubsystems::ISubsystemContext &Ctx) -> TSharedPtr<W> {
                 auto Identity = Ctx.GetDelegatedInterface<IOnlineIdentity>([](IOnlineSubsystem *OSS) {
                     return OSS->GetIdentityInterface();
                 });
                 auto Session = Ctx.GetDelegatedInterface<IOnlineSession>([](IOnlineSubsystem *OSS) {
                     return OSS->GetSessionInterface();
                 });
                 if (Identity.IsValid() && Session.IsValid())
                 {
                     return MakeShared<W>(W{Identity, Session});
                 }
                 return nullptr;
             }))
    {
        this->WrappedSubsystems.Add({KV.Key, KV.Value->Identity, KV.Value->Session});
    }
}

void FSyntheticPartySessionManager::RegisterEvents()
{
    for (const auto &KV : this->WrappedSubsystems)
    {
        if (auto Session = KV.Session.Pin())
        {
            this->SessionUserInviteAcceptedDelegateHandles.Add(
                KV.Session,
                Session->AddOnSessionUserInviteAcceptedDelegate_Handle(
                    FOnSessionUserInviteAcceptedDelegate::CreateLambda([WeakThis = GetWeakThis(this)](
                                                                           const bool bWasSuccessful,
                                                                           const int32 ControllerId,
                                                                           const TSharedPtr<const FUniqueNetId> &UserId,
                                                                           const FOnlineSessionSearchResult
                                                                               &InviteResult) {
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            UE_LOG(LogEOS, Verbose, TEXT("Received invite from synthetic party"));

                            if (bWasSuccessful && InviteResult.IsValid())
                            {
                                FString RealId;
                                if (InviteResult.Session.SessionSettings.Get<FString>("EOS_RealPartyId", RealId))
                                {
                                    This->HandlePartyInviteAccept(ControllerId, RealId);
                                }
                                else if (InviteResult.Session.SessionSettings.Get<FString>("EOS_RealSessionId", RealId))
                                {
                                    This->HandleSessionInviteAccept(ControllerId, RealId);
                                }
                                else if (InviteResult.Session.SessionSettings.Get<FString>("EOS_RealLobbyId", RealId))
                                {
                                    // Backwards compatibility.
                                    UE_LOG(
                                        LogEOS,
                                        Warning,
                                        TEXT("Different clients are running different versions of the EOS plugin. "
                                             "Please ensure you have upgraded all clients."));
                                    This->HandlePartyInviteAccept(ControllerId, RealId);
                                }
                                else
                                {
                                    UE_LOG(
                                        LogEOS,
                                        Error,
                                        TEXT("Received invite from synthetic party, but it didn't have "
                                             "EOS_RealLobbyId"));
                                }
                            }
                            else
                            {
                                UE_LOG(
                                    LogEOS,
                                    Error,
                                    TEXT("Received invite from synthetic party, but it wasn't successful"));
                            }
                        }
                    })));
        }
    }
}

void FSyntheticPartySessionManager::HandlePartyInviteAccept(int32 ControllerId, const FString &EOSPartyId)
{
    auto EOSParties = this->EOSPartySystem.Pin();
    auto EOSIdentity = this->EOSIdentitySystem.Pin();
    if (EOSParties && EOSIdentity)
    {
        if (EOSIdentity->GetLoginStatus(ControllerId) != ELoginStatus::LoggedIn)
        {
            // Routing of accepted invites in the editor is a bit jank, since wrapped subsystems might be singletons. If
            // they are, they'll cause both the PIE and global DefaultInstance to receive the event, and in that case
            // you'll get the message below forever (since DefaultInstance doesn't get logged into). In the editor, just
            // ignore invites if the local user isn't logged in.
#if !WITH_EDITOR
            // The user might not be authenticated yet, we need to try the invite again later.
            UE_LOG(
                LogEOS,
                Warning,
                TEXT("Received invite from synthetic party, but identity not ready yet, scheduling for later"));
            FUTicker::GetCoreTicker().AddTicker(
                FTickerDelegate::CreateLambda(
                    [WeakThis = GetWeakThis(this), ControllerId, EOSPartyId](float DeltaTime) {
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            This->HandlePartyInviteAccept(ControllerId, EOSPartyId);
                        }
                        return false;
                    }),
                0.2);
#endif
            return;
        }

        auto IdentityByController = EOSIdentity->GetUniquePlayerId(ControllerId);

        EPartyJoinabilityConstraint Constraint = this->Config->GetPartyJoinabilityConstraint();
        TArray<TSharedRef<const FOnlinePartyId>> JoinedParties;
        if (!EOSParties->GetJoinedParties(*IdentityByController, JoinedParties))
        {
            UE_LOG(LogEOS, Error, TEXT("GetJoinedParties returned error while processing synthetic party invite"));
            return;
        }

        if (JoinedParties.Num() >= 1 && Constraint == EPartyJoinabilityConstraint::IgnoreInvitesIfAlreadyInParty)
        {
            // This user is already in a party, so they can't join another one via a synthetic invite. Trigger the
            // appropriate event so the game developer can tell the player the invite was ignored.
            UE_LOG(
                LogEOS,
                Verbose,
                TEXT("Ignoring synthetic party invite because the local user is already in at least one party and the "
                     "party constraint configuration is IgnoreInvitesIfAlreadyInParty."));
            EOSParties->TriggerOnPartyInviteResponseReceivedDelegates(
                *IdentityByController,
                *MakeShared<FOnlinePartyIdEOS>(TCHAR_TO_ANSI(*EOSPartyId)),
                *IdentityByController,
                EInvitationResponse::Rejected);
            return;
        }

        EOSParties->LookupPartyById(
            TCHAR_TO_ANSI(*EOSPartyId),
            StaticCastSharedPtr<const FUniqueNetIdEOS>(IdentityByController)->GetProductUserId(),
            [EOSParties, IdentityByController](EOS_HLobbyDetails LobbyHandle) {
                if (LobbyHandle == nullptr)
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("Received invite from synthetic party, but could not resolve lobby ID to lobby handle. "
                             "Make sure the party is publicly advertised."));
                    return;
                }

                auto JoinInfo = MakeShared<FOnlinePartyJoinInfoEOS>(LobbyHandle);
                EOSParties->JoinParty(
                    IdentityByController.ToSharedRef().Get(),
                    JoinInfo.Get(),
                    FOnJoinPartyComplete::CreateLambda([](const FUniqueNetId &LocalUserId,
                                                          const FOnlinePartyId &PartyId,
                                                          const EJoinPartyCompletionResult Result,
                                                          const int32 NotApprovedReason) {
                        UE_LOG(LogEOS, Verbose, TEXT("Join party from synthetic party: %d"), Result);
                    }));
            });
    }
    else
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Received invite from synthetic party, but EOS parties and identity were not available at the time"));
    }
}

void FSyntheticPartySessionManager::HandleSessionInviteAccept(int32 ControllerId, const FString &EOSSessionId)
{
    auto EOSSessions = this->EOSSession.Pin();
    auto EOSIdentity = this->EOSIdentitySystem.Pin();
    if (EOSSessions && EOSIdentity)
    {
        if (EOSIdentity->GetLoginStatus(ControllerId) != ELoginStatus::LoggedIn)
        {
            // Routing of accepted invites in the editor is a bit jank, since wrapped subsystems might be singletons. If
            // they are, they'll cause both the PIE and global DefaultInstance to receive the event, and in that case
            // you'll get the message below forever (since DefaultInstance doesn't get logged into). In the editor, just
            // ignore invites if the local user isn't logged in.
#if !WITH_EDITOR
            // The user might not be authenticated yet, we need to try the invite again later.
            UE_LOG(
                LogEOS,
                Warning,
                TEXT("Received invite from synthetic session, but identity not ready yet, scheduling for later"));
            FUTicker::GetCoreTicker().AddTicker(
                FTickerDelegate::CreateLambda(
                    [WeakThis = GetWeakThis(this), ControllerId, EOSSessionId](float DeltaTime) {
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            This->HandleSessionInviteAccept(ControllerId, EOSSessionId);
                        }
                        return false;
                    }),
                0.2);
#endif
            return;
        }

        auto IdentityByController = EOSIdentity->GetUniquePlayerId(ControllerId);

        EOSSessions->FindSessionById(
            *IdentityByController,
            *EOSSessions->CreateSessionIdFromString(EOSSessionId),
            *IdentityByController,
            FOnSingleSessionResultCompleteDelegate::CreateLambda([WeakThis = GetWeakThis(this), IdentityByController](
                                                                     int32 LocalUserNum,
                                                                     bool bWasSuccessful,
                                                                     const FOnlineSessionSearchResult &SessionResult) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    if (!bWasSuccessful)
                    {
                        UE_LOG(
                            LogEOS,
                            Error,
                            TEXT("Received invite from synthetic session, but could not resolve session ID to session "
                                 "handle. Make sure the session is still publicly advertised."));
                        return;
                    }

                    auto EOSSessions = This->EOSSession.Pin();
                    if (!EOSSessions.IsValid())
                    {
                        UE_LOG(
                            LogEOS,
                            Error,
                            TEXT("Sessions are no longer available when the invite was resolved, so ignoring."));
                        return;
                    }

                    // Tell the game that we accepted the invite. This mirrors the same behaviour that you get when
                    // accepting a session invite in the EOS overlay.
                    EOSSessions->TriggerOnSessionUserInviteAcceptedDelegates(
                        true,
                        LocalUserNum,
                        IdentityByController,
                        SessionResult);
                }
            }));
    }
    else
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Received invite from synthetic session, but EOS sessions and identity were not available at the "
                 "time"));
    }
}

FSyntheticPartySessionManager::~FSyntheticPartySessionManager()
{
    for (auto &KV : this->SessionUserInviteAcceptedDelegateHandles)
    {
        if (auto Session = KV.Key.Pin())
        {
            Session->ClearOnSessionUserInviteAcceptedDelegate_Handle(KV.Value);
        }
    }
    this->SessionUserInviteAcceptedDelegateHandles.Empty();
}

void FSyntheticPartySessionManager::CreateSyntheticParty(
    const TSharedPtr<const FOnlinePartyId> &PartyId,
    const FSyntheticPartySessionOnComplete &OnComplete)
{
    auto EOSPartyId = StaticCastSharedPtr<const FOnlinePartyIdEOS>(PartyId);

    FMultiOperation<FWrappedSubsystem, bool>::Run(
        this->WrappedSubsystems,
        [WeakThis = GetWeakThis(this),
         PartyId,
         EOSPartyId](const FWrappedSubsystem &Subsystem, const std::function<void(bool)> &OnDone) -> bool {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (auto Session = Subsystem.Session.Pin())
                {
                    if (auto Identity = Subsystem.Identity.Pin())
                    {
                        auto UserId = Identity->GetUniquePlayerId(0);
                        if (UserId.IsValid())
                        {
                            auto SessionName = FString::Printf(
                                TEXT("SyntheticParty_%s_%s"),
                                *Subsystem.SubsystemName.ToString(),
                                *PartyId->ToString());
                            auto CreateHandleName = FString::Printf(TEXT("%s_Create"), *SessionName);

                            for (const auto &Integration : This->RuntimePlatform->GetIntegrations())
                            {
                                Integration->MutateSyntheticPartySessionNameIfRequired(
                                    Subsystem.SubsystemName,
                                    PartyId,
                                    SessionName);
                            }

                            FOnlineSessionSettings SessionSettings;
                            SessionSettings.bAllowJoinViaPresence = true;
                            SessionSettings.bAllowJoinViaPresenceFriendsOnly = false;
                            SessionSettings.bAllowInvites = true;
                            SessionSettings.bAllowJoinInProgress = true;
                            SessionSettings.bIsDedicated = false;
                            SessionSettings.bIsLANMatch = false;
                            SessionSettings.bShouldAdvertise = true;
                            SessionSettings.BuildUniqueId = 0;
                            SessionSettings.bUsesPresence = true;
                            SessionSettings.bUsesStats = false;
                            SessionSettings.NumPrivateConnections = 0;
                            // doesn't matter, other players never actually join the session.
                            SessionSettings.NumPublicConnections = 4;
#if defined(EOS_SESSION_HAS_LOBBY_OPTIONS)
                            SessionSettings.bAntiCheatProtected = false;
                            SessionSettings.bUseLobbiesIfAvailable = true;
                            SessionSettings.bUseLobbiesVoiceChatIfAvailable = false;
#endif
                            SessionSettings.Settings.Add(
                                TEXT("EOS_RealPartyId"),
                                FOnlineSessionSetting(
                                    ANSI_TO_TCHAR(EOSPartyId->GetLobbyId()),
                                    EOnlineDataAdvertisementType::ViaOnlineService));

                            auto Finalize = [WeakThis = GetWeakThis(This),
                                             OnDone,
                                             CreateHandleName,
                                             WeakSession = TWeakPtr<IOnlineSession, ESPMode::ThreadSafe>(Session)](
                                                bool bWasSuccessful) {
                                if (auto This = PinWeakThis(WeakThis))
                                {
                                    OnDone(bWasSuccessful);
                                    if (This->HandlesPendingRemoval.Contains(CreateHandleName))
                                    {
                                        auto CreateHandle = This->HandlesPendingRemoval[CreateHandleName];
                                        This->HandlesPendingRemoval.Remove(CreateHandleName);
                                        if (auto Session = WeakSession.Pin())
                                        {
                                            Session->ClearOnCreateSessionCompleteDelegate_Handle(CreateHandle);
                                        }
                                    }
                                }
                            };

                            This->HandlesPendingRemoval.Add(
                                CreateHandleName,
                                Session->AddOnCreateSessionCompleteDelegate_Handle(
                                    FOnCreateSessionCompleteDelegate::CreateLambda(
                                        [SessionName, Finalize](FName CreatedSessionName, bool bWasSuccessful) {
                                            if (CreatedSessionName.IsEqual(FName(*SessionName)))
                                            {
                                                if (!bWasSuccessful)
                                                {
                                                    UE_LOG(
                                                        LogEOS,
                                                        Error,
                                                        TEXT("Failed to create synthetic party: %s"),
                                                        *SessionName);
                                                }
                                                else
                                                {
                                                    UE_LOG(
                                                        LogEOS,
                                                        Verbose,
                                                        TEXT("Synthetic party has been set up: %s"),
                                                        *SessionName);
                                                }

                                                Finalize(bWasSuccessful);
                                            }
                                        })));

                            if (!Session
                                     ->CreateSession(UserId.ToSharedRef().Get(), FName(*SessionName), SessionSettings))
                            {
                                UE_LOG(
                                    LogEOS,
                                    Error,
                                    TEXT("Failed to create synthetic party (call failed): %s"),
                                    *SessionName);
                                Finalize(false);
                            }
                            return true;
                        }
                    }
                }
            }
            return false;
        },
        [OnComplete](const TArray<bool> &Values) {
            if (Values.Find(true) != -1)
            {
                OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::Success());
            }
            else
            {
                OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::UnexpectedError());
            }
        });
}

void FSyntheticPartySessionManager::DeleteSyntheticParty(
    const TSharedPtr<const FOnlinePartyId> &PartyId,
    const FSyntheticPartySessionOnComplete &OnComplete)
{
    auto EOSPartyId = StaticCastSharedPtr<const FOnlinePartyIdEOS>(PartyId);

    FMultiOperation<FWrappedSubsystem, bool>::Run(
        this->WrappedSubsystems,
        [PartyId, EOSPartyId](const FWrappedSubsystem &Subsystem, const std::function<void(bool)> &OnDone) -> bool {
            if (auto Session = Subsystem.Session.Pin())
            {
                if (auto Identity = Subsystem.Identity.Pin())
                {
                    auto UserId = Identity->GetUniquePlayerId(0);
                    if (UserId.IsValid())
                    {
                        auto SessionName = FName(*FString::Printf(
                            TEXT("SyntheticParty_%s_%s"),
                            *Subsystem.SubsystemName.ToString(),
                            *PartyId->ToString()));
                        if (Session->GetNamedSession(SessionName) != nullptr)
                        {
                            return Session->DestroySession(
                                SessionName,
                                FOnDestroySessionCompleteDelegate::CreateLambda(
                                    [SessionName, OnDone](FName DestroyedSessionName, bool bWasSuccessful) {
                                        if (DestroyedSessionName == SessionName)
                                        {
                                            if (!bWasSuccessful)
                                            {
                                                UE_LOG(
                                                    LogEOS,
                                                    Error,
                                                    TEXT("Failed to destroy synthetic party: %s"),
                                                    *SessionName.ToString());
                                            }

                                            OnDone(bWasSuccessful);
                                        }
                                    }));
                        }
                    }
                }
            }
            return false;
        },
        [OnComplete](const TArray<bool> &Values) {
            if (Values.Find(true) != -1)
            {
                OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::Success());
            }
            else
            {
                OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::UnexpectedError());
            }
        });
}

bool FSyntheticPartySessionManager::HasSyntheticParty(const TSharedPtr<const FOnlinePartyId> &PartyId)
{
    auto EOSPartyId = StaticCastSharedPtr<const FOnlinePartyIdEOS>(PartyId);

    bool bHasSyntheticParty = false;
    FMultiOperation<FWrappedSubsystem, bool>::Run(
        this->WrappedSubsystems,
        [PartyId,
         EOSPartyId,
         &bHasSyntheticParty](const FWrappedSubsystem &Subsystem, const std::function<void(bool)> &OnDone) -> bool {
            if (auto Session = Subsystem.Session.Pin())
            {
                if (auto Identity = Subsystem.Identity.Pin())
                {
                    auto UserId = Identity->GetUniquePlayerId(0);
                    if (UserId.IsValid())
                    {
                        auto SessionName = FName(*FString::Printf(
                            TEXT("SyntheticParty_%s_%s"),
                            *Subsystem.SubsystemName.ToString(),
                            *PartyId->ToString()));
                        if (Session->GetNamedSession(SessionName) != nullptr)
                        {
                            bHasSyntheticParty = true;
                        }
                    }
                }
            }
            return false;
        },
        [](const TArray<bool> &Values) {
        });
    return bHasSyntheticParty;
}

void FSyntheticPartySessionManager::CreateSyntheticSession(
    const TSharedPtr<const FUniqueNetIdEOSSession> &SessionId,
    const FSyntheticPartySessionOnComplete &OnComplete)
{
    FMultiOperation<FWrappedSubsystem, bool>::Run(
        this->WrappedSubsystems,
        [WeakThis = GetWeakThis(this),
         SessionId](const FWrappedSubsystem &Subsystem, const std::function<void(bool)> &OnDone) -> bool {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (auto Session = Subsystem.Session.Pin())
                {
                    if (auto Identity = Subsystem.Identity.Pin())
                    {
                        auto UserId = Identity->GetUniquePlayerId(0);
                        if (UserId.IsValid())
                        {
                            auto SessionName = FString::Printf(
                                TEXT("SyntheticSession_%s_%s"),
                                *Subsystem.SubsystemName.ToString(),
                                *SessionId->ToString());
                            auto CreateHandleName = FString::Printf(TEXT("%s_Create"), *SessionName);

                            for (const auto &Integration : This->RuntimePlatform->GetIntegrations())
                            {
                                Integration->MutateSyntheticSessionSessionNameIfRequired(
                                    Subsystem.SubsystemName,
                                    SessionId,
                                    SessionName);
                            }

                            FOnlineSessionSettings SessionSettings;
                            SessionSettings.bAllowJoinViaPresence = true;
                            SessionSettings.bAllowJoinViaPresenceFriendsOnly = false;
                            SessionSettings.bAllowInvites = true;
                            SessionSettings.bAllowJoinInProgress = true;
                            SessionSettings.bIsDedicated = false;
                            SessionSettings.bIsLANMatch = false;
                            SessionSettings.bShouldAdvertise = true;
                            SessionSettings.BuildUniqueId = 0;
                            SessionSettings.bUsesPresence = true;
                            SessionSettings.bUsesStats = false;
                            SessionSettings.NumPrivateConnections = 0;
                            // doesn't matter, other players never actually join the session.
                            SessionSettings.NumPublicConnections = 4;
#if defined(EOS_SESSION_HAS_LOBBY_OPTIONS)
                            SessionSettings.bAntiCheatProtected = false;
                            SessionSettings.bUseLobbiesIfAvailable = true;
                            SessionSettings.bUseLobbiesVoiceChatIfAvailable = false;
#endif
                            SessionSettings.Settings.Add(
                                TEXT("EOS_RealSessionId"),
                                FOnlineSessionSetting(
                                    SessionId->ToString(),
                                    EOnlineDataAdvertisementType::ViaOnlineService));

                            auto Finalize = [WeakThis = GetWeakThis(This),
                                             OnDone,
                                             CreateHandleName,
                                             WeakSession = TWeakPtr<IOnlineSession, ESPMode::ThreadSafe>(Session)](
                                                bool bWasSuccessful) {
                                if (auto This = PinWeakThis(WeakThis))
                                {
                                    OnDone(bWasSuccessful);
                                    if (This->HandlesPendingRemoval.Contains(CreateHandleName))
                                    {
                                        auto CreateHandle = This->HandlesPendingRemoval[CreateHandleName];
                                        This->HandlesPendingRemoval.Remove(CreateHandleName);
                                        if (auto Session = WeakSession.Pin())
                                        {
                                            Session->ClearOnCreateSessionCompleteDelegate_Handle(CreateHandle);
                                        }
                                    }
                                }
                            };

                            This->HandlesPendingRemoval.Add(
                                CreateHandleName,
                                Session->AddOnCreateSessionCompleteDelegate_Handle(
                                    FOnCreateSessionCompleteDelegate::CreateLambda(
                                        [SessionName, Finalize](FName CreatedSessionName, bool bWasSuccessful) {
                                            if (CreatedSessionName.IsEqual(FName(*SessionName)))
                                            {
                                                if (!bWasSuccessful)
                                                {
                                                    UE_LOG(
                                                        LogEOS,
                                                        Error,
                                                        TEXT("Failed to create synthetic session: %s"),
                                                        *SessionName);
                                                }
                                                else
                                                {
                                                    UE_LOG(
                                                        LogEOS,
                                                        Verbose,
                                                        TEXT("Synthetic session has been set up: %s"),
                                                        *SessionName);
                                                }

                                                Finalize(bWasSuccessful);
                                            }
                                        })));

                            if (!Session
                                     ->CreateSession(UserId.ToSharedRef().Get(), FName(*SessionName), SessionSettings))
                            {
                                UE_LOG(
                                    LogEOS,
                                    Error,
                                    TEXT("Failed to create synthetic session (call failed): %s"),
                                    *SessionName);
                                Finalize(false);
                            }
                            return true;
                        }
                    }
                }
            }
            return false;
        },
        [OnComplete](const TArray<bool> &Values) {
            if (Values.Find(true) != -1)
            {
                OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::Success());
            }
            else
            {
                OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::UnexpectedError());
            }
        });
}

void FSyntheticPartySessionManager::DeleteSyntheticSession(
    const TSharedPtr<const FUniqueNetIdEOSSession> &SessionId,
    const FSyntheticPartySessionOnComplete &OnComplete)
{
    FMultiOperation<FWrappedSubsystem, bool>::Run(
        this->WrappedSubsystems,
        [SessionId](const FWrappedSubsystem &Subsystem, const std::function<void(bool)> &OnDone) -> bool {
            if (auto Session = Subsystem.Session.Pin())
            {
                if (auto Identity = Subsystem.Identity.Pin())
                {
                    auto UserId = Identity->GetUniquePlayerId(0);
                    if (UserId.IsValid())
                    {
                        auto SessionName = FName(*FString::Printf(
                            TEXT("SyntheticSession_%s_%s"),
                            *Subsystem.SubsystemName.ToString(),
                            *SessionId->ToString()));
                        if (Session->GetNamedSession(SessionName) != nullptr)
                        {
                            return Session->DestroySession(
                                SessionName,
                                FOnDestroySessionCompleteDelegate::CreateLambda(
                                    [SessionName, OnDone](FName DestroyedSessionName, bool bWasSuccessful) {
                                        if (DestroyedSessionName == SessionName)
                                        {
                                            if (!bWasSuccessful)
                                            {
                                                UE_LOG(
                                                    LogEOS,
                                                    Error,
                                                    TEXT("Failed to destroy synthetic session: %s"),
                                                    *SessionName.ToString());
                                            }

                                            OnDone(bWasSuccessful);
                                        }
                                    }));
                        }
                    }
                }
            }
            return false;
        },
        [OnComplete](const TArray<bool> &Values) {
            if (Values.Find(true) != -1)
            {
                OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::Success());
            }
            else
            {
                OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::UnexpectedError());
            }
        });
}

bool FSyntheticPartySessionManager::HasSyntheticSession(const TSharedPtr<const FUniqueNetIdEOSSession> &SessionId)
{
    bool bHasSyntheticSession = false;
    FMultiOperation<FWrappedSubsystem, bool>::Run(
        this->WrappedSubsystems,
        [SessionId,
         &bHasSyntheticSession](const FWrappedSubsystem &Subsystem, const std::function<void(bool)> &OnDone) -> bool {
            if (auto Session = Subsystem.Session.Pin())
            {
                if (auto Identity = Subsystem.Identity.Pin())
                {
                    auto UserId = Identity->GetUniquePlayerId(0);
                    if (UserId.IsValid())
                    {
                        auto SessionName = FName(*FString::Printf(
                            TEXT("SyntheticSession_%s_%s"),
                            *Subsystem.SubsystemName.ToString(),
                            *SessionId->ToString()));
                        if (Session->GetNamedSession(SessionName) != nullptr)
                        {
                            bHasSyntheticSession = true;
                        }
                    }
                }
            }
            return false;
        },
        [](const TArray<bool> &Values) {
        });
    return bHasSyntheticSession;
}

bool FSyntheticPartySessionManager::HasSyntheticSession(
    const FName &SubsystemName,
    const TSharedPtr<const FUniqueNetIdEOSSession> &SessionId)
{
    for (const auto &Subsystem : this->WrappedSubsystems)
    {
        if (Subsystem.SubsystemName.IsEqual(SubsystemName))
        {
            if (auto Session = Subsystem.Session.Pin())
            {
                if (auto Identity = Subsystem.Identity.Pin())
                {
                    auto SessionName = FName(*FString::Printf(
                        TEXT("SyntheticSession_%s_%s"),
                        *Subsystem.SubsystemName.ToString(),
                        *SessionId->ToString()));
                    return Session->GetNamedSession(SessionName) != nullptr;
                }
            }
        }
    }
    return false;
}

FName FSyntheticPartySessionManager::GetSyntheticSessionNativeSessionName(
    const FName &SubsystemName,
    const TSharedPtr<const FUniqueNetIdEOSSession> &SessionId)
{
    return FName(*FString::Printf(TEXT("SyntheticSession_%s_%s"), *SubsystemName.ToString(), *SessionId->ToString()));
}

void FSyntheticPartySessionManager::SendInvitationToParty(
    int32 LocalUserNum,
    const TSharedPtr<const FOnlinePartyId> &PartyId,
    const TSharedPtr<const FUniqueNetId> &RecipientId,
    const FSyntheticPartySessionOnComplete &Delegate)
{
    if (auto IdentityEOS = PinWeakThis(this->EOSIdentitySystem))
    {
        for (const auto &Subsystem : this->WrappedSubsystems)
        {
            if (!Subsystem.SubsystemName.IsEqual(RecipientId->GetType()))
            {
                continue;
            }

            auto SessionName = FName(*FString::Printf(
                TEXT("SyntheticParty_%s_%s"),
                *Subsystem.SubsystemName.ToString(),
                *PartyId->ToString()));

            if (auto SessionInterface = Subsystem.Session.Pin())
            {
                if (!SessionInterface
                         ->SendSessionInviteToFriend(LocalUserNum, SessionName, RecipientId.ToSharedRef().Get()))
                {
                    auto CallUnexpectedlyFailedError = OnlineRedpointEOS::Errors::UnexpectedError(
                        TEXT("FSyntheticPartySessionManager::SendInvitationToParty"),
                        FString::Printf(
                            TEXT(
                                "The call to SendSessionInviteToFriend on the delegated subsystem (%s) unexpected "
                                "failed. Check the logs above to see if more specific error information is available."),
                            *Subsystem.SubsystemName.ToString()));
                    UE_LOG(LogEOS, Error, TEXT("%s"), *CallUnexpectedlyFailedError.ToLogString());
                    Delegate.ExecuteIfBound(CallUnexpectedlyFailedError);
                    return;
                }

                // Invitation was sent.
                Delegate.ExecuteIfBound(OnlineRedpointEOS::Errors::Success());
                return;
            }

            auto SessionInterfaceNoLongerAvailableError = OnlineRedpointEOS::Errors::Canceled(
                TEXT("FSyntheticPartySessionManager::SendInvitationToParty"),
                FString::Printf(
                    TEXT("The session interface on the delegated subsystem (%s) was no longer available at the time "
                         "the invitation was sent."),
                    *Subsystem.SubsystemName.ToString()));
            UE_LOG(LogEOS, Error, TEXT("%s"), *SessionInterfaceNoLongerAvailableError.ToLogString());
            Delegate.ExecuteIfBound(SessionInterfaceNoLongerAvailableError);
            return;
        }

        auto InvalidRecipientError = OnlineRedpointEOS::Errors::NotFound(
            TEXT("FSyntheticPartySessionManager::SendInvitationToParty"),
            FString::Printf(
                TEXT("There is no delegated subsystem configured for the recipient user ID's type (%s). Ensure you "
                     "have configured delegated subsystems correctly."),
                *RecipientId->GetType().ToString()));
        UE_LOG(LogEOS, Error, TEXT("%s"), *InvalidRecipientError.ToLogString());
        Delegate.ExecuteIfBound(InvalidRecipientError);
        return;
    }
}

void FSyntheticPartySessionManager::SendInvitationToSession(
    int32 LocalUserNum,
    const TSharedPtr<const FUniqueNetIdEOSSession> &SessionId,
    const TSharedPtr<const FUniqueNetId> &RecipientId,
    const FSyntheticPartySessionOnComplete &Delegate)
{
    for (const auto &Subsystem : this->WrappedSubsystems)
    {
        if (!Subsystem.SubsystemName.IsEqual(RecipientId->GetType()))
        {
            continue;
        }

        auto SessionName = FName(*FString::Printf(
            TEXT("SyntheticSession_%s_%s"),
            *Subsystem.SubsystemName.ToString(),
            *SessionId->ToString()));

        if (auto SessionInterface = Subsystem.Session.Pin())
        {
            if (!SessionInterface
                     ->SendSessionInviteToFriend(LocalUserNum, SessionName, RecipientId.ToSharedRef().Get()))
            {
                auto CallUnexpectedlyFailedError = OnlineRedpointEOS::Errors::UnexpectedError(
                    TEXT("FSyntheticPartySessionManager::SendInvitationToSession"),
                    FString::Printf(
                        TEXT("The call to SendSessionInviteToFriend on the delegated subsystem (%s) unexpected "
                             "failed. Check the logs above to see if more specific error information is available."),
                        *Subsystem.SubsystemName.ToString()));
                UE_LOG(LogEOS, Error, TEXT("%s"), *CallUnexpectedlyFailedError.ToLogString());
                Delegate.ExecuteIfBound(CallUnexpectedlyFailedError);
                return;
            }

            // Invitation was sent.
            Delegate.ExecuteIfBound(OnlineRedpointEOS::Errors::Success());
            return;
        }

        auto SessionInterfaceNoLongerAvailableError = OnlineRedpointEOS::Errors::Canceled(
            TEXT("FSyntheticPartySessionManager::SendInvitationToSession"),
            FString::Printf(
                TEXT("The session interface on the delegated subsystem (%s) was no longer available at the time "
                     "the invitation was sent."),
                *Subsystem.SubsystemName.ToString()));
        UE_LOG(LogEOS, Error, TEXT("%s"), *SessionInterfaceNoLongerAvailableError.ToLogString());
        Delegate.ExecuteIfBound(SessionInterfaceNoLongerAvailableError);
        return;
    }

    auto InvalidRecipientError = OnlineRedpointEOS::Errors::NotFound(
        TEXT("FSyntheticPartySessionManager::SendInvitationToSession"),
        FString::Printf(
            TEXT("There is no delegated subsystem configured for the recipient user ID's type (%s). Ensure you "
                 "have configured delegated subsystems correctly."),
            *RecipientId->GetType().ToString()));
    UE_LOG(LogEOS, Error, TEXT("%s"), *InvalidRecipientError.ToLogString());
    Delegate.ExecuteIfBound(InvalidRecipientError);
    return;
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION