// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "./TestPartyManager.h"
#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/HeapLambda.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlinePartyInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_MANAGED_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers,
    "OnlineSubsystemEOS.OnlinePartyInterface.PartyCountsAreCorrectWith4LocalUsers",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter,
    FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers_Manager);

class FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers_Manager
    : public TSharedFromThis<FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers_Manager>,
      public FSingleTestPartyManager
{
public:
    UE_NONCOPYABLE(FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers_Manager);
    virtual ~FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers_Manager() = default;

    typedef FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers_Manager TThisClass;

    class FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers *T;
    FMultiplayerScenarioInstance Host;
    FMultiplayerScenarioInstance Client1;
    FMultiplayerScenarioInstance Client2;
    FMultiplayerScenarioInstance Client3;
    TWeakPtr<IOnlinePartySystem, ESPMode::ThreadSafe> HostPartySystemWk;
    TWeakPtr<IOnlinePartySystem, ESPMode::ThreadSafe> Client1PartySystemWk;
    TWeakPtr<IOnlinePartySystem, ESPMode::ThreadSafe> Client2PartySystemWk;
    TWeakPtr<IOnlinePartySystem, ESPMode::ThreadSafe> Client3PartySystemWk;
    std::function<void()> OnDone;

    TSharedPtr<const FOnlinePartyId> ExpectedClientPartyId;

    TMap<int32, FDelegateHandle> ReceiveInviteHandle;
    TMap<int32, FUTickerDelegateHandle> InviteTimeoutHandle;
    TSet<int32> ClientsReceivedInvites;
    TSet<int32> ClientsJoinedParty;

    TSet<FString> SeenGlobalJoinEvents;

    void Start_CreatePartyOnHost();
    void Handle_CreatePartyOnHost(
        const FUniqueNetId &LocalUserId,
        const TSharedPtr<const FOnlinePartyId> &PartyId,
        const ECreatePartyCompletionResult Result);

    void Handle_GlobalJoinEvent(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &MemberId);

    void Start_SendInviteToClients();
    void Handle_SendInviteToClients(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &RecipientId,
        const ESendPartyInvitationCompletionResult Result,
        FMultiplayerScenarioInstance Client,
        int32 ClientNum);

    void Start_RestoreInvitesOnClient(FMultiplayerScenarioInstance InClient, int32 ClientNum);
    void Handle_RestoreInvitesOnClient(
        const FUniqueNetId &LocalUserId,
        const FOnlineError &Result,
        FMultiplayerScenarioInstance InClient,
        int32 ClientNum);

    bool HasClientReceivedInvite(int32 ClientNum) const
    {
        return this->ClientsReceivedInvites.Contains(ClientNum);
    }

    void Handle_ReceiveInviteOnClient(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &SenderId,
        FMultiplayerScenarioInstance InClient,
        int32 ClientNum);
    bool Handle_InviteTimeout(float DeltaTime);

    void Handle_JoinPartyOnClient(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const EJoinPartyCompletionResult Result,
        const int32 NotApprovedReason,
        FMultiplayerScenarioInstance Client,
        int32 ClientNum);

    void Start_HostVerifyPartyState();

    void Start();

    // NOLINTNEXTLINE(unreal-field-not-initialized-in-constructor)
    FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers_Manager(
        class FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers *InT,
        FMultiplayerScenarioInstance InHost,
        FMultiplayerScenarioInstance InClient1,
        FMultiplayerScenarioInstance InClient2,
        FMultiplayerScenarioInstance InClient3,
        std::function<void()> InOnDone)
    {
        this->T = InT;
        this->Host = MoveTemp(InHost);
        this->Client1 = MoveTemp(InClient1);
        this->Client2 = MoveTemp(InClient2);
        this->Client3 = MoveTemp(InClient3);
        this->OnDone = MoveTemp(InOnDone);

        this->HostPartySystemWk = this->Host.Subsystem.Pin()->GetPartyInterface();
        this->Client1PartySystemWk = this->Client1.Subsystem.Pin()->GetPartyInterface();
        this->Client2PartySystemWk = this->Client2.Subsystem.Pin()->GetPartyInterface();
        this->Client3PartySystemWk = this->Client3.Subsystem.Pin()->GetPartyInterface();
    }
};

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers_Manager::Start_CreatePartyOnHost()
{
    check(!this->HasParty(*this->Host.UserId));

    FOnlinePartyTypeId PartyTypeId = FOnlinePartyTypeId(1);

    FPartyConfiguration HostConfig;
    HostConfig.MaxMembers = 4;
    HostConfig.InvitePermissions = PartySystemPermissions::EPermissionType::Anyone;
    HostConfig.PresencePermissions = PartySystemPermissions::EPermissionType::Anyone;
    HostConfig.bIsAcceptingMembers = true;
    HostConfig.bChatEnabled = false;

    if (!T->TestTrue(
            "CreateParty operation started",
            this->HostPartySystemWk.Pin()->CreateParty(
                *this->Host.UserId,
                PartyTypeId,
                HostConfig,
                FOnCreatePartyComplete::CreateSP(this, &TThisClass::Handle_CreatePartyOnHost))))
    {
        UE_LOG(
            LogEOSTests,
            Warning,
            TEXT("[Diagnostics] CreateParty operation failed to start, so we're calling "
                 "CleanupPartiesAndThenCallOnDone"));
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers_Manager::Handle_CreatePartyOnHost(
    const FUniqueNetId &LocalUserId,
    const TSharedPtr<const FOnlinePartyId> &PartyId,
    const ECreatePartyCompletionResult Result)
{
    if (!T->TestEqual("Party was successfully created", Result, ECreatePartyCompletionResult::Succeeded))
    {
        UE_LOG(
            LogEOSTests,
            Warning,
            TEXT("[Diagnostics] CreateParty operation failed with error, so we're calling "
                 "CleanupPartiesAndThenCallOnDone"));
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    this->AddParty(this->Host, *PartyId);

    this->Start_SendInviteToClients();
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers_Manager::Handle_GlobalJoinEvent(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FUniqueNetId &MemberId)
{
    FString EventId =
        FString::Printf(TEXT("%s_%s_%s"), *LocalUserId.ToString(), *PartyId.ToString(), *MemberId.ToString());
    T->TestFalse(
        FString::Printf(TEXT("Event '%s' did not already occur"), *EventId),
        this->SeenGlobalJoinEvents.Contains(EventId));
    this->SeenGlobalJoinEvents.Add(EventId);
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers_Manager::Start_SendInviteToClients()
{
    TSharedPtr<const FOnlinePartyId> NotJoinedPartyId = this->Party(*this->Host.UserId);
    check(NotJoinedPartyId.IsValid());
    check(!this->ExpectedClientPartyId.IsValid());
    this->ExpectedClientPartyId = NotJoinedPartyId;

    UE_LOG(LogEOSTests, Log, TEXT("Sending invitation to client 1"));

    if (!T->TestTrue(
            FString::Printf(TEXT("SendInvitation operation started for %s"), *this->ExpectedClientPartyId->ToString()),
            this->HostPartySystemWk.Pin()->SendInvitation(
                *this->Host.UserId,
                *this->ExpectedClientPartyId,
                FPartyInvitationRecipient(this->Client1.UserId.ToSharedRef()),
                FOnSendPartyInvitationComplete::CreateSP(
                    this,
                    &TThisClass::Handle_SendInviteToClients,
                    this->Client1,
                    1))))
    {
        UE_LOG(
            LogEOSTests,
            Warning,
            TEXT("[Diagnostics] SendInvitation operation for client 1 failed to start, so we're calling "
                 "CleanupPartiesAndThenCallOnDone"));
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    UE_LOG(LogEOSTests, Log, TEXT("Sending invitation to client 2"));

    if (!T->TestTrue(
            FString::Printf(TEXT("SendInvitation operation started for %s"), *this->ExpectedClientPartyId->ToString()),
            this->HostPartySystemWk.Pin()->SendInvitation(
                *this->Host.UserId,
                *this->ExpectedClientPartyId,
                FPartyInvitationRecipient(this->Client2.UserId.ToSharedRef()),
                FOnSendPartyInvitationComplete::CreateSP(
                    this,
                    &TThisClass::Handle_SendInviteToClients,
                    this->Client2,
                    2))))
    {
        UE_LOG(
            LogEOSTests,
            Warning,
            TEXT("[Diagnostics] SendInvitation operation for client 2 failed to start, so we're calling "
                 "CleanupPartiesAndThenCallOnDone"));
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    UE_LOG(LogEOSTests, Log, TEXT("Sending invitation to client 3"));

    if (!T->TestTrue(
            FString::Printf(TEXT("SendInvitation operation started for %s"), *this->ExpectedClientPartyId->ToString()),
            this->HostPartySystemWk.Pin()->SendInvitation(
                *this->Host.UserId,
                *this->ExpectedClientPartyId,
                FPartyInvitationRecipient(this->Client3.UserId.ToSharedRef()),
                FOnSendPartyInvitationComplete::CreateSP(
                    this,
                    &TThisClass::Handle_SendInviteToClients,
                    this->Client3,
                    3))))
    {
        UE_LOG(
            LogEOSTests,
            Warning,
            TEXT("[Diagnostics] SendInvitation operation for client 3 failed to start, so we're calling "
                 "CleanupPartiesAndThenCallOnDone"));
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers_Manager::Handle_SendInviteToClients(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FUniqueNetId &RecipientId,
    const ESendPartyInvitationCompletionResult Result,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FMultiplayerScenarioInstance InClient,
    int32 ClientNum)
{
    T->TestTrue("Local user is sender", LocalUserId == *this->Host.UserId);
    T->TestTrue("Recipient matches", RecipientId == *InClient.UserId);

    if (!T->TestEqual(
            FString::Printf(TEXT("Invitation for %s was successfully sent"), *PartyId.ToString()),
            Result,
            ESendPartyInvitationCompletionResult::Succeeded))
    {
        UE_LOG(
            LogEOSTests,
            Warning,
            TEXT("[Diagnostics] SendInvitation operation failed with an error, so we're calling "
                 "CleanupPartiesAndThenCallOnDone"));
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    this->Start_RestoreInvitesOnClient(InClient, ClientNum);
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers_Manager::
    Start_RestoreInvitesOnClient(
        // NOLINTNEXTLINE(performance-unnecessary-value-param)
        FMultiplayerScenarioInstance InClient,
        int32 ClientNum)
{
    UE_LOG(LogEOSTests, Log, TEXT("[Diagnostics] Restoring invites on client %d"), ClientNum);

    InClient.Subsystem.Pin()->GetPartyInterface()->RestoreInvites(
        *InClient.UserId,
        FOnRestoreInvitesComplete::CreateSP(this, &TThisClass::Handle_RestoreInvitesOnClient, InClient, ClientNum));
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers_Manager::
    Handle_RestoreInvitesOnClient(
        const FUniqueNetId &LocalUserId,
        const FOnlineError &Result,
        // NOLINTNEXTLINE(performance-unnecessary-value-param)
        FMultiplayerScenarioInstance InClient,
        int32 ClientNum)
{
    if (!T->TestTrue(
            FString::Printf(TEXT("Restoring parties for client was successful: %s"), *Result.ToLogString()),
            Result.bSucceeded))
    {
        UE_LOG(
            LogEOSTests,
            Warning,
            TEXT("[Diagnostics] RestoreInvites operation failed with error '%s', so we're calling "
                 "CleanupPartiesAndThenCallOnDone"),
            *Result.ToLogString());
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    if (!this->HasClientReceivedInvite(ClientNum))
    {
        UE_LOG(LogEOSTests, Log, TEXT("[Diagnostics] Started an invite timeout for client %d"), ClientNum);
        this->InviteTimeoutHandle.Add(
            ClientNum,
            FUTicker::GetCoreTicker().AddTicker(
                FTickerDelegate::CreateSP(this, &TThisClass::Handle_InviteTimeout),
                30.0f));
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers_Manager::
    Handle_ReceiveInviteOnClient(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &SenderId,
        // NOLINTNEXTLINE(performance-unnecessary-value-param)
        FMultiplayerScenarioInstance InClient,
        int32 ClientNum)
{
    UE_LOG(
        LogEOSTests,
        Verbose,
        TEXT("Received invite on client: (LocalUser) %s (Party) %s (Sender) %s (ClientNum) %d"),
        *LocalUserId.ToString(),
        *PartyId.ToString(),
        *SenderId.ToString(),
        ClientNum);

    if (!this->ExpectedClientPartyId.IsValid() || this->ClientsReceivedInvites.Contains(ClientNum))
    {
        UE_LOG(
            LogEOSTests,
            Verbose,
            TEXT("Ignoring invite because the party ID was invalid or client already received an invite"));
        return;
    }
    if (PartyId != *this->ExpectedClientPartyId)
    {
        UE_LOG(LogEOSTests, Verbose, TEXT("Ignoring invite because party ID didn't match the expected party ID"));
        return;
    }
    if (LocalUserId != *InClient.UserId)
    {
        UE_LOG(LogEOSTests, Verbose, TEXT("Ignoring invite because this event is for a user other than our client"));
        return;
    }

    this->ClientsReceivedInvites.Add(ClientNum);

    this->AddParty(InClient, *this->ExpectedClientPartyId);
    if (this->InviteTimeoutHandle.Contains(ClientNum))
    {
        FUTicker::GetCoreTicker().RemoveTicker(this->InviteTimeoutHandle[ClientNum]);
        this->InviteTimeoutHandle.Remove(ClientNum);
    }

    UE_LOG(LogEOSTests, Log, TEXT("[Diagnostics] Retrieving invites on client %d"), ClientNum);

    TArray<IOnlinePartyJoinInfoConstRef> Invites;
    if (!T->TestTrue(
            "Can retrieve pending invites",
            InClient.Subsystem.Pin()->GetPartyInterface()->GetPendingInvites(*InClient.UserId, Invites)))
    {
        UE_LOG(
            LogEOSTests,
            Warning,
            TEXT("[Diagnostics] GetPendingInvites returned false, so we're calling "
                 "CleanupPartiesAndThenCallOnDone"));
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    UE_LOG(LogEOSTests, Log, TEXT("[Diagnostics] Processing invites on client %d"), ClientNum);

    T->TestTrue(
        TEXT("GetPendingInvites should return at least one invite, since we're in the event handler for receiving "
             "invites!"),
        Invites.Num() >= 1);

    // We may have more than one invite from previous tests. Reject any we don't care about
    // and use the one that we do.
    IOnlinePartyJoinInfoConstPtr DesiredInvite;
    for (const auto &JoinInfo : Invites)
    {
        UE_LOG(
            LogEOSTests,
            Verbose,
            TEXT("Client %d has invite for party ID %s"),
            ClientNum,
            *JoinInfo->GetPartyId()->ToString());

        if (*JoinInfo->GetPartyId() != *this->ExpectedClientPartyId)
        {
            if (!T->TestTrue(
                    "Can reject unwanted invitation",
                    InClient.Subsystem.Pin()->GetPartyInterface()->RejectInvitation(
                        *InClient.UserId,
                        *JoinInfo->GetSourceUserId())))
            {
                UE_LOG(
                    LogEOSTests,
                    Warning,
                    TEXT("[Diagnostics] RejectInvitation returned false, so we're calling "
                         "CleanupPartiesAndThenCallOnDone"));
                this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
                return;
            }
        }
        else
        {
            DesiredInvite = JoinInfo;
        }
    }

    if (!T->TestTrue("Got desired invite", DesiredInvite.IsValid()))
    {
        UE_LOG(
            LogEOSTests,
            Warning,
            TEXT("[Diagnostics] Did not get desired invite, so we're calling "
                 "CleanupPartiesAndThenCallOnDone"));
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    if (!T->TestTrue(
            "Can join party",
            InClient.Subsystem.Pin()->GetPartyInterface()->JoinParty(
                *InClient.UserId,
                *DesiredInvite,
                FOnJoinPartyComplete::CreateSP(this, &TThisClass::Handle_JoinPartyOnClient, InClient, ClientNum))))
    {
        UE_LOG(
            LogEOSTests,
            Warning,
            TEXT("[Diagnostics] JoinParty operation failed to start, so we're calling "
                 "CleanupPartiesAndThenCallOnDone"));
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }
}

bool FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers_Manager::Handle_InviteTimeout(
    float DeltaTime)
{
    UE_LOG(
        LogEOSTests,
        Warning,
        TEXT("[Diagnostics] Invitation timed out, so we're calling "
             "CleanupPartiesAndThenCallOnDone"));
    T->TestTrue("Invitation timed out", false);
    this->ExpectedClientPartyId.Reset();
    this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
    return false;
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers_Manager::Handle_JoinPartyOnClient(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const EJoinPartyCompletionResult Result,
    const int32 NotApprovedReason,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FMultiplayerScenarioInstance Client,
    int32 ClientNum)
{
    if (!T->TestEqual("Did join party", Result, EJoinPartyCompletionResult::Succeeded))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    TArray<TSharedRef<const FOnlinePartyId>> PartyIds;
    if (!T->TestTrue(
            "Can get parties list for user",
            Client.Subsystem.Pin()->GetPartyInterface()->GetJoinedParties(LocalUserId, PartyIds)))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    if (!T->TestEqual("Joined party count is 1", PartyIds.Num(), 1))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    this->ClientsJoinedParty.Add(ClientNum);

    if (this->ClientsJoinedParty.Contains(1) && this->ClientsJoinedParty.Contains(2) &&
        this->ClientsJoinedParty.Contains(3))
    {
        FUTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateLambda([this](float DeltaSeconds) {
                this->Start_HostVerifyPartyState();
                return false;
            }),
            5.0f);
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers_Manager::Start_HostVerifyPartyState()
{
    // Events are a little non-deterministic for the "host joined" event, so exclude testing for hosts. We really just
    // care how many times we saw clients join.
    TArray<FString> NonHostEvents;
    for (const auto &EventId : this->SeenGlobalJoinEvents)
    {
        if (EventId.StartsWith(this->Host.UserId->ToString()) && !EventId.EndsWith(this->Host.UserId->ToString()))
        {
            NonHostEvents.Add(EventId);
        }
    }

    T->TestEqual(TEXT("Saw 3 client join events"), NonHostEvents.Num(), 3);
    T->TestEqual(
        TEXT("GetPartyMemberCount returns 4"),
        this->HostPartySystemWk.Pin()->GetPartyMemberCount(*this->Host.UserId, *this->Party(*this->Host.UserId)),
        4);
    TArray<FOnlinePartyMemberConstRef> PartyMembers;
    T->TestTrue(
        TEXT("Can fetch party members"),
        this->HostPartySystemWk.Pin()
            ->GetPartyMembers(*this->Host.UserId, *this->Party(*this->Host.UserId), PartyMembers));
    T->TestEqual(TEXT("PartyMembers array has 4 entries"), PartyMembers.Num(), 4);

    this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers_Manager::Start()
{
    this->Client1PartySystemWk.Pin()->AddOnPartyInviteReceivedDelegate_Handle(
        FOnPartyInviteReceivedDelegate::CreateSP(this, &TThisClass::Handle_ReceiveInviteOnClient, this->Client1, 1));
    this->Client2PartySystemWk.Pin()->AddOnPartyInviteReceivedDelegate_Handle(
        FOnPartyInviteReceivedDelegate::CreateSP(this, &TThisClass::Handle_ReceiveInviteOnClient, this->Client2, 2));
    this->Client3PartySystemWk.Pin()->AddOnPartyInviteReceivedDelegate_Handle(
        FOnPartyInviteReceivedDelegate::CreateSP(this, &TThisClass::Handle_ReceiveInviteOnClient, this->Client3, 3));
    this->HostPartySystemWk.Pin()->AddOnPartyMemberJoinedDelegate_Handle(
        FOnPartyMemberJoinedDelegate::CreateSP(this, &TThisClass::Handle_GlobalJoinEvent));

    this->Start_CreatePartyOnHost();
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers::CreateManagingInstance(
    const FOnDone &OnDone,
    const std::function<
        void(const TSharedRef<FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers_Manager> &)>
        &OnInstanceCreated)
{
    if (MAX_LOCAL_PLAYERS < 4)
    {
        // We can't run this test, because this platform does not support multiple
        // local users. It's not an error.
        this->TestTrue(TEXT("Forcing test pass because this platform does not support multiple local users."), true);
        OnDone();
        return;
    }

    CreateUsersForTest_CreateOnDemand(
        this,
        4,
        OnDone,
        [this, OnInstanceCreated](
            const IOnlineSubsystemPtr &Subsystem,
            TArray<TSharedPtr<const FUniqueNetIdEOS>> UserIds,
            FOnDone OnDone) {
            FMultiplayerScenarioInstance Host;
            Host.Subsystem = Subsystem;
            Host.UserId = UserIds[0];

            FMultiplayerScenarioInstance Client1;
            Client1.Subsystem = Subsystem;
            Client1.UserId = UserIds[1];

            FMultiplayerScenarioInstance Client2;
            Client2.Subsystem = Subsystem;
            Client2.UserId = UserIds[2];

            FMultiplayerScenarioInstance Client3;
            Client3.Subsystem = Subsystem;
            Client3.UserId = UserIds[3];

            auto Instance =
                MakeShared<FOnlineSubsystemEOS_OnlinePartyInterface_PartyCountsAreCorrectWith4LocalUsers_Manager>(
                    this,
                    Host,
                    Client1,
                    Client2,
                    Client3,
                    OnDone);
            OnInstanceCreated(Instance);
            Instance->Start();
        });
}

#endif // #if EOS_HAS_AUTHENTICATION
