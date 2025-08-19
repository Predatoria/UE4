// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION && EOS_STEAM_ENABLED

#include "GetExternalCredentialsFromSteamNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/OnlineExternalCredentials.h"

#include "LogEOSPlatformDefault.h"
#include "OnlineSubsystemUtils.h"

#include "OnlineEncryptedAppTicketInterfaceSteam.h"
#include "OnlineSubsystemSteam.h"
THIRD_PARTY_INCLUDES_START
#include "steam/steam_api.h"
THIRD_PARTY_INCLUDES_END

EOS_ENABLE_STRICT_WARNINGS

#if EOS_VERSION_AT_LEAST(1, 15, 1) ||                                                                                  \
    (EOS_VERSION_AT_LEAST(1, 14, 2) && EOS_VERSION_AT_MOST(1, 15, 0) && !EOS_VERSION_AT_LEAST(1, 15, 0) &&             \
     defined(EOS_HOTFIX_VERSION) && EOS_HOTFIX_VERSION >= 2)
// Steam authentication using session tickets. Since this works for both EOS_Auth and EOS_Connect as of 1.15.1
// and 1.14.2-hf-2, we use it for both.
//
// We still gather the encrypted app ticket to expose in authentication attributes, in case customers are using
// it to authenticate with their own APIs, but we no longer use it for authentication ourselves.
#define USE_STEAM_SESSION_TICKETS 1
#else
// Legacy Steam authentication using encrypted app tickets. This is no longer permitted for Epic Games accounts
// (EOS_Auth) as of 1.15.1 and 1.14.2-hf-2.
#define USE_STEAM_SESSION_TICKETS 0
#endif

class FAuthTicketCallbackHandler
{
private:
    class FSteamCredentialObtainer *Obtainer;
    STEAM_CALLBACK(FAuthTicketCallbackHandler, OnGetAuthSessionTicketResponse, GetAuthSessionTicketResponse_t);

public:
    FAuthTicketCallbackHandler(class FSteamCredentialObtainer *InObtainer)
        : Obtainer(InObtainer){};
};

class FSteamCredentialObtainer
    : public FAuthenticationCredentialObtainer<FSteamCredentialObtainer, FSteamCredentialInfo>
{
    friend class FAuthTicketCallbackHandler;

private:
#if EOS_STEAM_HAS_NEWER_APP_TICKET_INTERFACE
    FDelegateHandle EncryptedAppTicketDelegateHandle;
#endif
    FString UserId;
    FString SessionTicket;
    FString AppTicket;
    bool bSuccess;
    bool bComplete;
    TWeakPtr<FOnlineEncryptedAppTicketSteam, ESPMode::ThreadSafe> OSSEncryptedAppTicketWk;
    TWeakPtr<IOnlineIdentity, ESPMode::ThreadSafe> OSSIdentityWk;
    HAuthTicket AuthTicketHandle;
    TSharedPtr<FAuthTicketCallbackHandler, ESPMode::ThreadSafe> CallbackHandler;
    bool bAuthTicketWaiting;
    bool bAuthTicketSynced;

    EOS_EResult ConvertToHexString(const TArray<uint8> &Data, FString &OutAppTicket);
    void OnEncryptedAppTicketObtained(bool bEncryptedDataAvailable, int32 ResultCode);
    void BeginObtainSessionTicket(FOnlineExternalCredentialsRefreshComplete OnComplete);

public:
    FSteamCredentialObtainer(const FSteamCredentialObtainer::FOnCredentialObtained &Cb)
        : FAuthenticationCredentialObtainer(Cb)
#if EOS_STEAM_HAS_NEWER_APP_TICKET_INTERFACE
        , EncryptedAppTicketDelegateHandle()
#endif
        , SessionTicket(TEXT(""))
        , AppTicket(TEXT(""))
        , bSuccess(false)
        , bComplete(false)
        , AuthTicketHandle()
        , CallbackHandler(nullptr)
        , bAuthTicketWaiting(false)
        , bAuthTicketSynced(false){};
    UE_NONCOPYABLE(FSteamCredentialObtainer);

    virtual bool Init(UWorld *World, int32 LocalUserNum) override;
    virtual bool Tick(float DeltaSeconds) override;
};

void FAuthTicketCallbackHandler::OnGetAuthSessionTicketResponse(GetAuthSessionTicketResponse_t *Callback)
{
    // We don't use the auth ticket in the callback, we just need to know when it's synced to the server.
    // WARNING: This code runs on a different thread than the main authentication graph.
    Obtainer->bAuthTicketSynced = true;
}

EOS_EResult FSteamCredentialObtainer::ConvertToHexString(const TArray<uint8> &Data, FString &OutHexString)
{
    uint8_t *Block = (uint8_t *)FMemory::Malloc(Data.Num());
    for (auto i = 0; i < Data.Num(); i++)
    {
        Block[i] = Data[i];
    }
    uint32_t TokenLen = 4096;
    char TokenBuffer[4096];
    auto Result = EOS_ByteArray_ToString(Block, Data.Num(), TokenBuffer, &TokenLen);
    FMemory::Free(Block);
    if (Result != EOS_EResult::EOS_Success)
    {
        OutHexString = TEXT("");
    }
    else
    {
        OutHexString = FString(ANSI_TO_TCHAR(TokenBuffer));
    }
    return Result;
}

void FSteamCredentialObtainer::OnEncryptedAppTicketObtained(bool bEncryptedDataAvailable, int32 ResultCode)
{
    auto OSSEncryptedAppTicket = this->OSSEncryptedAppTicketWk.Pin();
    auto OSSIdentity = this->OSSIdentityWk.Pin();
    if (!OSSEncryptedAppTicket.IsValid() || !OSSIdentity.IsValid())
    {
        this->EmitError(TEXT("Steam OSS is no longer available to handle encrypted app ticket response with."));
        this->bSuccess = false;
        this->bComplete = true;
        return;
    }

#if EOS_STEAM_HAS_NEWER_APP_TICKET_INTERFACE
    OSSEncryptedAppTicket->OnEncryptedAppTicketResultDelegate.Remove(this->EncryptedAppTicketDelegateHandle);
#else
    OSSEncryptedAppTicket->OnEncryptedAppTicketResultDelegate.Unbind();
#endif

#if USE_STEAM_SESSION_TICKETS
    // When using session tickets, it's optional for the encrypted app ticket to succeed.
    this->AppTicket = TEXT("");
    if (bEncryptedDataAvailable)
    {
        TArray<uint8> Data;
        auto Success = OSSEncryptedAppTicket->GetEncryptedAppTicket(Data);
        if (Success)
        {
            FString NewAppTicket;
            EOS_EResult Result = ConvertToHexString(Data, NewAppTicket);
            if (Result == EOS_EResult::EOS_Success)
            {
                this->AppTicket = NewAppTicket;
            }
        }
    }
#else
    if (!bEncryptedDataAvailable)
    {
        this->EmitError(FString::Printf(TEXT("Unable to get encrypted app ticket, result code: %d"), ResultCode));
        this->bSuccess = false;
        this->bComplete = true;
        return;
    }

    TArray<uint8> Data;
    auto Success = OSSEncryptedAppTicket->GetEncryptedAppTicket(Data);
    if (!Success)
    {
        this->EmitError(TEXT("Failed to GetEncryptedAppTicket after bEncryptedDataAvailable true"));
        this->bSuccess = false;
        this->bComplete = true;
        return;
    }

    FString NewAppTicket;
    EOS_EResult Result = ConvertToHexString(Data, NewAppTicket);
    if (Result != EOS_EResult::EOS_Success)
    {
        this->EmitError(TEXT("EOS_ByteArray_ToString call failed for encrypted app ticket"));
        this->bSuccess = false;
        this->bComplete = true;
        return;
    }

    this->AppTicket = NewAppTicket;
#endif

    this->UserId = OSSIdentity->GetUniquePlayerId(0)->ToString();

    // Get the SteamUser interface directly.
    auto SteamUserPtr = SteamUser();
    if (SteamUserPtr == nullptr)
    {
        this->EmitError(TEXT("SteamUser() interface not available to get session ticket"));
        this->bSuccess = false;
        this->bComplete = true;
        return;
    }

    // Now start obtaining the session ticket.
    const int SteamAuthMaxTicketLengthInBytes = 1024;
    uint8 AuthTicket[SteamAuthMaxTicketLengthInBytes];
    uint32 AuthTicketSize = 0;
    this->bAuthTicketWaiting = true;
    this->bAuthTicketSynced = false;
    this->CallbackHandler = MakeShareable(new FAuthTicketCallbackHandler(this));
    this->AuthTicketHandle =
        SteamUserPtr->GetAuthSessionTicket(AuthTicket, UE_ARRAY_COUNT(AuthTicket), &AuthTicketSize);
    if (this->AuthTicketHandle != k_HAuthTicketInvalid && AuthTicketSize > 0)
    {
        FString NewSessionTicket;
        EOS_EResult SessionTicketResult =
            ConvertToHexString(TArray<uint8>(AuthTicket, AuthTicketSize), NewSessionTicket);
        if (SessionTicketResult != EOS_EResult::EOS_Success)
        {
            this->EmitError(TEXT("EOS_ByteArray_ToString call failed for session ticket"));
            this->bSuccess = false;
            this->bComplete = true;
            return;
        }

        this->SessionTicket = NewSessionTicket;

        // Now we wait for the callback to fire.
    }
    else
    {
        this->bAuthTicketWaiting = false;
        this->bAuthTicketSynced = false;
        this->CallbackHandler.Reset();

        this->EmitError(TEXT("Unable to start obtainment of session ticket from SteamUser() interface"));
        this->bSuccess = false;
        this->bComplete = true;
    }
}

bool FSteamCredentialObtainer::Init(UWorld *World, int32 LocalUserNum)
{
    if (World == nullptr)
    {
        this->EmitError(TEXT("Could not authenticate with Steam, because the UWorld* pointer was null."));
        this->Done(false, FSteamCredentialInfo());
        return false;
    }

    FOnlineSubsystemSteam *OSSSubsystem = (FOnlineSubsystemSteam *)Online::GetSubsystem(World, STEAM_SUBSYSTEM);
    if (OSSSubsystem == nullptr)
    {
        this->EmitError(TEXT("Could not authenticate with online subsystem, because the subsystem was not "
                             "available. Check that Steam OSS is enabled in your DefaultEngine.ini file."));
        this->Done(false, FSteamCredentialInfo());
        return false;
    }

    FOnlineEncryptedAppTicketSteamPtr OSSEncryptedAppTicket = OSSSubsystem->GetEncryptedAppTicketInterface();
    if (OSSEncryptedAppTicket == nullptr)
    {
        this->EmitError(
            TEXT("Could not authenticate with Steam, because the encrypted app ticket interface was not available."));
        this->Done(false, FSteamCredentialInfo());
        return false;
    }

    TSharedPtr<IOnlineIdentity, ESPMode::ThreadSafe> OSSIdentity =
        StaticCastSharedPtr<IOnlineIdentity>(OSSSubsystem->GetIdentityInterface());
    if (!OSSIdentity.IsValid())
    {
        this->EmitError(TEXT("Could not authenticate with Steam, because the identity interface was not available."));
        this->Done(false, FSteamCredentialInfo());
        return false;
    }

    this->OSSEncryptedAppTicketWk = OSSEncryptedAppTicket;
    this->OSSIdentityWk = OSSIdentity;

#if EOS_STEAM_HAS_NEWER_APP_TICKET_INTERFACE
    this->EncryptedAppTicketDelegateHandle = OSSEncryptedAppTicket->OnEncryptedAppTicketResultDelegate.AddSP(
        this,
        &FSteamCredentialObtainer::OnEncryptedAppTicketObtained);
#else
    OSSEncryptedAppTicket->OnEncryptedAppTicketResultDelegate =
        FOnEncryptedAppTicketResponseDelegate::CreateSP(this, &FSteamCredentialObtainer::OnEncryptedAppTicketObtained);
#endif
    OSSEncryptedAppTicket->RequestEncryptedAppTicket(nullptr, 0);
    return true;
}

bool FSteamCredentialObtainer::Tick(float DeltaSeconds)
{
    if (!this->bComplete)
    {
        if (this->bAuthTicketWaiting && this->bAuthTicketSynced)
        {
            // Auth ticket has been synced.
            this->bAuthTicketWaiting = false;

            // This is the last thing to happen.
            this->bSuccess = true;
            this->bComplete = true;

            // Fallthrough to success handler below.
        }
        else
        {
            return true;
        }
    }

    if (this->bSuccess)
    {
        TMap<FString, FString> UserAuthAttributes;
        UserAuthAttributes.Add(EOS_AUTH_ATTRIBUTE_AUTHENTICATEDWITH, TEXT("steam"));
        UserAuthAttributes.Add(TEXT("steam.id"), this->UserId);
        UserAuthAttributes.Add(TEXT("steam.encryptedAppTicket"), this->AppTicket);
        UserAuthAttributes.Add(TEXT("steam.sessionTicket"), this->SessionTicket);

        FSteamCredentialInfo Info;
        Info.UserId = this->UserId;
        Info.EncryptedAppTicket = this->AppTicket;
        Info.SessionTicket = this->SessionTicket;
        Info.AuthAttributes = UserAuthAttributes;
        this->Done(true, Info);
    }
    else
    {
        this->Done(false, FSteamCredentialInfo());
    }
    return false;
}

class FSteamExternalCredentials : public IOnlineExternalCredentials
{
private:
    FSteamCredentialInfo CredentialInfo;

    void OnCredentialsRefreshed(
        bool bWasSuccessful,
        FSteamCredentialInfo NewCredentials,
        FOnlineExternalCredentialsRefreshComplete OnComplete);

public:
    FSteamExternalCredentials(const FSteamCredentialInfo &InCredentialInfo)
        : CredentialInfo(InCredentialInfo){};
    UE_NONCOPYABLE(FSteamExternalCredentials);
    virtual ~FSteamExternalCredentials(){};
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

FText FSteamExternalCredentials::GetProviderDisplayName() const
{
    return NSLOCTEXT("OnlineSubsystemEOSAuthSteam", "Platform_Steam", "Steam");
}

FString FSteamExternalCredentials::GetType() const
{
#if USE_STEAM_SESSION_TICKETS
    return TEXT("STEAM_SESSION_TICKET");
#else
    return TEXT("STEAM_APP_TICKET");
#endif
}

FString FSteamExternalCredentials::GetId() const
{
    return TEXT("");
}

FString FSteamExternalCredentials::GetToken() const
{
#if USE_STEAM_SESSION_TICKETS
    return this->CredentialInfo.SessionTicket;
#else
    return this->CredentialInfo.EncryptedAppTicket;
#endif
}

TMap<FString, FString> FSteamExternalCredentials::GetAuthAttributes() const
{
    return this->CredentialInfo.AuthAttributes;
}

FName FSteamExternalCredentials::GetNativeSubsystemName() const
{
    return STEAM_SUBSYSTEM;
}

void FSteamExternalCredentials::OnCredentialsRefreshed(
    bool bWasSuccessful,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FSteamCredentialInfo NewCredentialInfo,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FOnlineExternalCredentialsRefreshComplete OnComplete)
{
    if (!bWasSuccessful)
    {
        OnComplete.ExecuteIfBound(false);
        return;
    }

    this->CredentialInfo = MoveTemp(NewCredentialInfo);
    OnComplete.ExecuteIfBound(true);
}

void FSteamExternalCredentials::Refresh(
    TSoftObjectPtr<UWorld> InWorld,
    int32 LocalUserNum,
    FOnlineExternalCredentialsRefreshComplete OnComplete)
{
    FSteamCredentialObtainer::StartFromCredentialRefresh(
        InWorld,
        LocalUserNum,
        FSteamCredentialObtainer::FOnCredentialObtained::CreateSP(
            this,
            &FSteamExternalCredentials::OnCredentialsRefreshed,
            OnComplete));
}

void FGetExternalCredentialsFromSteamNode::OnCredentialsObtained(
    bool bWasSuccessful,
    struct FSteamCredentialInfo ObtainedCredentials,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<FAuthenticationGraphState> InState,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FAuthenticationGraphNodeOnDone InOnDone)
{
    if (!bWasSuccessful)
    {
        InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    InState->AvailableExternalCredentials.Add(MakeShared<FSteamExternalCredentials>(ObtainedCredentials));
    InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
}

void FGetExternalCredentialsFromSteamNode::Execute(
    TSharedRef<FAuthenticationGraphState> InState,
    FAuthenticationGraphNodeOnDone InOnDone)
{
    FSteamCredentialObtainer::StartFromAuthenticationGraph(
        InState,
        FSteamCredentialObtainer::FOnCredentialObtained::CreateSP(
            this,
            &FGetExternalCredentialsFromSteamNode::OnCredentialsObtained,
            InState,
            InOnDone));
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION && EOS_STEAM_ENABLED