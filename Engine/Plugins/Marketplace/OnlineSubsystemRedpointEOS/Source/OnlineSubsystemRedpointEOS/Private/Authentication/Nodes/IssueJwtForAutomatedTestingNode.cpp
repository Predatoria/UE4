// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#if !defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/IssueJwtForAutomatedTestingNode.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"

EOS_ENABLE_STRICT_WARNINGS

void FIssueJwtForAutomatedTestingNode::OnHttpResponse(
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FHttpRequestPtr Request,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FHttpResponsePtr Response,
    bool bConnectedSuccessfully,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<FAuthenticationGraphState> State,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FAuthenticationGraphNodeOnDone OnDone)
{
    if (!bConnectedSuccessfully)
    {
        State->ErrorMessages.Add(
            TEXT("Unable to connect to licensing.redpoint.games to obtain JWT for automated testing."));
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    if (Response->GetResponseCode() != EHttpResponseCodes::Ok)
    {
        State->ErrorMessages.Add(
            TEXT("Non-200 response from licensing.redpoint.games when obtaining JWT for automated testing."));
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    FString JWT = Response->GetContentAsString();
    JWT = JWT.TrimStartAndEnd();
    State->Metadata.Add(TEXT("AUTOMATED_TESTING_JWT"), JWT);
    State->ResultUserAuthAttributes.Add(TEXT("automatedTesting.jwt"), JWT);
    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
    return;
}

void FIssueJwtForAutomatedTestingNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
    if (State->AutomatedTestingEmailAddress.StartsWith("JWT:"))
    {
        State->Metadata.Add(
            TEXT("AUTOMATED_TESTING_JWT"),
            State->AutomatedTestingEmailAddress.Mid(FString(TEXT("JWT:")).Len()));
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        return;
    }
    if (State->ProvidedCredentials.Type == TEXT("AUTOMATED_TESTING_OSS") &&
        State->ProvidedCredentials.Id.StartsWith("JWT:"))
    {
        State->Metadata.Add(
            TEXT("AUTOMATED_TESTING_JWT"),
            State->ProvidedCredentials.Id.Mid(FString(TEXT("JWT:")).Len()));
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        return;
    }

    auto HttpRequest = FHttpModule::Get().CreateRequest();

    auto TestName = State->AutomatedTestingEmailAddress.Mid(FString(TEXT("CreateOnDemand:")).Len());

    UE_LOG(
        LogEOS,
        Verbose,
        TEXT("Requesting automation JWT with parameters: HostName=%s_%s_%s, TestName=%s, SubsystemId=%d"),
        TEXT(PREPROCESSOR_TO_STRING(PLATFORM_HEADER_NAME)),
        *FPlatformHttp::UrlEncode(FPlatformMisc::GetLoginId()),
        *FPlatformHttp::UrlEncode(FApp::GetInstanceId().ToString()),
        *FPlatformHttp::UrlEncode(TestName),
        FCString::Atoi(*(State->AutomatedTestingPassword)));

    HttpRequest->SetVerb("POST");
    HttpRequest->SetHeader("Content-Type", "application/x-www-form-urlencoded");
    HttpRequest->SetURL("https://licensing.redpoint.games/api/eos-automated-testing/issue");
    HttpRequest->SetContentAsString(FString::Printf(
        TEXT("hostName=%s_%s_%s&testName=%s&subsystemId=%d"),
        TEXT(PREPROCESSOR_TO_STRING(PLATFORM_HEADER_NAME)),
        *FPlatformHttp::UrlEncode(FPlatformMisc::GetLoginId()),
        *FPlatformHttp::UrlEncode(FApp::GetInstanceId().ToString()),
        *FPlatformHttp::UrlEncode(TestName),
        FCString::Atoi(*(State->AutomatedTestingPassword))));
    HttpRequest->OnProcessRequestComplete()
        .BindSP(this, &FIssueJwtForAutomatedTestingNode::OnHttpResponse, State, OnDone);
    if (!HttpRequest->ProcessRequest())
    {
        State->ErrorMessages.Add(TEXT("Unable to start HTTP request to obtain JWT for automated testing."));
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if !UE_BUILD_SHIPPING

#endif // #if EOS_HAS_AUTHENTICATION