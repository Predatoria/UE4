// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Containers/Queue.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/Full/InternetAddrEOSFull.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/Full/SocketEOSMap.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/ISocketEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "Sockets.h"

EOS_ENABLE_STRICT_WARNINGS

typedef int FSocketEOSMemoryId;

class FEOSRawPacket : public TSharedFromThis<FEOSRawPacket>
{
private:
    void *Data;
    int32 DataLen;

public:
    FEOSRawPacket(void *InData, int32 InDataLen)
        : Data(InData)
        , DataLen(InDataLen){};
    UE_NONCOPYABLE(FEOSRawPacket);
    ~FEOSRawPacket()
    {
        FMemory::Free(this->Data);
    }

    const void *GetData() const
    {
        return (const void *)this->Data;
    }
    int32 GetDataLen() const
    {
        return this->DataLen;
    }
};

class FSocketEOSFull : public ISocketEOS, public TSharedFromThis<FSocketEOSFull>
{
    friend class FSocketSubsystemEOSFull;
    friend class FSocketSubsystemEOSListenManager;
    friend class FEOSSocketRoleNone;
    friend class FEOSSocketRoleClient;
    friend class FEOSSocketRoleListening;
    friend class FEOSSocketRoleRemote;
    friend class IEOSSocketRole;
    friend class FGameplayDebuggerCategory_P2PConnections;

private:
    /**
     * The current role for this socket - the role is the actual implementation of behaviour for the socket.
     */
    TSharedRef<class IEOSSocketRole> Role;

    /**
     * Custom state that the role implementation uses between requests. When the role changes, the role state will be
     * replaced.
     */
    TSharedRef<class IEOSSocketRoleState> RoleState;

    /** Reference to the EOS P2P interface. */
    EOS_HP2P EOSP2P;

    /** The local address we are bound on. The user ID of this address is the sender of packets. */
    TSharedPtr<FInternetAddrEOSFull> BindAddress;

    /**
     * The memory ID of this socket; this ID is used to ensure that sockets are freed correctly when they are no longer
     * used.
     */
    FSocketEOSMemoryId SocketMemoryId;

    /**
     * The unique key used to identify this socket when the socket subsystem needs to route received packets to it. This
     * is a pointer because it doesn't get set to a valid value until after an operation is performed on the socket.
     */
    TSharedPtr<FSocketEOSKey> SocketKey;

    /**
     * When you are updating the socket key, you must call this function so the socket can register itself with the
     * socket subsystem correctly.
     */
    void UpdateSocketKey(const FSocketEOSKey &InSocketKey);

    /**
     * Used when removing the current socket key from the socket subsystem. Automatically calls EOS_P2P_CloseConnection
     * if this was the last socket that could receive packets from the remote connection.
     */
    void RemoveSocketKeyIfSet();

    /**
     * A list of received packets that the socket subsystem has received and assigned to this socket, but for which
     * the socket RecvFrom function has not yet been called to actually receive them.
     */
    TQueue<TSharedPtr<FEOSRawPacket>> ReceivedPacketsQueue;

    /**
     * The number of packets in the received queue, since TQueue does not have
     * a Num() function.
     */
    int32 ReceivedPacketsQueueCount = 0;

    /**
     * Note: We don't use this field for anything, except to ensure that the socket subsystem doesn't get destroyed
     * before the socket is. This is important because the socket registers events with EOS, and we need to ensure that
     * the online subsystem doesn't clean up the EOS_HPlatform handle while there are still events registered.
     */
    TSharedPtr<class FSocketSubsystemEOSFull> SocketSubsystem;

public:
    FSocketEOSFull(
        const TSharedRef<class FSocketSubsystemEOSFull> &InSocketSubsystem,
        EOS_HP2P InP2P,
        FSocketEOSMemoryId InSocketMemoryId,
        const FString &InSocketDescription);
    UE_NONCOPYABLE(FSocketEOSFull);
    ~FSocketEOSFull();

    virtual TSharedRef<ISocketEOS> AsSharedEOS() override;

    int GetSocketMemoryId() const
    {
        return this->SocketMemoryId;
    }

    virtual bool Shutdown(ESocketShutdownMode Mode) override;
    virtual bool Close() override;
    virtual bool Bind(const FInternetAddr &Addr) override;
    virtual bool Connect(const FInternetAddr &Addr) override;
    virtual bool Listen(int32 MaxBacklog) override;
    virtual bool WaitForPendingConnection(bool &bHasPendingConnection, const FTimespan &WaitTime) override;
    virtual bool HasPendingData(uint32 &PendingDataSize) override;
    virtual class FSocket *Accept(const FString &InSocketDescription) override;
    virtual class FSocket *Accept(FInternetAddr &InRemoteAddr, const FString &InSocketDescription) override;
    virtual bool SendTo(const uint8 *Data, int32 Count, int32 &BytesSent, const FInternetAddr &Destination) override;
    virtual bool Send(const uint8 *Data, int32 Count, int32 &BytesSent) override;
    virtual bool RecvFrom(
        uint8 *Data,
        int32 BufferSize,
        int32 &BytesRead,
        FInternetAddr &Source,
        ESocketReceiveFlags::Type Flags = ESocketReceiveFlags::None) override;
    virtual bool Recv(
        uint8 *Data,
        int32 BufferSize,
        int32 &BytesRead,
        ESocketReceiveFlags::Type Flags = ESocketReceiveFlags::None) override;
    virtual bool RecvMulti(FRecvMulti &MultiData, ESocketReceiveFlags::Type Flags = ESocketReceiveFlags::None) override;
    virtual bool Wait(ESocketWaitConditions::Type Condition, FTimespan WaitTime) override;
    virtual ESocketConnectionState GetConnectionState() override;
    virtual void GetAddress(FInternetAddr &OutAddr) override;
    virtual bool GetPeerAddress(FInternetAddr &OutAddr) override;
    virtual bool SetNonBlocking(bool bIsNonBlocking = true) override;
    virtual bool SetBroadcast(bool bAllowBroadcast = true) override;
    virtual bool SetNoDelay(bool bIsNoDelay = true) override;
    virtual bool JoinMulticastGroup(const FInternetAddr &GroupAddress) override;
    virtual bool JoinMulticastGroup(const FInternetAddr &GroupAddress, const FInternetAddr &InterfaceAddress) override;
    virtual bool LeaveMulticastGroup(const FInternetAddr &GroupAddress) override;
    virtual bool LeaveMulticastGroup(const FInternetAddr &GroupAddress, const FInternetAddr &InterfaceAddress) override;
    virtual bool SetMulticastLoopback(bool bLoopback) override;
    virtual bool SetMulticastTtl(uint8 TimeToLive) override;
    virtual bool SetMulticastInterface(const FInternetAddr &InterfaceAddress) override;
    virtual bool SetReuseAddr(bool bAllowReuse = true) override;
    virtual bool SetLinger(bool bShouldLinger = true, int32 Timeout = 0) override;
    virtual bool SetRecvErr(bool bUseErrorQueue = true) override;
    virtual bool SetSendBufferSize(int32 Size, int32 &NewSize) override;
    virtual bool SetReceiveBufferSize(int32 Size, int32 &NewSize) override;
    virtual int32 GetPortNo() override;
};

EOS_DISABLE_STRICT_WARNINGS
