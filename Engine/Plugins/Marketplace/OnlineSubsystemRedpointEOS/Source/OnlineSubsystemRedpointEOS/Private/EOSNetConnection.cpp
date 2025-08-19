// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/EOSNetConnection.h"

#include "IPAddress.h"
#include "Net/DataChannel.h"
#include "Net/NetworkProfiler.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/ISocketEOS.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/ISocketSubsystemEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "TimerManager.h"

EOS_ENABLE_STRICT_WARNINGS

// According to USteamSocketsNetConnection, UNetConnection can assert if this value is greater
// than 1024 (though I can't find out where it would do so).
static const int32 MAX_PACKET = EOS_P2P_MAX_PACKET_SIZE > 1024 ? 1024 : EOS_P2P_MAX_PACKET_SIZE;

void UEOSNetConnection::LogPacket(
    UNetDriver* NetDriver,
    const TSharedPtr<const FInternetAddr> &InRemoteAddr,
    bool bIncoming,
    const uint8 *Data,
    int32 CountBytes)
{
#if defined(EOS_ENABLE_SOCKET_LEVEL_NETWORK_TRACING)
    UE_LOG(
        LogEOSNetworkTrace,
        Verbose,
        TEXT("'%s' %s '%s': %s"),
        *NetDriver->GetLocalAddr()->ToString(true),
        bIncoming ? TEXT("<-") : TEXT("->"),
        *(InRemoteAddr->ToString(true)),
        *FString::FromBlob(Data, CountBytes));
#endif
}

void UEOSNetConnection::ReceivePacketFromDriver(
    const TSharedPtr<FInternetAddr> &ReceivedAddr,
    uint8 *Buffer,
    int32 BufferSize)
{
    // We do not use the packet handler infrastructure, so just forward
    // the received packet straight into the networking stack.

#if defined(UE_5_0_OR_LATER)
    UEOSNetConnection::LogPacket(this->Driver.Get(), ReceivedAddr, true, Buffer, BufferSize);
#else
    UEOSNetConnection::LogPacket(this->Driver, ReceivedAddr, true, Buffer, BufferSize);
#endif
    this->ReceivedRawPacket(Buffer, BufferSize);
}

void UEOSNetConnection::InitHandler()
{
    // We do not use the packet handler infrastructure on P2P connections;
    // we don't need stateless connection handling or encryption handling.
}

void UEOSNetConnection::InitBase(
    UNetDriver *InDriver,
    FSocket *InSocket,
    const FURL &InURL,
    EConnectionState InState,
    int32 InMaxPacket,
    int32 InPacketOverhead)
{
    UNetConnection::InitBase(
        InDriver,
        InSocket,
        InURL,
        InState,
        ((InMaxPacket == 0) ? MAX_PACKET : InMaxPacket),
        ((InPacketOverhead == 0) ? 1 : InPacketOverhead));

    this->Socket = ((ISocketEOS *)InSocket)->AsSharedEOS();
}

void UEOSNetConnection::InitRemoteConnection(
    UNetDriver *InDriver,
    FSocket *InSocket,
    const FURL &InURL,
    const FInternetAddr &InRemoteAddr,
    EConnectionState InState,
    int32 InMaxPacket,
    int32 InPacketOverhead)
{
    this->InitBase(InDriver, InSocket, InURL, InState, InMaxPacket, InPacketOverhead);

    this->RemoteAddr = InRemoteAddr.Clone();

    this->InitSendBuffer();
    this->SetClientLoginState(EClientLoginState::LoggingIn);
    this->SetExpectedClientLoginMsgType(NMT_Hello);
}

void UEOSNetConnection::InitLocalConnection(
    UNetDriver *InDriver,
    FSocket *InSocket,
    const FURL &InURL,
    EConnectionState InState,
    int32 InMaxPacket,
    int32 InPacketOverhead)
{
    this->InitBase(InDriver, InSocket, InURL, InState, InMaxPacket, InPacketOverhead);

    // Get the remote address (the one we are connecting to) from the InURL.
    this->RemoteAddr =
        InDriver->GetSocketSubsystem()->GetAddressFromString(FString::Printf(TEXT("%s:%d"), *InURL.Host, InURL.Port));

    this->InitSendBuffer();
}

void UEOSNetConnection::LowLevelSend(void *Data, int32 CountBits, FOutPacketTraits &Traits)
{
    TSharedPtr<ISocketEOS> SocketPinned = this->Socket.Pin();
    if (SocketPinned.IsValid())
    {
        const uint8 *SendData = reinterpret_cast<const uint8 *>(Data);

        // We don't use the packet handler infrastructure, so there's no
        // need to mutate the packet data with the handler.

        int32 BytesSent = 0;
        int32 BytesToSend = FMath::DivideAndRoundUp(CountBits, 8);

        if (BytesToSend > 0)
        {
#if defined(UE_5_0_OR_LATER)
            UEOSNetConnection::LogPacket(this->Driver.Get(), RemoteAddr, true, SendData, BytesSent);
#else
            UEOSNetConnection::LogPacket(this->Driver, RemoteAddr, true, SendData, BytesSent);
#endif
            if (!SocketPinned->SendTo(SendData, BytesToSend, BytesSent, *RemoteAddr))
            {
                UE_LOG(
                    LogNet,
                    Warning,
                    TEXT("UEOSNetConnection::LowLevelSend: Could not send %d data over socket to %s!"),
                    BytesToSend,
                    *RemoteAddr->ToString(true));
            }
            else
            {
                EOS_INC_DWORD_STAT(STAT_EOSNetP2PSentPackets);
                EOS_INC_DWORD_STAT_BY(STAT_EOSNetP2PSentBytes, BytesToSend);
                EOS_TRACE_COUNTER_INCREMENT(CTR_EOSNetP2PSentPackets);
                EOS_TRACE_COUNTER_ADD(CTR_EOSNetP2PSentBytes, BytesToSend);

                NETWORK_PROFILER(GNetworkProfiler.FlushOutgoingBunches(this));
                NETWORK_PROFILER(GNetworkProfiler.TrackSocketSendTo(
                    SocketPinned->GetDescription(),
                    SendData,
                    (uint16)BytesToSend,
                    (uint16)NumPacketIdBits,
                    (uint16)NumBunchBits,
                    (uint16)NumAckBits,
                    (uint16)NumPaddingBits,
                    this));
            }
        }
    }
    else
    {
        UE_LOG(LogNet, Error, TEXT("UEOSNetConnection::LowLevelSend: Could not send data because the socket is gone!"));
    }
}

FString UEOSNetConnection::LowLevelGetRemoteAddress(bool bAppendPort)
{
    if (this->RemoteAddr == nullptr)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("UEOSNetConnection::LowLevelGetRemoteAddress called on connection without a valid RemoteAddr. Has the "
                 "connection been closed?"));
        return TEXT("");
    }
    return this->RemoteAddr->ToString(true);
}

FString UEOSNetConnection::LowLevelDescribe()
{
    return TEXT("EOS Net Connection");
}

EOS_DISABLE_STRICT_WARNINGS