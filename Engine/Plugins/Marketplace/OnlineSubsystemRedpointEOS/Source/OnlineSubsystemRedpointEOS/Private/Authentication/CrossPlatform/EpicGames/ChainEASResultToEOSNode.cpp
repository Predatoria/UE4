// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/ChainEASResultToEOSNode.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/TryPersistentEASCredentialsNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGamesCrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"

EOS_ENABLE_STRICT_WARNINGS

class FEOSAuthTokenHolder : public TSharedFromThis<FEOSAuthTokenHolder>
{
private:
    TWeakPtr<FOnlineSubsystemEOS, ESPMode::ThreadSafe> Subsystem;
    EOS_Auth_Token *AuthToken;

public:
    UE_NONCOPYABLE(FEOSAuthTokenHolder);
    FEOSAuthTokenHolder(TWeakPtr<FOnlineSubsystemEOS, ESPMode::ThreadSafe> InSubsystem, EOS_Auth_Token *InAuthToken)
        : Subsystem(MoveTemp(InSubsystem))
        , AuthToken(InAuthToken){};
    ~FEOSAuthTokenHolder()
    {
        if (this->Subsystem.IsValid())
        {
            if (this->Subsystem.Pin()->IsPlatformInstanceValid())
            {
                EOS_Auth_Token_Release(this->AuthToken);
            }
        }
    }
    EOS_Auth_Token &Get() const
    {
        return *this->AuthToken;
    }
};

class FEpicExternalCredentials : public IOnlineExternalCredentials
{
private:
    TWeakPtr<class FOnlineSubsystemEOS, ESPMode::ThreadSafe> Subsystem;
    EOS_HAuth EOSAuth;
    TSharedRef<FEOSAuthTokenHolder> Token;
    FName NativeSubsystem;
    TMap<FString, FString> Attributes;
    TMap<FString, FString> ComputeAttributesFromToken(const TSharedRef<FEOSAuthTokenHolder> &InToken);

public:
    UE_NONCOPYABLE(FEpicExternalCredentials);
    FEpicExternalCredentials(
        TWeakPtr<class FOnlineSubsystemEOS, ESPMode::ThreadSafe> InSubsystem,
        EOS_HAuth InEOSAuth,
        FName InNativeSubsystem,
        const TSharedRef<FEOSAuthTokenHolder> &InToken);
    virtual ~FEpicExternalCredentials(){};
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
    EOS_EpicAccountId GetEpicAccountId() const;
};

FEpicExternalCredentials::FEpicExternalCredentials(
    TWeakPtr<class FOnlineSubsystemEOS, ESPMode::ThreadSafe> InSubsystem,
    EOS_HAuth InEOSAuth,
    FName InNativeSubsystem,
    const TSharedRef<FEOSAuthTokenHolder> &InToken)
    : Subsystem(MoveTemp(InSubsystem))
    , EOSAuth(InEOSAuth)
    , Token(InToken)
    , NativeSubsystem(InNativeSubsystem)
{
    this->Attributes = this->ComputeAttributesFromToken(InToken);
}

FText FEpicExternalCredentials::GetProviderDisplayName() const
{
    return NSLOCTEXT("OnlineRedpointRedpointEOS", "EpicGames", "Epic Games");
}

FString FEpicExternalCredentials::GetType() const
{
    return TEXT("EPIC");
}

FString FEpicExternalCredentials::GetId() const
{
    return TEXT("");
}

FString FEpicExternalCredentials::GetToken() const
{
    return ANSI_TO_TCHAR(this->Token->Get().AccessToken);
}

TMap<FString, FString> FEpicExternalCredentials::ComputeAttributesFromToken(
    const TSharedRef<FEOSAuthTokenHolder> &InToken)
{
    TMap<FString, FString> Attrs;
    Attrs.Add(EOS_AUTH_ATTRIBUTE_AUTHENTICATEDWITH, TEXT("epic"));
    Attrs.Add(TEXT("epic.accessToken"), UTF8_TO_TCHAR(InToken->Get().AccessToken));
    FString AccountIdStr;
    verify(EOSString_EpicAccountId::ToString(InToken->Get().AccountId, AccountIdStr) == EOS_EResult::EOS_Success);
    Attrs.Add(TEXT("epic.accountId"), AccountIdStr);
    Attrs.Add(TEXT("epic.app"), UTF8_TO_TCHAR(InToken->Get().App));
    switch (InToken->Get().AuthType)
    {
    case EOS_EAuthTokenType::EOS_ATT_Client:
        Attrs.Add(TEXT("epic.authTokenType"), TEXT("client"));
        break;
    case EOS_EAuthTokenType::EOS_ATT_User:
        Attrs.Add(TEXT("epic.authTokenType"), TEXT("user"));
        break;
    default:
        Attrs.Add(TEXT("epic.authTokenType"), TEXT("unknown"));
        break;
    }
    Attrs.Add(TEXT("epic.clientId"), UTF8_TO_TCHAR(InToken->Get().ClientId));
    Attrs.Add(TEXT("epic.expiresAt"), UTF8_TO_TCHAR(InToken->Get().ExpiresAt));
    Attrs.Add(TEXT("epic.refreshExpiresAt"), UTF8_TO_TCHAR(InToken->Get().RefreshExpiresAt));
    Attrs.Add(TEXT("epic.refreshToken"), UTF8_TO_TCHAR(InToken->Get().RefreshToken));
    return Attrs;
}

TMap<FString, FString> FEpicExternalCredentials::GetAuthAttributes() const
{
    return this->Attributes;
}

FName FEpicExternalCredentials::GetNativeSubsystemName() const
{
    return this->NativeSubsystem;
}

EOS_EpicAccountId FEpicExternalCredentials::GetEpicAccountId() const
{
    return this->Token->Get().AccountId;
}

void FEpicExternalCredentials::Refresh(
    TSoftObjectPtr<UWorld> InWorld,
    int32 LocalUserNum,
    FOnlineExternalCredentialsRefreshComplete OnComplete)
{
    auto SubsystemPinned = this->Subsystem.Pin();
    if (!SubsystemPinned.IsValid())
    {
        OnComplete.ExecuteIfBound(false);
        return;
    }

    UE_LOG(LogEOS, Verbose, TEXT("Copying updated refresh token from EOS Auth..."));

    EOS_Auth_Token *NewToken = nullptr;

    EOS_Auth_CopyUserAuthTokenOptions CopyOpts = {};
    CopyOpts.ApiVersion = EOS_AUTH_COPYUSERAUTHTOKEN_API_LATEST;
    auto CopyResult = EOS_Auth_CopyUserAuthToken(this->EOSAuth, &CopyOpts, this->Token->Get().AccountId, &NewToken);
    if (CopyResult != EOS_EResult::EOS_Success)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Failed to copy auth token during refresh, got result: %s"),
            ANSI_TO_TCHAR(EOS_EResult_ToString(CopyResult)));
        OnComplete.ExecuteIfBound(false);
        return;
    }

    this->Token = MakeShared<FEOSAuthTokenHolder>(SubsystemPinned.ToSharedRef(), NewToken);
    this->Attributes = this->ComputeAttributesFromToken(this->Token);

    UE_LOG(LogEOS, Verbose, TEXT("Successfully obtained updated EAS token for user"));

    OnComplete.ExecuteIfBound(true);
}

void FChainEASResultToEOSNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
    EOS_EpicAccountId AuthenticatedEpicAccountId = State->GetAuthenticatedEpicAccountId();

    check(EOSString_EpicAccountId::IsValid(AuthenticatedEpicAccountId));

    EOS_Auth_Token *AuthToken = nullptr;

    EOS_Auth_CopyUserAuthTokenOptions CopyOpts = {};
    CopyOpts.ApiVersion = EOS_AUTH_COPYUSERAUTHTOKEN_API_LATEST;

    {
        FString EpicAccountIdStr;
        if (EOSString_EpicAccountId::ToString(AuthenticatedEpicAccountId, EpicAccountIdStr) == EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Verbose,
                TEXT("ChainEASResultToEOSNode: Retrieving auth token for account: %s"),
                *EpicAccountIdStr);
        }
    }

    auto CopyResult = EOS_Auth_CopyUserAuthToken(State->EOSAuth, &CopyOpts, AuthenticatedEpicAccountId, &AuthToken);
    if (CopyResult != EOS_EResult::EOS_Success)
    {
        State->ErrorMessages.Add(FString::Printf(
            TEXT("ChainEASResultToEOSNode: Failed to copy user auth token from Epic account ID: %s"),
            ANSI_TO_TCHAR(EOS_EResult_ToString(CopyResult))));
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        return;
    }

    // Store the refresh token in metadata so it can be stored by future authentication nodes.
    State->Metadata.Add(EOS_METADATA_EAS_REFRESH_TOKEN, FString(ANSI_TO_TCHAR(AuthToken->RefreshToken)));

    // Get the native subsystem. If we authenticated into EAS with e.g. a Steam credential, then the
    // native subsystem should be Steam instead of Epic Games.
    FName NativeSubsystem = REDPOINT_EAS_SUBSYSTEM;
    if (State->Metadata.Contains(EOS_METADATA_EAS_NATIVE_SUBSYSTEM))
    {
        NativeSubsystem = State->Metadata[EOS_METADATA_EAS_NATIVE_SUBSYSTEM].GetValue<FName>();
    }

    // Create the Epic "external" credentials which we'll use to create the candidate. This means refreshing gets
    // handled for us, and our external credentials can be used for client-server authentication.
    TSharedRef<FEpicExternalCredentials> EpicCredentials = MakeShared<FEpicExternalCredentials>(
        State->Subsystem,
        State->EOSAuth,
        NativeSubsystem,
        MakeShared<FEOSAuthTokenHolder>(State->Subsystem, AuthToken));

    // Authenticate.
    FEOSAuthentication::DoRequest(
        State->EOSConnect,
        EpicCredentials->GetId(),
        EpicCredentials->GetToken(),
        StrToExternalCredentialType(EpicCredentials->GetType()),
        FEOSAuth_DoRequestComplete::CreateLambda(
            [State, OnDone, EpicCredentials](const EOS_Connect_LoginCallbackInfo *Data) {
                if (Data->ResultCode == EOS_EResult::EOS_Success || Data->ResultCode == EOS_EResult::EOS_InvalidUser)
                {
                    State->AddEOSConnectCandidateFromExternalCredentials(
                        Data,
                        EpicCredentials,
                        EAuthenticationGraphEOSCandidateType::CrossPlatform,
                        MakeShared<FEpicGamesCrossPlatformAccountId>(EpicCredentials->GetEpicAccountId()));
                }
                else
                {
                    State->ErrorMessages.Add(FString::Printf(
                        TEXT("Epic Account Services failed to authenticate with EOS Connect: %s"),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode))));
                }

                OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
            }));
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
