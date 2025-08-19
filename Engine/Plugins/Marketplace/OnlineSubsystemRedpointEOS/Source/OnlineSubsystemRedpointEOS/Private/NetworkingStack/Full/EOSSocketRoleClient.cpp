// Copyright June Rhodes. All Rights Reserved.

#include "EOSSocketRoleClient.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "SocketEOSFull.h"
#include "SocketTracing.h"
#include "SocketSubsystemEOSFull.h"
#include "SocketSubsystemEOSListenManager.h"

EOS_ENABLE_STRICT_WARNINGS

void FEOSSocketRoleClient::OnRemoteConnectionClosed(
    const TSharedRef<FSocketEOSFull> &Socket,
    const EOS_P2P_OnRemoteConnectionClosedInfo *Info) const
{
    Socket->OnConnectionClosed.ExecuteIfBound(Socket, Socket);
    Socket->Close();
}

bool FEOSSocketRoleClient::Close(FSocketEOSFull &Socket) const
{
    // Sockets are automatically removed from subsystem routing when Close() is called on FSocketEOSFull.
    return true;
}

bool FEOSSocketRoleClient::Listen(FSocketEOSFull &Socket, int32 MaxBacklog) const
{
    // Listen can not be called on client sockets.
    check(false);
    return false;
}

bool FEOSSocketRoleClient::Accept(
    class FSocketEOSFull &Socket,
    TSharedRef<FSocketEOSFull> InRemoteSocket,
    const FInternetAddrEOSFull &InRemoteAddr) const
{
    // Accept can not be called on client sockets.
    check(false);
    return false;
}

bool FEOSSocketRoleClient::Connect(FSocketEOSFull &Socket, const FInternetAddrEOSFull &InDestAddr) const
{
    auto State = this->GetState<FEOSSocketRoleClientState>(Socket);
    State->RemoteAddr = MakeShared<FInternetAddrEOSFull>();
    State->RemoteAddr->SetRawIp(InDestAddr.GetRawIp());
    State->bDidSendChannelReset = false;
    State->SocketResetId = Socket.SocketSubsystem->GetResetIdForOutboundConnection();
    check(State->RemoteAddr->GetSymmetricChannel() != 0);

    // Ensure the socket is bound.
    if (!Socket.BindAddress.IsValid())
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Attempted to call FEOSSocketRoleClient::Connect but the socket has not been locally bound."));
        return false;
    }

    Socket.UpdateSocketKey(FSocketEOSKey(
        Socket.BindAddress->GetUserId(),
        State->RemoteAddr->GetUserId(),
        State->RemoteAddr->GetSymmetricSocketId(),
        State->RemoteAddr->GetSymmetricChannel()));

    UE_LOG(
        LogEOSSocket,
        Verbose,
        TEXT("%s: Client socket opened with reset ID %u."),
        *Socket.SocketKey->ToString(false),
        State->SocketResetId);

    // Reset connections (via reset packets) are handled in the socket subsystem, but full
    // disconnections are handled in the listen manager. Since the listen manager only manages
    // non-client sockets, we need to listen for full connection close events for clients here
    // so that we can appropriately call Close and fire OnConnectionClosed.
    {
        EOS_P2P_SocketId SocketId = Socket.BindAddress->GetSymmetricSocketId();
        EOS_P2P_AddNotifyPeerConnectionClosedOptions CloseOpts = {};
        CloseOpts.ApiVersion = EOS_P2P_ADDNOTIFYPEERCONNECTIONCLOSED_API_LATEST;
        CloseOpts.LocalUserId = Socket.BindAddress->GetUserId();
        CloseOpts.SocketId = &SocketId;

        State->OnClosedEvent = EOSRegisterEvent<
            EOS_HP2P,
            EOS_P2P_AddNotifyPeerConnectionClosedOptions,
            EOS_P2P_OnRemoteConnectionClosedInfo>(
            Socket.EOSP2P,
            &CloseOpts,
            EOS_P2P_AddNotifyPeerConnectionClosed,
            EOS_P2P_RemoveNotifyPeerConnectionClosed,
            [WeakSocket = GetWeakThis(StaticCastSharedRef<FSocketEOSFull>(Socket.AsSharedEOS())),
             WeakThis = GetWeakThis(StaticCastSharedRef<const FEOSSocketRoleClient>(this->AsShared()))](
                const EOS_P2P_OnRemoteConnectionClosedInfo *Info) {
                if (auto Socket = PinWeakThis(WeakSocket))
                {
                    if (auto This = PinWeakThis(WeakThis))
                    {
                        This->OnRemoteConnectionClosed(Socket.ToSharedRef(), Info);
                    }
                }
            });
    }

    return true;
}

bool FEOSSocketRoleClient::HasPendingData(FSocketEOSFull &Socket, uint32 &PendingDataSize) const
{
    if (!Socket.ReceivedPacketsQueue.IsEmpty())
    {
        PendingDataSize = Socket.ReceivedPacketsQueue.Peek()->Get()->GetDataLen();
        return true;
    }
    return false;
}

bool FEOSSocketRoleClient::SendTo(
    FSocketEOSFull &Socket,
    const uint8 *Data,
    int32 Count,
    int32 &BytesSent,
    const FInternetAddrEOSFull &Destination) const
{
    auto State = this->GetState<FEOSSocketRoleClientState>(Socket);
    check(State->RemoteAddr.IsValid());

    // Ensure that the address we are sending to is our remote address.
    check(Destination == *State->RemoteAddr);

    // Ensure the socket is bound.
    if (!Socket.BindAddress.IsValid())
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Attempted to call FEOSSocketRoleClient::SendTo but the socket has not been locally bound."));
        return false;
    }

    // Send the channel reset if it hasn't already been done.
    if (!State->bDidSendChannelReset)
    {
        EOS_P2P_SocketId SocketId = State->RemoteAddr->GetSymmetricSocketId();

        check(State->SocketResetId != 0);

        uint8 ChannelToResetAndId[5] = {
            State->RemoteAddr->GetSymmetricChannel(),
            ((uint8 *)(&State->SocketResetId))[0],
            ((uint8 *)(&State->SocketResetId))[1],
            ((uint8 *)(&State->SocketResetId))[2],
            ((uint8 *)(&State->SocketResetId))[3],
        };

        EOS_P2P_SendPacketOptions Opts = {};
        Opts.ApiVersion = EOS_P2P_SENDPACKET_API_LATEST;
        Opts.LocalUserId = Socket.BindAddress->GetUserId();
        Opts.RemoteUserId = State->RemoteAddr->GetUserId();
        Opts.SocketId = &SocketId;
        Opts.Channel = EOS_CHANNEL_ID_CONTROL;
        Opts.DataLengthBytes = 5;
        Opts.Data = ChannelToResetAndId;
        Opts.bAllowDelayedDelivery = true;
        Opts.Reliability = EOS_EPacketReliability::EOS_PR_ReliableOrdered;

        EOSLogSocketPacket(Opts);

        EOS_EResult SendResult = EOS_P2P_SendPacket(Socket.EOSP2P, &Opts);

        if (SendResult == EOS_EResult::EOS_Success)
        {
            State->bDidSendChannelReset = true;
        }
        else
        {
            UE_LOG(LogEOSSocket, Warning, TEXT("Unable to send channel reset message."));
            BytesSent = 0;
            return false;
        }
    }

    // Send the actual packet.
    {
        EOS_P2P_SocketId SocketId = State->RemoteAddr->GetSymmetricSocketId();

        EOS_P2P_SendPacketOptions Opts = {};
        Opts.ApiVersion = EOS_P2P_SENDPACKET_API_LATEST;
        Opts.LocalUserId = Socket.BindAddress->GetUserId();
        Opts.RemoteUserId = State->RemoteAddr->GetUserId();
        Opts.SocketId = &SocketId;
        Opts.Channel = State->RemoteAddr->GetSymmetricChannel();
        Opts.DataLengthBytes = Count;
        Opts.Data = Data;
        Opts.bAllowDelayedDelivery = Socket.bEnableReliableSending;
        Opts.Reliability = Socket.bEnableReliableSending ? EOS_EPacketReliability::EOS_PR_ReliableOrdered
                                                         : EOS_EPacketReliability::EOS_PR_UnreliableUnordered;

        EOSLogSocketPacket(Opts);

        EOS_EResult SendResult = EOS_P2P_SendPacket(Socket.EOSP2P, &Opts);

        if (SendResult == EOS_EResult::EOS_Success)
        {
            BytesSent = Count;
            return true;
        }

        BytesSent = 0;
        return false;
    }
}

bool FEOSSocketRoleClient::RecvFrom(
    FSocketEOSFull &Socket,
    uint8 *Data,
    int32 BufferSize,
    int32 &BytesRead,
    FInternetAddr &Source,
    ESocketReceiveFlags::Type Flags) const
{
    TSharedPtr<FEOSRawPacket> ReceivedPacket;
    if (Socket.ReceivedPacketsQueue.Dequeue(ReceivedPacket))
    {
        Socket.ReceivedPacketsQueueCount--;
        check(ReceivedPacket.IsValid());
        int32 SizeToRead = FMath::Min(BufferSize, ReceivedPacket->GetDataLen());
        FMemory::Memcpy(Data, ReceivedPacket->GetData(), SizeToRead);
        BytesRead = SizeToRead;
        Source.SetRawIp(this->GetState<FEOSSocketRoleClientState>(Socket)->RemoteAddr->GetRawIp());
        return true;
    }

    return false;
}

bool FEOSSocketRoleClient::GetPeerAddress(class FSocketEOSFull &Socket, FInternetAddrEOSFull &OutAddr) const
{
    OutAddr.SetRawIp(this->GetState<FEOSSocketRoleClientState>(Socket)->RemoteAddr->GetRawIp());
    return true;
}

EOS_DISABLE_STRICT_WARNINGS