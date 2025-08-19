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
    FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloTwoPartiesWorks,
    "OnlineSubsystemEOS.OnlinePartyInterface.LeavePartySoloTwoPartiesWorks",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter,
    FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloTwoPartiesWorks_Manager);

class FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloTwoPartiesWorks_Manager
    : public TSharedFromThis<FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloTwoPartiesWorks_Manager>,
      public FMultiTestPartyManager
{
public:
    UE_NONCOPYABLE(FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloTwoPartiesWorks_Manager);
    virtual ~FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloTwoPartiesWorks_Manager() = default;

    typedef FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloTwoPartiesWorks_Manager TThisClass;

    class FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloTwoPartiesWorks *T;
    FMultiplayerScenarioInstance Host;
    TWeakPtr<IOnlinePartySystem, ESPMode::ThreadSafe> HostPartySystemWk;
    std::function<void()> OnDone;

    int PartiesCreated;

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
    FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloTwoPartiesWorks_Manager(
        class FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloTwoPartiesWorks *InT,
        FMultiplayerScenarioInstance InHost,
        std::function<void()> InOnDone)
    {
        this->T = InT;
        this->Host = MoveTemp(InHost);
        this->OnDone = MoveTemp(InOnDone);
        this->PartiesCreated = 0;

        this->HostPartySystemWk = this->Host.Subsystem.Pin()->GetPartyInterface();
    }
};

void FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloTwoPartiesWorks_Manager::Start_CreatePartyOnHost()
{
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

void FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloTwoPartiesWorks_Manager::Handle_CreatePartyOnHost(
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
    this->PartiesCreated++;

    TArray<TSharedRef<const FOnlinePartyId>> PartyIds;
    if (!T->TestTrue(
            "Can get parties list for user",
            this->HostPartySystemWk.Pin()->GetJoinedParties(LocalUserId, PartyIds)))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    if (!T->TestEqual(
            FString::Printf(TEXT("Joined party count is %d"), PartiesCreated),
            PartyIds.Num(),
            PartiesCreated))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    if (this->PartiesCreated == 1)
    {
        this->Start_CreatePartyOnHost();
    }
    else
    {
        this->Start_LeavePartyOnHost();
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloTwoPartiesWorks_Manager::Start_LeavePartyOnHost()
{
    check(this->PartyNum(*this->Host.UserId) > 0);
    check(this->PartyNum(*this->Host.UserId) == this->PartiesCreated);

    if (!T->TestTrue(
            "LeaveParty operation started",
            this->HostPartySystemWk.Pin()->LeaveParty(
                *this->Host.UserId,
                *this->Party(*this->Host.UserId, 0),
                FOnLeavePartyComplete::CreateSP(this, &TThisClass::Handle_LeavePartyOnHost))))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloTwoPartiesWorks_Manager::Handle_LeavePartyOnHost(
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
    this->PartiesCreated--;

    TArray<TSharedRef<const FOnlinePartyId>> PartyIds;
    if (!T->TestTrue(
            "Can get parties list for user",
            this->HostPartySystemWk.Pin()->GetJoinedParties(LocalUserId, PartyIds)))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    if (!T->TestEqual(
            FString::Printf(TEXT("Joined party count is %d"), PartiesCreated),
            PartyIds.Num(),
            PartiesCreated))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    if (this->PartiesCreated > 0)
    {
        this->Start_LeavePartyOnHost();
    }
    else
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloTwoPartiesWorks_Manager::Start()
{
    this->Start_CreatePartyOnHost();
}

void FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloTwoPartiesWorks::CreateManagingInstance(
    const FOnDone &OnDone,
    const std::function<
        void(const TSharedRef<FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloTwoPartiesWorks_Manager> &)>
        &OnInstanceCreated)
{
    CreateSubsystemsForTest_CreateOnDemand(
        this,
        1,
        OnDone,
        [this, OnInstanceCreated](const TArray<FMultiplayerScenarioInstance> &Instances, const FOnDone &OnDone) {
            auto Instance = MakeShared<FOnlineSubsystemEOS_OnlinePartyInterface_LeavePartySoloTwoPartiesWorks_Manager>(
                this,
                Instances[0],
                OnDone);
            OnInstanceCreated(Instance);
            Instance->Start();
        });
}

#endif // #if EOS_HAS_AUTHENTICATION
