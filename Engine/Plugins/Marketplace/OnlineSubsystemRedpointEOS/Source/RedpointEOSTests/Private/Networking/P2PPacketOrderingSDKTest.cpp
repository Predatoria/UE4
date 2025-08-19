// Copyright June Rhodes. All Rights Reserved.

#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/ISocketSubsystemEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/MultiOperation.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_MANAGED_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_Networking_P2PPacketOrderingSDK,
    "OnlineSubsystemEOS.Networking.P2PPacketOrderingSDK",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter,
    FOnlineSubsystemEOS_Networking_P2PPacketOrderingSDK_Manager);

class FOnlineSubsystemEOS_Networking_P2PPacketOrderingSDK_Manager
    : public TSharedFromThis<FOnlineSubsystemEOS_Networking_P2PPacketOrderingSDK_Manager>
{
public:
    UE_NONCOPYABLE(FOnlineSubsystemEOS_Networking_P2PPacketOrderingSDK_Manager);
    virtual ~FOnlineSubsystemEOS_Networking_P2PPacketOrderingSDK_Manager() = default;

    typedef FOnlineSubsystemEOS_Networking_P2PPacketOrderingSDK_Manager TThisClass;

    class FOnlineSubsystemEOS_Networking_P2PPacketOrderingSDK *T;
    FMultiplayerScenarioInstance Host;
    FMultiplayerScenarioInstance Client;
    std::function<void()> OnDone;

    int HostSentSequence;
    int HostReceivedSequence;
    int HostStartSequence;
    int ClientSentSequence;
    int ClientReceivedSequence;
    int ClientStartSequence;

    int HostTotal;
    int HostOOO;
    int ClientTotal;
    int ClientOOO;

    EOS_HP2P HostP2P;
    EOS_HP2P ClientP2P;
    EOS_ProductUserId HostUserId;
    EOS_ProductUserId ClientUserId;

    void Start();

    void SendPacketFromHostToClient();
    void SendPacketFromClientToHost();

    void ReceivePacketFromHost();
    void ReceivePacketFromClient();

    bool OnTick(float DeltaTime);

    // NOLINTNEXTLINE(unreal-field-not-initialized-in-constructor)
    FOnlineSubsystemEOS_Networking_P2PPacketOrderingSDK_Manager(
        class FOnlineSubsystemEOS_Networking_P2PPacketOrderingSDK *InT,
        FMultiplayerScenarioInstance InHost,
        FMultiplayerScenarioInstance InClient,
        std::function<void()> InOnDone)
    {
        this->T = InT;
        this->Host = MoveTemp(InHost);
        this->Client = MoveTemp(InClient);
        this->OnDone = MoveTemp(InOnDone);

        this->HostSentSequence = 0;
        this->HostReceivedSequence = -1;
        this->HostStartSequence = -1;
        this->ClientSentSequence = 0;
        this->ClientReceivedSequence = -1;
        this->ClientStartSequence = -1;

        this->HostTotal = 0;
        this->HostOOO = 0;
        this->ClientTotal = 0;
        this->ClientOOO = 0;

        StaticCastSharedPtr<FOnlineSubsystemEOS>(this->Host.Subsystem.Pin())->SocketSubsystem->SuspendReceive();
        StaticCastSharedPtr<FOnlineSubsystemEOS>(this->Client.Subsystem.Pin())->SocketSubsystem->SuspendReceive();

        this->HostP2P = EOS_Platform_GetP2PInterface(
            StaticCastSharedPtr<FOnlineSubsystemEOS>(this->Host.Subsystem.Pin())->GetPlatformInstance());
        this->ClientP2P = EOS_Platform_GetP2PInterface(
            StaticCastSharedPtr<FOnlineSubsystemEOS>(this->Client.Subsystem.Pin())->GetPlatformInstance());
        this->HostUserId = this->Host.UserId->GetProductUserId();
        this->ClientUserId = this->Client.UserId->GetProductUserId();
    }
};

void FOnlineSubsystemEOS_Networking_P2PPacketOrderingSDK_Manager::Start()
{
    // NOLINTNEXTLINE(unreal-ionlinesubsystem-get)
    FOnlineSubsystemEOS *OSS = (FOnlineSubsystemEOS *)IOnlineSubsystem::Get(REDPOINT_EOS_SUBSYSTEM);
    if (OSS != nullptr)
    {
        // We must suspend the editor trying to receive P2P packets
        OSS->SocketSubsystem->SuspendReceive();
    }

    FUTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateSP(this, &TThisClass::OnTick));
}

void FOnlineSubsystemEOS_Networking_P2PPacketOrderingSDK_Manager::SendPacketFromHostToClient()
{
    EOS_P2P_SocketId Socket = {};
    Socket.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;
    FMemory::Memzero(Socket.SocketName);
    FMemory::Memcpy(Socket.SocketName, "test", 5);

    EOS_P2P_SendPacketOptions SendOpts = {};
    SendOpts.ApiVersion = EOS_P2P_SENDPACKET_API_LATEST;
    SendOpts.LocalUserId = this->HostUserId;
    SendOpts.RemoteUserId = this->ClientUserId;
    SendOpts.SocketId = &Socket;
    SendOpts.Channel = 0;
    SendOpts.DataLengthBytes = sizeof(this->HostSentSequence);
    SendOpts.Data = &this->HostSentSequence;
    SendOpts.bAllowDelayedDelivery = false;
    SendOpts.Reliability = EOS_EPacketReliability::EOS_PR_UnreliableUnordered;

    T->TestEqual("SendPacket failed", EOS_P2P_SendPacket(this->HostP2P, &SendOpts), EOS_EResult::EOS_Success);

    if (this->HostStartSequence != -1)
    {
        UE_LOG(LogEOSTests, Verbose, TEXT("host send: value %d"), this->HostSentSequence);
    }

    this->HostSentSequence++;
}

void FOnlineSubsystemEOS_Networking_P2PPacketOrderingSDK_Manager::SendPacketFromClientToHost()
{
    EOS_P2P_SocketId Socket = {};
    Socket.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;
    FMemory::Memzero(Socket.SocketName);
    FMemory::Memcpy(Socket.SocketName, "test", 5);

    EOS_P2P_SendPacketOptions SendOpts = {};
    SendOpts.ApiVersion = EOS_P2P_SENDPACKET_API_LATEST;
    SendOpts.LocalUserId = this->ClientUserId;
    SendOpts.RemoteUserId = this->HostUserId;
    SendOpts.SocketId = &Socket;
    SendOpts.Channel = 0;
    SendOpts.DataLengthBytes = sizeof(this->ClientSentSequence);
    SendOpts.Data = &this->ClientSentSequence;
    SendOpts.bAllowDelayedDelivery = false;
    SendOpts.Reliability = EOS_EPacketReliability::EOS_PR_UnreliableUnordered;

    T->TestEqual("SendPacket failed", EOS_P2P_SendPacket(this->ClientP2P, &SendOpts), EOS_EResult::EOS_Success);

    if (this->ClientStartSequence != -1)
    {
        UE_LOG(LogEOSTests, Verbose, TEXT("clie send: value %d"), this->ClientSentSequence);
    }

    this->ClientSentSequence++;
}

void FOnlineSubsystemEOS_Networking_P2PPacketOrderingSDK_Manager::ReceivePacketFromHost()
{
    int Storage = 0;
    uint8_t Channel = 0;
    uint32_t DataBytes = 0;

    EOS_P2P_ReceivePacketOptions ReceiveOpts = {};
    ReceiveOpts.ApiVersion = EOS_P2P_RECEIVEPACKET_API_LATEST;
    ReceiveOpts.LocalUserId = this->HostUserId;
    ReceiveOpts.MaxDataSizeBytes = sizeof(Storage);
    ReceiveOpts.RequestedChannel = &Channel;

    EOS_ProductUserId ReceiveProductUserId;
    EOS_P2P_SocketId ReceiveSocketId = {};
    EOS_EResult ReceivePacketResult = EOS_P2P_ReceivePacket(
        this->HostP2P,
        &ReceiveOpts,
        &ReceiveProductUserId,
        &ReceiveSocketId,
        &Channel,
        &Storage,
        &DataBytes);
    if (ReceivePacketResult == EOS_EResult::EOS_Success && DataBytes == sizeof(Storage))
    {
        if (this->HostReceivedSequence == -1)
        {
            // Packets are dropped until connection is open. Handle it.
            this->HostReceivedSequence = Storage - 1;
            this->HostStartSequence = Storage;
        }
        UE_LOG(
            LogEOSTests,
            Verbose,
            TEXT("host recv: current %d .. actual %d == expect %d"),
            this->HostReceivedSequence,
            Storage,
            this->HostReceivedSequence + 1);
        if (Storage != this->HostReceivedSequence + 1)
        {
            this->HostOOO++;
        }
        this->HostTotal++;
        this->HostReceivedSequence = Storage;
    }
}

void FOnlineSubsystemEOS_Networking_P2PPacketOrderingSDK_Manager::ReceivePacketFromClient()
{
    int Storage = 0;
    uint8_t Channel = 0;
    uint32_t DataBytes = 0;

    EOS_P2P_ReceivePacketOptions ReceiveOpts = {};
    ReceiveOpts.ApiVersion = EOS_P2P_RECEIVEPACKET_API_LATEST;
    ReceiveOpts.LocalUserId = this->ClientUserId;
    ReceiveOpts.MaxDataSizeBytes = sizeof(Storage);
    ReceiveOpts.RequestedChannel = &Channel;

    EOS_ProductUserId ReceiveProductUserId;
    EOS_P2P_SocketId ReceiveSocketId = {};
    EOS_EResult ReceivePacketResult = EOS_P2P_ReceivePacket(
        this->ClientP2P,
        &ReceiveOpts,
        &ReceiveProductUserId,
        &ReceiveSocketId,
        &Channel,
        &Storage,
        &DataBytes);
    if (ReceivePacketResult == EOS_EResult::EOS_Success && DataBytes == sizeof(Storage))
    {
        if (this->ClientReceivedSequence == -1)
        {
            // Packets are dropped until connection is open. Handle it.
            this->ClientReceivedSequence = Storage - 1;
            this->ClientStartSequence = Storage;
        }
        UE_LOG(
            LogEOSTests,
            Verbose,
            TEXT("clie recv: current %d .. actual %d == expect %d"),
            this->ClientReceivedSequence,
            Storage,
            this->ClientReceivedSequence + 1);
        if (Storage != this->ClientReceivedSequence + 1)
        {
            this->ClientOOO++;
        }
        this->ClientTotal++;
        this->ClientReceivedSequence = Storage;
    }
}

bool FOnlineSubsystemEOS_Networking_P2PPacketOrderingSDK_Manager::OnTick(float DeltaTime)
{
    this->SendPacketFromHostToClient();
    this->SendPacketFromHostToClient();
    this->SendPacketFromHostToClient();

    this->SendPacketFromClientToHost();
    this->SendPacketFromClientToHost();
    this->SendPacketFromClientToHost();

    this->ReceivePacketFromClient();
    this->ReceivePacketFromClient();
    this->ReceivePacketFromClient();

    this->ReceivePacketFromHost();
    this->ReceivePacketFromHost();
    this->ReceivePacketFromHost();

    if (this->ClientReceivedSequence > this->ClientStartSequence + 100 &&
        this->HostReceivedSequence > this->HostStartSequence + 100)
    {
        // NOLINTNEXTLINE(unreal-ionlinesubsystem-get)
        FOnlineSubsystemEOS *OSS = (FOnlineSubsystemEOS *)IOnlineSubsystem::Get(REDPOINT_EOS_SUBSYSTEM);
        if (OSS != nullptr)
        {
            // We must suspend the editor trying to receive P2P packets
            OSS->SocketSubsystem->ResumeReceive();
        }

        float ClientOOOPerc = (float)this->ClientOOO / (float)this->ClientTotal;
        T->TestFalse(
            FString::Printf(
                TEXT("Client out-of-order was above the 10%% error threshold (error value: %f%%)"),
                ClientOOOPerc * 100.0f),
            ClientOOOPerc > 0.10f);

        float HostOOOPerc = (float)this->HostOOO / (float)this->HostTotal;
        T->TestFalse(
            FString::Printf(
                TEXT("Host out-of-order was above the 10%% error threshold (error value: %f%%)"),
                HostOOOPerc * 100.0f),
            HostOOOPerc > 0.10f);

        this->OnDone();
        return false;
    }

    return true;
}

void FOnlineSubsystemEOS_Networking_P2PPacketOrderingSDK::CreateManagingInstance(
    const FOnDone &OnDone,
    const std::function<void(const TSharedRef<FOnlineSubsystemEOS_Networking_P2PPacketOrderingSDK_Manager> &)>
        &OnInstanceCreated)
{
    CreateSubsystemsForTest_CreateOnDemand(
        this,
        2,
        OnDone,
        [this, OnInstanceCreated](const TArray<FMultiplayerScenarioInstance> &Instances, const FOnDone &OnDone) {
            auto Instance = MakeShared<FOnlineSubsystemEOS_Networking_P2PPacketOrderingSDK_Manager>(
                this,
                Instances[0],
                Instances[1],
                OnDone);
            OnInstanceCreated(Instance);
            Instance->Start();
        });
}
