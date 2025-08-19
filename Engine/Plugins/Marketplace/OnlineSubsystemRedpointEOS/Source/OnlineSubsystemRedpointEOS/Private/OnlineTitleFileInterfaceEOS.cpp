// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/OnlineTitleFileInterfaceEOS.h"

#if EOS_VERSION_AT_LEAST(1, 8, 0)

#include "OnlineSubsystemRedpointEOS/Private/FileTransfer/TitleStorage_ReadFileOperation.h"
#include "OnlineSubsystemRedpointEOS/Public/OnlineTitleFilePagedQueryEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

void FOnlineTitleFileInterfaceEOS::SubmitDownloadedFileToCache(
    const FString &InFileName,
    const TArray<uint8> &InFileContents)
{
    this->DownloadCacheByFilename.Add(InFileName, InFileContents);
}

FOnlineTitleFileInterfaceEOS::FOnlineTitleFileInterfaceEOS(
    EOS_HPlatform InPlatform,
    const TSharedRef<FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> &InIdentity)
    : EOSPlatform(InPlatform)
    , EOSTitleStorage(EOS_Platform_GetTitleStorageInterface(this->EOSPlatform))
    , Identity(InIdentity)
    , ReadOperations()
    , DownloadCacheByFilename()
{
}

FOnlineTitleFileInterfaceEOS::~FOnlineTitleFileInterfaceEOS()
{
}

EOS_ProductUserId FOnlineTitleFileInterfaceEOS::GetLocalUserId() const
{
    // Use a logged in user if we have one, or default to anonymous otherwise. Different games will have different
    // client policies for Title File, and we want to use a user if available in case the game does not permit anonymous
    // usage of Title File.

    if (this->Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
    {
        TSharedPtr<const FUniqueNetIdEOS> IdEOS =
            StaticCastSharedPtr<const FUniqueNetIdEOS>(this->Identity->GetUniquePlayerId(0));
        return IdEOS->GetProductUserId();
    }

    return nullptr;
}

bool FOnlineTitleFileInterfaceEOS::GetFileContents(const FString &FileName, TArray<uint8> &FileContents)
{
    if (!this->DownloadCacheByFilename.Contains(FileName))
    {
        return false;
    }
    FileContents = this->DownloadCacheByFilename[FileName];
    return true;
}

bool FOnlineTitleFileInterfaceEOS::ClearFiles()
{
    this->DownloadCacheByFilename.Empty();
    return true;
}

bool FOnlineTitleFileInterfaceEOS::ClearFile(const FString &FileName)
{
    if (!this->DownloadCacheByFilename.Contains(FileName))
    {
        return true;
    }
    this->DownloadCacheByFilename.Remove(FileName);
    return true;
}

void FOnlineTitleFileInterfaceEOS::DeleteCachedFiles(bool bSkipEnumerated)
{
    EOS_TitleStorage_DeleteCacheOptions DeleteCacheOpts = {};
    DeleteCacheOpts.ApiVersion = EOS_TITLESTORAGE_DELETECACHEOPTIONS_API_LATEST;
    DeleteCacheOpts.LocalUserId = this->GetLocalUserId();

    EOSRunOperation<EOS_HTitleStorage, EOS_TitleStorage_DeleteCacheOptions, EOS_TitleStorage_DeleteCacheCallbackInfo>(
        this->EOSTitleStorage,
        &DeleteCacheOpts,
        EOS_TitleStorage_DeleteCache,
        [WeakThis = GetWeakThis(this)](const EOS_TitleStorage_DeleteCacheCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Data->ResultCode != EOS_EResult::EOS_Success)
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("DeleteCachedFiles: Failed to deleted cached files, got result: %s"),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
                }
            }
        });
}

bool FOnlineTitleFileInterfaceEOS::EnumerateFiles(const FPagedQuery &Page)
{
    if (Page.Start != 0 || Page.Count != -1)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("EnumerateFiles: Must provide default FPagedQuery with Start set to 0 and Count set to -1; EOS does "
                 "not support pagination for "
                 "QueryFileList."));
        return false;
    }

    // If you want to customize the tags that are queryed over, pass in FPagedQueryEOS (located
    // in the "OnlineTitleFilePagedQueryEOS.h" header instead of the default FPagedQuery class.
    TArray<FString> Tags;
    if (FPagedQueryEOS::IsFPagedQueryEOS(Page))
    {
        Tags = ((const FPagedQueryEOS &)Page).Tags;
    }
    if (Tags.Num() == 0)
    {
        Tags.Add(TEXT("Default"));
    }

    EOS_TitleStorage_QueryFileListOptions Opts = {};
    Opts.ApiVersion = EOS_TITLESTORAGE_QUERYFILELISTOPTIONS_API_LATEST;
    Opts.LocalUserId = this->GetLocalUserId();
    Opts.ListOfTags = nullptr;
    Opts.ListOfTagsCount = 0;
    EOSString_TitleStorage_Tag::AllocateToCharList(Tags, Opts.ListOfTagsCount, (const char **&)Opts.ListOfTags);

    EOSRunOperation<
        EOS_HTitleStorage,
        EOS_TitleStorage_QueryFileListOptions,
        EOS_TitleStorage_QueryFileListCallbackInfo>(
        this->EOSTitleStorage,
        &Opts,
        EOS_TitleStorage_QueryFileList,
        [WeakThis = GetWeakThis(this), Opts](const EOS_TitleStorage_QueryFileListCallbackInfo *Data) {
            EOSString_TitleStorage_Tag::FreeFromCharListConst(
                Opts.ListOfTagsCount,
                (const char **const)Opts.ListOfTags);

            if (auto This = PinWeakThis(WeakThis))
            {
                if (Data->ResultCode != EOS_EResult::EOS_Success)
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("EnumerateFiles: Unable to query file list, got error: %s"),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
                    This->TriggerOnEnumerateFilesCompleteDelegates(
                        false,
                        FString::Printf(
                            TEXT("EnumerateFiles: Unable to query file list, got error: %s"),
                            ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode))));
                    return;
                }

                // The file list is now cached in EOS.
                This->TriggerOnEnumerateFilesCompleteDelegates(true, TEXT(""));
            }
        });
    return true;
}

void FOnlineTitleFileInterfaceEOS::GetFileList(TArray<FCloudFileHeader> &UserFiles)
{
    EOS_TitleStorage_GetFileMetadataCountOptions CountOpts = {};
    CountOpts.ApiVersion = EOS_TITLESTORAGE_GETFILEMETADATACOUNTOPTIONS_API_LATEST;
    CountOpts.LocalUserId = this->GetLocalUserId();

    int32_t FileMetadataCount = EOS_TitleStorage_GetFileMetadataCount(this->EOSTitleStorage, &CountOpts);
    TArray<FCloudFileHeader> Results;
    for (int32_t i = 0; i < FileMetadataCount; i++)
    {
        EOS_TitleStorage_CopyFileMetadataAtIndexOptions CopyOpts = {};
        CopyOpts.ApiVersion = EOS_TITLESTORAGE_COPYFILEMETADATAATINDEXOPTIONS_API_LATEST;
        CopyOpts.LocalUserId = this->GetLocalUserId();
        CopyOpts.Index = i;

        EOS_TitleStorage_FileMetadata *FileMetadata = nullptr;
        EOS_EResult CopyResult =
            EOS_TitleStorage_CopyFileMetadataAtIndex(this->EOSTitleStorage, &CopyOpts, &FileMetadata);
        if (CopyResult != EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("GetFileList: Unable to copy file metadata from index %d, got result: %s"),
                i,
                ANSI_TO_TCHAR(EOS_EResult_ToString(CopyResult)));
            UserFiles = TArray<FCloudFileHeader>();
            return;
        }

        FString Filename = EOSString_PlayerDataStorage_Filename::FromUtf8String(FileMetadata->Filename);
        FCloudFileHeader FileHeader = FCloudFileHeader(Filename, Filename, FileMetadata->FileSizeBytes);
        FileHeader.Hash = EOSString_PlayerDataStorage_MD5Hash::FromAnsiString(FileMetadata->MD5Hash);
        FileHeader.HashType = FName(TEXT("MD5"));
        EOS_TitleStorage_FileMetadata_Release(FileMetadata);
        FileMetadata = nullptr;

        Results.Add(FileHeader);
    }

    UserFiles = Results;
}

bool FOnlineTitleFileInterfaceEOS::ReadFile(const FString &FileName)
{
    if (this->ReadOperations.Contains(FileName))
    {
        UE_LOG(LogEOS, Error, TEXT("ReadFile: File download already in progress for: %s"), *FileName);
        return false;
    }

    if (this->Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
    {
        FEOS_TitleStorage_ReadFileOperation::StartOperation(
            this->AsShared(),
            StaticCastSharedPtr<const FUniqueNetIdEOS>(this->Identity->GetUniquePlayerId(0)),
            FileName);
    }
    else
    {
        FEOS_TitleStorage_ReadFileOperation::StartOperation(this->AsShared(), nullptr, FileName);
    }
    return true;
}

EOS_DISABLE_STRICT_WARNINGS

#endif