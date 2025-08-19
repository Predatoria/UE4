// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EOSCommon.h"
#include "Interfaces/OnlineUserCloudInterface.h"
#include "UniqueNetIdEOS.h"

EOS_ENABLE_STRICT_WARNINGS

// Default chunk size: 128kb per tick
// @todo: Make this configurable.
#define EOS_CHUNK_LENGTH_BYTES (32 * 4096)

class ONLINESUBSYSTEMREDPOINTEOS_API FOnlineUserCloudInterfaceEOS
    : public IOnlineUserCloud,
      public TSharedFromThis<FOnlineUserCloudInterfaceEOS, ESPMode::ThreadSafe>
{
    friend class FEOS_PlayerDataStorage_WriteFileOperation;
    friend class FEOS_PlayerDataStorage_ReadFileOperation;

private:
    typedef FString FOperationId;
    typedef FString FFilename;

    EOS_HPlatform EOSPlatform;
    EOS_HPlayerDataStorage EOSPlayerDataStorage;
    TMap<FOperationId, TSharedPtr<class FEOS_PlayerDataStorage_WriteFileOperation>> WriteOperations;
    TMap<FOperationId, TSharedPtr<class FEOS_PlayerDataStorage_ReadFileOperation>> ReadOperations;
    TUserIdMap<TMap<FFilename, TArray<uint8>>> DownloadCacheByUserThenFilename;

    void SubmitDownloadedFileToCache(
        const TSharedRef<const class FUniqueNetIdEOS> &InUserId,
        const FString &InFileName,
        const TArray<uint8> &InFileContents);

public:
    FOnlineUserCloudInterfaceEOS(EOS_HPlatform InPlatform);
    UE_NONCOPYABLE(FOnlineUserCloudInterfaceEOS);
    virtual ~FOnlineUserCloudInterfaceEOS();

    virtual bool GetFileContents(const FUniqueNetId &UserId, const FString &FileName, TArray<uint8> &FileContents)
        override;
    virtual bool ClearFiles(const FUniqueNetId &UserId) override;
    virtual bool ClearFile(const FUniqueNetId &UserId, const FString &FileName) override;
    virtual void EnumerateUserFiles(const FUniqueNetId &UserId) override;
    virtual void GetUserFileList(const FUniqueNetId &UserId, TArray<FCloudFileHeader> &UserFiles) override;
    virtual bool ReadUserFile(const FUniqueNetId &UserId, const FString &FileName) override;
    virtual bool WriteUserFile(
        const FUniqueNetId &UserId,
        const FString &FileName,
        TArray<uint8> &FileContents,
        bool bCompressBeforeUpload = false) override;
    virtual void CancelWriteUserFile(const FUniqueNetId &UserId, const FString &FileName) override;
    virtual bool DeleteUserFile(
        const FUniqueNetId &UserId,
        const FString &FileName,
        bool bShouldCloudDelete,
        bool bShouldLocallyDelete) override;
    virtual bool RequestUsageInfo(const FUniqueNetId &UserId) override;
    virtual void DumpCloudState(const FUniqueNetId &UserId) override;
    virtual void DumpCloudFileState(const FUniqueNetId &UserId, const FString &FileName) override;
};

EOS_DISABLE_STRICT_WARNINGS