// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/GetExternalCredentialsFromOnlineSubsystemNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationHelpers.h"

#include "OnlineSubsystemUtils.h"

EOS_ENABLE_STRICT_WARNINGS

class FOnlineSubsystemExternalCredentials : public IOnlineExternalCredentials
{
private:
    FName SubsystemName;
    EOS_EExternalCredentialType CredentialType;
    FOnlineAccountCredentials CurrentCredentials;
    FString AuthenticatedWithValue;
    FString TokenAuthAttributeName;

public:
    UE_NONCOPYABLE(FOnlineSubsystemExternalCredentials);
    FOnlineSubsystemExternalCredentials(
        FName InSubsystemName,
        EOS_EExternalCredentialType InCredentialType,
        const FOnlineAccountCredentials &InitialCredentials,
        const FString &InAuthenticatedWithValue,
        const FString &InTokenAuthAttributeName);
    virtual ~FOnlineSubsystemExternalCredentials(){};
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

FOnlineSubsystemExternalCredentials::FOnlineSubsystemExternalCredentials(
    FName InSubsystemName,
    EOS_EExternalCredentialType InCredentialType,
    const FOnlineAccountCredentials &InitialCredentials,
    const FString &InAuthenticatedWithValue,
    const FString &InTokenAuthAttributeName)
    : SubsystemName(InSubsystemName)
    , CredentialType(InCredentialType)
    , CurrentCredentials(InitialCredentials)
    , AuthenticatedWithValue(InAuthenticatedWithValue)
    , TokenAuthAttributeName(InTokenAuthAttributeName)
{
}

FText FOnlineSubsystemExternalCredentials::GetProviderDisplayName() const
{
    return FText::FromString(TEXT("TODO"));
}

FString FOnlineSubsystemExternalCredentials::GetType() const
{
    return this->CurrentCredentials.Type;
}

FString FOnlineSubsystemExternalCredentials::GetId() const
{
    return this->CurrentCredentials.Id;
}

FString FOnlineSubsystemExternalCredentials::GetToken() const
{
    return this->CurrentCredentials.Token;
}

TMap<FString, FString> FOnlineSubsystemExternalCredentials::GetAuthAttributes() const
{
    TMap<FString, FString> UserAuthAttributes;
    UserAuthAttributes.Add(EOS_AUTH_ATTRIBUTE_AUTHENTICATEDWITH, this->AuthenticatedWithValue);
    UserAuthAttributes.Add(this->TokenAuthAttributeName, this->CurrentCredentials.Token);
    return UserAuthAttributes;
}

FName FOnlineSubsystemExternalCredentials::GetNativeSubsystemName() const
{
    return this->SubsystemName;
}

void FOnlineSubsystemExternalCredentials::Refresh(
    TSoftObjectPtr<UWorld> InWorld,
    int32 LocalUserNum,
    FOnlineExternalCredentialsRefreshComplete OnComplete)
{
    if (!InWorld.IsValid())
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Could not refresh credentials with online subsystem, because the UWorld* pointer was null."));
        OnComplete.ExecuteIfBound(false);
        return;
    }

    UWorld *World = InWorld.Get();

    IOnlineSubsystem *OSSSubsystem = Online::GetSubsystem(World, this->SubsystemName);
    if (OSSSubsystem == nullptr)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Could not refresh credentials with online subsystem, because the subsystem was not available."));
        OnComplete.ExecuteIfBound(false);
        return;
    }

    TSharedPtr<IOnlineIdentity, ESPMode::ThreadSafe> OSSIdentity =
        StaticCastSharedPtr<IOnlineIdentity>(OSSSubsystem->GetIdentityInterface());
    if (!OSSIdentity.IsValid())
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Could not refresh credentials with online subsystem, because the identity interface was not "
                 "available."));
        OnComplete.ExecuteIfBound(false);
        return;
    }

    this->CurrentCredentials.Type = ExternalCredentialTypeToStr(this->CredentialType);
    this->CurrentCredentials.Token = OSSIdentity->GetAuthToken(LocalUserNum);
    this->CurrentCredentials.Id = OSSIdentity->GetPlayerNickname(LocalUserNum);
    if (this->CurrentCredentials.Id.IsEmpty())
    {
        this->CurrentCredentials.Id = TEXT("Anonymous");
    }

    UE_LOG(LogEOS, Verbose, TEXT("Authenticating with OSS auth token: %s"), *this->CurrentCredentials.Token);

    OnComplete.ExecuteIfBound(true);
}

void FGetExternalCredentialsFromOnlineSubsystemNode::Execute(
    TSharedRef<FAuthenticationGraphState> InState,
    FAuthenticationGraphNodeOnDone InOnDone)
{
#if !defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING
    if (InState->Config->IsAutomatedTesting())
    {
        // We are running through authentication unit tests, which are designed to test the logic flow of the OSS
        // authentication graph without actually requiring an external service.
        InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        return;
    }
#endif

    UWorld *World = InState->GetWorld();
    if (World == nullptr)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Could not authenticate with online subsystem, because the UWorld* pointer was null."));
        InState->ErrorMessages.Add(
            TEXT("Could not authenticate with online subsystem, because the UWorld* pointer was null."));
        InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    IOnlineSubsystem *OSSSubsystem = Online::GetSubsystem(World, this->SubsystemName);
    if (OSSSubsystem == nullptr)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Could not authenticate with online subsystem, because the subsystem was not available. Check that it "
                 "is "
                 "enabled in your DefaultEngine.ini file."));
        InState->ErrorMessages.Add(TEXT(
            "Could not authenticate with online subsystem, because the subsystem was not available. Check that it is "
            "enabled in your DefaultEngine.ini file."));
        InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    TSharedPtr<IOnlineIdentity, ESPMode::ThreadSafe> OSSIdentity =
        StaticCastSharedPtr<IOnlineIdentity>(OSSSubsystem->GetIdentityInterface());
    if (!OSSIdentity.IsValid())
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Could not authenticate with online subsystem, because the identity interface was not available."));
        InState->ErrorMessages.Add(
            TEXT("Could not authenticate with online subsystem, because the identity interface was not available."));
        InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    FOnlineAccountCredentials Credentials;
    Credentials.Type = ExternalCredentialTypeToStr(this->ObtainedCredentialType);
    Credentials.Token = OSSIdentity->GetAuthToken(InState->LocalUserNum);
    Credentials.Id = OSSIdentity->GetPlayerNickname(InState->LocalUserNum);
    if (Credentials.Id.IsEmpty())
    {
        Credentials.Id = TEXT("Anonymous");
    }

    UE_LOG(LogEOS, Verbose, TEXT("Authenticating with OSS auth token: %s"), *Credentials.Token);

    InState->AvailableExternalCredentials.Add(MakeShared<FOnlineSubsystemExternalCredentials>(
        this->SubsystemName,
        this->ObtainedCredentialType,
        Credentials,
        this->AuthenticatedWithValue,
        this->TokenAuthAttributeName));
    InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
