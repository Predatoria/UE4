// Copyright June Rhodes. All Rights Reserved.

#include "EOSMissingScopeAuthorizationCheck.h"

#include "../Launchers/AuthorizeLauncher.h"

const TArray<FEOSCheckEntry> FEOSMissingScopeAuthorizationCheck::ProcessLogMessage(
    EOS_ELogLevel InLogLevel,
    const FString &Category,
    const FString &Message) const
{
    if (Message.Contains("scope_consent_required") || Message.Contains("corrective_action_required"))
    {
        return TArray<FEOSCheckEntry>{FEOSCheckEntry(
            "FEOSMissingScopeAuthorizationCheck::Misconfigured",
            "One or more Epic Games accounts you are trying to use with the Developer Authentication Tool need to go "
            "through the scope approval flow.",
            TArray<FEOSCheckAction> {
#if PLATFORM_WINDOWS && EOS_VERSION_AT_LEAST(1, 15, 0)
                FEOSCheckAction(
                    "FEOSMissingScopeAuthorizationCheck::LaunchAuthorizer",
                    "Authorize an Epic Games account"),
#endif
                    FEOSCheckAction("FEOSMissingScopeAuthorizationCheck::OpenDocumentation", "Read documentation")
            })};
    }
    return IEOSCheck::EmptyEntries;
}

void FEOSMissingScopeAuthorizationCheck::HandleAction(const FString &CheckId, const FString &ActionId) const
{
    if (CheckId == "FEOSMissingScopeAuthorizationCheck::Misconfigured")
    {
#if PLATFORM_WINDOWS && EOS_VERSION_AT_LEAST(1, 15, 0)
        if (ActionId == "FEOSMissingScopeAuthorizationCheck::LaunchAuthorizer")
        {
            FAuthorizeLauncher::Launch(true);
        }
        else
#endif
            if (ActionId == "FEOSMissingScopeAuthorizationCheck::OpenDocumentation")
        {
            FPlatformProcess::LaunchURL(
                TEXT("https://docs.redpoint.games/eos-online-subsystem/docs/auth_dev_auth_tool_usage"),
                nullptr,
                nullptr);
        }
    }
}