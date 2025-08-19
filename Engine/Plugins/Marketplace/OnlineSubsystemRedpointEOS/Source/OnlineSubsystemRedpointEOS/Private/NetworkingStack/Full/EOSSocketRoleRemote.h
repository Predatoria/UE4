// Copyright June Rhodes. All Rights Reserved.

#pragma once

class FSocketEOSFull;

#include "CoreMinimal.h"
#include "EOSSocketRole.h"

EOS_ENABLE_STRICT_WARNINGS

/**
 * Remote sockets are created and managed by Listening sockets, and they represent remote sockets in the Client role.
 * They only ever communicate with the client they are associated with.
 */
class FEOSSocketRoleRemote : public IEOSSocketRole
{
public:
    UE_NONCOPYABLE(FEOSSocketRoleRemote);
    FEOSSocketRoleRemote() = default;
    virtual ~FEOSSocketRoleRemote() = default;

    virtual FName GetRoleName() const override
    {
        return FName(TEXT("Remote"));
    }

    virtual bool Close(FSocketEOSFull &Socket) const override;
    virtual bool Connect(FSocketEOSFull &Socket, const FInternetAddrEOSFull &InDestAddr) const override;
    virtual bool Listen(FSocketEOSFull &Socket, int32 MaxBacklog) const override;
    virtual bool Accept(
        FSocketEOSFull &Socket,
        TSharedRef<FSocketEOSFull> InRemoteSocket,
        const FInternetAddrEOSFull &InRemoteAddr) const override;
    virtual bool HasPendingData(FSocketEOSFull &Socket, uint32 &PendingDataSize) const override;
    virtual bool SendTo(
        FSocketEOSFull &Socket,
        const uint8 *Data,
        int32 Count,
        int32 &BytesSent,
        const FInternetAddrEOSFull &Destination) const override;
    virtual bool RecvFrom(
        FSocketEOSFull &Socket,
        uint8 *Data,
        int32 BufferSize,
        int32 &BytesRead,
        FInternetAddr &Source,
        ESocketReceiveFlags::Type Flags) const override;
    virtual bool GetPeerAddress(FSocketEOSFull &Socket, FInternetAddrEOSFull &OutAddr) const override;
};

class FEOSSocketRoleRemoteState : public IEOSSocketRoleState
{
public:
    UE_NONCOPYABLE(FEOSSocketRoleRemoteState);
    FEOSSocketRoleRemoteState() = default;
    virtual ~FEOSSocketRoleRemoteState() = default;

    // This is the address of the client that this remote socket represents.
    TSharedPtr<FInternetAddrEOSFull> RemoteAddr;

    // This field contains the local socket that owns it.
    TWeakPtr<FSocketEOSFull> RemoteSocketParent;

    // The assigned reset ID from the reset packet that was sent when this connection was established. A value of 0
    // indicates that this socket has not been assigned a reset ID yet (i.e. a normal data packet arrived first and
    // caused a new connection to be opened).
    uint32_t SocketAssignedResetId;
};

EOS_DISABLE_STRICT_WARNINGS
