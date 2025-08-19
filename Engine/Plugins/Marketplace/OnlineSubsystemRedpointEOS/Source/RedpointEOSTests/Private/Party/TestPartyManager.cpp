// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "TestPartyManager.h"

#include "OnlineSubsystemRedpointEOS/Shared/OnlinePartyInterfaceEOS.h"

void FTestPartyManager::AddParty(const FMultiplayerScenarioInstance &InInstance, const FOnlinePartyId &InPartyId)
{
    if (!this->Instances.Contains(*InInstance.UserId))
    {
        this->Instances.Add(*InInstance.UserId, InInstance);
    }

    if (!this->ConnectedPartiesByUser.Contains(*InInstance.UserId))
    {
        this->ConnectedPartiesByUser.Add(*InInstance.UserId, TArray<TSharedPtr<const FOnlinePartyId>>());
    }

    this->ConnectedPartiesByUser[*InInstance.UserId].Add(InPartyId.AsShared());
}

void FTestPartyManager::RemoveParty(const FMultiplayerScenarioInstance &InInstance, const FOnlinePartyId &InPartyId)
{
    for (int i = this->ConnectedPartiesByUser[*InInstance.UserId].Num() - 1; i >= 0; i--)
    {
        if (*this->ConnectedPartiesByUser[*InInstance.UserId][i] == InPartyId)
        {
            this->ConnectedPartiesByUser[*InInstance.UserId].RemoveAt(i);
        }
    }
}

void FTestPartyManager::Handle_OnLeavePartyComplete(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const ELeavePartyCompletionResult Result,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<FTestPartyManager> ThisRef,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    std::function<void()> OnDone)
{
    if (Result != ELeavePartyCompletionResult::Succeeded)
    {
        UE_LOG(
            LogEOSTests,
            Warning,
            TEXT("Automatically party leave operation failed (this may cause other tests to fail)"));
    }

    for (int i = this->ConnectedPartiesByUser[LocalUserId].Num() - 1; i >= 0; i--)
    {
        if (*this->ConnectedPartiesByUser[LocalUserId][i] == PartyId)
        {
            this->ConnectedPartiesByUser[LocalUserId].RemoveAt(i);
        }
    }

    this->CleanupPartiesAndThenCallOnDone(ThisRef, OnDone);
}

void FTestPartyManager::Handle_OnRestoreInvitesComplete(
    const FUniqueNetId &LocalUserId,
    const FOnlineError &Result,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<FTestPartyManager> ThisRef,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    std::function<void()> OnDone)
{
    this->RestoreEventsRemaining--;

    UE_LOG(
        LogEOSTests,
        Verbose,
        TEXT("Restore invites complete, now %d events remaining"),
        this->RestoreEventsRemaining);

    if (this->RestoreEventsRemaining == 0)
    {
        UE_LOG(LogEOSTests, Verbose, TEXT("All invites restored, rejecting remaining invitations..."));

        this->RejectInvitationsAndThenCallOnDone(ThisRef, OnDone);
    }
}

void FTestPartyManager::RejectInvitationsAndThenCallOnDone(
    const TSharedRef<FTestPartyManager> &ThisRef,
    const FOnDone &OnDone)
{
    for (const auto &KV : this->Instances)
    {
        TArray<IOnlinePartyJoinInfoConstRef> Invites;
        TSharedPtr<IOnlinePartySystem, ESPMode::ThreadSafe> PartySystemBase =
            KV.Value.Subsystem.Pin()->GetPartyInterface();
        TSharedPtr<FOnlinePartySystemEOS, ESPMode::ThreadSafe> PartySystem =
            StaticCastSharedPtr<FOnlinePartySystemEOS, IOnlinePartySystem, ESPMode::ThreadSafe>(PartySystemBase);
        if (!PartySystem->GetPendingInvites(*KV.Key, Invites))
        {
            continue;
        }
        if (Invites.Num() == 0)
        {
            continue;
        }

        UE_LOG(
            LogEOSTests,
            Verbose,
            TEXT("Rejecting invitation to %s from %s for party %s ..."),
            *KV.Key->ToString(),
            *Invites[0]->GetSourceUserId()->ToString(),
            *Invites[0]->GetPartyId()->ToString());

        if (PartySystem->RejectInvitation(
                *KV.Key,
                *Invites[0],
                FOnRejectPartyInvitationComplete::CreateSP(
                    ThisRef,
                    &FTestPartyManager::Handle_OnRejectPartyInvitationComplete,
                    ThisRef,
                    OnDone)))
        {
            return;
        }

        UE_LOG(LogEOSTests, Warning, TEXT("Unable to reject pending invitation, expect party tests to fail"));
    }

    // Finally call OnDone.
    UE_LOG(LogEOSTests, Verbose, TEXT("All remaining invitations rejected."));
    OnDone();
}

void FTestPartyManager::Handle_OnRejectPartyInvitationComplete(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const ERejectPartyInvitationCompletionResult Result,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<FTestPartyManager> ThisRef,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    std::function<void()> OnDone)
{
    if (Result != ERejectPartyInvitationCompletionResult::Succeeded)
    {
        UE_LOG(
            LogEOSTests,
            Warning,
            TEXT("Automatic reject invite operation failed (this may cause other tests to fail)"));
    }

    this->RejectInvitationsAndThenCallOnDone(ThisRef, OnDone);
}

void FTestPartyManager::CleanupPartiesAndThenCallOnDone(
    const TSharedRef<FTestPartyManager> &ThisRef,
    const FOnDone &OnDone)
{
    for (auto KV : this->ConnectedPartiesByUser)
    {
        if (KV.Value.Num() == 0)
        {
            continue;
        }

        auto Instance = this->Instances[*KV.Key];
        auto PartySystem = Instance.Subsystem.Pin()->GetPartyInterface();

        UE_LOG(
            LogEOSTests,
            Verbose,
            TEXT("Automatically leaving party %s for local user ID %s due to test cleanup"),
            *KV.Value[0]->ToString(),
            *KV.Key->ToString());

        if (PartySystem->LeaveParty(
                *KV.Key,
                *KV.Value[0],
                FOnLeavePartyComplete::CreateSP(
                    ThisRef,
                    &FTestPartyManager::Handle_OnLeavePartyComplete,
                    ThisRef,
                    OnDone)))
        {
            // Leave party still needs to happen.
            return;
        }

        UE_LOG(LogEOSTests, Warning, TEXT("Unable to leave party, expect party tests to fail"));
    }

    UE_LOG(
        LogEOSTests,
        Verbose,
        TEXT("Request restore of any pending invites to reject any unwanted invitations (%d events expected)"),
        this->Instances.Num());

    // Restore all invites for each registered instance, so we can reject any invitations
    // that are still outstanding.
    this->RestoreEventsRemaining = this->Instances.Num();
    for (const auto &KV : this->Instances)
    {
        auto PartySystem = KV.Value.Subsystem.Pin()->GetPartyInterface();
        PartySystem->RestoreInvites(
            *KV.Key,
            FOnRestoreInvitesComplete::CreateSP(
                ThisRef,
                &FTestPartyManager::Handle_OnRestoreInvitesComplete,
                ThisRef,
                OnDone));
    }
}

TSharedRef<const FOnlinePartyId> FSingleTestPartyManager::Party(const FUniqueNetId &InUserId) const
{
    return this->ConnectedPartiesByUser[InUserId][0].ToSharedRef();
}

bool FSingleTestPartyManager::HasParty(const FUniqueNetId &InUserId) const
{
    return this->ConnectedPartiesByUser.Contains(InUserId) && this->ConnectedPartiesByUser[InUserId].Num() > 0;
}

TSharedRef<const FOnlinePartyId> FMultiTestPartyManager::Party(const FUniqueNetId &InUserId, int InIndex) const
{
    return this->ConnectedPartiesByUser[InUserId][InIndex].ToSharedRef();
}

int32 FMultiTestPartyManager::PartyNum(const FUniqueNetId &InUserId) const
{
    if (!this->ConnectedPartiesByUser.Contains(InUserId))
    {
        return 0;
    }

    return this->ConnectedPartiesByUser[InUserId].Num();
}

const TArray<TSharedPtr<const FOnlinePartyId>> FMultiTestPartyManager::Parties(const FUniqueNetId &InUserId) const
{
    if (!this->ConnectedPartiesByUser.Contains(InUserId))
    {
        return TArray<TSharedPtr<const FOnlinePartyId>>();
    }

    return this->ConnectedPartiesByUser[InUserId];
}

#endif // #if EOS_HAS_AUTHENTICATION