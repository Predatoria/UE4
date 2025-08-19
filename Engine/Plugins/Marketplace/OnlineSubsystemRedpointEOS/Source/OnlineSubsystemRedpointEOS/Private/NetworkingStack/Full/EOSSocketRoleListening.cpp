// Copyright June Rhodes. All Rights Reserved.

#include "EOSSocketRoleListening.h"
#include "EOSSocketRoleRemote.h"
#include "SocketEOSFull.h"
#include "SocketSubsystemEOSFull.h"
#include "SocketSubsystemEOSListenManager.h"

EOS_ENABLE_STRICT_WARNINGS

bool FEOSSocketRoleListening::Close(FSocketEOSFull &Socket) const
{
    // Ensure the socket is bound.
    if (!Socket.BindAddress.IsValid())
    {
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("Attempted to call FEOSSocketRoleListening::Close but the socket has not been locally bound."));
        return false;
    }

    return Socket.SocketSubsystem->ListenManager->Remove(Socket, *Socket.BindAddress);
}

bool FEOSSocketRoleListening::Connect(FSocketEOSFull &Socket, const FInternetAddrEOSFull &InDestAddr) const
{
    // Connect can not be called on Listening sockets.
    return false;
}

bool FEOSSocketRoleListening::Listen(FSocketEOSFull &Socket, int32 MaxBacklog) const
{
    // Ensure the socket is bound.
    if (!Socket.BindAddress.IsValid())
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Attempted to call FEOSSocketRoleListening::Listen but the socket has not been locally bound."));
        return false;
    }

    return Socket.SocketSubsystem->ListenManager->Add(Socket, *Socket.BindAddress);
}

bool FEOSSocketRoleListening::Accept(
    class FSocketEOSFull &Socket,
    TSharedRef<FSocketEOSFull> InRemoteSocket,
    const FInternetAddrEOSFull &InRemoteAddr) const
{
    // Ensure the socket is bound.
    if (!Socket.BindAddress.IsValid())
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Attempted to call FEOSSocketRoleListening::Accept but the socket has not been locally bound."));
        return false;
    }

    this->GetState<FEOSSocketRoleListeningState>(Socket)->RemoteSockets.Add(InRemoteSocket);

    auto RemoteState = MakeShared<FEOSSocketRoleRemoteState>();
    RemoteState->RemoteAddr = MakeShared<FInternetAddrEOSFull>();
    RemoteState->RemoteAddr->SetRawIp(InRemoteAddr.GetRawIp());
    RemoteState->RemoteSocketParent = Socket.AsShared();
    checkf(
        EOSString_ProductUserId::IsValid(RemoteState->RemoteAddr->GetUserId()),
        TEXT("User ID for incoming address must be valid!"));

    InRemoteSocket->RoleState = RemoteState;
    InRemoteSocket->Role = Socket.SocketSubsystem->RoleInstance_Remote;

    InRemoteSocket->UpdateSocketKey(FSocketEOSKey(
        Socket.BindAddress->GetUserId(),
        InRemoteAddr.GetUserId(),
        InRemoteAddr.GetSymmetricSocketId(),
        InRemoteAddr.GetSymmetricChannel()));

    auto PendingResetKey = InRemoteAddr.ToString(true);
    if (Socket.SocketSubsystem->PendingResetIds.Contains(PendingResetKey))
    {
        RemoteState->SocketAssignedResetId = Socket.SocketSubsystem->PendingResetIds[PendingResetKey];
        Socket.SocketSubsystem->PendingResetIds.Remove(PendingResetKey);

        UE_LOG(
            LogEOSSocket,
            Verbose,
            TEXT("%s: When remote socket was opened, found a pending reset ID of %u to use."),
            *InRemoteSocket->SocketKey->ToString(false),
            RemoteState->SocketAssignedResetId);
    }
    else
    {
        // We have received data before the reset ID packet.
        RemoteState->SocketAssignedResetId = 0;

        UE_LOG(
            LogEOSSocket,
            Verbose,
            TEXT(
                "%s: When remote socket was opened, no pending reset ID was found. Expect one to be assigned shortly."),
            *InRemoteSocket->SocketKey->ToString(false));
    }

    return true;
}

bool FEOSSocketRoleListening::HasPendingData(FSocketEOSFull &Socket, uint32 &PendingDataSize) const
{
    for (const auto &RemoteSocketWk : this->GetState<FEOSSocketRoleListeningState>(Socket)->RemoteSockets)
    {
        if (auto RemoteSocket = RemoteSocketWk.Pin())
        {
            if (!RemoteSocket->ReceivedPacketsQueue.IsEmpty())
            {
                PendingDataSize = RemoteSocket->ReceivedPacketsQueue.Peek()->Get()->GetDataLen();
                return true;
            }
        }
    }

    return false;
}

bool FEOSSocketRoleListening::SendTo(
    FSocketEOSFull &Socket,
    const uint8 *Data,
    int32 Count,
    int32 &BytesSent,
    const FInternetAddrEOSFull &Destination) const
{
    for (const auto &RemoteSocketWk : this->GetState<FEOSSocketRoleListeningState>(Socket)->RemoteSockets)
    {
        if (auto RemoteSocket = RemoteSocketWk.Pin())
        {
            TSharedPtr<FInternetAddrEOSFull> PeerAddress = MakeShared<FInternetAddrEOSFull>();
            if (RemoteSocket->GetPeerAddress(*PeerAddress))
            {
                if (*PeerAddress == Destination)
                {
                    RemoteSocket->Role->SendTo(*RemoteSocket, Data, Count, BytesSent, Destination);
                    return true;
                }
            }
        }
    }

    UE_LOG(LogEOS, Error, TEXT("SentTo on a listening socket didn't match any connected client!"));
    return false;
}

bool FEOSSocketRoleListening::RecvFrom(
    FSocketEOSFull &Socket,
    uint8 *Data,
    int32 BufferSize,
    int32 &BytesRead,
    FInternetAddr &Source,
    ESocketReceiveFlags::Type Flags) const
{
    for (const auto &RemoteSocketWk : this->GetState<FEOSSocketRoleListeningState>(Socket)->RemoteSockets)
    {
        if (auto RemoteSocket = RemoteSocketWk.Pin())
        {
            if (!RemoteSocket->ReceivedPacketsQueue.IsEmpty())
            {
                return RemoteSocket->Role->RecvFrom(*RemoteSocket, Data, BufferSize, BytesRead, Source, Flags);
            }
        }
    }

    return false;
}

bool FEOSSocketRoleListening::GetPeerAddress(class FSocketEOSFull &Socket, FInternetAddrEOSFull &OutAddr) const
{
    UE_LOG(LogEOS, Error, TEXT("GetPeerAddress called on a listening socket!"));
    return false;
}

const TArray<TWeakPtr<FSocketEOSFull>> FEOSSocketRoleListening::GetOwnedSockets(class FSocketEOSFull &Socket) const
{
    return this->GetState<FEOSSocketRoleListeningState>(Socket)->RemoteSockets;
}

EOS_DISABLE_STRICT_WARNINGS