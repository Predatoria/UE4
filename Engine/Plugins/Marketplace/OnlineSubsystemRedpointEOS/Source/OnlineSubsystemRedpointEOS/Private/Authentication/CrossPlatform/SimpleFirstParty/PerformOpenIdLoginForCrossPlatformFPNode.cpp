// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/SimpleFirstParty/PerformOpenIdLoginForCrossPlatformFPNode.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/SimpleFirstPartyCrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"

EOS_ENABLE_STRICT_WARNINGS

void FPerformOpenIdLoginForCrossPlatformFPNode::HandleEOSAuthenticationCallback(
    const EOS_Connect_LoginCallbackInfo *Data,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<FAuthenticationGraphState> State,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FAuthenticationGraphNodeOnDone OnDone)
{
    if (Data->ResultCode == EOS_EResult::EOS_Success || Data->ResultCode == EOS_EResult::EOS_InvalidUser)
    {
        FAuthenticationGraphEOSCandidate Candidate = State->AddEOSConnectCandidate(
            FText::FromString(TEXT("SimpleFirstParty")),
            TMap<FString, FString>(),
            Data,
            FAuthenticationGraphRefreshEOSCredentials(),
            NAME_None,
            EAuthenticationGraphEOSCandidateType::CrossPlatform,
            MakeShared<FSimpleFirstPartyCrossPlatformAccountId>(
                FCString::Atoi64(*(State->Metadata["FIRST_PARTY_USER_ID"].GetValue<FString>()))));
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
    }
    else if (Data->ResultCode == EOS_EResult::EOS_UnexpectedError)
    {
        // Epic's authentication API has intermittent issues with OpenID and returns EOS_UnexpectedError in these
        // cases. Just try again.
        this->Execute(State, OnDone);
    }
    else
    {
        State->ErrorMessages.Add(FString::Printf(
            TEXT("PerformOpenIdLoginForAutomatedTestingNode: OpenID "
                 "failed to authenticate with EOS Connect: %s"),
            ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode))));
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
    }
}

void FPerformOpenIdLoginForCrossPlatformFPNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
    if (!State->Metadata.Contains(TEXT("FIRST_PARTY_ACCESS_TOKEN")) ||
        !State->Metadata.Contains(TEXT("FIRST_PARTY_USER_ID")))
    {
        State->ErrorMessages.Add(TEXT("Missing first party credentials to complete login"));
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    FEOSAuthentication::DoRequest(
        State->EOSConnect,
        TEXT(""),
        State->Metadata["FIRST_PARTY_ACCESS_TOKEN"],
        EOS_EExternalCredentialType::EOS_ECT_OPENID_ACCESS_TOKEN,
        FEOSAuth_DoRequestComplete::CreateSP(
            this,
            &FPerformOpenIdLoginForCrossPlatformFPNode::HandleEOSAuthenticationCallback,
            State,
            OnDone));
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
