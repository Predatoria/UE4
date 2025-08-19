// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "OnlineSubsystemRedpointEOS/Shared/PDSFriends/SerializedCachedFriend.h"
#include "OnlineSubsystemTypes.h"

EOS_ENABLE_STRICT_WARNINGS

class FOnlineFriendCached : public FOnlineFriend
{
private:
    TSharedRef<const FUniqueNetId> UserId;
    FSerializedCachedFriend CacheData;
    FOnlineUserPresence DefaultPresence;

public:
    FOnlineFriendCached(const FName &InSubsystemName, const FSerializedCachedFriend &InCacheData);
    virtual TSharedRef<const FUniqueNetId> GetUserId() const override;
    virtual FString GetRealName() const override;
    virtual FString GetDisplayName(const FString &Platform = FString()) const override;
    virtual bool GetUserAttribute(const FString &AttrName, FString &OutAttrValue) const override;
    virtual EInviteStatus::Type GetInviteStatus() const override;
    virtual const class FOnlineUserPresence &GetPresence() const override;
};

EOS_DISABLE_STRICT_WARNINGS