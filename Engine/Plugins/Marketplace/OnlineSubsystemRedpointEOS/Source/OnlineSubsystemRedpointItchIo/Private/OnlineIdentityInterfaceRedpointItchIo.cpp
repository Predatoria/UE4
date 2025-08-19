// Copyright June Rhodes. All Rights Reserved.

#include "OnlineIdentityInterfaceRedpointItchIo.h"

#if EOS_ITCH_IO_ENABLED

#include "Dom/JsonObject.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "LogRedpointItchIo.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "UniqueNetIdRedpointItchIo.h"

TSharedRef<const FUniqueNetId> FUserOnlineAccountRedpointItchIo::GetUserId() const
{
    return this->UserId;
}

FString FUserOnlineAccountRedpointItchIo::GetRealName() const
{
    return this->Username;
}

FString FUserOnlineAccountRedpointItchIo::GetDisplayName(const FString &Platform) const
{
    return this->DisplayName;
}

bool FUserOnlineAccountRedpointItchIo::GetUserAttribute(const FString &AttrName, FString &OutAttrValue) const
{
    if (AttrName == TEXT("coverUrl"))
    {
        OutAttrValue = this->CoverUrl;
        return true;
    }

    return false;
}

FString FUserOnlineAccountRedpointItchIo::GetAccessToken() const
{
    return TEXT("");
}

bool FUserOnlineAccountRedpointItchIo::GetAuthAttribute(const FString &AttrName, FString &OutAttrValue) const
{
    return false;
}

bool FUserOnlineAccountRedpointItchIo::SetUserAttribute(const FString &AttrName, const FString &AttrValue)
{
    return false;
}

FOnlineIdentityInterfaceRedpointItchIo::FOnlineIdentityInterfaceRedpointItchIo()
    : AuthenticatedUserId(nullptr)
    , AuthenticatedUserAccount(nullptr)
    , bLoggedIn(false)
{
}

FOnlineIdentityInterfaceRedpointItchIo::~FOnlineIdentityInterfaceRedpointItchIo()
{
}

bool FOnlineIdentityInterfaceRedpointItchIo::Login(
    int32 LocalUserNum,
    const FOnlineAccountCredentials &AccountCredentials)
{
    if (LocalUserNum != 0)
    {
        UE_LOG(LogRedpointItchIo, Error, TEXT("Only local player 0 can sign into itch.io."));
        return false;
    }

    if (this->bLoggedIn)
    {
        UE_LOG(LogRedpointItchIo, Error, TEXT("The user is already logged in."));
        return false;
    }

    FString ItchIoApiKey = FPlatformMisc::GetEnvironmentVariable(TEXT("ITCHIO_API_KEY"));
    if (ItchIoApiKey.IsEmpty())
    {
        UE_LOG(
            LogRedpointItchIo,
            Error,
            TEXT("Can't sign in with itch.io, ITCHIO_API_KEY environment variable is missing."));
        return false;
    }

    // Make a request to the itch.io API to get information about the user.
    auto Request = FHttpModule::Get().CreateRequest();
    Request->SetVerb(TEXT("GET"));
    Request->SetHeader(TEXT("Authorization"), ItchIoApiKey);
    Request->SetURL(TEXT("https://itch.io/api/1/jwt/me"));
    Request->OnProcessRequestComplete().BindLambda(
        [WeakThis = GetWeakThis(this),
         LocalUserNum](const FHttpRequestPtr &Request, const FHttpResponsePtr &Response, bool bConnectedSuccessfully) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (!bConnectedSuccessfully || !Response.IsValid() ||
                    !EHttpResponseCodes::IsOk(Response->GetResponseCode()))
                {
                    This->TriggerOnLoginCompleteDelegates(
                        LocalUserNum,
                        false,
                        *MakeShared<FUniqueNetIdRedpointItchIo>(0),
                        TEXT("Unable to get user information from itch.io API."));
                    return;
                }

                TSharedPtr<FJsonObject> ResponseJson;
                TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
                if (!FJsonSerializer::Deserialize(JsonReader, ResponseJson))
                {
                    This->TriggerOnLoginCompleteDelegates(
                        LocalUserNum,
                        false,
                        *MakeShared<FUniqueNetIdRedpointItchIo>(0),
                        TEXT("Unable to decode response from itch.io API."));
                    return;
                }

                TSharedPtr<FJsonObject> UserInfo = ResponseJson->GetObjectField("user");
                if (!UserInfo.IsValid())
                {
                    This->TriggerOnLoginCompleteDelegates(
                        LocalUserNum,
                        false,
                        *MakeShared<FUniqueNetIdRedpointItchIo>(0),
                        TEXT("Unable to decode response from itch.io API."));
                    return;
                }

                int32 UserId = UserInfo->GetIntegerField("id");
                FString CoverUrl = UserInfo->GetStringField("cover_url");
                FString Username = UserInfo->GetStringField("username");
                FString DisplayName = UserInfo->GetStringField("display_name");

                This->AuthenticatedUserId = MakeShared<FUniqueNetIdRedpointItchIo>(UserId);
                This->AuthenticatedUserAccount = MakeShared<FUserOnlineAccountRedpointItchIo>(
                    This->AuthenticatedUserId.ToSharedRef(),
                    DisplayName,
                    Username,
                    CoverUrl);
                This->bLoggedIn = true;
                This->TriggerOnLoginStatusChangedDelegates(
                    LocalUserNum,
                    ELoginStatus::NotLoggedIn,
                    ELoginStatus::LoggedIn,
                    *This->AuthenticatedUserId);
                This->TriggerOnLoginChangedDelegates(LocalUserNum);
                This->TriggerOnLoginCompleteDelegates(LocalUserNum, true, *This->AuthenticatedUserId, TEXT(""));
            }
        });
    Request->ProcessRequest();
    return true;
}

bool FOnlineIdentityInterfaceRedpointItchIo::Logout(int32 LocalUserNum)
{
    if (LocalUserNum != 0)
    {
        UE_LOG(LogRedpointItchIo, Error, TEXT("Only local player 0 can be signed into ItchIo."));
        return false;
    }

    if (!this->bLoggedIn)
    {
        UE_LOG(LogRedpointItchIo, Error, TEXT("The user is not logged in."));
        return false;
    }

    TSharedPtr<const FUniqueNetIdRedpointItchIo> OldUserId = this->AuthenticatedUserId;

    this->bLoggedIn = false;
    this->AuthenticatedUserAccount.Reset();
    this->AuthenticatedUserId.Reset();

    this->TriggerOnLoginStatusChangedDelegates(
        LocalUserNum,
        ELoginStatus::LoggedIn,
        ELoginStatus::NotLoggedIn,
        *OldUserId);
    this->TriggerOnLoginChangedDelegates(LocalUserNum);
    this->TriggerOnLogoutCompleteDelegates(LocalUserNum, true);
    return true;
}

bool FOnlineIdentityInterfaceRedpointItchIo::AutoLogin(int32 LocalUserNum)
{
    return this->Login(0, FOnlineAccountCredentials());
}

TSharedPtr<FUserOnlineAccount> FOnlineIdentityInterfaceRedpointItchIo::GetUserAccount(const FUniqueNetId &UserId) const
{
    if (!this->AuthenticatedUserId.IsValid())
    {
        return nullptr;
    }
    if (UserId == *this->AuthenticatedUserId)
    {
        return this->AuthenticatedUserAccount;
    }
    return nullptr;
}

TArray<TSharedPtr<FUserOnlineAccount>> FOnlineIdentityInterfaceRedpointItchIo::GetAllUserAccounts() const
{
    TArray<TSharedPtr<FUserOnlineAccount>> Results;
    if (this->AuthenticatedUserAccount.IsValid())
    {
        Results.Add(this->AuthenticatedUserAccount);
    }
    return Results;
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityInterfaceRedpointItchIo::GetUniquePlayerId(int32 LocalUserNum) const
{
    return this->AuthenticatedUserId;
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityInterfaceRedpointItchIo::CreateUniquePlayerId(uint8 *Bytes, int32 Size)
{
    FString Data = BytesToString(Bytes, Size);
    return this->CreateUniquePlayerId(Data);
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityInterfaceRedpointItchIo::CreateUniquePlayerId(const FString &Str)
{
    int32 UserId = FCString::Atoi(*Str);
    if (UserId == 0)
    {
        return nullptr;
    }

    return MakeShared<FUniqueNetIdRedpointItchIo>(UserId);
}

ELoginStatus::Type FOnlineIdentityInterfaceRedpointItchIo::GetLoginStatus(int32 LocalUserNum) const
{
    if (LocalUserNum == 0 && this->bLoggedIn)
    {
        return ELoginStatus::LoggedIn;
    }

    return ELoginStatus::NotLoggedIn;
}

ELoginStatus::Type FOnlineIdentityInterfaceRedpointItchIo::GetLoginStatus(const FUniqueNetId &UserId) const
{
    if (this->bLoggedIn && UserId == *this->AuthenticatedUserId)
    {
        return ELoginStatus::LoggedIn;
    }

    return ELoginStatus::NotLoggedIn;
}

FString FOnlineIdentityInterfaceRedpointItchIo::GetPlayerNickname(int32 LocalUserNum) const
{
    if (LocalUserNum == 0 && this->bLoggedIn)
    {
        return this->AuthenticatedUserAccount->GetDisplayName();
    }

    return TEXT("");
}

FString FOnlineIdentityInterfaceRedpointItchIo::GetPlayerNickname(const FUniqueNetId &UserId) const
{
    if (this->bLoggedIn && UserId == *this->AuthenticatedUserId)
    {
        return this->AuthenticatedUserAccount->GetDisplayName();
    }

    return TEXT("");
}

FString FOnlineIdentityInterfaceRedpointItchIo::GetAuthToken(int32 LocalUserNum) const
{
    if (this->bLoggedIn && LocalUserNum == 0)
    {
        return FPlatformMisc::GetEnvironmentVariable(TEXT("ITCHIO_API_KEY"));
    }

    return TEXT("");
}

void FOnlineIdentityInterfaceRedpointItchIo::RevokeAuthToken(
    const FUniqueNetId &LocalUserId,
    const FOnRevokeAuthTokenCompleteDelegate &Delegate)
{
    Delegate.ExecuteIfBound(LocalUserId, FOnlineError(EOnlineErrorResult::NotImplemented));
}

void FOnlineIdentityInterfaceRedpointItchIo::GetUserPrivilege(
    const FUniqueNetId &LocalUserId,
    EUserPrivileges::Type Privilege,
    const FOnGetUserPrivilegeCompleteDelegate &Delegate)
{
    Delegate.ExecuteIfBound(LocalUserId, Privilege, 0);
}

FPlatformUserId FOnlineIdentityInterfaceRedpointItchIo::GetPlatformUserIdFromUniqueNetId(
    const FUniqueNetId &UniqueNetId) const
{
    return FPlatformUserId();
}

FString FOnlineIdentityInterfaceRedpointItchIo::GetAuthType() const
{
    return TEXT("");
}

#endif