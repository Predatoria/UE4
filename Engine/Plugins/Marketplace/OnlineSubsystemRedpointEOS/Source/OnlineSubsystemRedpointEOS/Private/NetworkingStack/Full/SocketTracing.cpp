// Copyright June Rhodes. All Rights Reserved.

#include "SocketTracing.h"

#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"

void EOSLogSocketPacket(const EOS_P2P_SendPacketOptions &Opts)
{
#if defined(EOS_ENABLE_SOCKET_LEVEL_NETWORK_TRACING)
    FString LocalUserId, RemoteUserId;
    EOSString_ProductUserId::ToString(Opts.LocalUserId, LocalUserId);
    EOSString_ProductUserId::ToString(Opts.RemoteUserId, RemoteUserId);
    UE_LOG(
        LogEOSSocket,
        Verbose,
        TEXT("%s:%d: '%s' %s '%s': %s"),
        *EOSString_P2P_SocketName::FromAnsiString(Opts.SocketId->SocketName),
        Opts.Channel,
        *LocalUserId,
        TEXT("->"),
        *RemoteUserId,
        *FString::FromBlob((uint8*)Opts.Data, Opts.DataLengthBytes));
#endif
}

void EOSLogSocketPacket(
    const EOS_ProductUserId &SourceRemoteUserId,
    const EOS_ProductUserId &DestinationLocalUserId,
    const EOS_P2P_SocketId &SocketId,
    uint8_t Channel,
    void *Buffer,
    uint32_t BytesRead)
{
#if defined(EOS_ENABLE_SOCKET_LEVEL_NETWORK_TRACING)
    FString LocalUserId, RemoteUserId;
    EOSString_ProductUserId::ToString(DestinationLocalUserId, LocalUserId);
    EOSString_ProductUserId::ToString(SourceRemoteUserId, RemoteUserId);
    UE_LOG(
        LogEOSSocket,
        Verbose,
        TEXT("%s:%d: '%s' %s '%s': %s"),
        *EOSString_P2P_SocketName::FromAnsiString(SocketId.SocketName),
        Channel,
        *LocalUserId,
        TEXT("<-"),
        *RemoteUserId,
        *FString::FromBlob((uint8 *)Buffer, BytesRead));
#endif
}