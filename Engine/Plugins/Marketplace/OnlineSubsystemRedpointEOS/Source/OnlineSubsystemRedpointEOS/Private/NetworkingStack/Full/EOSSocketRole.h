// Copyright June Rhodes. All Rights Reserved.

#pragma once

class FSocketEOSFull;

#include "CoreMinimal.h"
#include "InternetAddrEOSFull.h"
#include "SocketEOSFull.h"
#include "Sockets.h"

EOS_ENABLE_STRICT_WARNINGS

class IEOSSocketRole : public TSharedFromThis<IEOSSocketRole>
{
protected:
    template <typename TState> TSharedRef<TState> GetState(FSocketEOSFull &Socket) const
    {
        return StaticCastSharedRef<TState>(Socket.RoleState);
    }

public:
    UE_NONCOPYABLE(IEOSSocketRole);
    IEOSSocketRole() = default;
    virtual ~IEOSSocketRole() = default;

    virtual FName GetRoleName() const = 0;

    virtual bool Close(FSocketEOSFull &Socket) const = 0;
    virtual bool Connect(FSocketEOSFull &Socket, const FInternetAddrEOSFull &InDestAddr) const = 0;
    virtual bool Listen(FSocketEOSFull &Socket, int32 MaxBacklog) const = 0;
    virtual bool Accept(
        FSocketEOSFull &Socket,
        TSharedRef<FSocketEOSFull> InRemoteSocket,
        const FInternetAddrEOSFull &InRemoteAddr) const = 0;
    virtual bool HasPendingData(FSocketEOSFull &Socket, uint32 &PendingDataSize) const = 0;
    virtual bool SendTo(
        FSocketEOSFull &Socket,
        const uint8 *Data,
        int32 Count,
        int32 &BytesSent,
        const FInternetAddrEOSFull &Destination) const = 0;
    virtual bool RecvFrom(
        FSocketEOSFull &Socket,
        uint8 *Data,
        int32 BufferSize,
        int32 &BytesRead,
        FInternetAddr &Source,
        ESocketReceiveFlags::Type Flags) const = 0;
    virtual bool GetPeerAddress(FSocketEOSFull &Socket, FInternetAddrEOSFull &OutAddr) const = 0;

    virtual const TArray<TWeakPtr<FSocketEOSFull>> GetOwnedSockets(FSocketEOSFull &Socket) const
    {
        return TArray<TWeakPtr<FSocketEOSFull>>();
    };
};

class IEOSSocketRoleState : public TSharedFromThis<IEOSSocketRoleState>
{
public:
    UE_NONCOPYABLE(IEOSSocketRoleState);
    IEOSSocketRoleState() = default;
    virtual ~IEOSSocketRoleState() = default;
};

EOS_DISABLE_STRICT_WARNINGS
