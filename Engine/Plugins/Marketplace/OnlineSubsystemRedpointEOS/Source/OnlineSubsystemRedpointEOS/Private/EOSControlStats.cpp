// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/EOSControlStats.h"

#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "Serialization/MemoryArchive.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"

TArray<uint8> SerializeStats(const FOnlineStatsUserUpdatedStats &Stats)
{
    TArray<uint8> Result;
    FMemoryWriter Writer(Result, true);

    FString SerializedUserId = Stats.Account->ToString();
    Writer << SerializedUserId;

    int32 MapCount = Stats.Stats.Num();
    Writer << MapCount;

    for (const auto &KV : Stats.Stats)
    {
        FString Key = KV.Key;
        Writer << Key;

        FOnlineStatValue StatValue = KV.Value.GetValue();
        switch (StatValue.GetType())
        {
        case EOnlineKeyValuePairDataType::Float: {
            int8 Type = 1;
            float Value;
            StatValue.GetValue(Value);
            Writer << Type;
            Writer << Value;
            break;
        }
        case EOnlineKeyValuePairDataType::Double: {
            int8 Type = 2;
            double Value;
            StatValue.GetValue(Value);
            Writer << Type;
            Writer << Value;
            break;
        }
        case EOnlineKeyValuePairDataType::Int32: {
            int8 Type = 3;
            int32 Value;
            StatValue.GetValue(Value);
            Writer << Type;
            Writer << Value;
            break;
        }
        case EOnlineKeyValuePairDataType::Bool: {
            int8 Type = 4;
            bool Value;
            StatValue.GetValue(Value);
            Writer << Type;
            Writer << Value;
            break;
        }
        default: {
            int8 Type = 0;
            Writer << Type;
        }
        }
    }

    Writer.Flush();
    return Result;
}

FOnlineStatsUserUpdatedStats DeserializeStats(const TArray<uint8> &Stats)
{
    FMemoryReader Reader(Stats, true);

    TMap<FString, FOnlineStatUpdate> Results;

    FString SerializedUserId;
    Reader << SerializedUserId;

    int32 MapCount;
    Reader << MapCount;

    for (int32 i = 0; i < MapCount; i++)
    {
        FString Key;
        Reader << Key;

        int8 Type;
        Reader << Type;

        if (Type == 1)
        {
            float Value;
            Reader << Value;
            Results.Add(Key, FOnlineStatUpdate(Value, FOnlineStatUpdate::EOnlineStatModificationType::Unknown));
        }
        else if (Type == 2)
        {
            double Value;
            Reader << Value;
            Results.Add(Key, FOnlineStatUpdate(Value, FOnlineStatUpdate::EOnlineStatModificationType::Unknown));
        }
        else if (Type == 3)
        {
            int32 Value;
            Reader << Value;
            Results.Add(Key, FOnlineStatUpdate(Value, FOnlineStatUpdate::EOnlineStatModificationType::Unknown));
        }
        else if (Type == 4)
        {
            bool Value;
            Reader << Value;
            Results.Add(Key, FOnlineStatUpdate(Value, FOnlineStatUpdate::EOnlineStatModificationType::Unknown));
        }
    }

    // OK to use FUniqueNetIdString here; caller is expected to do CreatePlayerNetId dance with
    // ToString to get actual EOS user ID.
#if defined(UE_5_0_OR_LATER)
    return FOnlineStatsUserUpdatedStats(FUniqueNetIdString::Create(SerializedUserId, REDPOINT_EOS_SUBSYSTEM), Results);
#else
    return FOnlineStatsUserUpdatedStats(MakeShared<FUniqueNetIdString>(SerializedUserId), Results);
#endif
}
