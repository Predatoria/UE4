// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/AntiCheat/EditorAntiCheat.h"

#if EOS_VERSION_AT_LEAST(1, 12, 0)

FEditorAntiCheatSession::FEditorAntiCheatSession(const TSharedRef<const FUniqueNetIdEOS> &InHostUserId)
    : bIsServer(false)
    , HostUserId(InHostUserId)
    , bIsDedicatedServerSession(false)
    , RegisteredPlayers(TUserIdMap<TSharedPtr<FRegisteredPlayer>>())
{
}

bool FEditorAntiCheat::Init()
{
    return true;
}

void FEditorAntiCheat::Shutdown()
{
}

bool FEditorAntiCheat::Exec(UWorld *InWorld, const TCHAR *Cmd, FOutputDevice &Ar)
{
    if (FParse::Command(&Cmd, TEXT("SIMULATEKICK"), false))
    {
        UE_LOG(LogEOS, Verbose, TEXT("Simulating Anti-Cheat kick for first user in session..."));
        for (const auto &Session : this->CurrentSessions)
        {
            if (Session->RegisteredPlayers.Num() > 0)
            {
                TArray<TSharedRef<const FUniqueNetId>> Keys;
                Session->RegisteredPlayers.GetKeys(Keys);
                this->OnPlayerActionRequired.Broadcast(
                    (const FUniqueNetIdEOS &)*Keys[0],
                    EOS_EAntiCheatCommonClientAction::EOS_ACCCA_RemovePlayer,
                    EOS_EAntiCheatCommonClientActionReason::EOS_ACCCAR_InternalError,
                    TEXT("Simulated Anti-Cheat kick!"));
            }
        }
        return true;
    }

    return false;
}

TSharedPtr<FAntiCheatSession> FEditorAntiCheat::CreateSession(
    bool bIsServer,
    const FUniqueNetIdEOS &HostUserId,
    bool bIsDedicatedServerSession,
    TSharedPtr<const FUniqueNetIdEOS> ListenServerUserId,
    FString ServerConnectionUrlOnClient)
{
    UE_LOG(LogEOS, Verbose, TEXT("Anti-Cheat: Creating editor AC session."));

    auto Session =
        MakeShared<FEditorAntiCheatSession>(StaticCastSharedRef<const FUniqueNetIdEOS>(HostUserId.AsShared()));
    Session->bIsServer = bIsServer;
    Session->bIsDedicatedServerSession = bIsDedicatedServerSession;
    this->CurrentSessions.Add(Session);
    return Session;
}

bool FEditorAntiCheat::DestroySession(FAntiCheatSession &Session)
{
    UE_LOG(LogEOS, Verbose, TEXT("Anti-Cheat: Destroying editor AC session."));
    this->CurrentSessions.Remove(StaticCastSharedRef<FEditorAntiCheatSession>(Session.AsShared()));

    return true;
}

bool FEditorAntiCheat::RegisterPlayer(
    FAntiCheatSession &Session,
    const FUniqueNetIdEOS &UserId,
    EOS_EAntiCheatCommonClientType ClientType,
    EOS_EAntiCheatCommonClientPlatform ClientPlatform)
{
    UE_LOG(LogEOS, Verbose, TEXT("Anti-Cheat: Registering player with editor AC session."));

    FEditorAntiCheatSession &EditorSession = (FEditorAntiCheatSession &)Session;

    TSharedRef<FEditorAntiCheatSession::FRegisteredPlayer> RegisteredPlayer =
        MakeShared<FEditorAntiCheatSession::FRegisteredPlayer>();
    RegisteredPlayer->ClientType = ClientType;
    RegisteredPlayer->ClientPlatform = ClientPlatform;
    EditorSession.RegisteredPlayers.Add(UserId, RegisteredPlayer);

    checkf(&UserId != nullptr, TEXT("FEditorAntiCheat::RegisterPlayer UserId is a null reference!"));

    // Send a message to the target.
    uint8 Bytes[40];
    uint32 Len = StringToBytes(TEXT("CLIENT-OK"), Bytes, 40);
    checkf(
        this->OnSendNetworkMessage.IsBound(),
        TEXT("Expected OnSendNetworkMessage to be bound for editor Anti-Cheat."));
    this->OnSendNetworkMessage
        .Execute(Session.AsShared(), *FUniqueNetIdEOS::DedicatedServerId(), UserId, &Bytes[0], Len);

    // Tell the server that the client is authenticated.
    this->OnPlayerAuthStatusChanged.Broadcast(
        UserId,
        EOS_EAntiCheatCommonClientAuthStatus::EOS_ACCCAS_RemoteAuthComplete);

    return true;
}

bool FEditorAntiCheat::UnregisterPlayer(FAntiCheatSession &Session, const FUniqueNetIdEOS &UserId)
{
    UE_LOG(LogEOS, Verbose, TEXT("Anti-Cheat: Unregistering player with editor AC session."));

    FEditorAntiCheatSession &EditorSession = (FEditorAntiCheatSession &)Session;

    EditorSession.RegisteredPlayers.Remove(UserId);

    return true;
}

bool FEditorAntiCheat::ReceiveNetworkMessage(
    FAntiCheatSession &Session,
    const FUniqueNetIdEOS &SourceUserId,
    const FUniqueNetIdEOS &TargetUserId,
    const uint8 *Data,
    uint32_t Size)
{
    FString EditorMessage = BytesToString(Data, Size);

    UE_LOG(
        LogEOS,
        Verbose,
        TEXT("Anti-Cheat: Receiving networking message for player with editor AC session: '%s'"),
        *EditorMessage);

    if (EditorMessage == TEXT("CLIENT-OK"))
    {
        checkf(
            &TargetUserId != nullptr,
            TEXT("FEditorAntiCheat::ReceiveNetworkMessage TargetUserId is a null reference!"));
        checkf(
            &SourceUserId != nullptr,
            TEXT("FEditorAntiCheat::ReceiveNetworkMessage SourceUserId is a null reference!"));

        // Respond to server.
        uint8 Bytes[40];
        uint32 Len = StringToBytes(TEXT("SERVER-OK"), Bytes, 40);
        checkf(
            this->OnSendNetworkMessage.IsBound(),
            TEXT("Expected OnSendNetworkMessage to be bound for editor Anti-Cheat."));
        this->OnSendNetworkMessage.Execute(Session.AsShared(), TargetUserId, SourceUserId, &Bytes[0], Len);
    }
    else if (EditorMessage == TEXT("SERVER-OK"))
    {
        // This client is "verified".
        this->OnPlayerAuthStatusChanged.Broadcast(
            SourceUserId,
            EOS_EAntiCheatCommonClientAuthStatus::EOS_ACCCAS_RemoteAuthComplete);
    }

    return true;
}

#endif // #if EOS_VERSION_AT_LEAST(1, 12, 0)