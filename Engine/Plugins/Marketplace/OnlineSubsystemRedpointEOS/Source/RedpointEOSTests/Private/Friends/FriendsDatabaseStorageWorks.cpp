// Copyright June Rhodes. All Rights Reserved.

#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineUserCloudInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/PDSFriends/FriendDatabase.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_Friends_FriendsDatabaseStorageWorks,
    "OnlineSubsystemEOS.Friends.FriendsDatabaseStorageWorks",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_Friends_FriendsDatabaseStorageWorks::RunAsyncTest(const std::function<void()> &OnDone)
{
    CreateSingleSubsystemForTest_CreateOnDemand(
        this,
        OnDone,
        [this](
            const IOnlineSubsystemPtr &Subsystem,
            const TSharedPtr<const FUniqueNetIdEOS> &UserId,
            const FOnDone &OnDone) {
            auto UserCloud = StaticCastSharedPtr<FOnlineUserCloudInterfaceEOS>(Subsystem->GetUserCloudInterface());

            auto Db = MakeShared<FFriendDatabase>(UserId.ToSharedRef(), UserCloud.ToSharedRef());
            Db->WaitUntilLoaded(FFriendDatabaseOperationComplete::CreateLambda([this, Db, UserId, OnDone, UserCloud](
                                                                                   bool bWasSuccessful) {
                TestTrue("Friend database can load from empty", bWasSuccessful);
                if (!bWasSuccessful)
                {
                    OnDone();
                    return;
                }

                TestEqual("Friend count is 0", Db->GetFriends().Num(), 0);
                TestEqual("Cached delegated subsystem count is 0", Db->GetDelegatedSubsystemCachedFriends().Num(), 0);
                TestEqual("Recent player count is 0", Db->GetRecentPlayers().Num(), 0);
                TestEqual("Blocked users count is 0", Db->GetBlockedUsers().Num(), 0);
                TestEqual("Pending friend request count is 0", Db->GetPendingFriendRequests().Num(), 0);

                Db->GetFriends().Add(*UserId, FSerializedFriend{UserId->GetProductUserIdString()});
                Db->DirtyAndFlush(FFriendDatabaseOperationComplete::CreateLambda(
                    [this, Db, UserId, OnDone, UserCloud](bool bWasSuccessful) {
                        TestTrue("Friend database could be saved", bWasSuccessful);
                        if (!bWasSuccessful)
                        {
                            OnDone();
                            return;
                        }

                        auto NewDb = MakeShared<FFriendDatabase>(UserId.ToSharedRef(), UserCloud.ToSharedRef());
                        NewDb->WaitUntilLoaded(FFriendDatabaseOperationComplete::CreateLambda(
                            [this, NewDb, UserId, OnDone](bool bWasSuccessful) {
                                TestTrue("Friend database can load from stored data", bWasSuccessful);
                                if (!bWasSuccessful)
                                {
                                    OnDone();
                                    return;
                                }

                                TestEqual("Friend count is 1", NewDb->GetFriends().Num(), 1);
                                TestTrue("Friends list contains own ID", NewDb->GetFriends().Contains(*UserId));
                                OnDone();
                            }));
                    }));
            }));
        });
}