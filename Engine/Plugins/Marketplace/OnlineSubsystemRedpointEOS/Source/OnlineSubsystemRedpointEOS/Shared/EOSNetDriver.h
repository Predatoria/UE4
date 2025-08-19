// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IpNetDriver.h"
#include "OnlineSubsystemRedpointEOS/Shared/AntiCheat/AntiCheat.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "UObject/ObjectMacros.h"

#if defined(EOS_BUILD_PLATFORM_NAME)
#include "eos_platform_prereqs.h"

#include "eos_base.h"
#endif

#include "eos_p2p_types.h"

#include "EOSNetDriver.generated.h"

EOS_ENABLE_STRICT_WARNINGS

#if !UE_BUILD_SHIPPING
#define EOS_SUPPORT_MULTI_IP_RESOLUTION 1
#endif

enum EEOSNetDriverRole : int8
{
    DedicatedServer,
    ListenServer,
    ClientConnectedToDedicatedServer,
    ClientConnectedToListenServer,
};

UCLASS(transient, config = OnlineSubsystemRedpointEOS)
class ONLINESUBSYSTEMREDPOINTEOS_API UEOSNetDriver : public UIpNetDriver
{
    friend class FConnectClientPIEToHostPIEBeacon;
    friend class FConnectClientPIEToHostPIE;
    friend class FCreateClientControlledBeaconToPIE;
    friend class UEOSControlChannel;
    friend class UEOSIpNetConnection;
    friend class FNetworkHelpers;

    GENERATED_BODY()

private:
    TWeakPtr<class ISocketEOS> Socket;
    TWeakPtr<class ISocketSubsystemEOS> SocketSubsystem;
    TWeakPtr<IAntiCheat> AntiCheat;
    TSharedPtr<FAntiCheatSession> AntiCheatSession;
    bool IsPerformingIpConnection;
    TWeakPtr<class FOnlineSubsystemEOS, ESPMode::ThreadSafe> RegisteredListeningSubsystem;
    EOS_ProductUserId RegisteredListeningUser;
    bool bDidRegisterListeningUser;
    bool bIsOwnedByBeacon;
#if defined(EOS_SUPPORT_MULTI_IP_RESOLUTION)
    TSharedPtr<class FSocketSubsystemMultiIpResolve> MultiIpResolveSubsystem;
#endif

    class UWorld *FindWorld();
    bool IsOwnedByBeacon();
    bool GetSubsystems(
        TSharedPtr<class ISocketSubsystemEOS> &OutSocketSubsystem,
        TSharedPtr<class FOnlineSubsystemEOS, ESPMode::ThreadSafe> &OutOnlineSubsystem);
    bool CanPerformP2PNetworking(const TSharedPtr<class ISocketSubsystemEOS> &InSocketSubsystem);
    bool CreateEOSSocket(
        const TSharedPtr<class ISocketSubsystemEOS> &InSocketSubsystem,
        const FString &InDescription,
        const FURL &InURL,
        bool bListening,
        TSharedPtr<class ISocketEOS> &OutSocket,
        EOS_ProductUserId &OutBindingUserId);

    void SendAntiCheatData(
        const TSharedRef<FAntiCheatSession> &Session,
        const FUniqueNetIdEOS &SourceUserId,
        const FUniqueNetIdEOS &TargetUserId,
        const uint8 *Data,
        uint32_t Size);

    bool Socket_OnIncomingConnection(
        const TSharedRef<class ISocketEOS> &Socket,
        const TSharedRef<class FUniqueNetIdEOS> &LocalUser,
        const TSharedRef<class FUniqueNetIdEOS> &RemoteUser);
    void Socket_OnConnectionAccepted(
        const TSharedRef<class ISocketEOS> &ListeningSocket,
        const TSharedRef<class ISocketEOS> &AcceptedSocket,
        const TSharedRef<class FUniqueNetIdEOS> &LocalUser,
        const TSharedRef<class FUniqueNetIdEOS> &RemoteUser);
    void Socket_OnConnectionClosed(
        const TSharedRef<class ISocketEOS> &ListeningSocket,
        const TSharedRef<class ISocketEOS> &ClosedSocket);

public:
    EEOSNetDriverRole GetEOSRole();

    virtual bool IsAvailable() const override;
    virtual bool IsNetResourceValid() override;
    virtual class ISocketSubsystem *GetSocketSubsystem() override;

    virtual void PostInitProperties() override;
    virtual bool InitConnect(FNetworkNotify *InNotify, const FURL &ConnectURL, FString &Error) override;
    virtual bool InitListen(FNetworkNotify *InNotify, FURL &ListenURL, bool bReuseAddressAndPort, FString &Error)
        override;

    // We strip out the StatelessConnectionHandler from the packet handler stack for
    // P2P connections, since the P2P framework already manages the initial connection
    // (and thus, there's no point including any of the DDoS/stateless connection management
    // for P2P connections; it just makes the connection process more brittle).
    virtual void InitConnectionlessHandler() override;


    // Checks for incoming packets on the socket, and dispatches them to the connections.
    virtual void TickDispatch(float DeltaTime) override;

    virtual void LowLevelSend(
        TSharedPtr<const FInternetAddr> Address,
        void *Data,
        int32 CountBits,
        FOutPacketTraits &Traits) override;
    virtual void LowLevelDestroy() override;
};

EOS_DISABLE_STRICT_WARNINGS