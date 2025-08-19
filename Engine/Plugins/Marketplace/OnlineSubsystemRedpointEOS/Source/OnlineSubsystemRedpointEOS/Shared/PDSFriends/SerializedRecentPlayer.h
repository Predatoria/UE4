// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/PDSFriends/SerializedFriend.h"
#include "Serialization/Archive.h"

EOS_ENABLE_STRICT_WARNINGS

struct FSerializedRecentPlayer
{
public:
    FString ProductUserId;
    FDateTime LastSeen;

    friend FArchive &operator<<(FArchive &Ar, FSerializedRecentPlayer &Obj)
    {
        int8 Version = -1;
        if (Ar.IsSaving())
        {
            Version = 1;
        }
        Ar << Version;
        Ar << Obj.ProductUserId;
        Ar << Obj.LastSeen;
        return Ar;
    }
};

EOS_DISABLE_STRICT_WARNINGS