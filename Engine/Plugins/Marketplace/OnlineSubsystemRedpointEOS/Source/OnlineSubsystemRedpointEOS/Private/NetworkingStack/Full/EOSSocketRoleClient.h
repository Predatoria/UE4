// Copyright June Rhodes. All Rights Reserved.

#pragma once

class FSocketEOSFull;

#include "CoreMinimal.h"
#include "EOSSocketRole.h"

EOS_ENABLE_STRICT_WARNINGS

/**
 * Client sockets are outbound connections that are actively made. That is, this machine actively connects to a remote
 * socket (that is in the Listening role), and this socket only ever communicates with that remote socket. Calling
 * SendTo for an address that doesn't match the remote address will fail. Calling RecvFrom will only ever receive
 * packets from that address.
 */
class FEOSSocketRoleClient : public IEOSSocketRole
{
private:
    void OnRemoteConnectionClosed(
        const TSharedRef<FSocketEOSFull> &Socket,
        const EOS_P2P_OnRemoteConnectionClosedInfo *Info) const;

public:
    UE_NONCOPYABLE(FEOSSocketRoleClient);
    FEOSSocketRoleClient() = default;
    virtual ~FEOSSocketRoleClient() = default;

    virtual FName GetRoleName() const override
    {
        return FName(TEXT("Client"));
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

class FEOSSocketRoleClientState : public IEOSSocketRoleState
{
public:
    UE_NONCOPYABLE(FEOSSocketRoleClientState);
    FEOSSocketRoleClientState() = default;
    virtual ~FEOSSocketRoleClientState() = default;

    // This is the address of the remote server that this client socket is connected to.
    TSharedPtr<FInternetAddrEOSFull> RemoteAddr;

    // When we first connect to a channel, we need to ensure that it is "reset" on the other side. This is because
    // connections to remote players are only fully closed when all channels are closed, but we may want to perform a
    // re-connect on channel A while B stays open the whole time.
    bool bDidSendChannelReset;

    // The reset ID that we will send when we first send a packet to the remote host. This mitigates timing issues with
    // sending the reset packet.
    uint32_t SocketResetId;

    // The event handler for full connection closures.
    TSharedPtr<EOSEventHandle<EOS_P2P_OnRemoteConnectionClosedInfo>> OnClosedEvent;
};

EOS_DISABLE_STRICT_WARNINGS
