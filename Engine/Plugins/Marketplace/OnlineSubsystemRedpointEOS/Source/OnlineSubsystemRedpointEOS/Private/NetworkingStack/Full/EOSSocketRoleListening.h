// Copyright June Rhodes. All Rights Reserved.

#pragma once

class FSocketEOSFull;
class IEOSSocketRole;

#include "CoreMinimal.h"
#include "EOSSocketRole.h"

EOS_ENABLE_STRICT_WARNINGS

/**
 * Listening sockets receive events from the subsystem when incoming connections are created or closed, and manages the
 * Remote sockets appropriately. The SendTo/RecvFrom implementations delegate to the remote sockets as appropriate.
 */
class FEOSSocketRoleListening : public IEOSSocketRole
{
public:
    UE_NONCOPYABLE(FEOSSocketRoleListening);
    FEOSSocketRoleListening() = default;
    virtual ~FEOSSocketRoleListening() = default;

    virtual FName GetRoleName() const override
    {
        return FName(TEXT("Listening"));
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

    virtual const TArray<TWeakPtr<FSocketEOSFull>> GetOwnedSockets(FSocketEOSFull &Socket) const override;
};

class FEOSSocketRoleListeningState : public IEOSSocketRoleState
{
public:
    UE_NONCOPYABLE(FEOSSocketRoleListeningState);
    FEOSSocketRoleListeningState() = default;
    virtual ~FEOSSocketRoleListeningState() = default;

    // A list of remote sockets that this listening socket owns.
    TArray<TWeakPtr<FSocketEOSFull>> RemoteSockets;
};

EOS_DISABLE_STRICT_WARNINGS
