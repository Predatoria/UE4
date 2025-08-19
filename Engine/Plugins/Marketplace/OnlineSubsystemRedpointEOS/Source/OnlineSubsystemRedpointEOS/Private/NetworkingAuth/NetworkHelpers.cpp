// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/NetworkHelpers.h"

#include "OnlineSubsystemUtils.h"

FOnlineSubsystemEOS *FNetworkHelpers::GetOSS(UNetConnection *Connection)
{
    auto Driver = Cast<UEOSNetDriver>(Connection->Driver);
    if (!IsValid(Driver))
    {
        return nullptr;
    }

    auto ConnWorld = Driver->FindWorld();
    if (ConnWorld == nullptr)
    {
        return nullptr;
    }

    FOnlineSubsystemEOS *OSS = (FOnlineSubsystemEOS *)Online::GetSubsystem(ConnWorld, REDPOINT_EOS_SUBSYSTEM);
    return OSS;
}

const FEOSConfig *FNetworkHelpers::GetConfig(UNetConnection *Connection)
{
    FOnlineSubsystemEOS *OSS = GetOSS(Connection);
    if (OSS == nullptr)
    {
        return nullptr;
    }

    return &OSS->GetConfig();
}

EEOSNetDriverRole FNetworkHelpers::GetRole(UNetConnection *Connection)
{
    auto LocalNetDriver = Cast<UEOSNetDriver>(Connection->Driver);
    if (!IsValid(LocalNetDriver))
    {
        return (EEOSNetDriverRole)-1;
    }
    return LocalNetDriver->GetEOSRole();
}

void FNetworkHelpers::GetAntiCheat(
    UNetConnection *Connection,
    TSharedPtr<IAntiCheat> &OutAntiCheat,
    TSharedPtr<FAntiCheatSession> &OutAntiCheatSession,
    bool &OutIsBeacon)
{
    auto LocalNetDriver = Cast<UEOSNetDriver>(Connection->Driver);
    if (!IsValid(LocalNetDriver))
    {
        OutIsBeacon = false;
        return;
    }
    OutAntiCheat = LocalNetDriver->AntiCheat.Pin();
    OutAntiCheatSession = LocalNetDriver->AntiCheatSession;
    OutIsBeacon = LocalNetDriver->bIsOwnedByBeacon;
}

ISocketSubsystem *FNetworkHelpers::GetSocketSubsystem(UNetConnection *Connection)
{
    auto LocalNetDriver = Cast<UEOSNetDriver>(Connection->Driver);
    if (!IsValid(LocalNetDriver))
    {
        return nullptr;
    }
    return LocalNetDriver->GetSocketSubsystem();
}