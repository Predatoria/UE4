// Copyright June Rhodes. All Rights Reserved.

#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineUserCloudInterfaceEOS.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlineUserCloudInterface_CanEnumerateFiles,
    "OnlineSubsystemEOS.OnlineUserCloudInterface.CanEnumerateFiles",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_OnlineUserCloudInterface_CanEnumerateFiles::RunAsyncTest(const std::function<void()> &OnDone)
{
    CreateSingleSubsystemForTest_CreateOnDemand(
        this,
        OnDone,
        [this](
            const IOnlineSubsystemPtr &Subsystem,
            const TSharedPtr<const FUniqueNetIdEOS> &UserId,
            const FOnDone &OnDone) {
            if (!(Subsystem.IsValid() && UserId.IsValid()))
            {
                this->AddError(FString::Printf(TEXT("Unable to init subsystem / authenticate")));
                OnDone();
                return;
            }

            auto UserCloud = Subsystem->GetUserCloudInterface();
            TestTrue("Online subsystem provides IOnlineUserCloud interface", UserCloud.IsValid());

            if (!UserCloud.IsValid())
            {
                OnDone();
                return;
            }

            RegisterOSSCallback(
                this,
                UserCloud,
                &IOnlineUserCloud::AddOnEnumerateUserFilesCompleteDelegate_Handle,
                &IOnlineUserCloud::ClearOnEnumerateUserFilesCompleteDelegate_Handle,
                std::function<void(bool, const FUniqueNetId &)>(
                    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                    [this, OnDone, UserCloud](bool bWasSuccessful, const FUniqueNetId &UserId) {
                        TestTrue("Could enumerate over files", bWasSuccessful);

                        TArray<FCloudFileHeader> Results;
                        UserCloud->GetUserFileList(UserId, Results);
                        if (Results.Num() > 0)
                        {
                            AddInfo(
                                FString::Printf(TEXT("Discovered user cloud files for user %s"), *UserId.ToString()));
                        }
                        for (const auto &File : Results)
                        {
                            AddInfo(FString::Printf(
                                TEXT("User cloud file present: %s (%d bytes) (%s hash)"),
                                *File.FileName,
                                File.FileSize,
                                *File.Hash));
                        }

                        OnDone();
                    }));
            UserCloud->EnumerateUserFiles(*UserId.Get());
        });
}