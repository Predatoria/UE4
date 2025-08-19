// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/TryPersistentEASCredentialsNode.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/SignOutEASAccountNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGamesCrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"

EOS_ENABLE_STRICT_WARNINGS

void FTryPersistentEASCredentialsNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
    check(!EOSString_EpicAccountId::IsValid(State->GetAuthenticatedEpicAccountId()));

    // If we have persistent authentication turned off, skip this node.
    if (!State->Config->GetPersistentLoginEnabled())
    {
        // Don't attempt persistent authentication.
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        return;
    }

    // Load the persistent authentication token if it's set.
    FString AuthenticationToken = TEXT("");
    if (State->Metadata.Contains(EOS_METADATA_EAS_REFRESH_TOKEN))
    {
        AuthenticationToken = State->Metadata[EOS_METADATA_EAS_REFRESH_TOKEN].GetValue<FString>();
    }

    // If this platform doesn't support logging in with no persistent token, or if we don't have a
    // persistent token and we're not logging in for the first user, do not attempt persistent authentication.
    bool bDoNotAttemptOnEmptyRefreshToken =
        State->Metadata.Contains(EOS_METADATA_NO_EMPTY_EAS_REFRESH_TOKEN_ATTEMPT) &&
        State->Metadata[EOS_METADATA_NO_EMPTY_EAS_REFRESH_TOKEN_ATTEMPT].GetValue<bool>();
    if ((bDoNotAttemptOnEmptyRefreshToken || State->LocalUserNum != 0) && AuthenticationToken.IsEmpty())
    {
        // Don't attempt persistent authentication.
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        return;
    }

    UE_LOG(LogEOS, Verbose, TEXT("TryPersistentEASCredentialsNode: Attempting persistent authentication"));

    FEASAuthentication::DoRequest(
        State->EOSAuth,
        TEXT(""),
        AuthenticationToken,
        EOS_ELoginCredentialType::EOS_LCT_PersistentAuth,
        FEASAuth_DoRequestComplete::CreateLambda(
            [WeakThis = GetWeakThis(this), State, OnDone](const EOS_Auth_LoginCallbackInfo *Data) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    if (Data->ResultCode != EOS_EResult::EOS_Success ||
                        !EOSString_EpicAccountId::IsValid(Data->LocalUserId))
                    {
                        // Unable to authenticate with persistent token.
                        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
                        return;
                    }

                    // Register the "on error" sign out logic.
                    State->AddCleanupNode(MakeShared<FSignOutEASAccountNode>(
                        MakeShared<FEpicGamesCrossPlatformAccountId>(Data->LocalUserId)));

                    // Store how we authenticated with Epic.
                    State->ResultUserAuthAttributes.Add("epic.authenticatedWith", "persistent");

                    // Set the authenticated Epic account ID into the state.
                    State->AuthenticatedCrossPlatformAccountId =
                        MakeShared<FEpicGamesCrossPlatformAccountId>(Data->LocalUserId);
                    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
                    return;
                }
            }));
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION