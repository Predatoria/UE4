// Copyright June Rhodes. All Rights Reserved.

#include "UniqueNetIdRedpointDiscord.h"

#include "OnlineSubsystemRedpointDiscord.h"

#if EOS_DISCORD_ENABLED

FUniqueNetIdRedpointDiscord::FUniqueNetIdRedpointDiscord()
    : UserId(0)
{
}

FUniqueNetIdRedpointDiscord::FUniqueNetIdRedpointDiscord(discord::UserId Id)
    : UserId(Id)
{
}

discord::UserId FUniqueNetIdRedpointDiscord::GetUserId() const
{
    return this->UserId;
}

FName FUniqueNetIdRedpointDiscord::GetType() const
{
    return REDPOINT_DISCORD_SUBSYSTEM;
}

const uint8 *FUniqueNetIdRedpointDiscord::GetBytes() const
{
    return (uint8 *)&UserId;
}

int32 FUniqueNetIdRedpointDiscord::GetSize() const
{
    return sizeof(discord::UserId);
}

bool FUniqueNetIdRedpointDiscord::IsValid() const
{
    return UserId != 0;
}

FString FUniqueNetIdRedpointDiscord::ToString() const
{
    return FString::Printf(TEXT("%llu"), UserId);
}

FString FUniqueNetIdRedpointDiscord::ToDebugString() const
{
    return ToString();
}

const TSharedRef<const FUniqueNetId> &FUniqueNetIdRedpointDiscord::EmptyId()
{
    static const TSharedRef<const FUniqueNetId> EmptyId(MakeShared<FUniqueNetIdRedpointDiscord>());
    return EmptyId;
}

#endif