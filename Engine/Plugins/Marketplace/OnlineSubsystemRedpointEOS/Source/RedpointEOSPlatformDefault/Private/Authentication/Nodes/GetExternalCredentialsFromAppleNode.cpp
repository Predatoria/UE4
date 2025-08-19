// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION && EOS_APPLE_ENABLED

#include "GetExternalCredentialsFromAppleNode.h"

#if EOS_APPLE_HAS_RUNTIME_SUPPORT

#include "LogEOSPlatformDefault.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationHelpers.h"
#include "OnlineSubsystemUtils.h"

#import <AuthenticationServices/AuthenticationServices.h>

EOS_ENABLE_STRICT_WARNINGS

class FAppleCredentialObtainer : public FAuthenticationCredentialObtainer<FAppleCredentialObtainer, FString>
{
public:
    FAppleCredentialObtainer(const FAppleCredentialObtainer::FOnCredentialObtained &Cb)
        : FAuthenticationCredentialObtainer(Cb){};

    virtual bool Init(UWorld *World, int32 LocalUserNum) override;
    virtual bool Tick(float DeltaSeconds) override;
};

bool FAppleCredentialObtainer::Init(UWorld *World, int32 LocalUserNum)
{
    if (World == nullptr)
    {
        this->EmitError(TEXT("Could not authenticate with online subsystem, because the UWorld* pointer was null."));
        this->Done(false, TEXT(""));
        return false;
    }

    IOnlineSubsystem *OSSSubsystem = Online::GetSubsystem(World, APPLE_SUBSYSTEM);
    if (OSSSubsystem == nullptr)
    {
        this->EmitError(
            TEXT("Could not authenticate with online subsystem, because the subsystem was not available. Check that it "
                 "is enabled in your DefaultEngine.ini file."));
        this->Done(false, TEXT(""));
        return false;
    }

    TSharedPtr<IOnlineIdentity, ESPMode::ThreadSafe> OSSIdentity =
        StaticCastSharedPtr<IOnlineIdentity>(OSSSubsystem->GetIdentityInterface());
    if (!OSSIdentity.IsValid())
    {
        this->EmitError(
            TEXT("Could not authenticate with online subsystem, because the identity interface was not available."));
        this->Done(false, TEXT(""));
        return false;
    }

    TSharedPtr<const FUniqueNetId> UserId = OSSIdentity->GetUniquePlayerId(LocalUserNum);
    if (!UserId.IsValid())
    {
        this->EmitError(TEXT("Could not authenticate because the local user isn't signed in"));
        this->Done(false, TEXT(""));
        return false;
    }

    FString Token = OSSIdentity->GetAuthToken(LocalUserNum);

    UE_LOG(LogEOSPlatformDefault, Verbose, TEXT("Authenticating with Apple auth token: %s"), *Token);

    this->Done(true, Token);
    return true;
}

bool FAppleCredentialObtainer::Tick(float DeltaSeconds)
{
    return false;
}

class FAppleExternalCredentials : public IOnlineExternalCredentials
{
private:
    FString Token;
    TMap<FString, FString> AuthAttributes;

    void OnIdTokenObtained(bool bWasSuccessful, FString IdToken, FOnlineExternalCredentialsRefreshComplete OnComplete);

public:
    FAppleExternalCredentials(const FString &InToken, const TMap<FString, FString> &InAuthAttributes);
    virtual ~FAppleExternalCredentials(){};
    virtual FText GetProviderDisplayName() const override;
    virtual FString GetType() const override;
    virtual FString GetId() const override;
    virtual FString GetToken() const override;
    virtual TMap<FString, FString> GetAuthAttributes() const override;
    virtual FName GetNativeSubsystemName() const override;
    virtual void Refresh(
        TSoftObjectPtr<UWorld> InWorld,
        int32 LocalUserNum,
        FOnlineExternalCredentialsRefreshComplete OnComplete) override;
};

FAppleExternalCredentials::FAppleExternalCredentials(
    const FString &InToken,
    const TMap<FString, FString> &InAuthAttributes)
{
    this->Token = InToken;
    this->AuthAttributes = InAuthAttributes;
}

FText FAppleExternalCredentials::GetProviderDisplayName() const
{
    return NSLOCTEXT("OnlineSubsystemRedpointEOS", "Platform_Apple", "Apple");
}

FString FAppleExternalCredentials::GetType() const
{
    return TEXT("APPLE_ID_TOKEN");
}

FString FAppleExternalCredentials::GetId() const
{
    return TEXT("Anonymous");
}

FString FAppleExternalCredentials::GetToken() const
{
    return this->Token;
}

TMap<FString, FString> FAppleExternalCredentials::GetAuthAttributes() const
{
    return this->AuthAttributes;
}

FName FAppleExternalCredentials::GetNativeSubsystemName() const
{
    return FName(TEXT("APPLE"));
}

void FAppleExternalCredentials::OnIdTokenObtained(
    bool bWasSuccessful,
    FString IdToken,
    FOnlineExternalCredentialsRefreshComplete OnComplete)
{
    if (!bWasSuccessful)
    {
        OnComplete.ExecuteIfBound(false);
        return;
    }

    this->Token = IdToken;
    this->AuthAttributes.Add(TEXT("apple.idToken"), IdToken);
    OnComplete.ExecuteIfBound(true);
}

void FAppleExternalCredentials::Refresh(
    TSoftObjectPtr<UWorld> InWorld,
    int32 LocalUserNum,
    FOnlineExternalCredentialsRefreshComplete OnComplete)
{
    FAppleCredentialObtainer::StartFromCredentialRefresh(
        InWorld,
        LocalUserNum,
        FAppleCredentialObtainer::FOnCredentialObtained::CreateSP(
            this,
            &FAppleExternalCredentials::OnIdTokenObtained,
            OnComplete));
}

void FGetExternalCredentialsFromAppleNode::OnIdTokenObtained(
    bool bWasSuccessful,
    FString IdToken,
    TSharedRef<FAuthenticationGraphState> InState,
    FAuthenticationGraphNodeOnDone InOnDone)
{
    if (!bWasSuccessful)
    {
        InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    TMap<FString, FString> UserAuthAttributes;
    UserAuthAttributes.Add(EOS_AUTH_ATTRIBUTE_AUTHENTICATEDWITH, TEXT("apple"));
    UserAuthAttributes.Add(TEXT("apple.idToken"), IdToken);

    InState->AvailableExternalCredentials.Add(MakeShared<FAppleExternalCredentials>(IdToken, UserAuthAttributes));
    InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
}

void FGetExternalCredentialsFromAppleNode::Execute(
    TSharedRef<FAuthenticationGraphState> InState,
    FAuthenticationGraphNodeOnDone InOnDone)
{
    FAppleCredentialObtainer::StartFromAuthenticationGraph(
        InState,
        FAppleCredentialObtainer::FOnCredentialObtained::CreateSP(
            this,
            &FGetExternalCredentialsFromAppleNode::OnIdTokenObtained,
            InState,
            InOnDone));
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_APPLE_HAS_RUNTIME_SUPPORT

#endif // #if EOS_HAS_AUTHENTICATION && EOS_APPLE_ENABLED