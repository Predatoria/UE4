// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Serialization/Archive.h"

EOS_ENABLE_STRICT_WARNINGS

struct FSerializedExternalAccount
{
public:
    FString AccountType;
    FString AccountId;
    FString AccountDisplayName;

    friend FArchive &operator<<(FArchive &Ar, FSerializedExternalAccount &Obj)
    {
        int8 Version = -1;
        if (Ar.IsSaving())
        {
            Version = 1;
        }
        Ar << Version;
        Ar << Obj.AccountType;
        Ar << Obj.AccountId;
        Ar << Obj.AccountDisplayName;
        return Ar;
    }
};

EOS_DISABLE_STRICT_WARNINGS