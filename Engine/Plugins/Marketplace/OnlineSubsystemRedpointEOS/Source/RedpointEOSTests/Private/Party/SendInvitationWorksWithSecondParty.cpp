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
    FOnlineSubsystemEOS_OnlinePartyInterface_SendInvitationWorksWithSecondParty,
    "OnlineSubsystemEOS.OnlinePartyInterface.SendInvitationWorksWithSecondParty",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter,
    FOnlineSubsystemEOS_OnlinePartyInterface_SendInvitationWorksWithSecondParty_Manager);

class FOnlineSubsystemEOS_OnlinePartyInterface_SendInvitationWorksWithSecondParty_Manager
    : public TSharedFromThis<FOnlineSubsystemEOS_OnlinePartyInterface_SendInvitationWorksWithSecondParty_Manager>,
      public FMultiTestPartyManager
{
public:
    UE_NONCOPYABLE(FOnlineSubsystemEOS_OnlinePartyInterface_SendInvitationWorksWithSecondParty_Manager);
    virtual ~FOnlineSubsystemEOS_OnlinePartyInterface_SendInvitationWorksWithSecondParty_Manager() = default;

    typedef FOnlineSubsystemEOS_OnlinePartyInterface_SendInvitationWorksWithSecondParty_Manager TThisClass;

    class FOnlineSubsystemEOS_OnlinePartyInterface_SendInvitationWorksWithSecondParty *T;
    FMultiplayerScenarioInstance Host;
    FMultiplayerScenarioInstance Client;
    TWeakPtr<IOnlinePartySystem, ESPMode::ThreadSafe> HostPartySystemWk;
    TWeakPtr<IOnlinePartySystem, ESPMode::ThreadSafe> ClientPartySystemWk;
    std::function<void()> OnDone;

    TSharedPtr<const FOnlinePartyId> ExpectedClientPartyId;

    FDelegateHandle ReceiveInviteHandle;
    FUTickerDelegateHandle InviteTimeoutHandle;

    void Start_CreatePartyOnHost();
    void Handle_CreatePartyOnHost(
        const FUniqueNetId &LocalUserId,
        const TSharedPtr<const FOnlinePartyId> &PartyId,
        const ECreatePartyCompletionResult Result,
        int PartyNumericId);

    void Start_SendInviteToClient();
    void Handle_SendInviteToClient(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &RecipientId,
        const ESendPartyInvitationCompletionResult Result);

    void Start_RestoreInvitesOnClient();
    void Handle_RestoreInvitesOnClient(const FUniqueNetId &LocalUserId, const FOnlineError &Result);

    void Handle_JoinParty(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const EJoinPartyCompletionResult Result,
        const int32 NotApprovedReason);

    bool HasClientReceivedInvite() const
    {
        if (!this->ExpectedClientPartyId.IsValid())
        {
            return true;
        }
        for (const auto &ClientParty : this->Parties(*this->Client.UserId))
        {
            if (*ClientParty == *this->ExpectedClientPartyId)
            {
                return true;
            }
        }
        return false;
    }

    void Handle_ReceiveInviteOnClient(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &SenderId);
    bool Handle_InviteTimeout(float DeltaTime);

    void Start();

    // NOLINTNEXTLINE(unreal-field-not-initialized-in-constructor)
    FOnlineSubsystemEOS_OnlinePartyInterface_SendInvitationWorksWithSecondParty_Manager(
        class FOnlineSubsystemEOS_OnlinePartyInterface_SendInvitationWorksWithSecondParty *InT,
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

void FOnlineSubsystemEOS_OnlinePartyInterface_SendInvitationWorksWithSecondParty_Manager::Start_CreatePartyOnHost()
{
    check(this->PartyNum(*this->Host.UserId) < 2);

    int NextPartyId = this->PartyNum(*this->Host.UserId) + 1;
    FOnlinePartyTypeId PartyTypeId = FOnlinePartyTypeId(NextPartyId);

    FPartyConfiguration HostConfig;
    HostConfig.MaxMembers = 4;
    HostConfig.InvitePermissions = PartySystemPermissions::EPermissionType::Anyone;
    HostConfig.PresencePermissions = PartySystemPermissions::EPermissionType::Anyone;
    HostConfig.bIsAcceptingMembers = true;
    HostConfig.bChatEnabled = false;

    UE_LOG(LogEOSTests, Log, TEXT("Creating party 1"));
    if (!T->TestTrue(
            FString::Printf(TEXT("CreateParty %d operation started"), NextPartyId),
            this->HostPartySystemWk.Pin()->CreateParty(
                *this->Host.UserId,
                PartyTypeId,
                HostConfig,
                FOnCreatePartyComplete::CreateSP(this, &TThisClass::Handle_CreatePartyOnHost, NextPartyId))))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_SendInvitationWorksWithSecondParty_Manager::Handle_CreatePartyOnHost(
    const FUniqueNetId &LocalUserId,
    const TSharedPtr<const FOnlinePartyId> &PartyId,
    const ECreatePartyCompletionResult Result,
    int PartyNumericId)
{
    if (!T->TestEqual(
            FString::Printf(TEXT("Party %d was successfully created"), PartyNumericId),
            Result,
            ECreatePartyCompletionResult::Succeeded))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    this->AddParty(this->Host, *PartyId);

    if (this->PartyNum(*this->Host.UserId) == 1)
    {
        // Create the second party.
        this->Start_CreatePartyOnHost();
    }
    else if (this->PartyNum(*this->Host.UserId) == 2)
    {
        // Send the first invite.
        this->Start_SendInviteToClient();
    }
    else
    {
        check(false /* Bug! */);
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_SendInvitationWorksWithSecondParty_Manager::Start_SendInviteToClient()
{
    TSharedPtr<const FOnlinePartyId> NotJoinedPartyId;
    for (const auto &HostParty : this->Parties(*this->Host.UserId))
    {
        bool bAvailable = true;
        for (const auto &ClientParty : this->Parties(*this->Client.UserId))
        {
            if (*ClientParty == *HostParty)
            {
                bAvailable = false;
                break;
            }
        }
        if (bAvailable)
        {
            NotJoinedPartyId = HostParty;
            break;
        }
    }
    check(NotJoinedPartyId.IsValid());

    check(!this->ExpectedClientPartyId.IsValid());
    this->ExpectedClientPartyId = NotJoinedPartyId;

    UE_LOG(LogEOSTests, Log, TEXT("Sending invitation"));
    if (!T->TestTrue(
            FString::Printf(TEXT("SendInvitation operation started for %s"), *NotJoinedPartyId->ToString()),
            this->HostPartySystemWk.Pin()->SendInvitation(
                *this->Host.UserId,
                *NotJoinedPartyId,
                FPartyInvitationRecipient(this->Client.UserId.ToSharedRef()),
                FOnSendPartyInvitationComplete::CreateSP(this, &TThisClass::Handle_SendInviteToClient))))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_SendInvitationWorksWithSecondParty_Manager::Handle_SendInviteToClient(
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

void FOnlineSubsystemEOS_OnlinePartyInterface_SendInvitationWorksWithSecondParty_Manager::Start_RestoreInvitesOnClient()
{
    this->ClientPartySystemWk.Pin()->RestoreInvites(
        *this->Client.UserId,
        FOnRestoreInvitesComplete::CreateSP(this, &TThisClass::Handle_RestoreInvitesOnClient));
}

void FOnlineSubsystemEOS_OnlinePartyInterface_SendInvitationWorksWithSecondParty_Manager::Handle_RestoreInvitesOnClient(
    const FUniqueNetId &LocalUserId,
    const FOnlineError &Result)
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

void FOnlineSubsystemEOS_OnlinePartyInterface_SendInvitationWorksWithSecondParty_Manager::Handle_ReceiveInviteOnClient(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FUniqueNetId &SenderId)
{
    if (!this->ExpectedClientPartyId.IsValid())
    {
        return;
    }
    if (PartyId != *this->ExpectedClientPartyId)
    {
        return;
    }

    this->AddParty(this->Client, *this->ExpectedClientPartyId);
    if (this->InviteTimeoutHandle.IsValid())
    {
        FUTicker::GetCoreTicker().RemoveTicker(this->InviteTimeoutHandle);
        this->InviteTimeoutHandle.Reset();
    }
    auto PartyToJoin = this->ExpectedClientPartyId;
    this->ExpectedClientPartyId.Reset();

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
        if (*JoinInfo->GetPartyId() != *PartyToJoin)
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

void FOnlineSubsystemEOS_OnlinePartyInterface_SendInvitationWorksWithSecondParty_Manager::Handle_JoinParty(
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

    if (this->PartyNum(*this->Client.UserId) == 2)
    {
        // Test complete.
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
    }
    else if (this->PartyNum(*this->Client.UserId) == 1)
    {
        // Need to send the next invitation.
        this->Start_SendInviteToClient();
    }
    else
    {
        check(false /* Bug */);
    }
}

bool FOnlineSubsystemEOS_OnlinePartyInterface_SendInvitationWorksWithSecondParty_Manager::Handle_InviteTimeout(
    float DeltaTime)
{
    T->TestTrue("Invitation timed out", false);
    this->ExpectedClientPartyId.Reset();
    this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
    return false;
}

void FOnlineSubsystemEOS_OnlinePartyInterface_SendInvitationWorksWithSecondParty_Manager::Start()
{
    this->ClientPartySystemWk.Pin()->AddOnPartyInviteReceivedDelegate_Handle(
        FOnPartyInviteReceivedDelegate::CreateSP(this, &TThisClass::Handle_ReceiveInviteOnClient));

    this->Start_CreatePartyOnHost();
}

void FOnlineSubsystemEOS_OnlinePartyInterface_SendInvitationWorksWithSecondParty::CreateManagingInstance(
    const FOnDone &OnDone,
    const std::function<
        void(const TSharedRef<FOnlineSubsystemEOS_OnlinePartyInterface_SendInvitationWorksWithSecondParty_Manager> &)>
        &OnInstanceCreated)
{
    CreateSubsystemsForTest_CreateOnDemand(
        this,
        2,
        OnDone,
        [this, OnInstanceCreated](TArray<FMultiplayerScenarioInstance> Instances, const FOnDone &OnDone) {
            auto Instance =
                MakeShared<FOnlineSubsystemEOS_OnlinePartyInterface_SendInvitationWorksWithSecondParty_Manager>(
                    this,
                    Instances[0],
                    Instances[1],
                    OnDone);
            OnInstanceCreated(Instance);
            Instance->Start();
        });
}

#endif // #if EOS_HAS_AUTHENTICATION
