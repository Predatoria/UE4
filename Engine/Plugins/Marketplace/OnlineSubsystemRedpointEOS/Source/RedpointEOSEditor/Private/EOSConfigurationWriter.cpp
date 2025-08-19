// Copyright June Rhodes. All Rights Reserved.

#include "EOSConfigurationWriter.h"

#include "Misc/FileHelper.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSDefines.h"
#include "UObject/Class.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UObjectGlobals.h"
#include "OnlineSubsystemRedpointEOS/Public/OnlineSubsystemRedpointEOSModule.h"

#if defined(UE_5_0_OR_LATER)
#define FIND_CONFIG_FILE(Filename) GConfig->Find(Filename)
#else
#define FIND_CONFIG_FILE(Filename) GConfig->Find(Filename, true)
#endif

FEOSConfigurationWriter::FEOSConfigurationWriter()
    : bRequireFullReload(false)
    , EOSLoadedFiles()
{
}

void SetArray(FConfigFile *F, const TCHAR *Section, const TCHAR *Key, const TArray<FString> &Value)
{
    F->SetArray(Section, Key, Value);
}

FEOSConfigurationWriter::FEOSLoadedConfigFile FEOSConfigurationWriter::GetConfigFile(const FString &Path)
{
#if defined(UE_5_0_OR_LATER)
    FConfigFile *File = GConfig->Find(Path);
    if (File == nullptr)
    {
        File = &GConfig->Add(Path, FConfigFile());
        File->Read(Path);
    }
#else
    FConfigFile *File = GConfig->Find(Path, true);
    checkf(File != nullptr, TEXT("GConfig->Find must not return nullptr"));
#endif
    if (!this->EOSLoadedFiles.Contains(Path))
    {
        this->EOSLoadedFiles.Add(Path, MakeShared<FEOSConfigFile>(Path));
    }
    return FEOSLoadedConfigFile(File, this->EOSLoadedFiles[Path].ToSharedRef());
}

bool FEOSConfigurationWriter::SetEnumInternal(
    const FString &Key,
    const FString &EnumClass,
    uint8 Value,
    const FString &Section,
    EEOSConfigurationFileType File,
    FName Platform)
{
#if defined(UE_5_1_OR_LATER)
    const UEnum *Enum =
        FindObject<UEnum>(FTopLevelAssetPath(ONLINESUBSYSTEMREDPOINTEOS_PACKAGE_PATH, *EnumClass), true);
#else
    const UEnum *Enum = FindObject<UEnum>((UObject *)ANY_PACKAGE, *EnumClass, true);
#endif
    FString EnumValue = Enum->GetNameStringByValue((int64)Value);

    FEOSLoadedConfigFile LCF = GetConfigFile(EOSConfiguration::GetFilePath(File, Platform));
    if (!LCF.F)
    {
        return false;
    }
    FString ExistingValue;
    if (LCF.F->GetString(*Section, *Key, ExistingValue))
    {
        if (ExistingValue == EnumValue)
        {
            // This already matches the intended value, don't
            // do anything.
            return false;
        }
    }
    LCF.F->SetString(*Section, *Key, *EnumValue);
    LCF.F->Dirty = true;
    LCF.Info->Upserts.Add(FSectionKey(Section, Key), FEOSSetting(EnumValue));
    LCF.Info->bIsModified = true;
    return true;
}

bool FEOSConfigurationWriter::SetBool(
    const FString &Key,
    bool Value,
    const FString &Section,
    EEOSConfigurationFileType File,
    FName Platform)
{
    FEOSLoadedConfigFile LCF = this->GetConfigFile(EOSConfiguration::GetFilePath(File, Platform));
    if (!LCF.F)
    {
        return false;
    }
    bool bExistingValue;
    if (LCF.F->GetBool(*Section, *Key, bExistingValue))
    {
        if (bExistingValue == Value)
        {
            // This already matches the intended value, don't
            // do anything.
            return false;
        }
    }
    LCF.F->SetString(*Section, *Key, Value ? TEXT("True") : TEXT("False"));
    LCF.F->Dirty = true;
    LCF.Info->Upserts.Add(FSectionKey(Section, Key), FEOSSetting(Value));
    LCF.Info->bIsModified = true;
    return true;
}

bool FEOSConfigurationWriter::SetString(
    const FString &Key,
    const FString &Value,
    const FString &Section,
    EEOSConfigurationFileType File,
    FName Platform)
{
    FEOSLoadedConfigFile LCF = this->GetConfigFile(EOSConfiguration::GetFilePath(File, Platform));
    if (!LCF.F)
    {
        return false;
    }
    FString ExistingValue;
    if (LCF.F->GetString(*Section, *Key, ExistingValue))
    {
        if (ExistingValue == Value)
        {
            // This already matches the intended value, don't
            // do anything.
            return false;
        }
    }
    LCF.F->SetString(*Section, *Key, *Value);
    LCF.F->Dirty = true;
    LCF.Info->Upserts.Add(FSectionKey(Section, Key), FEOSSetting(Value));
    LCF.Info->bIsModified = true;
    return true;
}

bool FEOSConfigurationWriter::ReplaceArray(
    const FString &Key,
    const TArray<FString> &Value,
    const FString &Section,
    EEOSConfigurationFileType File,
    FName Platform)
{
    FEOSLoadedConfigFile LCF = this->GetConfigFile(EOSConfiguration::GetFilePath(File, Platform));
    if (!LCF.F)
    {
        return false;
    }

    TArray<FString> ExistingValue;
    if (LCF.F->GetArray(*Section, *Key, ExistingValue))
    {
        if (ExistingValue.Num() == Value.Num())
        {
            bool bExactMatch = true;
            for (int32 i = 0; i < ExistingValue.Num(); i++)
            {
                if (ExistingValue[i] != Value[i])
                {
                    bExactMatch = false;
                    break;
                }
            }
            if (bExactMatch)
            {
                // This already matches the intended value, don't
                // do anything.
                return false;
            }
        }
    }

    // Arrays in config are super broken across a lot of scenarios. We have to force a full reload.
    this->bRequireFullReload = true;

    FConfigSection *Sec = LCF.F->Find(Section);
    if (Sec)
    {
        Sec->Remove(*Key);
        Sec->Remove(FName(*FString::Printf(TEXT("!%s"), *Key)));
        Sec->Remove(FName(*FString::Printf(TEXT("+%s"), *Key)));
    }

    // We don't call SetArray here. Replacements in non-EOS sections
    // have to be done by manually writing the ! and + instructions
    // into the INI file before we do a full reload. Instead we store
    // the value into the Replacements array.
    LCF.Info->Replacements.Add(FSectionKey(Section, Key), FEOSSetting(Value));

    LCF.F->Dirty = true;
    LCF.Info->Upserts.Add(FSectionKey(Section, Key), FEOSSetting(Value));
    LCF.Info->bIsModified = true;
    return true;
}

bool FEOSConfigurationWriter::EnsureArrayElements(
    const FString &Key,
    const TArray<FString> &Value,
    const FString &Section,
    EEOSConfigurationFileType File,
    FName Platform)
{
    FEOSLoadedConfigFile LCF = this->GetConfigFile(EOSConfiguration::GetFilePath(File, Platform));
    if (!LCF.F)
    {
        return false;
    }

    TArray<FString> ExistingValue;
    if (LCF.F->GetArray(*Section, *Key, ExistingValue) ||
        LCF.F->GetArray(*Section, *FString::Printf(TEXT("+%s"), *Key), ExistingValue))
    {
        bool bContainsAll = true;
        for (const auto &Entry : Value)
        {
            if (!ExistingValue.Contains(Entry))
            {
                bContainsAll = false;
                break;
            }
        }

        if (bContainsAll)
        {
            // This already matches the intended value, don't
            // do anything.
            return false;
        }
    }

    // Arrays in config are super broken across a lot of scenarios. We have to force a full reload.
    this->bRequireFullReload = true;

    FConfigSection *Sec = LCF.F->Find(Section);
    if (Sec == nullptr)
    {
        Sec = &LCF.F->Add(Section, FConfigSection());
    }
    for (const auto &Entry : Value)
    {
        if (!ExistingValue.Contains(Entry))
        {
            Sec->Add(*FString::Printf(TEXT("+%s"), *Key), Entry);
        }
    }

    // It doesn't matter that the above code adds entries with the + prefix (which
    // doesn't lead to the in-memory state being correct), because we do a full
    // reload which fixes things up after saving.

    LCF.F->Dirty = true;
    LCF.Info->Upserts.Add(FSectionKey(Section, Key), FEOSSetting(ExistingValue));
    LCF.Info->bIsModified = true;
    return true;
}

bool FEOSConfigurationWriter::Remove(
    const FString &Key,
    const FString &Section,
    EEOSConfigurationFileType File,
    FName Platform)
{
    FEOSLoadedConfigFile LCF = this->GetConfigFile(EOSConfiguration::GetFilePath(File, Platform));
    if (!LCF.F)
    {
        return false;
    }

    FConfigSection *Sec = LCF.F->FindOrAddSection(Section);
    if (!Sec)
    {
        return false;
    }

    Sec->Remove(*Key);
    LCF.F->Dirty = true;
    LCF.Info->Removals.Add(FSectionKey(Section, Key));
    LCF.Info->bIsModified = true;
    return true;
}

void FEOSConfigurationWriter::FlushChanges()
{
    for (const auto &KV : this->EOSLoadedFiles)
    {
        check(KV.Value->Path == KV.Key);
        FEOSLoadedConfigFile LCF = this->GetConfigFile(KV.Value->Path);
        if (LCF.F != nullptr)
        {
            FConfigSection *UnwantedSection =
                LCF.F->Find(TEXT("/Script/OnlineSubsystemEOSEditor.OnlineSubsystemEOSEditorConfig"));
            if (UnwantedSection)
            {
                // Remove the section that gets automatically added by the config serialization system if it already
                // exists.
                LCF.F->Remove(TEXT("/Script/OnlineSubsystemEOSEditor.OnlineSubsystemEOSEditorConfig"));
                LCF.F->Dirty = true;
                LCF.Info->bIsModified = true;
            }
        }
    }

    for (const auto &KV : this->EOSLoadedFiles)
    {
        if (KV.Value->bIsModified)
        {
            check(KV.Value->Path == KV.Key);
            FEOSLoadedConfigFile LCF = this->GetConfigFile(KV.Value->Path);
            check(LCF.F != nullptr);

            // Write the changes to disk.
            LCF.F->NoSave = false;
            LCF.F->Write(KV.Value->Path);

            if (this->bRequireFullReload && LCF.Info->Replacements.Num() > 0)
            {
                // Prevent further writes by the editor.
                LCF.F->NoSave = true;
                LCF.F->Dirty = false;

                // Bucket replacements by section.
                TMap<FString, TArray<TTuple<FString, TArray<FString>>>> KeysAndValuesBySection;
                for (const auto &Replacement : LCF.Info->Replacements)
                {
                    if (!KeysAndValuesBySection.Contains(Replacement.Key.Section))
                    {
                        KeysAndValuesBySection.Add(Replacement.Key.Section, TArray<TTuple<FString, TArray<FString>>>());
                    }
                    KeysAndValuesBySection[Replacement.Key.Section].Add(
                        TTuple<FString, TArray<FString>>(Replacement.Key.Key, Replacement.Value.GetStringArray()));
                }

                // To generate array replacements, we have to manually emit the ! and +
                // instructions into the INI file. That's because SetArray emits it in the
                // form A=.. instead of !A=ClearArray \n +A=.. which is required for
                // NetDriver configuration.
                FString Contents;
                if (FFileHelper::LoadFileToString(Contents, *KV.Value->Path))
                {
                    const TCHAR *Ptr = Contents.Len() > 0 ? *Contents : nullptr;
                    const TCHAR *Original = Ptr;
                    const TCHAR *LastEndOfLineWithContent = Ptr;
                    bool Done = false;
                    FString CurrentSection = TEXT("");
                    while (!Done && Ptr != nullptr)
                    {
                        while (*Ptr == '\r' || *Ptr == '\n')
                        {
                            Ptr++;
                        }

                        const TCHAR *BeforeMove = Ptr;

                        FString TheLine;
                        int32 LinesConsumed = 0;
                        FParse::LineExtended(&Ptr, TheLine, LinesConsumed, false);
                        if (Ptr == nullptr || *Ptr == 0)
                        {
                            Done = true;
                        }

                        TCHAR *Start = const_cast<TCHAR *>(*TheLine);

                        while (*Start && FChar::IsWhitespace(Start[FCString::Strlen(Start) - 1]))
                        {
                            Start[FCString::Strlen(Start) - 1] = 0;
                        }

                        if (*Start == '[' && Start[FCString::Strlen(Start) - 1] == ']')
                        {
                            // If this is the end of a section we care about, emit our entries prior to Start.
                            if (KeysAndValuesBySection.Contains(CurrentSection))
                            {
                                TArray<FString> NewLines;
                                for (const auto &Entry : KeysAndValuesBySection[CurrentSection])
                                {
                                    NewLines.Add(FString::Printf(TEXT("!%s=ClearArray"), *Entry.Key));
                                    for (const auto &Subentry : Entry.Value)
                                    {
                                        NewLines.Add(FString::Printf(TEXT("+%s=%s"), *Entry.Key, *Subentry));
                                    }
                                }
                                FString NewContent = FString::Join(NewLines, TEXT("\n"));
                                int32 NewOffset = Ptr - Original + NewContent.Len();
                                Contents = Contents.Mid(0, LastEndOfLineWithContent - Original) + +TEXT("\n") +
                                           NewContent + Contents.Mid(LastEndOfLineWithContent - Original);
                                Original = *Contents;
                                Ptr = (*Contents) + NewOffset;
                                LastEndOfLineWithContent = Ptr;
                                KeysAndValuesBySection.Remove(CurrentSection);
                            }

                            // This is a section.
                            Start++;
                            Start[FCString::Strlen(Start) - 1] = 0;
                            CurrentSection = Start;
                        }

                        if (!FString(Start).IsEmpty())
                        {
                            LastEndOfLineWithContent = BeforeMove + FCString::Strlen(Start);
                        }

                        // We don't care about any other types of lines.
                    }

                    Contents = Contents.TrimEnd();

                    if (KeysAndValuesBySection.Num() > 0)
                    {
                        // For any sections we haven't emitted, they don't already exist in the file, so
                        // we have to add them at the end.
                        TArray<FString> ExtendedLines;
                        for (const auto &Sec : KeysAndValuesBySection)
                        {
                            ExtendedLines.Add(TEXT(""));
                            ExtendedLines.Add(FString::Printf(TEXT("[%s]"), *Sec.Key));
                            for (const auto &Entry : KeysAndValuesBySection[Sec.Key])
                            {
                                ExtendedLines.Add(FString::Printf(TEXT("!%s=ClearArray"), *Entry.Key));
                                for (const auto &Subentry : Entry.Value)
                                {
                                    ExtendedLines.Add(FString::Printf(TEXT("+%s=%s"), *Entry.Key, *Subentry));
                                }
                            }
                        }
                        Contents = (Contents + TEXT("\n")).TrimStart() + FString::Join(ExtendedLines, TEXT("\n"));
                    }

                    FFileHelper::SaveStringToFile(Contents, *KV.Value->Path);
                }
            }

            if (!this->bRequireFullReload)
            {
                // Sync the individual changes up to other config files
                // in memory, based on their cache keys.
#if defined(EOS_CONFIG_REQUIRES_MODERN_ITERATION)
                for (const FString &IniPairName : GConfig->GetFilenames())
                {
                    FConfigFile *ConfigFile = FIND_CONFIG_FILE(IniPairName);
#else
                for (TPair<FString, FConfigFile> &IniPair : *GConfig)
                {
                    FConfigFile &ConfigFileRef = IniPair.Value;
                    FConfigFile *ConfigFile = &ConfigFileRef;
#endif
                    if (ConfigFile != nullptr && ConfigFile->Num() > 0)
                    {
                        for (const auto &H : ConfigFile->SourceIniHierarchy)
                        {
#if defined(UE_5_1_OR_LATER)
                            if (H.Value == KV.Value->Path)
#else
                            if (H.Value.Filename == KV.Value->Path)
#endif
                            {
                                // Apply our changes to this file as well.
                                for (const auto &Upsert : KV.Value->Upserts)
                                {
                                    switch (Upsert.Value.GetType())
                                    {
                                    case EEOSSettingType::Bool:
                                        ConfigFile->SetString(
                                            *Upsert.Key.Section,
                                            *Upsert.Key.Key,
                                            Upsert.Value.GetBool() ? TEXT("True") : TEXT("False"));
                                        break;
                                    case EEOSSettingType::String:
                                        ConfigFile->SetString(
                                            *Upsert.Key.Section,
                                            *Upsert.Key.Key,
                                            *Upsert.Value.GetString());
                                        break;
                                    case EEOSSettingType::StringArray:
                                        SetArray(
                                            ConfigFile,
                                            *Upsert.Key.Section,
                                            *Upsert.Key.Key,
                                            Upsert.Value.GetStringArray());
                                        break;
                                    }
                                }
                                for (const auto &Remove : KV.Value->Removals)
                                {
                                    FConfigSection *Sec = ConfigFile->FindOrAddSection(*Remove.Section);
                                    if (Sec)
                                    {
                                        Sec->Remove(*Remove.Key);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (this->bRequireFullReload)
    {
        // Reload the configuration hierarchy so the defaults we just wrote will propagate
        // to the platform-specific ini file registered in GEngineIni.
#if defined(EOS_CONFIG_REQUIRES_MODERN_ITERATION)
        for (const FString &IniPairName : GConfig->GetFilenames())
        {
            FConfigFile *ConfigFile = FIND_CONFIG_FILE(IniPairName);
#else
        for (TPair<FString, FConfigFile> &IniPair : *GConfig)
        {
            FConfigFile &ConfigFileRef = IniPair.Value;
            FConfigFile *ConfigFile = &ConfigFileRef;
#endif
            if (ConfigFile != nullptr && ConfigFile->Num() > 0)
            {
                FName BaseName = ConfigFile->Name;
                verify(FConfigCacheIni::LoadLocalIniFile(*ConfigFile, *BaseName.ToString(), true, nullptr, true));
            }
        }
    }
}