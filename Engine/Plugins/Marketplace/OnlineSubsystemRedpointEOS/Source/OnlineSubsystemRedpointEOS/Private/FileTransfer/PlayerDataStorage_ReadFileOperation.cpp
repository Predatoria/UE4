// Copyright June Rhodes. All Rights Reserved.

#include "PlayerDataStorage_ReadFileOperation.h"

#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSErrorConv.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineUserCloudInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

EOS_PlayerDataStorage_EReadResult FEOS_PlayerDataStorage_ReadFileOperation::HandleReadFileDataCallback(
    const EOS_PlayerDataStorage_ReadFileDataCallbackInfo *Data)
{
    TWeakPtr<FEOS_PlayerDataStorage_ReadFileOperation> *WeakThisPtr =
        (TWeakPtr<FEOS_PlayerDataStorage_ReadFileOperation> *)Data->ClientData;
    TSharedPtr<FEOS_PlayerDataStorage_ReadFileOperation> ThisPtr = WeakThisPtr->Pin();
    if (ThisPtr.IsValid())
    {
        return ThisPtr->HandleReadFileDataCallbackInstance(Data);
    }
    return EOS_PlayerDataStorage_EReadResult::EOS_RR_CancelRequest;
}

EOS_PlayerDataStorage_EReadResult FEOS_PlayerDataStorage_ReadFileOperation::HandleReadFileDataCallbackInstance(
    const EOS_PlayerDataStorage_ReadFileDataCallbackInfo *Data)
{
    if (auto UserCloud = this->UserCloudWk.Pin())
    {
        if (this->FileData == nullptr)
        {
            // We haven't allocated memory to store this file in yet. Allocate the block of memory and set the
            // total filesize.
            this->FileData = (uint8 *)FMemory::MallocZeroed(Data->TotalFileSizeBytes);
            this->FileDataSize = Data->TotalFileSizeBytes;
        }

        check(this->CurrentOffset + Data->DataChunkLengthBytes <= this->FileDataSize);
        FMemory::Memcpy(this->FileData + this->CurrentOffset, Data->DataChunk, Data->DataChunkLengthBytes);
        this->CurrentOffset += Data->DataChunkLengthBytes;

        return EOS_PlayerDataStorage_EReadResult::EOS_RR_ContinueReading;
    }

    return EOS_PlayerDataStorage_EReadResult::EOS_RR_FailRequest;
}

void FEOS_PlayerDataStorage_ReadFileOperation::HandleFileTransferProgressCallback(
    const EOS_PlayerDataStorage_FileTransferProgressCallbackInfo *Data)
{
    TWeakPtr<FEOS_PlayerDataStorage_ReadFileOperation> *WeakThisPtr =
        (TWeakPtr<FEOS_PlayerDataStorage_ReadFileOperation> *)Data->ClientData;
    TSharedPtr<FEOS_PlayerDataStorage_ReadFileOperation> ThisPtr = WeakThisPtr->Pin();
    if (ThisPtr.IsValid())
    {
        return ThisPtr->HandleFileTransferProgressCallbackInstance(Data);
    }
}

void FEOS_PlayerDataStorage_ReadFileOperation::HandleFileTransferProgressCallbackInstance(
    const EOS_PlayerDataStorage_FileTransferProgressCallbackInfo *Data)
{
    // NOTE: The IOnlineUserCloud interface does not have a "read progress" event, but the IOnlineTitleFile
    // service does, so we keep the function definition here (and the EOS callback registration) to make it
    // easier to keep the UserCloud and TitleFile code in sync.
}

void FEOS_PlayerDataStorage_ReadFileOperation::HandleReadFileCallback(
    const EOS_PlayerDataStorage_ReadFileCallbackInfo *Data)
{
    if (!EOS_EResult_IsOperationComplete(Data->ResultCode))
    {
        // This isn't a terminating result code. Ignore it.
        FOnlineError Error = ConvertError(
            TEXT("FEOS_PlayerDataStorage_ReadFileOperation::HandleReadFileCallback"),
            TEXT("Received non-terminating result code for read file callback; ignoring and waiting for a terminating "
                 "result code."),
            Data->ResultCode);
        UE_LOG(LogEOSFileTransfer, Warning, TEXT("%s"), *Error.ToLogString());
        return;
    }

    TWeakPtr<FEOS_PlayerDataStorage_ReadFileOperation> *WeakThisPtr =
        (TWeakPtr<FEOS_PlayerDataStorage_ReadFileOperation> *)Data->ClientData;
    TSharedPtr<FEOS_PlayerDataStorage_ReadFileOperation> ThisPtr = WeakThisPtr->Pin();
    delete (TWeakPtr<FEOS_PlayerDataStorage_ReadFileOperation> *)Data->ClientData;
    if (ThisPtr.IsValid())
    {
        return ThisPtr->HandleReadFileCallbackInstance(Data);
    }
}

void FEOS_PlayerDataStorage_ReadFileOperation::HandleReadFileCallbackInstance(
    const EOS_PlayerDataStorage_ReadFileCallbackInfo *Data)
{
    if (auto UserCloud = this->UserCloudWk.Pin())
    {
        if (Data->ResultCode != EOS_EResult::EOS_Success)
        {
            UserCloud->TriggerOnReadUserFileCompleteDelegates(false, *this->UserId, this->FileName);
            UserCloud->ReadOperations.Remove(GetOperationKey(this->UserId.ToSharedRef(), this->FileName));
            return;
        }

        UserCloud->SubmitDownloadedFileToCache(
            this->UserId.ToSharedRef(),
            this->FileName,
            TArray<uint8>(this->FileData, this->FileDataSize));

        UserCloud->TriggerOnReadUserFileCompleteDelegates(true, *this->UserId, this->FileName);
        UserCloud->ReadOperations.Remove(GetOperationKey(this->UserId.ToSharedRef(), this->FileName));
    }
}

FEOS_PlayerDataStorage_ReadFileOperation::FEOS_PlayerDataStorage_ReadFileOperation(
    const TSharedRef<FOnlineUserCloudInterfaceEOS, ESPMode::ThreadSafe> &InUserCloud,
    const TSharedRef<const FUniqueNetIdEOS> &InUserId,
    const FString &InFileName)
    : FileName(InFileName)
    , UserCloudWk(InUserCloud)
    , UserId(InUserId)
    , RequestHandle()
    , ReadOpts({})
    , bIsValid(false)
    , CurrentOffset(0)
    , FileDataSize(0)
    , FileData(nullptr)
{
    this->ReadOpts.ApiVersion = EOS_PLAYERDATASTORAGE_READFILEOPTIONS_API_LATEST;
    this->ReadOpts.LocalUserId = InUserId->GetProductUserId();
    this->ReadOpts.Filename = nullptr;

    EOS_EResult Result =
        EOSString_PlayerDataStorage_Filename::AllocateToCharBuffer(InFileName, this->ReadOpts.Filename);
    this->bIsValid = Result == EOS_EResult::EOS_Success;
    if (!this->bIsValid)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("EOS_PlayerDataStorage_ReadFileOperation: Can not allocate filename '%s' to character buffer, got "
                 "error: %s"),
            *FileName,
            ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
        return;
    }

    this->CurrentOffset = 0;
    this->FileData = nullptr;
    this->FileDataSize = 0;

    this->ReadOpts.ReadChunkLengthBytes = EOS_CHUNK_LENGTH_BYTES;
    this->ReadOpts.ReadFileDataCallback = &FEOS_PlayerDataStorage_ReadFileOperation::HandleReadFileDataCallback;
    this->ReadOpts.FileTransferProgressCallback =
        &FEOS_PlayerDataStorage_ReadFileOperation::HandleFileTransferProgressCallback;
}

TSharedPtr<FEOS_PlayerDataStorage_ReadFileOperation> FEOS_PlayerDataStorage_ReadFileOperation::StartOperation(
    const TSharedRef<FOnlineUserCloudInterfaceEOS, ESPMode::ThreadSafe> &InUserCloud,
    const TSharedRef<const FUniqueNetIdEOS> &InUserId,
    const FString &InFileName)
{
    TSharedRef<FEOS_PlayerDataStorage_ReadFileOperation> Operation =
        MakeShareable(new FEOS_PlayerDataStorage_ReadFileOperation(InUserCloud, InUserId, InFileName));
    if (!Operation->bIsValid)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("EOS_PlayerDataStorage_ReadFileOperation: Could not create read operation for: %s"),
            *InFileName);
        return nullptr;
    }

    InUserCloud->ReadOperations.Add(GetOperationKey(InUserId, InFileName), Operation);

    TWeakPtr<FEOS_PlayerDataStorage_ReadFileOperation> *WeakThisPtr =
        new TWeakPtr<FEOS_PlayerDataStorage_ReadFileOperation>(Operation);

    Operation->RequestHandle = EOS_PlayerDataStorage_ReadFile(
        InUserCloud->EOSPlayerDataStorage,
        &Operation->ReadOpts,
        WeakThisPtr,
        &FEOS_PlayerDataStorage_ReadFileOperation::HandleReadFileCallback);

    return Operation;
}

FEOS_PlayerDataStorage_ReadFileOperation::~FEOS_PlayerDataStorage_ReadFileOperation()
{
    if (this->bIsValid)
    {
        EOSString_PlayerDataStorage_Filename::FreeFromCharBuffer(this->ReadOpts.Filename);
        if (this->FileData != nullptr)
        {
            FMemory::Free(this->FileData);
        }
    }
}

void FEOS_PlayerDataStorage_ReadFileOperation::Cancel()
{
    EOS_PlayerDataStorageFileTransferRequest_CancelRequest(this->RequestHandle);
}

EOS_DISABLE_STRICT_WARNINGS