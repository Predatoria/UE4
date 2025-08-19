// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/TryExchangeCodeAuthenticationNode.h"

#include "Misc/CommandLine.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/SignOutEASAccountNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/TryPersistentEASCredentialsNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGamesCrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"

EOS_ENABLE_STRICT_WARNINGS

void FTryExchangeCodeAuthenticationNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
    FString AuthType;
    if (!FParse::Value(FCommandLine::Get(), TEXT("AUTH_TYPE="), AuthType, true))
    {
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        return;
    }
    if (AuthType != TEXT("exchangecode"))
    {
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        return;
    }

    UE_LOG(
        LogEOS,
        Verbose,
        TEXT("TryExchangeCodeAuthenticationNode: Detected Epic Games Launcher, performing exchange code "
             "authentication."));

    // Try doing auth with EOS_LCT_ExchangeCode, which is the mechanism used by Epic Games Launcher.
    FString ExchangeCode;
    if (!FParse::Value(FCommandLine::Get(), TEXT("AUTH_PASSWORD="), ExchangeCode, true))
    {
        // No provided exchange code, even though we expected to get one.
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("TryExchangeCodeAuthenticationNode: Missing -AUTH_PASSWORD= parameter for exchange code "
                 "authentication."));
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        return;
    }

    FEASAuthentication::DoRequest(
        State->EOSAuth,
        TEXT(""),
        ExchangeCode,
        EOS_ELoginCredentialType::EOS_LCT_ExchangeCode,
        FEASAuth_DoRequestComplete::CreateLambda(
            [WeakThis = GetWeakThis(this), State, OnDone](const EOS_Auth_LoginCallbackInfo *Data) {
                if (auto This = StaticCastSharedPtr<FTryExchangeCodeAuthenticationNode>(PinWeakThis(WeakThis)))
                {
                    if (Data->ResultCode != EOS_EResult::EOS_Success ||
                        !EOSString_EpicAccountId::IsValid(Data->LocalUserId))
                    {
                        // Unable to authenticate with exchange code.
                        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
                        return;
                    }

                    // Register the "on error" sign out logic.
                    State->AddCleanupNode(MakeShared<FSignOutEASAccountNode>(
                        MakeShared<FEpicGamesCrossPlatformAccountId>(Data->LocalUserId)));

                    // This is *definitely* the Epic Games subsystem if we're doing exchange code.
                    State->Metadata.Add(EOS_METADATA_EAS_NATIVE_SUBSYSTEM, REDPOINT_EAS_SUBSYSTEM);

                    // Store how we authenticated with Epic.
                    State->ResultUserAuthAttributes.Add("epic.authenticatedWith", "exchangeCode");

                    // Set the authenticated Epic account ID into the state.
                    State->AuthenticatedCrossPlatformAccountId =
                        MakeShared<FEpicGamesCrossPlatformAccountId>(Data->LocalUserId);
                    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
                    return;
                }
            }));
#else
    // This authentication mode is not supported on non-desktop platforms.
    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
#endif
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION