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
    FOnlineSubsystemEOS_OnlineTitleFileInterface_CanReadFile,
    "OnlineSubsystemEOS.OnlineTitleFileInterface.CanReadFile",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

const char TF_TestFile_Small[] = {'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
const size_t TF_TestFile_SmallSize = sizeof(TF_TestFile_Small);

void FOnlineSubsystemEOS_OnlineTitleFileInterface_CanReadFile::RunAsyncTest(const std::function<void()> &OnDone)
{
    FString TargetFilename = TEXT("TestFile.Small.txt");

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
                // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                std::function<void(bool, const FString &)>([this, OnDone, TargetFilename, TitleFile](
                                                               bool bWasSuccessful,
                                                               const FString &Error) {
                    TestTrue("Could enumerate over files", bWasSuccessful);
                    if (!bWasSuccessful)
                    {
                        OnDone();
                        return;
                    }

                    FDelegateHandle ReadProgressHandle = TitleFile->AddOnReadFileProgressDelegate_Handle(
                        FOnReadFileProgressDelegate::CreateLambda([this](const FString &Filename, uint64 BytesRead) {
                            this->AddInfo(FString::Printf(
                                TEXT("ReadFile progress, %llu bytes read for filename %s"),
                                BytesRead,
                                *Filename));
                        }));

                    auto CancelReadFile = RegisterOSSCallback(
                        this,
                        TitleFile,
                        &IOnlineTitleFile::AddOnReadFileCompleteDelegate_Handle,
                        &IOnlineTitleFile::ClearOnReadFileCompleteDelegate_Handle,
                        std::function<void(bool, const FString &)>(
                            // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                            [this, OnDone, TargetFilename, TitleFile, ReadProgressHandle](
                                bool bWasSuccessful,
                                const FString &Filename) {
                                FDelegateHandle ReadProgressHandleCopy = ReadProgressHandle;
                                TitleFile->ClearOnReadFileProgressDelegate_Handle(ReadProgressHandleCopy);

                                TestEqual("Read filename matches expected value", Filename, TargetFilename);
                                TestTrue("Read operation succeeded", bWasSuccessful);
                                if (!bWasSuccessful || Filename != TargetFilename)
                                {
                                    OnDone();
                                    return;
                                }

                                TArray<uint8> FileContentsRead;
                                TestTrue(
                                    "Can call GetFileContents",
                                    TitleFile->GetFileContents(Filename, FileContentsRead));
                                TestEqual("File has expected length", FileContentsRead.Num(), TF_TestFile_SmallSize);
                                if (FileContentsRead.Num() == TF_TestFile_SmallSize)
                                {
                                    for (int i = 0; i < TF_TestFile_SmallSize; i++)
                                    {
                                        TestEqual(
                                            FString::Printf(TEXT("File byte at index %d matches"), i),
                                            FileContentsRead[i],
                                            TF_TestFile_Small[i]);
                                    }
                                }
                                OnDone();
                            }));

                    bool bStarted = TitleFile->ReadFile(TargetFilename);
                    TestTrue("ReadFile operation started", bStarted);
                    if (!bStarted)
                    {
                        TitleFile->ClearOnReadFileProgressDelegate_Handle(ReadProgressHandle);
                        CancelReadFile();
                        OnDone();
                    }
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