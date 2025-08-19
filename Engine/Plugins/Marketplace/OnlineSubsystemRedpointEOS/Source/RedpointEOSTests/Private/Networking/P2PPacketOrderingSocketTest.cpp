// Copyright June Rhodes. All Rights Reserved.

#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/IInternetAddrEOS.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/ISocketSubsystemEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/MultiOperation.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_MANAGED_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_Networking_P2PPacketOrderingSocket,
    "OnlineSubsystemEOS.Networking.P2PPacketOrderingSocket",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter,
    FOnlineSubsystemEOS_Networking_P2PPacketOrderingSocket_Manager);

class FOnlineSubsystemEOS_Networking_P2PPacketOrderingSocket_Manager
    : public TSharedFromThis<FOnlineSubsystemEOS_Networking_P2PPacketOrderingSocket_Manager>
{
public:
    UE_NONCOPYABLE(FOnlineSubsystemEOS_Networking_P2PPacketOrderingSocket_Manager);

    typedef FOnlineSubsystemEOS_Networking_P2PPacketOrderingSocket_Manager TThisClass;

    class FOnlineSubsystemEOS_Networking_P2PPacketOrderingSocket *T;
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

    TSharedPtr<class ISocketSubsystemEOS> HostSocketSubsystem;
    TSharedPtr<class ISocketSubsystemEOS> ClientSocketSubsystem;

    FSocket *HostSocket;
    FSocket *ClientSocket;

    TSharedPtr<FInternetAddr> HostAddress;
    TSharedPtr<FInternetAddr> ClientAddress;

    bool bClientHasConnected;

    void Start();

    void SendPacketFromHostToClient();
    void SendPacketFromClientToHost();

    void ReceivePacketFromHost();
    void ReceivePacketFromClient();

    bool OnTick(float DeltaTime);

    // NOLINTNEXTLINE(unreal-field-not-initialized-in-constructor)
    FOnlineSubsystemEOS_Networking_P2PPacketOrderingSocket_Manager(
        class FOnlineSubsystemEOS_Networking_P2PPacketOrderingSocket *InT,
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

        this->bClientHasConnected = false;

        this->HostSocketSubsystem =
            StaticCastSharedPtr<FOnlineSubsystemEOS>(this->Host.Subsystem.Pin())->SocketSubsystem;
        this->ClientSocketSubsystem =
            StaticCastSharedPtr<FOnlineSubsystemEOS>(this->Client.Subsystem.Pin())->SocketSubsystem;

        this->HostSocket = nullptr;
        this->ClientSocket = nullptr;
    }
    ~FOnlineSubsystemEOS_Networking_P2PPacketOrderingSocket_Manager()
    {
        if (this->HostSocketSubsystem && this->HostSocket)
        {
            this->HostSocketSubsystem->DestroySocket(this->HostSocket);
            this->HostSocket = nullptr;
        }
        if (this->ClientSocketSubsystem && this->ClientSocket)
        {
            this->ClientSocketSubsystem->DestroySocket(this->ClientSocket);
            this->ClientSocket = nullptr;
        }
    }
};

void FOnlineSubsystemEOS_Networking_P2PPacketOrderingSocket_Manager::Start()
{
    this->HostSocket = this->HostSocketSubsystem->CreateSocket(NAME_DGram, TEXT("SocketTestListen"), NAME_None);
    this->ClientSocket = this->ClientSocketSubsystem->CreateSocket(NAME_DGram, TEXT("SocketTestListen"), NAME_None);
    if (!(T->TestTrue("Host socket was created", this->HostSocket != nullptr) &&
          T->TestTrue("Client socket was created", this->ClientSocket != nullptr)))
    {
        this->OnDone();
        return;
    }

    auto HostBindAddr = StaticCastSharedRef<IInternetAddrEOS>(this->HostSocketSubsystem->CreateInternetAddr());
    HostBindAddr->SetFromParameters(this->Host.UserId->GetProductUserId(), TEXT("default"), 1);
    bool bOk = T->TestTrue("Host socket can bind", this->HostSocket->Bind(*HostBindAddr));
    bOk = bOk && T->TestTrue("Host socket can listen", this->HostSocket->Listen(0));
    if (!bOk)
    {
        this->OnDone();
        return;
    }

    this->HostAddress = this->HostSocketSubsystem->CreateInternetAddr();
    this->HostSocket->GetAddress(*this->HostAddress);

    auto ClientBindAddr = StaticCastSharedRef<IInternetAddrEOS>(this->HostSocketSubsystem->CreateInternetAddr());
    ClientBindAddr->SetFromParameters(this->Client.UserId->GetProductUserId(), TEXT("default"), 1);
    bOk = bOk && T->TestTrue("Client socket can bind", this->ClientSocket->Bind(*ClientBindAddr));
    bOk = bOk && T->TestTrue("Client socket can connect", this->ClientSocket->Connect(*this->HostAddress));
    if (!bOk)
    {
        this->OnDone();
        return;
    }

    this->ClientAddress = this->ClientSocketSubsystem->CreateInternetAddr();
    this->ClientSocket->GetAddress(*this->ClientAddress);

    FUTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateSP(this, &TThisClass::OnTick));
}

void FOnlineSubsystemEOS_Networking_P2PPacketOrderingSocket_Manager::SendPacketFromHostToClient()
{
    int32 BytesSent;
    T->TestTrue(
        "Host can sent data to client",
        this->HostSocket->SendTo(
            (uint8 *)&this->HostSentSequence,
            sizeof(this->HostSentSequence),
            BytesSent,
            *this->ClientAddress));

    if (this->HostStartSequence != -1)
    {
        UE_LOG(LogEOSTests, Verbose, TEXT("host send: value %d"), this->HostSentSequence);
    }

    this->HostSentSequence++;
}

void FOnlineSubsystemEOS_Networking_P2PPacketOrderingSocket_Manager::SendPacketFromClientToHost()
{
    int32 BytesSent;
    T->TestTrue(
        "Client can sent data to host",
        this->ClientSocket->SendTo(
            (uint8 *)&this->ClientSentSequence,
            sizeof(this->ClientSentSequence),
            BytesSent,
            *this->HostAddress));

    if (this->ClientStartSequence != -1)
    {
        UE_LOG(LogEOSTests, Verbose, TEXT("clie send: value %d"), this->ClientSentSequence);
    }

    this->ClientSentSequence++;
}

void FOnlineSubsystemEOS_Networking_P2PPacketOrderingSocket_Manager::ReceivePacketFromHost()
{
    uint32 PendingDataSize;
    if (this->HostSocket->HasPendingData(PendingDataSize))
    {
        int Storage = 0;
        uint8_t Channel = 0;
        uint32_t DataBytes = 0;

        int32 BytesRead;
        TSharedRef<FInternetAddr> RecvFrom = this->HostSocketSubsystem->CreateInternetAddr();
        bool GotData = T->TestTrue(
            "Host can receive packet when it has pending data",
            this->HostSocket->RecvFrom((uint8 *)&Storage, sizeof(Storage), BytesRead, *RecvFrom));
        if (GotData && T->TestEqual("Host receives correct amount of data", BytesRead, sizeof(Storage)))
        {
            this->bClientHasConnected = true;

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
}

void FOnlineSubsystemEOS_Networking_P2PPacketOrderingSocket_Manager::ReceivePacketFromClient()
{
    uint32 PendingDataSize;
    if (this->ClientSocket->HasPendingData(PendingDataSize))
    {
        int Storage = 0;
        uint8_t Channel = 0;
        uint32_t DataBytes = 0;

        int32 BytesRead;
        TSharedRef<FInternetAddr> RecvFrom = this->ClientSocketSubsystem->CreateInternetAddr();
        bool GotData = T->TestTrue(
            "Client can receive packet when it has pending data",
            this->ClientSocket->RecvFrom((uint8 *)&Storage, sizeof(Storage), BytesRead, *RecvFrom));
        if (GotData && T->TestEqual("Client receives correct amount of data", BytesRead, sizeof(Storage)))
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
}

bool FOnlineSubsystemEOS_Networking_P2PPacketOrderingSocket_Manager::OnTick(float DeltaTime)
{
    if (this->bClientHasConnected)
    {
        this->SendPacketFromHostToClient();
        this->SendPacketFromHostToClient();
        this->SendPacketFromHostToClient();
    }

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

void FOnlineSubsystemEOS_Networking_P2PPacketOrderingSocket::CreateManagingInstance(
    const FOnDone &OnDone,
    const std::function<void(const TSharedRef<FOnlineSubsystemEOS_Networking_P2PPacketOrderingSocket_Manager> &)>
        &OnInstanceCreated)
{
    CreateSubsystemsForTest_CreateOnDemand(
        this,
        2,
        OnDone,
        [this, OnInstanceCreated](const TArray<FMultiplayerScenarioInstance> &Instances, const FOnDone &OnDone) {
            auto Instance = MakeShared<FOnlineSubsystemEOS_Networking_P2PPacketOrderingSocket_Manager>(
                this,
                Instances[0],
                Instances[1],
                OnDone);
            OnInstanceCreated(Instance);
            Instance->Start();
        });
}
