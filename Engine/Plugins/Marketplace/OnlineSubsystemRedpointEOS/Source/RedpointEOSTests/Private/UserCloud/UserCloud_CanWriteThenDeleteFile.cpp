// Copyright June Rhodes. All Rights Reserved.

#include "./UserCloud_Common.h"
#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineUserCloudInterfaceEOS.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlineUserCloudInterface_CanWriteThenDeleteFile,
    "OnlineSubsystemEOS.OnlineUserCloudInterface.CanWriteThenDeleteFile",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_OnlineUserCloudInterface_CanWriteThenDeleteFile::RunAsyncTest(
    const std::function<void()> &OnDone)
{
    FString TargetFilename = TEXT("TestFile.Small");

    CreateSingleSubsystemForTest_CreateOnDemand(
        this,
        OnDone,
        [this, TargetFilename](
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

            FDelegateHandle WriteProgressHandle =
                UserCloud->AddOnWriteUserFileProgressDelegate_Handle(FOnWriteUserFileProgressDelegate::CreateLambda(
                    [this](int32 BytesWritten, const FUniqueNetId &UserId, const FString &Filename) {
                        this->AddInfo(FString::Printf(
                            TEXT("WriteUserFile progress, %d bytes written for filename %s"),
                            BytesWritten,
                            *Filename));
                    }));

            auto CancelWriteUserFile = RegisterOSSCallback(
                this,
                UserCloud,
                &IOnlineUserCloud::AddOnWriteUserFileCompleteDelegate_Handle,
                &IOnlineUserCloud::ClearOnWriteUserFileCompleteDelegate_Handle,
                std::function<void(bool, const FUniqueNetId &, const FString &)>(
                    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                    [this, OnDone, UserCloud, TargetFilename, WriteProgressHandle](
                        bool bWasSuccessful,
                        const FUniqueNetId &UserId,
                        const FString &Filename) {
                        FDelegateHandle WriteProgressHandleCopy = WriteProgressHandle;
                        UserCloud->ClearOnWriteUserFileProgressDelegate_Handle(WriteProgressHandleCopy);

                        TestEqual("Write filename matches expected value", Filename, TargetFilename);
                        TestTrue("Write operation succeeded", bWasSuccessful);
                        if (!bWasSuccessful || Filename != TargetFilename)
                        {
                            OnDone();
                            return;
                        }

                        // Query the file list to make sure the written file appears.
                        RegisterOSSCallback(
                            this,
                            UserCloud,
                            &IOnlineUserCloud::AddOnEnumerateUserFilesCompleteDelegate_Handle,
                            &IOnlineUserCloud::ClearOnEnumerateUserFilesCompleteDelegate_Handle,
                            // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                            std::function<void(bool, const FUniqueNetId &)>([this, OnDone, UserCloud, TargetFilename](
                                                                                bool bWasSuccessful,
                                                                                const FUniqueNetId &UserId) {
                                TestTrue("Could enumerate over files", bWasSuccessful);
                                if (!bWasSuccessful)
                                {
                                    OnDone();
                                    return;
                                }

                                bool bFoundWrittenFile = false;
                                TArray<FCloudFileHeader> Results;
                                UserCloud->GetUserFileList(UserId, Results);
                                for (const auto &File : Results)
                                {
                                    if (File.FileName == TargetFilename)
                                    {
                                        bFoundWrittenFile = true;
                                        break;
                                    }
                                }
                                TestTrue("Written file appears in the user's file list", bFoundWrittenFile);
                                if (!bFoundWrittenFile)
                                {
                                    OnDone();
                                    return;
                                }

                                // Now delete the file.
                                auto CancelDeleteUserFile = RegisterOSSCallback(
                                    this,
                                    UserCloud,
                                    &IOnlineUserCloud::AddOnDeleteUserFileCompleteDelegate_Handle,
                                    &IOnlineUserCloud::ClearOnDeleteUserFileCompleteDelegate_Handle,
                                    std::function<void(bool, const FUniqueNetId &, const FString &)>(
                                        // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                                        [this, OnDone, UserCloud, TargetFilename](
                                            bool bWasSuccessful,
                                            const FUniqueNetId &UserId,
                                            const FString &Filename) {
                                            TestTrue("Could delete file", bWasSuccessful);
                                            if (!bWasSuccessful)
                                            {
                                                OnDone();
                                                return;
                                            }

                                            // Query the file list again to make sure the written file is gone.
                                            RegisterOSSCallback(
                                                this,
                                                UserCloud,
                                                &IOnlineUserCloud::AddOnEnumerateUserFilesCompleteDelegate_Handle,
                                                &IOnlineUserCloud::ClearOnEnumerateUserFilesCompleteDelegate_Handle,
                                                std::function<void(bool, const FUniqueNetId &)>(
                                                    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                                                    [this, OnDone, UserCloud, TargetFilename](
                                                        bool bWasSuccessful,
                                                        const FUniqueNetId &UserId) {
                                                        TestTrue("Could enumerate over files", bWasSuccessful);
                                                        if (!bWasSuccessful)
                                                        {
                                                            OnDone();
                                                            return;
                                                        }

                                                        bool bFoundWrittenFile = false;
                                                        TArray<FCloudFileHeader> Results;
                                                        UserCloud->GetUserFileList(UserId, Results);
                                                        for (const auto &File : Results)
                                                        {
                                                            if (File.FileName == TargetFilename)
                                                            {
                                                                bFoundWrittenFile = true;
                                                                break;
                                                            }
                                                        }
                                                        TestFalse(
                                                            "Written file must not appear in the user's file list",
                                                            bFoundWrittenFile);
                                                        OnDone();
                                                    }));
                                            UserCloud->EnumerateUserFiles(UserId);
                                        }));
                                bool bStarted = UserCloud->DeleteUserFile(UserId, TargetFilename, true, true);
                                TestTrue("DeleteUserFile operation started", bStarted);
                                if (!bStarted)
                                {
                                    CancelDeleteUserFile();
                                    OnDone();
                                }
                            }));
                        UserCloud->EnumerateUserFiles(UserId);
                    }));

            TArray<uint8> FileContents = TArray<uint8>((uint8 *)UC_TestFile_Small, UC_TestFile_SmallSize);
            bool bStarted = UserCloud->WriteUserFile(*UserId.Get(), TargetFilename, FileContents);
            TestTrue("WriteUserFile operation started", bStarted);
            if (!bStarted)
            {
                UserCloud->ClearOnWriteUserFileProgressDelegate_Handle(WriteProgressHandle);
                CancelWriteUserFile();
                OnDone();
            }
        });
}