// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Private/NullOSS/UniqueNetIdNull.h"

#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

EOS_ENABLE_STRICT_WARNINGS

FUniqueNetIdRedpointNull::FUniqueNetIdRedpointNull(const FString &InInternalProductUserId, bool bInIsDedicatedServer)
    : ProductUserId(InInternalProductUserId)
    , DataBytes(nullptr)
    , DataBytesSize(0)
    , bIsDedicatedServer(bInIsDedicatedServer)
{
    auto Str = StringCast<ANSICHAR>(*this->ToString());
    this->DataBytesSize = Str.Length() + 1;
    this->DataBytes = (uint8 *)FMemory::MallocZeroed(this->DataBytesSize);
    FMemory::Memcpy(this->DataBytes, Str.Get(), Str.Length());
}

TSharedRef<const FUniqueNetIdRedpointNull> FUniqueNetIdRedpointNull::MakeInvalidId()
{
    return MakeShareable(new FUniqueNetIdRedpointNull(TEXT(""), false));
}

TSharedRef<const FUniqueNetIdRedpointNull> FUniqueNetIdRedpointNull::MakeDedicatedServerId()
{
    return MakeShareable(new FUniqueNetIdRedpointNull(TEXT(""), true));
}

bool FUniqueNetIdRedpointNull::Compare(const FUniqueNetId &Other) const
{
    if (Other.GetType() != GetType())
    {
        return false;
    }

    return (GetSize() == Other.GetSize()) && (FMemory::Memcmp(GetBytes(), Other.GetBytes(), GetSize()) == 0);
}

FUniqueNetIdRedpointNull::~FUniqueNetIdRedpointNull()
{
    FMemory::Free(this->DataBytes);
}

FName FUniqueNetIdRedpointNull::GetType() const
{
    return REDPOINT_EOS_SUBSYSTEM;
}

const uint8 *FUniqueNetIdRedpointNull::GetBytes() const
{
    return this->DataBytes;
}

int32 FUniqueNetIdRedpointNull::GetSize() const
{
    return this->DataBytesSize;
}

bool FUniqueNetIdRedpointNull::IsValid() const
{
    return this->bIsDedicatedServer || !this->ProductUserId.IsEmpty();
}

FString FUniqueNetIdRedpointNull::ToString() const
{
    if (this->bIsDedicatedServer)
    {
        return EOS_DEDICATED_SERVER_ID;
    }

    return this->GetProductUserIdString(false);
}

FString FUniqueNetIdRedpointNull::ToDebugString() const
{
    if (this->bIsDedicatedServer)
    {
        return EOS_DEDICATED_SERVER_ID;
    }

    return this->GetProductUserIdString(true);
}

TSharedPtr<const FUniqueNetIdRedpointNull> FUniqueNetIdRedpointNull::ParseFromString(const FString &ProductUserIdStr)
{
    if (ProductUserIdStr == FString(EOS_DEDICATED_SERVER_ID))
    {
        return FUniqueNetIdRedpointNull::MakeDedicatedServerId();
    }

    return MakeShareable(new FUniqueNetIdRedpointNull(ProductUserIdStr, false));
}

uint32 GetTypeHash(const FUniqueNetIdRedpointNull &A)
{
    return GetTypeHash(A.ToString());
}

const TSharedRef<const FUniqueNetIdRedpointNull> &FUniqueNetIdRedpointNull::InvalidId()
{
    static const TSharedRef<const FUniqueNetIdRedpointNull> InvalidId = FUniqueNetIdRedpointNull::MakeInvalidId();
    return InvalidId;
}

const TSharedRef<const FUniqueNetIdRedpointNull> &FUniqueNetIdRedpointNull::DedicatedServerId()
{
    static const TSharedRef<const FUniqueNetIdRedpointNull> DedicatedServerId = FUniqueNetIdRedpointNull::MakeDedicatedServerId();
    return DedicatedServerId;
}

FArchive &operator<<(FArchive &Ar, FUniqueNetIdRedpointNull &OtherId)
{
    auto IdSer = OtherId.ToString();
    return Ar << IdSer;
}

FString FUniqueNetIdRedpointNull::GetProductUserIdString(bool Debug) const
{
    return this->ProductUserId;
}

bool FUniqueNetIdRedpointNull::IsDedicatedServer() const
{
    return this->bIsDedicatedServer;
}

EOS_DISABLE_STRICT_WARNINGS