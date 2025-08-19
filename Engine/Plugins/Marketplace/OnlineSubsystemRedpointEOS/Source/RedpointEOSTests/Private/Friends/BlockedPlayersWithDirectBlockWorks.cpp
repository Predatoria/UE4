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
    FOnlineSubsystemEOS_Friends_BlockedPlayersWithDirectBlockWorks,
    "OnlineSubsystemEOS.Friends.BlockedPlayersWithDirectBlockWorks",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_Friends_BlockedPlayersWithDirectBlockWorks::RunAsyncTest(const std::function<void()> &OnDone)
{
    CreateSubsystemsForTest_CreateOnDemand(
        this,
        2,
        OnDone,
        [this](const TArray<FMultiplayerScenarioInstance> &Instances, const FOnDone &OnDone) {
            auto UserId = Instances[0].UserId;
            auto Friends = Instances[0].Subsystem.Pin()->GetFriendsInterface();

            TSharedRef<FDelegateHandle> BlockHandle = MakeShared<FDelegateHandle>();
            *BlockHandle = Friends->AddOnBlockedPlayerCompleteDelegate_Handle(
                0,
                FOnBlockedPlayerCompleteDelegate::CreateLambda(
                    [this,
                     OnDone,
                     FriendsWk = TWeakPtr<IOnlineFriends, ESPMode::ThreadSafe>(Friends),
                     UserIdEOS = UserId](
                        int32 LocalUserNum,
                        bool bWasSuccessful,
                        const FUniqueNetId &UniqueID,
                        const FString &ListName,
                        const FString &ErrorStr) {
                        TestTrue("BlockPlayer operation succeeds", bWasSuccessful);
                        if (!bWasSuccessful)
                        {
                            OnDone();
                            return;
                        }

                        TArray<TSharedRef<FOnlineBlockedPlayer>> BlockedPlayers;
                        TestTrue(
                            "Can get blocked players",
                            FriendsWk.Pin()->GetBlockedPlayers(*UserIdEOS, BlockedPlayers));
                        TestEqual("Blocked players equals 1", BlockedPlayers.Num(), 1);

                        OnDone();
                    }));

            TArray<FReportPlayedWithUser> Players;
            Players.Add(FReportPlayedWithUser(Instances[1].UserId.ToSharedRef(), TEXT("")));

            if (!TestTrue(TEXT("Can call BlockPlayer"), Friends->BlockPlayer(0, *Instances[1].UserId)))
            {
                Friends->ClearOnBlockedPlayerCompleteDelegate_Handle(0, *BlockHandle);
                OnDone();
            }
        });
}