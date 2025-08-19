// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineIdentityInterface.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FUniqueNetIdCached : public FUniqueNetId
{
private:
    FName Type;
    FString StrValue;
    uint8 *DataBytes;
    int32 DataBytesSize;
    uint32 TypeHash;

public:
    FUniqueNetIdCached(
        const FName &InType,
        const FString &InValue,
        const TArray<uint8> &InBytes,
        const uint32 &InTypeHash);
    // This operation isn't safe, because the copy won't have been created through TSharedPtr, and thus
    // you won't be able to call AsShared on it. Prevent any accidental copies by removing the copy constructor.
    UE_NONCOPYABLE(FUniqueNetIdCached);
    ~FUniqueNetIdCached();

    virtual bool Compare(const FUniqueNetId &Other) const override;
    virtual FName GetType() const override;
    virtual const uint8 *GetBytes() const override;
    virtual int32 GetSize() const override;
    virtual bool IsValid() const override;
    virtual FString ToString() const override;
    virtual FString ToDebugString() const override;
    friend uint32 GetTypeHash(const FUniqueNetIdCached &A);
};

EOS_DISABLE_STRICT_WARNINGS