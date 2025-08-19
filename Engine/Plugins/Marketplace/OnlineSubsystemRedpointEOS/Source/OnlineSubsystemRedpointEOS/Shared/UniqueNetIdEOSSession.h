// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "Containers/UnrealString.h"
#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"

EOS_ENABLE_STRICT_WARNINGS

FORCEINLINE uint32 FUniqueNetIdEOSSession_GetTypeHash(const FString &S)
{
    return GetTypeHash(S);
}

class FUniqueNetIdEOSSession : public FUniqueNetId
{
private:
    FString SessionId;
    uint8 *DataBytes;
    int32 DataBytesSize;

public:
    FUniqueNetIdEOSSession() = delete;

    // This operation isn't safe, because the copy won't have been created through TSharedPtr, and thus
    // you won't be able to call AsShared on it. Prevent any accidental copies by removing the copy constructor.
    UE_NONCOPYABLE(FUniqueNetIdEOSSession);

    FUniqueNetIdEOSSession(const FString &InSessionId)
        : SessionId(InSessionId)
        , DataBytes(nullptr)
        , DataBytesSize(0)
    {
        verify(
            EOSString_SessionModification_SessionId::AllocateToCharBuffer(
                InSessionId,
                (const char *&)this->DataBytes,
                this->DataBytesSize) == EOS_EResult::EOS_Success);
    }

    FUniqueNetIdEOSSession(const char *InSessionId)
        : FUniqueNetIdEOSSession(FString(ANSI_TO_TCHAR(InSessionId)))
    {
    }

    virtual bool Compare(const FUniqueNetId &Other) const override
    {
        if (Other.GetType() != GetType())
        {
            return false;
        }

        return (GetSize() == Other.GetSize()) && (FMemory::Memcmp(GetBytes(), Other.GetBytes(), GetSize()) == 0);
    }

    ~FUniqueNetIdEOSSession()
    {
        EOSString_SessionModification_SessionId::FreeFromCharBuffer((const char *&)this->DataBytes);
    }

    virtual FString GetSessionId() const
    {
        return this->SessionId;
    }

    virtual FName GetType() const override
    {
        return REDPOINT_EOS_SUBSYSTEM_SESSION;
    }

    virtual const uint8 *GetBytes() const override
    {
        return this->DataBytes;
    }

    virtual int32 GetSize() const override
    {
        return this->DataBytesSize;
    }

    virtual bool IsValid() const override
    {
        return true;
    }

    virtual FString ToString() const override
    {
        return this->SessionId;
    }

    virtual FString ToDebugString() const override
    {
        return this->SessionId;
    }

    friend uint32 GetTypeHash(const FUniqueNetIdEOSSession &A)
    {
        return FUniqueNetIdEOSSession_GetTypeHash(A.ToString());
    }

    friend FArchive &operator<<(FArchive &Ar, FUniqueNetIdEOSSession &OtherId)
    {
        auto IdSer = OtherId.ToString();
        return Ar << IdSer;
    }
};

EOS_DISABLE_STRICT_WARNINGS