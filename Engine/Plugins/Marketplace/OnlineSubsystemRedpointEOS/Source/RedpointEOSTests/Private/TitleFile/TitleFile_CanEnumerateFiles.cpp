// Copyright June Rhodes. All Rights Reserved.

#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineTitleFileInterfaceEOS.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

#if EOS_VERSION_AT_LEAST(1, 8, 0)

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlineTitleFileInterface_CanEnumerateFiles,
    "OnlineSubsystemEOS.OnlineTitleFileInterface.CanEnumerateFiles",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_OnlineTitleFileInterface_CanEnumerateFiles::RunAsyncTest(const std::function<void()> &OnDone)
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

            auto TitleFile = Subsystem->GetTitleFileInterface();
            TestTrue("Online subsystem provides IOnlineTitleFile interface", TitleFile.IsValid());

            if (!TitleFile.IsValid())
            {
                OnDone();
                return;
            }

            auto CancelEnumeration = RegisterOSSCallback(
                this,
                TitleFile,
                &IOnlineTitleFile::AddOnEnumerateFilesCompleteDelegate_Handle,
                &IOnlineTitleFile::ClearOnEnumerateFilesCompleteDelegate_Handle,
                std::function<void(bool, const FString &)>(
                    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                    [this, OnDone, TitleFile](bool bWasSuccessful, const FString &Error) {
                        TestTrue("Could enumerate over files", bWasSuccessful);

                        TArray<FCloudFileHeader> Results;
                        TitleFile->GetFileList(Results);
                        for (const auto &File : Results)
                        {
                            AddInfo(FString::Printf(
                                TEXT("Title file present: %s (%d bytes) (%s hash)"),
                                *File.FileName,
                                File.FileSize,
                                *File.Hash));
                        }

                        OnDone();
                    }));
            bool bWasStarted = TitleFile->EnumerateFiles();
            TestTrue("Enumeration was started", bWasStarted);
            if (!bWasStarted)
            {
                CancelEnumeration();
                OnDone();
            }
        });
}

#endif