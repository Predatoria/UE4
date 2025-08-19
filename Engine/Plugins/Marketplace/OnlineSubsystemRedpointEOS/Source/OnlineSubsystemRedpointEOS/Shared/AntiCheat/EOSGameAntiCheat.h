// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "OnlineSubsystemRedpointEOS/Shared/AntiCheat/AntiCheat.h"

#if EOS_VERSION_AT_LEAST(1, 12, 0)

class FEOSGameAntiCheatSession : public FAntiCheatSession
{
private:
    TSharedRef<class FAntiCheatPlayerTracker> PlayerTracking;

public:
    bool bIsServer;
    TSharedRef<const FUniqueNetIdEOS> HostUserId;
    TSharedPtr<const FUniqueNetIdEOS> ListenServerUserId;
    bool bIsDedicatedServerSession;
    int StackCount;
    FString ServerConnectionUrlOnClient;

    FEOSGameAntiCheatSession(TSharedRef<const FUniqueNetIdEOS> InHostUserId);
    UE_NONCOPYABLE(FEOSGameAntiCheatSession);
    EOS_AntiCheatCommon_ClientHandle AddPlayer(const FUniqueNetIdEOS &UserId, bool &bOutShouldRegister);
    void RemovePlayer(const FUniqueNetIdEOS &UserId);
    bool ShouldDeregisterPlayerBeforeRemove(const FUniqueNetIdEOS &UserId) const;
    TSharedPtr<const FUniqueNetIdEOS> GetPlayer(EOS_AntiCheatCommon_ClientHandle Handle);
    EOS_AntiCheatCommon_ClientHandle GetHandle(const FUniqueNetIdEOS &UserId);
    bool HasPlayer(const FUniqueNetIdEOS &UserId);
};

class FEOSGameAntiCheat : public IAntiCheat, public TSharedFromThis<FEOSGameAntiCheat>
{
private:
    EOS_HAntiCheatClient EOSACClient;
    TSharedPtr<FEOSGameAntiCheatSession> CurrentSession;
    TSharedPtr<EOSEventHandle<EOS_AntiCheatClient_OnMessageToServerCallbackInfo>> NotifyMessageToServer;
    TSharedPtr<EOSEventHandle<EOS_AntiCheatCommon_OnMessageToClientCallbackInfo>> NotifyMessageToPeer;
    TSharedPtr<EOSEventHandle<EOS_AntiCheatCommon_OnClientActionRequiredCallbackInfo>> NotifyClientActionRequired;
    TSharedPtr<EOSEventHandle<EOS_AntiCheatCommon_OnClientAuthStatusChangedCallbackInfo>> NotifyClientAuthStatusChanged;

public:
    FEOSGameAntiCheat(EOS_HPlatform InPlatform);
    UE_NONCOPYABLE(FEOSGameAntiCheat);

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