// Copyright June Rhodes. All Rights Reserved.

#include "EOSEditorRequiresRestartForOSSCheck.h"

#include "../Launchers/DevAuthToolLauncher.h"
#include "OnlineSubsystem.h"
#include "UnrealEdMisc.h"

const TArray<FEOSCheckEntry> FEOSEditorRequiresRestartForOSSCheck::ProcessCustomSignal(
    const FString &Context,
    const FString &SignalId) const
{
    if (Context == TEXT("Configuration") && SignalId == TEXT("NotConfiguredForEOS"))
    {
        // NOLINTNEXTLINE(unreal-ionlinesubsystem-get)
        IOnlineSubsystem *OSS = IOnlineSubsystem::Get();
        if (OSS == nullptr || !OSS->GetSubsystemName().IsEqual(FName(TEXT("RedpointEOS"))))
        {
            return TArray<FEOSCheckEntry>{FEOSCheckEntry(
                "FEOSEditorRequiresRestartForOSSCheck::NotRunning",
                "EOS Online Subsystem has set itself as the online subsystem, but this change requires an editor "
                "restart to take effect.",
                TArray<FEOSCheckAction>{
                    FEOSCheckAction("FEOSEditorRequiresRestartForOSSCheck::RestartEditor", "Restart editor"),
                    FEOSCheckAction("FEOSEditorRequiresRestartForOSSCheck::OpenDocumentation", "Read documentation")})};
        }
    }
    return IEOSCheck::EmptyEntries;
}

void FEOSEditorRequiresRestartForOSSCheck::HandleAction(const FString &CheckId, const FString &ActionId) const
{
    if (CheckId == "FEOSEditorRequiresRestartForOSSCheck::NotRunning")
    {
        if (ActionId == "FEOSEditorRequiresRestartForOSSCheck::RestartEditor")
        {
            const bool bWarn = false;
            FUnrealEdMisc::Get().RestartEditor(bWarn);
        }
        else if (ActionId == "FEOSEditorRequiresRestartForOSSCheck::OpenDocumentation")
        {
            FPlatformProcess::LaunchURL(
                TEXT("https://docs.redpoint.games/eos-online-subsystem/docs/core_getting_started"),
                nullptr,
                nullptr);
        }
    }
}