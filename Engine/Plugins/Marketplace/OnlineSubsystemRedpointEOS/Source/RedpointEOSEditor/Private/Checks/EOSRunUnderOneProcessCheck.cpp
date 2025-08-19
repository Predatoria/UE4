// Copyright June Rhodes. All Rights Reserved.

#include "EOSRunUnderOneProcessCheck.h"

#include "Settings/LevelEditorPlaySettings.h"

const TArray<FEOSCheckEntry> FEOSRunUnderOneProcessCheck::Tick(float DeltaSeconds) const
{
    ULevelEditorPlaySettings *PlaySettingsConfig = GetMutableDefault<ULevelEditorPlaySettings>();

    if (IsValid(PlaySettingsConfig))
    {
        bool bRunUnderOneProcess;
        int32 NumberOfClients;
        EPlayNetMode NetMode;
        if (PlaySettingsConfig->GetRunUnderOneProcess(bRunUnderOneProcess) &&
            PlaySettingsConfig->GetPlayNumberOfClients(NumberOfClients) && PlaySettingsConfig->GetPlayNetMode(NetMode))
        {
            if (!bRunUnderOneProcess)
            {
                if (NumberOfClients == 1 && PlaySettingsConfig->bLaunchSeparateServer &&
                    NetMode == EPlayNetMode::PIE_Standalone)
                {
                    // This is supported.
                }
                else
                {
                    return TArray<FEOSCheckEntry>{FEOSCheckEntry(
                        "FEOSRunUnderOneProcessCheck::Misconfigured",
                        "The Redpoint EOS plugin does not support turning off the \"Run under one process\" setting, "
                        "unless the player count is set to 1, 'Launch Separate Server' is turned on and the "
                        "play-in-editor mode is set to 'Standalone'. To test separate processes with multiple players, "
                        "use \"Standalone Game\" as the launch mode instead.",
                        TArray<FEOSCheckAction>{FEOSCheckAction("FEOSRunUnderOneProcessCheck::AutoFix", "Fix this")})};
                }
            }
        }
    }

    return IEOSCheck::EmptyEntries;
}

void FEOSRunUnderOneProcessCheck::HandleAction(const FString &CheckId, const FString &ActionId) const
{
    if (CheckId == "FEOSRunUnderOneProcessCheck::Misconfigured" && ActionId == "FEOSRunUnderOneProcessCheck::AutoFix")
    {
        ULevelEditorPlaySettings *PlaySettingsConfig = GetMutableDefault<ULevelEditorPlaySettings>();
        if (IsValid(PlaySettingsConfig))
        {
            PlaySettingsConfig->LastExecutedPlayModeType = EPlayModeType::PlayMode_InNewProcess;
            PlaySettingsConfig->SetRunUnderOneProcess(true);
            PlaySettingsConfig->SaveConfig();
        }
    }
}