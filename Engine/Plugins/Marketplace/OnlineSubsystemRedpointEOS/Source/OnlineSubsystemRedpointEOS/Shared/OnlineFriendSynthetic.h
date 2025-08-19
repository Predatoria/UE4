// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineUserEOS.h"
#include "OnlineSubsystemTypes.h"

EOS_ENABLE_STRICT_WARNINGS

class FOnlineFriendSynthetic : public FOnlineFriend
{
private:
    TSharedPtr<FOnlineUserEOS> UserEOS;
    TMap<FName, TSharedPtr<FOnlineFriend>> WrappedFriends;
    TSharedPtr<FOnlineFriend> PreferredFriend;
    FString PreferredSubsystemName;
    TWeakPtr<class FFriendDatabase> FriendDatabase;
    FOnlineUserPresence NullPresence;

public:
    FOnlineFriendSynthetic(
        TSharedPtr<FOnlineUserEOS> InUserEOS,
        const TMap<FName, TSharedPtr<FOnlineFriend>> &InWrappedFriends,
        TWeakPtr<class FFriendDatabase> InFriendDatabase);

    virtual TSharedRef<const FUniqueNetId> GetUserId() const override;
    virtual FString GetRealName() const override;
    virtual FString GetDisplayName(const FString &Platform = FString()) const override;
    virtual bool GetUserAttribute(const FString &AttrName, FString &OutAttrValue) const override;
    virtual EInviteStatus::Type GetInviteStatus() const override;
    virtual const class FOnlineUserPresence &GetPresence() const override;

    const TMap<FName, TSharedPtr<FOnlineFriend>> &GetWrappedFriends() const
    {
        return this->WrappedFriends;
    }
};

EOS_DISABLE_STRICT_WARNINGS