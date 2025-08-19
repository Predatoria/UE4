// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Serialization/Archive.h"

EOS_ENABLE_STRICT_WARNINGS

struct FSerializedCachedFriend
{
public:
    FString AccountId;
    TArray<uint8> AccountIdBytes;
    uint32 AccountIdTypeHash;
    FString AccountDisplayName;
    FString AccountRealName;
    FString ExternalAccountId;
    int32 ExternalAccountIdType;
    FString AccountAvatarUrl;

    friend FArchive &operator<<(FArchive &Ar, FSerializedCachedFriend &Obj)
    {
        int8 Version = -1;
        if (Ar.IsSaving())
        {
            Version = 2;
        }
        Ar << Version;
        Ar << Obj.AccountId;
        Ar << Obj.AccountIdBytes;
        Ar << Obj.AccountIdTypeHash;
        Ar << Obj.AccountDisplayName;
        Ar << Obj.AccountRealName;
        Ar << Obj.ExternalAccountId;
        Ar << Obj.ExternalAccountIdType;
        if (Version >= 2)
        {
            Ar << Obj.AccountAvatarUrl;
        }
        return Ar;
    }
};

EOS_DISABLE_STRICT_WARNINGS