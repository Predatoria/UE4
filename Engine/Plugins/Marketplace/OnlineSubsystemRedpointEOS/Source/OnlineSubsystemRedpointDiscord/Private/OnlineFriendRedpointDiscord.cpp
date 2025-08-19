// Copyright June Rhodes. All Rights Reserved.

#include "OnlineFriendRedpointDiscord.h"

#if EOS_DISCORD_ENABLED

FOnlineFriendRedpointDiscord::FOnlineFriendRedpointDiscord(
    TSharedRef<const FUniqueNetIdRedpointDiscord> InUserId,
    TSharedRef<FOnlineUserPresenceRedpointDiscord> InPresenceInfo,
    const discord::Relationship &InRelationshipInfo)
    : UserId(MoveTemp(InUserId))
    , PresenceInfo(MoveTemp(InPresenceInfo))
    , RelationshipInfo(InRelationshipInfo)
{
}

FOnlineFriendRedpointDiscord::~FOnlineFriendRedpointDiscord()
{
}

void FOnlineFriendRedpointDiscord::UpdateFromRelationship(const discord::Relationship &NewRelationship)
{
    // Copy the new data across.
    this->RelationshipInfo = NewRelationship;

    // Also, update presence.
    this->PresenceInfo->UpdateFromPresence(NewRelationship.GetPresence());
}

TSharedRef<const FUniqueNetId> FOnlineFriendRedpointDiscord::GetUserId() const
{
    return this->UserId;
}

FString FOnlineFriendRedpointDiscord::GetRealName() const
{
    return UTF8_TO_TCHAR(this->RelationshipInfo.GetUser().GetUsername());
}

FString FOnlineFriendRedpointDiscord::GetDisplayName(const FString &Platform) const
{
    return this->GetRealName();
}

bool FOnlineFriendRedpointDiscord::GetUserAttribute(const FString &AttrName, FString &OutAttrValue) const
{
    return false;
}

EInviteStatus::Type FOnlineFriendRedpointDiscord::GetInviteStatus() const
{
    switch (this->RelationshipInfo.GetType())
    {
    case discord::RelationshipType::Friend:
        return EInviteStatus::Accepted;
    case discord::RelationshipType::Blocked:
        return EInviteStatus::Blocked;
    case discord::RelationshipType::PendingIncoming:
        return EInviteStatus::PendingInbound;
    case discord::RelationshipType::PendingOutgoing:
        return EInviteStatus::PendingOutbound;
    case discord::RelationshipType::Implicit:
        return EInviteStatus::Suggested;
    case discord::RelationshipType::None:
    default:
        return EInviteStatus::Unknown;
    }
    return EInviteStatus::Accepted;
}

const class FOnlineUserPresence &FOnlineFriendRedpointDiscord::GetPresence() const
{
    return *this->PresenceInfo;
}

#endif