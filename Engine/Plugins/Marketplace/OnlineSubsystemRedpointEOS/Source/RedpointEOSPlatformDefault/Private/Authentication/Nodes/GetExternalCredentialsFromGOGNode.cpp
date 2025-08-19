// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION && EOS_GOG_ENABLED

#include "GetExternalCredentialsFromGOGNode.h"

#include "LogEOSPlatformDefault.h"

THIRD_PARTY_INCLUDES_START
#include <galaxy/GalaxyApi.h>
THIRD_PARTY_INCLUDES_END

EOS_ENABLE_STRICT_WARNINGS

// There is no documentation on how long this ticket is.
#define ENCRYPTED_APP_TICKET_LENGTH 2048

DECLARE_DELEGATE_TwoParams(FOnGOGAppTicketResult, bool /* bWasSuccessful */, FString /* AppTicket */);

class FGOGEncryptedAppTicketTask : public galaxy::api::IEncryptedAppTicketListener,
                                   public TSharedFromThis<FGOGEncryptedAppTicketTask>
{
private:
    TSharedPtr<FGOGEncryptedAppTicketTask> SelfReference;

    FOnGOGAppTicketResult Callback;

public:
    FGOGEncryptedAppTicketTask(const FOnGOGAppTicketResult &InCallback)
    {
        this->Callback = InCallback;
    }

    virtual void Start();

    virtual void OnEncryptedAppTicketRetrieveSuccess() override;
    virtual void OnEncryptedAppTicketRetrieveFailure(FailureReason failureReason) override;
};

void FGOGEncryptedAppTicketTask::Start()
{
    checkf(!this->SelfReference.IsValid(), TEXT("GOG encrypted app ticket task already started!"));

    galaxy::api::User()->RequestEncryptedAppTicket(nullptr, 0, this);
    this->SelfReference = this->AsShared(); // Keep ourselves alive until the async task completes.
}

void FGOGEncryptedAppTicketTask::OnEncryptedAppTicketRetrieveSuccess()
{
    void *EncryptedAppTicketBuffer = FMemory::Malloc(ENCRYPTED_APP_TICKET_LENGTH);
    uint32 EncryptedAppTicketSize = 0;

    galaxy::api::User()->GetEncryptedAppTicket(
        EncryptedAppTicketBuffer,
        ENCRYPTED_APP_TICKET_LENGTH,
        EncryptedAppTicketSize);

    uint32_t TokenLen = 4096;
    char TokenBuffer[4096];
    auto Result = EOS_ByteArray_ToString(
        (const uint8_t *)EncryptedAppTicketBuffer,
        EncryptedAppTicketSize,
        TokenBuffer,
        &TokenLen);
    FMemory::Free(EncryptedAppTicketBuffer);
    if (Result != EOS_EResult::EOS_Success)
    {
        this->Callback.ExecuteIfBound(false, TEXT(""));
    }
    else
    {
        this->Callback.ExecuteIfBound(true, ANSI_TO_TCHAR(TokenBuffer));
    }
    this->SelfReference = nullptr;
}

void FGOGEncryptedAppTicketTask::OnEncryptedAppTicketRetrieveFailure(
    galaxy::api::IEncryptedAppTicketListener::FailureReason failureReason)
{
    this->Callback.ExecuteIfBound(false, TEXT(""));
    this->SelfReference = nullptr;
}

class FGOGExternalCredentials : public IOnlineExternalCredentials
{
private:
    FString SessionTicket;
    TMap<FString, FString> AuthAttributes;

    void OnEncryptedAppTicketResult(
        bool bWasSuccessful,
        FString EncodedAppTicket,
        FOnlineExternalCredentialsRefreshComplete OnComplete);

public:
    FGOGExternalCredentials(const FString &InAppTicket, const TMap<FString, FString> &InAuthAttributes);
    virtual ~FGOGExternalCredentials(){};
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

FGOGExternalCredentials::FGOGExternalCredentials(
    const FString &InAppTicket,
    const TMap<FString, FString> &InAuthAttributes)
{
    this->SessionTicket = InAppTicket;
    this->AuthAttributes = InAuthAttributes;
}

FText FGOGExternalCredentials::GetProviderDisplayName() const
{
    return NSLOCTEXT("OnlineSubsystemRedpointEOS", "Platform_GOG", "GOG");
}

FString FGOGExternalCredentials::GetType() const
{
    return TEXT("GOG_SESSION_TICKET");
}

FString FGOGExternalCredentials::GetId() const
{
    return TEXT("");
}

FString FGOGExternalCredentials::GetToken() const
{
    return this->SessionTicket;
}

TMap<FString, FString> FGOGExternalCredentials::GetAuthAttributes() const
{
    return this->AuthAttributes;
}

FName FGOGExternalCredentials::GetNativeSubsystemName() const
{
    return FName(TEXT("GOG"));
}

void FGOGExternalCredentials::Refresh(
    TSoftObjectPtr<UWorld> InWorld,
    int32 LocalUserNum,
    FOnlineExternalCredentialsRefreshComplete OnComplete)
{
    TSharedPtr<FGOGEncryptedAppTicketTask> Task = MakeShared<FGOGEncryptedAppTicketTask>(
        FOnGOGAppTicketResult::CreateSP(this, &FGOGExternalCredentials::OnEncryptedAppTicketResult, OnComplete));
    Task->Start();
}

void FGOGExternalCredentials::OnEncryptedAppTicketResult(
    bool bWasSuccessful,
    FString AppTicket,
    FOnlineExternalCredentialsRefreshComplete OnComplete)
{
    if (!bWasSuccessful)
    {
        UE_LOG(LogEOSPlatformDefault, Error, TEXT("Unable to refresh encrypted app ticket from GOG"));
        OnComplete.ExecuteIfBound(false);
        return;
    }

    auto UserId = galaxy::api::User()->GetGalaxyID();

    this->AuthAttributes.Add(TEXT("gog.id"), FString::Printf(TEXT("%llu"), UserId.ToUint64()));
    this->AuthAttributes.Add(TEXT("gog.encryptedAppTicket"), AppTicket);

    UE_LOG(LogEOSPlatformDefault, Verbose, TEXT("Refreshed with new GOG app ticket: %s"), *AppTicket);

    this->SessionTicket = AppTicket;
    OnComplete.ExecuteIfBound(true);
}

void FGetExternalCredentialsFromGOGNode::OnEncryptedAppTicketResult(
    bool bWasSuccessful,
    FString AppTicket,
    TSharedRef<FAuthenticationGraphState> InState,
    FAuthenticationGraphNodeOnDone InOnDone)
{
    if (!bWasSuccessful)
    {
        UE_LOG(LogEOSPlatformDefault, Error, TEXT("Unable to get encrypted app ticket from GOG"));
        InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    auto UserId = galaxy::api::User()->GetGalaxyID();

    TMap<FString, FString> UserAuthAttributes;
    UserAuthAttributes.Add(EOS_AUTH_ATTRIBUTE_AUTHENTICATEDWITH, TEXT("gog"));
    UserAuthAttributes.Add(TEXT("gog.id"), FString::Printf(TEXT("%llu"), UserId.ToUint64()));
    UserAuthAttributes.Add(TEXT("gog.encryptedAppTicket"), AppTicket);

    UE_LOG(LogEOSPlatformDefault, Verbose, TEXT("Authenticating with GOG app ticket: %s"), *AppTicket);

    InState->AvailableExternalCredentials.Add(MakeShared<FGOGExternalCredentials>(AppTicket, UserAuthAttributes));
    InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
    return;
}

void FGetExternalCredentialsFromGOGNode::Execute(
    TSharedRef<FAuthenticationGraphState> InState,
    FAuthenticationGraphNodeOnDone InOnDone)
{
    TSharedPtr<FGOGEncryptedAppTicketTask> Task =
        MakeShared<FGOGEncryptedAppTicketTask>(FOnGOGAppTicketResult::CreateSP(
            this,
            &FGetExternalCredentialsFromGOGNode::OnEncryptedAppTicketResult,
            InState,
            InOnDone));
    Task->Start();
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION && EOS_STEAM_ENABLED