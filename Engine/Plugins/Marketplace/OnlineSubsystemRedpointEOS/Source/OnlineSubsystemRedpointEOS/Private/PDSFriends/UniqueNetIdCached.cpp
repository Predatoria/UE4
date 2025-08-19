// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/PDSFriends/UniqueNetIdCached.h"

EOS_ENABLE_STRICT_WARNINGS

FUniqueNetIdCached::FUniqueNetIdCached(
    const FName &InType,
    const FString &InValue,
    const TArray<uint8> &InBytes,
    const uint32 &InTypeHash)
    : Type(InType)
    , StrValue(InValue)
    , DataBytes(nullptr)
    , DataBytesSize(0)
    , TypeHash(InTypeHash)
{
    this->DataBytes = (uint8 *)FMemory::Malloc(InBytes.Num());
    this->DataBytesSize = InBytes.Num();
    FMemory::Memcpy(this->DataBytes, InBytes.GetData(), InBytes.Num());
}

FUniqueNetIdCached::~FUniqueNetIdCached()
{
    FMemory::Free(this->DataBytes);
}

bool FUniqueNetIdCached::Compare(const FUniqueNetId &Other) const
{
    if (Other.GetType() != GetType())
    {
        return false;
    }

    return (GetSize() == Other.GetSize()) && (FMemory::Memcmp(GetBytes(), Other.GetBytes(), GetSize()) == 0);
}

FName FUniqueNetIdCached::GetType() const
{
    return this->Type;
}

const uint8 *FUniqueNetIdCached::GetBytes() const
{
    return this->DataBytes;
}

int32 FUniqueNetIdCached::GetSize() const
{
    return this->DataBytesSize;
}

bool FUniqueNetIdCached::IsValid() const
{
    return true;
}

FString FUniqueNetIdCached::ToString() const
{
    return this->StrValue;
}

FString FUniqueNetIdCached::ToDebugString() const
{
    return this->StrValue;
}

uint32 GetTypeHash(const FUniqueNetIdCached &A)
{
    return A.TypeHash;
}

EOS_DISABLE_STRICT_WARNINGS