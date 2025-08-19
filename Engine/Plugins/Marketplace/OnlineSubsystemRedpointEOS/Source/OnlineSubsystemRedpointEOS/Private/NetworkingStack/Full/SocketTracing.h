// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

void EOSLogSocketPacket(const EOS_P2P_SendPacketOptions& Opts);
void EOSLogSocketPacket(
    const EOS_ProductUserId &SourceRemoteUserId,
    const EOS_ProductUserId &DestinationLocalUserId,
    const EOS_P2P_SocketId &SocketId,
    uint8_t Channel,
    void *Buffer,
    uint32_t BytesRead);