// Copyright June Rhodes. All Rights Reserved.

#include "StorageEOSConfigurer.h"

#include "../PlatformHelpers.h"

void FStorageEOSConfigurer::InitDefaults(FEOSConfigurerContext &Context, UOnlineSubsystemEOSEditorConfig *Instance)
{
}

void FStorageEOSConfigurer::Load(
    FEOSConfigurerContext &Context,
    FEOSConfigurationReader &Reader,
    UOnlineSubsystemEOSEditorConfig *Instance)
{
    Reader.GetString(TEXT("PlayerDataEncryptionKey"), Instance->PlayerDataEncryptionKey);
}

bool FStorageEOSConfigurer::Validate(FEOSConfigurerContext &Context, UOnlineSubsystemEOSEditorConfig *Instance)
{
    TArray<TCHAR> PermittedChars = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    bool bGenerate = false;
    if (Instance->PlayerDataEncryptionKey.IsEmpty() || Instance->PlayerDataEncryptionKey.Len() != 64)
    {
        bGenerate = true;
    }
    else
    {
        for (const auto &Char : Instance->PlayerDataEncryptionKey)
        {
            if (!PermittedChars.Contains(Char))
            {
                bGenerate = true;
                break;
            }
        }
    }

    if (bGenerate)
    {
        Instance->PlayerDataEncryptionKey = TEXT("");
        for (int32 i = 0; i < 64; i++)
        {
            Instance->PlayerDataEncryptionKey += PermittedChars[FMath::Rand() % PermittedChars.Num()];
        }
        return true;
    }

    return false;
}

void FStorageEOSConfigurer::Save(
    FEOSConfigurerContext &Context,
    FEOSConfigurationWriter &Writer,
    UOnlineSubsystemEOSEditorConfig *Instance)
{
    Writer.SetString(TEXT("PlayerDataEncryptionKey"), Instance->PlayerDataEncryptionKey);
}