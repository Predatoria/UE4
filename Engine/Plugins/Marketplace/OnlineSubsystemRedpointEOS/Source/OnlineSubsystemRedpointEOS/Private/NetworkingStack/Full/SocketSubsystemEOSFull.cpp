// Copyright June Rhodes. All Rights Reserved.

#include "./SocketSubsystemEOSFull.h"

#include "./EOSSocketRoleClient.h"
#include "./EOSSocketRoleListening.h"
#include "./EOSSocketRoleNone.h"
#include "./EOSSocketRoleRemote.h"
#include "./InternetAddrEOSFull.h"
#include "./SocketSubsystemEOSListenManager.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "./SocketTracing.h"

EOS_ENABLE_STRICT_WARNINGS

FSocketSubsystemEOSFull::FSocketSubsystemEOSFull(
    const TSharedRef<FOnlineSubsystemEOS, ESPMode::ThreadSafe> &InSubsystem,
    EOS_HPlatform InPlatform,
    TSharedRef<FEOSConfig> InConfig,
    TSharedRef<FEOSRegulatedTicker> InRegulatedTicker)
    : EOSP2P(EOS_Platform_GetP2PInterface(InPlatform))
    , bReceiveSuspended(false)
    , Subsystem(InSubsystem)
    , Config(MoveTemp(InConfig))
    , RegulatedTicker(MoveTemp(InRegulatedTicker))
    , ReceivePacketsDelegateHandle()
    , HeldSockets()
    , NextSocketId(1000)
    , AssignedSockets()
    , RoleInstance_None(MakeShared<FEOSSocketRoleNone>())
    , RoleInstance_Client(MakeShared<FEOSSocketRoleClient>())
    , RoleInstance_Listening(MakeShared<FEOSSocketRoleListening>())
    , RoleInstance_Remote(MakeShared<FEOSSocketRoleRemote>())
    , ListenManager(MakeShared<FSocketSubsystemEOSListenManager>(EOS_Platform_GetP2PInterface(InPlatform)))
    // If the game were to crash and this was simply initialized to 0 every time, then when the player attempts to
    // reconnect to the server they were playing on, it would result in a reset ID of 0 being sent. If the server hasn't
    // yet cleaned up the connection (due to unclean shutdown), then it would not be aware that it needs to reset the
    // connection. By setting this value to a timestamp, we mitigate this problem. In addition we don't care about
    // overflows here because the reset ID is only ever compared exactly, and is not used in relative comparisons.
    , NextResetId((uint32_t)FDateTime::UtcNow().ToUnixTimestamp())
    , PendingResetIds()
#if EOS_VERSION_AT_LEAST(1, 11, 0)
    , IncomingPacketQueueFull()
#endif // #if EOS_VERSION_AT_LEAST(1, 11, 0)
{
    check(this->EOSP2P != nullptr);
}

FSocketSubsystemEOSFull::~FSocketSubsystemEOSFull()
{
}

FString FSocketSubsystemEOSFull::GetSocketName(bool bListening, FURL InURL) const
{
    if (bListening)
    {
        // If we are listening, get it from the option.
        auto SocketName = FString(InURL.GetOption(TEXT("SocketName="), TEXT("default")));
        if (SocketName.Len() > EOS_P2P_SOCKET_NAME_MAX_LENGTH)
        {
            return SocketName.Mid(0, 32);
        }
        else
        {
            return SocketName;
        }
    }
    else
    {
        // Otherwise get it from the URL hostname.
        auto Addr = MakeShared<FInternetAddrEOSFull>();
        bool bIsValid = false;
        Addr->SetIp(*InURL.Host, bIsValid);
        if (!bIsValid)
        {
            UE_LOG(
                LogEOS,
                Warning,
                TEXT("Unable to parse address as EOS address to determine socket name '%s', defaulting to 'default'."),
                *InURL.Host);
            return FString(TEXT("default"));
        }
        else
        {
            return Addr->GetSymmetricSocketName();
        }
    }
}

bool FSocketSubsystemEOSFull::Init(FString &Error)
{
    Error = TEXT("");

#if EOS_VERSION_AT_LEAST(1, 11, 0)
    // Show default P2P queue size.
    EOS_P2P_GetPacketQueueInfoOptions QueueOpts = {};
    QueueOpts.ApiVersion = EOS_P2P_GETPACKETQUEUEINFO_API_LATEST;
    EOS_P2P_PacketQueueInfo QueueInfo = {};
    EOS_EResult QueueResult = EOS_P2P_GetPacketQueueInfo(this->EOSP2P, &QueueOpts, &QueueInfo);
    if (QueueResult == EOS_EResult::EOS_Success)
    {
        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("EOS P2P packet queue info: incoming_max_size=%llu, incoming_current_size=%llu, "
                 "incoming_packet_count=%llu, outgoing_max_size=%llu, outgoing_current_size=%llu, "
                 "outgoing_packet_count=%llu"),
            QueueInfo.IncomingPacketQueueMaxSizeBytes,
            QueueInfo.IncomingPacketQueueCurrentSizeBytes,
            QueueInfo.IncomingPacketQueueCurrentPacketCount,
            QueueInfo.OutgoingPacketQueueMaxSizeBytes,
            QueueInfo.OutgoingPacketQueueCurrentSizeBytes,
            QueueInfo.OutgoingPacketQueueCurrentPacketCount);
    }

    // Set P2P to have unlimited packet queue sizes.
    EOS_P2P_SetPacketQueueSizeOptions SizeOpts = {};
    SizeOpts.ApiVersion = EOS_P2P_SETPACKETQUEUESIZE_API_LATEST;
    SizeOpts.IncomingPacketQueueMaxSizeBytes = EOS_P2P_MAX_QUEUE_SIZE_UNLIMITED;
    SizeOpts.OutgoingPacketQueueMaxSizeBytes = EOS_P2P_MAX_QUEUE_SIZE_UNLIMITED;
    EOS_EResult SizeResult = EOS_P2P_SetPacketQueueSize(this->EOSP2P, &SizeOpts);
    if (SizeResult != EOS_EResult::EOS_Success)
    {
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("Unable to set P2P packet queue size to unlimited: %s"),
            ANSI_TO_TCHAR(EOS_EResult_ToString(SizeResult)));
    }

    // Show new P2P queue size after adjustment.
    QueueResult = EOS_P2P_GetPacketQueueInfo(this->EOSP2P, &QueueOpts, &QueueInfo);
    if (QueueResult == EOS_EResult::EOS_Success)
    {
        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("EOS P2P packet queue info: incoming_max_size=%llu, incoming_current_size=%llu, "
                 "incoming_packet_count=%llu, outgoing_max_size=%llu, outgoing_current_size=%llu, "
                 "outgoing_packet_count=%llu"),
            QueueInfo.IncomingPacketQueueMaxSizeBytes,
            QueueInfo.IncomingPacketQueueCurrentSizeBytes,
            QueueInfo.IncomingPacketQueueCurrentPacketCount,
            QueueInfo.OutgoingPacketQueueMaxSizeBytes,
            QueueInfo.OutgoingPacketQueueCurrentSizeBytes,
            QueueInfo.OutgoingPacketQueueCurrentPacketCount);
    }

    // Listen for notifications that the packet queue is full.
    EOS_P2P_AddNotifyIncomingPacketQueueFullOptions QueueNotifyOpts = {};
    QueueNotifyOpts.ApiVersion = EOS_P2P_ADDNOTIFYINCOMINGPACKETQUEUEFULL_API_LATEST;
    this->IncomingPacketQueueFull = EOSRegisterEvent<
        EOS_HP2P,
        EOS_P2P_AddNotifyIncomingPacketQueueFullOptions,
        EOS_P2P_OnIncomingPacketQueueFullInfo>(
        this->EOSP2P,
        &QueueNotifyOpts,
        &EOS_P2P_AddNotifyIncomingPacketQueueFull,
        &EOS_P2P_RemoveNotifyIncomingPacketQueueFull,
        [WeakThis = GetWeakThis(this)](const EOS_P2P_OnIncomingPacketQueueFullInfo *Info) {
            if (auto This = PinWeakThis(WeakThis))
            {
                uint64_t NewPacketQueueSize =
                    Info->PacketQueueMaxSizeBytes + ((uint64_t)Info->OverflowPacketSizeBytes * 2);
                UE_LOG(
                    LogEOS,
                    Warning,
                    TEXT("Incoming packet queue has become full! Max size = %llu, current size = %llu, overflow "
                         "size = %llu, new size = %llu."),
                    Info->PacketQueueMaxSizeBytes,
                    Info->PacketQueueCurrentSizeBytes,
                    Info->OverflowPacketSizeBytes,
                    NewPacketQueueSize);

                EOS_P2P_SetPacketQueueSizeOptions NewSizeOpts = {};
                NewSizeOpts.ApiVersion = EOS_P2P_SETPACKETQUEUESIZE_API_LATEST;
                NewSizeOpts.IncomingPacketQueueMaxSizeBytes = NewPacketQueueSize;
                NewSizeOpts.OutgoingPacketQueueMaxSizeBytes = EOS_P2P_MAX_QUEUE_SIZE_UNLIMITED;
                EOS_EResult SizeResult = EOS_P2P_SetPacketQueueSize(This->EOSP2P, &NewSizeOpts);
                if (SizeResult != EOS_EResult::EOS_Success)
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("Unable to set P2P packet queue size in response to overflow! Packets will be lost! Got "
                             "result code: %s"),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(SizeResult)));
                }
            }
        });
#endif

    // Start ticking to receive P2P packets.
    this->ReceivePacketsDelegateHandle = this->RegulatedTicker->AddTicker(
        FTickerDelegate::CreateSP(this->AsShared(), &FSocketSubsystemEOSFull::Tick_ReceiveP2PPackets));

    return true;
}

void FSocketSubsystemEOSFull::Shutdown()
{
#if EOS_VERSION_AT_LEAST(1, 11, 0)
    // Deregister the packet queue handler.
    this->IncomingPacketQueueFull.Reset();
#endif

    // Stop the packet receive ticker.
    this->RegulatedTicker->RemoveTicker(this->ReceivePacketsDelegateHandle);
    this->ReceivePacketsDelegateHandle.Reset();

    // Remove all sockets from the assignment map, since we are about to free them.
    this->AssignedSockets.Empty();

    // Free all sockets in memory.
    TArray<int> Keys;
    this->HeldSockets.GetKeys(Keys);
    for (int Key : Keys)
    {
        // Because DestroySocket also destroys children, and those children
        // will be removed from the HeldSockets array after we've captured
        // the keys..
        if (this->HeldSockets.Contains(Key))
        {
            FSocketEOSFull *SocketEOS = this->HeldSockets[Key].Get();
            this->DestroySocket(SocketEOS);
        }
    }
    check(this->HeldSockets.Num() == 0);

    // So that IsUnique check will pass during OSS shutdown.
    this->Subsystem.Reset();
}

bool FSocketSubsystemEOSFull::Tick_ReceiveP2PPackets(float DeltaSeconds)
{
    if (!this->Subsystem.IsValid())
    {
        return false;
    }

    if (this->bReceiveSuspended)
    {
        return true;
    }

    IOnlineIdentityPtr Identity = this->Subsystem->GetIdentityInterface();
    if (!Identity.IsValid())
    {
        return true;
    }

    TArray<TSharedPtr<FUserOnlineAccount>> UserAccounts = Identity->GetAllUserAccounts();
    for (const auto &UserAccount : UserAccounts)
    {
        auto UserIdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(UserAccount->GetUserId());
        if (!UserIdEOS->HasValidProductUserId() || UserIdEOS->IsDedicatedServer())
        {
            continue;
        }

        EOS_ProductUserId DestinationUserId = UserIdEOS->GetProductUserId();

        while (true)
        {
            EOS_P2P_GetNextReceivedPacketSizeOptions Opts = {};
            Opts.ApiVersion = EOS_P2P_GETNEXTRECEIVEDPACKETSIZE_API_LATEST;
            Opts.LocalUserId = DestinationUserId;
            Opts.RequestedChannel = nullptr;

            uint32 PendingDataSize;
            if (EOS_P2P_GetNextReceivedPacketSize(this->EOSP2P, &Opts, &PendingDataSize) != EOS_EResult::EOS_Success)
            {
                break;
            }

            void *Buffer = FMemory::Malloc(PendingDataSize);

            EOS_P2P_ReceivePacketOptions ReceiveOpts = {};
            ReceiveOpts.ApiVersion = EOS_P2P_RECEIVEPACKET_API_LATEST;
            ReceiveOpts.LocalUserId = DestinationUserId;
            ReceiveOpts.MaxDataSizeBytes = PendingDataSize;
            ReceiveOpts.RequestedChannel = nullptr;

            EOS_ProductUserId SourceUserId = {};
            EOS_P2P_SocketId SymmetricSocketId = {};
            uint8_t SymmetricChannel = 0;
            uint32_t BytesRead = 0;

            EOS_EResult ReceiveResult = EOS_P2P_ReceivePacket(
                this->EOSP2P,
                &ReceiveOpts,
                &SourceUserId,
                &SymmetricSocketId,
                &SymmetricChannel,
                Buffer,
                &BytesRead);
            if (ReceiveResult != EOS_EResult::EOS_Success)
            {
                UE_LOG(
                    LogEOS,
                    Error,
                    TEXT("Failed to receive packet after EOS_P2P_GetNextReceivedPacketSize returned successful, got "
                         "result code %s from EOS_P2P_ReceivePacket"),
                    ANSI_TO_TCHAR(EOS_EResult_ToString(ReceiveResult)));
                FMemory::Free(Buffer);
                continue;
            }

            EOSLogSocketPacket(SourceUserId, DestinationUserId, SymmetricSocketId, SymmetricChannel, Buffer, BytesRead);

            if (SymmetricChannel == EOS_CHANNEL_ID_CONTROL)
            {
                // This is a reset packet, designed so we can support re-connections on one channel while another
                // channel remains open.

                FSocketEOSKey SocketKey(
                    DestinationUserId,
                    SourceUserId,
                    SymmetricSocketId,
                    (uint8)((uint8 *)Buffer)[0]);

                uint32_t ResetId = *(uint32_t *)(((uint8_t *)Buffer) + 1);
                bool bStoreResetId = true;

                if (this->AssignedSockets.Contains(SocketKey))
                {
                    if (auto SocketToReset = this->AssignedSockets[SocketKey].Pin())
                    {
                        auto State = StaticCastSharedRef<FEOSSocketRoleRemoteState>(SocketToReset->RoleState);

                        if (State->SocketAssignedResetId == 0)
                        {
                            // This is the reset ID for a socket that was opened due to a data packet. We now need to
                            // assign it to socket instead of closing it.
                            UE_LOG(
                                LogEOSSocket,
                                Verbose,
                                TEXT("%s: Received reset packet ID %u, matching unassigned socket. Assigning reset ID "
                                     "to socket."),
                                *SocketKey.ToString(),
                                ResetId);
                            State->SocketAssignedResetId = ResetId;
                            bStoreResetId = false;
                        }
                        else if (State->SocketAssignedResetId == ResetId)
                        {
                            // This packet must be a duplicate? Odd.
                            UE_LOG(
                                LogEOSSocket,
                                Verbose,
                                TEXT("%s: Received reset packet ID %u, identical to socket that is already open, "
                                     "ignoring."),
                                *SocketKey.ToString(),
                                ResetId);
                            bStoreResetId = false;
                        }
                        else
                        {
                            // This reset packet doesn't match the existing socket, thus we must reset the socket. Close
                            // the existing socket first.
                            UE_LOG(
                                LogEOSSocket,
                                Verbose,
                                TEXT("%s: Received reset packet ID %u, closing socket (Socket should re-open itself as "
                                     "a new "
                                     "connection shortly)."),
                                *SocketKey.ToString(),
                                ResetId);
                            SocketToReset->Close();

                            // Store the pending reset ID for the next opened socket.
                            bStoreResetId = true;
                        }
                    }
                }
                else
                {
                    UE_LOG(
                        LogEOSSocket,
                        Verbose,
                        TEXT("%s: Received reset packet ID %u, but no socket exists yet. Storing for when the socket "
                             "is opened later."),
                        *SocketKey.ToString(),
                        ResetId);
                }

                // Store the reset ID if needed (if there was no socket already, or if the existing socket was closed).
                if (bStoreResetId)
                {
                    auto RemoteAddr = MakeShared<FInternetAddrEOSFull>(
                        SourceUserId,
                        EOSString_P2P_SocketName::FromAnsiString(SymmetricSocketId.SocketName),
                        (uint8)((uint8 *)Buffer)[0]);
                    this->PendingResetIds.Add(RemoteAddr->ToString(true), ResetId);
                }
            }
            else
            {
                FSocketEOSKey SocketKey(DestinationUserId, SourceUserId, SymmetricSocketId, SymmetricChannel);

                if (this->AssignedSockets.Contains(SocketKey))
                {
                    if (auto Socket = this->AssignedSockets[SocketKey].Pin())
                    {
                        UE_LOG(LogEOSSocket, VeryVerbose, TEXT("%s: Received packet."), *SocketKey.ToString());

                        Socket->ReceivedPacketsQueueCount++;
                        Socket->ReceivedPacketsQueue.Enqueue(MakeShared<FEOSRawPacket>(Buffer, BytesRead));
                    }
                    else
                    {
                        UE_LOG(
                            LogEOS,
                            Error,
                            TEXT("AssignedSockets contains a stale socket '%s', unable to route traffic."),
                            *SocketKey.ToString());
                    }
                }
                else
                {
                    // This packet might be a new connection for a listening socket.
                    TSharedPtr<FSocketEOSFull> ListeningSocket = nullptr;
                    if (this->ListenManager->GetListeningSocketForNewInboundConnection(SocketKey, ListeningSocket))
                    {
                        UE_LOG(
                            LogEOSSocket,
                            Verbose,
                            TEXT("%s: Registering new socket for inbound connection."),
                            *SocketKey.ToString());
                        TSharedPtr<FInternetAddrEOSFull> SourceAddr = MakeShared<FInternetAddrEOSFull>();
                        SourceAddr->SetFromParameters(
                            SourceUserId,
                            EOSString_P2P_SocketName::FromAnsiString(SymmetricSocketId.SocketName),
                            SymmetricChannel);
                        FSocketEOSFull *AcceptedSocket =
                            (FSocketEOSFull *)ListeningSocket->Accept(*SourceAddr, TEXT("EOS socket (remote)"));
                        check(AcceptedSocket != nullptr);
                        AcceptedSocket->ReceivedPacketsQueueCount++;
                        AcceptedSocket->ReceivedPacketsQueue.Enqueue(MakeShared<FEOSRawPacket>(Buffer, BytesRead));
                    }
                    else
                    {
                        if (GIsAutomationTesting)
                        {
                            UE_LOG(
                                LogEOS,
                                Verbose,
                                TEXT("Packet arrived for intended for socket '%s', but no socket exists to receive the "
                                     "packet. "
                                     "Discarding."),
                                *SocketKey.ToString());
                        }
                        else
                        {
                            UE_LOG(
                                LogEOS,
                                Error,
                                TEXT("Packet arrived for intended for socket '%s', but no socket exists to receive the "
                                     "packet. "
                                     "Discarding."),
                                *SocketKey.ToString());
                        }
                        FMemory::Free(Buffer);
                        continue;
                    }
                }
            }
        }
    }

    return true;
}

EOS_ProductUserId FSocketSubsystemEOSFull::GetBindingProductUserId_P2POnly() const
{
    check(this->Subsystem.IsValid());

    IOnlineIdentityPtr Identity = this->Subsystem->GetIdentityInterface();
    TArray<TSharedPtr<FUserOnlineAccount>> AllAccounts = Identity->GetAllUserAccounts();
    if (AllAccounts.Num() == 0)
    {
        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("EOSNetDriver: No local user is authenticated with EOS, so there is no local user "
                 "ID to use for the address"));
        return nullptr;
    }
    TSharedPtr<FUserOnlineAccount> FirstAccount = AllAccounts[0];
    check(FirstAccount.IsValid());
    TSharedPtr<const FUniqueNetId> FirstPlayerId = FirstAccount->GetUserId();
    check(FirstPlayerId.IsValid());
    TSharedPtr<const FUniqueNetIdEOS> FirstPlayerIdEOS = StaticCastSharedPtr<const FUniqueNetIdEOS>(FirstPlayerId);
    check(FirstPlayerIdEOS->HasValidProductUserId());

    if (FirstPlayerIdEOS->IsDedicatedServer())
    {
        // This is a dedicated server and the product user ID will be empty (but present for API calls).
        // Return nullptr so we don't try to use EOS P2P.
        return nullptr;
    }

    EOS_ProductUserId ProductUserId = FirstPlayerIdEOS->GetProductUserId();
    return ProductUserId;
}

bool FSocketSubsystemEOSFull::GetBindingProductUserId_P2POrDedicatedServer(EOS_ProductUserId &OutPUID) const
{
    check(this->Subsystem.IsValid());

    IOnlineIdentityPtr Identity = this->Subsystem->GetIdentityInterface();
    TArray<TSharedPtr<FUserOnlineAccount>> AllAccounts = Identity->GetAllUserAccounts();
    if (AllAccounts.Num() == 0)
    {
        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("EOSNetDriver: No local user is authenticated with EOS, so there is no local user "
                 "ID to use for the address"));
        OutPUID = nullptr;
        return false;
    }
    TSharedPtr<FUserOnlineAccount> FirstAccount = AllAccounts[0];
    check(FirstAccount.IsValid());
    TSharedPtr<const FUniqueNetId> FirstPlayerId = FirstAccount->GetUserId();
    check(FirstPlayerId.IsValid());
    TSharedPtr<const FUniqueNetIdEOS> FirstPlayerIdEOS = StaticCastSharedPtr<const FUniqueNetIdEOS>(FirstPlayerId);
    check(FirstPlayerIdEOS->HasValidProductUserId());

    if (FirstPlayerIdEOS->IsDedicatedServer())
    {
        OutPUID = nullptr;
    }
    else
    {
        OutPUID = FirstPlayerIdEOS->GetProductUserId();
    }
    return true;
}

FSocket *FSocketSubsystemEOSFull::CreateSocket(
    const FName &SocketType,
    const FString &SocketDescription,
    const FName &ProtocolName)
{
    FSocketEOSMemoryId SocketId = this->NextSocketId++;
    TSharedPtr<FSocketEOSFull> SocketEOS =
        MakeShared<FSocketEOSFull>(this->AsShared(), this->EOSP2P, SocketId, SocketDescription);
    HeldSockets.Add(SocketId, SocketEOS);
    return SocketEOS.Get();
}

void FSocketSubsystemEOSFull::DestroySocket(FSocket *Socket)
{
    check(Socket != nullptr);
    check(Socket->GetProtocol() == REDPOINT_EOS_SUBSYSTEM);
    FSocketEOSFull *EOSSocket = (FSocketEOSFull *)Socket;
    EOSSocket->Close();
    TSharedPtr<FSocketEOSFull> EOSSocketPtr = EOSSocket->AsShared();
    HeldSockets.Remove(EOSSocketPtr->GetSocketMemoryId());
    TArray<TWeakPtr<FSocketEOSFull>> ChildSocketPtrs = EOSSocket->Role->GetOwnedSockets(*EOSSocketPtr);
    while (ChildSocketPtrs.Num() > 0)
    {
        if (TSharedPtr<FSocketEOSFull> FirstChildSocket = ChildSocketPtrs[0].Pin())
        {
            ChildSocketPtrs.RemoveAt(0);
            FSocketEOSFull *ChildSocketRaw = FirstChildSocket.Get();
            FirstChildSocket.Reset();
            this->DestroySocket(ChildSocketRaw);
        }
    }
    check(EOSSocketPtr.IsUnique());
}

FAddressInfoResult FSocketSubsystemEOSFull::GetAddressInfo(
    const TCHAR *HostName,
    const TCHAR *ServiceName,
    EAddressInfoFlags QueryFlags,
    const FName ProtocolTypeName,
    ESocketType SocketType)
{
    return ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)
        ->GetAddressInfo(HostName, ServiceName, QueryFlags, ProtocolTypeName, SocketType);
}

TSharedPtr<FInternetAddr> FSocketSubsystemEOSFull::GetAddressFromString(const FString &InAddress)
{
    TSharedPtr<FInternetAddrEOSFull> Addr = MakeShared<FInternetAddrEOSFull>();
    bool bIsValid;
    Addr->SetIp(*InAddress, bIsValid);
    if (bIsValid)
    {
        return Addr;
    }

    UE_LOG(
        LogEOS,
        Error,
        TEXT("FSocketSubsystemEOSFull::GetAddressFromString asked to generate address from string '%s', but it wasn't "
             "valid. This should never happen, as the EOS socket subsystem should not be used outside "
             "UEOSNetDriver/UEOSNetConnection."));
    return nullptr;
}

bool FSocketSubsystemEOSFull::RequiresChatDataBeSeparate()
{
    return false;
}

bool FSocketSubsystemEOSFull::RequiresEncryptedPackets()
{
    return false;
}

bool FSocketSubsystemEOSFull::GetHostName(FString &HostName)
{
    return false;
}

TSharedRef<FInternetAddr> FSocketSubsystemEOSFull::CreateInternetAddr()
{
    return MakeShared<FInternetAddrEOSFull>();
}

bool FSocketSubsystemEOSFull::HasNetworkDevice()
{
    return true;
}

const TCHAR *FSocketSubsystemEOSFull::GetSocketAPIName() const
{
    return TEXT("EOS");
}

ESocketErrors FSocketSubsystemEOSFull::GetLastErrorCode()
{
    return ESocketErrors::SE_NO_ERROR;
}

ESocketErrors FSocketSubsystemEOSFull::TranslateErrorCode(int32 Code)
{
    return ESocketErrors::SE_NO_ERROR;
}

bool FSocketSubsystemEOSFull::IsSocketWaitSupported() const
{
    return false;
}

void FSocketSubsystemEOSFull::SuspendReceive()
{
    this->bReceiveSuspended = true;
}

void FSocketSubsystemEOSFull::ResumeReceive()
{
    this->bReceiveSuspended = false;
}

EOS_DISABLE_STRICT_WARNINGS