// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EOSCommon.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystemRedpointEOS/Shared/UserIdMap.h"

EOS_ENABLE_STRICT_WARNINGS

#define EOS_DEDICATED_SERVER_ID TEXT("DEDICATED_SERVER")

class ONLINESUBSYSTEMREDPOINTEOS_API FUniqueNetIdEOS : public FUniqueNetId
{
private:
    EOS_ProductUserId ProductUserId;
    uint8 *DataBytes;
    int32 DataBytesSize;
    bool bIsDedicatedServer;

private:
    FUniqueNetIdEOS(EOS_ProductUserId InInternalProductUserId, bool bInIsDedicatedServer);

    static TSharedRef<const FUniqueNetIdEOS> MakeInvalidId();
    static TSharedRef<const FUniqueNetIdEOS> MakeDedicatedServerId();

public:
    static const TSharedRef<const FUniqueNetIdEOS> &InvalidId();
    static const TSharedRef<const FUniqueNetIdEOS> &DedicatedServerId();
    FUniqueNetIdEOS(EOS_ProductUserId InProductUserId);
    virtual bool Compare(const FUniqueNetId &Other) const override;
    // This operation isn't safe, because the copy won't have been created through TSharedPtr, and thus
    // you won't be able to call AsShared on it. Prevent any accidental copies by removing the copy constructor.
    UE_NONCOPYABLE(FUniqueNetIdEOS);
    ~FUniqueNetIdEOS();
    /** Get the product user ID. Do not compare this with null for validation; instead call HasValidProductUserId. */
    EOS_ProductUserId GetProductUserId() const;
    bool HasValidProductUserId() const;
    virtual FName GetType() const override;
    virtual const uint8 *GetBytes() const override;
    virtual int32 GetSize() const override;
    virtual bool IsValid() const override;
    virtual FString ToString() const override;
    virtual FString ToDebugString() const override;
    static TSharedPtr<const FUniqueNetIdEOS> ParseFromString(const FString &ProductUserIdStr);
    friend uint32 GetTypeHash(const FUniqueNetIdEOS &A);
    friend FArchive &operator<<(FArchive &Ar, FUniqueNetIdEOS &OtherId);
    FString GetProductUserIdString(bool Debug = false) const;
    bool IsDedicatedServer() const;
};

EOS_DISABLE_STRICT_WARNINGS