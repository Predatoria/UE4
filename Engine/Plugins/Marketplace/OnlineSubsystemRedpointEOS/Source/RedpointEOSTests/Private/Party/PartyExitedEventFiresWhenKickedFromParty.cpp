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
    FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty,
    "OnlineSubsystemEOS.OnlinePartyInterface.PartyExitedEventFiresWhenKickedFromParty",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter,
    FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty_Manager);

class FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty_Manager
    : public TSharedFromThis<FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty_Manager>,
      public FSingleTestPartyManager
{
public:
    UE_NONCOPYABLE(FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty_Manager);
    virtual ~FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty_Manager() = default;

    typedef FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty_Manager TThisClass;

    class FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty *T;
    FMultiplayerScenarioInstance Host;
    FMultiplayerScenarioInstance Client;
    TWeakPtr<IOnlinePartySystem, ESPMode::ThreadSafe> HostPartySystemWk;
    TWeakPtr<IOnlinePartySystem, ESPMode::ThreadSafe> ClientPartySystemWk;
    std::function<void()> OnDone;

    FDelegateHandle PartyMemberJoinedHandle;
    FDelegateHandle ReceiveInviteHandle;
    FUTickerDelegateHandle InviteTimeoutHandle;
    FDelegateHandle PartyExitedHandle;
    FUTickerDelegateHandle KickTimeoutHandle;

    void Start_CreatePartyOnHost();
    void Handle_CreatePartyOnHost(
        const FUniqueNetId &LocalUserId,
        const TSharedPtr<const FOnlinePartyId> &PartyId,
        const ECreatePartyCompletionResult Result);

    void Start_SendInviteToClient();
    void Handle_SendInviteToClient(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &RecipientId,
        const ESendPartyInvitationCompletionResult Result);

    void Start_RestoreInvitesOnClient();
    void Handle_RestoreInvitesOnClient(const FUniqueNetId &LocalUserId, const FOnlineError &Result);

    bool HasClientReceivedInvite() const
    {
        if (this->HasParty(*this->Client.UserId))
        {
            return true;
        }
        return false;
    }

    void Handle_ReceiveInviteOnClient(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &SenderId);
    bool Handle_InviteTimeout(float DeltaTime);

    void Handle_JoinParty(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const EJoinPartyCompletionResult Result,
        const int32 NotApprovedReason);
    void Handle_MemberJoined(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &MemberId);
    bool Start_KickMember(
        float DeltaTime,
        TSharedRef<const FUniqueNetId> LocalUserId,
        TSharedRef<const FOnlinePartyId> PartyId,
        TSharedRef<const FUniqueNetId> MemberId);
    void Handle_PartyExited(const FUniqueNetId &LocalUserId, const FOnlinePartyId &PartyId);
    void Handle_KickPartyMemberComplete(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &MemberId,
        const EKickMemberCompletionResult Result);
    bool Handle_KickTimeout(float DeltaTime);

    void Start();

    // NOLINTNEXTLINE(unreal-field-not-initialized-in-constructor)
    FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty_Manager(
        class FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty *InT,
        FMultiplayerScenarioInstance InHost,
        FMultiplayerScenarioInstance InClient,
        std::function<void()> InOnDone)
    {
        this->T = InT;
        this->Host = MoveTemp(InHost);
        this->Client = MoveTemp(InClient);
        this->OnDone = MoveTemp(InOnDone);

        this->HostPartySystemWk = this->Host.Subsystem.Pin()->GetPartyInterface();
        this->ClientPartySystemWk = this->Client.Subsystem.Pin()->GetPartyInterface();
    }
};

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty_Manager::
    Start_CreatePartyOnHost()
{
    FOnlinePartyTypeId PartyTypeId = FOnlinePartyTypeId(1);

    FPartyConfiguration HostConfig;
    HostConfig.MaxMembers = 4;
    HostConfig.InvitePermissions = PartySystemPermissions::EPermissionType::Anyone;
    HostConfig.PresencePermissions = PartySystemPermissions::EPermissionType::Anyone;
    HostConfig.bIsAcceptingMembers = true;
    HostConfig.bChatEnabled = false;

    UE_LOG(LogEOSTests, Log, TEXT("Creating party 1"));
    if (!T->TestTrue(
            TEXT("CreateParty operation started"),
            this->HostPartySystemWk.Pin()->CreateParty(
                *this->Host.UserId,
                PartyTypeId,
                HostConfig,
                FOnCreatePartyComplete::CreateSP(this, &TThisClass::Handle_CreatePartyOnHost))))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty_Manager::
    Handle_CreatePartyOnHost(
        const FUniqueNetId &LocalUserId,
        const TSharedPtr<const FOnlinePartyId> &PartyId,
        const ECreatePartyCompletionResult Result)
{
    if (!T->TestEqual(TEXT("Party was successfully created"), Result, ECreatePartyCompletionResult::Succeeded))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    this->AddParty(this->Host, *PartyId);

    this->Start_SendInviteToClient();
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty_Manager::
    Start_SendInviteToClient()
{
    UE_LOG(LogEOSTests, Log, TEXT("Registering event to handle member join"));
    this->PartyMemberJoinedHandle = this->HostPartySystemWk.Pin()->AddOnPartyMemberJoinedDelegate_Handle(
        FOnPartyMemberJoinedDelegate::CreateSP(this, &TThisClass::Handle_MemberJoined));

    UE_LOG(LogEOSTests, Log, TEXT("Sending invitation"));
    if (!T->TestTrue(
            TEXT("SendInvitation operation started"),
            this->HostPartySystemWk.Pin()->SendInvitation(
                *this->Host.UserId,
                *this->Party(*this->Host.UserId),
                FPartyInvitationRecipient(this->Client.UserId.ToSharedRef()),
                FOnSendPartyInvitationComplete::CreateSP(this, &TThisClass::Handle_SendInviteToClient))))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty_Manager::
    Handle_SendInviteToClient(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &RecipientId,
        const ESendPartyInvitationCompletionResult Result)
{
    T->TestTrue("Local user is sender", LocalUserId == *this->Host.UserId);
    T->TestTrue("Recipient matches", RecipientId == *Client.UserId);

    if (!T->TestEqual(
            FString::Printf(TEXT("Invitation for %s was successfully sent"), *PartyId.ToString()),
            Result,
            ESendPartyInvitationCompletionResult::Succeeded))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    this->Start_RestoreInvitesOnClient();
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty_Manager::
    Start_RestoreInvitesOnClient()
{
    this->ClientPartySystemWk.Pin()->RestoreInvites(
        *this->Client.UserId,
        FOnRestoreInvitesComplete::CreateSP(this, &TThisClass::Handle_RestoreInvitesOnClient));
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty_Manager::
    Handle_RestoreInvitesOnClient(const FUniqueNetId &LocalUserId, const FOnlineError &Result)
{
    if (!T->TestTrue(TEXT("Restoring parties for client was successful"), Result.bSucceeded))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    if (!this->HasClientReceivedInvite())
    {
        this->InviteTimeoutHandle = FUTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateSP(this, &TThisClass::Handle_InviteTimeout),
            30.0f);
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty_Manager::
    Handle_ReceiveInviteOnClient(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &SenderId)
{
    if (!this->HasParty(*this->Host.UserId))
    {
        return;
    }
    if (PartyId != *this->Party(*this->Host.UserId) || this->HasParty(*this->Client.UserId))
    {
        return;
    }

    this->AddParty(this->Client, PartyId);

    if (this->InviteTimeoutHandle.IsValid())
    {
        FUTicker::GetCoreTicker().RemoveTicker(this->InviteTimeoutHandle);
        this->InviteTimeoutHandle.Reset();
    }

    TArray<IOnlinePartyJoinInfoConstRef> Invites;
    if (!T->TestTrue(
            "Can retrieve pending invites",
            this->ClientPartySystemWk.Pin()->GetPendingInvites(*this->Client.UserId, Invites)))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    // We may have more than one invite from previous tests. Reject any we don't care about
    // and use the one that we do.
    IOnlinePartyJoinInfoConstPtr DesiredInvite;
    for (const auto &JoinInfo : Invites)
    {
        if (*JoinInfo->GetPartyId() != *this->Party(*this->Client.UserId))
        {
            if (!T->TestTrue(
                    "Can reject unwanted invitation",
                    this->ClientPartySystemWk.Pin()->RejectInvitation(
                        *this->Client.UserId,
                        *JoinInfo->GetSourceUserId())))
            {
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
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    if (!T->TestTrue(
            "Can join party",
            this->ClientPartySystemWk.Pin()->JoinParty(
                *this->Client.UserId,
                *DesiredInvite,
                FOnJoinPartyComplete::CreateSP(this, &TThisClass::Handle_JoinParty))))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }
}

bool FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty_Manager::Handle_InviteTimeout(
    float DeltaTime)
{
    T->TestTrue("Invitation timed out", false);
    this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
    return false;
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty_Manager::Handle_JoinParty(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const EJoinPartyCompletionResult Result,
    const int32 NotApprovedReason)
{
    if (!T->TestEqual("Did join party", Result, EJoinPartyCompletionResult::Succeeded))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    // The host will receive the Handle_MemberJoined callback, and it will proceed from there.
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty_Manager::Handle_MemberJoined(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FUniqueNetId &MemberId)
{
    if (LocalUserId == *this->Host.UserId && PartyId == *this->Party(*this->Host.UserId) &&
        MemberId == *this->Client.UserId)
    {
        // We must wait a small amount of time for the EOS plugin's handler of JoinParty on the client side to complete
        // (it's technically possible to kick the member between JoinParty completing successfully and the handler
        // calling EOS_Lobby_CopyLobbyDetailsHandle).
        FUTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateSP(
                this,
                &TThisClass::Start_KickMember,
                LocalUserId.AsShared(),
                PartyId.AsShared(),
                MemberId.AsShared()),
            1.0f);
    }
}

bool FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty_Manager::Start_KickMember(
    float DeltaTime,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<const FUniqueNetId> LocalUserId,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<const FOnlinePartyId> PartyId,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<const FUniqueNetId> MemberId)
{
    // This is the join event we're interested in.
    auto HostPartySystem = this->HostPartySystemWk.Pin();
    auto ClientPartySystem = this->ClientPartySystemWk.Pin();

    HostPartySystem->ClearOnPartyMemberJoinedDelegate_Handle(this->PartyMemberJoinedHandle);
    this->PartyMemberJoinedHandle.Reset();

    this->PartyExitedHandle = ClientPartySystem->AddOnPartyExitedDelegate_Handle(
        FOnPartyExitedDelegate::CreateSP(this, &TThisClass::Handle_PartyExited));

    HostPartySystem->KickMember(
        *LocalUserId,
        *PartyId,
        *MemberId,
        FOnKickPartyMemberComplete::CreateSP(this, &TThisClass::Handle_KickPartyMemberComplete));

    return false;
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty_Manager::Handle_PartyExited(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId)
{
    T->TestTrue("Correct local user ID on exit", LocalUserId == *this->Client.UserId);
    T->TestTrue("Exited expected party", PartyId == *this->Party(*this->Client.UserId));

    this->ClientPartySystemWk.Pin()->ClearOnPartyExitedDelegate_Handle(this->PartyExitedHandle);
    FUTicker::GetCoreTicker().RemoveTicker(this->KickTimeoutHandle);

    this->RemoveParty(this->Client, PartyId);

    this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty_Manager::
    Handle_KickPartyMemberComplete(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &MemberId,
        const EKickMemberCompletionResult Result)
{
    if (!T->TestEqual("Did kick member successfully", Result, EKickMemberCompletionResult::Succeeded))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    this->KickTimeoutHandle =
        FUTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateSP(this, &TThisClass::Handle_KickTimeout), 5.0f);
}

bool FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty_Manager::Handle_KickTimeout(
    float DeltaTime)
{
    T->TestTrue("PartyExited event timed out", false);
    this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
    return false;
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty_Manager::Start()
{
    this->ClientPartySystemWk.Pin()->AddOnPartyInviteReceivedDelegate_Handle(
        FOnPartyInviteReceivedDelegate::CreateSP(this, &TThisClass::Handle_ReceiveInviteOnClient));

    this->Start_CreatePartyOnHost();
}

void FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty::CreateManagingInstance(
    const FOnDone &OnDone,
    const std::function<void(
        const TSharedRef<FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty_Manager> &)>
        &OnInstanceCreated)
{
    CreateSubsystemsForTest_CreateOnDemand(
        this,
        2,
        OnDone,
        [this, OnInstanceCreated](TArray<FMultiplayerScenarioInstance> Instances, const FOnDone &OnDone) {
            auto Instance =
                MakeShared<FOnlineSubsystemEOS_OnlinePartyInterface_PartyExitedEventFiresWhenKickedFromParty_Manager>(
                    this,
                    Instances[0],
                    Instances[1],
                    OnDone);
            OnInstanceCreated(Instance);
            Instance->Start();
        });
}

#endif // #if EOS_HAS_AUTHENTICATION
