// Copyright June Rhodes. All Rights Reserved.

#include "TitleStorage_ReadFileOperation.h"

#if EOS_VERSION_AT_LEAST(1, 8, 0)

#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSErrorConv.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineTitleFileInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

EOS_TitleStorage_EReadResult FEOS_TitleStorage_ReadFileOperation::HandleReadFileDataCallback(
    const EOS_TitleStorage_ReadFileDataCallbackInfo *Data)
{
    TWeakPtr<FEOS_TitleStorage_ReadFileOperation> *WeakThisPtr =
        (TWeakPtr<FEOS_TitleStorage_ReadFileOperation> *)Data->ClientData;
    TSharedPtr<FEOS_TitleStorage_ReadFileOperation> ThisPtr = WeakThisPtr->Pin();
    if (ThisPtr.IsValid())
    {
        return ThisPtr->HandleReadFileDataCallbackInstance(Data);
    }
    return EOS_TitleStorage_EReadResult::EOS_TS_RR_CancelRequest;
}

EOS_TitleStorage_EReadResult FEOS_TitleStorage_ReadFileOperation::HandleReadFileDataCallbackInstance(
    const EOS_TitleStorage_ReadFileDataCallbackInfo *Data)
{
    if (auto TitleFile = this->TitleFileWk.Pin())
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

        return EOS_TitleStorage_EReadResult::EOS_TS_RR_ContinueReading;
    }

    return EOS_TitleStorage_EReadResult::EOS_TS_RR_FailRequest;
}

void FEOS_TitleStorage_ReadFileOperation::HandleFileTransferProgressCallback(
    const EOS_TitleStorage_FileTransferProgressCallbackInfo *Data)
{
    TWeakPtr<FEOS_TitleStorage_ReadFileOperation> *WeakThisPtr =
        (TWeakPtr<FEOS_TitleStorage_ReadFileOperation> *)Data->ClientData;
    TSharedPtr<FEOS_TitleStorage_ReadFileOperation> ThisPtr = WeakThisPtr->Pin();
    if (ThisPtr.IsValid())
    {
        return ThisPtr->HandleFileTransferProgressCallbackInstance(Data);
    }
}

void FEOS_TitleStorage_ReadFileOperation::HandleFileTransferProgressCallbackInstance(
    const EOS_TitleStorage_FileTransferProgressCallbackInfo *Data)
{
    if (auto TitleFile = this->TitleFileWk.Pin())
    {
        TitleFile->TriggerOnReadFileProgressDelegates(this->FileName, Data->BytesTransferred);
    }
}

void FEOS_TitleStorage_ReadFileOperation::HandleReadFileCallback(const EOS_TitleStorage_ReadFileCallbackInfo *Data)
{
    if (!EOS_EResult_IsOperationComplete(Data->ResultCode))
    {
        // This isn't a terminating result code. Ignore it.
        FOnlineError Error = ConvertError(
            TEXT("FEOS_TitleStorage_ReadFileOperation::HandleReadFileCallback"),
            TEXT("Received non-terminating result code for read file callback; ignoring and waiting for a terminating "
                 "result code."),
            Data->ResultCode);
        UE_LOG(LogEOSFileTransfer, Warning, TEXT("%s"), *Error.ToLogString());
        return;
    }

    TWeakPtr<FEOS_TitleStorage_ReadFileOperation> *WeakThisPtr =
        (TWeakPtr<FEOS_TitleStorage_ReadFileOperation> *)Data->ClientData;
    TSharedPtr<FEOS_TitleStorage_ReadFileOperation> ThisPtr = WeakThisPtr->Pin();
    delete (TWeakPtr<FEOS_TitleStorage_ReadFileOperation> *)Data->ClientData;
    if (ThisPtr.IsValid())
    {
        return ThisPtr->HandleReadFileCallbackInstance(Data);
    }
}

void FEOS_TitleStorage_ReadFileOperation::HandleReadFileCallbackInstance(
    const EOS_TitleStorage_ReadFileCallbackInfo *Data)
{
    if (auto TitleFile = this->TitleFileWk.Pin())
    {
        if (Data->ResultCode != EOS_EResult::EOS_Success)
        {
            TitleFile->TriggerOnReadFileCompleteDelegates(false, this->FileName);
            TitleFile->ReadOperations.Remove(this->FileName);
            return;
        }

        TitleFile->SubmitDownloadedFileToCache(this->FileName, TArray<uint8>(this->FileData, this->FileDataSize));

        TitleFile->TriggerOnReadFileCompleteDelegates(true, this->FileName);
        TitleFile->ReadOperations.Remove(this->FileName);
    }
}

FEOS_TitleStorage_ReadFileOperation::FEOS_TitleStorage_ReadFileOperation(
    const TSharedRef<FOnlineTitleFileInterfaceEOS, ESPMode::ThreadSafe> &InTitleFile,
    const TSharedPtr<const FUniqueNetIdEOS> &InUserId,
    const FString &InFileName)
    : FileName(InFileName)
    , TitleFileWk(InTitleFile)
    , RequestHandle()
    , ReadOpts({})
    , bIsValid(false)
    , CurrentOffset(0)
    , FileDataSize(0)
    , FileData(nullptr)
{
    this->ReadOpts.ApiVersion = EOS_TITLESTORAGE_READFILEOPTIONS_API_LATEST;
    this->ReadOpts.LocalUserId = InUserId.IsValid() ? InUserId->GetProductUserId() : nullptr;
    this->ReadOpts.Filename = nullptr;

    EOS_EResult Result = EOSString_TitleStorage_Filename::AllocateToCharBuffer(InFileName, this->ReadOpts.Filename);
    this->bIsValid = Result == EOS_EResult::EOS_Success;
    if (!this->bIsValid)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("EOS_TitleStorage_ReadFileOperation: Can not allocate filename '%s' to character buffer, got "
                 "error: %s"),
            *FileName,
            ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
        return;
    }

    this->CurrentOffset = 0;
    this->FileData = nullptr;
    this->FileDataSize = 0;

    this->ReadOpts.ReadChunkLengthBytes = EOS_CHUNK_LENGTH_BYTES;
    this->ReadOpts.ReadFileDataCallback = &FEOS_TitleStorage_ReadFileOperation::HandleReadFileDataCallback;
    this->ReadOpts.FileTransferProgressCallback =
        &FEOS_TitleStorage_ReadFileOperation::HandleFileTransferProgressCallback;
}

TSharedPtr<FEOS_TitleStorage_ReadFileOperation> FEOS_TitleStorage_ReadFileOperation::StartOperation(
    const TSharedRef<FOnlineTitleFileInterfaceEOS, ESPMode::ThreadSafe> &InTitleFile,
    const TSharedPtr<const FUniqueNetIdEOS> &InUserId,
    const FString &InFileName)
{
    TSharedRef<FEOS_TitleStorage_ReadFileOperation> Operation =
        MakeShareable(new FEOS_TitleStorage_ReadFileOperation(InTitleFile, InUserId, InFileName));
    if (!Operation->bIsValid)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("EOS_TitleStorage_ReadFileOperation: Could not create read operation for: %s"),
            *InFileName);
        return nullptr;
    }

    InTitleFile->ReadOperations.Add(InFileName, Operation);

    TWeakPtr<FEOS_TitleStorage_ReadFileOperation> *WeakThisPtr =
        new TWeakPtr<FEOS_TitleStorage_ReadFileOperation>(Operation);

    Operation->RequestHandle = EOS_TitleStorage_ReadFile(
        InTitleFile->EOSTitleStorage,
        &Operation->ReadOpts,
        WeakThisPtr,
        &FEOS_TitleStorage_ReadFileOperation::HandleReadFileCallback);

    return Operation;
}

FEOS_TitleStorage_ReadFileOperation::~FEOS_TitleStorage_ReadFileOperation()
{
    if (this->bIsValid)
    {
        EOSString_TitleStorage_Filename::FreeFromCharBuffer(this->ReadOpts.Filename);
        if (this->FileData != nullptr)
        {
            FMemory::Free(this->FileData);
        }
    }
}

void FEOS_TitleStorage_ReadFileOperation::Cancel()
{
    EOS_TitleStorageFileTransferRequest_CancelRequest(this->RequestHandle);
}

EOS_DISABLE_STRICT_WARNINGS

#endif