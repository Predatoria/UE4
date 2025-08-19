// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/EOSIpNetConnection.h"

#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSDefines.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSNetDriver.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "OnlineSubsystemUtils.h"
#include "PacketHandler.h"

EOS_ENABLE_STRICT_WARNINGS

// According to USteamSocketsNetConnection, UNetConnection can assert if this value is greater
// than 1024 (though I can't find out where it would do so).
static const int32 MAX_PACKET_IP = EOS_P2P_MAX_PACKET_SIZE > 1024 ? 1024 : EOS_P2P_MAX_PACKET_SIZE;

void UEOSIpNetConnection::InitBase(
    UNetDriver *InDriver,
    FSocket *InSocket,
    const FURL &InURL,
    EConnectionState InState,
    int32 InMaxPacket,
    int32 InPacketOverhead)
{
    UIpConnection::InitBase(
        InDriver,
        InSocket,
        InURL,
        InState,
        ((InMaxPacket == 0) ? MAX_PACKET_IP : InMaxPacket),
        ((InPacketOverhead == 0) ? 1 : InPacketOverhead));

    auto EOSDriver = Cast<UEOSNetDriver>(InDriver);
    if (IsValid(EOSDriver))
    {
        auto ConnWorld = EOSDriver->FindWorld();
        if (ConnWorld != nullptr)
        {
            FOnlineSubsystemEOS *OSS = (FOnlineSubsystemEOS *)Online::GetSubsystem(ConnWorld, REDPOINT_EOS_SUBSYSTEM);
            if (OSS != nullptr)
            {
                auto &Config = OSS->GetConfig();
                if (Config.GetEnableTrustedDedicatedServers())
                {
                    if (!this->Handler->GetEncryptionComponent().IsValid())
                    {
                        UE_LOG(
                            LogEOS,
                            Error,
                            TEXT("You must set [PacketHandlerComponents] EncryptionComponent=AESGCMHandlerComponent in "
                                 "DefaultEngine.ini in order to connect to dedicated servers on EOS."));
                        this->Close();
                    }
                }
            }
        }
    }
}

EOS_DISABLE_STRICT_WARNINGS