// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#if !defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/AutomatedTesting/PerformOpenIdLoginForCrossPlatformATNode.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/AutomatedTestingCrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"

EOS_ENABLE_STRICT_WARNINGS

void FPerformOpenIdLoginForCrossPlatformAutomatedTestingNode::HandleEOSAuthenticationCallback(
    const EOS_Connect_LoginCallbackInfo *Data,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<FAuthenticationGraphState> State,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FAuthenticationGraphNodeOnDone OnDone)
{
    if (Data->ResultCode == EOS_EResult::EOS_Success || Data->ResultCode == EOS_EResult::EOS_InvalidUser)
    {
        FAuthenticationGraphEOSCandidate Candidate = State->AddEOSConnectCandidate(
            FText::FromString(TEXT("AutomatedTestingCrossPlatform")),
            TMap<FString, FString>(),
            Data,
            FAuthenticationGraphRefreshEOSCredentials(),
            NAME_None,
            EAuthenticationGraphEOSCandidateType::CrossPlatform,
            MakeShared<FAutomatedTestingCrossPlatformAccountId>(TEXT("ThisValueDoesNotMatterForTestingPurposes")));
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
            TEXT("PerformOpenIdLoginForCrossPlatformAutomatedTestingNode: OpenID "
                 "failed to authenticate with EOS Connect: %s"),
            ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode))));
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
    }
}

void FPerformOpenIdLoginForCrossPlatformAutomatedTestingNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
    checkf(State->Metadata.Contains(TEXT("AUTOMATED_TESTING_JWT")), TEXT("AUTOMATED_TESTING_JWT metadata not present"));

    FEOSAuthentication::DoRequest(
        State->EOSConnect,
        TEXT(""),
        State->Metadata["AUTOMATED_TESTING_JWT"],
        EOS_EExternalCredentialType::EOS_ECT_OPENID_ACCESS_TOKEN,
        FEOSAuth_DoRequestComplete::CreateSP(
            this,
            &FPerformOpenIdLoginForCrossPlatformAutomatedTestingNode::HandleEOSAuthenticationCallback,
            State,
            OnDone));
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if !UE_BUILD_SHIPPING

#endif // #if EOS_HAS_AUTHENTICATION
