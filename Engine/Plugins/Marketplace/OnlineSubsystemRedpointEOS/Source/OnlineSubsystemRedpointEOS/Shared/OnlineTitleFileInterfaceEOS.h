// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EOSCommon.h"
#include "Interfaces/OnlineTitleFileInterface.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineIdentityInterfaceEOS.h"

EOS_ENABLE_STRICT_WARNINGS

#if EOS_VERSION_AT_LEAST(1, 8, 0)

// Default chunk size: 128kb per tick
// @todo: Make this configurable.
#define EOS_CHUNK_LENGTH_BYTES (32 * 4096)

class ONLINESUBSYSTEMREDPOINTEOS_API FOnlineTitleFileInterfaceEOS
    : public IOnlineTitleFile,
      public TSharedFromThis<FOnlineTitleFileInterfaceEOS, ESPMode::ThreadSafe>
{
    friend class FEOS_TitleStorage_ReadFileOperation;

private:
    typedef FString FOperationId;
    typedef FString FFilename;

    EOS_HPlatform EOSPlatform;
    EOS_HTitleStorage EOSTitleStorage;
    TSharedPtr<FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> Identity;
    TMap<FOperationId, TSharedPtr<class FEOS_TitleStorage_ReadFileOperation>> ReadOperations;
    TMap<FFilename, TArray<uint8>> DownloadCacheByFilename;

    void SubmitDownloadedFileToCache(const FString &InFileName, const TArray<uint8> &InFileContents);
    EOS_ProductUserId GetLocalUserId() const;

public:
    FOnlineTitleFileInterfaceEOS(
        EOS_HPlatform InPlatform,
        const TSharedRef<FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> &InIdentity);
    UE_NONCOPYABLE(FOnlineTitleFileInterfaceEOS);
    virtual ~FOnlineTitleFileInterfaceEOS();

    virtual bool GetFileContents(const FString &FileName, TArray<uint8> &FileContents) override;
    virtual bool ClearFiles() override;
    virtual bool ClearFile(const FString &FileName) override;
    virtual void DeleteCachedFiles(bool bSkipEnumerated) override;
    virtual bool EnumerateFiles(const FPagedQuery &Page = FPagedQuery()) override;
    virtual void GetFileList(TArray<FCloudFileHeader> &UserFiles) override;
    virtual bool ReadFile(const FString &FileName) override;
};

#endif

EOS_DISABLE_STRICT_WARNINGS