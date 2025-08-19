// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Serialization/Archive.h"

EOS_ENABLE_STRICT_WARNINGS

struct FSerializedBlockedUser
{
public:
    FString ProductUserId;

    friend FArchive &operator<<(FArchive &Ar, FSerializedBlockedUser &Obj)
    {
        int8 Version = -1;
        if (Ar.IsSaving())
        {
            Version = 1;
        }
        Ar << Version;
        Ar << Obj.ProductUserId;
        return Ar;
    }
};

EOS_DISABLE_STRICT_WARNINGS