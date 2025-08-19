// Copyright June Rhodes. All Rights Reserved.

#include "EOSSocketRoleNone.h"
#include "EOSSocketRoleClient.h"
#include "EOSSocketRoleListening.h"
#include "SocketEOSFull.h"
#include "SocketSubsystemEOSFull.h"
#include "SocketSubsystemEOSListenManager.h"

EOS_ENABLE_STRICT_WARNINGS

bool FEOSSocketRoleNone::Close(class FSocketEOSFull &Socket) const
{
    // Sockets with the None role don't have anything to close.
    return false;
}

bool FEOSSocketRoleNone::Listen(FSocketEOSFull &Socket, int32 MaxBacklog) const
{
    // Listen indicates that this should be a listening socket.

    auto OldState = Socket.RoleState;
    Socket.RoleState = MakeShared<FEOSSocketRoleListeningState>();
    if (!Socket.SocketSubsystem->RoleInstance_Listening->Listen(Socket, MaxBacklog))
    {
        Socket.RoleState = OldState;
        return false;
    }

    Socket.Role = Socket.SocketSubsystem->RoleInstance_Listening;
    return true;
}

bool FEOSSocketRoleNone::Accept(
    class FSocketEOSFull &Socket,
    TSharedRef<FSocketEOSFull> InRemoteSocket,
    const FInternetAddrEOSFull &InRemoteAddr) const
{
    // Accept can not be called on none sockets.
    return false;
}

bool FEOSSocketRoleNone::Connect(FSocketEOSFull &Socket, const FInternetAddrEOSFull &InDestAddr) const
{
    // Connect indicates that this should be a client socket.

    auto OldState = Socket.RoleState;
    Socket.RoleState = MakeShared<FEOSSocketRoleClientState>();
    if (!Socket.SocketSubsystem->RoleInstance_Client->Connect(Socket, InDestAddr))
    {
        Socket.RoleState = OldState;
        return false;
    }

    Socket.Role = Socket.SocketSubsystem->RoleInstance_Client;
    return true;
}

bool FEOSSocketRoleNone::HasPendingData(FSocketEOSFull &Socket, uint32 &PendingDataSize) const
{
    // Sockets with no role can never have pending data, as they don't have a valid FSocketEOSKey.
    return false;
}

bool FEOSSocketRoleNone::SendTo(
    FSocketEOSFull &Socket,
    const uint8 *Data,
    int32 Count,
    int32 &BytesSent,
    const FInternetAddrEOSFull &Destination) const
{
    // Sockets with no role can not send packets - Connect or Listen should be called instead.
    return false;
}

bool FEOSSocketRoleNone::RecvFrom(
    FSocketEOSFull &Socket,
    uint8 *Data,
    int32 BufferSize,
    int32 &BytesRead,
    FInternetAddr &Source,
    ESocketReceiveFlags::Type Flags) const
{
    // Sockets with no role can never have pending data, as they don't have a valid FSocketEOSKey.
    return false;
}

bool FEOSSocketRoleNone::GetPeerAddress(class FSocketEOSFull &Socket, FInternetAddrEOSFull &OutAddr) const
{
    UE_LOG(LogEOS, Error, TEXT("GetPeerAddress called for socket with the None role."));
    return false;
}

EOS_DISABLE_STRICT_WARNINGS