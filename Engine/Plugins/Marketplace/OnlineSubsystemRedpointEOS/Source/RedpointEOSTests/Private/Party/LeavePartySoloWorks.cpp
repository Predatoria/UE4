// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "./TestPartyManager.h"
#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/HeapLambda.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlinePartyInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_MANAGED_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloWorks,
    "OnlineSubsystemEOS.OnlinePartyInterface.LeavePartySoloWorks",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter,
    FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloWorks_Manager);

class FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloWorks_Manager
    : public TSharedFromThis<FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloWorks_Manager>,
      public FSingleTestPartyManager
{
public:
    UE_NONCOPYABLE(FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloWorks_Manager);
    virtual ~FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloWorks_Manager() = default;

    typedef FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloWorks_Manager TThisClass;

    class FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloWorks *T;
    FMultiplayerScenarioInstance Host;
    TWeakPtr<IOnlinePartySystem, ESPMode::ThreadSafe> HostPartySystemWk;
    std::function<void()> OnDone;

    void Start_CreatePartyOnHost();
    void Handle_CreatePartyOnHost(
        const FUniqueNetId &LocalUserId,
        const TSharedPtr<const FOnlinePartyId> &PartyId,
        const ECreatePartyCompletionResult Result);

    void Start_LeavePartyOnHost();
    void Handle_LeavePartyOnHost(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const ELeavePartyCompletionResult Result);

    void Start();

    // NOLINTNEXTLINE(unreal-field-not-initialized-in-constructor)
    FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloWorks_Manager(
        class FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloWorks *InT,
        FMultiplayerScenarioInstance InHost,
        std::function<void()> InOnDone)
    {
        this->T = InT;
        this->Host = MoveTemp(InHost);
        this->OnDone = MoveTemp(InOnDone);

        this->HostPartySystemWk = this->Host.Subsystem.Pin()->GetPartyInterface();
    }
};

void FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloWorks_Manager::Start_CreatePartyOnHost()
{
    check(!this->HasParty(*this->Host.UserId));

    FOnlinePartyTypeId PartyTypeId = FOnlinePartyTypeId(1);

    FPartyConfiguration HostConfig;
    HostConfig.MaxMembers = 4;
    HostConfig.InvitePermissions = PartySystemPermissions::EPermissionType::Anyone;
    HostConfig.bChatEnabled = false;

    if (!T->TestTrue(
            "CreateParty operation started",
            this->HostPartySystemWk.Pin()->CreateParty(
                *this->Host.UserId,
                PartyTypeId,
                HostConfig,
                FOnCreatePartyComplete::CreateSP(this, &TThisClass::Handle_CreatePartyOnHost))))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloWorks_Manager::Handle_CreatePartyOnHost(
    const FUniqueNetId &LocalUserId,
    const TSharedPtr<const FOnlinePartyId> &PartyId,
    const ECreatePartyCompletionResult Result)
{
    if (!T->TestEqual("Party was successfully created", Result, ECreatePartyCompletionResult::Succeeded))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    this->AddParty(this->Host, *PartyId);

    TArray<TSharedRef<const FOnlinePartyId>> PartyIds;
    if (!T->TestTrue(
            "Can get parties list for user",
            this->HostPartySystemWk.Pin()->GetJoinedParties(LocalUserId, PartyIds)))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    if (!T->TestEqual("Joined party count is 1", PartyIds.Num(), 1))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    this->Start_LeavePartyOnHost();
}

void FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloWorks_Manager::Start_LeavePartyOnHost()
{
    check(this->HasParty(*this->Host.UserId));

    if (!T->TestTrue(
            "LeaveParty operation started",
            this->HostPartySystemWk.Pin()->LeaveParty(
                *this->Host.UserId,
                *this->Party(*this->Host.UserId),
                FOnLeavePartyComplete::CreateSP(this, &TThisClass::Handle_LeavePartyOnHost))))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloWorks_Manager::Handle_LeavePartyOnHost(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const ELeavePartyCompletionResult Result)
{
    if (!T->TestEqual("Party was successfully left", Result, ELeavePartyCompletionResult::Succeeded))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    this->RemoveParty(this->Host, PartyId);

    TArray<TSharedRef<const FOnlinePartyId>> PartyIds;
    if (!T->TestTrue(
            "Can get parties list for user",
            this->HostPartySystemWk.Pin()->GetJoinedParties(LocalUserId, PartyIds)))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    if (!T->TestEqual("Joined party count is 0", PartyIds.Num(), 0))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
}

void FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloWorks_Manager::Start()
{
    this->Start_CreatePartyOnHost();
}

void FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloWorks::CreateManagingInstance(
    const FOnDone &OnDone,
    const std::function<void(const TSharedRef<FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloWorks_Manager> &)>
        &OnInstanceCreated)
{
    CreateSubsystemsForTest_CreateOnDemand(
        this,
        1,
        OnDone,
        [this, OnInstanceCreated](const TArray<FMultiplayerScenarioInstance> &Instances, const FOnDone &OnDone) {
            auto Instance = MakeShared<FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloWorks_Manager>(
                this,
                Instances[0],
                OnDone);
            OnInstanceCreated(Instance);
            Instance->Start();
        });
}

#endif // #if EOS_HAS_AUTHENTICATION
