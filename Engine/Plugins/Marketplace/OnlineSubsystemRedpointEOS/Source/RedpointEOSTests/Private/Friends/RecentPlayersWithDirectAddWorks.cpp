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
    FOnlineSubsystemEOS_Friends_RecentPlayersWithDirectAddWorks,
    "OnlineSubsystemEOS.Friends.RecentPlayersWithDirectAddWorks",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_Friends_RecentPlayersWithDirectAddWorks::RunAsyncTest(const std::function<void()> &OnDone)
{
    CreateSubsystemsForTest_CreateOnDemand(
        this,
        2,
        OnDone,
        [this](const TArray<FMultiplayerScenarioInstance> &Instances, const FOnDone &OnDone) {
            auto UserId = Instances[0].UserId;
            auto Friends = Instances[0].Subsystem.Pin()->GetFriendsInterface();

            FDateTime EarliestTime = FDateTime::UtcNow();

            TSharedRef<int> CallCount = MakeShared<int>(0);
            TSharedRef<int> PlayersAddedCount = MakeShared<int>(0);

            Friends->AddOnRecentPlayersAddedDelegate_Handle(FOnRecentPlayersAddedDelegate::CreateLambda(
                [this, CallCount, PlayersAddedCount](
                    const FUniqueNetId &UserId,
                    const TArray<TSharedRef<FOnlineRecentPlayer>> &AddedPlayers) {
                    *CallCount += 1;
                    *PlayersAddedCount += AddedPlayers.Num();
                }));

            TArray<FReportPlayedWithUser> Players;
            Players.Add(FReportPlayedWithUser(Instances[1].UserId.ToSharedRef(), TEXT("")));

            Friends->AddRecentPlayers(
                *UserId,
                Players,
                TEXT(""),
                FOnAddRecentPlayersComplete::CreateLambda(
                    [this,
                     OnDone,
                     CallCount,
                     PlayersAddedCount,
                     FriendsWk = TWeakPtr<IOnlineFriends, ESPMode::ThreadSafe>(Friends),
                     UserIdEOS = UserId](const FUniqueNetId &UserId, const FOnlineError &Error) {
                        TestTrue("AddRecentPlayers operation succeeds", Error.bSucceeded);
                        TestEqual("OnRecentPlayersAdded fired once", *CallCount, 1);
                        TestEqual("OnRecentPlayersAdded reported one user added", *PlayersAddedCount, 1);
                        if (!Error.bSucceeded)
                        {
                            OnDone();
                            return;
                        }

                        TArray<TSharedRef<FOnlineRecentPlayer>> RecentPlayers;
                        TestTrue(
                            "Can get recent players",
                            FriendsWk.Pin()->GetRecentPlayers(*UserIdEOS, TEXT(""), RecentPlayers));
                        TestEqual("Recent players equals 1", RecentPlayers.Num(), 1);

                        OnDone();
                    }));
        });
}
