// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/ConfigCacheIni.h"

enum EEOSConfigurationFileType
{
    /**
     * DefaultEngine.ini or a platform's Engine.ini file.
     */
    Engine,

    /**
     * DefaultGame.ini
     */
    Game,

    /**
     * DefaultEditor.ini
     */
    Editor,

    /**
     * DedicatedServerEngine.ini
     */
    DedicatedServer,

    /**
     * Build/NoRedist/TrustedEOSClient.ini
     */
    TrustedClient,
};

enum EEOSSettingType
{
    Bool,
    String,
    StringArray,
};

class FEOSSetting
{
private:
    bool bValue;
    FString StrValue;
    TArray<FString> ArrValue;
    EEOSSettingType Type;

public:
    FEOSSetting(bool bInValue)
        : bValue(bInValue)
        , StrValue()
        , ArrValue()
        , Type(EEOSSettingType::Bool){};
    FEOSSetting(const FString &InValue)
        : bValue()
        , StrValue(InValue)
        , ArrValue()
        , Type(EEOSSettingType::String){};
    FEOSSetting(const TArray<FString> &InValue)
        : bValue()
        , StrValue()
        , ArrValue(InValue)
        , Type(EEOSSettingType::StringArray){};

    FORCEINLINE EEOSSettingType GetType() const
    {
        return this->Type;
    }
    FORCEINLINE bool GetBool() const
    {
        check(this->Type == EEOSSettingType::Bool);
        return this->bValue;
    }
    FORCEINLINE const FString &GetString() const
    {
        check(this->Type == EEOSSettingType::String);
        return this->StrValue;
    }
    FORCEINLINE const TArray<FString> &GetStringArray() const
    {
        check(this->Type == EEOSSettingType::StringArray);
        return this->ArrValue;
    }
};

#define DefaultEOSSection TEXT("EpicOnlineServices")

namespace EOSConfiguration
{
extern TMap<FString, FString> FilePathCache;
FString GetFilePath(EEOSConfigurationFileType File, FName Platform);
}; // namespace EOSConfiguration