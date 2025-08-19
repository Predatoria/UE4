// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#if !defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/PerformOpenIdLoginForAutomatedTestingNode.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"

EOS_ENABLE_STRICT_WARNINGS

class FAutomatedTestingExternalCredentials : public IOnlineExternalCredentials
{
private:
    FString Token;

public:
    UE_NONCOPYABLE(FAutomatedTestingExternalCredentials);
    FAutomatedTestingExternalCredentials(FString InToken)
        : Token(MoveTemp(InToken)){};
    virtual ~FAutomatedTestingExternalCredentials(){};
    virtual FText GetProviderDisplayName() const override
    {
        return FText::FromString(TEXT("AutomatedTesting"));
    }
    virtual FString GetType() const override
    {
        return TEXT("OPENID_ACCESS_TOKEN");
    }
    virtual FString GetId() const override
    {
        return TEXT("");
    }
    virtual FString GetToken() const override
    {
        return this->Token;
    }
    virtual TMap<FString, FString> GetAuthAttributes() const override
    {
        return TMap<FString, FString>();
    }
    virtual FName GetNativeSubsystemName() const override
    {
        return NAME_None;
    }
    virtual void Refresh(
        TSoftObjectPtr<UWorld> InWorld,
        int32 LocalUserNum,
        FOnlineExternalCredentialsRefreshComplete OnComplete) override
    {
        OnComplete.ExecuteIfBound(false);
    }
};

void FPerformOpenIdLoginForAutomatedTestingNode::HandleEOSAuthenticationCallback(
    const EOS_Connect_LoginCallbackInfo *Data,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<FAuthenticationGraphState> State,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FAuthenticationGraphNodeOnDone OnDone)
{
    if (Data->ResultCode == EOS_EResult::EOS_Success || Data->ResultCode == EOS_EResult::EOS_InvalidUser)
    {
        FString JwtToken = State->Metadata["AUTOMATED_TESTING_JWT"];
        TSharedRef<FAutomatedTestingExternalCredentials> Credentials =
            MakeShared<FAutomatedTestingExternalCredentials>(JwtToken);

        FAuthenticationGraphEOSCandidate Candidate =
            State->AddEOSConnectCandidateFromExternalCredentials(Data, Credentials);
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

void FPerformOpenIdLoginForAutomatedTestingNode::Execute(
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
            &FPerformOpenIdLoginForAutomatedTestingNode::HandleEOSAuthenticationCallback,
            State,
            OnDone));
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if !UE_BUILD_SHIPPING

#endif // #if EOS_HAS_AUTHENTICATION
