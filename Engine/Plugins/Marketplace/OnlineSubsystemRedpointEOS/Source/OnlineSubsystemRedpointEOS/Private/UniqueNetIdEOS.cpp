// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"

#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"

EOS_ENABLE_STRICT_WARNINGS

FUniqueNetIdEOS::FUniqueNetIdEOS(EOS_ProductUserId InInternalProductUserId, bool bInIsDedicatedServer)
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

TSharedRef<const FUniqueNetIdEOS> FUniqueNetIdEOS::MakeInvalidId()
{
    return MakeShareable(new FUniqueNetIdEOS(nullptr, false));
}

TSharedRef<const FUniqueNetIdEOS> FUniqueNetIdEOS::MakeDedicatedServerId()
{
    return MakeShareable(new FUniqueNetIdEOS(EOS_ProductUserId_FromString(""), true));
}

FUniqueNetIdEOS::FUniqueNetIdEOS(EOS_ProductUserId InProductUserId)
    : FUniqueNetIdEOS(InProductUserId, false)
{
    check(EOSString_ProductUserId::IsValid(InProductUserId));
}

bool FUniqueNetIdEOS::Compare(const FUniqueNetId &Other) const
{
    if (Other.GetType() != GetType())
    {
        return false;
    }

    if (Other.GetType() == REDPOINT_EOS_SUBSYSTEM)
    {
        const FUniqueNetIdEOS &OtherEOS = (const FUniqueNetIdEOS &)Other;
        if (EOSString_ProductUserId::IsValid(OtherEOS.GetProductUserId()) &&
            EOSString_ProductUserId::IsValid(this->GetProductUserId()))
        {
            // We only want to compare product user IDs, since most APIs won't be able
            // to provide the Epic account ID that might be present in the IDs returned by
            // the identity interface.
            return OtherEOS.GetProductUserIdString() == this->GetProductUserIdString();
        }
    }

    return (GetSize() == Other.GetSize()) && (FMemory::Memcmp(GetBytes(), Other.GetBytes(), GetSize()) == 0);
}

FUniqueNetIdEOS::~FUniqueNetIdEOS()
{
    FMemory::Free(this->DataBytes);
}

/** Get the product user ID. Do not compare this with null for validation; instead call HasValidProductUserId. */
EOS_ProductUserId FUniqueNetIdEOS::GetProductUserId() const
{
    return this->ProductUserId;
}

bool FUniqueNetIdEOS::HasValidProductUserId() const
{
    if (this->bIsDedicatedServer)
    {
        return true;
    }

    return EOSString_ProductUserId::IsValid(this->ProductUserId);
}

FName FUniqueNetIdEOS::GetType() const
{
    return REDPOINT_EOS_SUBSYSTEM;
}

const uint8 *FUniqueNetIdEOS::GetBytes() const
{
    return this->DataBytes;
}

int32 FUniqueNetIdEOS::GetSize() const
{
    return this->DataBytesSize;
}

bool FUniqueNetIdEOS::IsValid() const
{
    return this->bIsDedicatedServer || EOSString_ProductUserId::IsValid(this->ProductUserId);
}

FString FUniqueNetIdEOS::ToString() const
{
    if (this->bIsDedicatedServer)
    {
        return EOS_DEDICATED_SERVER_ID;
    }

    return this->GetProductUserIdString(false);
}

FString FUniqueNetIdEOS::ToDebugString() const
{
    if (this->bIsDedicatedServer)
    {
        return EOS_DEDICATED_SERVER_ID;
    }

    return this->GetProductUserIdString(true);
}

TSharedPtr<const FUniqueNetIdEOS> FUniqueNetIdEOS::ParseFromString(const FString &ProductUserIdStr)
{
    if (ProductUserIdStr == FString(EOS_DEDICATED_SERVER_ID))
    {
        return FUniqueNetIdEOS::MakeDedicatedServerId();
    }

    EOS_ProductUserId ProductUserId = nullptr;
    if (!ProductUserIdStr.IsEmpty())
    {
        if (EOSString_ProductUserId::FromString(ProductUserIdStr, ProductUserId) != EOS_EResult::EOS_Success)
        {
            UE_LOG(LogEOS, Error, TEXT("Malformed product user ID: %s"), *ProductUserIdStr);
            return nullptr;
        }
    }

    if (!EOSString_ProductUserId::IsValid(ProductUserId))
    {
        UE_LOG(LogEOS, Error, TEXT("Malformed product user ID: %s"), *ProductUserIdStr);
        return nullptr;
    }

    return MakeShared<FUniqueNetIdEOS>(ProductUserId);
}

uint32 GetTypeHash(const FUniqueNetIdEOS &A)
{
    return GetTypeHash(A.ToString());
}

const TSharedRef<const FUniqueNetIdEOS> &FUniqueNetIdEOS::InvalidId()
{
    static const TSharedRef<const FUniqueNetIdEOS> InvalidId = FUniqueNetIdEOS::MakeInvalidId();
    return InvalidId;
}

const TSharedRef<const FUniqueNetIdEOS> &FUniqueNetIdEOS::DedicatedServerId()
{
    static const TSharedRef<const FUniqueNetIdEOS> DedicatedServerId = FUniqueNetIdEOS::MakeDedicatedServerId();
    return DedicatedServerId;
}

FArchive &operator<<(FArchive &Ar, FUniqueNetIdEOS &OtherId)
{
    auto IdSer = OtherId.ToString();
    return Ar << IdSer;
}

FString FUniqueNetIdEOS::GetProductUserIdString(bool Debug) const
{
    if (EOSString_ProductUserId::IsNone(this->ProductUserId))
    {
        return Debug ? TEXT("<no product user>") : TEXT("");
    }

    FString Str;
    if (EOSString_ProductUserId::ToString(this->ProductUserId, Str) == EOS_EResult::EOS_Success)
    {
        return Str;
    }

    return Debug ? TEXT("<unknown product user>") : TEXT("");
}

bool FUniqueNetIdEOS::IsDedicatedServer() const
{
    return this->bIsDedicatedServer;
}

EOS_DISABLE_STRICT_WARNINGS