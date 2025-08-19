// Copyright June Rhodes. All Rights Reserved.

#include "EOSNetDriverSettingsCheck.h"

#include "../EOSConfiguration.h"

#if defined(UE_5_0_OR_LATER)
#define FIND_CONFIG_FILE(Filename) GConfig->Find(Filename)
#else
#define FIND_CONFIG_FILE(Filename) GConfig->Find(Filename, true)
#endif

FEOSNetDriverSettingsCheck::FEOSNetDriverSettingsCheck()
{
}

const TArray<FEOSCheckEntry> FEOSNetDriverSettingsCheck::ProcessCustomSignal(
    const FString &Context,
    const FString &SignalId) const
{
    if (Context == TEXT("Configuration") && SignalId == TEXT("Load"))
    {
        FConfigFile *F = FIND_CONFIG_FILE(EOSConfiguration::GetFilePath(EEOSConfigurationFileType::Engine, TEXT("")));
        if (F)
        {
            if (F->Contains("/Script/OnlineSubsystemUtils.IpNetDriver"))
            {
                return TArray<FEOSCheckEntry>{FEOSCheckEntry(
                    "FEOSNetDriverSettingsCheck::OldEntries",
                    "You have settings in DefaultEngine.ini for IpNetDriver that will be ignored when using the EOS "
                    "networking driver. You should either delete these settings from DefaultEngine.ini, or move "
                    "them to DefaultOnlineSubsystemRedpointEOS.ini under the "
                    "[/Script/OnlineSubsystemRedpointEOS.EOSNetDriver] section. Refer to the "
                    "documentation for more information.",
                    TArray<FEOSCheckAction>{
                        FEOSCheckAction("FEOSNetDriverSettingsCheck::OpenDocumentation", "Read documentation")})};
            }
        }
    }

    return IEOSCheck::EmptyEntries;
}

void FEOSNetDriverSettingsCheck::HandleAction(const FString &CheckId, const FString &ActionId) const
{
    if (CheckId == "FEOSNetDriverSettingsCheck::OldEntries")
    {
        if (ActionId == "FEOSNetDriverSettingsCheck::OpenDocumentation")
        {
            FPlatformProcess::LaunchURL(
                TEXT("https://docs.redpoint.games/eos-online-subsystem/docs/"
                     "networking_configuration#customizing-unetdriver-settings"),
                nullptr,
                nullptr);
        }
    }
}