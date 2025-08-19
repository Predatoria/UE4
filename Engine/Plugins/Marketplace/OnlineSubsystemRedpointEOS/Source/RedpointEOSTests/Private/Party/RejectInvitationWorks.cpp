// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

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

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlinePartyInterface_RejectInvitationWorks,
    "OnlineSubsystemEOS.OnlinePartyInterface.RejectInvitationWorks",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_OnlinePartyInterface_RejectInvitationWorks::RunAsyncTest(const std::function<void()> &OnDone)
{
    CreateSubsystemsForTest_CreateOnDemand(
        this,
        2,
        OnDone,
        [this](const TArray<FMultiplayerScenarioInstance> &Instances, const FOnDone &OnDone) {
            FMultiplayerScenarioInstance Host = Instances[0];
            FMultiplayerScenarioInstance Client = Instances[1];

            auto PartyHost = Host.Subsystem.Pin()->GetPartyInterface();
            auto PartyClient = Client.Subsystem.Pin()->GetPartyInterface();
            TestTrue("Party interface exists for host", PartyHost.IsValid());
            TestTrue("Party interface exists for client", PartyClient.IsValid());

            if (!PartyHost.IsValid() || !PartyClient.IsValid())
            {
                OnDone();
                return;
            }

            bool *bDidReceiveInvite = new bool(false);
            FHeapLambda<EHeapLambdaFlags::OneShotCleanup> OnDoneAndCleanup;
            FHeapLambda<EHeapLambdaFlags::OneShot> CleanupTimer;

            FDelegateHandle PartyInviteHandle =
                PartyClient->AddOnPartyInviteReceivedDelegate_Handle(FOnPartyInviteReceivedDelegate::CreateLambda(
                    [this, Client, Host, bDidReceiveInvite, OnDoneAndCleanup, PartyClientWk = GetWeakThis(PartyClient)](
                        const FUniqueNetId &LocalUserId,
                        const FOnlinePartyId &PartyId,
                        const FUniqueNetId &SenderId) {
                        auto PartyClient = PinWeakThis(PartyClientWk);
                        if (TestTrue("Party client valid", PartyClient.IsValid()))
                        {
                            TestTrue("Local user is client", LocalUserId == *Client.UserId);
                            TestTrue("Sending user is host", SenderId == *Host.UserId);
                            *bDidReceiveInvite = true;

                            // Check that we have the invite in our party system.
                            TArray<IOnlinePartyJoinInfoConstRef> Invites;
                            TestTrue("Can get pending invites", PartyClient->GetPendingInvites(LocalUserId, Invites));
                            TestEqual("Invite is present in party system after receiving it", Invites.Num(), 1);
                            TestTrue("Rejection was successful", PartyClient->RejectInvitation(LocalUserId, SenderId));
                            TestTrue("Can get pending invites", PartyClient->GetPendingInvites(LocalUserId, Invites));
                            TestEqual("Invite is not present in party system after rejecting it", Invites.Num(), 0);
                        }

                        OnDoneAndCleanup();
                    }));

            OnDoneAndCleanup = [this,
                                PartyClientWk = GetWeakThis(PartyClient),
                                PartyInviteHandle,
                                bDidReceiveInvite,
                                CleanupTimer,
                                OnDone]() {
                FDelegateHandle HandleCopy = PartyInviteHandle;
                if (auto PartyClient = PinWeakThis(PartyClientWk))
                {
                    PartyClient->ClearOnPartyInviteReceivedDelegate_Handle(HandleCopy);
                }
                TestTrue("Should receive invite notification", *bDidReceiveInvite);
                delete bDidReceiveInvite;
                UE_LOG(LogEOSTests, Log, TEXT("Clearing timeout for receiver"));
                CleanupTimer();
                UE_LOG(LogEOSTests, Log, TEXT("Calling OnDone"));
                OnDone();
            };

            FPartyConfiguration HostConfig;
            HostConfig.MaxMembers = 4;
            HostConfig.InvitePermissions = PartySystemPermissions::EPermissionType::Anyone;
            HostConfig.PresencePermissions = PartySystemPermissions::EPermissionType::Anyone;
            HostConfig.bIsAcceptingMembers = true;
            HostConfig.bChatEnabled = false;

            UE_LOG(LogEOSTests, Log, TEXT("Creating party"));
            if (!TestTrue(
                    "CreateParty operation started",
                    PartyHost->CreateParty(
                        *Host.UserId,
                        IOnlinePartySystem::GetPrimaryPartyTypeId(),
                        HostConfig,
                        FOnCreatePartyComplete::CreateLambda([this,
                                                              Host,
                                                              Client,
                                                              OnDoneAndCleanup,
                                                              CleanupTimer,
                                                              PartyHostWk = GetWeakThis(PartyHost),
                                                              PartyClientWk = GetWeakThis(PartyClient)](
                                                                 const FUniqueNetId &LocalUserId,
                                                                 const TSharedPtr<const FOnlinePartyId> &PartyId,
                                                                 const ECreatePartyCompletionResult Result) {
                            auto PartyHost = PinWeakThis(PartyHostWk);
                            auto PartyClient = PinWeakThis(PartyClientWk);
                            if (PartyHost && PartyClient)
                            {
                                if (!TestEqual("Party was created", Result, ECreatePartyCompletionResult::Succeeded))
                                {
                                    OnDoneAndCleanup();
                                    return;
                                }

                                UE_LOG(LogEOSTests, Log, TEXT("Created party: %s"), *PartyId->ToString());

                                UE_LOG(LogEOSTests, Log, TEXT("Sending invitation"));
                                if (!TestTrue(
                                        "SendInvitation operation started",
                                        PartyHost->SendInvitation(
                                            *Host.UserId,
                                            *PartyId,
                                            FPartyInvitationRecipient(Client.UserId.ToSharedRef()),
                                            FOnSendPartyInvitationComplete::CreateLambda(
                                                [this,
                                                 Host,
                                                 Client,
                                                 OnDoneAndCleanup,
                                                 CleanupTimer,
                                                 PartyClientWk = GetWeakThis(PartyClient),
                                                 OrigPartyId = PartyId](
                                                    const FUniqueNetId &LocalUserId,
                                                    const FOnlinePartyId &PartyId,
                                                    const FUniqueNetId &RecipientId,
                                                    const ESendPartyInvitationCompletionResult Result) {
                                                    TestTrue("Local user is sender", LocalUserId == *Host.UserId);
                                                    TestTrue("Party matches", PartyId == *OrigPartyId);
                                                    TestTrue("Recipient matches", RecipientId == *Client.UserId);
                                                    if (!TestEqual(
                                                            "Result was successful",
                                                            Result,
                                                            ESendPartyInvitationCompletionResult::Succeeded))
                                                    {
                                                        OnDoneAndCleanup();
                                                        return;
                                                    }

                                                    UE_LOG(LogEOSTests, Log, TEXT("Sent invitation"));

                                                    auto PartyClient = PinWeakThis(PartyClientWk);
                                                    if (TestTrue("Party client pointer valid", PartyClient.IsValid()))
                                                    {
                                                        UE_LOG(LogEOSTests, Log, TEXT("Restoring invites"));

                                                        // Make sure we explicitly query for missing invites.
                                                        // Because the register listener, create, send happens so
                                                        // quickly in tests, occasionally the listener hasn't fully
                                                        // registered with the backend and misses the invite when
                                                        // it's sent.
                                                        PartyClient->RestoreInvites(
                                                            *Client.UserId,
                                                            FOnRestoreInvitesComplete::CreateLambda(
                                                                [this, OnDoneAndCleanup, CleanupTimer](
                                                                    const FUniqueNetId &LocalUserId,
                                                                    const FOnlineError &Result) {
                                                                    this->TestTrue(
                                                                        "Restore invites was successful",
                                                                        Result.bSucceeded);

                                                                    UE_LOG(LogEOSTests, Log, TEXT("Restored invites"));

                                                                    UE_LOG(
                                                                        LogEOSTests,
                                                                        Log,
                                                                        TEXT("Starting timeout for receiver"));
                                                                    FUTickerDelegateHandle CancelTimer =
                                                                        FUTicker::GetCoreTicker().AddTicker(
                                                                            FTickerDelegate::CreateLambda(
                                                                                [OnDoneAndCleanup](float DeltaTime) {
                                                                                    // This will check if the
                                                                                    // invitation was received.
                                                                                    OnDoneAndCleanup();
                                                                                    return false;
                                                                                }),
                                                                            30.0f);
                                                                    UE_LOG(
                                                                        LogEOSTests,
                                                                        Log,
                                                                        TEXT("Started timeout for receiver"));
                                                                    CleanupTimer.Assign([CancelTimer]() {
                                                                        FUTicker::GetCoreTicker().RemoveTicker(
                                                                            CancelTimer);
                                                                        UE_LOG(
                                                                            LogEOSTests,
                                                                            Log,
                                                                            TEXT("Cancelled timeout for receiver"));
                                                                    });
                                                                }));
                                                    }
                                                }))))
                                {
                                    OnDoneAndCleanup();
                                };
                            }
                        }))))
            {
                OnDoneAndCleanup();
            }
        });
}

#endif // #if EOS_HAS_AUTHENTICATION
