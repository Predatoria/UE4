// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "OnlineSubsystemRedpointEOS/Shared/AntiCheat/AntiCheat.h"

class FNullAntiCheatSession : public FAntiCheatSession
{
public:
    FNullAntiCheatSession() = default;
    UE_NONCOPYABLE(FNullAntiCheatSession);
};

class FNullAntiCheat : public IAntiCheat
{
public:
    FNullAntiCheat() = default;
    UE_NONCOPYABLE(FNullAntiCheat);

    virtual bool Init() override;
    virtual void Shutdown() override;

    virtual bool Exec(UWorld *InWorld, const TCHAR *Cmd, FOutputDevice &Ar) override;

    virtual TSharedPtr<FAntiCheatSession> CreateSession(
        bool bIsServer,
        const FUniqueNetIdEOS &HostUserId,
        bool bIsDedicatedServerSession,
        TSharedPtr<const FUniqueNetIdEOS> ListenServerUserId,
        FString ServerConnectionUrlOnClient) override;
    virtual bool DestroySession(FAntiCheatSession &Session) override;

    virtual bool RegisterPlayer(
        FAntiCheatSession &Session,
        const FUniqueNetIdEOS &UserId,
        EOS_EAntiCheatCommonClientType ClientType,
        EOS_EAntiCheatCommonClientPlatform ClientPlatform) override;
    virtual bool UnregisterPlayer(FAntiCheatSession &Session, const FUniqueNetIdEOS &UserId) override;

    virtual bool ReceiveNetworkMessage(
        FAntiCheatSession &Session,
        const FUniqueNetIdEOS &SourceUserId,
        const FUniqueNetIdEOS &TargetUserId,
        const uint8 *Data,
        uint32_t Size) override;
};