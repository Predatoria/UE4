// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#if !defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/PerformAutomatedTestingEASLoginNode.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/SignOutEASAccountNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGamesCrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"

EOS_ENABLE_STRICT_WARNINGS

void FPerformAutomatedTestingEASLoginNode::HandleEASAuthenticationCallback(
    const EOS_Auth_LoginCallbackInfo *Data,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<FAuthenticationGraphState> State,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FAuthenticationGraphNodeOnDone OnDone)
{
    if (Data->ResultCode != EOS_EResult::EOS_Success || !EOSString_EpicAccountId::IsValid(Data->LocalUserId))
    {
        State->ErrorMessages.Add(FString::Printf(
            TEXT("PerformAutomatedTestingEASLoginNode: Failed to login with automated testing account: %s"),
            ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode))));
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    // Register the "on error" sign out logic.
    State->AddCleanupNode(
        MakeShared<FSignOutEASAccountNode>(MakeShared<FEpicGamesCrossPlatformAccountId>(Data->LocalUserId)));

    // Store how we authenticated with Epic.
    State->ResultUserAuthAttributes.Add("epic.authenticatedWith", "automatedTesting");

    State->AuthenticatedCrossPlatformAccountId = MakeShared<FEpicGamesCrossPlatformAccountId>(Data->LocalUserId);
    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
    return;
}

void FPerformAutomatedTestingEASLoginNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
    check(!EOSString_EpicAccountId::IsValid(State->GetAuthenticatedEpicAccountId()));

    if (State->AutomatedTestingEmailAddress.IsEmpty() || State->AutomatedTestingPassword.IsEmpty())
    {
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        return;
    }

    FEASAuthentication::DoRequest(
        State->EOSAuth,
        State->AutomatedTestingEmailAddress,
        State->AutomatedTestingPassword,
        EOS_ELoginCredentialType::EOS_LCT_Password,
        FEASAuth_DoRequestComplete::CreateSP(
            this,
            &FPerformAutomatedTestingEASLoginNode::HandleEASAuthenticationCallback,
            State,
            OnDone));
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if !UE_BUILD_SHIPPING

#endif // #if EOS_HAS_AUTHENTICATION