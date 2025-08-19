// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/PerformInteractiveLinkExternalCredentialsToEASAccountNode.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/SignOutEASAccountNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGamesCrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"

EOS_ENABLE_STRICT_WARNINGS

void FPerformInteractiveLinkExternalCredentialsToEASAccountNode::OnLinkCancel(
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<FAuthenticationGraphState> State,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FAuthenticationGraphNodeOnDone OnDone)
{
    if (this->Widget.IsValid())
    {
        this->Widget->RemoveFromState(State);
        this->Widget.Reset();
    }

    State->ErrorMessages.Add(TEXT("Account link was cancelled."));
    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
    return;
}

void FPerformInteractiveLinkExternalCredentialsToEASAccountNode::HandleEASAuthenticationCallback(
    const FEOSAuthCallbackInfo &Data,
    const TSharedRef<FAuthenticationGraphState> &State,
    const FAuthenticationGraphNodeOnDone &OnDone)
{
    if (Data.PinGrantInfo() != nullptr && Data.ResultCode() == EOS_EResult::EOS_Auth_PinGrantCode)
    {
        // The user needs to enter a PIN code on another device. Show the UI.

        check(this->Widget == nullptr);

        this->Widget = MakeShared<TUserInterfaceRef<
            IEOSUserInterface_EnterDevicePinCode,
            UEOSUserInterface_EnterDevicePinCode,
            UEOSUserInterface_EnterDevicePinCode_Context>>(
            State,
            State->Config->GetWidgetClass(
                TEXT("EnterDevicePinCode"),
                TEXT("/OnlineSubsystemRedpointEOS/"
                     "EOSDefaultUserInterface_EnterDevicePinCode.EOSDefaultUserInterface_EnterDevicePinCode_C")),
            TEXT("IEOSUserInterface_EnterDevicePinCode"));

        if (!this->Widget->IsValid())
        {
            State->ErrorMessages.Add(this->Widget->GetInvalidErrorMessage());
            this->Widget = nullptr;
            OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
            return;
        }

        check(!State->HasCurrentUserInterfaceWidget());

        this->Widget->GetContext()->SetOnCancel(UEOSUserInterface_EnterDevicePinCode_OnCancel::CreateSP(
            this,
            &FPerformInteractiveLinkExternalCredentialsToEASAccountNode::OnLinkCancel,
            State,
            OnDone));

        IEOSUserInterface_EnterDevicePinCode::Execute_SetupUserInterface(
            this->Widget->GetWidget(),
            this->Widget->GetContext(),
            ANSI_TO_TCHAR(Data.PinGrantInfo()->VerificationURI),
            ANSI_TO_TCHAR(Data.PinGrantInfo()->UserCode));

        this->Widget->AddToState(State);
        return;
    }

    // Remove the widget if it's currently visible.
    if (this->Widget.IsValid())
    {
        this->Widget->RemoveFromState(State);
        this->Widget.Reset();
    }

    if (Data.ResultCode() != EOS_EResult::EOS_Success || !EOSString_EpicAccountId::IsValid(Data.LocalUserId()))
    {
        // Link failed. Treat as a failure because the user has already done something interactive.
        State->ErrorMessages.Add(FString::Printf(
            TEXT("PerformInteractiveLinkExternalCredentialsToEASAccountNode: Interactive authentication failed with "
                 "error %s"),
            ANSI_TO_TCHAR(EOS_EResult_ToString(Data.ResultCode()))));
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    // Register the "on error" sign out logic.
    State->AddCleanupNode(
        MakeShared<FSignOutEASAccountNode>(MakeShared<FEpicGamesCrossPlatformAccountId>(Data.LocalUserId())));

    // Store how we authenticated with Epic.
    State->ResultUserAuthAttributes.Add("epic.authenticatedWith", "interactive");

    // Set the authenticated Epic account ID into the state.
    State->AuthenticatedCrossPlatformAccountId = MakeShared<FEpicGamesCrossPlatformAccountId>(Data.LocalUserId());

    // Otherwise, we are done.
    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
}

void FPerformInteractiveLinkExternalCredentialsToEASAccountNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
    check(!EOSString_EpicAccountId::IsValid(State->GetAuthenticatedEpicAccountId()));
    check(!EOSString_ContinuanceToken::IsNone(State->EASExternalContinuanceToken));

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

    auto Flags = EOS_ELinkAccountFlags::EOS_LA_NoFlags;
    if (State->ExistingExternalCredentials.IsValid() &&
        State->ExistingExternalCredentials->GetType() == "NINTENDO_NSA_ID_TOKEN")
    {
        Flags = EOS_ELinkAccountFlags::EOS_LA_NintendoNsaId;
    }

    FEASAuthentication::DoRequestLink(
        State->EOSAuth,
        State->EASExternalContinuanceToken,
        Flags,
        nullptr,
        FEASAuth_DoRequestLinkComplete::CreateLambda(
            [WeakThis = GetWeakThis(this), State, OnDone](const EOS_Auth_LinkAccountCallbackInfo *Data) {
                if (auto This = StaticCastSharedPtr<FPerformInteractiveLinkExternalCredentialsToEASAccountNode>(
                        PinWeakThis(WeakThis)))
                {
                    This->HandleEASAuthenticationCallback(FEOSAuthCallbackInfo(Data), State, OnDone);
                }
            }));
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION