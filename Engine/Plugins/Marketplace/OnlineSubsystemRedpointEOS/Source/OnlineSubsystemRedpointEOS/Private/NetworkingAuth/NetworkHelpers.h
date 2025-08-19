// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/NetConnection.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSNetDriver.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"

class FNetworkHelpers
{
public:
    static FOnlineSubsystemEOS *GetOSS(UNetConnection *Connection);
    static const FEOSConfig *GetConfig(UNetConnection *Connection);
    static EEOSNetDriverRole GetRole(UNetConnection *Connection);
    static void GetAntiCheat(
        UNetConnection *Connection,
        TSharedPtr<IAntiCheat> &OutAntiCheat,
        TSharedPtr<FAntiCheatSession> &OutAntiCheatSession,
        bool &OutIsBeacon);
    static ISocketSubsystem *GetSocketSubsystem(UNetConnection *Connection);
};