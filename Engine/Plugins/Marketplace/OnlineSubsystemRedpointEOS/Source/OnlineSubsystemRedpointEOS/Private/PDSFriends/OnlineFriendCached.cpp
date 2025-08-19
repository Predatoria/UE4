// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/PDSFriends/OnlineFriendCached.h"
#include "OnlineSubsystemRedpointEOS/Shared/PDSFriends/UniqueNetIdCached.h"

EOS_ENABLE_STRICT_WARNINGS

FOnlineFriendCached::FOnlineFriendCached(const FName &InSubsystemName, const FSerializedCachedFriend &InCacheData)
    : UserId(MakeShared<FUniqueNetIdCached>(
          InSubsystemName,
          InCacheData.AccountId,
          InCacheData.AccountIdBytes,
          InCacheData.AccountIdTypeHash))
    , CacheData(InCacheData)
    , DefaultPresence()
{
}

TSharedRef<const FUniqueNetId> FOnlineFriendCached::GetUserId() const
{
    return this->UserId;
}

FString FOnlineFriendCached::GetRealName() const
{
    return this->CacheData.AccountRealName;
}

FString FOnlineFriendCached::GetDisplayName(const FString &Platform) const
{
    return this->CacheData.AccountDisplayName;
}

bool FOnlineFriendCached::GetUserAttribute(const FString &AttrName, FString &OutAttrValue) const
{
    if (AttrName == TEXT("isCachedFriend"))
    {
        OutAttrValue = TEXT("true");
        return true;
    }

    if (AttrName == TEXT("cachedAvatarUrl"))
    {
        OutAttrValue = this->CacheData.AccountAvatarUrl;
        return !OutAttrValue.IsEmpty();
    }

    // We don't cache any attributes, because there's no way to enumerate them all
    // when syncing to the friend database.
    return false;
}

EInviteStatus::Type FOnlineFriendCached::GetInviteStatus() const
{
    return EInviteStatus::Unknown;
}

const class FOnlineUserPresence &FOnlineFriendCached::GetPresence() const
{
    return this->DefaultPresence;
}

EOS_DISABLE_STRICT_WARNINGS