// Copyright June Rhodes. All Rights Reserved.

#include "SocketSubsystemEOSListenManager.h"

#include "./SocketEOSFull.h"
#include "EOSSocketRoleListening.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "SocketSubsystemEOSFull.h"

EOS_ENABLE_STRICT_WARNINGS

FString FSocketSubsystemEOSListenManager::FSocketEOSListeningKey::ToString() const
{
    FString LocalUserIdString;
    EOS_EResult LocalIdToStringResult = EOSString_ProductUserId::ToString(this->LocalUserId, LocalUserIdString);
    if (LocalIdToStringResult != EOS_EResult::EOS_Success)
    {
        return TEXT("");
    }

    return FString::Printf(
        TEXT("%s:%s"),
        *LocalUserIdString,
        *EOSString_P2P_SocketName::FromAnsiString(this->SymmetricSocketId.SocketName));
}

FSocketSubsystemEOSListenManager::FSocketSubsystemEOSListenManager(EOS_HP2P InP2P)
    : EOSP2P(InP2P)
    , ListeningSockets()
    , AcceptEvents()
    , ClosedEvents()
{
}

void FSocketSubsystemEOSListenManager::OnIncomingConnectionRequest(const EOS_P2P_OnIncomingConnectionRequestInfo *Info)
{
    FSocketEOSListeningKey ListeningKey(Info->LocalUserId, *(Info->SocketId));

    check(this->ListeningSockets.Contains(ListeningKey));

    TSharedRef<FUniqueNetIdEOS> LocalUserId = MakeShared<FUniqueNetIdEOS>(Info->LocalUserId);
    TSharedRef<FUniqueNetIdEOS> RemoteUserId = MakeShared<FUniqueNetIdEOS>(Info->RemoteUserId);

    bool bAccept = false;
    bool bAllNotBound = true;
    for (const auto &Socket : this->ListeningSockets[ListeningKey])
    {
        if (Socket->OnIncomingConnection.IsBound())
        {
            bAllNotBound = false;
            if (Socket->OnIncomingConnection.Execute(Socket, LocalUserId, RemoteUserId))
            {
                bAccept = true;
                break;
            }
        }
    }
    if (this->ListeningSockets[ListeningKey].Num() > 0 && bAllNotBound)
    {
        // If none of the sockets that are listening have an OnIncomingConnection delegate
        // registered, then we default to accept. This might be because the user of the socket
        // does not have access or is not using ISocketEOS, and is not filtering connections
        // at all.
        bAccept = true;
    }

    if (bAccept)
    {
        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("Accepting P2P connection from '%s' on socket ID '%s'"),
            *LocalUserId->ToString(),
            *EOSString_P2P_SocketName::FromAnsiString(Info->SocketId->SocketName));

        EOS_P2P_AcceptConnectionOptions AcceptOpts = {};
        AcceptOpts.ApiVersion = EOS_P2P_ACCEPTCONNECTION_API_LATEST;
        AcceptOpts.LocalUserId = Info->LocalUserId;
        AcceptOpts.RemoteUserId = Info->RemoteUserId;
        AcceptOpts.SocketId = Info->SocketId;

        EOS_EResult AcceptResult = EOS_P2P_AcceptConnection(this->EOSP2P, &AcceptOpts);
        if (AcceptResult != EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("Failed to accept connection, got status code %s"),
                ANSI_TO_TCHAR(EOS_EResult_ToString(AcceptResult)));
        }
    }
    else
    {
        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("Rejecting P2P connection from '%s' on socket ID '%s'"),
            *LocalUserId->ToString(),
            *EOSString_P2P_SocketName::FromAnsiString(Info->SocketId->SocketName));

        // In issue #216, the EOS SDK does not correctly tell the client that their
        // connection was rejected (it doesn't raise the RemoteConnectionClosed event). This
        // means the client doesn't know that their pending connection was terminated.
        //
        // However, we can workaround this by accepting-then-closing the connection
        // on the server. This seems to be enough to get the client to properly notify the
        // plugin that the connection was closed.

        EOS_P2P_AcceptConnectionOptions AcceptOpts = {};
        AcceptOpts.ApiVersion = EOS_P2P_ACCEPTCONNECTION_API_LATEST;
        AcceptOpts.LocalUserId = Info->LocalUserId;
        AcceptOpts.RemoteUserId = Info->RemoteUserId;
        AcceptOpts.SocketId = Info->SocketId;

        EOS_EResult AcceptResult = EOS_P2P_AcceptConnection(this->EOSP2P, &AcceptOpts);
        if (AcceptResult != EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("Failed to temporarily accept connection prior to reject, got status code %s"),
                ANSI_TO_TCHAR(EOS_EResult_ToString(AcceptResult)));
        }

        // Now actually close/reject the connection.

        EOS_P2P_CloseConnectionOptions RejectOpts = {};
        RejectOpts.ApiVersion = EOS_P2P_CLOSECONNECTION_API_LATEST;
        RejectOpts.LocalUserId = Info->LocalUserId;
        RejectOpts.RemoteUserId = Info->RemoteUserId;
        RejectOpts.SocketId = Info->SocketId;

        EOS_EResult RejectResult = EOS_P2P_CloseConnection(this->EOSP2P, &RejectOpts);
        if (RejectResult != EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("Failed to reject connection, got status code %s"),
                ANSI_TO_TCHAR(EOS_EResult_ToString(RejectResult)));
        }
    }
}

void FSocketSubsystemEOSListenManager::OnRemoteConnectionClosed(const EOS_P2P_OnRemoteConnectionClosedInfo *Info)
{
    FSocketEOSListeningKey ClosedListeningKey(Info->LocalUserId, *Info->SocketId);
    FString ClosedRemoteUserId;
    verify(EOSString_ProductUserId::ToString(Info->RemoteUserId, ClosedRemoteUserId) == EOS_EResult::EOS_Success);

    // Find all of the listening keys that might want to know about this connection closing.
    // @todo: This is tightly coupled to the internal state of sockets; we should probably abstract it out at some
    // point.
    for (const auto &ListeningKey : this->ListeningSockets)
    {
        if (ListeningKey.Key.ToString() == ClosedListeningKey.ToString())
        {
            for (const auto &ListeningSocket : ListeningKey.Value)
            {
                auto ListeningRoleState = StaticCastSharedRef<FEOSSocketRoleListeningState>(ListeningSocket->RoleState);

                for (auto i = ListeningRoleState->RemoteSockets.Num() - 1; i >= 0; i--)
                {
                    auto RemoteSocket = ListeningRoleState->RemoteSockets[i];
                    if (auto RemoteSocketPinned = RemoteSocket.Pin())
                    {
                        auto InternetAddr = StaticCastSharedRef<FInternetAddrEOSFull>(
                            RemoteSocketPinned->SocketSubsystem->CreateInternetAddr());
                        RemoteSocketPinned->GetPeerAddress(*InternetAddr);
                        FString RemoteUserId;
                        verify(
                            EOSString_ProductUserId::ToString(InternetAddr->GetUserId(), RemoteUserId) ==
                            EOS_EResult::EOS_Success);
                        if (RemoteUserId == ClosedRemoteUserId)
                        {
                            UE_LOG(
                                LogEOS,
                                Verbose,
                                TEXT("Listening socket %s detected that remote socket %s was closed by the remote "
                                     "host; removing remote socket."),
                                *ListeningKey.Key.ToString(),
                                *RemoteUserId);

                            // Close the remote socket.
                            RemoteSocketPinned->Close();
                        }
                    }
                }
            }
        }
    }

    UE_LOG(LogEOS, Verbose, TEXT("Remote connection closed!"));
}

bool FSocketSubsystemEOSListenManager::Add(FSocketEOSFull &InSocket, const FInternetAddrEOSFull &InLocalAddr)
{
    FSocketEOSListeningKey ListeningKey(InLocalAddr.GetUserId(), InLocalAddr.GetSymmetricSocketId());

    if (this->ListeningSockets.Contains(ListeningKey))
    {
        this->ListeningSockets[ListeningKey].Add(InSocket.AsShared());
        return true;
    }

    // New socket to listen on; set up the events.

    EOS_P2P_SocketId SocketId = InLocalAddr.GetSymmetricSocketId();

    {
        EOS_P2P_AddNotifyPeerConnectionRequestOptions AddOpts = {};
        AddOpts.ApiVersion = EOS_P2P_ADDNOTIFYPEERCONNECTIONREQUEST_API_LATEST;
        AddOpts.LocalUserId = InLocalAddr.GetUserId();
        AddOpts.SocketId = &SocketId;

        this->AcceptEvents.Add(
            ListeningKey,
            EOSRegisterEvent<
                EOS_HP2P,
                EOS_P2P_AddNotifyPeerConnectionRequestOptions,
                EOS_P2P_OnIncomingConnectionRequestInfo>(
                this->EOSP2P,
                &AddOpts,
                EOS_P2P_AddNotifyPeerConnectionRequest,
                EOS_P2P_RemoveNotifyPeerConnectionRequest,
                [WeakThis = GetWeakThis(this)](const EOS_P2P_OnIncomingConnectionRequestInfo *Info) {
                    if (auto This = PinWeakThis(WeakThis))
                    {
                        This->OnIncomingConnectionRequest(Info);
                    }
                }));
    }

    {
        EOS_P2P_AddNotifyPeerConnectionClosedOptions CloseOpts = {};
        CloseOpts.ApiVersion = EOS_P2P_ADDNOTIFYPEERCONNECTIONCLOSED_API_LATEST;
        CloseOpts.LocalUserId = InLocalAddr.GetUserId();
        CloseOpts.SocketId = &SocketId;

        this->ClosedEvents.Add(
            ListeningKey,
            EOSRegisterEvent<
                EOS_HP2P,
                EOS_P2P_AddNotifyPeerConnectionClosedOptions,
                EOS_P2P_OnRemoteConnectionClosedInfo>(
                this->EOSP2P,
                &CloseOpts,
                EOS_P2P_AddNotifyPeerConnectionClosed,
                EOS_P2P_RemoveNotifyPeerConnectionClosed,
                [WeakThis = GetWeakThis(this)](const EOS_P2P_OnRemoteConnectionClosedInfo *Info) {
                    if (auto This = PinWeakThis(WeakThis))
                    {
                        This->OnRemoteConnectionClosed(Info);
                    }
                }));
    }

    this->ListeningSockets.Add(ListeningKey, TArray<TSharedRef<FSocketEOSFull>>());
    this->ListeningSockets[ListeningKey].Add(InSocket.AsShared());

    return true;
}

bool FSocketSubsystemEOSListenManager::Remove(FSocketEOSFull &InSocket, const FInternetAddrEOSFull &InLocalAddr)
{
    FSocketEOSListeningKey ListeningKey(InLocalAddr.GetUserId(), InLocalAddr.GetSymmetricSocketId());

    if (!this->ListeningSockets.Contains(ListeningKey))
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("FSocketSubsystemEOSListenManager::Remove called for socket that is not registered with listen "
                 "manager"));
        return false;
    }

    auto OldNum = this->ListeningSockets[ListeningKey].Num();
    this->ListeningSockets[ListeningKey].Remove(InSocket.AsShared());
    check(this->ListeningSockets[ListeningKey].Num() == OldNum - 1);

    if (this->ListeningSockets[ListeningKey].Num() == 0)
    {
        this->ListeningSockets.Remove(ListeningKey);

        // This will also de-register the events.
        this->AcceptEvents.Remove(ListeningKey);
        this->ClosedEvents.Remove(ListeningKey);
    }

    return true;
}

bool FSocketSubsystemEOSListenManager::GetListeningSocketForNewInboundConnection(
    const FSocketEOSKey &InLookupKey,
    TSharedPtr<FSocketEOSFull> &OutSocket)
{
    FSocketEOSListeningKey ListeningKey(InLookupKey.LocalUserId, InLookupKey.SymmetricSocketId);

    if (this->ListeningSockets.Contains(ListeningKey))
    {
        for (const auto &Socket : this->ListeningSockets[ListeningKey])
        {
            if (Socket->BindAddress->GetSymmetricChannel() == InLookupKey.SymmetricChannel)
            {
                OutSocket = Socket;
                return true;
            }
        }
    }

    return false;
}

EOS_DISABLE_STRICT_WARNINGS