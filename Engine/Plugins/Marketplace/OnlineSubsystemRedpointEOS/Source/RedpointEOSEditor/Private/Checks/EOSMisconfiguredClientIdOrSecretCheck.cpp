// Copyright June Rhodes. All Rights Reserved.

#include "EOSMisconfiguredClientIdOrSecretCheck.h"

const TArray<FEOSCheckEntry> FEOSMisconfiguredClientIdOrSecretCheck::ProcessLogMessage(
    EOS_ELogLevel InLogLevel,
    const FString &Category,
    const FString &Message) const
{
    if (Message.Contains("EOS_Auth_WrongClient") ||
        Message.Contains("errors.com.epicgames.common.oauth.invalid_client"))
    {
        return TArray<FEOSCheckEntry>{FEOSCheckEntry(
            "FEOSMisconfiguredClientIdOrSecretCheck::Misconfigured",
            "The client secret is not correct for your EOS Client ID, or the EOS Client ID you supplied does not "
            "belong to the product identified by your EOS Product ID. Check the Project Settings match the IDs and "
            "secrets provided in the Developer Portal and try again.",
            TArray<FEOSCheckAction>{
                FEOSCheckAction("FEOSMisconfiguredClientIdOrSecretCheck::OpenDevPortal", "Open Developer Portal"),
                FEOSCheckAction("FEOSMisconfiguredClientIdOrSecretCheck::OpenDocumentation", "Read documentation")})};
    }
    return IEOSCheck::EmptyEntries;
}

void FEOSMisconfiguredClientIdOrSecretCheck::HandleAction(const FString &CheckId, const FString &ActionId) const
{
    if (CheckId == "FEOSMisconfiguredClientIdOrSecretCheck::Misconfigured")
    {
        if (ActionId == "FEOSMisconfiguredClientIdOrSecretCheck::OpenDevPortal")
        {
            FPlatformProcess::LaunchURL(TEXT("https://dev.epicgames.com/portal/"), nullptr, nullptr);
        }
        else if (ActionId == "FEOSMisconfiguredClientIdOrSecretCheck::OpenDocumentation")
        {
            FPlatformProcess::LaunchURL(
                TEXT("https://docs.redpoint.games/eos-online-subsystem/docs/"
                     "core_getting_started#create-the-client-id-and-secret-for-your-product"),
                nullptr,
                nullptr);
        }
    }
}