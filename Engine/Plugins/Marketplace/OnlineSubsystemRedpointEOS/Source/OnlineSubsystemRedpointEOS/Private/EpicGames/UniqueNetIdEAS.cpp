// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/UniqueNetIdEAS.h"

#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"

EOS_ENABLE_STRICT_WARNINGS

FUniqueNetIdEAS::FUniqueNetIdEAS(EOS_EpicAccountId InInternalEpicAccountId, bool bUnused)
    : EpicAccountId(InInternalEpicAccountId)
    , DataBytes(nullptr)
    , DataBytesSize(0)
{
    auto Str = StringCast<ANSICHAR>(*this->ToString());
    this->DataBytesSize = Str.Length() + 1;
    this->DataBytes = (uint8 *)FMemory::MallocZeroed(this->DataBytesSize);
    FMemory::Memcpy(this->DataBytes, Str.Get(), Str.Length());
}

TSharedRef<const FUniqueNetIdEAS> FUniqueNetIdEAS::MakeInvalidId()
{
    return MakeShareable(new FUniqueNetIdEAS(nullptr, false));
}

FUniqueNetIdEAS::FUniqueNetIdEAS(EOS_EpicAccountId InEpicAccountId)
    : FUniqueNetIdEAS(InEpicAccountId, false)
{
    check(EOSString_EpicAccountId::IsValid(InEpicAccountId));
}

bool FUniqueNetIdEAS::Compare(const FUniqueNetId &Other) const
{
    if (Other.GetType() != GetType())
    {
        return false;
    }

    if (Other.GetType() == REDPOINT_EAS_SUBSYSTEM)
    {
        const FUniqueNetIdEAS &OtherEOS = (const FUniqueNetIdEAS &)Other;
        if (EOSString_EpicAccountId::IsValid(OtherEOS.GetEpicAccountId()) &&
            EOSString_EpicAccountId::IsValid(this->GetEpicAccountId()))
        {
            return OtherEOS.GetEpicAccountIdString() == this->GetEpicAccountIdString();
        }
    }

    return (GetSize() == Other.GetSize()) && (FMemory::Memcmp(GetBytes(), Other.GetBytes(), GetSize()) == 0);
}

FUniqueNetIdEAS::~FUniqueNetIdEAS()
{
    FMemory::Free(this->DataBytes);
}

EOS_EpicAccountId FUniqueNetIdEAS::GetEpicAccountId() const
{
    return this->EpicAccountId;
}

bool FUniqueNetIdEAS::HasValidEpicAccountId() const
{
    return EOSString_EpicAccountId::IsValid(this->EpicAccountId);
}

FName FUniqueNetIdEAS::GetType() const
{
    return REDPOINT_EAS_SUBSYSTEM;
}

const uint8 *FUniqueNetIdEAS::GetBytes() const
{
    return this->DataBytes;
}

int32 FUniqueNetIdEAS::GetSize() const
{
    return this->DataBytesSize;
}

bool FUniqueNetIdEAS::IsValid() const
{
    return EOSString_EpicAccountId::IsValid(this->EpicAccountId);
}

FString FUniqueNetIdEAS::ToString() const
{
    return this->GetEpicAccountIdString(false);
}

FString FUniqueNetIdEAS::ToDebugString() const
{
    return this->GetEpicAccountIdString(true);
}

TSharedPtr<const FUniqueNetId> FUniqueNetIdEAS::ParseFromString(const FString &EpicAccountIdStr)
{
    EOS_EpicAccountId EpicAccountId = nullptr;
    if (!EpicAccountIdStr.IsEmpty())
    {
        if (EOSString_EpicAccountId::FromString(EpicAccountIdStr, EpicAccountId) != EOS_EResult::EOS_Success)
        {
            UE_LOG(LogEOS, Error, TEXT("Malformed Epic account ID: %s"), *EpicAccountIdStr);
            return nullptr;
        }
    }

    return MakeShared<FUniqueNetIdEAS>(EpicAccountId);
}

uint32 GetTypeHash(const FUniqueNetIdEAS &A)
{
    return GetTypeHash(A.ToString());
}

const TSharedRef<const FUniqueNetIdEAS> &FUniqueNetIdEAS::InvalidId()
{
    static const TSharedRef<const FUniqueNetIdEAS> InvalidId = FUniqueNetIdEAS::MakeInvalidId();
    return InvalidId;
}

FArchive &operator<<(FArchive &Ar, FUniqueNetIdEAS &OtherId)
{
    auto IdSer = OtherId.ToString();
    return Ar << IdSer;
}

FString FUniqueNetIdEAS::GetEpicAccountIdString(bool Debug) const
{
    if (EOSString_EpicAccountId::IsNone(this->EpicAccountId))
    {
        return Debug ? TEXT("<no epic account id>") : TEXT("");
    }

    FString Str;
    if (EOSString_EpicAccountId::ToString(this->EpicAccountId, Str) == EOS_EResult::EOS_Success)
    {
        return Str;
    }

    return Debug ? TEXT("<unknown epic account id>") : TEXT("");
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION