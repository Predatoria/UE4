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
    FOnlineSubsystemEOS_OnlineUserCloudInterface_CanWriteLargeFile,
    "OnlineSubsystemEOS.OnlineUserCloudInterface.CanWriteLargeFile",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_OnlineUserCloudInterface_CanWriteLargeFile::RunAsyncTest(const std::function<void()> &OnDone)
{
    FString TargetFilename = TEXT("TestFile.Large");

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
                        OnDone();
                    }));

            // Allocate 170kb of data for the write.
            const size_t FileSize = 174080;
            uint8 *FileContentsRaw = (uint8 *)FMemory::Malloc(FileSize);
            for (size_t i = 0; i < FileSize; i++)
            {
                FileContentsRaw[i] = (uint8)('a' + (i % 4));
            }
            TArray<uint8> FileContents(FileContentsRaw, FileSize);
            FMemory::Free(FileContentsRaw);
            FileContentsRaw = nullptr;

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