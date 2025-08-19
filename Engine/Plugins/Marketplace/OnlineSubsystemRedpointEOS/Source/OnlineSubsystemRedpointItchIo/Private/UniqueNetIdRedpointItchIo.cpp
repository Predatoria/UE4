// Copyright June Rhodes. All Rights Reserved.

#include "UniqueNetIdRedpointItchIo.h"

#include "OnlineSubsystemRedpointItchIoConstants.h"

#if EOS_ITCH_IO_ENABLED

FUniqueNetIdRedpointItchIo::FUniqueNetIdRedpointItchIo()
    : UserId(0)
{
}

FUniqueNetIdRedpointItchIo::FUniqueNetIdRedpointItchIo(int32 Id)
    : UserId(Id)
{
}

int32 FUniqueNetIdRedpointItchIo::GetUserId() const
{
    return this->UserId;
}

FName FUniqueNetIdRedpointItchIo::GetType() const
{
    return REDPOINT_ITCH_IO_SUBSYSTEM;
}

const uint8 *FUniqueNetIdRedpointItchIo::GetBytes() const
{
    return (uint8 *)&UserId;
}

int32 FUniqueNetIdRedpointItchIo::GetSize() const
{
    return sizeof(int32);
}

bool FUniqueNetIdRedpointItchIo::IsValid() const
{
    return UserId != 0;
}

FString FUniqueNetIdRedpointItchIo::ToString() const
{
    return FString::Printf(TEXT("%d"), UserId);
}

FString FUniqueNetIdRedpointItchIo::ToDebugString() const
{
    return ToString();
}

const TSharedRef<const FUniqueNetId> &FUniqueNetIdRedpointItchIo::EmptyId()
{
    static const TSharedRef<const FUniqueNetId> EmptyId(MakeShared<FUniqueNetIdRedpointItchIo>());
    return EmptyId;
}

#endif