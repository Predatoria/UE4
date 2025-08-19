// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/EOSMessagingHub.h"

#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/IInternetAddrEOS.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/ISocketSubsystemEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineIdentityInterfaceEOS.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"

#define MSG_HUB_SOCKET_ID TEXT("MsgHub")
#define MSG_HUB_CHANNEL_ID 1
#define MSG_HUB_MAX_SIZE EOS_P2P_MAX_PACKET_SIZE

FEOSMessagingHub::FEOSMessagingHub(
    const TSharedRef<FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> &InIdentity,
    const TSharedRef<ISocketSubsystemEOS> &InSocketSubsystem)
    : OnMessageReceivedDelegate()
    , Identity(InIdentity)
    , SocketSubsystem(InSocketSubsystem)
    , LocalUserSockets()
    , MessagesPendingAck()
    , ConnectedSockets()
    , RecvBuffer((uint8 *)FMemory::Malloc(MSG_HUB_MAX_SIZE))
    , NextMsgId(1000)
    , TickHandle()
{
}

FEOSMessagingHub::~FEOSMessagingHub()
{
    FMemory::Free(this->RecvBuffer);
    FUTicker::GetCoreTicker().RemoveTicker(this->TickHandle);
    TSet<FSocket *> SocketsToDestroy;
    for (const auto &Socket : this->ConnectedSockets)
    {
        SocketsToDestroy.Add(&*Socket.Socket);
    }
    for (const auto &KV : this->LocalUserSockets)
    {
        SocketsToDestroy.Add(&*KV.Value);
    }
    this->ConnectedSockets.Empty();
    this->LocalUserSockets.Empty();
    this->MessagesPendingAck.Empty();
    for (const auto &Socket : SocketsToDestroy)
    {
        this->SocketSubsystem->DestroySocket(Socket);
    }
}

void FEOSMessagingHub::OnLoginStatusChanged(
    int32 LocalUserNum,
    ELoginStatus::Type OldStatus,
    ELoginStatus::Type NewStatus,
    const FUniqueNetId &NewId)
{
    if (OldStatus == ELoginStatus::NotLoggedIn && NewStatus == ELoginStatus::LoggedIn)
    {
        // Create listening socket for messaging hub.
        if (!this->LocalUserSockets.Contains(LocalUserNum))
        {
            TSharedRef<const FUniqueNetIdEOS> NewIdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(NewId.AsShared());
            TSharedPtr<IInternetAddrEOS> BindAddr =
                StaticCastSharedRef<IInternetAddrEOS>(this->SocketSubsystem->CreateInternetAddr());
            BindAddr->SetFromParameters(NewIdEOS->GetProductUserId(), MSG_HUB_SOCKET_ID, MSG_HUB_CHANNEL_ID);
            ISocketEOS *Socket = (ISocketEOS *)this->SocketSubsystem->CreateSocket(
                FName(TEXT("MsgSocket")),
                FString(TEXT("")),
                REDPOINT_EOS_SUBSYSTEM);
            Socket->OnConnectionClosed.BindSP(this, &FEOSMessagingHub::OnConnectionClosed);
            Socket->OnConnectionAccepted.BindSP(this, &FEOSMessagingHub::OnConnectionAccepted, LocalUserNum);
            Socket->bEnableReliableSending = true;
            check(Socket != nullptr);
            verify(Socket->Bind(*BindAddr));
            verify(Socket->Listen(0));
            this->LocalUserSockets.Add(LocalUserNum, Socket->AsSharedEOS());
        }
        else
        {
            UE_LOG(LogEOS, Warning, TEXT("Local user already had a message hub socket listening!"));
        }
    }
    else if (OldStatus == ELoginStatus::LoggedIn && NewStatus == ELoginStatus::NotLoggedIn)
    {
        // Destroy listening socket for messaging hub.
        if (this->LocalUserSockets.Contains(LocalUserNum))
        {
            FSocket *Socket = &*this->LocalUserSockets[LocalUserNum];
            for (int i = this->ConnectedSockets.Num() - 1; i >= 0; i--)
            {
                if (this->ConnectedSockets[i].LocalUserNum == LocalUserNum)
                {
                    FSocket *ChildSocket = &*this->ConnectedSockets[i].Socket;
                    this->ConnectedSockets.RemoveAt(i);
                    this->SocketSubsystem->DestroySocket(ChildSocket);
                }
            }
            this->LocalUserSockets.Remove(LocalUserNum);
            this->SocketSubsystem->DestroySocket(Socket);
        }
        else
        {
            UE_LOG(LogEOS, Warning, TEXT("Local user does not have a message hub socket listening!"));
        }
    }
}

void FEOSMessagingHub::OnConnectionAccepted(
    const TSharedRef<class ISocketEOS> &ListeningSocket,
    const TSharedRef<class ISocketEOS> &AcceptedSocket,
    const TSharedRef<class FUniqueNetIdEOS> &LocalUser,
    const TSharedRef<class FUniqueNetIdEOS> &RemoteUser,
    int32 LocalUserNum)
{
    auto AcceptedSocketEOS = AcceptedSocket->AsSharedEOS();
    this->ConnectedSockets.Add(FConnectedSocketPair(
        LocalUserNum,
        StaticCastSharedPtr<const FUniqueNetIdEOS>(this->Identity->GetUniquePlayerId(LocalUserNum)).ToSharedRef(),
        AcceptedSocketEOS));
}

bool FEOSMessagingHub::Tick(float DeltaSeconds)
{
    for (FConnectedSocketPair &Pair : this->ConnectedSockets)
    {
        int32 BytesRead;
        TSharedRef<IInternetAddrEOS> ReceivedAddr =
            StaticCastSharedRef<IInternetAddrEOS>(this->SocketSubsystem->CreateInternetAddr());
        if (Pair.Socket
                ->RecvFrom(this->RecvBuffer, MSG_HUB_MAX_SIZE, BytesRead, *ReceivedAddr, ESocketReceiveFlags::None))
        {
            Pair.ExpiresAt = FDateTime::UtcNow() + FTimespan::FromMinutes(3);

            TArray<uint8> ReadBuffer(this->RecvBuffer, BytesRead);
            FMemoryReader Reader(ReadBuffer, true);
            int32 MsgId;
            FString Type, Data;
            bool bIsAck;
            Reader << MsgId;
            Reader << bIsAck;
            if (!bIsAck)
            {
                Reader << Type;
                Reader << Data;

                // Send ack - this happens on accepted sockets.
                TArray<uint8> Bytes;
                FMemoryWriter Writer(Bytes, true);
                bool bDidAck = true;
                Writer << MsgId;
                Writer << bDidAck;
                Writer.Flush();
                int32 BytesSent;
                if (Pair.Socket->SendTo(Bytes.GetData(), Bytes.Num(), BytesSent, *ReceivedAddr) &&
                    BytesSent == Bytes.Num())
                {
                    this->OnMessageReceived()
                        .Broadcast(*MakeShared<FUniqueNetIdEOS>(ReceivedAddr->GetUserId()), *Pair.OwnerId, Type, Data);
                }
            }
            else
            {
                // Lookup ack - this happens on outbound sockets.
                if (this->MessagesPendingAck.Contains(MsgId))
                {
                    auto &MessageAcked = this->MessagesPendingAck[MsgId];
                    if (*MessageAcked.SenderId == *Pair.OwnerId)
                    {
                        MessageAcked.Callback.ExecuteIfBound(true);
                        this->MessagesPendingAck.Remove(MsgId);
                    }
                    else
                    {
                        UE_LOG(LogEOS, Warning, TEXT("Received ack for incorrect owner in EOS messaging hub."));
                    }
                }
                else
                {
                    UE_LOG(LogEOS, Warning, TEXT("Missing message ID to ack in EOS messaging hub."));
                }
            }
        }
    }

    TSet<FSocket *> SocketsToClose;
    FDateTime ExpireNow = FDateTime::UtcNow();
    for (int i = this->ConnectedSockets.Num() - 1; i >= 0; i--)
    {
        if (this->ConnectedSockets[i].ExpiresAt < ExpireNow)
        {
            UE_LOG(
                LogEOS,
                Verbose,
                TEXT("Closing message hub connection as it has been idle for more than 3 minutes."));
            SocketsToClose.Add(&this->ConnectedSockets[i].Socket.Get());
            this->ConnectedSockets.RemoveAt(i);
        }
    }
    if (SocketsToClose.Num() > 0)
    {
        TArray<int32> MessagesToTimeout;
        for (const auto &KV : this->MessagesPendingAck)
        {
            if (SocketsToClose.Contains(&KV.Value.Socket.Get()))
            {
                MessagesToTimeout.Add(KV.Key);
            }
        }
        for (const auto &Key : MessagesToTimeout)
        {
            this->MessagesPendingAck[Key].Callback.ExecuteIfBound(false);
            this->MessagesPendingAck.Remove(Key);
        }
        for (const auto &Socket : SocketsToClose)
        {
            this->SocketSubsystem->DestroySocket(Socket);
        }
    }

    return true;
}

void FEOSMessagingHub::OnConnectionClosed(
    const TSharedRef<class ISocketEOS> &ListeningSocket,
    const TSharedRef<class ISocketEOS> &ClosedSocket)
{
    for (int i = this->ConnectedSockets.Num() - 1; i >= 0; i--)
    {
        if (&this->ConnectedSockets[i].Socket.Get() == &ClosedSocket.Get())
        {
            this->ConnectedSockets.RemoveAt(i);
        }
    }

    TArray<int32> MessagesToTimeout;
    for (const auto &KV : this->MessagesPendingAck)
    {
        if (&KV.Value.Socket.Get() == &ClosedSocket.Get())
        {
            MessagesToTimeout.Add(KV.Key);
        }
    }
    for (const auto &Key : MessagesToTimeout)
    {
        this->MessagesPendingAck[Key].Callback.ExecuteIfBound(false);
        this->MessagesPendingAck.Remove(Key);
    }
}

void FEOSMessagingHub::RegisterEvents()
{
    for (int32 i = 0; i < MAX_LOCAL_PLAYERS; i++)
    {
        this->Identity->AddOnLoginStatusChangedDelegate_Handle(
            i,
            FOnLoginStatusChangedDelegate::CreateSP(this, &FEOSMessagingHub::OnLoginStatusChanged));
    }

    this->TickHandle = FUTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateSP(this, &FEOSMessagingHub::Tick));
}

FOnEOSHubMessageReceived &FEOSMessagingHub::OnMessageReceived()
{
    return this->OnMessageReceivedDelegate;
}

void FEOSMessagingHub::SendMessage(
    const FUniqueNetIdEOS &SenderId,
    const FUniqueNetIdEOS &ReceiverId,
    const FString &MessageType,
    const FString &MessageData,
    const FOnEOSHubMessageSent &Delegate)
{
    int32 LocalUserNum;
    if (!this->Identity->GetLocalUserNum(SenderId, LocalUserNum))
    {
        UE_LOG(LogEOS, Warning, TEXT("Sender for EOS messaging hub is not a locally signed in user."));
        Delegate.ExecuteIfBound(false);
        return;
    }

    TSharedRef<const FUniqueNetIdEOS> SenderIdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(SenderId.AsShared());
    TSharedRef<const FUniqueNetIdEOS> ReceiverIdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(ReceiverId.AsShared());
    TSharedPtr<IInternetAddrEOS> DestAddr =
        StaticCastSharedRef<IInternetAddrEOS>(this->SocketSubsystem->CreateInternetAddr());
    DestAddr->SetFromParameters(ReceiverIdEOS->GetProductUserId(), MSG_HUB_SOCKET_ID, MSG_HUB_CHANNEL_ID);

    TSharedPtr<ISocketEOS> SocketEOS;
    TSharedPtr<IInternetAddrEOS> PeerAddr =
        StaticCastSharedRef<IInternetAddrEOS>(this->SocketSubsystem->CreateInternetAddr());
    for (auto &Pair : this->ConnectedSockets)
    {
        if (Pair.Socket->GetPeerAddress(*PeerAddr))
        {
            if (*MakeShared<FUniqueNetIdEOS>(PeerAddr->GetUserId()) == ReceiverId)
            {
                // We already have a connection to this user, because they previously connected to us.
                SocketEOS = Pair.Socket;
                Pair.ExpiresAt = FDateTime::UtcNow() + FTimespan::FromMinutes(3);
                break;
            }
        }
    }
    if (!SocketEOS.IsValid())
    {
        // We haven't yet made a connection to this user. Explicitly establish one.
        TSharedPtr<IInternetAddrEOS> BindAddr =
            StaticCastSharedRef<IInternetAddrEOS>(this->SocketSubsystem->CreateInternetAddr());
        BindAddr->SetFromParameters(SenderIdEOS->GetProductUserId(), MSG_HUB_SOCKET_ID, MSG_HUB_CHANNEL_ID);
        ISocketEOS *Socket = (ISocketEOS *)this->SocketSubsystem->CreateSocket(
            FName(TEXT("MsgSocket")),
            FString(TEXT("")),
            REDPOINT_EOS_SUBSYSTEM);
        Socket->OnConnectionClosed.BindSP(this, &FEOSMessagingHub::OnConnectionClosed);
        Socket->bEnableReliableSending = true;
        check(Socket != nullptr);
        verify(Socket->Bind(*BindAddr));
        verify(Socket->Connect(*DestAddr));
        check(DestAddr->GetPort() == BindAddr->GetPort());
        this->ConnectedSockets.Add(FConnectedSocketPair(LocalUserNum, SenderIdEOS, Socket->AsSharedEOS()));
        SocketEOS = Socket->AsSharedEOS();
    }

    int32 MsgId = this->NextMsgId++;
    this->MessagesPendingAck.Add(MsgId, FMessageAckData(SenderIdEOS, ReceiverIdEOS, SocketEOS.ToSharedRef(), Delegate));

    TArray<uint8> Bytes;
    FMemoryWriter Writer(Bytes, true);
    bool bIsAck = false;
    FString MessageTypeCopy = MessageType;
    FString MessageDataCopy = MessageData;
    Writer << MsgId;
    Writer << bIsAck;
    Writer << MessageTypeCopy;
    Writer << MessageDataCopy;
    Writer.Flush();
    int32 BytesSent;
    if (!SocketEOS->SendTo(Bytes.GetData(), Bytes.Num(), BytesSent, *DestAddr) || BytesSent != Bytes.Num())
    {
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("Failed to call SendTo to send EOS messaging hub message to recipient, or couldn't send all the "
                 "desired data."));
        this->MessagesPendingAck.Remove(MsgId);
        Delegate.ExecuteIfBound(false);
    }
}