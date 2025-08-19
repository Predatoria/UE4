// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/GetEASCTOAForExistingExternalCredentialsNode.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/SignOutEASAccountNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGamesCrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/MultiOperation.h"

EOS_ENABLE_STRICT_WARNINGS

void FGetEASContinuanceTokenOrAccountForExistingExternalCredentialsNode::OnResultCallback(
    const EOS_Auth_LoginCallbackInfo *Data,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<FAuthenticationGraphState> State,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FAuthenticationGraphNodeOnDone OnDone)
{
    if (Data->ResultCode == EOS_EResult::EOS_Success && EOSString_EpicAccountId::IsValid(Data->LocalUserId))
    {
        // Register the "on error" sign out logic.
        State->AddCleanupNode(
            MakeShared<FSignOutEASAccountNode>(MakeShared<FEpicGamesCrossPlatformAccountId>(Data->LocalUserId)));

        State->AuthenticatedCrossPlatformAccountId = MakeShared<FEpicGamesCrossPlatformAccountId>(Data->LocalUserId);
    }
    else if (Data->ResultCode == EOS_EResult::EOS_InvalidUser)
    {
        State->EASExternalContinuanceToken = Data->ContinuanceToken;
    }
    else
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Got unexpected result when trying to obtain continuance token or EAS account using external "
                 "credentials (%s)."),
            ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
    }

    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
}

void FGetEASContinuanceTokenOrAccountForExistingExternalCredentialsNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
    check(!EOSString_EpicAccountId::IsValid(State->GetAuthenticatedEpicAccountId()));
    checkf(
        State->ExistingExternalCredentials.IsValid(),
        TEXT("Expected the user to be signed in with external credentials already when running "
             "FGetEASContinuanceTokenOrAccountForExistingExternalCredentialsNode node."));

    FString DesktopCrossplayError = CheckForDesktopCrossplayError(State->EOSPlatform);
    if (!DesktopCrossplayError.IsEmpty())
    {
        if (State->Config->GetRequireCrossPlatformAccount())
        {
            // We will never be able to sign into an Epic Games account
            // without the service, and it's mandatory, so fail fast.
            State->ErrorMessages.Add(DesktopCrossplayError);
            OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        }
        else
        {
            // Emit a warning, since this might not block authenticating
            // with just the local account.
            UE_LOG(LogEOS, Warning, TEXT("%s"), *DesktopCrossplayError);
            OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        }
        return;
    }

    FEASAuthentication::DoRequestExternal(
        State->EOSAuth,
        State->ExistingExternalCredentials->GetId(),
        State->ExistingExternalCredentials->GetToken(),
        StrToExternalCredentialType(State->ExistingExternalCredentials->GetType()),
        FEASAuth_DoRequestComplete::CreateSP(
            this,
            &FGetEASContinuanceTokenOrAccountForExistingExternalCredentialsNode::OnResultCallback,
            State,
            OnDone));
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION