// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/OnlineUserCloudInterfaceEOS.h"

#include "OnlineSubsystemRedpointEOS/Private/FileTransfer/PlayerDataStorage_ReadFileOperation.h"
#include "OnlineSubsystemRedpointEOS/Private/FileTransfer/PlayerDataStorage_WriteFileOperation.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

void FOnlineUserCloudInterfaceEOS::SubmitDownloadedFileToCache(
    const TSharedRef<const FUniqueNetIdEOS> &InUserId,
    const FString &InFileName,
    const TArray<uint8> &InFileContents)
{
    if (!this->DownloadCacheByUserThenFilename.Contains(*InUserId))
    {
        this->DownloadCacheByUserThenFilename.Add(*InUserId, TMap<FFilename, TArray<uint8>>());
    }
    TMap<FFilename, TArray<uint8>> &MapRef = this->DownloadCacheByUserThenFilename[*InUserId];
    MapRef.Add(InFileName, InFileContents);
}

FOnlineUserCloudInterfaceEOS::FOnlineUserCloudInterfaceEOS(EOS_HPlatform InPlatform)
    : EOSPlatform(InPlatform)
    , EOSPlayerDataStorage(EOS_Platform_GetPlayerDataStorageInterface(this->EOSPlatform))
    , WriteOperations()
    , ReadOperations()
    , DownloadCacheByUserThenFilename()
{
}

FOnlineUserCloudInterfaceEOS::~FOnlineUserCloudInterfaceEOS()
{
}

bool FOnlineUserCloudInterfaceEOS::GetFileContents(
    const FUniqueNetId &UserId,
    const FString &FileName,
    TArray<uint8> &FileContents)
{
    if (UserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("GetFileContents: User ID was not an EOS user ID"));
        return false;
    }

    if (!this->DownloadCacheByUserThenFilename.Contains(UserId))
    {
        return false;
    }
    if (!this->DownloadCacheByUserThenFilename[UserId].Contains(FileName))
    {
        return false;
    }
    FileContents = this->DownloadCacheByUserThenFilename[UserId][FileName];
    return true;
}

bool FOnlineUserCloudInterfaceEOS::ClearFiles(const FUniqueNetId &UserId)
{
    if (UserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("ClearFiles: User ID was not an EOS user ID"));
        return false;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared());

    if (this->DownloadCacheByUserThenFilename.Contains(UserId))
    {
        this->DownloadCacheByUserThenFilename.Remove(UserId);
    }
    return true;
}

bool FOnlineUserCloudInterfaceEOS::ClearFile(const FUniqueNetId &UserId, const FString &FileName)
{
    if (UserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("ClearFile: User ID was not an EOS user ID"));
        return false;
    }

    if (!this->DownloadCacheByUserThenFilename.Contains(UserId))
    {
        return true;
    }
    if (!this->DownloadCacheByUserThenFilename[UserId].Contains(FileName))
    {
        return true;
    }
    this->DownloadCacheByUserThenFilename[UserId].Remove(FileName);
    return true;
}

void FOnlineUserCloudInterfaceEOS::EnumerateUserFiles(const FUniqueNetId &UserId)
{
    if (UserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("EnumerateUserFiles: User ID was not an EOS user ID"));
        this->TriggerOnEnumerateUserFilesCompleteDelegates(false, UserId);
        return;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared());

    EOS_PlayerDataStorage_QueryFileListOptions Opts = {};
    Opts.ApiVersion = EOS_PLAYERDATASTORAGE_QUERYFILELISTOPTIONS_API_LATEST;
    Opts.LocalUserId = EOSUser->GetProductUserId();

    EOSRunOperation<
        EOS_HPlayerDataStorage,
        EOS_PlayerDataStorage_QueryFileListOptions,
        EOS_PlayerDataStorage_QueryFileListCallbackInfo>(
        this->EOSPlayerDataStorage,
        &Opts,
        EOS_PlayerDataStorage_QueryFileList,
        [WeakThis = GetWeakThis(this), EOSUser](const EOS_PlayerDataStorage_QueryFileListCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                // EOS_NotFound is returned if the user has no files stored at all.
                if (Data->ResultCode != EOS_EResult::EOS_Success && Data->ResultCode != EOS_EResult::EOS_NotFound)
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("EnumerateUserFiles: Unable to query file list, got error: %s"),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
                    This->TriggerOnEnumerateUserFilesCompleteDelegates(false, *EOSUser);
                    return;
                }

                // The file list is now cached in EOS.
                This->TriggerOnEnumerateUserFilesCompleteDelegates(true, *EOSUser);
            }
        });
}

void FOnlineUserCloudInterfaceEOS::GetUserFileList(const FUniqueNetId &UserId, TArray<FCloudFileHeader> &UserFiles)
{
    if (UserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("GetUserFileList: User ID was not an EOS user ID"));
        UserFiles = TArray<FCloudFileHeader>();
        return;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared());

    EOS_PlayerDataStorage_GetFileMetadataCountOptions CountOpts = {};
    CountOpts.ApiVersion = EOS_PLAYERDATASTORAGE_GETFILEMETADATACOUNTOPTIONS_API_LATEST;
    CountOpts.LocalUserId = EOSUser->GetProductUserId();

    int32_t FileMetadataCount;
    EOS_EResult CountResult =
        EOS_PlayerDataStorage_GetFileMetadataCount(this->EOSPlayerDataStorage, &CountOpts, &FileMetadataCount);
    if (CountResult != EOS_EResult::EOS_Success)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("GetUserFileList: Unable to retrieve file count, got result: %s"),
            ANSI_TO_TCHAR(EOS_EResult_ToString(CountResult)));
        UserFiles = TArray<FCloudFileHeader>();
        return;
    }

    TArray<FCloudFileHeader> Results;
    for (int32_t i = 0; i < FileMetadataCount; i++)
    {
        EOS_PlayerDataStorage_CopyFileMetadataAtIndexOptions CopyOpts = {};
        CopyOpts.ApiVersion = EOS_PLAYERDATASTORAGE_COPYFILEMETADATAATINDEXOPTIONS_API_LATEST;
        CopyOpts.LocalUserId = EOSUser->GetProductUserId();
        CopyOpts.Index = i;

        EOS_PlayerDataStorage_FileMetadata *FileMetadata = nullptr;
        EOS_EResult CopyResult =
            EOS_PlayerDataStorage_CopyFileMetadataAtIndex(this->EOSPlayerDataStorage, &CopyOpts, &FileMetadata);
        if (CopyResult != EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("GetUserFileList: Unable to copy file metadata from index %d, got result: %s"),
                i,
                ANSI_TO_TCHAR(EOS_EResult_ToString(CopyResult)));
            UserFiles = TArray<FCloudFileHeader>();
            return;
        }

        FString Filename = EOSString_PlayerDataStorage_Filename::FromUtf8String(FileMetadata->Filename);
        FCloudFileHeader FileHeader = FCloudFileHeader(Filename, Filename, FileMetadata->FileSizeBytes);
        FileHeader.Hash = EOSString_PlayerDataStorage_MD5Hash::FromAnsiString(FileMetadata->MD5Hash);
        FileHeader.HashType = FName(TEXT("MD5"));
        EOS_PlayerDataStorage_FileMetadata_Release(FileMetadata);
        FileMetadata = nullptr;

        Results.Add(FileHeader);
    }

    UserFiles = Results;
}

bool FOnlineUserCloudInterfaceEOS::ReadUserFile(const FUniqueNetId &UserId, const FString &FileName)
{
    if (UserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("ReadUserFile: User ID was not an EOS user ID"));
        return false;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared());

    auto OpKey = FEOS_PlayerDataStorage_ReadFileOperation::GetOperationKey(EOSUser, FileName);
    if (this->ReadOperations.Contains(OpKey))
    {
        UE_LOG(LogEOS, Error, TEXT("ReadUserFile: File download already in progress for: %s"), *FileName);
        return false;
    }

    return FEOS_PlayerDataStorage_ReadFileOperation::StartOperation(this->AsShared(), EOSUser, FileName).IsValid();
}

bool FOnlineUserCloudInterfaceEOS::WriteUserFile(
    const FUniqueNetId &UserId,
    const FString &FileName,
    TArray<uint8> &FileContents,
    bool bCompressBeforeUpload)
{
    if (UserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("WriteUserFile: User ID was not an EOS user ID"));
        return false;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared());

    auto OpKey = FEOS_PlayerDataStorage_WriteFileOperation::GetOperationKey(EOSUser, FileName);
    if (this->WriteOperations.Contains(OpKey))
    {
        UE_LOG(LogEOS, Error, TEXT("WriteUserFile: File upload already in progress for: %s"), *FileName);
        return false;
    }

    return FEOS_PlayerDataStorage_WriteFileOperation::StartOperation(this->AsShared(), EOSUser, FileName, FileContents)
        .IsValid();
}

void FOnlineUserCloudInterfaceEOS::CancelWriteUserFile(const FUniqueNetId &UserId, const FString &FileName)
{
    if (UserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("CancelWriteUserFile: User ID was not an EOS user ID"));
        return;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared());

    auto OpKey = FEOS_PlayerDataStorage_WriteFileOperation::GetOperationKey(EOSUser, FileName);
    if (!this->WriteOperations.Contains(OpKey))
    {
        UE_LOG(LogEOS, Error, TEXT("CancelWriteUserFile: No write operation in progress for: %s"), *FileName);
        return;
    }

    this->WriteOperations[OpKey]->Cancel();
}

bool FOnlineUserCloudInterfaceEOS::DeleteUserFile(
    const FUniqueNetId &UserId,
    const FString &FileName,
    bool bShouldCloudDelete,
    bool bShouldLocallyDelete)
{
    if (UserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("DeleteUserFile: User ID was not an EOS user ID"));
        return false;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared());

    EOSString_PlayerDataStorage_Filename::Utf8String FilenameUtf8 =
        EOSString_PlayerDataStorage_Filename::ToUtf8String(FileName);

    EOS_PlayerDataStorage_DeleteFileOptions DeleteOpts = {};
    DeleteOpts.ApiVersion = EOS_PLAYERDATASTORAGE_DELETEFILEOPTIONS_API_LATEST;
    DeleteOpts.LocalUserId = EOSUser->GetProductUserId();
    DeleteOpts.Filename = FilenameUtf8.GetAsChar();

    EOSRunOperation<
        EOS_HPlayerDataStorage,
        EOS_PlayerDataStorage_DeleteFileOptions,
        EOS_PlayerDataStorage_DeleteFileCallbackInfo>(
        this->EOSPlayerDataStorage,
        &DeleteOpts,
        *EOS_PlayerDataStorage_DeleteFile,
        [WeakThis = GetWeakThis(this), EOSUser, FileName](const EOS_PlayerDataStorage_DeleteFileCallbackInfo *Info) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Info->ResultCode != EOS_EResult::EOS_Success)
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("DeleteUserFile: EOS_PlayerDataStorage_DeleteFile failed with error code %s"),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Info->ResultCode)));
                    This->TriggerOnDeleteUserFileCompleteDelegates(false, *EOSUser, FileName);
                    return;
                }

                // Delete the file from the local cache as well if we have it.
                This->ClearFile(*EOSUser, FileName);

                This->TriggerOnDeleteUserFileCompleteDelegates(true, *EOSUser, FileName);
            }
        });
    return true;
}

bool FOnlineUserCloudInterfaceEOS::RequestUsageInfo(const FUniqueNetId &UserId)
{
    UE_LOG(LogEOS, Error, TEXT("RequestUsageInfo: EOS does not support querying usage info for Player Data Storage"));
    return false;
}

void FOnlineUserCloudInterfaceEOS::DumpCloudState(const FUniqueNetId &UserId)
{
    // @todo: Dump a summary of in-progress operations and the download cache?
}

void FOnlineUserCloudInterfaceEOS::DumpCloudFileState(const FUniqueNetId &UserId, const FString &FileName)
{
    // @todo: Dump the current operation on the file (if there is one) and it's size in the download cache?
}

EOS_DISABLE_STRICT_WARNINGS