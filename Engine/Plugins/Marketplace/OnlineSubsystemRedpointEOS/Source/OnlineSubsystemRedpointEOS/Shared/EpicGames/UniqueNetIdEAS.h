// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FUniqueNetIdEAS : public FUniqueNetId
{
private:
    EOS_EpicAccountId EpicAccountId;
    uint8 *DataBytes;
    int32 DataBytesSize;

private:
    FUniqueNetIdEAS(EOS_EpicAccountId InInternalEpicAccountId, bool bUnused);

    static TSharedRef<const FUniqueNetIdEAS> MakeInvalidId();

public:
    static const TSharedRef<const FUniqueNetIdEAS> &InvalidId();
    FUniqueNetIdEAS(EOS_EpicAccountId InProductUserId);
    virtual bool Compare(const FUniqueNetId &Other) const override;
    // This operation isn't safe, because the copy won't have been created through TSharedPtr, and thus
    // you won't be able to call AsShared on it. Prevent any accidental copies by removing the copy constructor.
    UE_NONCOPYABLE(FUniqueNetIdEAS);
    ~FUniqueNetIdEAS();
    /** Get the Epic account ID. Do not compare this with null for validation; instead call HasValidProductUserId. */
    EOS_EpicAccountId GetEpicAccountId() const;
    bool HasValidEpicAccountId() const;
    virtual FName GetType() const override;
    virtual const uint8 *GetBytes() const override;
    virtual int32 GetSize() const override;
    virtual bool IsValid() const override;
    virtual FString ToString() const override;
    virtual FString ToDebugString() const override;
    static TSharedPtr<const FUniqueNetId> ParseFromString(const FString &EpicAccountIdStr);
    friend uint32 GetTypeHash(const FUniqueNetIdEAS &A);
    friend FArchive &operator<<(FArchive &Ar, FUniqueNetIdEAS &OtherId);
    FString GetEpicAccountIdString(bool Debug = false) const;
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION