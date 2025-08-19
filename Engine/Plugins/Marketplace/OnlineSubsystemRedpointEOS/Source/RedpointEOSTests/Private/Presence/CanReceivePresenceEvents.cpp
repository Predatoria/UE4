// Copyright June Rhodes. All Rights Reserved.

#include "HAL/MemoryMisc.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/HeapLambda.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_MANAGED_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlinePresenceInterface_CanReceivePresenceEvents,
    "OnlineSubsystemEOS.OnlinePresenceInterface.CanReceivePresenceEvents",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter,
    FOnlineSubsystemEOS_OnlinePresenceInterface_CanReceivePresenceEvents_Manager);

class FOnlineSubsystemEOS_OnlinePresenceInterface_CanReceivePresenceEvents_Manager
    : public TSharedFromThis<FOnlineSubsystemEOS_OnlinePresenceInterface_CanReceivePresenceEvents_Manager>
{
public:
    UE_NONCOPYABLE(FOnlineSubsystemEOS_OnlinePresenceInterface_CanReceivePresenceEvents_Manager);
    virtual ~FOnlineSubsystemEOS_OnlinePresenceInterface_CanReceivePresenceEvents_Manager() = default;

    typedef FOnlineSubsystemEOS_OnlinePresenceInterface_CanReceivePresenceEvents_Manager TThisClass;

    class FOnlineSubsystemEOS_OnlinePresenceInterface_CanReceivePresenceEvents *T;
    FMultiplayerScenarioInstance Host;
    FMultiplayerScenarioInstance Client;
    TWeakPtr<IOnlinePresence, ESPMode::ThreadSafe> HostPresenceSystemWk;
    TWeakPtr<IOnlinePresence, ESPMode::ThreadSafe> ClientPresenceSystemWk;
    std::function<void()> OnDone;

    bool bPresenceDidFireInInitialQuery;
    bool bPresenceDidFireInSetHandler;
    FDelegateHandle PresenceDataReceivedHandle;
    FUTickerDelegateHandle PresenceTimeoutHandle;

    void Handle_OnPresenceReceived_BeforeSet(
        const class FUniqueNetId &UserId,
        const TSharedRef<FOnlineUserPresence> &Presence);
    void Handle_OnPresenceReceived_AfterSet(
        const class FUniqueNetId &UserId,
        const TSharedRef<FOnlineUserPresence> &Presence);

    void Start_QueryPresenceOnHost();
    void Handle_QueryPresenceOnHost(const class FUniqueNetId &UserId, const bool bWasSuccessful);

    void Start_SetPresenceOnClient();
    void Handle_SetPresenceOnClient(const class FUniqueNetId &UserId, const bool bWasSuccessful);

    bool Handle_OnPresenceTimeout(float DeltaSeconds);

    void Start();

    // NOLINTNEXTLINE(unreal-field-not-initialized-in-constructor)
    FOnlineSubsystemEOS_OnlinePresenceInterface_CanReceivePresenceEvents_Manager(
        class FOnlineSubsystemEOS_OnlinePresenceInterface_CanReceivePresenceEvents *InT,
        FMultiplayerScenarioInstance InHost,
        FMultiplayerScenarioInstance InClient,
        std::function<void()> InOnDone)
    {
        this->T = InT;
        this->Host = MoveTemp(InHost);
        this->Client = MoveTemp(InClient);
        this->OnDone = MoveTemp(InOnDone);

        this->HostPresenceSystemWk = this->Host.Subsystem.Pin()->GetPresenceInterface();
        this->ClientPresenceSystemWk = this->Client.Subsystem.Pin()->GetPresenceInterface();
    }
};

void FOnlineSubsystemEOS_OnlinePresenceInterface_CanReceivePresenceEvents_Manager::Handle_OnPresenceReceived_BeforeSet(
    const class FUniqueNetId &UserId,
    const TSharedRef<FOnlineUserPresence> &Presence)
{
    this->bPresenceDidFireInInitialQuery = true;
    this->HostPresenceSystemWk.Pin()->ClearOnPresenceReceivedDelegate_Handle(this->PresenceDataReceivedHandle);
    this->PresenceDataReceivedHandle.Reset();
}

void FOnlineSubsystemEOS_OnlinePresenceInterface_CanReceivePresenceEvents_Manager::Handle_OnPresenceReceived_AfterSet(
    const class FUniqueNetId &UserId,
    const TSharedRef<FOnlineUserPresence> &Presence)
{
    this->PresenceTimeoutHandle.Reset();

    // We got the update we were expecting.
    this->bPresenceDidFireInSetHandler = true;

    T->TestTrue(TEXT("OnPresenceReceived was fired in response to SetPresence from a friend"), true);
    this->OnDone();
}

void FOnlineSubsystemEOS_OnlinePresenceInterface_CanReceivePresenceEvents_Manager::Start_QueryPresenceOnHost()
{
    this->PresenceDataReceivedHandle = this->HostPresenceSystemWk.Pin()->AddOnPresenceReceivedDelegate_Handle(
        FOnPresenceReceivedDelegate::CreateSP(this, &TThisClass::Handle_OnPresenceReceived_BeforeSet));
    this->bPresenceDidFireInInitialQuery = false;

    this->HostPresenceSystemWk.Pin()->QueryPresence(
        *this->Host.UserId,
        IOnlinePresence::FOnPresenceTaskCompleteDelegate::CreateSP(this, &TThisClass::Handle_QueryPresenceOnHost));
}

void FOnlineSubsystemEOS_OnlinePresenceInterface_CanReceivePresenceEvents_Manager::Handle_QueryPresenceOnHost(
    const class FUniqueNetId &UserId,
    const bool bWasSuccessful)
{
    T->TestTrue(
        TEXT("OnPresenceReceived was fired during initial QueryPresence call"),
        this->bPresenceDidFireInInitialQuery);

    if (!this->bPresenceDidFireInInitialQuery)
    {
        this->HostPresenceSystemWk.Pin()->ClearOnPresenceReceivedDelegate_Handle(this->PresenceDataReceivedHandle);
        this->PresenceDataReceivedHandle.Reset();
        this->OnDone();
        return;
    }

    this->Start_SetPresenceOnClient();
}

void FOnlineSubsystemEOS_OnlinePresenceInterface_CanReceivePresenceEvents_Manager::Start_SetPresenceOnClient()
{
    this->PresenceDataReceivedHandle = this->HostPresenceSystemWk.Pin()->AddOnPresenceReceivedDelegate_Handle(
        FOnPresenceReceivedDelegate::CreateSP(this, &TThisClass::Handle_OnPresenceReceived_AfterSet));
    this->PresenceTimeoutHandle.Reset();

    this->bPresenceDidFireInSetHandler = false;

    FOnlineUserPresenceStatus Status;
    Status.State = EOnlinePresenceState::Online;
    Status.StatusStr = TEXT("This is a status update from an automated test");

    this->ClientPresenceSystemWk.Pin()->SetPresence(
        *this->Client.UserId,
        Status,
        IOnlinePresence::FOnPresenceTaskCompleteDelegate::CreateSP(this, &TThisClass::Handle_SetPresenceOnClient));
}

void FOnlineSubsystemEOS_OnlinePresenceInterface_CanReceivePresenceEvents_Manager::Handle_SetPresenceOnClient(
    const class FUniqueNetId &UserId,
    const bool bWasSuccessful)
{
    if (!this->bPresenceDidFireInSetHandler)
    {
        this->PresenceTimeoutHandle = FUTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateSP(this, &TThisClass::Handle_OnPresenceTimeout),
            10.f);
    }
}

bool FOnlineSubsystemEOS_OnlinePresenceInterface_CanReceivePresenceEvents_Manager::Handle_OnPresenceTimeout(
    float DeltaSeconds)
{
    if (this->PresenceTimeoutHandle.IsValid())
    {
        this->HostPresenceSystemWk.Pin()->ClearOnPresenceReceivedDelegate_Handle(this->PresenceDataReceivedHandle);
        this->PresenceDataReceivedHandle.Reset();

        T->TestTrue(TEXT("OnPresenceReceived was fired in response to SetPresence from a friend"), false);
        this->OnDone();
    }

    return false;
}

void FOnlineSubsystemEOS_OnlinePresenceInterface_CanReceivePresenceEvents_Manager::Start()
{
    this->Start_QueryPresenceOnHost();
}

void FOnlineSubsystemEOS_OnlinePresenceInterface_CanReceivePresenceEvents::CreateManagingInstance(
    const FOnDone &OnDone,
    const std::function<
        void(const TSharedRef<FOnlineSubsystemEOS_OnlinePresenceInterface_CanReceivePresenceEvents_Manager> &)>
        &OnInstanceCreated)
{
    CreateSubsystemsForTest_EpicGames(
        this,
        EEpicGamesAccountCount::Two,
        OnDone,
        [this, OnInstanceCreated](const TArray<FMultiplayerScenarioInstance> &Instances, const FOnDone &OnDone) {
            auto Instance = MakeShared<FOnlineSubsystemEOS_OnlinePresenceInterface_CanReceivePresenceEvents_Manager>(
                this,
                Instances[0],
                Instances[1],
                OnDone);
            OnInstanceCreated(Instance);
            Instance->Start();
        });
}
