// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/TryDeveloperAuthenticationEASCredentialsNode.h"

#include "Engine/Engine.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/SignOutEASAccountNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGamesCrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"

EOS_ENABLE_STRICT_WARNINGS

void FTryDeveloperAuthenticationEASCredentialsNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
    check(!EOSString_EpicAccountId::IsValid(State->GetAuthenticatedEpicAccountId()));

#if WITH_EDITOR
    FString CredentialAddress = State->Config->GetDeveloperToolAddress();

    if (CredentialAddress.StartsWith(TEXT("localhost:")))
    {
#if PLATFORM_WINDOWS
        bool bIsDevToolRunning = FPlatformProcess::IsApplicationRunning(TEXT("EOS_DevAuthTool.exe"));
#else
        bool bIsDevToolRunning = FPlatformProcess::IsApplicationRunning(TEXT("EOS_DevAuthTool"));
#endif
        if (!bIsDevToolRunning)
        {
            // Developer Authentication Tool is not running, don't try to authenticate.
            OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
            return;
        }
    }

    if (!State->AutomatedTestingEmailAddress.IsEmpty())
    {
        // Prevent the Developer Authentication Tool being used during automated tests.
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        return;
    }

    FString CredentialName = this->GetCredentialName(State);
    if (CredentialName.IsEmpty())
    {
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        return;
    }
    if (State->AttemptedDeveloperAuthenticationCredentialNames.Contains(CredentialName))
    {
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        return;
    }

    UE_LOG(
        LogEOS,
        Verbose,
        TEXT("TryDeveloperAuthenticationEASCredentialsNode: Detected EOS Dev Tool, using address '%s' and credential "
             "name '%s'"),
        *CredentialAddress,
        *CredentialName);

    FEASAuthentication::DoRequest(
        State->EOSAuth,
        CredentialAddress,
        CredentialName,
        EOS_ELoginCredentialType::EOS_LCT_Developer,
        FEASAuth_DoRequestComplete::CreateLambda([WeakThis = GetWeakThis(this), State, OnDone, CredentialName](
                                                     const EOS_Auth_LoginCallbackInfo *Data) {
            if (auto This = StaticCastSharedPtr<FTryDeveloperAuthenticationEASCredentialsNode>(PinWeakThis(WeakThis)))
            {
                State->AttemptedDeveloperAuthenticationCredentialNames.Add(CredentialName);

                if (Data->ResultCode != EOS_EResult::EOS_Success ||
                    !EOSString_EpicAccountId::IsValid(Data->LocalUserId))
                {
                    // Unable to authenticate with developer tool.
                    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
                    return;
                }

                // Register the "on error" sign out logic.
                State->AddCleanupNode(MakeShared<FSignOutEASAccountNode>(
                    MakeShared<FEpicGamesCrossPlatformAccountId>(Data->LocalUserId)));

                // Store how we authenticated with Epic.
                State->ResultUserAuthAttributes.Add("epic.authenticatedWith", "devTool");

                // Set the authenticated Epic account ID into the state.
                State->AuthenticatedCrossPlatformAccountId =
                    MakeShared<FEpicGamesCrossPlatformAccountId>(Data->LocalUserId);
                OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
                return;
            }
        }));
    return;
#endif

    // Not built with editor support, skip developer authentication.
    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
}

FString FTryPIEDeveloperAuthenticationEASCredentialsNode::GetCredentialName(TSharedRef<FAuthenticationGraphState> State)
{
#if WITH_EDITOR
    FString CommandLine = FString(FCommandLine::Get()).ToUpper();
    if (CommandLine.Contains(TEXT("-GAME")))
    {
        // For standalone games launched from the editor, we actually have a hint at what context they're running for,
        // in the form of the GameUserSettingsINI path. The filename of this path contains the real "PIE instance" ID.
        TArray<FString> Tokens;
        TArray<FString> Switches;
        FCommandLine::Parse(*CommandLine, Tokens, Switches);
        for (const auto &Token : Tokens)
        {
#if defined(UE_5_0_OR_LATER)
            if (Token.StartsWith(TEXT("GAMEUSERSETTINGSINI=PIEGAMEUSERSETTINGS")))
            {
                int Start = Token.Find(TEXT("PIEGAMEUSERSETTINGS"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) +
                            FString(TEXT("PIEGAMEUSERSETTINGS")).Len();
                int End = Token.Len();
                FString PIENumberAsString = Token.Mid(Start, End - Start);
                return FString::Printf(TEXT("Context_%d"), FCString::Atoi(*PIENumberAsString) + 1);
            }
#else
            if (Token.StartsWith(TEXT("GAMEUSERSETTINGSINI")) && Token.EndsWith(TEXT(".INI")))
            {
                int Start = Token.Find(TEXT("PIEGAMEUSERSETTINGS"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) +
                            FString(TEXT("PIEGAMEUSERSETTINGS")).Len();
                int End = Token.Len() - FString(TEXT(".INI")).Len();
                FString PIENumberAsString = Token.Mid(Start, End - Start);
                return FString::Printf(TEXT("Context_%d"), FCString::Atoi(*PIENumberAsString) + 1);
            }
#endif
        }
    }
#endif
    FWorldContext *WorldContext = GEngine->GetWorldContextFromHandle(State->WorldContextHandle);
    if (WorldContext != nullptr)
    {
        bool bIsHostingDedicatedServer = false;
        for (const auto &WC : GEngine->GetWorldContexts())
        {
            if (WC.RunAsDedicated)
            {
                bIsHostingDedicatedServer = true;
                break;
            }
        }

        // If bIsHostingDedicatedServer is true, then the first client context is PIEInstance 2, so we need to adjust
        // down for that.
        return FString::Printf(TEXT("Context_%d"), WorldContext->PIEInstance + (bIsHostingDedicatedServer ? 0 : 1));
    }
    return TEXT("");
}

FString FTryDefaultDeveloperAuthenticationEASCredentialsNode::GetCredentialName(
    TSharedRef<FAuthenticationGraphState> State)
{
    return State->Config->GetDeveloperToolDefaultCredentialName();
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION