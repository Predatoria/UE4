// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"

DECLARE_MULTICAST_DELEGATE_FourParams(
    FOnEOSHubMessageReceived,
    const FUniqueNetIdEOS & /* SenderId */,
    const FUniqueNetIdEOS & /* ReceiverId */,
    const FString & /* MessageType */,
    const FString & /* MessageData */);

DECLARE_DELEGATE_OneParam(FOnEOSHubMessageSent, bool /* bWasReceived */);

class ONLINESUBSYSTEMREDPOINTEOS_API FEOSMessagingHub : public TSharedFromThis<FEOSMessagingHub>
{
private:
    struct FConnectedSocketPair
    {
        int32 LocalUserNum;
        TSharedRef<const FUniqueNetIdEOS> OwnerId;
        TSharedRef<class ISocketEOS> Socket;
        FDateTime ExpiresAt;
        FConnectedSocketPair(
            int32 InLocalUserNum,
            TSharedRef<const FUniqueNetIdEOS> InOwnerId,
            TSharedRef<class ISocketEOS> InSocket)
            : LocalUserNum(InLocalUserNum)
            , OwnerId(MoveTemp(InOwnerId))
            , Socket(MoveTemp(InSocket))
            , ExpiresAt(FDateTime::UtcNow() + FTimespan::FromMinutes(3)){};
    };
    struct FMessageAckData
    {
        TSharedRef<const FUniqueNetIdEOS> SenderId;
        TSharedRef<const FUniqueNetIdEOS> ReceiverId;
        TSharedRef<class ISocketEOS> Socket;
        FOnEOSHubMessageSent Callback;
        FMessageAckData(
            TSharedRef<const FUniqueNetIdEOS> InSenderId,
            TSharedRef<const FUniqueNetIdEOS> InReceiverId,
            TSharedRef<class ISocketEOS> InSocket,
            FOnEOSHubMessageSent InCallback)
            : SenderId(MoveTemp(InSenderId))
            , ReceiverId(MoveTemp(InReceiverId))
            , Socket(MoveTemp(InSocket))
            , Callback(MoveTemp(InCallback)){};
    };

    FOnEOSHubMessageReceived OnMessageReceivedDelegate;
    TSharedRef<class FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> Identity;
    TSharedRef<class ISocketSubsystemEOS> SocketSubsystem;
    TMap<int32, TSharedRef<ISocketEOS>> LocalUserSockets;
    TMap<int32, FMessageAckData> MessagesPendingAck;
    TArray<FConnectedSocketPair> ConnectedSockets;
    uint8 *RecvBuffer;
    int32 NextMsgId;
    FUTickerDelegateHandle TickHandle;

    void OnLoginStatusChanged(
        int32 LocalUserNum,
        ELoginStatus::Type OldStatus,
        ELoginStatus::Type NewStatus,
        const FUniqueNetId &NewId);
    bool Tick(float DeltaSeconds);
    void OnConnectionClosed(
        const TSharedRef<class ISocketEOS> &ListeningSocket,
        const TSharedRef<class ISocketEOS> &ClosedSocket);
    void OnConnectionAccepted(
        const TSharedRef<class ISocketEOS> &ListeningSocket,
        const TSharedRef<class ISocketEOS> &AcceptedSocket,
        const TSharedRef<class FUniqueNetIdEOS> &LocalUser,
        const TSharedRef<class FUniqueNetIdEOS> &RemoteUser,
        int32 LocalUserNum);

public:
    FEOSMessagingHub(
        const TSharedRef<class FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> &InIdentity,
        const TSharedRef<class ISocketSubsystemEOS> &InSocketSubsystem);
    UE_NONCOPYABLE(FEOSMessagingHub);
    ~FEOSMessagingHub();
    void RegisterEvents();

    /**
     * The delegate that is fired whenever a message arrives from another EOS user.
     */
    FOnEOSHubMessageReceived &OnMessageReceived();

    /**
     * Send a message to another EOS user.
     */
    void SendMessage(
        const FUniqueNetIdEOS &SenderId,
        const FUniqueNetIdEOS &ReceiverId,
        const FString &MessageType,
        const FString &MessageData,
        const FOnEOSHubMessageSent &Delegate = FOnEOSHubMessageSent());
};