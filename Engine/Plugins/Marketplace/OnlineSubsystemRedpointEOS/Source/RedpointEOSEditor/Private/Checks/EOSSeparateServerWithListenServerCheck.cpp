// Copyright June Rhodes. All Rights Reserved.

#include "EOSSeparateServerWithListenServerCheck.h"

#include "Settings/LevelEditorPlaySettings.h"

const TArray<FEOSCheckEntry> FEOSSeparateServerWithListenServerCheck::Tick(float DeltaSeconds) const
{
    ULevelEditorPlaySettings *PlaySettingsConfig = GetMutableDefault<ULevelEditorPlaySettings>();

    if (IsValid(PlaySettingsConfig))
    {
        EPlayNetMode NetMode;
        if (PlaySettingsConfig->GetPlayNetMode(NetMode))
        {
            if (NetMode == EPlayNetMode::PIE_ListenServer)
            {
                if (PlaySettingsConfig->bLaunchSeparateServer)
                {
                    return TArray<FEOSCheckEntry>{FEOSCheckEntry(
                        "FEOSSeparateServerWithListenServerCheck::Misconfigured",
                        "\"Run separate server\" can not be enabled while the play net mode is set to \"Play as listen "
                        "server\".",
                        TArray<FEOSCheckAction>{
                            FEOSCheckAction("FEOSSeparateServerWithListenServerCheck::AutoFix", "Fix this")})};
                }
            }
        }
    }

    return IEOSCheck::EmptyEntries;
}

void FEOSSeparateServerWithListenServerCheck::HandleAction(const FString &CheckId, const FString &ActionId) const
{
    if (CheckId == "FEOSSeparateServerWithListenServerCheck::Misconfigured" &&
        ActionId == "FEOSSeparateServerWithListenServerCheck::AutoFix")
    {
        ULevelEditorPlaySettings *PlaySettingsConfig = GetMutableDefault<ULevelEditorPlaySettings>();
        if (IsValid(PlaySettingsConfig))
        {
            PlaySettingsConfig->bLaunchSeparateServer = false;
            PlaySettingsConfig->SaveConfig();
        }
    }
}