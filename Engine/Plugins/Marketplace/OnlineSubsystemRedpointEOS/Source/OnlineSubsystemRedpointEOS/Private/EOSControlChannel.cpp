// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Private/EOSControlChannel.h"

#include "Engine/ActorChannel.h"
#include "GameFramework/PlayerController.h"
#include "Misc/Base64.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthBeaconPhase.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthBeaconPhaseContext.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthConnectionPhase.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthConnectionPhaseContext.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthLoginPhase.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthLoginPhaseContext.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthPhase.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthPhaseFailureCode.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthVerificationPhase.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthVerificationPhaseContext.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/NetworkHelpers.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/PhasesConnection/AutomaticEncryptionPhase.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/PhasesLogin/AntiCheatIntegrityPhase.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/PhasesLogin/AntiCheatProofPhase.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/PhasesVerification/IdTokenAuthPhase.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/PhasesVerification/LegacyCredentialAuthPhase.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/PhasesVerification/LegacyIdentityCheckPhase.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/PhasesVerification/P2PAddressCheckPhase.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/PhasesVerification/SanctionCheckPhase.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/Queues/QueuedBeaconEntry.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/Queues/QueuedEntry.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/Queues/QueuedLoginEntry.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/IInternetAddrEOS.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/ISocketEOS.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/ISocketSubsystemEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSControlStats.h"
#include "OnlineSubsystemUtils.h"
#if defined(UE_5_1_OR_LATER)
#include "Net/NetPing.h"
#endif

UEOSControlChannel::UEOSControlChannel(const FObjectInitializer &ObjectInitializer)
    : UControlChannel(ObjectInitializer)
{
    this->AuthConnectionContext.Reset();
    this->QueuedLogins.Reset();
    this->QueuedBeacons.Reset();

    IAutomaticEncryptionCommon::RegisterRoutes(this);
    FAntiCheatIntegrityPhase::RegisterRoutes(this);
    FAntiCheatProofPhase::RegisterRoutes(this);
    FIdTokenAuthPhase::RegisterRoutes(this);
    FLegacyCredentialAuthPhase::RegisterRoutes(this);
    FLegacyIdentityCheckPhase::RegisterRoutes(this);
    FP2PAddressCheckPhase::RegisterRoutes(this);
    FSanctionCheckPhase::RegisterRoutes(this);
}

void UEOSControlChannel::AddRoute(uint8 MessageType, const FAuthPhaseRoute &Route)
{
    this->Routes.Add(MessageType, Route);
}

TSharedPtr<FAuthConnectionPhaseContext> UEOSControlChannel::GetAuthConnectionPhaseContext()
{
    if (!this->AuthConnectionContext.IsValid())
    {
        EEOSNetDriverRole Role = FNetworkHelpers::GetRole(Connection);
        if (Role == EEOSNetDriverRole::ClientConnectedToDedicatedServer ||
            Role == EEOSNetDriverRole::ClientConnectedToListenServer)
        {
            // Implicitly create the authentication context on clients, since
            // we don't set them up as we send the NMT_Hello outbound.
            this->AuthConnectionContext = MakeShared<FAuthConnectionPhaseContext>(this);
            TArray<TSharedRef<IAuthConnectionPhase>> Phases;
#if !defined(EOS_AUTOMATIC_ENCRYPTION_UNAVAILABLE)
            Phases.Add(MakeShared<FAutomaticEncryptionPhase>());
#else
            Phases.Add(MakeShared<FUnavailableAutomaticEncryptionPhase>());
#endif // #if !defined(EOS_AUTOMATIC_ENCRYPTION_UNAVAILABLE)
            this->AuthConnectionContext->RegisterPhasesForClientRouting(Phases);
            return this->AuthConnectionContext;
        }
    }

    return this->AuthConnectionContext;
}

TSharedPtr<FAuthVerificationPhaseContext> UEOSControlChannel::GetAuthVerificationPhaseContext(
    const FUniqueNetIdRepl &InRepl)
{
    if (!InRepl.IsValid() || !InRepl->IsValid())
    {
        return nullptr;
    }

    if (this->AuthVerificationContexts.Contains(*InRepl))
    {
        return this->AuthVerificationContexts[*InRepl];
    }

    EEOSNetDriverRole Role = FNetworkHelpers::GetRole(Connection);
    if (Role == EEOSNetDriverRole::ClientConnectedToDedicatedServer ||
        Role == EEOSNetDriverRole::ClientConnectedToListenServer)
    {
        // Implicitly create the verification context on clients, since
        // we don't set them up as we send the NMT_Login/NMT_BeaconJoin outbound.
        // We always register all phases, since the server drives what network
        // messages get sent to the clients.
        TSharedRef<FAuthVerificationPhaseContext> Context = MakeShared<FAuthVerificationPhaseContext>(
            this,
            StaticCastSharedRef<const FUniqueNetIdEOS>(InRepl.GetUniqueNetId().ToSharedRef()));
        this->AuthVerificationContexts.Add(*InRepl, Context);
        TArray<TSharedRef<IAuthVerificationPhase>> Phases;
        const FEOSConfig *Config = Context->GetConfig();
        Phases.Add(MakeShared<FIdTokenAuthPhase>());
        Phases.Add(MakeShared<FP2PAddressCheckPhase>());
        Phases.Add(MakeShared<FLegacyCredentialAuthPhase>());
        Phases.Add(MakeShared<FLegacyIdentityCheckPhase>());
#if EOS_VERSION_AT_LEAST(1, 11, 0)
        Phases.Add(MakeShared<FSanctionCheckPhase>());
#endif // #if EOS_VERSION_AT_LEAST(1, 11, 0)
        Context->RegisterPhasesForClientRouting(Phases);
        return this->AuthVerificationContexts[*InRepl];
    }

    return nullptr;
}

TSharedPtr<FAuthLoginPhaseContext> UEOSControlChannel::GetAuthLoginPhaseContext(const FUniqueNetIdRepl &InRepl)
{
    if (!InRepl.IsValid() || !InRepl->IsValid())
    {
        return nullptr;
    }

    if (this->QueuedLogins.Contains(*InRepl))
    {
        return this->QueuedLogins[*InRepl]->Context;
    }

    EEOSNetDriverRole Role = FNetworkHelpers::GetRole(Connection);
    if (Role == EEOSNetDriverRole::ClientConnectedToDedicatedServer ||
        Role == EEOSNetDriverRole::ClientConnectedToListenServer)
    {
        // Implicitly create the queued login on clients, since
        // we don't set them up as we send the NMT_Login outbound.
        // We always register all phases, since the server drives what network
        // messages get sent to the clients.
        TSharedRef<FAuthLoginPhaseContext> Context = MakeShared<FAuthLoginPhaseContext>(
            this,
            StaticCastSharedRef<const FUniqueNetIdEOS>(InRepl.GetUniqueNetId().ToSharedRef()));
        TArray<TSharedRef<IAuthLoginPhase>> Phases;
#if EOS_VERSION_AT_LEAST(1, 12, 0)
        Phases.Add(MakeShared<FAntiCheatProofPhase>());
        Phases.Add(MakeShared<FAntiCheatIntegrityPhase>());
#endif // #if EOS_VERSION_AT_LEAST(1, 12, 0)
        Context->RegisterPhasesForClientRouting(Phases);
        auto Entry = MakeShared<FQueuedLoginEntry>(TEXT(""), TEXT(""), InRepl, TEXT(""), this);
        Entry->SetContext(Context);
        Entry->Track();
        return Context;
    }

    return nullptr;
}

TSharedPtr<FAuthBeaconPhaseContext> UEOSControlChannel::GetAuthBeaconPhaseContext(
    const FUniqueNetIdRepl &InRepl,
    const FString &InBeaconName)
{
    if (!InRepl.IsValid() || !InRepl->IsValid())
    {
        return nullptr;
    }

    if (this->QueuedBeacons.Contains(*InRepl))
    {
        auto &Beacons = this->QueuedBeacons[*InRepl];
        if (Beacons.Contains(InBeaconName))
        {
            return Beacons[InBeaconName]->Context;
        }
    }

    EEOSNetDriverRole Role = FNetworkHelpers::GetRole(Connection);
    if (Role == EEOSNetDriverRole::ClientConnectedToDedicatedServer ||
        Role == EEOSNetDriverRole::ClientConnectedToListenServer)
    {
        // Implicitly create the queued beacon on clients, since
        // we don't set them up as we send the NMT_BeaconJoin outbound.
        // We always register all phases, since the server drives what network
        // messages get sent to the clients.
        TSharedRef<FAuthBeaconPhaseContext> Context = MakeShared<FAuthBeaconPhaseContext>(
            this,
            StaticCastSharedRef<const FUniqueNetIdEOS>(InRepl.GetUniqueNetId().ToSharedRef()),
            InBeaconName);
        TArray<TSharedRef<IAuthBeaconPhase>> Phases;
        Context->RegisterPhasesForClientRouting(Phases);
        auto Entry = MakeShared<FQueuedBeaconEntry>(InBeaconName, InRepl, this);
        Entry->SetContext(Context);
        Entry->Track();
        return Context;
    }

    return nullptr;
}

void UEOSControlChannel::On_NMT_Hello(const FOriginalParameters_NMT_Hello& Parameters)
{
    if (this->AuthConnectionContext.IsValid())
    {
        // We're already processing NMT_Hello.
        return;
    }

    TSharedRef<FAuthConnectionPhaseContext> Context = MakeShared<FAuthConnectionPhaseContext>(this);
    this->AuthConnectionContext = Context;
    const FEOSConfig *Config = Context->GetConfig();
    EEOSNetDriverRole Role = Context->GetRole();

    TArray<TSharedRef<IAuthConnectionPhase>> Phases;

    if (Role == EEOSNetDriverRole::DedicatedServer)
    {
        // GetEnableTrustedDedicatedServers already evaluates the network authentication
        // mode and forces itself to on under Recommended mode.
        if (Config->GetEnableTrustedDedicatedServers() &&
            Config->GetEnableAutomaticEncryptionOnTrustedDedicatedServers())
        {
            bool bAddAutomaticEncryption = true;

#if WITH_EDITOR
            // If we have automatic encryption turned on and we're in the editor, make sure we can read the
            // public/private keypair. If we can't, force automatic encryption off.
            bool bLoadedKey = false;
            hydro_sign_keypair server_key_pair = {};
            FString PublicKey = Config->GetDedicatedServerPublicKey();
            FString PrivateKey = Config->GetDedicatedServerPrivateKey();
            if (!PublicKey.IsEmpty() && !PrivateKey.IsEmpty())
            {
                TArray<uint8> PublicKeyBytes, PrivateKeyBytes;
                if (FBase64::Decode(PublicKey, PublicKeyBytes) && FBase64::Decode(PrivateKey, PrivateKeyBytes))
                {
                    if (PublicKeyBytes.Num() == sizeof(server_key_pair.pk) &&
                        PrivateKeyBytes.Num() == sizeof(server_key_pair.sk))
                    {
                        FMemory::Memcpy(server_key_pair.pk, PublicKeyBytes.GetData(), sizeof(server_key_pair.pk));
                        FMemory::Memcpy(server_key_pair.sk, PrivateKeyBytes.GetData(), sizeof(server_key_pair.sk));
                        bLoadedKey = true;
                    }
                }
            }
            if (!bLoadedKey)
            {
                UE_LOG(
                    LogEOS,
                    Warning,
                    TEXT("Automatic encryption and trusted dedicated servers are being turned off, because the "
                         "public/private keypair could not be read "
                         "and you're running an editor build. Ensure the keys are set correctly in your "
                         "configuration, "
                         "otherwise network connections will not work when the game is packaged."));
                bAddAutomaticEncryption = false;
            }
#endif // #if WITH_EDITOR

            if (bAddAutomaticEncryption)
            {
#if !defined(EOS_AUTOMATIC_ENCRYPTION_UNAVAILABLE)
                Phases.Add(MakeShared<FAutomaticEncryptionPhase>());
#else
                Phases.Add(MakeShared<FUnavailableAutomaticEncryptionPhase>());
#endif // #if !defined(EOS_AUTOMATIC_ENCRYPTION_UNAVAILABLE)
            }
        }
    }

    Context->OnCompleted().AddUObject(
        this,
        &UEOSControlChannel::Finalize_NMT_Hello, Parameters);
    Context->Start(Context, Phases);
}

void UEOSControlChannel::Finalize_NMT_Hello(
    EAuthPhaseFailureCode Result,
    const FString &ErrorMessage,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FOriginalParameters_NMT_Hello Parameters)
{
    FString EmptyEncryptionToken = TEXT("");
    FOutBunch Bunch;
    Bunch << Parameters.IsLittleEndian;
    Bunch << Parameters.RemoteNetworkVersion;
    Bunch << Parameters.EncryptionToken;
#if defined(UE_5_1_OR_LATER)
    Bunch << Parameters.NetworkFeatures;
#endif

    FInBunch InBunch(this->Connection, Bunch.GetData(), Bunch.GetNumBits());
    this->Connection->Driver->Notify->NotifyControlMessage(this->Connection, NMT_Hello, InBunch);

    // Keep the auth connection context if it succeeded.
    if (Result != EAuthPhaseFailureCode::Success)
    {
        this->AuthConnectionContext.Reset();
    }
}

#if EOS_VERSION_AT_LEAST(1, 12, 0)

void UEOSControlChannel::On_NMT_EOS_AntiCheatMessage(
    const FUniqueNetIdRepl &SourceUserId,
    const FUniqueNetIdRepl &TargetUserId,
    const TArray<uint8> &AntiCheatData)
{
    TSharedPtr<IAntiCheat> AntiCheat;
    TSharedPtr<FAntiCheatSession> AntiCheatSession;
    bool bIsOwnedByBeacon;
    FNetworkHelpers::GetAntiCheat(this->Connection, AntiCheat, AntiCheatSession, bIsOwnedByBeacon);
    if (bIsOwnedByBeacon)
    {
        // Beacons don't do Anti-Cheat.
        UE_LOG(LogEOS, Error, TEXT("Ignoring Anti-Cheat network message for beacon."));
        return;
    }
    if (!AntiCheat.IsValid() || !AntiCheatSession.IsValid())
    {
        // No anti-cheat session.
        UE_LOG(LogEOS, Error, TEXT("Received Anti-Cheat network message, but no active Anti-Cheat session."));
        return;
    }

    EEOSNetDriverRole Role = FNetworkHelpers::GetRole(this->Connection);
    if (Role == EEOSNetDriverRole::DedicatedServer || Role == EEOSNetDriverRole::ListenServer)
    {
        if (!SourceUserId.IsValid() || !this->VerificationDatabase.Contains(*SourceUserId) ||
            (this->VerificationDatabase[*SourceUserId] != EUserVerificationStatus::WaitingForAntiCheatIntegrity &&
             this->VerificationDatabase[*SourceUserId] != EUserVerificationStatus::Verified))
        {
            // Drop this packet, it's not coming from a verified user.
            UE_LOG(
                LogEOS,
                Error,
                TEXT("Dropping Anti-Cheat packet because it's not coming from a verified user or a user that is "
                     "establishing Anti-Cheat integrity. The packet came from %s."),
                *SourceUserId->ToString());
            return;
        }
    }
    else if (!this->bGotCachedEACSourceUserId)
    {
        if (Role == EEOSNetDriverRole::ClientConnectedToDedicatedServer)
        {
            // Reset to dedicated server ID.
            this->CachedEACSourceUserId = FUniqueNetIdEOS::DedicatedServerId();
            this->bGotCachedEACSourceUserId = true;
        }
        else if (Role == EEOSNetDriverRole::ClientConnectedToListenServer)
        {
            // Set it to the unique net ID of the server.
            auto LocalNetDriver = Cast<UEOSNetDriver>(this->Connection->Driver);
            if (!IsValid(LocalNetDriver))
            {
                // What?
                return;
            }

            auto SocketEOS = LocalNetDriver->Socket.Pin();
            if (!SocketEOS.IsValid())
            {
                return;
            }
            auto SocketSubsystem = LocalNetDriver->SocketSubsystem.Pin();
            if (!SocketSubsystem.IsValid())
            {
                return;
            }

            TSharedPtr<IInternetAddrEOS> PeerAddr =
                StaticCastSharedRef<IInternetAddrEOS>(SocketSubsystem->CreateInternetAddr());
            verifyf(SocketEOS->GetPeerAddress(*PeerAddr), TEXT("Peer address could not be read for P2P socket"));

            this->CachedEACSourceUserId = MakeShared<FUniqueNetIdEOS>(PeerAddr->GetUserId());
            this->bGotCachedEACSourceUserId = true;
        }
    }

    const FUniqueNetIdEOS *SourceUserIdEOS = nullptr;
    if (this->bGotCachedEACSourceUserId)
    {
        SourceUserIdEOS = this->CachedEACSourceUserId.Get();
    }
    else
    {
        SourceUserIdEOS = (const FUniqueNetIdEOS *)SourceUserId.GetUniqueNetId().Get();
    }

    if (!AntiCheat->ReceiveNetworkMessage(
            *AntiCheatSession,
            *SourceUserIdEOS,
            (const FUniqueNetIdEOS &)*TargetUserId.GetUniqueNetId(),
            AntiCheatData.GetData(),
            AntiCheatData.Num()))
    {
        UE_LOG(LogEOS, Error, TEXT("Anti-Cheat network message was not received at control channel level."));
    }
}

void UEOSControlChannel::OnAntiCheatPlayerAuthStatusChanged(
    const FUniqueNetIdEOS &TargetUserId,
    EOS_EAntiCheatCommonClientAuthStatus NewAuthStatus)
{
    if (this->QueuedLogins.Contains(TargetUserId))
    {
        // We make a copy of the array here, since the OnAntiCheatPlayerAuthStatusChanged call might modify the
        // authentication state.
        TArray<TSharedRef<IAuthLoginPhase>> Phases = this->QueuedLogins[TargetUserId]->Context->Phases;
        TSharedRef<FAuthLoginPhaseContext> Context = this->QueuedLogins[TargetUserId]->Context.ToSharedRef();
        for (const auto &Phase : Phases)
        {
            Phase->OnAntiCheatPlayerAuthStatusChanged(Context, NewAuthStatus);
        }
    }
}

void UEOSControlChannel::OnAntiCheatPlayerActionRequired(
    const FUniqueNetIdEOS &TargetUserId,
    EOS_EAntiCheatCommonClientAction ClientAction,
    EOS_EAntiCheatCommonClientActionReason ActionReasonCode,
    const FString &ActionReasonDetailsString)
{
    if (this->QueuedLogins.Contains(TargetUserId))
    {
        // We make a copy of the array here, since the OnAntiCheatPlayerActionRequired call might modify the
        // authentication state.
        TArray<TSharedRef<IAuthLoginPhase>> Phases = this->QueuedLogins[TargetUserId]->Context->Phases;
        TSharedRef<FAuthLoginPhaseContext> Context = this->QueuedLogins[TargetUserId]->Context.ToSharedRef();
        for (const auto &Phase : Phases)
        {
            Phase->OnAntiCheatPlayerActionRequired(Context, ClientAction, ActionReasonCode, ActionReasonDetailsString);
        }
    }
    else if (
        IsValid(this->Connection) && TargetUserId == this->Connection->PlayerId &&
        ClientAction == EOS_EAntiCheatCommonClientAction::EOS_ACCCA_RemovePlayer)
    {
        // Send the client a security warning message and close the connection.
        FString DetailsString = FString::Printf(
            TEXT("Disconnected from server due to Anti-Cheat error. Reason: '%s'. Details: '%s'."),
            *EOS_EAntiCheatCommonClientActionReason_ToString(ActionReasonCode),
            *ActionReasonDetailsString);
        if (FNetworkHelpers::GetRole(this->Connection) == EEOSNetDriverRole::DedicatedServer ||
            FNetworkHelpers::GetRole(this->Connection) == EEOSNetDriverRole::ListenServer)
        {
            // Clients understand NMT_SecurityViolation.
            FNetControlMessage<NMT_SecurityViolation>::Send(Connection, DetailsString);
        }
        else
        {
            FNetControlMessage<NMT_Failure>::Send(Connection, DetailsString);
        }
        Connection->FlushNet(true);
        Connection->Close();
    }
}

#endif // #if EOS_VERSION_AT_LEAST(1, 12, 0)

/**
 * Starts the authentication sequence. This *only* gets called on the server, so it doesn't set up the authentication
 * phases on clients. Refer to the implicit context creation in the Get*PhaseContext functions above.
 */
void UEOSControlChannel::StartAuthentication(
    const FUniqueNetIdRepl &IncomingUser,
    const TSharedRef<IQueuedEntry> &NetworkEntry,
    const FString &BeaconName,
    bool bIsBeacon)
{
    EEOSNetDriverRole Role = FNetworkHelpers::GetRole(Connection);
    checkf(
        Role == EEOSNetDriverRole::DedicatedServer || Role == EEOSNetDriverRole::ListenServer,
        TEXT("UEOSControlChannel::StartAuthentication can only be called on servers!"));

    if (!IncomingUser.IsValid() || IncomingUser.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        // Determine if network authentication is turned off. If it is, then we just passthrough the
        // connection.
        bool bNetworkAuthenticationEnabled = true;
        auto Driver = Cast<UEOSNetDriver>(Connection->Driver);
        if (IsValid(Driver))
        {
            auto ConnWorld = Driver->FindWorld();
            if (ConnWorld != nullptr)
            {
                FOnlineSubsystemEOS *OSS =
                    (FOnlineSubsystemEOS *)Online::GetSubsystem(ConnWorld, REDPOINT_EOS_SUBSYSTEM);
                {
                    if (OSS->GetConfig().GetNetworkAuthenticationMode() == ENetworkAuthenticationMode::Off)
                    {
                        bNetworkAuthenticationEnabled = false;
                    }
                }
            }
        }

        if (!bNetworkAuthenticationEnabled)
        {
            NetworkEntry->SendSuccess();
            return;
        }

#if WITH_EDITOR
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("The connecting user has not signed into EOS. Use the \"Login before play-in-editor\" option in the "
                 "toolbar if you want to have players sign into EOS before connecting to the game server. Permitting "
                 "this connection "
                 "only because you're running in the editor."));
        NetworkEntry->SendSuccess();
        return;
#else
        UE_LOG(LogEOS, Error, TEXT("Missing user ID during connection."));
        NetworkEntry->SendFailure(TEXT("Missing user ID during connection."));
        return;
#endif // #if WITH_EDITOR
    }

    if (NetworkEntry->Contains())
    {
        // We're already processing this entry.
        return;
    }

    TSharedPtr<IAuthPhaseContext> Context;
    std::function<void()> PhaseSetupAndStart;
    if (!bIsBeacon)
    {
        TSharedRef<FAuthLoginPhaseContext> LoginContext = MakeShared<FAuthLoginPhaseContext>(
            this,
            StaticCastSharedRef<const FUniqueNetIdEOS>(IncomingUser.GetUniqueNetId().ToSharedRef()));
        NetworkEntry->SetContext(StaticCastSharedRef<IAuthPhaseContext>(LoginContext));
        NetworkEntry->Track();
        Context = LoginContext;

        PhaseSetupAndStart = [this, LoginContext, NetworkEntry]() {
            const FEOSConfig *Config = LoginContext->GetConfig();
            EEOSNetDriverRole Role = LoginContext->GetRole();

            TArray<TSharedRef<IAuthLoginPhase>> Phases;
#if EOS_VERSION_AT_LEAST(1, 12, 0)
            if (Config->GetEnableAntiCheat())
            {
                Phases.Add(MakeShared<FAntiCheatProofPhase>());
                Phases.Add(MakeShared<FAntiCheatIntegrityPhase>());
            }
#endif // #if EOS_VERSION_AT_LEAST(1, 12, 0)
            LoginContext->OnCompleted().AddUObject(this, &UEOSControlChannel::OnAuthenticationComplete, NetworkEntry);
            LoginContext->Start(LoginContext, Phases);
        };
    }
    else
    {
        TSharedRef<FAuthBeaconPhaseContext> BeaconContext = MakeShared<FAuthBeaconPhaseContext>(
            this,
            StaticCastSharedRef<const FUniqueNetIdEOS>(IncomingUser.GetUniqueNetId().ToSharedRef()),
            BeaconName);
        NetworkEntry->SetContext(StaticCastSharedRef<IAuthPhaseContext>(BeaconContext));
        NetworkEntry->Track();
        Context = BeaconContext;

        PhaseSetupAndStart = [this, BeaconContext, NetworkEntry]() {
            TArray<TSharedRef<IAuthBeaconPhase>> Phases;
            // There are no beacon-specific phases at this time.
            BeaconContext->OnCompleted().AddUObject(this, &UEOSControlChannel::OnAuthenticationComplete, NetworkEntry);
            BeaconContext->Start(BeaconContext, Phases);
        };
    }

    if (this->VerificationDatabase.Contains(*IncomingUser) &&
        this->VerificationDatabase[*IncomingUser] != EUserVerificationStatus::Failed)
    {
        if (this->VerificationDatabase[*IncomingUser] == EUserVerificationStatus::Verified)
        {
            // We've already verified this connection is permitted to login
            // as this user, immediately start the login phases.
            PhaseSetupAndStart();
        }
        else
        {
            this->AuthVerificationContexts[*IncomingUser]->OnCompleted().AddWeakLambda(
                this,
                [Context, PhaseSetupAndStart](EAuthPhaseFailureCode FailureCode, const FString &ErrorMessage) {
                    if (FailureCode == EAuthPhaseFailureCode::Success)
                    {
                        if (Context->GetConfig()->GetNetworkAuthenticationMode() != ENetworkAuthenticationMode::Off)
                        {
                            // Ensure the connection is associated with a player ID before
                            // continuing.
                            checkf(
                                IsValid(Context->GetConnection()) && Context->GetConnection()->PlayerId.IsValid(),
                                TEXT("Verification phases must set the player ID of the connection."));
                        }

                        // Verification passed, continue login phases.
                        PhaseSetupAndStart();
                    }
                    else
                    {
                        // Verification failed, finish now.
                        Context->Finish(FailureCode);
                    }
                });
        }
    }
    else
    {
        // We don't have a verification phase running. Start one and tell it to forward to us when it's done.
        TSharedRef<FAuthVerificationPhaseContext> VerificationContext = MakeShared<FAuthVerificationPhaseContext>(
            this,
            StaticCastSharedRef<const FUniqueNetIdEOS>(IncomingUser.GetUniqueNetId().ToSharedRef()));

        this->VerificationDatabase.Add(*IncomingUser, EUserVerificationStatus::NotStarted);
        this->AuthVerificationContexts.Add(*IncomingUser, VerificationContext);

        const FEOSConfig *Config = Context->GetConfig();

        TArray<TSharedRef<IAuthVerificationPhase>> Phases;

        if (Config->GetNetworkAuthenticationMode() == ENetworkAuthenticationMode::IDToken)
        {
            if (Role == EEOSNetDriverRole::DedicatedServer)
            {
                Phases.Add(MakeShared<FIdTokenAuthPhase>());
            }
            else if (Role == EEOSNetDriverRole::ListenServer)
            {
                Phases.Add(MakeShared<FP2PAddressCheckPhase>());
            }
            else
            {
                checkf(
                    false,
                    TEXT("Network authentication mode was ID Token, but the role of the networking driver was neither "
                         "DedicatedServer nor ListenServer (was %d)."),
                    (int)Role);
            }
        }
        else if (Config->GetNetworkAuthenticationMode() == ENetworkAuthenticationMode::UserCredentials)
        {
            if (Role == EEOSNetDriverRole::DedicatedServer)
            {
                if (Config->GetEnableTrustedDedicatedServers() &&
                    Config->GetEnableAutomaticEncryptionOnTrustedDedicatedServers())
                {
                    Phases.Add(MakeShared<FLegacyCredentialAuthPhase>());
                }
                else
                {
                    checkf(
                        false,
                        TEXT("Network authentication mode was User Credentials, but the dedicated server isn't configured to actually verify the user identity."));
                }
            }
            else if (Role == EEOSNetDriverRole::ListenServer)
            {
                if (Config->GetEnableIdentityChecksOnListenServers())
                {
                    Phases.Add(MakeShared<FLegacyIdentityCheckPhase>());
                }
                else
                {
                    checkf(
                        false,
                        TEXT("Network authentication mode was User Credentials, but the listen server isn't "
                             "configured to check the user's identity."));
                }
            }
            else
            {
                checkf(
                    false,
                    TEXT("Network authentication mode was User Credentials, but the role of the networking driver was neither "
                         "DedicatedServer nor ListenServer (was %d)."),
                    (int)Role);
            }
        }

        // Configuration calls below already evaluate the network authentication
        // mode and adjust themselves appropriately for the networking mode.
#if EOS_VERSION_AT_LEAST(1, 11, 0)
        if (Config->GetEnableSanctionChecks())
        {
            Phases.Add(MakeShared<FSanctionCheckPhase>());
        }
#endif // #if EOS_VERSION_AT_LEAST(1, 11, 0)

        VerificationContext->OnCompleted().AddWeakLambda(
            this,
            [Context, PhaseSetupAndStart](EAuthPhaseFailureCode FailureCode, const FString &ErrorMessage) {
                if (FailureCode == EAuthPhaseFailureCode::Success)
                {
                    if (Context->GetConfig()->GetNetworkAuthenticationMode() != ENetworkAuthenticationMode::Off)
                    {
                        // Ensure the connection is associated with a player ID before
                        // continuing.
                        checkf(
                            IsValid(Context->GetConnection()) && Context->GetConnection()->PlayerId.IsValid(),
                            TEXT("Verification phases must set the player ID of the connection."));
                    }

                    // Verification passed, continue login phases.
                    PhaseSetupAndStart();
                }
                else
                {
                    // Verification failed, finish now.
                    Context->Finish(FailureCode);
                }
            });
        VerificationContext->Start(VerificationContext, Phases);
    }
}

void UEOSControlChannel::OnAuthenticationComplete(
    EAuthPhaseFailureCode Result,
    const FString &ErrorMessage,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<IQueuedEntry> NetworkEntry)
{
    if (Result == EAuthPhaseFailureCode::Success)
    {
        NetworkEntry->SendSuccess();
    }
    else
    {
        NetworkEntry->SendFailure(ErrorMessage);
    }
    NetworkEntry->Release();
}

void UEOSControlChannel::On_NMT_Login(
    FString ClientResponse,
    FString URLString,
    FUniqueNetIdRepl IncomingUser,
    FString OnlinePlatformNameString)
{
    TSharedRef<FQueuedLoginEntry> Entry =
        MakeShared<FQueuedLoginEntry>(ClientResponse, URLString, IncomingUser, OnlinePlatformNameString, this);
    StartAuthentication(IncomingUser, StaticCastSharedRef<IQueuedEntry>(Entry), TEXT(""), false);
}

void UEOSControlChannel::On_NMT_BeaconJoin(FString BeaconType, FUniqueNetIdRepl IncomingUser)
{
    TSharedRef<FQueuedBeaconEntry> Entry = MakeShared<FQueuedBeaconEntry>(BeaconType, IncomingUser, this);
    StartAuthentication(IncomingUser, StaticCastSharedRef<IQueuedEntry>(Entry), BeaconType, true);
}

void UEOSControlChannel::Init(UNetConnection *InConnection, int32 InChIndex, EChannelCreateFlags CreateFlags)
{
    this->bClientTrustsServer = false;
    this->bRegisteredForAntiCheat.Reset();
    this->VerificationDatabase.Reset();
    this->bGotCachedEACSourceUserId = false;
    this->CachedEACSourceUserId.Reset();
    this->AuthStatusChangedHandle.Reset();
    this->ActionRequiredHandle.Reset();

    UControlChannel::Init(InConnection, InChIndex, CreateFlags);

    TSharedPtr<IAntiCheat> AntiCheat;
    TSharedPtr<FAntiCheatSession> AntiCheatSession;
    bool bIsOwnedByBeacon;
    FNetworkHelpers::GetAntiCheat(InConnection, AntiCheat, AntiCheatSession, bIsOwnedByBeacon);

    if (!bIsOwnedByBeacon)
    {
        this->AuthStatusChangedHandle = AntiCheat->OnPlayerAuthStatusChanged.AddUObject(
            this,
            &UEOSControlChannel::OnAntiCheatPlayerAuthStatusChanged);
        this->ActionRequiredHandle =
            AntiCheat->OnPlayerActionRequired.AddUObject(this, &UEOSControlChannel::OnAntiCheatPlayerActionRequired);
    }
}

bool UEOSControlChannel::CleanUp(const bool bForDestroy, EChannelCloseReason CloseReason)
{
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    // note: CleanUp gets called from garbage collection; it's not valid to try and use
    // phases or contexts here because they use TSoftObjectPtr.
    TSharedPtr<IAntiCheat> AntiCheat;
    TSharedPtr<FAntiCheatSession> AntiCheatSession;
    bool bIsOwnedByBeacon;
    FNetworkHelpers::GetAntiCheat(Connection, AntiCheat, AntiCheatSession, bIsOwnedByBeacon);
    if (!bIsOwnedByBeacon)
    {
        for (const auto &KV : this->bRegisteredForAntiCheat)
        {
            if (KV.Value)
            {
                if (AntiCheat.IsValid() && AntiCheatSession.IsValid())
                {
                    if (!AntiCheat->UnregisterPlayer(
                            *AntiCheatSession,
                            *StaticCastSharedRef<const FUniqueNetIdEOS>(KV.Key)))
                    {
                        UE_LOG(LogEOS, Warning, TEXT("Anti-Cheat UnregisterPlayer call failed!"));
                    }
                }
                else
                {
                    UE_LOG(
                        LogEOS,
                        Warning,
                        TEXT("Unable to unregister player because Anti-Cheat or Anti-Cheat session is no longer "
                             "valid!"));
                }
            }
        }
    }
#endif // #if EOS_VERSION_AT_LEAST(1, 12, 0)

    return UControlChannel::CleanUp(bForDestroy, CloseReason);
}

/**
 * This implementation is copied from UControlChannel::ReceivedBunch, because we can't meaningfully intercept just
 * the NMT_Login message without copying all of the logic.
 */
void UEOSControlChannel::ReceivedBunch(FInBunch &Bunch)
{
#if defined(UE_5_1_OR_LATER)
    using namespace UE::Net;
#endif

    check(!Closing);

    // If this is a new client connection inspect the raw packet for endianess
    if (Connection && bNeedsEndianInspection && !CheckEndianess(Bunch))
    {
        // Send close bunch and shutdown this connection
        UE_LOG(
            LogNet,
            Warning,
            TEXT("UControlChannel::ReceivedBunch: NetConnection::Close() [%s] [%s] [%s] from CheckEndianess(). "
                 "FAILED. "
                 "Closing connection."),
            Connection->Driver ? *Connection->Driver->NetDriverName.ToString() : TEXT("NULL"),
            Connection->PlayerController ? *Connection->PlayerController->GetName() : TEXT("NoPC"),
            Connection->OwningActor ? *Connection->OwningActor->GetName() : TEXT("No Owner"));

        Connection->Close();
        return;
    }

    // Process the packet
#if defined(UE_5_0_OR_LATER)
    while (!Bunch.AtEnd() && Connection != NULL &&
           Connection->GetConnectionState() !=
               USOCK_Closed) // if the connection got closed, we don't care about the rest
#else
    while (!Bunch.AtEnd() && Connection != NULL &&
           Connection->State != USOCK_Closed) // if the connection got closed, we don't care about the rest
#endif // #if defined(UE_5_0_OR_LATER)
    {
        uint8 MessageType = 0;
        Bunch << MessageType;
        if (Bunch.IsError())
        {
            break;
        }
        int32 Pos = Bunch.GetPosBits();

        /*
        UE_NET_TRACE_DYNAMIC_NAME_SCOPE(
            FNetControlMessageInfo::GetName(MessageType),
            Bunch,
            Connection ? Connection->GetInTraceCollector() : nullptr,
            ENetTraceVerbosity::Trace);
        */

        // we handle Actor channel failure notifications ourselves
        if (MessageType == NMT_ActorChannelFailure)
        {
            if (Connection->Driver->ServerConnection == NULL)
            {
                int32 ChannelIndex;

                if (FNetControlMessage<NMT_ActorChannelFailure>::Receive(Bunch, ChannelIndex))
                {
                    UE_LOG(
                        LogNet,
                        Log,
                        TEXT("Server connection received: %s %d %s"),
                        FNetControlMessageInfo::GetName(MessageType),
                        ChannelIndex,
                        *Describe());

                    // Check if Channel index provided by client is valid and within range of channel on server
                    if (ChannelIndex >= 0 && ChannelIndex < Connection->Channels.Num())
                    {
                        // Get the actor channel that the client provided as having failed
                        UActorChannel *ActorChan = Cast<UActorChannel>(Connection->Channels[ChannelIndex]);

                        UE_CLOG(
                            ActorChan != nullptr,
                            LogNet,
                            Log,
                            TEXT("Actor channel failed: %s"),
                            *ActorChan->Describe());

                        // The channel and the actor attached to the channel exists on the server
                        if (ActorChan != nullptr && ActorChan->Actor != nullptr)
                        {
                            // The channel that failed is the player controller thus the connection is broken
                            if (ActorChan->Actor == Connection->PlayerController)
                            {
                                UE_LOG(
                                    LogNet,
                                    Warning,
                                    TEXT("UControlChannel::ReceivedBunch: NetConnection::Close() [%s] [%s] [%s] "
                                         "from "
                                         "failed to initialize the PlayerController channel. Closing connection."),
                                    Connection->Driver ? *Connection->Driver->NetDriverName.ToString() : TEXT("NULL"),
                                    Connection->PlayerController ? *Connection->PlayerController->GetName()
                                                                 : TEXT("NoPC"),
                                    Connection->OwningActor ? *Connection->OwningActor->GetName() : TEXT("No Owner"));

                                Connection->Close();
                            }
                            // The client has a PlayerController connection, report the actor failure to
                            // PlayerController
                            else if (Connection->PlayerController != nullptr)
                            {
                                Connection->PlayerController->NotifyActorChannelFailure(ActorChan);
                            }
                            // The PlayerController connection doesn't exist for the client
                            // but the client is reporting an actor channel failure that isn't the PlayerController
                            else
                            {
                                // UE_LOG(LogNet, Warning, TEXT("UControlChannel::ReceivedBunch: PlayerController
                                // doesn't exist for the client, but the client is reporting an actor channel
                                // failure that isn't the PlayerController."));
                            }
                        }
                    }
                    // The client is sending an actor channel failure message with an invalid
                    // actor channel index
                    // @PotentialDOSAttackDetection
                    else
                    {
                        UE_LOG(
                            LogNet,
                            Warning,
                            TEXT("UControlChannel::RecievedBunch: The client is sending an actor channel failure "
                                 "message with an invalid actor channel index."));
                    }
                }
            }
        }
        else if (MessageType == NMT_GameSpecific)
        {
            // the most common Notify handlers do not support subclasses by default and so we redirect the game
            // specific messaging to the GameInstance instead
            uint8 MessageByte;
            FString MessageStr;
            if (FNetControlMessage<NMT_GameSpecific>::Receive(Bunch, MessageByte, MessageStr))
            {
                if (Connection->Driver->World != NULL && Connection->Driver->World->GetGameInstance() != NULL)
                {
                    Connection->Driver->World->GetGameInstance()->HandleGameNetControlMessage(
                        Connection,
                        MessageByte,
                        MessageStr);
                }
                else
                {
                    FWorldContext *Context = GEngine->GetWorldContextFromPendingNetGameNetDriver(Connection->Driver);
                    if (Context != NULL && Context->OwningGameInstance != NULL)
                    {
                        Context->OwningGameInstance->HandleGameNetControlMessage(Connection, MessageByte, MessageStr);
                    }
                }
            }
        }
        else if (MessageType == NMT_SecurityViolation)
        {
            FString DebugMessage;
            if (FNetControlMessage<NMT_SecurityViolation>::Receive(Bunch, DebugMessage))
            {
                UE_LOG(
                    LogSecurity,
                    Warning,
                    TEXT("%s: Closed: %s"),
                    *Connection->RemoteAddressToString(),
                    *DebugMessage);
                break;
            }
        }
#if defined(UE_5_1_OR_LATER)
        else if (MessageType == NMT_DestructionInfo)
        {
            if (!Connection->Driver->IsServer())
            {
                ReceiveDestructionInfo(Bunch);
            }
            else
            {
                UE_LOG(
                    LogNet,
                    Warning,
                    TEXT("UControlChannel::ReceivedBunch: Server received a NMT_DestructionInfo which is not "
                         "supported. Closing connection."));
                Connection->Close(ENetCloseResult::ClientSentDestructionInfo);
            }
        }
        else if (MessageType == NMT_CloseReason)
        {
            FString CloseReasonList;

            if (FNetControlMessage<NMT_CloseReason>::Receive(Bunch, CloseReasonList) && !CloseReasonList.IsEmpty())
            {
                Connection->HandleReceiveCloseReason(CloseReasonList);
            }

            break;
        }
        else if (MessageType == NMT_NetPing)
        {
            ENetPingControlMessage NetPingMessageType;
            FString MessageStr;

            if (FNetControlMessage<NMT_NetPing>::Receive(Bunch, NetPingMessageType, MessageStr) &&
                NetPingMessageType <= ENetPingControlMessage::Max)
            {
                UE::Net::FNetPing::HandleNetPingControlMessage(Connection, NetPingMessageType, MessageStr);
            }

            break;
        }
#elif defined(UE_5_0_OR_LATER)
        else if (MessageType == NMT_DestructionInfo)
        {
            ReceiveDestructionInfo(Bunch);
        }
        else if (MessageType == NMT_CloseReason)
        {
            FString CloseReasonList;

            if (FNetControlMessage<NMT_CloseReason>::Receive(Bunch, CloseReasonList) && !CloseReasonList.IsEmpty())
            {
                Connection->HandleReceiveCloseReason(CloseReasonList);
            }

            break;
        }
#else
        else if (MessageType == NMT_DestructionInfo)
        {
            ReceiveDestructionInfo(Bunch);
        }
#endif
        else if (
            MessageType == NMT_EncryptionAck && FNetworkHelpers::GetConfig(this->Connection) != nullptr &&
            FNetworkHelpers::GetConfig(this->Connection)->GetEnableAutomaticEncryptionOnTrustedDedicatedServers() &&
            FNetworkHelpers::GetRole(this->Connection) == EEOSNetDriverRole::ClientConnectedToDedicatedServer)
        {
            // This packet gets sent by servers when they call EnableEncryptionServer. We manually call
            // EnableEncryptionServer in our negotiation, but we don't use NMT_EncryptionAck (which will trigger
            // callbacks we're not handling).
            //
            // This packet will get discarded below, instead of being forwarded to the notify.
        }
        else if (
            MessageType == NMT_Failure && this->AuthConnectionContext.IsValid() &&
            this->AuthConnectionContext->GetRole() == EEOSNetDriverRole::DedicatedServer)
        {
            // Handle NMT_Failure messages sent from clients (normally this is only sent by servers to clients, but
            // we use it to indicate errors during negotiation).
            FString ClientError;
            if (FNetControlMessage<NMT_Failure>::Receive(Bunch, ClientError))
            {
                UE_LOG(LogNet, Error, TEXT("NMT_Failure received from client: %s"), *ClientError);
            }
            else
            {
                UE_LOG(LogNet, Error, TEXT("NMT_Failure received from client (could not decode)"));
            }
            this->Connection->Close();
        }
        else if (MessageType == NMT_Hello)
        {
            // Intercept NMT_Hello to perform connection-based authentication if needed.
            uint8 IsLittleEndian;
            uint32 RemoteNetworkVersion;
            FString EncryptionToken;
#if defined(UE_5_1_OR_LATER)
            EEngineNetworkRuntimeFeatures RemoteNetworkFeatures = EEngineNetworkRuntimeFeatures::None;
            if (FNetControlMessage<NMT_Hello>::Receive(
                    Bunch,
                    IsLittleEndian,
                    RemoteNetworkVersion,
                    EncryptionToken,
                    RemoteNetworkFeatures))
            {
                this->On_NMT_Hello(FOriginalParameters_NMT_Hello{
                    IsLittleEndian,
                    RemoteNetworkVersion,
                    EncryptionToken,
                    RemoteNetworkFeatures});
            }
#else
            if (FNetControlMessage<NMT_Hello>::Receive(Bunch, IsLittleEndian, RemoteNetworkVersion, EncryptionToken))
            {
                this->On_NMT_Hello(
                    FOriginalParameters_NMT_Hello{IsLittleEndian, RemoteNetworkVersion, EncryptionToken});
            }
#endif
        }
        else if (MessageType == NMT_Login)
        {
            FString ClientResponse;
            FString URLString;
            FUniqueNetIdRepl IncomingUser;
            FString OnlinePlatformNameString;
            if (FNetControlMessage<NMT_Login>::Receive(
                    Bunch,
                    ClientResponse,
                    URLString,
                    IncomingUser,
                    OnlinePlatformNameString))
            {
                this->On_NMT_Login(ClientResponse, URLString, IncomingUser, OnlinePlatformNameString);
            }
        }
        else if (MessageType == NMT_BeaconJoin)
        {
            FString BeaconType;
            FUniqueNetIdRepl IncomingUser;
            if (FNetControlMessage<NMT_BeaconJoin>::Receive(Bunch, BeaconType, IncomingUser))
            {
                this->On_NMT_BeaconJoin(BeaconType, IncomingUser);
            }
        }
#if EOS_VERSION_AT_LEAST(1, 12, 0)
        else if (MessageType == NMT_EOS_AntiCheatMessage)
        {
            FUniqueNetIdRepl SourceUserId;
            FUniqueNetIdRepl TargetUserId;
            TArray<uint8> AntiCheatData;
            if (FNetControlMessage<NMT_EOS_AntiCheatMessage>::Receive(Bunch, SourceUserId, TargetUserId, AntiCheatData))
            {
                this->On_NMT_EOS_AntiCheatMessage(SourceUserId, TargetUserId, AntiCheatData);
            }
        }
#endif // #if EOS_VERSION_AT_LEAST(1, 12, 0)
        else if (Routes.Contains(MessageType))
        {
            if (!Routes[MessageType].IsBound() || !Routes[MessageType].Execute(this, Bunch))
            {
                UE_LOG(
                    LogNet,
                    Error,
                    TEXT("Unhandled EOS-specific control message %d on role %d. The registered router could not route "
                         "or decode the packet successfully. Closing connection."),
                    MessageType,
                    FNetworkHelpers::GetRole(Connection));
                Connection->Close();
            }
        }
        else if (MessageType == NMT_EOS_WriteStat)
        {
            TArray<uint8> SerializedStat;
            if (FNetControlMessage<NMT_EOS_WriteStat>::Receive(Bunch, SerializedStat))
            {
                FOnlineStatsUserUpdatedStats Stats = DeserializeStats(SerializedStat);

                EEOSNetDriverRole Role = FNetworkHelpers::GetRole(this->Connection);
                if (Role != EEOSNetDriverRole::ClientConnectedToDedicatedServer &&
                    Role != EEOSNetDriverRole::ClientConnectedToListenServer)
                {
                }
                else if (!FNetworkHelpers::GetConfig(this->Connection)->GetAcceptStatWriteRequestsFromServers())
                {
                    UE_LOG(
                        LogEOS,
                        Warning,
                        TEXT("Ignoring NMT_WriteStat request because the client is not configured to accept stat write "
                             "requests from servers."));
                }
                else
                {
                    auto LocalNetDriver = Cast<UEOSNetDriver>(this->Connection->Driver);
                    if (IsValid(LocalNetDriver))
                    {
                        TSharedPtr<class ISocketSubsystemEOS> _;
                        TSharedPtr<class FOnlineSubsystemEOS, ESPMode::ThreadSafe> OnlineSubsystem;
                        if (LocalNetDriver->GetSubsystems(_, OnlineSubsystem))
                        {
                            auto StatsInterface = OnlineSubsystem->GetStatsInterface();
                            auto IdentityInterface = OnlineSubsystem->GetIdentityInterface();
                            TArray<FOnlineStatsUserUpdatedStats> StatsArray;
                            TSharedRef<const FUniqueNetId> DeserializedId =
                                IdentityInterface->CreateUniquePlayerId(Stats.Account->ToString()).ToSharedRef();
                            StatsArray.Add(FOnlineStatsUserUpdatedStats(DeserializedId, Stats.Stats));
                            StatsInterface->UpdateStats(
                                DeserializedId,
                                StatsArray,
                                FOnlineStatsUpdateStatsComplete::CreateLambda([](const FOnlineError &ResultState) {
                                    if (!ResultState.bSucceeded)
                                    {
                                        UE_LOG(
                                            LogEOS,
                                            Error,
                                            TEXT("Error while writing stat in response to NMT_WriteStat operation: %s"),
                                            *ResultState.ToLogString());
                                    }
                                }));
                        }
                        else
                        {
                            UE_LOG(
                                LogEOS,
                                Warning,
                                TEXT("Unable to write stats in response to NMT_WriteStat request because the online "
                                     "subsystem is no longer available."));
                        }
                    }
                    else
                    {
                        UE_LOG(
                            LogEOS,
                            Warning,
                            TEXT("Unable to write stats in response to NMT_WriteStat request because the network "
                                 "driver is not valid."));
                    }
                }
            }
        }
        else if (MessageType >= EOS_CONTROL_CHANNEL_MESSAGE_MIN && MessageType <= EOS_CONTROL_CHANNEL_MESSAGE_MAX)
        {
            UE_LOG(
                LogNet,
                Error,
                TEXT("Unexpected EOS-specific control message %d on role %d, closing connection."),
                MessageType,
                FNetworkHelpers::GetRole(Connection));
            Connection->Close();
        }
        else if (Connection->Driver->Notify != nullptr)
        {
            // Process control message on client/server connection
            Connection->Driver->Notify->NotifyControlMessage(Connection, MessageType, Bunch);
        }

        // if the message was not handled, eat it ourselves
        if (Pos == Bunch.GetPosBits() && !Bunch.IsError())
        {
            switch (MessageType)
            {
            case NMT_EOS_RequestClientEphemeralKey:
                FNetControlMessage<NMT_EOS_RequestClientEphemeralKey>::Discard(Bunch);
                break;
            case NMT_EOS_DeliverClientEphemeralKey:
                FNetControlMessage<NMT_EOS_DeliverClientEphemeralKey>::Discard(Bunch);
                break;
            case NMT_EOS_SymmetricKeyExchange:
                FNetControlMessage<NMT_EOS_SymmetricKeyExchange>::Discard(Bunch);
                break;
            case NMT_EOS_EnableEncryption:
                FNetControlMessage<NMT_EOS_EnableEncryption>::Discard(Bunch);
                break;
            case NMT_EOS_RequestClientToken:
                FNetControlMessage<NMT_EOS_RequestClientToken>::Discard(Bunch);
                break;
            case NMT_EOS_DeliverClientToken:
                FNetControlMessage<NMT_EOS_DeliverClientToken>::Discard(Bunch);
                break;
            case NMT_Hello:
                FNetControlMessage<NMT_Hello>::Discard(Bunch);
                break;
            case NMT_Welcome:
                FNetControlMessage<NMT_Welcome>::Discard(Bunch);
                break;
            case NMT_Upgrade:
                FNetControlMessage<NMT_Upgrade>::Discard(Bunch);
                break;
            case NMT_Challenge:
                FNetControlMessage<NMT_Challenge>::Discard(Bunch);
                break;
            case NMT_Netspeed:
                FNetControlMessage<NMT_Netspeed>::Discard(Bunch);
                break;
            case NMT_Login:
                FNetControlMessage<NMT_Login>::Discard(Bunch);
                break;
            case NMT_Failure:
                FNetControlMessage<NMT_Failure>::Discard(Bunch);
                break;
            case NMT_Join:
                // FNetControlMessage<NMT_Join>::Discard(Bunch);
                break;
            case NMT_JoinSplit:
                FNetControlMessage<NMT_JoinSplit>::Discard(Bunch);
                break;
            case NMT_Skip:
                FNetControlMessage<NMT_Skip>::Discard(Bunch);
                break;
            case NMT_Abort:
                FNetControlMessage<NMT_Abort>::Discard(Bunch);
                break;
            case NMT_PCSwap:
                FNetControlMessage<NMT_PCSwap>::Discard(Bunch);
                break;
            case NMT_ActorChannelFailure:
                FNetControlMessage<NMT_ActorChannelFailure>::Discard(Bunch);
                break;
            case NMT_DebugText:
                FNetControlMessage<NMT_DebugText>::Discard(Bunch);
                break;
            case NMT_NetGUIDAssign:
                FNetControlMessage<NMT_NetGUIDAssign>::Discard(Bunch);
                break;
            case NMT_EncryptionAck:
                // FNetControlMessage<NMT_EncryptionAck>::Discard(Bunch);
                break;
#if defined(UE_5_0_OR_LATER)
            case NMT_CloseReason:
                FNetControlMessage<NMT_CloseReason>::Discard(Bunch);
                break;
#endif
#if defined(UE_5_1_OR_LATER)
            case NMT_NetPing:
                FNetControlMessage<NMT_NetPing>::Discard(Bunch);
                break;
#endif
            case NMT_BeaconWelcome:
                // FNetControlMessage<NMT_BeaconWelcome>::Discard(Bunch);
                break;
            case NMT_BeaconJoin:
                FNetControlMessage<NMT_BeaconJoin>::Discard(Bunch);
                break;
            case NMT_BeaconAssignGUID:
                FNetControlMessage<NMT_BeaconAssignGUID>::Discard(Bunch);
                break;
            case NMT_BeaconNetGUIDAck:
                FNetControlMessage<NMT_BeaconNetGUIDAck>::Discard(Bunch);
                break;
            default:
                // if this fails, a case is missing above for an implemented message type
                // or the connection is being sent potentially malformed packets
                // @PotentialDOSAttackDetection
                check(!FNetControlMessageInfo::IsRegistered(MessageType));

                UE_LOG(
                    LogNet,
                    Log,
                    TEXT("Received unknown control channel message %i. Closing connection."),
                    int32(MessageType));
                Connection->Close();
                return;
            }
        }
        if (Bunch.IsError())
        {
            UE_LOG(
                LogNet,
                Error,
                TEXT("Failed to read control channel message '%s'"),
                FNetControlMessageInfo::GetName(MessageType));
            break;
        }
    }

    if (Bunch.IsError())
    {
        UE_LOG(LogNet, Error, TEXT("UControlChannel::ReceivedBunch: Failed to read control channel message"));

        if (Connection != NULL)
        {
            Connection->Close();
        }
    }
}
