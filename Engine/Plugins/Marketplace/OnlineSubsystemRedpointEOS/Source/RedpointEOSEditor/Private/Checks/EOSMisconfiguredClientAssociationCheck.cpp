// Copyright June Rhodes. All Rights Reserved.

#include "EOSMisconfiguredClientAssociationCheck.h"

const TArray<FEOSCheckEntry> FEOSMisconfiguredClientAssociationCheck::ProcessLogMessage(
    EOS_ELogLevel InLogLevel,
    const FString &Category,
    const FString &Message) const
{
    if (Message.Contains("Client is not associated with an application") || Message.Contains("The Client is not configured correctly. Please make sure it is associated with an Application."))
    {
        return TArray<FEOSCheckEntry>{FEOSCheckEntry(
            "FEOSMisconfiguredClientAssociationCheck::Misconfigured",
            "Your EOS Client ID is not associated with an Epic Games application in the Developer Portal. Signing into "
            "Epic Games accounts can not work until this is fixed.",
            TArray<FEOSCheckAction>{
                FEOSCheckAction("FEOSMisconfiguredClientAssociationCheck::OpenDocumentation", "Read documentation")})};
    }
    return IEOSCheck::EmptyEntries;
}

void FEOSMisconfiguredClientAssociationCheck::HandleAction(const FString &CheckId, const FString &ActionId) const
{
    if (CheckId == "FEOSMisconfiguredClientAssociationCheck::Misconfigured")
    {
        if (ActionId == "FEOSMisconfiguredClientAssociationCheck::OpenDocumentation")
        {
            FPlatformProcess::LaunchURL(
                TEXT("https://docs.redpoint.games/eos-online-subsystem/docs/auth_xplat_epic#prerequisites"),
                nullptr,
                nullptr);
        }
    }
}