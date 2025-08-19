// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EOSConfiguration.h"

class FEOSConfigurationReader
{
private:
    bool GetEnumInternal(
        const FString &Key,
        const FString &EnumClass,
        uint8 &OutValue,
        const FString &Section = DefaultEOSSection,
        EEOSConfigurationFileType File = EEOSConfigurationFileType::Engine,
        FName Platform = NAME_None);

public:
    FEOSConfigurationReader();

    bool GetBool(
        const FString &Key,
        bool &OutValue,
        const FString &Section = DefaultEOSSection,
        EEOSConfigurationFileType File = EEOSConfigurationFileType::Engine,
        FName Platform = NAME_None);
    bool GetString(
        const FString &Key,
        FString &OutValue,
        const FString &Section = DefaultEOSSection,
        EEOSConfigurationFileType File = EEOSConfigurationFileType::Engine,
        FName Platform = NAME_None);

    template <typename T>
    bool GetEnum(
        const FString &Key,
        const FString &EnumClass,
        T &OutValue,
        const FString &Section = DefaultEOSSection,
        EEOSConfigurationFileType File = EEOSConfigurationFileType::Engine,
        FName Platform = NAME_None)
    {
        static_assert(sizeof(T) == sizeof(uint8), "must be uint8 enum");
        return GetEnumInternal(Key, EnumClass, (uint8 &)OutValue, Section, File, Platform);
    }

    bool GetArray(
        const FString &Key,
        TArray<FString> &OutValue,
        const FString &Section = DefaultEOSSection,
        EEOSConfigurationFileType File = EEOSConfigurationFileType::Engine,
        FName Platform = NAME_None);
};