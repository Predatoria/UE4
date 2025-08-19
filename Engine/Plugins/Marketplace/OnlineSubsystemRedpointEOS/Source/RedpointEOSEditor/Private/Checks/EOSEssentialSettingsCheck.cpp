// Copyright June Rhodes. All Rights Reserved.

#include "EOSEssentialSettingsCheck.h"

#include "../OnlineSubsystemEOSEditorConfig.h"
#include "Framework/Docking/TabManager.h"
#include "ISettingsModule.h"
#include "OnlineSubsystemRedpointEOSModule.h"
#include "UnrealEdMisc.h"
#include "Widgets/Docking/SDockTab.h"

FEOSEssentialSettingsCheck::FEOSEssentialSettingsCheck()
    : Config(MakeShared<FEOSConfigIni>())
{
}

const TArray<FEOSCheckEntry> FEOSEssentialSettingsCheck::Tick(float DeltaSeconds) const
{
    TSharedPtr<SDockTab> ExistingProjectSettingsTab =
        FGlobalTabmanager::Get()->FindExistingLiveTab(FName("ProjectSettings"));

    if (!ExistingProjectSettingsTab.IsValid() ||
        (!ExistingProjectSettingsTab->IsActive() && !ExistingProjectSettingsTab->IsForeground()))
    {
        FModuleManager &ModuleManager = FModuleManager::Get();
        IModuleInterface *RedpointEOSModule = ModuleManager.GetModule("OnlineSubsystemRedpointEOS");

        if (this->Config->GetProductName().IsEmpty() || this->Config->GetProductVersion().IsEmpty())
        {
            return TArray<FEOSCheckEntry>{FEOSCheckEntry(
                "FEOSEssentialSettingsCheck::Misconfigured",
                "You are missing essential settings that are required for Epic Online Services to work when the "
                "editor started. You must set values for at least \"Product Name\" and \"Product Version\" and then "
                "restart the editor. Epic Online Services will not work until the editor is restarted after setting "
                "these values.",
                TArray<FEOSCheckAction>{
                    FEOSCheckAction("FEOSEssentialSettingsCheck::OpenProjectSettings", "Open settings"),
                    FEOSCheckAction("FEOSEssentialSettingsCheck::OpenDocumentation", "Read documentation")})};
        }
        else if (
            RedpointEOSModule != nullptr &&
            ((FOnlineSubsystemRedpointEOSModule *)RedpointEOSModule)->IsOperatingInNullMode())
        {
            return TArray<FEOSCheckEntry>{FEOSCheckEntry(
                "FEOSEssentialSettingsCheck::EditorRestartRequired",
                "You were missing essential settings that are required for Epic Online Services to work when the "
                "editor started. These values have been set, but you must now restart the editor for the EOS plugin to "
                "start working.",
                TArray<FEOSCheckAction>{
                    FEOSCheckAction("FEOSEssentialSettingsCheck::RestartEditor", "Restart editor")})};
        }

        if (this->Config->GetProductId().IsEmpty() || this->Config->GetSandboxId().IsEmpty() ||
            this->Config->GetDeploymentId().IsEmpty() || this->Config->GetClientId().IsEmpty() ||
            this->Config->GetClientSecret().IsEmpty())
        {
            return TArray<FEOSCheckEntry>{FEOSCheckEntry(
                "FEOSEssentialSettingsCheck::Misconfigured",
                "You're missing essential settings that are required for Epic Online Services to work.",
                TArray<FEOSCheckAction>{
                    FEOSCheckAction("FEOSEssentialSettingsCheck::OpenProjectSettings", "Open settings"),
                    FEOSCheckAction("FEOSEssentialSettingsCheck::OpenDocumentation", "Read documentation")})};
        }
    }

    return IEOSCheck::EmptyEntries;
}

void FEOSEssentialSettingsCheck::HandleAction(const FString &CheckId, const FString &ActionId) const
{
    if (CheckId == "FEOSEssentialSettingsCheck::Misconfigured" ||
        CheckId == "FEOSEssentialSettingsCheck::EditorRestartRequired")
    {
        if (ActionId == "FEOSEssentialSettingsCheck::OpenProjectSettings")
        {
            FModuleManager::LoadModuleChecked<ISettingsModule>("Settings")
                .ShowViewer(FName("Project"), FName("Game"), FName("Epic Online Services"));
        }
        else if (ActionId == "FEOSEssentialSettingsCheck::RestartEditor")
        {
            const bool bWarn = false;
            FUnrealEdMisc::Get().RestartEditor(bWarn);
        }
        else if (ActionId == "FEOSEssentialSettingsCheck::OpenDocumentation")
        {
            FPlatformProcess::LaunchURL(
                TEXT("https://docs.redpoint.games/eos-online-subsystem/docs/core_getting_started"),
                nullptr,
                nullptr);
        }
    }
}