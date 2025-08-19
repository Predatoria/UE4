// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EOSConfiguration.h"

class FEOSConfigurationWriter
{
private:
    struct FSectionKey
    {
        FSectionKey(const FString &InSection, const FString &InKey)
            : Section(InSection)
            , Key(InKey){};
        FString Section;
        FString Key;

        bool operator==(const FSectionKey &Other) const
        {
            return this->Section == Other.Section && this->Key == Other.Key;
        }

        friend uint32 GetTypeHash(const FSectionKey &Val)
        {
            return GetTypeHash(FString::Printf(TEXT("%s:%s"), *Val.Section, *Val.Key));
        }
    };

    struct FEOSConfigFile
    {
    public:
        FEOSConfigFile(const FString &InPath)
            : Path(InPath)
            , Removals()
            , Upserts()
            , bIsModified(false){};
        UE_NONCOPYABLE(FEOSConfigFile);
        FString Path;
        TSet<FSectionKey> Removals;
        TMap<FSectionKey, FEOSSetting> Upserts;
        TMap<FSectionKey, FEOSSetting> Replacements;
        bool bIsModified;
    };

    /**
     * It's not safe to store FConfigFile* pointers for any length of time, so this value should only
     * ever be temporarily on the stack while doing the write and then released.
     */
    struct FEOSLoadedConfigFile
    {
    public:
        FEOSLoadedConfigFile(FConfigFile *InF, const TSharedRef<FEOSConfigFile> &InInfo)
            : F(InF)
            , Info(InInfo){};
        FConfigFile *F;
        TSharedRef<FEOSConfigFile> Info;
    };

    bool bRequireFullReload;
    TMap<FString, TSharedPtr<FEOSConfigFile>> EOSLoadedFiles;

    FEOSLoadedConfigFile GetConfigFile(const FString &Path);

    bool SetEnumInternal(
        const FString &Key,
        const FString &EnumClass,
        uint8 Value,
        const FString &Section = DefaultEOSSection,
        EEOSConfigurationFileType File = EEOSConfigurationFileType::Engine,
        FName Platform = NAME_None);

public:
    FEOSConfigurationWriter();

    bool SetBool(
        const FString &Key,
        bool Value,
        const FString &Section = DefaultEOSSection,
        EEOSConfigurationFileType File = EEOSConfigurationFileType::Engine,
        FName Platform = NAME_None);
    bool SetString(
        const FString &Key,
        const FString &Value,
        const FString &Section = DefaultEOSSection,
        EEOSConfigurationFileType File = EEOSConfigurationFileType::Engine,
        FName Platform = NAME_None);

    template <typename T>
    bool SetEnum(
        const FString &Key,
        const FString &EnumClass,
        T Value,
        const FString &Section = DefaultEOSSection,
        EEOSConfigurationFileType File = EEOSConfigurationFileType::Engine,
        FName Platform = NAME_None)
    {
        static_assert(sizeof(T) == sizeof(uint8), "must be uint8 enum");
        return SetEnumInternal(Key, EnumClass, (uint8)Value, Section, File, Platform);
    }

    bool ReplaceArray(
        const FString &Key,
        const TArray<FString> &Value,
        const FString &Section = DefaultEOSSection,
        EEOSConfigurationFileType File = EEOSConfigurationFileType::Engine,
        FName Platform = NAME_None);
    bool EnsureArrayElements(
        const FString &Key,
        const TArray<FString> &Value,
        const FString &Section = DefaultEOSSection,
        EEOSConfigurationFileType File = EEOSConfigurationFileType::Engine,
        FName Platform = NAME_None);

    bool Remove(
        const FString &Key,
        const FString &Section = DefaultEOSSection,
        EEOSConfigurationFileType File = EEOSConfigurationFileType::Engine,
        FName Platform = NAME_None);

    void FlushChanges();
};