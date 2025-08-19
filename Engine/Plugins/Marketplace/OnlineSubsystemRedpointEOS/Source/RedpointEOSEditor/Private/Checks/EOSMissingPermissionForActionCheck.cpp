// Copyright June Rhodes. All Rights Reserved.

#include "EOSMissingPermissionForActionCheck.h"

#include "Internationalization/Regex.h"

const TArray<FEOSCheckEntry> FEOSMissingPermissionForActionCheck::ProcessLogMessage(
    EOS_ELogLevel InLogLevel,
    const FString &Category,
    const FString &Message) const
{
    if (Message.Contains("errors.com.epicgames.common.policy_missing_action"))
    {
        FRegexPattern Pattern(TEXT("ErrorCode=\\[errors\\.com\\.epicgames\\.common\\.policy_missing_action\\].+"
                                   "ErrorMessage=\\[Client ([a-zA-Z0-9]+) missing ([a-zA-Z0-9:]+)\\]"));
        FRegexMatcher Matcher(Pattern, Message);

        if (Matcher.FindNext())
        {
            FString ClientId = Matcher.GetCaptureGroup(1);
            FString MissingPermission = Matcher.GetCaptureGroup(2);

            if (MissingPermission == TEXT("matchmaking:createSession"))
            {
                return TArray<FEOSCheckEntry>{FEOSCheckEntry(
                    "FEOSMissingPermissionForActionCheck::MissingCreateSession",
                    FString::Printf(
                        TEXT("Your EOS Client ID '%s' does not have permission to create sessions due to the client "
                             "policy it is configured with in the Developer Portal. This is usually because you've "
                             "selected 'GameClient' as the policy, and you're trying to create sessions with a listen "
                             "server. If you don't need to register or unregister players to a session, you should "
                             "change the client policy to 'PeerToPeer'. If you do need to register or unregister "
                             "players to sessions, you'll need to create a custom policy and assign it to the "
                             "client, then grant that custom policy permission to create sessions and manage players "
                             "(under the 'Matchmaking' feature on the client policy)."),
                        *ClientId),
                    TArray<FEOSCheckAction>{FEOSCheckAction(
                        "FEOSMissingPermissionForActionCheck::OpenDevPortal",
                        "Open Developer Portal")})};
            }
            else if (MissingPermission == TEXT("matchmaking:managePlayers"))
            {
                return TArray<FEOSCheckEntry>{FEOSCheckEntry(
                    "FEOSMissingPermissionForActionCheck::MissingManagePlayers",
                    FString::Printf(
                        TEXT("Your EOS Client ID '%s' does not have permission to register or unregister players from "
                             "sessions due to the client policy it is configured with in the Developer Portal. This is "
                             "usually because you've selected 'PeerToPeer' as the policy. You'll need to create a "
                             "custom policy instead, enable the managePlayers permission (under the 'Matchmaking' "
                             "feature) on the client policy, and then assign the custom policy to the client."),
                        *ClientId),
                    TArray<FEOSCheckAction>{FEOSCheckAction(
                        "FEOSMissingPermissionForActionCheck::OpenDevPortal",
                        "Open Developer Portal")})};
            }
            else
            {
                return TArray<FEOSCheckEntry>{FEOSCheckEntry(
                    "FEOSMissingPermissionForActionCheck::MissingGenericPermission",
                    FString::Printf(
                        TEXT("Your EOS Client ID '%s' does not have the '%s' permission due to the client "
                             "policy it is configured with in the Developer Portal. You'll need to enable the "
                             "permission on the custom client policy you've assigned to the client. If you're not "
                             "currently using a custom client policy, you'll need to create one and then assign it to "
                             "the client first."),
                        *ClientId,
                        *MissingPermission),
                    TArray<FEOSCheckAction>{FEOSCheckAction(
                        "FEOSMissingPermissionForActionCheck::OpenDevPortal",
                        "Open Developer Portal")})};
            }
        }
    }

    if (Message.Contains("errors.com.epicgames.common.missing_feature"))
    {
        FRegexPattern Pattern(TEXT("ServiceName=\\[([a-zA-Z0-9:]+)\\].+OperationName=\\[([a-zA-Z0-9:]+)\\].+ErrorCode="
                                   "\\[errors\\.com\\.epicgames\\.common\\.missing_feature\\]"));
        FRegexMatcher Matcher(Pattern, Message);

        if (Matcher.FindNext())
        {
            FString ServiceName = Matcher.GetCaptureGroup(1);
            FString OperationName = Matcher.GetCaptureGroup(2);

            return TArray<FEOSCheckEntry>{FEOSCheckEntry(
                "FEOSMissingPermissionForActionCheck::MissingFeature",
                FString::Printf(
                    TEXT("Your EOS Client ID can not call the '%s' API because the '%s' service is turned off, usually "
                         "due to the client policy configuration. Check your client policies have the service enabled "
                         "and check that you've agreed to the latest EOS service agreements (as the owner account of "
                         "the organisation), and then try again."),
                    *OperationName,
                    *ServiceName),
                TArray<FEOSCheckAction>{
                    FEOSCheckAction("FEOSMissingPermissionForActionCheck::OpenDevPortal", "Open Developer Portal")})};
        }
    }

    return IEOSCheck::EmptyEntries;
}

void FEOSMissingPermissionForActionCheck::HandleAction(const FString &CheckId, const FString &ActionId) const
{
    if (CheckId.StartsWith("FEOSMissingPermissionForActionCheck::"))
    {
        if (ActionId == "FEOSMissingPermissionForActionCheck::OpenDevPortal")
        {
            FPlatformProcess::LaunchURL(TEXT("https://dev.epicgames.com/portal/"), nullptr, nullptr);
        }
    }
}