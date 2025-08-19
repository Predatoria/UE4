// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/AntiCheat/NullAntiCheat.h"

bool FNullAntiCheat::Init()
{
    return true;
}

void FNullAntiCheat::Shutdown()
{
}

bool FNullAntiCheat::Exec(UWorld *InWorld, const TCHAR *Cmd, FOutputDevice &Ar)
{
    return false;
}

TSharedPtr<FAntiCheatSession> FNullAntiCheat::CreateSession(
    bool bIsServer,
    const FUniqueNetIdEOS &HostUserId,
    bool bIsDedicatedServerSession,
    TSharedPtr<const FUniqueNetIdEOS> ListenServerUserId,
    FString ServerConnectionUrlOnClient)
{
#if !(PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX)
    UE_LOG(
        LogEOS,
        Warning,
        TEXT("This platform does not support hosting Anti-Cheat sessions. This game session will not be protected by "
             "Anti-Cheat, even for clients that connect on desktop platforms."))
#endif
    return MakeShared<FNullAntiCheatSession>();
}

bool FNullAntiCheat::DestroySession(FAntiCheatSession &Session)
{
    return true;
}

bool FNullAntiCheat::RegisterPlayer(
    FAntiCheatSession &Session,
    const FUniqueNetIdEOS &UserId,
    EOS_EAntiCheatCommonClientType ClientType,
    EOS_EAntiCheatCommonClientPlatform ClientPlatform)
{
    this->OnPlayerAuthStatusChanged.Broadcast(
        UserId,
        EOS_EAntiCheatCommonClientAuthStatus::EOS_ACCCAS_RemoteAuthComplete);
    return true;
}

bool FNullAntiCheat::UnregisterPlayer(FAntiCheatSession &Session, const FUniqueNetIdEOS &UserId)
{
    return true;
}

bool FNullAntiCheat::ReceiveNetworkMessage(
    FAntiCheatSession &Session,
    const FUniqueNetIdEOS &SourceUserId,
    const FUniqueNetIdEOS &TargetUserId,
    const uint8 *Data,
    uint32_t Size)
{
    checkf(false, TEXT("Did not expect to receive network messages in null Anti-Cheat implementation."));
    return false;
}