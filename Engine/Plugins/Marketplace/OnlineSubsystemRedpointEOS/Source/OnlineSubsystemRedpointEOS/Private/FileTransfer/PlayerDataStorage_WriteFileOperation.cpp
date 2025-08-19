// Copyright June Rhodes. All Rights Reserved.

#include "PlayerDataStorage_WriteFileOperation.h"

#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSErrorConv.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineUserCloudInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

EOS_PlayerDataStorage_EWriteResult FEOS_PlayerDataStorage_WriteFileOperation::HandleWriteFileDataCallback(
    const EOS_PlayerDataStorage_WriteFileDataCallbackInfo *Data,
    void *OutDataBuffer,
    uint32_t *OutDataWritten)
{
    UE_LOG(
        LogEOSFileTransfer,
        Verbose,
        TEXT("WriteFileOperation: %p: WriteFileDataCallback: Filename=%s, DataBufferLengthBytes=%u"),
        Data->ClientData,
        ANSI_TO_TCHAR(Data->Filename),
        Data->DataBufferLengthBytes);

    TWeakPtr<FEOS_PlayerDataStorage_WriteFileOperation> *WeakThisPtr =
        (TWeakPtr<FEOS_PlayerDataStorage_WriteFileOperation> *)Data->ClientData;
    TSharedPtr<FEOS_PlayerDataStorage_WriteFileOperation> ThisPtr = WeakThisPtr->Pin();
    if (ThisPtr.IsValid())
    {
        return ThisPtr->HandleWriteFileDataCallbackInstance(Data, OutDataBuffer, OutDataWritten);
    }
    return EOS_PlayerDataStorage_EWriteResult::EOS_WR_CancelRequest;
}

EOS_PlayerDataStorage_EWriteResult FEOS_PlayerDataStorage_WriteFileOperation::HandleWriteFileDataCallbackInstance(
    const EOS_PlayerDataStorage_WriteFileDataCallbackInfo *Data,
    void *OutDataBuffer,
    uint32_t *OutDataWritten)
{
    if (auto UserCloud = this->UserCloudWk.Pin())
    {
        EOS_PlayerDataStorage_EWriteResult Outcome = EOS_PlayerDataStorage_EWriteResult::EOS_WR_ContinueWriting;
        uint32_t Len = Data->DataBufferLengthBytes;
        uint32_t RemainingBytes = this->FileDataSize - this->CurrentOffset;
        if (Len > RemainingBytes)
        {
            Len = RemainingBytes;
            Outcome = EOS_PlayerDataStorage_EWriteResult::EOS_WR_CompleteRequest;
        }
        FMemory::Memcpy(OutDataBuffer, &this->FileData[this->CurrentOffset], Len);
        *OutDataWritten = Len;
        this->CurrentOffset += Len;
        UE_LOG(
            LogEOSFileTransfer,
            Verbose,
            TEXT("WriteFileOperation handled WriteFileDataCallback (length: %u, remaining bytes: %u, current offset: "
                 "%u, file data size: %u, outcome: %d)"),
            Data->DataBufferLengthBytes,
            RemainingBytes,
            this->CurrentOffset,
            this->FileDataSize,
            Outcome);
        return Outcome;
    }

    return EOS_PlayerDataStorage_EWriteResult::EOS_WR_FailRequest;
}

void FEOS_PlayerDataStorage_WriteFileOperation::HandleFileTransferProgressCallback(
    const EOS_PlayerDataStorage_FileTransferProgressCallbackInfo *Data)
{
    UE_LOG(
        LogEOSFileTransfer,
        Verbose,
        TEXT("WriteFileOperation: %p: FileTransferProgressCallback: Filename=%s, TotalFileSizeBytes=%u, BytesTransferred=%u"),
        Data->ClientData,
        ANSI_TO_TCHAR(Data->Filename),
        Data->TotalFileSizeBytes,
        Data->BytesTransferred);

    TWeakPtr<FEOS_PlayerDataStorage_WriteFileOperation> *WeakThisPtr =
        (TWeakPtr<FEOS_PlayerDataStorage_WriteFileOperation> *)Data->ClientData;
    TSharedPtr<FEOS_PlayerDataStorage_WriteFileOperation> ThisPtr = WeakThisPtr->Pin();
    if (ThisPtr.IsValid())
    {
        ThisPtr->HandleFileTransferProgressCallbackInstance(Data);
    }
}

void FEOS_PlayerDataStorage_WriteFileOperation::HandleFileTransferProgressCallbackInstance(
    const EOS_PlayerDataStorage_FileTransferProgressCallbackInfo *Data)
{
    if (auto UserCloud = this->UserCloudWk.Pin())
    {
        UserCloud->TriggerOnWriteUserFileProgressDelegates(Data->BytesTransferred, *this->UserId, this->FileName);
    }
}

void FEOS_PlayerDataStorage_WriteFileOperation::HandleWriteFileCallback(
    const EOS_PlayerDataStorage_WriteFileCallbackInfo *Data)
{
    UE_LOG(
        LogEOSFileTransfer,
        Verbose,
        TEXT("WriteFileOperation: %p: WriteFileCallback: Filename=%s, ResultCode=%d, bIsComplete=%s"
             "BytesTransferred=%u"),
        Data->ClientData,
        ANSI_TO_TCHAR(Data->Filename),
        Data->ResultCode,
        EOS_EResult_IsOperationComplete(Data->ResultCode) ? TEXT("true") : TEXT("false"));

    if (!EOS_EResult_IsOperationComplete(Data->ResultCode))
    {
        // This isn't a terminating result code. Ignore it.
        FOnlineError Error = ConvertError(
            TEXT("FEOS_PlayerDataStorage_WriteFileOperation::HandleWriteFileCallback"),
            TEXT("Received non-terminating result code for write file callback; ignoring and waiting for a terminating "
                 "result code."),
            Data->ResultCode);
        UE_LOG(LogEOSFileTransfer, Warning, TEXT("%s"), *Error.ToLogString());
        return;
    }

    TWeakPtr<FEOS_PlayerDataStorage_WriteFileOperation> *WeakThisPtr =
        (TWeakPtr<FEOS_PlayerDataStorage_WriteFileOperation> *)Data->ClientData;
    TSharedPtr<FEOS_PlayerDataStorage_WriteFileOperation> ThisPtr = WeakThisPtr->Pin();
    delete (TWeakPtr<FEOS_PlayerDataStorage_WriteFileOperation> *)Data->ClientData;
    UE_LOG(
        LogEOSFileTransfer,
        Verbose,
        TEXT("WriteFileOperation: %p: Released memory for operation"),
        Data->ClientData);
    if (ThisPtr.IsValid())
    {
        ThisPtr->HandleWriteFileCallbackInstance(Data);
    }
}

void FEOS_PlayerDataStorage_WriteFileOperation::HandleWriteFileCallbackInstance(
    const EOS_PlayerDataStorage_WriteFileCallbackInfo *Data)
{
    if (auto UserCloud = this->UserCloudWk.Pin())
    {
        if (Data->ResultCode != EOS_EResult::EOS_Success)
        {
            UserCloud->TriggerOnWriteUserFileCompleteDelegates(false, *this->UserId, this->FileName);
            UserCloud->WriteOperations.Remove(GetOperationKey(this->UserId.ToSharedRef(), this->FileName));
            return;
        }

        UserCloud->TriggerOnWriteUserFileCompleteDelegates(true, *this->UserId, this->FileName);
        UserCloud->WriteOperations.Remove(GetOperationKey(this->UserId.ToSharedRef(), this->FileName));
    }
}

FEOS_PlayerDataStorage_WriteFileOperation::FEOS_PlayerDataStorage_WriteFileOperation(
    const TSharedRef<FOnlineUserCloudInterfaceEOS, ESPMode::ThreadSafe> &InUserCloud,
    const TSharedRef<const FUniqueNetIdEOS> &InUserId,
    const FString &InFileName,
    TArray<uint8> &InFileContents)
    : FileName(InFileName)
    , UserCloudWk(InUserCloud)
    , UserId(InUserId)
    , RequestHandle()
    , WriteOpts({})
    , bIsValid(false)
    , CurrentOffset(0)
    , FileDataSize(0)
    , FileData(nullptr)
{
    this->WriteOpts.ApiVersion = EOS_PLAYERDATASTORAGE_WRITEFILEOPTIONS_API_LATEST;
    this->WriteOpts.LocalUserId = InUserId->GetProductUserId();
    this->WriteOpts.Filename = nullptr;

    EOS_EResult Result =
        EOSString_PlayerDataStorage_Filename::AllocateToCharBuffer(InFileName, this->WriteOpts.Filename);
    this->bIsValid = Result == EOS_EResult::EOS_Success;
    if (!this->bIsValid)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("EOS_PlayerDataStorage_WriteFileOperation: Can not allocate filename '%s' to character buffer, got "
                 "error: %s"),
            *FileName,
            ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
        return;
    }

    this->CurrentOffset = 0;
    this->FileData = (uint8_t *)FMemory::MallocZeroed(InFileContents.Num());
    this->FileDataSize = InFileContents.Num();
    FMemory::Memcpy(this->FileData, InFileContents.GetData(), this->FileDataSize);

    this->WriteOpts.ChunkLengthBytes = EOS_CHUNK_LENGTH_BYTES;
    this->WriteOpts.WriteFileDataCallback = &FEOS_PlayerDataStorage_WriteFileOperation::HandleWriteFileDataCallback;
    this->WriteOpts.FileTransferProgressCallback =
        &FEOS_PlayerDataStorage_WriteFileOperation::HandleFileTransferProgressCallback;
}

TSharedPtr<FEOS_PlayerDataStorage_WriteFileOperation> FEOS_PlayerDataStorage_WriteFileOperation::StartOperation(
    const TSharedRef<FOnlineUserCloudInterfaceEOS, ESPMode::ThreadSafe> &InUserCloud,
    const TSharedRef<const FUniqueNetIdEOS> &InUserId,
    const FString &InFileName,
    TArray<uint8> &InFileContents)
{
    TSharedRef<FEOS_PlayerDataStorage_WriteFileOperation> Operation =
        MakeShareable(new FEOS_PlayerDataStorage_WriteFileOperation(InUserCloud, InUserId, InFileName, InFileContents));
    if (!Operation->bIsValid)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("EOS_PlayerDataStorage_WriteFileOperation: Could not create write operation for: %s"),
            *InFileName);
        return nullptr;
    }

    InUserCloud->WriteOperations.Add(GetOperationKey(InUserId, InFileName), Operation);

    TWeakPtr<FEOS_PlayerDataStorage_WriteFileOperation> *WeakThisPtr =
        new TWeakPtr<FEOS_PlayerDataStorage_WriteFileOperation>(Operation);

    Operation->RequestHandle = EOS_PlayerDataStorage_WriteFile(
        InUserCloud->EOSPlayerDataStorage,
        &Operation->WriteOpts,
        WeakThisPtr,
        &FEOS_PlayerDataStorage_WriteFileOperation::HandleWriteFileCallback);

    return Operation;
}

FEOS_PlayerDataStorage_WriteFileOperation::~FEOS_PlayerDataStorage_WriteFileOperation()
{
    if (this->bIsValid)
    {
        EOSString_PlayerDataStorage_Filename::FreeFromCharBuffer(this->WriteOpts.Filename);
        FMemory::Free(this->FileData);
    }
}

void FEOS_PlayerDataStorage_WriteFileOperation::Cancel()
{
    EOS_PlayerDataStorageFileTransferRequest_CancelRequest(this->RequestHandle);
}

EOS_DISABLE_STRICT_WARNINGS