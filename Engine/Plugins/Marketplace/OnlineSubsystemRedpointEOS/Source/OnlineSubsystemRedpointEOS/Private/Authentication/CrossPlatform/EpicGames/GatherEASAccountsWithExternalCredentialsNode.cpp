// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/GatherEASAccountsWithExternalCredentialsNode.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/SignOutEASAccountNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/TryPersistentEASCredentialsNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGamesCrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/MultiOperation.h"

EOS_ENABLE_STRICT_WARNINGS

void FGatherEASAccountsWithExternalCredentialsNode::OnResultCallback(
    const EOS_Auth_LoginCallbackInfo *Data,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<FAuthenticationGraphState> State,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<IOnlineExternalCredentials> ExternalCredentials,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    std::function<void(FGatherEASAccountResponse)> OnPlatformDone)
{
    if (Data->ResultCode == EOS_EResult::EOS_Success && EOSString_EpicAccountId::IsValid(Data->LocalUserId))
    {
        // Add the native platform's attributes.
        for (const auto &KV : ExternalCredentials->GetAuthAttributes())
        {
            if (KV.Key != EOS_AUTH_ATTRIBUTE_AUTHENTICATEDWITH)
            {
                State->ResultUserAuthAttributes.Add(KV.Key, KV.Value);
            }
            else
            {
                State->ResultUserAuthAttributes.Add("epic.authenticatedWith", KV.Value);
            }
        }

        // Pass the native subsystem on.
        State->Metadata.Add(EOS_METADATA_EAS_NATIVE_SUBSYSTEM, ExternalCredentials->GetNativeSubsystemName());

        // Register the "on error" sign out logic.
        State->AddCleanupNode(
            MakeShared<FSignOutEASAccountNode>(MakeShared<FEpicGamesCrossPlatformAccountId>(Data->LocalUserId)));

        // Return the Epic Games account.
        OnPlatformDone(FGatherEASAccountResponse(Data->LocalUserId, nullptr));
    }
    else if (Data->ResultCode == EOS_EResult::EOS_InvalidUser)
    {
        // This credential is not associated with any Epic Games account.
        OnPlatformDone(FGatherEASAccountResponse(nullptr, Data->ContinuanceToken));
    }
    else
    {
        State->ErrorMessages.Add(FString::Printf(
            TEXT("FGatherEASAccountsWithExternalCredentialsNode: External credential "
                 "'%s' failed to authenticate with EAS: %s"),
            *ExternalCredentials->GetType(),
            ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode))));
        OnPlatformDone(FGatherEASAccountResponse(nullptr, nullptr));
    }
}

void FGatherEASAccountsWithExternalCredentialsNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnStepDone)
{
    check(!EOSString_EpicAccountId::IsValid(State->GetAuthenticatedEpicAccountId()));

    FString DesktopCrossplayError = CheckForDesktopCrossplayError(State->EOSPlatform);
    if (!DesktopCrossplayError.IsEmpty())
    {
        if (State->Config->GetRequireCrossPlatformAccount())
        {
            // We will never be able to sign into an Epic Games account
            // without the service, and it's mandatory, so fail fast.
            State->ErrorMessages.Add(DesktopCrossplayError);
            OnStepDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        }
        else
        {
            // Emit a warning, since this might not block authenticating
            // with just the local account.
            UE_LOG(LogEOS, Warning, TEXT("%s"), *DesktopCrossplayError);
            OnStepDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        }
        return;
    }

    FMultiOperation<TSharedRef<IOnlineExternalCredentials>, FGatherEASAccountResponse>::Run(
        State->AvailableExternalCredentials,
        [WeakThis = GetWeakThis(this), State](
            const TSharedRef<IOnlineExternalCredentials> &ExternalCredentials,
            std::function<void(FGatherEASAccountResponse)> OnPlatformDone) -> bool {
            if (auto This = StaticCastSharedPtr<FGatherEASAccountsWithExternalCredentialsNode>(PinWeakThis(WeakThis)))
            {
                FEASAuthentication::DoRequestExternal(
                    State->EOSAuth,
                    ExternalCredentials->GetId(),
                    ExternalCredentials->GetToken(),
                    StrToExternalCredentialType(ExternalCredentials->GetType()),
                    FEASAuth_DoRequestComplete::CreateSP(
                        This.ToSharedRef(),
                        &FGatherEASAccountsWithExternalCredentialsNode::OnResultCallback,
                        State,
                        ExternalCredentials,
                        MoveTemp(OnPlatformDone)));
                return true;
            }
            return false;
        },
        [State, OnStepDone](const TArray<FGatherEASAccountResponse> &Results) {
            // Select the first valid Epic Account ID from the results.
            for (auto Result : Results)
            {
                if (EOSString_EpicAccountId::IsValid(Result.Key))
                {
                    State->AuthenticatedCrossPlatformAccountId =
                        MakeShared<FEpicGamesCrossPlatformAccountId>(Result.Key);
                    break;
                }
            }

            // It no account was found, select the first continuance token.
            if (!State->AuthenticatedCrossPlatformAccountId.IsValid())
            {
                for (auto Result : Results)
                {
                    if (!EOSString_ContinuanceToken::IsNone(Result.Value))
                    {
                        State->EASExternalContinuanceToken = Result.Value;
                        break;
                    }
                }
            }

            OnStepDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        });
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION