// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION && EOS_GOOGLE_ENABLED

#include "GetExternalCredentialsFromGoogleNode.h"

#include "LogEOSPlatformDefault.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationHelpers.h"
#include "OnlineSubsystemUtils.h"

// Path hack to get private header, relying on the fact that OnlineSubsystemGoogle sits next to OnlineSubsystem.
#include "../OnlineSubsystemGoogle/Source/Private/OnlineAccountGoogleCommon.h"

// Hack because Google doesn't surface the ID token in any way, even though it collects it on Android.
// We can't call any of the methods of FUserOnlineAccountGoogleCommon because it's not exported, but that
// doesn't really matter because as long as we know where the FAuthTokenGoogle sits in memory, we can get it.
class FGiveUsTheAuthTokenPlease : public FUserOnlineAccountGoogleCommon
{
public:
    static FString GetFirstNameGoogle(const FUserOnlineAccountGoogleCommon &Acc)
    {
        return ((FGiveUsTheAuthTokenPlease *)(&Acc))->FirstName;
    }
    static FAuthTokenGoogle GetAuthTokenGoogle(const FUserOnlineAccountGoogleCommon &Acc)
    {
        return ((FGiveUsTheAuthTokenPlease *)(&Acc))->AuthToken;
    }
};

EOS_ENABLE_STRICT_WARNINGS

class FGoogleCredentialObtainer : public FAuthenticationCredentialObtainer<FGoogleCredentialObtainer, FNameAndToken>
{
public:
    FGoogleCredentialObtainer(const FGoogleCredentialObtainer::FOnCredentialObtained &Cb)
        : FAuthenticationCredentialObtainer(Cb){};

    virtual bool Init(UWorld *World, int32 LocalUserNum) override;
    virtual bool Tick(float DeltaSeconds) override;
};

bool FGoogleCredentialObtainer::Init(UWorld *World, int32 LocalUserNum)
{
    if (World == nullptr)
    {
        this->EmitError(TEXT("Could not authenticate with online subsystem, because the UWorld* pointer was null."));
        this->Done(false, FNameAndToken());
        return false;
    }

    IOnlineSubsystem *OSSSubsystem = Online::GetSubsystem(World, GOOGLE_SUBSYSTEM);
    if (OSSSubsystem == nullptr)
    {
        this->EmitError(
            TEXT("Could not authenticate with online subsystem, because the subsystem was not available. Check that it "
                 "is enabled in your DefaultEngine.ini file."));
        this->Done(false, FNameAndToken());
        return false;
    }

    TSharedPtr<IOnlineIdentity, ESPMode::ThreadSafe> OSSIdentity =
        StaticCastSharedPtr<IOnlineIdentity>(OSSSubsystem->GetIdentityInterface());
    if (!OSSIdentity.IsValid())
    {
        this->EmitError(
            TEXT("Could not authenticate with online subsystem, because the identity interface was not available."));
        this->Done(false, FNameAndToken());
        return false;
    }

    TSharedPtr<const FUniqueNetId> UserId = OSSIdentity->GetUniquePlayerId(LocalUserNum);
    if (!UserId.IsValid())
    {
        this->EmitError(TEXT("Could not authenticate because the local user isn't signed in"));
        this->Done(false, FNameAndToken());
        return false;
    }

    TSharedPtr<FUserOnlineAccountGoogleCommon> UserAccount =
        StaticCastSharedPtr<FUserOnlineAccountGoogleCommon>(OSSIdentity->GetUserAccount(*UserId));
    if (!UserAccount.IsValid())
    {
        this->EmitError(TEXT("Could not retrieve Google account"));
        this->Done(false, FNameAndToken());
        return false;
    }

    FString GivenName = FGiveUsTheAuthTokenPlease::GetFirstNameGoogle(*UserAccount);
    FAuthTokenGoogle AuthToken = FGiveUsTheAuthTokenPlease::GetAuthTokenGoogle(*UserAccount);

    UE_LOG(
        LogEOSPlatformDefault,
        Verbose,
        TEXT("Authenticating with Google, first name: '%s', auth token: '%s'"),
        *GivenName,
        *AuthToken.IdToken);

    this->Done(true, FNameAndToken(GivenName, AuthToken.IdToken));
    return true;
}

bool FGoogleCredentialObtainer::Tick(float DeltaSeconds)
{
    return false;
}

class FGoogleExternalCredentials : public IOnlineExternalCredentials
{
private:
    FString GivenName;
    FString Token;
    TMap<FString, FString> AuthAttributes;

    void OnIdTokenObtained(
        bool bWasSuccessful,
        FNameAndToken GivenNameAndIdToken,
        FOnlineExternalCredentialsRefreshComplete OnComplete);

public:
    FGoogleExternalCredentials(
        const FString &InGivenName,
        const FString &InToken,
        const TMap<FString, FString> &InAuthAttributes);
    virtual ~FGoogleExternalCredentials(){};
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

FGoogleExternalCredentials::FGoogleExternalCredentials(
    const FString &InGivenName,
    const FString &InToken,
    const TMap<FString, FString> &InAuthAttributes)
{
    this->GivenName = InGivenName;
    this->Token = InToken;
    this->AuthAttributes = InAuthAttributes;
}

FText FGoogleExternalCredentials::GetProviderDisplayName() const
{
    return NSLOCTEXT("OnlineSubsystemRedpointEOS", "Platform_Google", "Google");
}

FString FGoogleExternalCredentials::GetType() const
{
    return TEXT("GOOGLE_ID_TOKEN");
}

FString FGoogleExternalCredentials::GetId() const
{
    if (this->GivenName == TEXT(""))
    {
        return TEXT("Anonymous");
    }

    return this->GivenName;
}

FString FGoogleExternalCredentials::GetToken() const
{
    return this->Token;
}

TMap<FString, FString> FGoogleExternalCredentials::GetAuthAttributes() const
{
    return this->AuthAttributes;
}

FName FGoogleExternalCredentials::GetNativeSubsystemName() const
{
    return GOOGLE_SUBSYSTEM;
}

void FGoogleExternalCredentials::OnIdTokenObtained(
    bool bWasSuccessful,
    FNameAndToken GivenNameAndIdToken,
    FOnlineExternalCredentialsRefreshComplete OnComplete)
{
    if (!bWasSuccessful)
    {
        OnComplete.ExecuteIfBound(false);
        return;
    }

    this->GivenName = GivenNameAndIdToken.GivenName;
    this->Token = GivenNameAndIdToken.IdToken;
    this->AuthAttributes.Add(TEXT("google.givenName"), GivenNameAndIdToken.GivenName);
    this->AuthAttributes.Add(TEXT("google.idToken"), GivenNameAndIdToken.IdToken);
    OnComplete.ExecuteIfBound(true);
}

void FGoogleExternalCredentials::Refresh(
    TSoftObjectPtr<UWorld> InWorld,
    int32 LocalUserNum,
    FOnlineExternalCredentialsRefreshComplete OnComplete)
{
    FGoogleCredentialObtainer::StartFromCredentialRefresh(
        InWorld,
        LocalUserNum,
        FGoogleCredentialObtainer::FOnCredentialObtained::CreateSP(
            this,
            &FGoogleExternalCredentials::OnIdTokenObtained,
            OnComplete));
}

void FGetExternalCredentialsFromGoogleNode::OnIdTokenObtained(
    bool bWasSuccessful,
    FNameAndToken GivenNameAndIdToken,
    TSharedRef<FAuthenticationGraphState> InState,
    FAuthenticationGraphNodeOnDone InOnDone)
{
    if (!bWasSuccessful)
    {
        InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    TMap<FString, FString> UserAuthAttributes;
    UserAuthAttributes.Add(EOS_AUTH_ATTRIBUTE_AUTHENTICATEDWITH, TEXT("google"));
    UserAuthAttributes.Add(TEXT("google.givenName"), GivenNameAndIdToken.GivenName);
    UserAuthAttributes.Add(TEXT("google.idToken"), GivenNameAndIdToken.IdToken);

    InState->AvailableExternalCredentials.Add(MakeShared<FGoogleExternalCredentials>(
        GivenNameAndIdToken.GivenName,
        GivenNameAndIdToken.IdToken,
        UserAuthAttributes));
    InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
}

void FGetExternalCredentialsFromGoogleNode::Execute(
    TSharedRef<FAuthenticationGraphState> InState,
    FAuthenticationGraphNodeOnDone InOnDone)
{
    FGoogleCredentialObtainer::StartFromAuthenticationGraph(
        InState,
        FGoogleCredentialObtainer::FOnCredentialObtained::CreateSP(
            this,
            &FGetExternalCredentialsFromGoogleNode::OnIdTokenObtained,
            InState,
            InOnDone));
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION && EOS_GOOGLE_ENABLED