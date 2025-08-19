// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"
#include "Templates/SharedPointer.h"

EOS_ENABLE_STRICT_WARNINGS

class FEOS_PlayerDataStorage_ReadFileOperation : public TSharedFromThis<FEOS_PlayerDataStorage_ReadFileOperation>
{
private:
    FString FileName;
    TWeakPtr<class FOnlineUserCloudInterfaceEOS, ESPMode::ThreadSafe> UserCloudWk;
    TSharedPtr<const FUniqueNetIdEOS> UserId;
    EOS_HPlayerDataStorageFileTransferRequest RequestHandle;
    EOS_PlayerDataStorage_ReadFileOptions ReadOpts;
    bool bIsValid;
    uint32_t CurrentOffset;
    uint32_t FileDataSize;
    uint8_t *FileData;

    static EOS_PlayerDataStorage_EReadResult HandleReadFileDataCallback(
        const EOS_PlayerDataStorage_ReadFileDataCallbackInfo *Data);
    EOS_PlayerDataStorage_EReadResult HandleReadFileDataCallbackInstance(
        const EOS_PlayerDataStorage_ReadFileDataCallbackInfo *Data);
    static void HandleFileTransferProgressCallback(const EOS_PlayerDataStorage_FileTransferProgressCallbackInfo *Data);
    void HandleFileTransferProgressCallbackInstance(const EOS_PlayerDataStorage_FileTransferProgressCallbackInfo *Data);
    static void HandleReadFileCallback(const EOS_PlayerDataStorage_ReadFileCallbackInfo *Data);
    void HandleReadFileCallbackInstance(const EOS_PlayerDataStorage_ReadFileCallbackInfo *Data);

    FEOS_PlayerDataStorage_ReadFileOperation(
        const TSharedRef<class FOnlineUserCloudInterfaceEOS, ESPMode::ThreadSafe> &InUserCloud,
        const TSharedRef<const FUniqueNetIdEOS> &InUserId,
        const FString &InFileName);
    UE_NONCOPYABLE(FEOS_PlayerDataStorage_ReadFileOperation);

public:
    static TSharedPtr<FEOS_PlayerDataStorage_ReadFileOperation> StartOperation(
        const TSharedRef<class FOnlineUserCloudInterfaceEOS, ESPMode::ThreadSafe> &InUserCloud,
        const TSharedRef<const FUniqueNetIdEOS> &InUserId,
        const FString &InFileName);

    static FString GetOperationKey(const TSharedRef<const FUniqueNetIdEOS> &InUserId, const FString &InFileName)
    {
        return FString::Printf(TEXT("RR_%s_%s"), *InUserId->GetProductUserIdString(), *InFileName);
    }

    ~FEOS_PlayerDataStorage_ReadFileOperation();

    void Cancel();
};

EOS_DISABLE_STRICT_WARNINGS