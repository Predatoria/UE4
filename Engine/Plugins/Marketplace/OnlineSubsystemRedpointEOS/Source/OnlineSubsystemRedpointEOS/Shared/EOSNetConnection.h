// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/NetConnection.h"
#include "UObject/ObjectMacros.h"

#include "EOSNetConnection.generated.h"

EOS_ENABLE_STRICT_WARNINGS

UCLASS(transient, config = OnlineSubsystemRedpointEOS)
class UEOSNetConnection : public UNetConnection
{
    friend class UEOSNetDriver;

    GENERATED_BODY()

private:
    TWeakPtr<class ISocketEOS> Socket;

    void ReceivePacketFromDriver(const TSharedPtr<FInternetAddr> &ReceivedAddr, uint8 *Buffer, int32 BufferSize);

public:
    static void LogPacket(
        class UNetDriver *NetDriver,
        const TSharedPtr<const FInternetAddr> &InRemoteAddr,
        bool bIncoming,
        const uint8 *Data,
        int32 CountBytes);

    // We strip out the StatelessConnectionHandler from the packet handler stack for
    // P2P connections, since the P2P framework already manages the initial connection
    // (and thus, there's no point including any of the DDoS/stateless connection management
    // for P2P connections; it just makes the connection process more brittle).
    virtual void InitHandler() override;

    virtual void InitBase(
        UNetDriver *InDriver,
        FSocket *InSocket,
        const FURL &InURL,
        EConnectionState InState,
        int32 InMaxPacket = 0,
        int32 InPacketOverhead = 0) override;
    virtual void InitRemoteConnection(
        UNetDriver *InDriver,
        FSocket *InSocket,
        const FURL &InURL,
        const FInternetAddr &InRemoteAddr,
        EConnectionState InState,
        int32 InMaxPacket = 0,
        int32 InPacketOverhead = 0) override;
    virtual void InitLocalConnection(
        UNetDriver *InDriver,
        FSocket *InSocket,
        const FURL &InURL,
        EConnectionState InState,
        int32 InMaxPacket = 0,
        int32 InPacketOverhead = 0) override;

    virtual void LowLevelSend(void *Data, int32 CountBits, FOutPacketTraits &Traits) override;
    FString LowLevelGetRemoteAddress(bool bAppendPort = false) override;
    FString LowLevelDescribe() override;
};

EOS_DISABLE_STRICT_WARNINGS