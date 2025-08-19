// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "OnlineSubsystemRedpointEOS/Shared/AntiCheat/AntiCheat.h"

#if EOS_VERSION_AT_LEAST(1, 12, 0)

/**
 * This is an emulated implementation of Anti-Cheat, which allows testing Anti-Cheat in the editor without actually
 * using the Anti-Cheat APIs (which can't work in the editor or with the debugger attached).
 */

class FEditorAntiCheatSession : public FAntiCheatSession
{
public:
    struct FRegisteredPlayer
    {
        EOS_EAntiCheatCommonClientType ClientType;
        EOS_EAntiCheatCommonClientPlatform ClientPlatform;
    };

    bool bIsServer;
    TSharedRef<const FUniqueNetId> HostUserId;
    bool bIsDedicatedServerSession;
    TUserIdMap<TSharedPtr<FRegisteredPlayer>> RegisteredPlayers;

    FEditorAntiCheatSession(const TSharedRef<const FUniqueNetIdEOS> &InHostUserId);
    UE_NONCOPYABLE(FEditorAntiCheatSession);
};

class FEditorAntiCheat : public IAntiCheat
{
private:
    /** Used for ::Exec handling. */
    TArray<TSharedRef<FEditorAntiCheatSession>> CurrentSessions;

public:
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

#endif // #if EOS_VERSION_AT_LEAST(1, 12, 0)