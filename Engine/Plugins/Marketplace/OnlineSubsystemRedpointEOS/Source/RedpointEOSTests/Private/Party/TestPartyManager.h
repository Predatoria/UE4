// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/HeapLambda.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlinePartyInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

class FTestPartyManager
{
    friend class FSingleTestPartyManager;
    friend class FMultiTestPartyManager;

private:
    TUserIdMap<TArray<TSharedPtr<const FOnlinePartyId>>> ConnectedPartiesByUser;
    TUserIdMap<FMultiplayerScenarioInstance> Instances;

    int RestoreEventsRemaining;
    void Handle_OnLeavePartyComplete(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const ELeavePartyCompletionResult Result,
        TSharedRef<FTestPartyManager> ThisRef,
        std::function<void()> OnDone);
    void Handle_OnRestoreInvitesComplete(
        const FUniqueNetId &LocalUserId,
        const FOnlineError &Result,
        TSharedRef<FTestPartyManager> ThisRef,
        std::function<void()> OnDone);
    void RejectInvitationsAndThenCallOnDone(const TSharedRef<FTestPartyManager> &ThisRef, const FOnDone &OnDone);
    void Handle_OnRejectPartyInvitationComplete(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const ERejectPartyInvitationCompletionResult Result,
        TSharedRef<FTestPartyManager> ThisRef,
        std::function<void()> OnDone);

protected:
    void AddParty(const FMultiplayerScenarioInstance &InInstance, const FOnlinePartyId &InPartyId);
    void RemoveParty(const FMultiplayerScenarioInstance &InInstance, const FOnlinePartyId &InPartyId);

    void CleanupPartiesAndThenCallOnDone(const TSharedRef<FTestPartyManager> &ThisRef, const FOnDone &OnDone);
};

class FSingleTestPartyManager : public FTestPartyManager
{
protected:
    TSharedRef<const FOnlinePartyId> Party(const FUniqueNetId &InUserId) const;
    bool HasParty(const FUniqueNetId &InUserId) const;
};

class FMultiTestPartyManager : public FTestPartyManager
{
protected:
    TSharedRef<const FOnlinePartyId> Party(const FUniqueNetId &InUserId, int InIndex) const;
    int32 PartyNum(const FUniqueNetId &InUserId) const;
    const TArray<TSharedPtr<const FOnlinePartyId>> Parties(const FUniqueNetId &InUserId) const;
};

#endif // #if EOS_HAS_AUTHENTICATION