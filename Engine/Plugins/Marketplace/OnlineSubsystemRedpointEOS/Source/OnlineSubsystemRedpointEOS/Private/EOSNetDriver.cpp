// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/EOSNetDriver.h"

#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "OnlineBeacon.h"
#include "OnlineSubsystemRedpointEOS/Private/EOSControlChannel.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/IInternetAddrEOS.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/ISocketEOS.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/ISocketSubsystemEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSDefines.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSIpNetConnection.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSNetConnection.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"
#include "OnlineSubsystemUtils.h"
#include "Sockets.h"
#if defined(EOS_SUPPORT_MULTI_IP_RESOLUTION)
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/SocketSubsystemMultiIpResolve.h"
#endif
#if !UE_BUILD_SHIPPING
#include "Net/OnlineEngineInterface.h"
#endif

EOS_ENABLE_STRICT_WARNINGS

UWorld *UEOSNetDriver::FindWorld()
{
    // If we have an explicit world reference set, then return that.
    UWorld *ExplicitWorld = this->GetWorld();
    if (IsValid(ExplicitWorld))
    {
        return ExplicitWorld;
    }

    // When a pending net game is calling InitConnect, we're attached to
    // a pending net game but we don't have a world reference. But we can
    // ask the engine for our world context...
    if (GEngine != nullptr)
    {
        FWorldContext *WorldContextFromPending = GEngine->GetWorldContextFromPendingNetGameNetDriver(this);
        if (WorldContextFromPending != nullptr)
        {
            return WorldContextFromPending->World();
        }
    }

    // When the net driver is being created for a beacon, the net driver instance
    // does not get a world set against it until after InitListen/InitConnect happens.
    // However, we need the world before then.
    //
    // The net driver also won't appear as a pending net game, because it's being set
    // up for a custom beacon.
    //
    // In this case, we have to iterate through the worlds, and then iterate through
    // all of the AOnlineBeacon* actors (both host and client beacons) in each world,
    // and see if any of them have this network driver associated with them. If they do,
    // then the world that the beacon is in is the world that we are associated with.
    if (GEngine != nullptr)
    {
        for (const auto &WorldContext : GEngine->GetWorldContexts())
        {
            UWorld *ItWorld = WorldContext.World();
            if (ItWorld != nullptr)
            {
                for (TActorIterator<AOnlineBeacon> It(ItWorld); It; ++It)
                {
                    if (It->GetNetDriver() == this)
                    {
                        return ItWorld;
                    }
                }
            }
        }
    }

    // No world could be found.
    return nullptr;
}

bool UEOSNetDriver::IsOwnedByBeacon()
{
    if (GEngine != nullptr)
    {
        for (const auto &WorldContext : GEngine->GetWorldContexts())
        {
            UWorld *ItWorld = WorldContext.World();
            if (ItWorld != nullptr)
            {
                for (TActorIterator<AOnlineBeacon> It(ItWorld); It; ++It)
                {
                    if (It->GetNetDriver() == this)
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

IOnlineSubsystem *GetSubsystemWithAutomationCompatibility(const UWorld *World)
{
    return Online::GetSubsystem(World);
}

bool UEOSNetDriver::GetSubsystems(
    TSharedPtr<ISocketSubsystemEOS> &OutSocketSubsystem,
    TSharedPtr<class FOnlineSubsystemEOS, ESPMode::ThreadSafe> &OutOnlineSubsystem)
{
    check(!this->HasAnyFlags(EObjectFlags::RF_ClassDefaultObject));

    // Get the current world.
    UWorld *WorldRef = this->FindWorld();
    if (WorldRef == nullptr)
    {
        UE_LOG(LogEOS, Error, TEXT("UEOSNetDriver could not locate the current world."));
        return false;
    }

    // Get the online subsystem.
    IOnlineSubsystem *Subsystem = GetSubsystemWithAutomationCompatibility(WorldRef);
    if (Subsystem == nullptr)
    {
        UE_LOG(LogEOS, Error, TEXT("No online subsystem is present, so UEOSNetDriver can not work."));
        return false;
    }
    if (Subsystem->GetSubsystemName() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("The current online subsystem is not the EOS online subsystem, so UEOSNetDriver can not work."));
        return false;
    }
#if WITH_EDITOR
    FModuleManager &ModuleManager = FModuleManager::Get();
    IModuleInterface *RedpointEOSModule = ModuleManager.GetModule("OnlineSubsystemRedpointEOS");
    if (RedpointEOSModule != nullptr)
    {
        if (((FOnlineSubsystemRedpointEOSModule *)RedpointEOSModule)->IsOperatingInNullMode())
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("The EOS online subsystem was not configured properly when the editor started up, so UEOSNetDriver can not work."));
            return false;
        }
    }
#endif
    TSharedRef<FOnlineSubsystemEOS, ESPMode::ThreadSafe> SubsystemEOS = ((FOnlineSubsystemEOS *)Subsystem)->AsShared();
    TSharedPtr<ISocketSubsystemEOS> SocketSubsystemPtr = SubsystemEOS->SocketSubsystem;
    if (SocketSubsystemPtr == nullptr)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("The EOS online subsystem does not have a socket subsystem, so UEOSNetDriver can not work."));
        return false;
    }

    OutSocketSubsystem = SocketSubsystemPtr;
    OutOnlineSubsystem = SubsystemEOS;
    return true;
}

bool UEOSNetDriver::CanPerformP2PNetworking(const TSharedPtr<ISocketSubsystemEOS> &InSocketSubsystem)
{
    return EOSString_ProductUserId::IsValid(InSocketSubsystem->GetBindingProductUserId_P2POnly());
}

bool UEOSNetDriver::CreateEOSSocket(
    const TSharedPtr<ISocketSubsystemEOS> &InSocketSubsystem,
    const FString &InDescription,
    const FURL &InURL,
    bool bListening,
    TSharedPtr<ISocketEOS> &OutSocket,
    EOS_ProductUserId &OutBindingUserId)
{
    check(!this->HasAnyFlags(EObjectFlags::RF_ClassDefaultObject));

    // Get the socket name from the networking stack.
    FString SocketName = InSocketSubsystem->GetSocketName(bListening, InURL);

    // Retrieve the local binding address.
    EOS_ProductUserId BindingUserId = InSocketSubsystem->GetBindingProductUserId_P2POnly();
    if (!EOSString_ProductUserId::IsValid(BindingUserId))
    {
        // No valid local user for binding.
        return false;
    }
    TSharedPtr<IInternetAddrEOS> BindAddr =
        StaticCastSharedRef<IInternetAddrEOS>(InSocketSubsystem->CreateInternetAddr());
    BindAddr->SetFromParameters(BindingUserId, SocketName, InURL.Port % EOS_CHANNEL_ID_MODULO);

    // Make the actual socket.
    ISocketEOS *NewSocket =
        (ISocketEOS *)InSocketSubsystem->CreateSocket(FName(TEXT("EOSSocket")), InDescription, REDPOINT_EOS_SUBSYSTEM);
    check(NewSocket != nullptr);
    verify(NewSocket->Bind(*BindAddr));

    // Call Listen or Connect based on what this socket is used for.
    if (bListening)
    {
        verify(NewSocket->Listen(0));
    }
    else
    {
        TSharedPtr<IInternetAddrEOS> DestAddr =
            StaticCastSharedRef<IInternetAddrEOS>(InSocketSubsystem->CreateInternetAddr());
        bool bIsValid = false;
        DestAddr->SetIp(*InURL.Host, bIsValid);
        DestAddr->SetPort(InURL.Port);
        check(bIsValid);
        verify(NewSocket->Connect(*DestAddr));
        check(DestAddr->GetPort() == BindAddr->GetPort());
    }

    OutSocket = NewSocket->AsSharedEOS();
    OutBindingUserId = BindingUserId;
    return true;
}

void UEOSNetDriver::SendAntiCheatData(
    const TSharedRef<FAntiCheatSession> &Session,
    const FUniqueNetIdEOS &SourceUserId,
    const FUniqueNetIdEOS &TargetUserId,
    const uint8 *Data,
    uint32_t Size)
{
#if EOS_VERSION_AT_LEAST(1, 12, 0)
#define FUNCTION_CONTEXT_MACRO "SendAntiCheatData(Session: %p, SourceUserId: %s, TargetUserId: %s, Data: ..., Size: %u)"
#define FUNCTION_CONTEXT_ARGS &Session.Get(), *SourceUserId.ToString(), *TargetUserId.ToString(), Size

    UE_LOG(LogEOSAntiCheat, VeryVerbose, TEXT(FUNCTION_CONTEXT_MACRO ": Called"), FUNCTION_CONTEXT_ARGS);

    checkf(
        Session == this->AntiCheatSession.ToSharedRef(),
        TEXT("Network send must match Anti-Cheat session of net driver!"));

    FUniqueNetIdRepl SourceUserIdRepl(SourceUserId.AsShared());
    FUniqueNetIdRepl TargetUserIdRepl(TargetUserId.AsShared());
    TArray<uint8> DataArr(Data, Size);

    if (this->ServerConnection)
    {
        // Connected to remote server.
        FNetControlMessage<NMT_EOS_AntiCheatMessage>::Send(
            this->ServerConnection,
            SourceUserIdRepl,
            TargetUserIdRepl,
            DataArr);
        UE_LOG(
            LogEOSAntiCheat,
            VeryVerbose,
            TEXT(FUNCTION_CONTEXT_MACRO ": Server connection: %p: Sent EAC packet to server."),
            FUNCTION_CONTEXT_ARGS,
            (UNetConnection *)this->ServerConnection);
    }
    else
    {
        // Send to remote client.
        bool bDidSendToRemoteClient = false;
        for (const auto &ClientConnection : this->ClientConnections)
        {
            if (IsValid(ClientConnection) && !ClientConnection->bPendingDestroy && TargetUserId.IsValid() &&
                ClientConnection->PlayerId.IsValid() && TargetUserId == *ClientConnection->PlayerId.GetUniqueNetId())
            {
                FNetControlMessage<NMT_EOS_AntiCheatMessage>::Send(
                    ClientConnection,
                    SourceUserIdRepl,
                    TargetUserIdRepl,
                    DataArr);
                bDidSendToRemoteClient = true;
                UE_LOG(
                    LogEOSAntiCheat,
                    VeryVerbose,
                    TEXT(FUNCTION_CONTEXT_MACRO ": Client connection: %p: Sent EAC packet to client."),
                    FUNCTION_CONTEXT_ARGS,
                    (UNetConnection *)ClientConnection);
                break;
            }
            else
            {
                bool bIsValid = IsValid(ClientConnection);
                bool bIsPendingDestroy = bIsValid && ClientConnection->bPendingDestroy;
                bool bTargetUserIdIsValid = TargetUserId.IsValid();
                bool bClientConnectionUserIdIsValid = bIsValid && ClientConnection->PlayerId.IsValid();
                bool bTargetUserIdIsConnectionUserId = bTargetUserIdIsValid && bClientConnectionUserIdIsValid &&
                                                       TargetUserId == *ClientConnection->PlayerId.GetUniqueNetId();

                UE_LOG(
                    LogEOSAntiCheat,
                    VeryVerbose,
                    TEXT(FUNCTION_CONTEXT_MACRO
                         ": Client connection: %p: Connection does not meet requirements for send (bIsValid: %s, "
                         "bIsPendingDestroy: %s, bTargetUserIdIsValid: %s, bClientConnectionUserIdIsValid: %s, "
                         "bTargetUserIdIsConnectionUserId: %s)"),
                    FUNCTION_CONTEXT_ARGS,
                    (UNetConnection *)ClientConnection,
                    bIsValid ? TEXT("true") : TEXT("false"),
                    bIsPendingDestroy ? TEXT("true") : TEXT("false"),
                    bTargetUserIdIsValid ? TEXT("true") : TEXT("false"),
                    bClientConnectionUserIdIsValid ? TEXT("true") : TEXT("false"),
                    bTargetUserIdIsConnectionUserId ? TEXT("true") : TEXT("false"));
            }
        }
        if (!bDidSendToRemoteClient)
        {
            UE_LOG(
                LogEOSAntiCheat,
                Warning,
                TEXT(FUNCTION_CONTEXT_MACRO
                     ": There is no client connection for the target user, so this EAC packet was dropped."),
                FUNCTION_CONTEXT_ARGS);
        }
    }
#undef FUNCTION_CONTEXT_MACRO
#undef FUNCTION_CONTEXT_ARGS
#endif // #if EOS_VERSION_AT_LEAST(1, 12, 0)
}

bool UEOSNetDriver::Socket_OnIncomingConnection(
    const TSharedRef<class ISocketEOS> &ListeningSocket,
    const TSharedRef<class FUniqueNetIdEOS> &LocalUser,
    const TSharedRef<class FUniqueNetIdEOS> &RemoteUser)
{
    EOS_SCOPE_CYCLE_COUNTER(STAT_EOSNetDriverOnIncomingConnection);

    // If we are a server, use the notify interface to determine if we want to accept the connection.
    if (ServerConnection == nullptr)
    {
        if (auto SocketPinned = this->Socket.Pin())
        {
            if (SocketPinned.IsValid())
            {
                if (Notify->NotifyAcceptingConnection() == EAcceptConnection::Accept)
                {
                    UE_LOG(
                        LogEOS,
                        Verbose,
                        TEXT("UEOSNetDriver::Socket_OnIncomingConnection allowed a new incoming connection."));
                    return true;
                }

                UE_LOG(
                    LogEOS,
                    Warning,
                    TEXT("UEOSNetDriver::Socket_OnIncomingConnection rejected a new incoming connection."));
                return false;
            }
        }
    }

    // If we are a client, we'll receive the incoming connection call when the server starts talking back to
    // us for the first time. Always accept these connections. Calling NotifyAcceptingConnection would fail since
    // we're the ones who initiated the outbound connection.
    return true;
}

void UEOSNetDriver::Socket_OnConnectionAccepted(
    const TSharedRef<class ISocketEOS> &ListeningSocket,
    const TSharedRef<class ISocketEOS> &AcceptedSocket,
    const TSharedRef<class FUniqueNetIdEOS> &LocalUser,
    const TSharedRef<class FUniqueNetIdEOS> &RemoteUser)
{
    EOS_SCOPE_CYCLE_COUNTER(STAT_EOSNetDriverOnConnectionAccepted);

    if (auto SocketPinned = this->Socket.Pin())
    {
        if (SocketPinned.IsValid())
        {
            check(&ListeningSocket.Get() == SocketPinned.Get());

            if (ServerConnection == nullptr)
            {
                // Get the address.
                TSharedPtr<IInternetAddrEOS> IncomingAddr =
                    StaticCastSharedRef<IInternetAddrEOS>(this->SocketSubsystem.Pin()->CreateInternetAddr());
                verify(AcceptedSocket->GetPeerAddress(*IncomingAddr));

#if !UE_BUILD_SHIPPING
                if (!FParse::Param(FCommandLine::Get(), TEXT("emulateeosshipping")))
                {
                    if (auto SocketSubsystemPinned = this->SocketSubsystem.Pin())
                    {
                        for (auto ExistingClientConnection : this->ClientConnections)
                        {
                            auto ExistingClientConnectionEOS = Cast<UEOSNetConnection>(ExistingClientConnection);
                            if (ExistingClientConnectionEOS)
                            {
                                if (auto ExistingSocketPinned = ExistingClientConnectionEOS->Socket.Pin())
                                {
                                    auto ExistingAddr = SocketSubsystemPinned->CreateInternetAddr();
                                    if (ExistingSocketPinned->GetPeerAddress(*ExistingAddr))
                                    {
                                        // Check to make sure the peer address of the newly accepted socket does not
                                        // conflict with any existing client connection. If it does, this is a bug
                                        // because the new connection should only be accepted after the cleanup has
                                        // finished running in ::Socket_OnConnectionClosed if this is a reconnect.
                                        check(!(*ExistingAddr == *IncomingAddr));
                                    }
                                }
                            }
                        }
                    }
                }
#endif

                // We are a server and we have a new client connection, construct
                // the connection object and add it to our list of client connections.
                //
                // Server connections to clients *MUST* be created in the USOCK_Open status,
                // otherwise handshaking (challenge response) won't be handled correctly
                // on the server.
                UEOSNetConnection *ClientConnection = NewObject<UEOSNetConnection>();
                check(ClientConnection);
                ClientConnection->InitRemoteConnection(
                    this,
                    &AcceptedSocket.Get(),
                    World ? World->URL : FURL(),
                    *IncomingAddr,
                    USOCK_Open);
                Notify->NotifyAcceptedConnection(ClientConnection);
                this->AddClientConnection(ClientConnection);

                UE_LOG(
                    LogEOS,
                    Verbose,
                    TEXT("UEOSNetDriver::Socket_OnConnectionAccepted accepted a new client connection."));
            }
            else
            {
                // We are a client and we just got a connection back from the server. Update our state
                // to USOCK_Open.
#if defined(UE_5_0_OR_LATER)
                ServerConnection->SetConnectionState(USOCK_Open);
#else
                ServerConnection->State = USOCK_Open;
#endif // #if defined(UE_5_0_OR_LATER)
            }
        }
    }
}

void UEOSNetDriver::Socket_OnConnectionClosed(
    const TSharedRef<class ISocketEOS> &ListeningSocket,
    const TSharedRef<class ISocketEOS> &ClosedSocket)
{
    EOS_SCOPE_CYCLE_COUNTER(STAT_EOSNetDriverOnConnectionClosed);

    UE_LOG(
        LogEOS,
        Verbose,
        TEXT("UEOSNetDriver::Socket_OnConnectionClosed received notification that a socket is closing."));

    // When a remote socket is closed on a listening socket, we need to remove the client connection from the driver. If
    // we don't do this, then the server won't handshake correctly on the next connection.
    if (ServerConnection == nullptr)
    {
        for (auto i = this->ClientConnections.Num() - 1; i >= 0; i--)
        {
            UEOSNetConnection *ClientConnection = Cast<UEOSNetConnection>(this->ClientConnections[i]);
            if (ClientConnection)
            {
                // Is it the same socket as the one that is closing?
                if (ClientConnection->Socket.Pin().Get() == &ClosedSocket.Get())
                {
                    UE_LOG(
                        LogEOS,
                        Verbose,
                        TEXT("UEOSNetDriver::Socket_OnConnectionClosed removed a client connection because the "
                             "underlying socket is closing."));
                    ClientConnection->CleanUp();
                }
            }
        }
    }
}

EEOSNetDriverRole UEOSNetDriver::GetEOSRole()
{
    if (this->IsPerformingIpConnection)
    {
        if (IsValid(this->ServerConnection))
        {
            return EEOSNetDriverRole::ClientConnectedToDedicatedServer;
        }
        else
        {
            return EEOSNetDriverRole::DedicatedServer;
        }
    }
    else
    {
        if (IsValid(this->ServerConnection))
        {
            return EEOSNetDriverRole::ClientConnectedToListenServer;
        }
        else
        {
            return EEOSNetDriverRole::ListenServer;
        }
    }
}

bool UEOSNetDriver::IsAvailable() const
{
    // If we are connecting over an IP address, defer to the IpNetDriver.
    if (this->IsPerformingIpConnection)
    {
        return UIpNetDriver::IsAvailable();
    }

    // @ todo
    return true;
}

bool UEOSNetDriver::IsNetResourceValid()
{
    // If we are connecting over an IP address, defer to the IpNetDriver.
    if (this->IsPerformingIpConnection)
    {
        return UIpNetDriver::IsNetResourceValid();
    }

    return this->Socket.IsValid();
}

class ISocketSubsystem *UEOSNetDriver::GetSocketSubsystem()
{
    // If we are connecting over an IP address, defer to the IpNetDriver.
    if (this->IsPerformingIpConnection)
    {
#if defined(EOS_SUPPORT_MULTI_IP_RESOLUTION)
        if (!FParse::Param(FCommandLine::Get(), TEXT("emulateeosshipping")))
        {
            return this->MultiIpResolveSubsystem.Get();
        }
        else
        {
            return UIpNetDriver::GetSocketSubsystem();
        }
#else
        return UIpNetDriver::GetSocketSubsystem();
#endif
    }

    // If we have a reference to a socket subsystem already, return that.
    if (TSharedPtr<ISocketSubsystemEOS> SocketSubsystemPtr = this->SocketSubsystem.Pin())
    {
        return SocketSubsystemPtr.Get();
    }

    // Otherwise, find it through the current world and current online subsystem.
    UWorld *WorldRef = this->FindWorld();
    check(WorldRef != nullptr);
    IOnlineSubsystem *Subsystem = GetSubsystemWithAutomationCompatibility(WorldRef);
    check(Subsystem != nullptr);
    TSharedRef<FOnlineSubsystemEOS, ESPMode::ThreadSafe> SubsystemEOS = ((FOnlineSubsystemEOS *)Subsystem)->AsShared();
    return SubsystemEOS->SocketSubsystem.Get();
}

bool UEOSNetDriver::InitConnect(FNetworkNotify *InNotify, const FURL &InConnectURL, FString &Error)
{
    EOS_SCOPE_CYCLE_COUNTER(STAT_EOSNetDriverInitConnect);

    // We need to be able to mutate the connection URL if we are fixing it up for the editor.
    FURL ConnectURL = InConnectURL;

    check(!this->HasAnyFlags(EObjectFlags::RF_ClassDefaultObject));

    // Check that the config is set up correctly.
    check(this->RelevantTimeout >= 0.0);

    // Cache whether we are being used by a beacon.
    this->bIsOwnedByBeacon = this->IsOwnedByBeacon();

    // Get subsystems.
    TSharedPtr<ISocketSubsystemEOS> RefSocketSubsystem;
    TSharedPtr<FOnlineSubsystemEOS, ESPMode::ThreadSafe> OnlineSubsystem;
    if (!this->GetSubsystems(RefSocketSubsystem, OnlineSubsystem))
    {
        // GetSubsystems will already log an appropriate error.
        return false;
    }

    if (!this->bIsOwnedByBeacon)
    {
        // Assign Anti-Cheat early so we can use it.
        this->AntiCheat = OnlineSubsystem->AntiCheat;
    }

#if WITH_EDITOR
    // When launching a multiplayer game in the editor through play-in-editor, the editor always tells clients
    // to connect to 127.0.0.1 on a given port. However, if the server is listening on a P2P address, this won't
    // succeed.
    //
    // Try to detect this scenario and redirect the connection URL to the correct EOS P2P URL for the game.
    if (ConnectURL.Host == TEXT("127.0.0.1") && GEngine != nullptr)
    {
        FWorldContext *WorldContextFromPending = GEngine->GetWorldContextFromPendingNetGameNetDriver(this);
        if (WorldContextFromPending != nullptr)
        {
            if (WorldContextFromPending->WorldType == EWorldType::PIE)
            {
                // Try to locate the host world context.
                for (auto CandidateWorldContext : GEngine->GetWorldContexts())
                {
                    if (CandidateWorldContext.WorldType == EWorldType::PIE && CandidateWorldContext.PIEInstance == 0)
                    {
                        // If this check fails, it means the logic inside FOnlineIdentityInterfaceEOS to make clients
                        // wait for the server to be ready is not correct.
                        if (CandidateWorldContext.bWaitingOnOnlineSubsystem)
                        {
                            UE_LOG(
                                LogEOS,
                                Error,
                                TEXT("PIE client attempted to connect to localhost, but host PIE context is still "
                                     "waiting for online subsystem. This is a bug!"));
                            break;
                        }
                        if (CandidateWorldContext.ActiveNetDrivers.Num() == 0 ||
                            !IsValid(CandidateWorldContext.ActiveNetDrivers[0].NetDriver))
                        {
                            UE_LOG(
                                LogEOS,
                                Warning,
                                TEXT("PIE client is attempting to connect to localhost, but the first PIE instance "
                                     "doesn't appear to be a listening server. If the host you are connecting to is "
                                     "listening over EOS P2P, this connection will fail."));
                            break;
                        }

                        auto NetDriver = Cast<UEOSNetDriver>(CandidateWorldContext.ActiveNetDrivers[0].NetDriver);

                        if (IsValid(NetDriver))
                        {
                            if (NetDriver->IsPerformingIpConnection)
                            {
                                // The network driver is listening on an IP address; the default 127.0.0.1 behaviour is
                                // fine.
                            }
                            else if (auto NetSocketPinned = NetDriver->Socket.Pin())
                            {
                                // The network driver is listening on a P2P address, we need to modify the target
                                // address.
                                auto BindAddr = RefSocketSubsystem->CreateInternetAddr();
                                NetSocketPinned->GetAddress(*BindAddr);
                                ConnectURL.Host = BindAddr->ToString(false);
                                // @todo: Also set the ConnectURL port here? It should already match.
                            }
                        }

                        break;
                    }
                }
            }
        }
    }
#endif

    // If the host of the connection URL does not end with .eosp2p, then this is an IP-based connection.
    if (!ConnectURL.Host.EndsWith(TEXT(".eosp2p")))
    {
        this->IsPerformingIpConnection = true;
#if defined(EOS_SUPPORT_MULTI_IP_RESOLUTION)
        if (!FParse::Param(FCommandLine::Get(), TEXT("emulateeosshipping")))
        {
            if (ConnectURL.Host.Contains(TEXT(",")))
            {
                // Modify connection settings so we can get through the list of
                // multiple IP addresses quickly.

				//  ==============  ==============  ============== 
				// EDITED by Vipe, this seems to get in the way of connecting to servers, and matches the timeout error we see in the logs.
				//this->InitialConnectTimeout = 2.0f;
				if (GConfig)
				{
					GConfig->GetFloat(TEXT("/Script/OnlineSubsystemRedpointEOS.EOSNetDriver"), TEXT("InitialConnectTimeout"), this->InitialConnectTimeout, GEngineIni);
				}
				else
				{
					this->InitialConnectTimeout = 10.f;
				}
				// ============== END Vipe's edit. ============== 

                this->bNoTimeouts = false;
            }
        }
#endif
        this->NetConnectionClass = UEOSIpNetConnection::StaticClass();

        if (!this->bIsOwnedByBeacon)
        {
            if (auto AC = this->AntiCheat.Pin())
            {
                // Try to find the connecting user of this net driver.
                if (OnlineSubsystem->GetIdentityInterface()->GetLoginStatus(0) == ELoginStatus::LoggedIn)
                {
                    auto UserId = OnlineSubsystem->GetIdentityInterface()->GetUniquePlayerId(0);
                    checkf(UserId.IsValid(), TEXT("UserId must be valid if logged in!"));

                    // Create the Anti-Cheat session for IP.
                    checkf(
                        !this->AntiCheatSession.IsValid(),
                        TEXT("Expect Anti-Cheat session to not already be created."));
                    this->AntiCheatSession = AC->CreateSession(
                        false,
                        (const FUniqueNetIdEOS &)*UserId,
                        true,
                        nullptr,
                        FString::Printf(TEXT("%s:%d"), *ConnectURL.Host, ConnectURL.Port));
                    if (!this->AntiCheatSession.IsValid())
                    {
                        UE_LOG(LogEOS, Error, TEXT("Net driver failed to set up Anti-Cheat session."));
                        return false;
                    }
                    checkf(
                        !AC->OnSendNetworkMessage.IsBound(),
                        TEXT("IAntiCheat::OnSendNetworkMessage should not be bound yet."));
                    AC->OnSendNetworkMessage.BindUObject(this, &UEOSNetDriver::SendAntiCheatData);
                }
            }
        }

        bool bConnectSuccess = UIpNetDriver::InitConnect(InNotify, ConnectURL, Error);
        if (!bConnectSuccess)
        {
            return false;
        }
        return true;
    }

    // Perform common initialization.
    // NOLINTNEXTLINE(bugprone-parent-virtual-call)
    if (!UNetDriver::InitBase(true, InNotify, ConnectURL, false, Error))
    {
        UE_LOG(LogEOS, Error, TEXT("UEOSNetDriver: InitConnect failed while setting up base information"));
        return false;
    }

    // Create the socket.
    TSharedPtr<ISocketEOS> CreatedSocket;
    EOS_ProductUserId BindingUserId = {};
    if (!this->CreateEOSSocket(
            RefSocketSubsystem,
            TEXT("Unreal client (EOS)"),
            ConnectURL,
            false,
            CreatedSocket,
            BindingUserId))
    {
        return false;
    }

    // Set up Anti-Cheat for P2P.
    if (!this->bIsOwnedByBeacon)
    {
        if (auto AC = this->AntiCheat.Pin())
        {
            // Create the Anti-Cheat session.
            checkf(!this->AntiCheatSession.IsValid(), TEXT("Expect Anti-Cheat session to not already be created."));
            TSharedPtr<IInternetAddrEOS> DestAddr =
                StaticCastSharedRef<IInternetAddrEOS>(RefSocketSubsystem->CreateInternetAddr());
            verifyf(
                CreatedSocket->GetPeerAddress(*DestAddr),
                TEXT("Peer address can be read for connecting P2P socket"));
            this->AntiCheatSession = AC->CreateSession(
                false,
                *MakeShared<FUniqueNetIdEOS>(BindingUserId),
                false,
                MakeShared<FUniqueNetIdEOS>(DestAddr->GetUserId()),
                FString::Printf(TEXT("%s:%d"), *ConnectURL.Host, ConnectURL.Port));
            if (!this->AntiCheatSession.IsValid())
            {
                UE_LOG(LogEOS, Error, TEXT("Net driver failed to set up Anti-Cheat session."));
                return false;
            }
            checkf(
                !AC->OnSendNetworkMessage.IsBound(),
                TEXT("IAntiCheat::OnSendNetworkMessage should not be bound yet."));
            AC->OnSendNetworkMessage.BindUObject(this, &UEOSNetDriver::SendAntiCheatData);
        }
    }

    // Set LocalAddr, which contains the address we bound on.
    this->LocalAddr = StaticCastSharedRef<IInternetAddrEOS>(RefSocketSubsystem->CreateInternetAddr());
    CreatedSocket->GetAddress(*this->LocalAddr);

    // Listen for connection open / close.
    CreatedSocket->OnConnectionClosed.BindUObject(this, &UEOSNetDriver::Socket_OnConnectionClosed);
    CreatedSocket->OnConnectionAccepted.BindUObject(this, &UEOSNetDriver::Socket_OnConnectionAccepted);
    CreatedSocket->OnIncomingConnection.BindUObject(this, &UEOSNetDriver::Socket_OnIncomingConnection);

    // Create the network connection to the server.
    this->ServerConnection = NewObject<UEOSNetConnection>();
    this->ServerConnection->InitLocalConnection(this, CreatedSocket.Get(), ConnectURL, USOCK_Pending);
    this->CreateInitialClientChannels();

    this->Socket = CreatedSocket;
    this->SocketSubsystem = RefSocketSubsystem;

    UE_LOG(LogEOS, Verbose, TEXT("UEOSNetDriver: InitConnect completed successfully"));
    return true;
}

void UEOSNetDriver::PostInitProperties()
{
    Super::PostInitProperties();

    this->Socket = nullptr;
    this->RegisteredListeningUser = nullptr;
    this->RegisteredListeningSubsystem = nullptr;
    this->bDidRegisterListeningUser = false;
    this->IsPerformingIpConnection = false;
    this->NetConnectionClass = UEOSNetConnection::StaticClass();
#if defined(EOS_SUPPORT_MULTI_IP_RESOLUTION)
    if (!FParse::Param(FCommandLine::Get(), TEXT("emulateeosshipping")))
    {
        this->MultiIpResolveSubsystem = MakeShared<FSocketSubsystemMultiIpResolve>();
    }
#endif
}

bool UEOSNetDriver::InitListen(FNetworkNotify *InNotify, FURL &ListenURL, bool bReuseAddressAndPort, FString &Error)
{
    EOS_SCOPE_CYCLE_COUNTER(STAT_EOSNetDriverInitListen);

    check(!this->HasAnyFlags(EObjectFlags::RF_ClassDefaultObject));

    // Check that the config is set up correctly.
    check(this->RelevantTimeout >= 0.0);

    // Cache whether we are being used by a beacon.
    this->bIsOwnedByBeacon = this->IsOwnedByBeacon();

    // Get subsystems.
    TSharedPtr<ISocketSubsystemEOS> RefSocketSubsystem;
    TSharedPtr<FOnlineSubsystemEOS, ESPMode::ThreadSafe> OnlineSubsystem;
    if (!this->GetSubsystems(RefSocketSubsystem, OnlineSubsystem))
    {
        // GetSubsystems will already log an appropriate error.
        return false;
    }

    if (!this->bIsOwnedByBeacon)
    {
        // Assign Anti-Cheat early so we can use it.
        this->AntiCheat = OnlineSubsystem->AntiCheat;
    }

    // Determine what mode to run networking in.
    FString ListenMode = FString(ListenURL.GetOption(TEXT("NetMode="), TEXT("auto"))).ToLower();
    bool P2PAvailable = this->CanPerformP2PNetworking(RefSocketSubsystem);
    if (ListenMode == TEXT("forceip") || (!P2PAvailable && ListenMode != TEXT("forcep2p")))
    {
        this->IsPerformingIpConnection = true;
        this->NetConnectionClass = UEOSIpNetConnection::StaticClass();

        if (!UIpNetDriver::InitListen(InNotify, ListenURL, bReuseAddressAndPort, Error))
        {
            return false;
        }

        EOS_ProductUserId IpBindingUserId = {};
        if (RefSocketSubsystem->GetBindingProductUserId_P2POrDedicatedServer(IpBindingUserId) &&
            this->GetSocket() != nullptr)
        {
            // Only register the IP-based address if the binding user is valid. If the binding user isn't
            // valid, then the user can't use the session API to create sessions anyway, so we don't need
            // to worry about it not being registered.
            TSharedRef<FInternetAddr> IpAddr = UIpNetDriver::GetSocketSubsystem()->CreateInternetAddr();
            this->GetSocket()->GetAddress(*IpAddr);
            IpAddr->SetPort(this->GetSocket()->GetPortNo());
            TArray<TSharedPtr<FInternetAddr>> DevIpAddrs;
#if defined(EOS_SUPPORT_MULTI_IP_RESOLUTION)
            if (!FParse::Param(FCommandLine::Get(), TEXT("emulateeosshipping")))
            {
                if (this->GetSocketSubsystem()->GetLocalAdapterAddresses(DevIpAddrs))
                {
                    TSharedRef<FInternetAddr> LocalhostAddr = this->GetSocketSubsystem()->CreateInternetAddr();
                    LocalhostAddr->SetLoopbackAddress();
                    DevIpAddrs.Add(LocalhostAddr);
                }
            }
#endif
            if (!this->bIsOwnedByBeacon)
            {
                OnlineSubsystem->RegisterListeningAddress(IpBindingUserId, IpAddr, DevIpAddrs);
            }
            this->RegisteredListeningSubsystem = OnlineSubsystem;
            this->RegisteredListeningUser = IpBindingUserId;
            this->bDidRegisterListeningUser = true;
        }

        if (!this->bIsOwnedByBeacon)
        {
            if (auto AC = this->AntiCheat.Pin())
            {
                // Create the Anti-Cheat session for IP.
                checkf(!this->AntiCheatSession.IsValid(), TEXT("Expect Anti-Cheat session to not already be created."));
                this->AntiCheatSession = AC->CreateSession(
                    true,
                    // IP-based connections protected by Anti-Cheat *must* assume dedicated server, because connecting
                    // clients will not be able to know if the target server is logged in or not when they are
                    // connecting.
                    *FUniqueNetIdEOS::DedicatedServerId(),
                    true,
                    nullptr,
                    TEXT(""));
                if (!this->AntiCheatSession.IsValid())
                {
                    UE_LOG(LogEOS, Error, TEXT("Net driver failed to set up Anti-Cheat session."));
                    return false;
                }
                checkf(
                    !AC->OnSendNetworkMessage.IsBound(),
                    TEXT("IAntiCheat::OnSendNetworkMessage should not be bound yet."));
                AC->OnSendNetworkMessage.BindUObject(this, &UEOSNetDriver::SendAntiCheatData);
            }
        }

        // IP-based connection has finished being set up.
        return true;
    }
    else if (ListenMode == TEXT("forcep2p"))
    {
        if (!P2PAvailable)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("UEOSNetDriver: NetMode=ForceP2P but peer-to-peer networking is not available (because the "
                     "user "
                     "is not signed into an EOS account)"));
            return false;
        }
    }

    // Perform common initialization.
    // NOLINTNEXTLINE(bugprone-parent-virtual-call)
    if (!UNetDriver::InitBase(false, InNotify, ListenURL, bReuseAddressAndPort, Error))
    {
        UE_LOG(LogEOS, Error, TEXT("UEOSNetDriver: InitListen failed while setting up base information"));
        return false;
    }

    // We used to call InitConnectionlessHandler here, but this code path is only
    // for P2P connections, and we no longer use the packet handler infrastructure
    // for those types of connections.

    // Create the socket.
    TSharedPtr<ISocketEOS> CreatedSocket;
    EOS_ProductUserId BindingUserId = {};
    if (!this->CreateEOSSocket(
            RefSocketSubsystem,
            TEXT("Unreal server (EOS)"),
            ListenURL,
            true,
            CreatedSocket,
            BindingUserId))
    {
        return false;
    }

    // Set up Anti-Cheat for P2P.
    if (!this->bIsOwnedByBeacon)
    {
        if (auto AC = this->AntiCheat.Pin())
        {
            // Create the Anti-Cheat session.
            checkf(!this->AntiCheatSession.IsValid(), TEXT("Expect Anti-Cheat session to not already be created."));
            this->AntiCheatSession =
                AC->CreateSession(true, *MakeShared<FUniqueNetIdEOS>(BindingUserId), false, nullptr, TEXT(""));
            if (!this->AntiCheatSession.IsValid())
            {
                UE_LOG(LogEOS, Error, TEXT("Net driver failed to set up Anti-Cheat session."));
                return false;
            }
            checkf(
                !AC->OnSendNetworkMessage.IsBound(),
                TEXT("IAntiCheat::OnSendNetworkMessage should not be bound yet."));
            AC->OnSendNetworkMessage.BindUObject(this, &UEOSNetDriver::SendAntiCheatData);
        }
    }

    // Set LocalAddr, which contains the address we bound on.
    check(!this->LocalAddr.IsValid());
    this->LocalAddr = StaticCastSharedRef<IInternetAddrEOS>(RefSocketSubsystem->CreateInternetAddr());
    CreatedSocket->GetAddress(*this->LocalAddr);

    // Listen for connection open / close.
    CreatedSocket->OnConnectionClosed.BindUObject(this, &UEOSNetDriver::Socket_OnConnectionClosed);
    CreatedSocket->OnConnectionAccepted.BindUObject(this, &UEOSNetDriver::Socket_OnConnectionAccepted);
    CreatedSocket->OnIncomingConnection.BindUObject(this, &UEOSNetDriver::Socket_OnIncomingConnection);

    // Register this socket as a listening socket on the online subsystem.
    if (!this->bIsOwnedByBeacon)
    {
        OnlineSubsystem->RegisterListeningAddress(
            BindingUserId,
            this->LocalAddr.ToSharedRef(),
            TArray<TSharedPtr<FInternetAddr>>());
    }
    this->RegisteredListeningSubsystem = OnlineSubsystem;
    this->RegisteredListeningUser = BindingUserId;
    this->bDidRegisterListeningUser = true;

    this->Socket = CreatedSocket;
    this->SocketSubsystem = RefSocketSubsystem;

    return true;
}

void UEOSNetDriver::InitConnectionlessHandler()
{
    if (this->IsPerformingIpConnection)
    {
        // IP passthrough needs normal connectionless handler, including
        // stateless connection handler.
        UIpNetDriver::InitConnectionlessHandler();
    }
    else
    {
        // We do not use the packet handler infrastructure on P2P connections;
        // we don't need stateless connection handling or encryption handling.
    }
}

void UEOSNetDriver::TickDispatch(float DeltaTime)
{
    EOS_SCOPE_CYCLE_COUNTER(STAT_EOSNetDriverTickDispatch);

    // If we are connecting over an IP address, defer to the IpNetDriver.
    if (this->IsPerformingIpConnection)
    {
        UIpNetDriver::TickDispatch(DeltaTime);
        return;
    }

    {
        EOS_SCOPE_CYCLE_COUNTER(STAT_EOSNetDriverBaseTickDispatch);
        // NOLINTNEXTLINE(bugprone-parent-virtual-call)
        UNetDriver::TickDispatch(DeltaTime);
    }

    while (true)
    {
        EOS_INC_DWORD_STAT(STAT_EOSNetP2PReceivedLoopIters);
        EOS_TRACE_COUNTER_INCREMENT(CTR_EOSNetP2PReceivedLoopIters);

        // We MUST scope the socket here before ReceivePacketFromDriver gets called. In ReceivePacketFromDriver, we can
        // end up processing user code which might want to disconnect the connection or destroy a beacon. If we don't
        // scope the socket here, then the check(EOSSocketPtr.IsUnique()); inside FSocketSubsystemEOSFull::DestroySocket
        // will fail.
        uint8 *Buffer = nullptr;
        int32 BytesRead = 0;
        TSharedPtr<IInternetAddrEOS> ReceivedAddr;
        if (auto SocketSubsystemPinned = this->SocketSubsystem.Pin())
        {
            ReceivedAddr = StaticCastSharedRef<IInternetAddrEOS>(SocketSubsystemPinned->CreateInternetAddr());
            if (TSharedPtr<ISocketEOS> SocketPinned = this->Socket.Pin())
            {
                if (!SocketPinned.IsValid())
                {
                    // Socket is no longer valid.
                    break;
                }

                uint32 PendingDataSize = 0;
                if (!SocketPinned->HasPendingData(PendingDataSize) || PendingDataSize == 0)
                {
                    // No more pending data.
                    break;
                }

                Buffer = (uint8 *)FMemory::Malloc(PendingDataSize);
                if (!SocketPinned
                         ->RecvFrom(Buffer, PendingDataSize, BytesRead, *ReceivedAddr, ESocketReceiveFlags::None))
                {
                    FMemory::Free(Buffer);
                    UE_LOG(LogEOS, Error, TEXT("UEOSNetDriver::TickDispatch: Socket RecvFrom failed!"));
                    break;
                }
            }
            else
            {
                // Socket is no longer valid.
                break;
            }
        }
        else
        {
            // Socket is no longer valid.
            break;
        }

        if (BytesRead == 0)
        {
            FMemory::Free(Buffer);
            UE_LOG(LogEOS, Error, TEXT("UEOSNetDriver::TickDispatch: Socket RecvFrom read 0 bytes of data!"));
            break;
        }

        EOS_INC_DWORD_STAT(STAT_EOSNetP2PReceivedPackets);
        EOS_INC_DWORD_STAT_BY(STAT_EOSNetP2PReceivedBytes, BytesRead);
        EOS_TRACE_COUNTER_INCREMENT(CTR_EOSNetP2PReceivedPackets);
        EOS_TRACE_COUNTER_ADD(CTR_EOSNetP2PReceivedBytes, BytesRead);

        if (ServerConnection != nullptr)
        {
            // This is a client connection to a remote server.
            Cast<UEOSNetConnection>(ServerConnection)->ReceivePacketFromDriver(ReceivedAddr, Buffer, BytesRead);
        }
        else
        {
            // This is data coming in from a remote client to a local listen/dedicated server.
            bool bFoundClientConnection = false;
            for (UNetConnection *ClientConnection : this->ClientConnections)
            {
                UEOSNetConnection *EOSClientConnection = Cast<UEOSNetConnection>(ClientConnection);

                // Fetch the client address; again, the pinned socket must be pinned before ReceivePacketFromDriver is
                // called.
                TSharedPtr<IInternetAddrEOS> ClientAddr =
                    StaticCastSharedRef<IInternetAddrEOS>(this->SocketSubsystem.Pin()->CreateInternetAddr());
                if (TSharedPtr<ISocketEOS> ClientSocketPinned = EOSClientConnection->Socket.Pin())
                {
                    if (ClientSocketPinned.IsValid())
                    {
                        ClientSocketPinned->GetPeerAddress(*ClientAddr);
                    }
                }

                // If this is the right client, have it receive the data.
                if (*ClientAddr == *ReceivedAddr)
                {
                    bFoundClientConnection = true;
                    EOSClientConnection->ReceivePacketFromDriver(ReceivedAddr, Buffer, BytesRead);
                    break;
                }
            }

            if (!bFoundClientConnection)
            {
                UE_LOG(
                    LogEOS,
                    Warning,
                    TEXT("UEOSNetDriver::TickDispatch: Could not find client connection to receive data!"));
            }
        }

        FMemory::Free(Buffer);
    }
}

void UEOSNetDriver::LowLevelSend(
    TSharedPtr<const FInternetAddr> Address,
    void *Data,
    int32 CountBits,
    FOutPacketTraits &Traits)
{
    // If we are connecting over an IP address, defer to the IpNetDriver.
    if (this->IsPerformingIpConnection)
    {
        UIpNetDriver::LowLevelSend(Address, Data, CountBits, Traits);
        return;
    }

    TSharedPtr<ISocketEOS> SocketPinned = this->Socket.Pin();
    if (SocketPinned.IsValid() && Address.IsValid() && Address->IsValid())
    {
        const uint8 *SendData = reinterpret_cast<const uint8 *>(Data);

        // We don't use the packet handler infrastructure, so there's no
        // need to mutate the packet data with the handler.

        if (CountBits > 0)
        {
            int32 BytesToSend = FMath::DivideAndRoundUp(CountBits, 8);
            int32 SentBytes = 0;

            // Our sendto will find the correct socket to send over.
            UEOSNetConnection::LogPacket(this, Address, false, SendData, BytesToSend);
            if (!SocketPinned->SendTo(SendData, BytesToSend, SentBytes, *Address))
            {
                UE_LOG(
                    LogNet,
                    Warning,
                    TEXT("UEOSNetDriver::LowLevelSend: Could not send %d data over socket to %s!"),
                    BytesToSend,
                    *Address->ToString(true));
            }
            else
            {
                EOS_INC_DWORD_STAT(STAT_EOSNetP2PSentPackets);
                EOS_INC_DWORD_STAT_BY(STAT_EOSNetP2PSentBytes, BytesToSend);
                EOS_TRACE_COUNTER_INCREMENT(CTR_EOSNetP2PSentPackets);
                EOS_TRACE_COUNTER_ADD(CTR_EOSNetP2PSentBytes, BytesToSend);
            }
        }
    }
    else
    {
        UE_LOG(
            LogNet,
            Error,
            TEXT("UEOSNetDriver::LowLevelSend: Could not send data because either the socket or address is "
                 "invalid!"));
    }
}

void UEOSNetDriver::LowLevelDestroy()
{
    if (auto AC = this->AntiCheat.Pin())
    {
        if (this->AntiCheatSession.IsValid())
        {
            AC->OnSendNetworkMessage.Unbind();
            if (!AC->DestroySession(*this->AntiCheatSession))
            {
                UE_LOG(LogEOS, Warning, TEXT("Net driver failed to destroy Anti-Cheat session!"));
            }
        }
    }

    if (TSharedPtr<FOnlineSubsystemEOS, ESPMode::ThreadSafe> OnlineSubsystem = this->RegisteredListeningSubsystem.Pin())
    {
        if (this->bDidRegisterListeningUser)
        {
            if (this->LocalAddr.IsValid())
            {
                if (!this->bIsOwnedByBeacon)
                {
                    OnlineSubsystem->DeregisterListeningAddress(
                        this->RegisteredListeningUser,
                        this->LocalAddr.ToSharedRef());
                }
            }
            this->RegisteredListeningUser = nullptr;
            this->RegisteredListeningSubsystem = nullptr;
            this->bDidRegisterListeningUser = false;
        }
    }

    // If we are connecting over an IP address, defer to the IpNetDriver.
    if (this->IsPerformingIpConnection)
    {
        UIpNetDriver::LowLevelDestroy();
        return;
    }

    if (TSharedPtr<ISocketEOS> SocketPtr = this->Socket.Pin())
    {
        if (TSharedPtr<ISocketSubsystemEOS> SocketSubsystemPtr = this->SocketSubsystem.Pin())
        {
            // We must get the pointer and then set SocketPtr to nullptr so that the
            // the socket subsystem has the singular remaining strong reference to the
            // socket.
            ISocketEOS *SocketRaw = SocketPtr.Get();
            SocketPtr = nullptr;

            // Destroy the socket.
            SocketSubsystemPtr->DestroySocket(SocketRaw);
        }
    }

    this->Socket = nullptr;
    this->SocketSubsystem = nullptr;
}

EOS_DISABLE_STRICT_WARNINGS