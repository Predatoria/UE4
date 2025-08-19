// Copyright June Rhodes. All Rights Reserved.

#pragma once

class FSocketEOSFull;

#include "CoreMinimal.h"
#include "EOSSocketRole.h"

EOS_ENABLE_STRICT_WARNINGS

/**
 * The None role is the initial role for newly created sockets, and allows them to move into either Client or Listening.
 */
class FEOSSocketRoleNone : public IEOSSocketRole
{
public:
    UE_NONCOPYABLE(FEOSSocketRoleNone);
    FEOSSocketRoleNone() = default;
    virtual ~FEOSSocketRoleNone() = default;

    virtual FName GetRoleName() const override
    {
        return FName(TEXT("None"));
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

class FEOSSocketRoleNoneState : public IEOSSocketRoleState
{
public:
    UE_NONCOPYABLE(FEOSSocketRoleNoneState);
    FEOSSocketRoleNoneState() = default;
    virtual ~FEOSSocketRoleNoneState() = default;
};

EOS_DISABLE_STRICT_WARNINGS
