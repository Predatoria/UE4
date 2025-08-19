// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineUserPresenceEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/UniqueNetIdEAS.h"
#include "OnlineSubsystemTypes.h"

EOS_ENABLE_STRICT_WARNINGS

class FOnlineFriendEAS : public FOnlineFriend
{
private:
    TSharedRef<const FUniqueNetIdEAS> UserId;
    TSharedRef<const FOnlineUserPresenceEAS> PresenceInfo;
    EOS_UserInfo *UserInfo;

public:
    FOnlineFriendEAS(
        TSharedRef<const FUniqueNetIdEAS> InUserId,
        TSharedRef<const FOnlineUserPresenceEAS> InPresenceInfo,
        EOS_UserInfo *InUserInfo);
    virtual ~FOnlineFriendEAS();

    virtual TSharedRef<const FUniqueNetId> GetUserId() const override;
    virtual FString GetRealName() const override;
    virtual FString GetDisplayName(const FString &Platform = FString()) const override;
    virtual bool GetUserAttribute(const FString &AttrName, FString &OutAttrValue) const override;
    virtual EInviteStatus::Type GetInviteStatus() const override;
    virtual const class FOnlineUserPresence &GetPresence() const override;
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION