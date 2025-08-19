// Copyright June Rhodes. All Rights Reserved.

#include "EOSSocketRoleRemote.h"
#include "EOSSocketRoleListening.h"
#include "SocketEOSFull.h"
#include "SocketSubsystemEOSFull.h"
#include "SocketSubsystemEOSListenManager.h"
#include "SocketTracing.h"

EOS_ENABLE_STRICT_WARNINGS

bool FEOSSocketRoleRemote::Close(class FSocketEOSFull &Socket) const
{
    // Get our parent listening socket.
    auto State = this->GetState<FEOSSocketRoleRemoteState>(Socket);
    TSharedPtr<FSocketEOSFull> ListeningSocket = State->RemoteSocketParent.Pin();
    if (ListeningSocket.IsValid())
    {
        // Notify the listening socket that a remote connection is closing.
        ListeningSocket->OnConnectionClosed.ExecuteIfBound(ListeningSocket.ToSharedRef(), Socket.AsShared());

        // Remove ourselves from the listening socket.
        auto ListeningRoleState = StaticCastSharedRef<FEOSSocketRoleListeningState>(ListeningSocket->RoleState);
        for (int i = ListeningRoleState->RemoteSockets.Num() - 1; i >= 0; i--)
        {
            if (ListeningRoleState->RemoteSockets[i].Pin().Get() == &Socket)
            {
                ListeningRoleState->RemoteSockets.RemoveAt(i);
            }
        }
    }

    // Sockets are automatically removed from subsystem routing when Close() is called on FSocketEOSFull.

    return true;
}

bool FEOSSocketRoleRemote::Connect(class FSocketEOSFull &Socket, const FInternetAddrEOSFull &InDestAddr) const
{
    check(false /* Connect can not be called on a remote socket */);
    UE_LOG(LogEOS, Error, TEXT("Connect called on a remote socket!"));
    return false;
}

bool FEOSSocketRoleRemote::Listen(class FSocketEOSFull &Socket, int32 MaxBacklog) const
{
    check(false /* Listen can not be called on a remote socket */);
    UE_LOG(LogEOS, Error, TEXT("Listen called on a remote socket!"));
    return false;
}

bool FEOSSocketRoleRemote::Accept(
    class FSocketEOSFull &Socket,
    TSharedRef<FSocketEOSFull> InRemoteSocket,
    const FInternetAddrEOSFull &InRemoteAddr) const
{
    check(false /* Accept can not be called on a remote socket */);
    UE_LOG(LogEOS, Error, TEXT("Listen called on a remote socket!"));
    return false;
}

bool FEOSSocketRoleRemote::HasPendingData(class FSocketEOSFull &Socket, uint32 &PendingDataSize) const
{
    if (!Socket.ReceivedPacketsQueue.IsEmpty())
    {
        PendingDataSize = Socket.ReceivedPacketsQueue.Peek()->Get()->GetDataLen();
        return true;
    }
    return false;
}

bool FEOSSocketRoleRemote::SendTo(
    class FSocketEOSFull &Socket,
    const uint8 *Data,
    int32 Count,
    int32 &BytesSent,
    const FInternetAddrEOSFull &Destination) const
{
    auto State = this->GetState<FEOSSocketRoleRemoteState>(Socket);
    TSharedPtr<FSocketEOSFull> ParentSocket = State->RemoteSocketParent.Pin();
    check(ParentSocket.IsValid());

    // Ensure that the address we are sending to is our remote address.
    check(Destination == *State->RemoteAddr);

    EOS_P2P_SocketId SocketId = State->RemoteAddr->GetSymmetricSocketId();

    EOS_P2P_SendPacketOptions Opts = {};
    Opts.ApiVersion = EOS_P2P_SENDPACKET_API_LATEST;
    Opts.LocalUserId = ParentSocket->BindAddress->GetUserId();
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

bool FEOSSocketRoleRemote::RecvFrom(
    class FSocketEOSFull &Socket,
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
        int32 SizeToRead = FMath::Min(BufferSize, ReceivedPacket->GetDataLen());
        FMemory::Memcpy(Data, ReceivedPacket->GetData(), SizeToRead);
        BytesRead = SizeToRead;
        Source.SetRawIp(this->GetState<FEOSSocketRoleRemoteState>(Socket)->RemoteAddr->GetRawIp());
        return true;
    }

    return false;
}

bool FEOSSocketRoleRemote::GetPeerAddress(class FSocketEOSFull &Socket, FInternetAddrEOSFull &OutAddr) const
{
    OutAddr.SetRawIp(this->GetState<FEOSSocketRoleRemoteState>(Socket)->RemoteAddr->GetRawIp());
    return true;
}

EOS_DISABLE_STRICT_WARNINGS