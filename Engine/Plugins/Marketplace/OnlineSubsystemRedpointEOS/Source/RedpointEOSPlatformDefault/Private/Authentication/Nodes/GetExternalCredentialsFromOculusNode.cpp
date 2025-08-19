// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION && EOS_OCULUS_ENABLED

#include "GetExternalCredentialsFromOculusNode.h"

#include "LogEOSPlatformDefault.h"
#include "OVR_Platform.h"
#include "OVR_User.h"
#include "OnlineSubsystemUtils.h"

EOS_ENABLE_STRICT_WARNINGS

class FOculusExternalCredentials : public IOnlineExternalCredentials
{
private:
    FString ProofToken;
    FString DisplayName;
    TMap<FString, FString> AuthAttributes;

    void OnUserProofReceived(
        ovrMessageHandle InHandle,
        bool bIsError,
        ovrID InUserId,
        FOnlineExternalCredentialsRefreshComplete OnComplete);

public:
    FOculusExternalCredentials(
        const FString &InProofToken,
        const FString &InDisplayName,
        const TMap<FString, FString> &InAuthAttributes);
    UE_NONCOPYABLE(FOculusExternalCredentials);
    virtual ~FOculusExternalCredentials(){};
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

FOculusExternalCredentials::FOculusExternalCredentials(
    const FString &InProofToken,
    const FString &InDisplayName,
    const TMap<FString, FString> &InAuthAttributes)
{
    this->ProofToken = InProofToken;
    this->DisplayName = InDisplayName;
    this->AuthAttributes = InAuthAttributes;
}

FText FOculusExternalCredentials::GetProviderDisplayName() const
{
    return NSLOCTEXT("OnlineSubsystemRedpointEOS", "Platform_Oculus", "Oculus");
}

FString FOculusExternalCredentials::GetType() const
{
    return TEXT("OCULUS_USERID_NONCE");
}

FString FOculusExternalCredentials::GetId() const
{
    if (this->DisplayName.IsEmpty())
    {
        return TEXT("Anonymous");
    }

    return this->DisplayName;
}

FString FOculusExternalCredentials::GetToken() const
{
    return this->ProofToken;
}

TMap<FString, FString> FOculusExternalCredentials::GetAuthAttributes() const
{
    return this->AuthAttributes;
}

FName FOculusExternalCredentials::GetNativeSubsystemName() const
{
    return OCULUS_SUBSYSTEM;
}

void FOculusExternalCredentials::OnUserProofReceived(
    ovrMessageHandle InHandle,
    bool bIsError,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    ovrID InUserId,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FOnlineExternalCredentialsRefreshComplete OnComplete)
{
    if (bIsError || ovr_Message_IsError(InHandle))
    {
        UE_LOG(
            LogEOSPlatformDefault,
            Error,
            TEXT("Can't refresh token for Oculus because the Oculus user proof operation failed."));
        OnComplete.ExecuteIfBound(false);
        return;
    }

    ovrUserProofHandle ProofHandle = ovr_Message_GetUserProof(InHandle);
    const char *ProofNonce = ovr_UserProof_GetNonce(ProofHandle);

    TMap<FString, FString> OculusAttrs;
    OculusAttrs.Add(TEXT("oculus.userId"), FString::Printf(TEXT("%llu"), InUserId));
    OculusAttrs.Add(TEXT("oculus.proofNonce"), ANSI_TO_TCHAR(ProofNonce));

    this->ProofToken = FString::Printf(TEXT("%llu|%s"), InUserId, ANSI_TO_TCHAR(ProofNonce));
    this->AuthAttributes = OculusAttrs;

    UE_LOG(LogEOSPlatformDefault, Verbose, TEXT("Obtained refresh Oculus user proof token: %s"), *this->ProofToken);
    OnComplete.ExecuteIfBound(true);
}

void FOculusExternalCredentials::Refresh(
    TSoftObjectPtr<UWorld> InWorld,
    int32 LocalUserNum,
    FOnlineExternalCredentialsRefreshComplete OnComplete)
{
    ovrID OculusId = ovr_GetLoggedInUserID();
    if (OculusId == 0)
    {
        UE_LOG(
            LogEOSPlatformDefault,
            Error,
            TEXT("Can't refresh token for Oculus because you are no longer signed into Oculus."));
        OnComplete.ExecuteIfBound(false);
        return;
    }

    FOnlineSubsystemOculus *OSSSubsystem =
        (FOnlineSubsystemOculus *)Online::GetSubsystem(InWorld.Get(), OCULUS_SUBSYSTEM);
    if (OSSSubsystem == nullptr)
    {
        UE_LOG(
            LogEOSPlatformDefault,
            Error,
            TEXT("Can't refresh token for Oculus because the Oculus subsystem is no longer available."));
        OnComplete.ExecuteIfBound(false);
        return;
    }

    ovrRequest UserProofRequest = ovr_User_GetUserProof();
    OSSSubsystem->AddRequestDelegate(
        UserProofRequest,
        FOculusMessageOnCompleteDelegate::CreateSP(
            this,
            &FOculusExternalCredentials::OnUserProofReceived,
            OculusId,
            OnComplete));
}

void FGetExternalCredentialsFromOculusNode::OnUserProofReceived(
    ovrMessageHandle InHandle,
    bool bIsError,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    ovrID InUserId,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FString InDisplayName,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<FAuthenticationGraphState> InState,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FAuthenticationGraphNodeOnDone InOnDone)
{
    if (bIsError || ovr_Message_IsError(InHandle))
    {
        UE_LOG(LogEOSPlatformDefault, Error, TEXT("Authentication with Oculus failed."));
        InState->ErrorMessages.Add(TEXT("Authentication with Oculus failed."));
        InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    ovrUserProofHandle ProofHandle = ovr_Message_GetUserProof(InHandle);
    const char *ProofNonce = ovr_UserProof_GetNonce(ProofHandle);

    FString OculusToken = FString::Printf(TEXT("%llu|%s"), InUserId, ANSI_TO_TCHAR(ProofNonce));

    TMap<FString, FString> OculusAttrs;
    OculusAttrs.Add(TEXT("oculus.userId"), FString::Printf(TEXT("%llu"), InUserId));
    OculusAttrs.Add(TEXT("oculus.proofNonce"), ANSI_TO_TCHAR(ProofNonce));

    UE_LOG(LogEOSPlatformDefault, Verbose, TEXT("Authenticating with Oculus user proof token: %s"), *OculusToken);

    InState->AvailableExternalCredentials.Add(
        MakeShared<FOculusExternalCredentials>(OculusToken, InDisplayName, OculusAttrs));
    InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
}

void FGetExternalCredentialsFromOculusNode::OnUserDetailsReceived(
    ovrMessageHandle InHandle,
    bool bIsError,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    ovrID InUserId,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<FAuthenticationGraphState> InState,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FAuthenticationGraphNodeOnDone InOnDone)
{
    if (bIsError || ovr_Message_IsError(InHandle))
    {
        UE_LOG(LogEOSPlatformDefault, Error, TEXT("Authentication with Oculus failed."));
        InState->ErrorMessages.Add(TEXT("Authentication with Oculus failed."));
        InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    ovrUserHandle UserInfo = ovr_Message_GetUser(InHandle);

    // We can not use ovr_User_GetDisplayName because the Oculus SDK that Unreal ships with is too old.
    const char *DisplayName = ovr_User_GetOculusID(UserInfo);

    FOnlineSubsystemOculus *OSSSubsystem =
        (FOnlineSubsystemOculus *)Online::GetSubsystem(InState->GetWorld(), OCULUS_SUBSYSTEM);
    checkf(OSSSubsystem != nullptr, TEXT("Oculus OSS went away between requests!"));

    ovrRequest UserProofRequest = ovr_User_GetUserProof();
    OSSSubsystem->AddRequestDelegate(
        UserProofRequest,
        FOculusMessageOnCompleteDelegate::CreateSP(
            this,
            &FGetExternalCredentialsFromOculusNode::OnUserProofReceived,
            InUserId,
            FString(UTF8_TO_TCHAR(DisplayName)),
            InState,
            InOnDone));
}

void FGetExternalCredentialsFromOculusNode::Execute(
    TSharedRef<FAuthenticationGraphState> InState,
    FAuthenticationGraphNodeOnDone InOnDone)
{
    ovrID OculusId = ovr_GetLoggedInUserID();
    if (OculusId == 0)
    {
        UE_LOG(
            LogEOSPlatformDefault,
            Error,
            TEXT("Can't sign into Oculus. Make sure Oculus is running and you are entitled to the app."));
        InState->ErrorMessages.Add(
            TEXT("Can't sign into Oculus. Make sure Oculus is running and you are entitled to the app."));
        InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    FOnlineSubsystemOculus *OSSSubsystem =
        (FOnlineSubsystemOculus *)Online::GetSubsystem(InState->GetWorld(), OCULUS_SUBSYSTEM);
    if (OSSSubsystem == nullptr)
    {
        UE_LOG(
            LogEOSPlatformDefault,
            Error,
            TEXT("Could not authenticate with Oculus, because the Oculus OSS was not available. Check that Oculus OSS "
                 "is enabled in your DefaultEngine.ini file."));
        InState->ErrorMessages.Add(TEXT("Could not authenticate with Oculus, because the Oculus OSS was not available. "
                                        "Check that Oculus OSS is enabled in your DefaultEngine.ini file."));
        InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    ovrRequest UserDetailsRequest = ovr_User_GetLoggedInUser();
    OSSSubsystem->AddRequestDelegate(
        UserDetailsRequest,
        FOculusMessageOnCompleteDelegate::CreateSP(
            this,
            &FGetExternalCredentialsFromOculusNode::OnUserDetailsReceived,
            OculusId,
            InState,
            InOnDone));
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION && EOS_OCULUS_ENABLED