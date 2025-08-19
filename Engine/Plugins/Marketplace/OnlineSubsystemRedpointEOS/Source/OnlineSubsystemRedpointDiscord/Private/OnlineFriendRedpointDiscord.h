// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if EOS_DISCORD_ENABLED

#include "DiscordGameSDK.h"
#include "OnlineUserPresenceRedpointDiscord.h"
#include "UniqueNetIdRedpointDiscord.h"

class FOnlineFriendRedpointDiscord : public FOnlineFriend
{
private:
    TSharedRef<const FUniqueNetIdRedpointDiscord> UserId;
    TSharedRef<FOnlineUserPresenceRedpointDiscord> PresenceInfo;
    discord::Relationship RelationshipInfo;

public:
    FOnlineFriendRedpointDiscord(
        TSharedRef<const FUniqueNetIdRedpointDiscord> InUserId,
        TSharedRef<FOnlineUserPresenceRedpointDiscord> InPresenceInfo,
        const discord::Relationship &InRelationshipInfo);
    virtual ~FOnlineFriendRedpointDiscord();

    void UpdateFromRelationship(const discord::Relationship &NewRelationship);

    virtual TSharedRef<const FUniqueNetId> GetUserId() const override;
    virtual FString GetRealName() const override;
    virtual FString GetDisplayName(const FString &Platform = FString()) const override;
    virtual bool GetUserAttribute(const FString &AttrName, FString &OutAttrValue) const override;
    virtual EInviteStatus::Type GetInviteStatus() const override;
    virtual const class FOnlineUserPresence &GetPresence() const override;
};

#endif