// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineIdentityInterface.h"

EOS_ENABLE_STRICT_WARNINGS

#define EOS_DEDICATED_SERVER_ID TEXT("DEDICATED_SERVER")

class ONLINESUBSYSTEMREDPOINTEOS_API FUniqueNetIdRedpointNull : public FUniqueNetId
{
private:
    FString ProductUserId;
    uint8 *DataBytes;
    int32 DataBytesSize;
    bool bIsDedicatedServer;

private:
    FUniqueNetIdRedpointNull(const FString &InProductUserId, bool bInIsDedicatedServer);

    static TSharedRef<const FUniqueNetIdRedpointNull> MakeInvalidId();
    static TSharedRef<const FUniqueNetIdRedpointNull> MakeDedicatedServerId();

public:
    static const TSharedRef<const FUniqueNetIdRedpointNull> &InvalidId();
    static const TSharedRef<const FUniqueNetIdRedpointNull> &DedicatedServerId();
    virtual bool Compare(const FUniqueNetId &Other) const override;
    // This operation isn't safe, because the copy won't have been created through TSharedPtr, and thus
    // you won't be able to call AsShared on it. Prevent any accidental copies by removing the copy constructor.
    UE_NONCOPYABLE(FUniqueNetIdRedpointNull);
    ~FUniqueNetIdRedpointNull();
    virtual FName GetType() const override;
    virtual const uint8 *GetBytes() const override;
    virtual int32 GetSize() const override;
    virtual bool IsValid() const override;
    virtual FString ToString() const override;
    virtual FString ToDebugString() const override;
    static TSharedPtr<const FUniqueNetIdRedpointNull> ParseFromString(const FString &ProductUserIdStr);
    friend uint32 GetTypeHash(const FUniqueNetIdRedpointNull &A);
    friend FArchive &operator<<(FArchive &Ar, FUniqueNetIdRedpointNull &OtherId);
    FString GetProductUserIdString(bool Debug = false) const;
    bool IsDedicatedServer() const;
};

EOS_DISABLE_STRICT_WARNINGS