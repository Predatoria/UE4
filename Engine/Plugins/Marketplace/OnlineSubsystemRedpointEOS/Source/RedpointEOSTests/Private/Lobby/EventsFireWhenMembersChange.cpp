// Copyright June Rhodes. All Rights Reserved.

#include "Containers/Ticker.h"
#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "Misc/Guid.h"
#include "RedpointEOSInterfaces/Private/Interfaces/OnlineLobbyInterface.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

#if defined(IMPLEMENT_MANAGED_ASYNC_AUTOMATION_TEST)

IMPLEMENT_MANAGED_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange,
    "OnlineSubsystemEOS.OnlineLobbyInterface.EventsFireWhenMembersChange",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter,
    FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager);

class FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager
    : public TSharedFromThis<FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager>
{
public:
    UE_NONCOPYABLE(FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager);
    virtual ~FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager() = default;

    typedef FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager TThisClass;

    class FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange *T;
    FMultiplayerScenarioInstance Host;
    FMultiplayerScenarioInstance Client;
    TWeakPtr<IOnlineLobby, ESPMode::ThreadSafe> HostLobbyWk;
    TWeakPtr<IOnlineLobby, ESPMode::ThreadSafe> ClientLobbyWk;
    std::function<void()> OnDone;

    TSharedPtr<class FOnlineLobby> HostLobby;
    bool bGotMemberConnectedOnHostEvent;
    bool bGotMemberUpdatedOnHostEvent;
    bool bGotMemberUpdatedOnClientEvent;
    bool bGotMemberDisconnectedOnHostEvent;
    bool bGotMemberDisconnectedOnClientEvent;

    void FinalizeThenOnDone();
    void FinalizeThenOnDone_DeleteLobbyOnHost(const FOnlineError &Error, const FUniqueNetId &UserId);

    void Start_CreateLobbyOnHost();
    void Handle_CreateLobbyOnHost(
        const FOnlineError &Error,
        const FUniqueNetId &UserId,
        const TSharedPtr<class FOnlineLobby> &Lobby);

    void Start_ConnectLobbyOnClient();
    void Handle_ConnectLobbyOnClient(
        const FOnlineError &Error,
        const FUniqueNetId &UserId,
        const TSharedPtr<class FOnlineLobby> &Lobby);

    void Start_UpdateSelfDataOnClient();
    void Handle_UpdateSelfDataOnClient(const FOnlineError &Error, const FUniqueNetId &UserId);

    void Start_DisconnectLobbyOnClient();
    void Handle_DisconnectLobbyOnClient(const FOnlineError &Error, const FUniqueNetId &UserId);

    void Start();

    void HandleHost_MemberConnected(
        const FUniqueNetId &UserId,
        const FOnlineLobbyId &LobbyId,
        const FUniqueNetId &MemberId);
    void HandleHost_MemberUpdated(
        const FUniqueNetId &UserId,
        const FOnlineLobbyId &LobbyId,
        const FUniqueNetId &MemberId);
    void HandleHost_MemberDisconnected(
        const FUniqueNetId &UserId,
        const FOnlineLobbyId &LobbyId,
        const FUniqueNetId &MemberId,
        bool bWasKicked);
    void HandleClient_MemberUpdated(
        const FUniqueNetId &UserId,
        const FOnlineLobbyId &LobbyId,
        const FUniqueNetId &MemberId);
    void HandleClient_MemberDisconnected(
        const FUniqueNetId &UserId,
        const FOnlineLobbyId &LobbyId,
        const FUniqueNetId &MemberId,
        bool bWasKicked);

    // NOLINTNEXTLINE(unreal-field-not-initialized-in-constructor)
    FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager(
        class FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange *InT,
        FMultiplayerScenarioInstance InHost,
        FMultiplayerScenarioInstance InClient,
        std::function<void()> InOnDone)
    {
        this->T = InT;
        this->Host = MoveTemp(InHost);
        this->Client = MoveTemp(InClient);
        this->OnDone = MoveTemp(InOnDone);
        this->HostLobby = nullptr;
        this->bGotMemberConnectedOnHostEvent = false;
        this->bGotMemberUpdatedOnHostEvent = false;
        this->bGotMemberUpdatedOnClientEvent = false;
        this->bGotMemberDisconnectedOnHostEvent = false;
        this->bGotMemberDisconnectedOnClientEvent = false;

        this->HostLobbyWk = Online::GetLobbyInterface(this->Host.Subsystem.Pin().Get());
        this->ClientLobbyWk = Online::GetLobbyInterface(this->Client.Subsystem.Pin().Get());
    }
};

void FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager::FinalizeThenOnDone()
{
    T->TestTrue("Host got member connected event", this->bGotMemberConnectedOnHostEvent);
    T->TestTrue("Host got member updated event", this->bGotMemberUpdatedOnHostEvent);
    T->TestTrue("Host got member disconnected event", this->bGotMemberDisconnectedOnHostEvent);
    T->TestTrue("Client got member updated event", this->bGotMemberUpdatedOnClientEvent);
    T->TestTrue("Client got member disconnected event", this->bGotMemberDisconnectedOnClientEvent);

    if (this->HostLobby.IsValid())
    {
        if (auto HostLobbyInterface = this->HostLobbyWk.Pin())
        {
            if (HostLobbyInterface->DeleteLobby(
                    *this->Host.UserId,
                    *this->HostLobby->Id,
                    FOnLobbyOperationComplete::CreateSP(this, &TThisClass::FinalizeThenOnDone_DeleteLobbyOnHost)))
            {
                return;
            }
        }
    }

    this->OnDone();
    return;
}

void FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager::FinalizeThenOnDone_DeleteLobbyOnHost(
    const FOnlineError &Error,
    const FUniqueNetId &UserId)
{
    T->TestTrue("Host lobby was deleted successfully", Error.bSucceeded);
    this->OnDone();
}

void FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager::Start_CreateLobbyOnHost()
{
    if (auto HostLobbyInterface = this->HostLobbyWk.Pin())
    {
        TSharedPtr<FOnlineLobbyTransaction> Txn = HostLobbyInterface->MakeCreateLobbyTransaction(*this->Host.UserId);
        if (!T->TestTrue("CreateLobbyTransaction should return new transaction", Txn.IsValid()))
        {
            this->FinalizeThenOnDone();
            return;
        }

        Txn->Capacity = 4;
        Txn->Locked = true;
        Txn->Public = true;

        if (!T->TestTrue(
                "CreateLobby operation started",
                HostLobbyInterface->CreateLobby(
                    *this->Host.UserId,
                    *Txn,
                    FOnLobbyCreateOrConnectComplete::CreateSP(this, &TThisClass::Handle_CreateLobbyOnHost))))
        {
            this->FinalizeThenOnDone();
            return;
        }
    }
    else
    {
        this->FinalizeThenOnDone();
    }
}

void FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager::Handle_CreateLobbyOnHost(
    const FOnlineError &Error,
    const FUniqueNetId &UserId,
    const TSharedPtr<class FOnlineLobby> &Lobby)
{
    if (!T->TestTrue("CreateLobby operation was successful", Error.bSucceeded) ||
        !T->TestTrue("CreateLobby user ID is host", UserId == *this->Host.UserId))
    {
        this->FinalizeThenOnDone();
        return;
    }

    this->HostLobby = Lobby;

    this->Start_ConnectLobbyOnClient();
}

void FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager::Start_ConnectLobbyOnClient()
{
    if (auto ClientLobbyInterface = this->ClientLobbyWk.Pin())
    {
        if (!T->TestTrue(
                "ConnectLobby operation started",
                ClientLobbyInterface->ConnectLobby(
                    *this->Client.UserId,
                    *this->HostLobby->Id,
                    FOnLobbyCreateOrConnectComplete::CreateSP(this, &TThisClass::Handle_ConnectLobbyOnClient))))
        {
            this->FinalizeThenOnDone();
            return;
        }
    }
    else
    {
        this->FinalizeThenOnDone();
    }
}

void FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager::Handle_ConnectLobbyOnClient(
    const FOnlineError &Error,
    const FUniqueNetId &UserId,
    const TSharedPtr<class FOnlineLobby> &Lobby)
{
    if (!T->TestTrue("ConnectLobby operation was successful", Error.bSucceeded) ||
        !T->TestTrue("ConnectLobby user ID is client", UserId == *this->Client.UserId) ||
        !T->TestTrue("ConnectLobby lobby ID matches host", *Lobby->Id == *this->HostLobby->Id))
    {
        this->FinalizeThenOnDone();
        return;
    }

    this->Start_UpdateSelfDataOnClient();
}

void FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager::Start_UpdateSelfDataOnClient()
{
    if (auto ClientLobbyInterface = this->ClientLobbyWk.Pin())
    {
        TSharedPtr<FOnlineLobbyMemberTransaction> Txn = ClientLobbyInterface->MakeUpdateLobbyMemberTransaction(
            *this->Client.UserId,
            *this->HostLobby->Id,
            *this->Client.UserId);
        if (!T->TestTrue("MakeUpdateLobbyMemberTransaction should return new transaction", Txn.IsValid()))
        {
            this->FinalizeThenOnDone();
            return;
        }

        Txn->SetMetadata.Add(TEXT("TestBool"), FVariantData((bool)true));
        Txn->SetMetadata.Add(TEXT("TestInt64"), FVariantData((int64)1));
        Txn->SetMetadata.Add(TEXT("TestDouble"), FVariantData((double)1.0));
        Txn->SetMetadata.Add(TEXT("TestString"), FVariantData(FString(TEXT("HelloWorld"))));

        if (!T->TestTrue(
                "UpdateMemberSelf operation started",
                ClientLobbyInterface->UpdateMemberSelf(
                    *this->Client.UserId,
                    *this->HostLobby->Id,
                    *Txn,
                    FOnLobbyOperationComplete::CreateSP(this, &TThisClass::Handle_UpdateSelfDataOnClient))))
        {
            this->FinalizeThenOnDone();
            return;
        }
    }
    else
    {
        this->FinalizeThenOnDone();
    }
}

void FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager::Handle_UpdateSelfDataOnClient(
    const FOnlineError &Error,
    const FUniqueNetId &UserId)
{
    if (!T->TestTrue("UpdateMemberSelf operation was successful", Error.bSucceeded) ||
        !T->TestTrue("UpdateMemberSelf user ID is client", UserId == *this->Client.UserId))
    {
        this->FinalizeThenOnDone();
        return;
    }

    this->Start_DisconnectLobbyOnClient();
}

void FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager::Start_DisconnectLobbyOnClient()
{
    if (auto ClientLobbyInterface = this->ClientLobbyWk.Pin())
    {
        if (!T->TestTrue(
                "DisconnectLobby operation started",
                ClientLobbyInterface->DisconnectLobby(
                    *this->Client.UserId,
                    *this->HostLobby->Id,
                    FOnLobbyOperationComplete::CreateSP(this, &TThisClass::Handle_DisconnectLobbyOnClient))))
        {
            this->FinalizeThenOnDone();
            return;
        }
    }
    else
    {
        this->FinalizeThenOnDone();
    }
}

void FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager::Handle_DisconnectLobbyOnClient(
    const FOnlineError &Error,
    const FUniqueNetId &UserId)
{
    if (!T->TestTrue("DisconnectLobby operation was successful", Error.bSucceeded) ||
        !T->TestTrue("DisconnectLobby user ID is client", UserId == *this->Client.UserId))
    {
        this->FinalizeThenOnDone();
        return;
    }

    this->FinalizeThenOnDone();
}

void FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager::Start()
{
    if (auto HostLobbyInterface = this->HostLobbyWk.Pin())
    {
        HostLobbyInterface->AddOnMemberConnectDelegate_Handle(
            FOnMemberConnectDelegate::CreateSP(this, &TThisClass::HandleHost_MemberConnected));
        HostLobbyInterface->AddOnMemberUpdateDelegate_Handle(
            FOnMemberUpdateDelegate::CreateSP(this, &TThisClass::HandleHost_MemberUpdated));
        HostLobbyInterface->AddOnMemberDisconnectDelegate_Handle(
            FOnMemberDisconnectDelegate::CreateSP(this, &TThisClass::HandleHost_MemberDisconnected));
    }
    if (auto ClientLobbyInterface = this->ClientLobbyWk.Pin())
    {
        ClientLobbyInterface->AddOnMemberUpdateDelegate_Handle(
            FOnMemberUpdateDelegate::CreateSP(this, &TThisClass::HandleClient_MemberUpdated));
        ClientLobbyInterface->AddOnMemberDisconnectDelegate_Handle(
            FOnMemberDisconnectDelegate::CreateSP(this, &TThisClass::HandleClient_MemberDisconnected));
    }

    this->Start_CreateLobbyOnHost();
}

void FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager::HandleHost_MemberConnected(
    const FUniqueNetId &UserId,
    const FOnlineLobbyId &LobbyId,
    const FUniqueNetId &MemberId)
{
    if (this->HostLobby.IsValid() && LobbyId == *this->HostLobby->Id && UserId == *this->Host.UserId &&
        MemberId == *this->Client.UserId)
    {
        this->bGotMemberConnectedOnHostEvent = true;
    }
}

void FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager::HandleHost_MemberUpdated(
    const FUniqueNetId &UserId,
    const FOnlineLobbyId &LobbyId,
    const FUniqueNetId &MemberId)
{
    if (this->HostLobby.IsValid() && LobbyId == *this->HostLobby->Id && UserId == *this->Host.UserId &&
        MemberId == *this->Client.UserId)
    {
        this->bGotMemberUpdatedOnHostEvent = true;
    }
}

void FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager::HandleHost_MemberDisconnected(
    const FUniqueNetId &UserId,
    const FOnlineLobbyId &LobbyId,
    const FUniqueNetId &MemberId,
    bool bWasKicked)
{
    if (this->HostLobby.IsValid() && LobbyId == *this->HostLobby->Id && UserId == *this->Host.UserId &&
        MemberId == *this->Client.UserId)
    {
        this->bGotMemberDisconnectedOnHostEvent = true;
    }
}

void FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager::HandleClient_MemberUpdated(
    const FUniqueNetId &UserId,
    const FOnlineLobbyId &LobbyId,
    const FUniqueNetId &MemberId)
{
    if (this->HostLobby.IsValid() && LobbyId == *this->HostLobby->Id && UserId == *this->Client.UserId &&
        MemberId == *this->Client.UserId)
    {
        this->bGotMemberUpdatedOnClientEvent = true;
    }
}

void FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager::HandleClient_MemberDisconnected(
    const FUniqueNetId &UserId,
    const FOnlineLobbyId &LobbyId,
    const FUniqueNetId &MemberId,
    bool bWasKicked)
{
    if (this->HostLobby.IsValid() && LobbyId == *this->HostLobby->Id && UserId == *this->Client.UserId &&
        MemberId == *this->Client.UserId)
    {
        this->bGotMemberDisconnectedOnClientEvent = true;
    }
}

void FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange::CreateManagingInstance(
    const FOnDone &OnDone,
    const std::function<
        void(const TSharedRef<FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager> &)>
        &OnInstanceCreated)
{
    CreateSubsystemsForTest_CreateOnDemand(
        this,
        2,
        OnDone,
        [this, OnInstanceCreated](const TArray<FMultiplayerScenarioInstance> &Instances, const FOnDone &OnDone) {
            auto Instance = MakeShared<FOnlineSubsystemEOS_OnlineLobbyInterface_EventsFireWhenMembersChange_Manager>(
                this,
                Instances[0],
                Instances[1],
                OnDone);
            OnInstanceCreated(Instance);
            Instance->Start();
        });
}

#endif