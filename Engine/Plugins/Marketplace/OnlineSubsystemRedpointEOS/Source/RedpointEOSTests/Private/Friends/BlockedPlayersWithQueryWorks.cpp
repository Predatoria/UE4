// Copyright June Rhodes. All Rights Reserved.

#include "HAL/MemoryMisc.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineUserCloudInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/PDSFriends/FriendDatabase.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_Friends_BlockedPlayersWithQueryWorks,
    "OnlineSubsystemEOS.Friends.BlockedPlayersWithQueryWorks",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_Friends_BlockedPlayersWithQueryWorks::RunAsyncTest(const std::function<void()> &OnDone)
{
    CreateSubsystemsForTest_CreateOnDemand(
        this,
        2,
        OnDone,
        [this](const TArray<FMultiplayerScenarioInstance> &Instances, const FOnDone &OnDone) {
            auto UserId = Instances[0].UserId;
            auto Friends = Instances[0].Subsystem.Pin()->GetFriendsInterface();

            TArray<TSharedRef<FOnlineBlockedPlayer>> BlockedPlayers;
            AddExpectedError(TEXT("You must call QueryBlockedPlayers"));
            TestFalse("Can't get blocked players", Friends->GetBlockedPlayers(*UserId, BlockedPlayers));

            TSharedRef<FDelegateHandle> QueryHandle = MakeShared<FDelegateHandle>();
            *QueryHandle = Friends->AddOnQueryBlockedPlayersCompleteDelegate_Handle(
                FOnQueryBlockedPlayersCompleteDelegate::CreateLambda(
                    [this,
                     OnDone,
                     QueryHandle,
                     FriendsWk = TWeakPtr<IOnlineFriends, ESPMode::ThreadSafe>(Friends),
                     UserIdEOS = UserId](const FUniqueNetId &UserId, bool bWasSuccessful, const FString &Error) {
                        TestTrue("Can query blocked players", bWasSuccessful);
                        if (!bWasSuccessful)
                        {
                            OnDone();
                            return;
                        }

                        TArray<TSharedRef<FOnlineBlockedPlayer>> BlockedPlayers;
                        TestTrue(
                            "Can get blocked players",
                            FriendsWk.Pin()->GetBlockedPlayers(*UserIdEOS, BlockedPlayers));
                        TestEqual("Blocked players equals 0", BlockedPlayers.Num(), 0);

                        OnDone();
                    }));

            if (!TestTrue("Can call QueryBlockedPlayers", Friends->QueryBlockedPlayers(*UserId)))
            {
                Friends->ClearOnQueryBlockedPlayersCompleteDelegate_Handle(*QueryHandle);
                OnDone();
            }
        });
}
