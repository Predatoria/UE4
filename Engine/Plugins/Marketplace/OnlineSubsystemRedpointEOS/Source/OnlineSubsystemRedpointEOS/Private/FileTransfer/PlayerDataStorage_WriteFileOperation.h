// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"
#include "Templates/SharedPointer.h"

EOS_ENABLE_STRICT_WARNINGS

class FEOS_PlayerDataStorage_WriteFileOperation : public TSharedFromThis<FEOS_PlayerDataStorage_WriteFileOperation>
{
private:
    FString FileName;
    TWeakPtr<class FOnlineUserCloudInterfaceEOS, ESPMode::ThreadSafe> UserCloudWk;
    TSharedPtr<const FUniqueNetIdEOS> UserId;
    EOS_HPlayerDataStorageFileTransferRequest RequestHandle;
    EOS_PlayerDataStorage_WriteFileOptions WriteOpts;
    bool bIsValid;
    uint32_t CurrentOffset;
    uint32_t FileDataSize;
    uint8_t *FileData;

    static EOS_PlayerDataStorage_EWriteResult HandleWriteFileDataCallback(
        const EOS_PlayerDataStorage_WriteFileDataCallbackInfo *Data,
        void *OutDataBuffer,
        uint32_t *OutDataWritten);
    EOS_PlayerDataStorage_EWriteResult HandleWriteFileDataCallbackInstance(
        const EOS_PlayerDataStorage_WriteFileDataCallbackInfo *Data,
        void *OutDataBuffer,
        uint32_t *OutDataWritten);
    static void HandleFileTransferProgressCallback(const EOS_PlayerDataStorage_FileTransferProgressCallbackInfo *Data);
    void HandleFileTransferProgressCallbackInstance(const EOS_PlayerDataStorage_FileTransferProgressCallbackInfo *Data);
    static void HandleWriteFileCallback(const EOS_PlayerDataStorage_WriteFileCallbackInfo *Data);
    void HandleWriteFileCallbackInstance(const EOS_PlayerDataStorage_WriteFileCallbackInfo *Data);

    FEOS_PlayerDataStorage_WriteFileOperation(
        const TSharedRef<class FOnlineUserCloudInterfaceEOS, ESPMode::ThreadSafe> &InUserCloud,
        const TSharedRef<const FUniqueNetIdEOS> &InUserId,
        const FString &InFileName,
        TArray<uint8> &InFileContents);
    UE_NONCOPYABLE(FEOS_PlayerDataStorage_WriteFileOperation);

public:
    static TSharedPtr<FEOS_PlayerDataStorage_WriteFileOperation> StartOperation(
        const TSharedRef<class FOnlineUserCloudInterfaceEOS, ESPMode::ThreadSafe> &InUserCloud,
        const TSharedRef<const FUniqueNetIdEOS> &InUserId,
        const FString &InFileName,
        TArray<uint8> &InFileContents);

    static FString GetOperationKey(const TSharedRef<const FUniqueNetIdEOS> &InUserId, const FString &InFileName)
    {
        return FString::Printf(TEXT("WR_%s_%s"), *InUserId->GetProductUserIdString(), *InFileName);
    }

    ~FEOS_PlayerDataStorage_WriteFileOperation();

    void Cancel();
};

EOS_DISABLE_STRICT_WARNINGS