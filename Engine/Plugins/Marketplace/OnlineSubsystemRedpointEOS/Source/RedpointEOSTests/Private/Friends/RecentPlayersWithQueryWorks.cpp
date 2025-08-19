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
    FOnlineSubsystemEOS_Friends_RecentPlayersWithQueryWorks,
    "OnlineSubsystemEOS.Friends.RecentPlayersWithQueryWorks",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_Friends_RecentPlayersWithQueryWorks::RunAsyncTest(const std::function<void()> &OnDone)
{
    CreateSubsystemsForTest_CreateOnDemand(
        this,
        2,
        OnDone,
        [this](const TArray<FMultiplayerScenarioInstance> &Instances, const FOnDone &OnDone) {
            auto UserId = Instances[0].UserId;
            auto Friends = Instances[0].Subsystem.Pin()->GetFriendsInterface();

            TArray<TSharedRef<FOnlineRecentPlayer>> RecentPlayers;
            AddExpectedError(TEXT("You must call QueryRecentPlayers"));
            TestFalse("Can't get recent players", Friends->GetRecentPlayers(*UserId, TEXT(""), RecentPlayers));

            TSharedRef<FDelegateHandle> QueryHandle = MakeShared<FDelegateHandle>();
            *QueryHandle = Friends->AddOnQueryRecentPlayersCompleteDelegate_Handle(
                FOnQueryRecentPlayersCompleteDelegate::CreateLambda(
                    [this,
                     OnDone,
                     QueryHandle,
                     FriendsWk = TWeakPtr<IOnlineFriends, ESPMode::ThreadSafe>(Friends),
                     UserIdEOS = UserId](
                        const FUniqueNetId &UserId,
                        const FString &Namespace,
                        bool bWasSuccessful,
                        const FString &Error) {
                        TestTrue("Can query recent players", bWasSuccessful);
                        if (!bWasSuccessful)
                        {
                            FriendsWk.Pin()->ClearOnQueryRecentPlayersCompleteDelegate_Handle(*QueryHandle);
                            OnDone();
                            return;
                        }

                        TArray<TSharedRef<FOnlineRecentPlayer>> RecentPlayers;
                        TestTrue(
                            "Can get recent players",
                            FriendsWk.Pin()->GetRecentPlayers(*UserIdEOS, TEXT(""), RecentPlayers));
                        TestEqual("Recent players equals 0", RecentPlayers.Num(), 0);

                        OnDone();
                    }));

            if (!TestTrue("Can call QueryRecentPlayers", Friends->QueryRecentPlayers(*UserId, TEXT(""))))
            {
                Friends->ClearOnQueryRecentPlayersCompleteDelegate_Handle(*QueryHandle);
                OnDone();
            }
        });
}
