// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/PerformInteractiveEASLoginNode.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/SignOutEASAccountNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGamesCrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"

EOS_ENABLE_STRICT_WARNINGS

void FPerformInteractiveEASLoginNode::OnSignInCancel(
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

    State->ErrorMessages.Add(TEXT("Sign in was cancelled."));
    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
    return;
}

void FPerformInteractiveEASLoginNode::HandleEASAuthenticationCallback(
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
            &FPerformInteractiveEASLoginNode::OnSignInCancel,
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
        // Authentication failed. In this case, the user has interactively logged in. It's be weird if
        // the user logged in through their web browser, we get a backend error and then continue to
        // sign them in with some other unrelated account.
        //
        // So instead we treat this as a full authentication failure.
        State->ErrorMessages.Add(FString::Printf(
            TEXT("PerformInteractiveEASLoginNode: Interactive authentication failed with error %s"),
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

    // If we have a continuance token from a Nintendo NSA ID token, we now need to link the NSA account with the Epic
    // account.
    bool bIsNSAContinuanceToken = State->EASExternalContinuanceToken != nullptr &&
                                  State->Metadata.Contains("EAS_EXTERNAL_CONTINUANCE_TOKEN_IS_NSA") &&
                                  State->Metadata["EAS_EXTERNAL_CONTINUANCE_TOKEN_IS_NSA"].GetValue<bool>();
    if (bIsNSAContinuanceToken)
    {
        FEASAuthentication::DoRequestLink(
            State->EOSAuth,
            State->EASExternalContinuanceToken,
            EOS_ELinkAccountFlags::EOS_LA_NintendoNsaId,
            State->GetAuthenticatedEpicAccountId(),
            FEASAuth_DoRequestLinkComplete::CreateLambda(
                [WeakThis = GetWeakThis(this), State, OnDone](const EOS_Auth_LinkAccountCallbackInfo *Data) {
                    if (auto This = StaticCastSharedPtr<FPerformInteractiveEASLoginNode>(PinWeakThis(WeakThis)))
                    {
                        if (Data->ResultCode != EOS_EResult::EOS_Success)
                        {
                            UE_LOG(
                                LogEOS,
                                Warning,
                                TEXT("PerformInteractiveEASLoginNode: Failed to associate NSA ID token with Epic Games "
                                     "account: "
                                     "%s"),
                                ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
                        }

                        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
                    }
                }));
        return;
    }

    // Otherwise, we are done.
    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
}

void FPerformInteractiveEASLoginNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
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

    bool bIsNSAContinuanceToken = State->EASExternalContinuanceToken != nullptr &&
                                  State->Metadata.Contains("EAS_EXTERNAL_CONTINUANCE_TOKEN_IS_NSA") &&
                                  State->Metadata["EAS_EXTERNAL_CONTINUANCE_TOKEN_IS_NSA"].GetValue<bool>();

    if (State->EASExternalContinuanceToken != nullptr && !bIsNSAContinuanceToken)
    {
        FEASAuthentication::DoRequestLink(
            State->EOSAuth,
            State->EASExternalContinuanceToken,
            EOS_ELinkAccountFlags::EOS_LA_NoFlags,
            nullptr,
            FEASAuth_DoRequestLinkComplete::CreateLambda(
                [WeakThis = GetWeakThis(this), State, OnDone](const EOS_Auth_LinkAccountCallbackInfo *Data) {
                    if (auto This = StaticCastSharedPtr<FPerformInteractiveEASLoginNode>(PinWeakThis(WeakThis)))
                    {
                        This->HandleEASAuthenticationCallback(FEOSAuthCallbackInfo(Data), State, OnDone);
                    }
                }));
    }
    else
    {
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_IOS || PLATFORM_ANDROID || PLATFORM_LINUX
        auto CredentialType = EOS_ELoginCredentialType::EOS_LCT_AccountPortal;
#else
        auto CredentialType = EOS_ELoginCredentialType::EOS_LCT_DeviceCode;
#endif

        FEASAuthentication::DoRequest(
            State->EOSAuth,
            TEXT(""),
            TEXT(""),
            CredentialType,
            FEASAuth_DoRequestComplete::CreateLambda(
                [WeakThis = GetWeakThis(this), State, OnDone](const EOS_Auth_LoginCallbackInfo *Data) {
                    if (auto This = StaticCastSharedPtr<FPerformInteractiveEASLoginNode>(PinWeakThis(WeakThis)))
                    {
                        This->HandleEASAuthenticationCallback(FEOSAuthCallbackInfo(Data), State, OnDone);
                    }
                }));
    }
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION