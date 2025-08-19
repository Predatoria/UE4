// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "DiscordGameSDK.h"
#include "Interfaces/OnlineIdentityInterface.h"

#if EOS_DISCORD_ENABLED

FORCEINLINE uint32 FUniqueNetIdRedpointDiscord_GetTypeHash(const int64 &S)
{
    return GetTypeHash(S);
}

class FUniqueNetIdRedpointDiscord : public FUniqueNetId
{
private:
    discord::UserId UserId;

public:
    FUniqueNetIdRedpointDiscord();
    FUniqueNetIdRedpointDiscord(discord::UserId Id);
    UE_NONCOPYABLE(FUniqueNetIdRedpointDiscord);
    discord::UserId GetUserId() const;
    virtual FName GetType() const override;
    virtual const uint8 *GetBytes() const override;
    virtual int32 GetSize() const override;
    virtual bool IsValid() const override;
    virtual FString ToString() const override;
    virtual FString ToDebugString() const override;

    static const TSharedRef<const FUniqueNetId> &EmptyId();

    friend uint32 GetTypeHash(const FUniqueNetIdRedpointDiscord &A)
    {
        return FUniqueNetIdRedpointDiscord_GetTypeHash((int64)A.UserId);
    }
    friend FArchive &operator<<(FArchive &Ar, FUniqueNetIdRedpointDiscord &OtherId)
    {
        return Ar << OtherId.UserId;
    }
};

#endif