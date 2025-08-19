// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Interfaces/OnlineIdentityInterface.h"

#if EOS_ITCH_IO_ENABLED

FORCEINLINE uint32 FUniqueNetIdRedpointItchIo_GetTypeHash(const int64 &S)
{
    return GetTypeHash(S);
}

class FUniqueNetIdRedpointItchIo : public FUniqueNetId
{
private:
    int32 UserId;

public:
    FUniqueNetIdRedpointItchIo();
    FUniqueNetIdRedpointItchIo(int32 Id);
    UE_NONCOPYABLE(FUniqueNetIdRedpointItchIo);
    int32 GetUserId() const;
    virtual FName GetType() const override;
    virtual const uint8 *GetBytes() const override;
    virtual int32 GetSize() const override;
    virtual bool IsValid() const override;
    virtual FString ToString() const override;
    virtual FString ToDebugString() const override;

    static const TSharedRef<const FUniqueNetId> &EmptyId();

    friend uint32 GetTypeHash(const FUniqueNetIdRedpointItchIo &A)
    {
        return FUniqueNetIdRedpointItchIo_GetTypeHash((int64)A.UserId);
    }
    friend FArchive &operator<<(FArchive &Ar, FUniqueNetIdRedpointItchIo &OtherId)
    {
        return Ar << OtherId.UserId;
    }
};

#endif