// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/AntiCheat/EOSDedicatedServerAntiCheat.h"

#if WITH_SERVER_CODE && EOS_VERSION_AT_LEAST(1, 12, 0)

#include "OnlineSubsystemRedpointEOS/Private/AntiCheat/AntiCheatPlayerTracker.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

FEOSDedicatedServerAntiCheatSession::FEOSDedicatedServerAntiCheatSession()
    : PlayerTracking(MakeShared<FAntiCheatPlayerTracker>())
    , StackCount(0)
{
}

EOS_AntiCheatCommon_ClientHandle FEOSDedicatedServerAntiCheatSession::AddPlayer(
    const FUniqueNetIdEOS &UserId,
    bool &bOutShouldRegister)
{
    UE_LOG(
        LogEOSAntiCheat,
        Verbose,
        TEXT("Dedicated server Anti-Cheat: %p: AddPlayer(UserId: %s)"),
        this,
        *UserId.ToString());

    return this->PlayerTracking->AddPlayer(UserId, bOutShouldRegister);
}

void FEOSDedicatedServerAntiCheatSession::RemovePlayer(const FUniqueNetIdEOS &UserId)
{
    UE_LOG(
        LogEOSAntiCheat,
        Verbose,
        TEXT("Dedicated server Anti-Cheat: %p: RemovePlayer(UserId: %s)"),
        this,
        *UserId.ToString());

    this->PlayerTracking->RemovePlayer(UserId);
}

bool FEOSDedicatedServerAntiCheatSession::ShouldDeregisterPlayerBeforeRemove(const FUniqueNetIdEOS &UserId) const
{
    return this->PlayerTracking->ShouldDeregisterPlayerBeforeRemove(UserId);
}

TSharedPtr<const FUniqueNetIdEOS> FEOSDedicatedServerAntiCheatSession::GetPlayer(
    EOS_AntiCheatCommon_ClientHandle Handle)
{
    return this->PlayerTracking->GetPlayer(Handle);
}

EOS_AntiCheatCommon_ClientHandle FEOSDedicatedServerAntiCheatSession::GetHandle(const FUniqueNetIdEOS &UserId)
{
    return this->PlayerTracking->GetHandle(UserId);
}

bool FEOSDedicatedServerAntiCheatSession::HasPlayer(const FUniqueNetIdEOS &UserId)
{
    return this->PlayerTracking->HasPlayer(UserId);
}

FEOSDedicatedServerAntiCheat::FEOSDedicatedServerAntiCheat(EOS_HPlatform InPlatform)
    : EOSACServer(EOS_Platform_GetAntiCheatServerInterface(InPlatform)){};

bool FEOSDedicatedServerAntiCheat::Init()
{
    UE_LOG(LogEOSAntiCheat, Verbose, TEXT("Dedicated server Anti-Cheat: %p: Init"), this);

    EOS_AntiCheatServer_AddNotifyMessageToClientOptions MsgOpts = {};
    MsgOpts.ApiVersion = EOS_ANTICHEATSERVER_ADDNOTIFYMESSAGETOCLIENT_API_LATEST;
    this->NotifyMessageToClient = EOSRegisterEvent<
        EOS_HAntiCheatServer,
        EOS_AntiCheatServer_AddNotifyMessageToClientOptions,
        EOS_AntiCheatCommon_OnMessageToClientCallbackInfo>(
        this->EOSACServer,
        &MsgOpts,
        EOS_AntiCheatServer_AddNotifyMessageToClient,
        EOS_AntiCheatServer_RemoveNotifyMessageToClient,
        [WeakThis = GetWeakThis(this)](const EOS_AntiCheatCommon_OnMessageToClientCallbackInfo *Data) {
#define FUNCTION_CONTEXT_MACRO                                                                                         \
    "Dedicated server Anti-Cheat: %p: NotifyMessageToClient(ClientHandle: %d, MessageData: ..., "                      \
    "MessageDataSizeInBytes: %u)"
#define FUNCTION_CONTEXT_ARGS                                                                                          \
    WeakThis.IsValid() ? WeakThis.Pin().Get() : nullptr, Data->ClientHandle, Data->MessageDataSizeBytes
            if (auto This = PinWeakThis(WeakThis))
            {
                if (This->CurrentSession.IsValid())
                {
                    TSharedPtr<const FUniqueNetIdEOS> PlayerId = This->CurrentSession->GetPlayer(Data->ClientHandle);
                    if (PlayerId.IsValid())
                    {
                        This->OnSendNetworkMessage.ExecuteIfBound(
                            StaticCastSharedRef<FAntiCheatSession>(This->CurrentSession.ToSharedRef()),
                            *FUniqueNetIdEOS::DedicatedServerId(),
                            *PlayerId,
                            (const uint8 *)Data->MessageData,
                            Data->MessageDataSizeBytes);

                        if (!This->OnSendNetworkMessage.IsBound())
                        {
                            UE_LOG(
                                LogEOSAntiCheat,
                                Warning,
                                TEXT(FUNCTION_CONTEXT_MACRO
                                     ": Propagated event OnSendNetworkMessage() handler, but no handlers were "
                                     "bound."),
                                FUNCTION_CONTEXT_ARGS);
                        }
                        else
                        {
                            UE_LOG(
                                LogEOSAntiCheat,
                                VeryVerbose,
                                TEXT(FUNCTION_CONTEXT_MACRO ": Propagated event to OnSendNetworkMessage() handler."),
                                FUNCTION_CONTEXT_ARGS);
                        }
                    }
                    else
                    {
                        UE_LOG(
                            LogEOSAntiCheat,
                            Error,
                            TEXT(FUNCTION_CONTEXT_MACRO ": Ignoring call as it refers to an invalid player ID."),
                            FUNCTION_CONTEXT_ARGS);
                    }
                }
                else
                {
                    UE_LOG(
                        LogEOSAntiCheat,
                        Error,
                        TEXT(FUNCTION_CONTEXT_MACRO ": Ignoring call as there is no current session."),
                        FUNCTION_CONTEXT_ARGS);
                }
            }
            else
            {
                UE_LOG(
                    LogEOSAntiCheat,
                    Warning,
                    TEXT(FUNCTION_CONTEXT_MACRO ": Ignoring call as the instance is no longer valid."),
                    FUNCTION_CONTEXT_ARGS);
            }

#undef FUNCTION_CONTEXT_MACRO
#undef FUNCTION_CONTEXT_ARGS
        });

    EOS_AntiCheatServer_AddNotifyClientActionRequiredOptions ActOpts = {};
    ActOpts.ApiVersion = EOS_ANTICHEATSERVER_ADDNOTIFYCLIENTACTIONREQUIRED_API_LATEST;
    this->NotifyClientActionRequired = EOSRegisterEvent<
        EOS_HAntiCheatServer,
        EOS_AntiCheatServer_AddNotifyClientActionRequiredOptions,
        EOS_AntiCheatCommon_OnClientActionRequiredCallbackInfo>(
        this->EOSACServer,
        &ActOpts,
        EOS_AntiCheatServer_AddNotifyClientActionRequired,
        EOS_AntiCheatServer_RemoveNotifyClientActionRequired,
        [WeakThis = GetWeakThis(this)](const EOS_AntiCheatCommon_OnClientActionRequiredCallbackInfo *Data) {
#define FUNCTION_CONTEXT_MACRO                                                                                         \
    "Dedicated server Anti-Cheat: %p: NotifyClientActionRequired(ClientHandle: %d, ClientAction: %u, "                 \
    "ActionReasonCode: %u, ActionReasonDetailsString: %s)"
#define FUNCTION_CONTEXT_ARGS                                                                                          \
    WeakThis.IsValid() ? WeakThis.Pin().Get() : nullptr, Data->ClientHandle, Data->ClientAction,                       \
        Data->ActionReasonCode,                                                                                        \
        *EOSString_AntiCheat_ActionReasonDetailsString::FromUtf8String(Data->ActionReasonDetailsString)
            if (auto This = PinWeakThis(WeakThis))
            {
                if (This->CurrentSession.IsValid())
                {
                    This->OnPlayerActionRequired.Broadcast(
                        *This->CurrentSession->GetPlayer(Data->ClientHandle),
                        Data->ClientAction,
                        Data->ActionReasonCode,
                        EOSString_AntiCheat_ActionReasonDetailsString::FromUtf8String(Data->ActionReasonDetailsString));

                    if (!This->OnPlayerActionRequired.IsBound())
                    {
                        UE_LOG(
                            LogEOSAntiCheat,
                            Warning,
                            TEXT(FUNCTION_CONTEXT_MACRO
                                 ": Propagated event OnPlayerActionRequired() handler, but no handlers were "
                                 "bound."),
                            FUNCTION_CONTEXT_ARGS);
                    }
                    else
                    {
                        UE_LOG(
                            LogEOSAntiCheat,
                            Verbose,
                            TEXT(FUNCTION_CONTEXT_MACRO ": Propagated event to OnPlayerActionRequired() handler."),
                            FUNCTION_CONTEXT_ARGS);
                    }
                }
                else
                {
                    UE_LOG(
                        LogEOSAntiCheat,
                        Error,
                        TEXT(FUNCTION_CONTEXT_MACRO ": Ignoring call as there is no current session."),
                        FUNCTION_CONTEXT_ARGS);
                }
            }
            else
            {
                UE_LOG(
                    LogEOSAntiCheat,
                    Warning,
                    TEXT(FUNCTION_CONTEXT_MACRO ": Ignoring call as the instance is no longer valid."),
                    FUNCTION_CONTEXT_ARGS);
            }

#undef FUNCTION_CONTEXT_MACRO
#undef FUNCTION_CONTEXT_ARGS
        });

    EOS_AntiCheatServer_AddNotifyClientAuthStatusChangedOptions AuthOpts = {};
    AuthOpts.ApiVersion = EOS_ANTICHEATSERVER_ADDNOTIFYCLIENTAUTHSTATUSCHANGED_API_LATEST;
    this->NotifyClientAuthStatusChanged = EOSRegisterEvent<
        EOS_HAntiCheatServer,
        EOS_AntiCheatServer_AddNotifyClientAuthStatusChangedOptions,
        EOS_AntiCheatCommon_OnClientAuthStatusChangedCallbackInfo>(
        this->EOSACServer,
        &AuthOpts,
        EOS_AntiCheatServer_AddNotifyClientAuthStatusChanged,
        EOS_AntiCheatServer_RemoveNotifyClientAuthStatusChanged,
        [WeakThis = GetWeakThis(this)](const EOS_AntiCheatCommon_OnClientAuthStatusChangedCallbackInfo *Data) {
#define FUNCTION_CONTEXT_MACRO                                                                                         \
    "Dedicated server Anti-Cheat: %p: NotifyClientAuthStatusChanged(ClientHandle: %d, ClientAuthStatus: %u)"
#define FUNCTION_CONTEXT_ARGS                                                                                          \
    WeakThis.IsValid() ? WeakThis.Pin().Get() : nullptr, Data->ClientHandle, Data->ClientAuthStatus
            if (auto This = PinWeakThis(WeakThis))
            {
                if (This->CurrentSession.IsValid())
                {
                    This->OnPlayerAuthStatusChanged.Broadcast(
                        *This->CurrentSession->GetPlayer(Data->ClientHandle),
                        Data->ClientAuthStatus);

                    if (!This->OnPlayerAuthStatusChanged.IsBound())
                    {
                        UE_LOG(
                            LogEOSAntiCheat,
                            Warning,
                            TEXT(FUNCTION_CONTEXT_MACRO
                                 ": Propagated event OnPlayerAuthStatusChanged() handler, but no handlers were "
                                 "bound."),
                            FUNCTION_CONTEXT_ARGS);
                    }
                    else
                    {
                        UE_LOG(
                            LogEOSAntiCheat,
                            Verbose,
                            TEXT(FUNCTION_CONTEXT_MACRO ": Propagated event to OnPlayerAuthStatusChanged() handler."),
                            FUNCTION_CONTEXT_ARGS);
                    }
                }
                else
                {
                    UE_LOG(
                        LogEOSAntiCheat,
                        Error,
                        TEXT(FUNCTION_CONTEXT_MACRO ": Ignoring call as there is no current session."),
                        FUNCTION_CONTEXT_ARGS);
                }
            }
            else
            {
                UE_LOG(
                    LogEOSAntiCheat,
                    Warning,
                    TEXT(FUNCTION_CONTEXT_MACRO ": Ignoring call as the instance is no longer valid."),
                    FUNCTION_CONTEXT_ARGS);
            }

#undef FUNCTION_CONTEXT_MACRO
#undef FUNCTION_CONTEXT_ARGS
        });

    return true;
}

void FEOSDedicatedServerAntiCheat::Shutdown()
{
    UE_LOG(LogEOSAntiCheat, Verbose, TEXT("Dedicated server Anti-Cheat: %p: Shutdown"), this);

    this->NotifyMessageToClient.Reset();
    this->NotifyClientActionRequired.Reset();
    this->NotifyClientAuthStatusChanged.Reset();
}

bool FEOSDedicatedServerAntiCheat::Exec(UWorld *InWorld, const TCHAR *Cmd, FOutputDevice &Ar)
{
    return false;
}

TSharedPtr<FAntiCheatSession> FEOSDedicatedServerAntiCheat::CreateSession(
    bool bIsServer,
    const FUniqueNetIdEOS &HostUserId,
    bool bIsDedicatedServerSession,
    TSharedPtr<const FUniqueNetIdEOS> ListenServerUserId,
    FString ServerConnectionUrlOnClient)
{
#define FUNCTION_CONTEXT_MACRO                                                                                         \
    "Dedicated server Anti-Cheat: %p: CreateSession(bIsServer: %s, HostUserId: %s, bIsDedicatedServerSession: %s, "    \
    "ListenServerUserId: %s, ServerConnectionUrlOnClient: %s)"
#define FUNCTION_CONTEXT_ARGS                                                                                          \
    this, bIsServer ? TEXT("true") : TEXT("false"), *HostUserId.ToString(),                                            \
        bIsDedicatedServerSession ? TEXT("true") : TEXT("false"),                                                      \
        ListenServerUserId.IsValid() ? *ListenServerUserId->ToString() : TEXT("(none)"), *ServerConnectionUrlOnClient

    UE_LOG(LogEOSAntiCheat, Verbose, TEXT(FUNCTION_CONTEXT_MACRO ": Called"), FUNCTION_CONTEXT_ARGS);

    checkf(bIsServer, TEXT("Dedicated server Anti-Cheat expects bIsServer=true"));

    if (this->CurrentSession.IsValid())
    {
        UE_LOG(
            LogEOSAntiCheat,
            Verbose,
            TEXT(FUNCTION_CONTEXT_MACRO ": Current session is already valid, "
                                        "incrementing stack count to %d."),
            FUNCTION_CONTEXT_ARGS,
            this->CurrentSession->StackCount + 1);

        this->CurrentSession->StackCount++;
        return this->CurrentSession;
    }

    EOS_AntiCheatServer_BeginSessionOptions Opts = {};
    Opts.ApiVersion = EOS_ANTICHEATSERVER_BEGINSESSION_API_LATEST;
    Opts.RegisterTimeoutSeconds = 60; // Recommended value for EOS
    Opts.ServerName = nullptr;        // @todo: Use the session ID for NAME_GameSession
    Opts.bEnableGameplayData = false; // @todo: Implement gameplay data.
    Opts.LocalUserId = nullptr;
    EOS_EResult Result = EOS_AntiCheatServer_BeginSession(this->EOSACServer, &Opts);
    if (Result != EOS_EResult::EOS_Success)
    {
        UE_LOG(
            LogEOSAntiCheat,
            Error,
            TEXT(FUNCTION_CONTEXT_MACRO ": Unable to begin dedicated server session (got result %s)."),
            FUNCTION_CONTEXT_ARGS,
            ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
        return nullptr;
    }

    TSharedRef<FEOSDedicatedServerAntiCheatSession> Session = MakeShared<FEOSDedicatedServerAntiCheatSession>();
    Session->StackCount = 1;
    this->CurrentSession = Session;

    UE_LOG(
        LogEOSAntiCheat,
        Verbose,
        TEXT(FUNCTION_CONTEXT_MACRO ": Started new session; current session is now %p."),
        FUNCTION_CONTEXT_ARGS,
        this->CurrentSession.Get());

    return Session;

#undef FUNCTION_CONTEXT_MACRO
#undef FUNCTION_CONTEXT_ARGS
}

bool FEOSDedicatedServerAntiCheat::DestroySession(FAntiCheatSession &Session)
{
#define FUNCTION_CONTEXT_MACRO "Dedicated server Anti-Cheat: %p: DestroySession(Session: %p)"
#define FUNCTION_CONTEXT_ARGS this, &Session

    UE_LOG(LogEOSAntiCheat, Verbose, TEXT(FUNCTION_CONTEXT_MACRO ": Called"), FUNCTION_CONTEXT_ARGS);

    if (!this->CurrentSession.IsValid())
    {
        UE_LOG(
            LogEOSAntiCheat,
            Verbose,
            TEXT(FUNCTION_CONTEXT_MACRO ": There is no current session, ignoring DestroySession call."),
            FUNCTION_CONTEXT_ARGS);

        return true;
    }

    checkf(&Session == this->CurrentSession.Get(), TEXT("Session in DestroySession must match current session!"));
    this->CurrentSession->StackCount--;

    UE_LOG(
        LogEOSAntiCheat,
        Verbose,
        TEXT(FUNCTION_CONTEXT_MACRO ": Decremented stack count of session; stack count is now at %d."),
        FUNCTION_CONTEXT_ARGS,
        this->CurrentSession->StackCount);

    if (this->CurrentSession->StackCount == 0)
    {
        EOS_AntiCheatServer_EndSessionOptions Opts = {};
        Opts.ApiVersion = EOS_ANTICHEATSERVER_ENDSESSION_API_LATEST;
        EOS_EResult Result = EOS_AntiCheatServer_EndSession(this->EOSACServer, &Opts);
        if (Result != EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT(FUNCTION_CONTEXT_MACRO ": Unable to end dedicated server session (got result %s)."),
                FUNCTION_CONTEXT_ARGS,
                ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
            return false;
        }

        UE_LOG(
            LogEOSAntiCheat,
            Verbose,
            TEXT(FUNCTION_CONTEXT_MACRO ": Successfully ended dedicated server session."),
            FUNCTION_CONTEXT_ARGS);

        this->CurrentSession.Reset();
    }

    return true;

#undef FUNCTION_CONTEXT_MACRO
#undef FUNCTION_CONTEXT_ARGS
}

bool FEOSDedicatedServerAntiCheat::RegisterPlayer(
    FAntiCheatSession &Session,
    const FUniqueNetIdEOS &UserId,
    EOS_EAntiCheatCommonClientType ClientType,
    EOS_EAntiCheatCommonClientPlatform ClientPlatform)
{
#define FUNCTION_CONTEXT_MACRO                                                                                         \
    "Dedicated server Anti-Cheat: %p: RegisterPlayer(Session: %p, UserId: %s, ClientType: %u, ClientPlatform: "        \
    "%u)"
#define FUNCTION_CONTEXT_ARGS this, &Session, *UserId.ToString(), ClientType, ClientPlatform

    UE_LOG(LogEOSAntiCheat, Verbose, TEXT(FUNCTION_CONTEXT_MACRO ": Called"), FUNCTION_CONTEXT_ARGS);

    checkf(&Session == this->CurrentSession.Get(), TEXT("Session in RegisterPlayer must match current session!"));

    FEOSDedicatedServerAntiCheatSession &ServerSession = (FEOSDedicatedServerAntiCheatSession &)Session;

    bool bShouldRegister = false;
    EOS_AntiCheatCommon_ClientHandle ClientHandle = ServerSession.AddPlayer(UserId, bShouldRegister);

    UE_LOG(
        LogEOSAntiCheat,
        Verbose,
        TEXT(FUNCTION_CONTEXT_MACRO ": Should we register the specified player? %s"),
        FUNCTION_CONTEXT_ARGS,
        bShouldRegister ? TEXT("true") : TEXT("false"));

    if (bShouldRegister)
    {
        EOS_AntiCheatServer_RegisterClientOptions Opts = {};
        Opts.ApiVersion = EOS_ANTICHEATSERVER_REGISTERCLIENT_API_LATEST;
        Opts.ClientHandle = ClientHandle;
        Opts.ClientType = ClientType;
        Opts.ClientPlatform = ClientPlatform;
        EOSString_ProductUserId::AllocateToCharBuffer(UserId.GetProductUserId(), Opts.AccountId);
        Opts.IpAddress = nullptr; // We never use the IpAddress field for peers, because it will always be over EOS P2P.
        EOS_EResult Result = EOS_AntiCheatServer_RegisterClient(this->EOSACServer, &Opts);
        if (Result != EOS_EResult::EOS_Success)
        {
            ServerSession.RemovePlayer(UserId);
            UE_LOG(
                LogEOS,
                Error,
                TEXT(FUNCTION_CONTEXT_MACRO ": Unable to register client (got result %s)."),
                FUNCTION_CONTEXT_ARGS,
                ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
            return false;
        }

        UE_LOG(
            LogEOSAntiCheat,
            Verbose,
            TEXT(FUNCTION_CONTEXT_MACRO ": Successfully registered player with Anti-Cheat interface."),
            FUNCTION_CONTEXT_ARGS);
    }

    return true;

#undef FUNCTION_CONTEXT_MACRO
#undef FUNCTION_CONTEXT_ARGS
}

bool FEOSDedicatedServerAntiCheat::UnregisterPlayer(FAntiCheatSession &Session, const FUniqueNetIdEOS &UserId)
{
#define FUNCTION_CONTEXT_MACRO "Dedicated server Anti-Cheat: %p: UnregisterPlayer(Session: %p, UserId: %s)"
#define FUNCTION_CONTEXT_ARGS this, &Session, *UserId.ToString()

    UE_LOG(LogEOSAntiCheat, Verbose, TEXT(FUNCTION_CONTEXT_MACRO ": Called"), FUNCTION_CONTEXT_ARGS);

    checkf(&Session == this->CurrentSession.Get(), TEXT("Session in UnregisterPlayer must match current session!"));

    FEOSDedicatedServerAntiCheatSession &ServerSession = (FEOSDedicatedServerAntiCheatSession &)Session;

    if (ServerSession.ShouldDeregisterPlayerBeforeRemove(UserId))
    {
        UE_LOG(
            LogEOSAntiCheat,
            Verbose,
            TEXT(FUNCTION_CONTEXT_MACRO
                 ": Player tracking indicated we should try to deregister player with Anti-Cheat interface before "
                 "removing them from tracking."),
            FUNCTION_CONTEXT_ARGS);

        EOS_AntiCheatServer_UnregisterClientOptions Opts = {};
        Opts.ApiVersion = EOS_ANTICHEATSERVER_UNREGISTERCLIENT_API_LATEST;
        Opts.ClientHandle = ServerSession.GetHandle(UserId);
        EOS_EResult Result = EOS_AntiCheatServer_UnregisterClient(this->EOSACServer, &Opts);
        if (Result != EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("Anti-Cheat: Unable to unregister client (got result %s)."),
                ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
            return false;
        }

        UE_LOG(
            LogEOSAntiCheat,
            Verbose,
            TEXT(FUNCTION_CONTEXT_MACRO ": Successfully unregistered player from Anti-Cheat interface."),
            FUNCTION_CONTEXT_ARGS);
    }

    ServerSession.RemovePlayer(UserId);

    UE_LOG(
        LogEOSAntiCheat,
        Verbose,
        TEXT(FUNCTION_CONTEXT_MACRO ": Removed player from player tracking."),
        FUNCTION_CONTEXT_ARGS);

    return true;

#undef FUNCTION_CONTEXT_MACRO
#undef FUNCTION_CONTEXT_ARGS
}

bool FEOSDedicatedServerAntiCheat::ReceiveNetworkMessage(
    FAntiCheatSession &Session,
    const FUniqueNetIdEOS &SourceUserId,
    const FUniqueNetIdEOS &TargetUserId,
    const uint8 *Data,
    uint32_t Size)
{
#define FUNCTION_CONTEXT_MACRO                                                                                         \
    "Dedicated server Anti-Cheat: %p: ReceiveNetworkMessage(Session: %p, SourceUserId: %s, TargetUserId: %s, Data: "   \
    "..., Size: %u)"
#define FUNCTION_CONTEXT_ARGS this, &Session, *SourceUserId.ToString(), *TargetUserId.ToString(), Size

    EOS_AntiCheatServer_ReceiveMessageFromClientOptions Opts = {};
    Opts.ApiVersion = EOS_ANTICHEATSERVER_RECEIVEMESSAGEFROMCLIENT_API_LATEST;
    Opts.ClientHandle = this->CurrentSession->GetHandle(SourceUserId);
    Opts.Data = Data;
    Opts.DataLengthBytes = Size;
    EOS_EResult ReceiveResult = EOS_AntiCheatServer_ReceiveMessageFromClient(this->EOSACServer, &Opts);
    if (ReceiveResult != EOS_EResult::EOS_Success)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT(FUNCTION_CONTEXT_MACRO ": Failed to receive message from client: %s"),
            FUNCTION_CONTEXT_ARGS,
            ANSI_TO_TCHAR(EOS_EResult_ToString(ReceiveResult)));
        return false;
    }

    UE_LOG(
        LogEOSAntiCheat,
        VeryVerbose,
        TEXT(FUNCTION_CONTEXT_MACRO ": Received network message."),
        FUNCTION_CONTEXT_ARGS);

    return true;

#undef FUNCTION_CONTEXT_MACRO
#undef FUNCTION_CONTEXT_ARGS
}

#endif // #if WITH_SERVER_CODE && EOS_VERSION_AT_LEAST(1, 12, 0)