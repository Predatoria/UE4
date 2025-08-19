// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"
#include "Templates/SharedPointer.h"

#if EOS_VERSION_AT_LEAST(1, 8, 0)

EOS_ENABLE_STRICT_WARNINGS

class FEOS_TitleStorage_ReadFileOperation : public TSharedFromThis<FEOS_TitleStorage_ReadFileOperation>
{
private:
    FString FileName;
    TWeakPtr<class FOnlineTitleFileInterfaceEOS, ESPMode::ThreadSafe> TitleFileWk;
    EOS_HTitleStorageFileTransferRequest RequestHandle;
    EOS_TitleStorage_ReadFileOptions ReadOpts;
    bool bIsValid;
    uint32_t CurrentOffset;
    uint32_t FileDataSize;
    uint8_t *FileData;

    static EOS_TitleStorage_EReadResult HandleReadFileDataCallback(
        const EOS_TitleStorage_ReadFileDataCallbackInfo *Data);
    EOS_TitleStorage_EReadResult HandleReadFileDataCallbackInstance(
        const EOS_TitleStorage_ReadFileDataCallbackInfo *Data);
    static void HandleFileTransferProgressCallback(const EOS_TitleStorage_FileTransferProgressCallbackInfo *Data);
    void HandleFileTransferProgressCallbackInstance(const EOS_TitleStorage_FileTransferProgressCallbackInfo *Data);
    static void HandleReadFileCallback(const EOS_TitleStorage_ReadFileCallbackInfo *Data);
    void HandleReadFileCallbackInstance(const EOS_TitleStorage_ReadFileCallbackInfo *Data);

    FEOS_TitleStorage_ReadFileOperation(
        const TSharedRef<class FOnlineTitleFileInterfaceEOS, ESPMode::ThreadSafe> &InTitleFile,
        const TSharedPtr<const FUniqueNetIdEOS> &InUserId,
        const FString &InFileName);
    UE_NONCOPYABLE(FEOS_TitleStorage_ReadFileOperation);

public:
    static TSharedPtr<FEOS_TitleStorage_ReadFileOperation> StartOperation(
        const TSharedRef<class FOnlineTitleFileInterfaceEOS, ESPMode::ThreadSafe> &InTitleFile,
        const TSharedPtr<const FUniqueNetIdEOS> &InUserId,
        const FString &InFileName);

    ~FEOS_TitleStorage_ReadFileOperation();

    void Cancel();
};

EOS_DISABLE_STRICT_WARNINGS

#endif