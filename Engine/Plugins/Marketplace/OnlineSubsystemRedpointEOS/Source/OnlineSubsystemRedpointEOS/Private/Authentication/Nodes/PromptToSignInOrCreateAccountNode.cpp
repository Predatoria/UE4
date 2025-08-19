// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/PromptToSignInOrCreateAccountNode.h"

#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/UserInterface/EOSUserInterface_SignInOrCreateAccount.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"

EOS_ENABLE_STRICT_WARNINGS

#if !defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING
EEOSUserInterface_SignInOrCreateAccount_Choice FPromptToSignInOrCreateAccountNode_AutomationSetting::UserChoice =
    EEOSUserInterface_SignInOrCreateAccount_Choice::CreateAccount;
#endif

void FPromptToSignInOrCreateAccountNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
#if !defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING
    if (State->Config->IsAutomatedTesting())
    {
        // We are running through authentication unit tests, which are designed to test the logic flow of the OSS
        // authentication graph without actually requiring an external service.
        if (State->ProvidedCredentials.Id ==
            TEXT("CreateOnDemand:FOnlineSubsystemEOS_Authentication_CrossPlatformUpgradeFlowWorks"))
        {
            State->LastSignInChoice = EEOSUserInterface_SignInOrCreateAccount_Choice::CreateAccount;
        }
        else if (
            State->ProvidedCredentials.Id ==
            TEXT("CreateOnDemand:FOnlineSubsystemEOS_Authentication_CrossPlatformOptionalFlowWorks"))
        {
            State->LastSignInChoice = FPromptToSignInOrCreateAccountNode_AutomationSetting::UserChoice;
        }
        else
        {
            State->LastSignInChoice = EEOSUserInterface_SignInOrCreateAccount_Choice::SignIn;
        }
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        return;
    }
#endif

    check(this->Widget == nullptr);

    this->Widget = MakeShared<TUserInterfaceRef<
        IEOSUserInterface_SignInOrCreateAccount,
        UEOSUserInterface_SignInOrCreateAccount,
        UEOSUserInterface_SignInOrCreateAccount_Context>>(
        State,
        State->Config->GetWidgetClass(
            TEXT("SignInOrCreateAccount"),
            TEXT("/OnlineSubsystemRedpointEOS/"
                 "EOSDefaultUserInterface_SignInOrCreateAccount.EOSDefaultUserInterface_"
                 "SignInOrCreateAccount_C")),
        TEXT("IEOSUserInterface_SignInOrCreateAccount"));

    if (!this->Widget->IsValid())
    {
        State->ErrorMessages.Add(this->Widget->GetInvalidErrorMessage());
        this->Widget = nullptr;
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    check(!State->HasCurrentUserInterfaceWidget());

    this->Widget->GetContext()->SetOnChoice(UEOSUserInterface_SignInOrCreateAccount_OnChoice::CreateSP(
        this,
        &FPromptToSignInOrCreateAccountNode::SelectChoice,
        State,
        OnDone));

    IEOSUserInterface_SignInOrCreateAccount::Execute_SetupUserInterface(
        this->Widget->GetWidget(),
        this->Widget->GetContext());

    this->Widget->AddToState(State);
}

void FPromptToSignInOrCreateAccountNode::SelectChoice(
    EEOSUserInterface_SignInOrCreateAccount_Choice SelectedChoice,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<FAuthenticationGraphState> State,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FAuthenticationGraphNodeOnDone OnDone)
{
    State->LastSignInChoice = SelectedChoice;

    this->Widget->RemoveFromState(State);
    this->Widget = nullptr;

    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION