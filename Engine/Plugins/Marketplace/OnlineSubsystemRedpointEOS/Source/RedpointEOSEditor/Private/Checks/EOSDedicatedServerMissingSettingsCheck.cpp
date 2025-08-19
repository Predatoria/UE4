// Copyright June Rhodes. All Rights Reserved.

#include "EOSDedicatedServerMissingSettingsCheck.h"

#include "../OnlineSubsystemEOSEditorConfig.h"
#include "Framework/Docking/TabManager.h"
#include "ISettingsModule.h"
#include "OnlineSubsystemRedpointEOSModule.h"
#include "Settings/LevelEditorPlaySettings.h"
#include "UnrealEdMisc.h"
#include "Widgets/Docking/SDockTab.h"

FEOSDedicatedServerMissingSettingsCheck::FEOSDedicatedServerMissingSettingsCheck()
    : Config(MakeShared<FEOSConfigIni>())
{
}

const TArray<FEOSCheckEntry> FEOSDedicatedServerMissingSettingsCheck::Tick(float DeltaSeconds) const
{
    ULevelEditorPlaySettings *PlaySettingsConfig = GetMutableDefault<ULevelEditorPlaySettings>();

    if (IsValid(PlaySettingsConfig))
    {
        EPlayNetMode NetMode;
        if (PlaySettingsConfig->GetPlayNetMode(NetMode))
        {
            if (PlaySettingsConfig->bLaunchSeparateServer || NetMode == EPlayNetMode::PIE_Client)
            {
                TSharedPtr<SDockTab> ExistingProjectSettingsTab =
                    FGlobalTabmanager::Get()->FindExistingLiveTab(FName("ProjectSettings"));

                if (!ExistingProjectSettingsTab.IsValid() ||
                    (!ExistingProjectSettingsTab->IsActive() && !ExistingProjectSettingsTab->IsForeground()))
                {
                    FModuleManager &ModuleManager = FModuleManager::Get();
                    IModuleInterface *RedpointEOSModule = ModuleManager.GetModule("OnlineSubsystemRedpointEOS");

                    if (this->Config->GetDedicatedServerClientId().IsEmpty() ||
                        this->Config->GetDedicatedServerClientSecret().IsEmpty() ||
                        this->Config->GetDedicatedServerPrivateKey().IsEmpty() ||
                        this->Config->GetDedicatedServerPublicKey().IsEmpty())
                    {
                        return TArray<FEOSCheckEntry>{FEOSCheckEntry(
                            "FEOSDedicatedServerMissingSettingsCheck::Misconfigured",
                            "You are missing required settings for dedicated servers to work in the editor, yet your "
                            "current play-in-editor preferences will try to start one. You must set values for at "
                            "least \"Dedicated Server Client Id\", \"Dedicated Server Client Secret\", \"Dedicated "
                            "Server Private Key\" and \"Dedicated Server Public Key\". If the public or private keys "
                            "are missing, you can clear those fields to generate a new keypair.",
                            TArray<FEOSCheckAction>{
                                FEOSCheckAction(
                                    "FEOSDedicatedServerMissingSettingsCheck::OpenProjectSettings",
                                    "Open settings"),
                                FEOSCheckAction(
                                    "FEOSDedicatedServerMissingSettingsCheck::OpenDocumentation",
                                    "Read documentation")})};
                    }
                }
            }
        }
    }

    return IEOSCheck::EmptyEntries;
}

void FEOSDedicatedServerMissingSettingsCheck::HandleAction(const FString &CheckId, const FString &ActionId) const
{
    if (CheckId == "FEOSDedicatedServerMissingSettingsCheck::Misconfigured")
    {
        if (ActionId == "FEOSDedicatedServerMissingSettingsCheck::OpenProjectSettings")
        {
            FModuleManager::LoadModuleChecked<ISettingsModule>("Settings")
                .ShowViewer(FName("Project"), FName("Game"), FName("Epic Online Services"));
        }
        else if (ActionId == "FEOSDedicatedServerMissingSettingsCheck::OpenDocumentation")
        {
            FPlatformProcess::LaunchURL(
                TEXT("https://docs.redpoint.games/eos-online-subsystem/docs/"
                     "dedis_preparing_your_game_server#creating-a-client-id-and-secret-for-your-game-server-binaries"),
                nullptr,
                nullptr);
        }
    }
}