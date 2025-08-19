// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "OnlineSubsystemRedpointEOS/Shared/AntiCheat/AntiCheat.h"

#if WITH_SERVER_CODE && EOS_VERSION_AT_LEAST(1, 12, 0)

class FEOSDedicatedServerAntiCheatSession : public FAntiCheatSession
{
private:
    TSharedRef<class FAntiCheatPlayerTracker> PlayerTracking;

public:
    int StackCount;

    FEOSDedicatedServerAntiCheatSession();
    UE_NONCOPYABLE(FEOSDedicatedServerAntiCheatSession);
    EOS_AntiCheatCommon_ClientHandle AddPlayer(const FUniqueNetIdEOS &UserId, bool &bOutShouldRegister);
    void RemovePlayer(const FUniqueNetIdEOS &UserId);
    bool ShouldDeregisterPlayerBeforeRemove(const FUniqueNetIdEOS &UserId) const;
    TSharedPtr<const FUniqueNetIdEOS> GetPlayer(EOS_AntiCheatCommon_ClientHandle Handle);
    EOS_AntiCheatCommon_ClientHandle GetHandle(const FUniqueNetIdEOS &UserId);
    bool HasPlayer(const FUniqueNetIdEOS &UserId);
};

class FEOSDedicatedServerAntiCheat : public IAntiCheat, public TSharedFromThis<FEOSDedicatedServerAntiCheat>
{
private:
    EOS_HAntiCheatServer EOSACServer;
    TSharedPtr<FEOSDedicatedServerAntiCheatSession> CurrentSession;
    TSharedPtr<EOSEventHandle<EOS_AntiCheatCommon_OnMessageToClientCallbackInfo>> NotifyMessageToClient;
    TSharedPtr<EOSEventHandle<EOS_AntiCheatCommon_OnClientActionRequiredCallbackInfo>> NotifyClientActionRequired;
    TSharedPtr<EOSEventHandle<EOS_AntiCheatCommon_OnClientAuthStatusChangedCallbackInfo>> NotifyClientAuthStatusChanged;

public:
    FEOSDedicatedServerAntiCheat(EOS_HPlatform InPlatform);
    UE_NONCOPYABLE(FEOSDedicatedServerAntiCheat);

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

#endif // #if WITH_SERVER_CODE && EOS_VERSION_AT_LEAST(1, 12, 0)