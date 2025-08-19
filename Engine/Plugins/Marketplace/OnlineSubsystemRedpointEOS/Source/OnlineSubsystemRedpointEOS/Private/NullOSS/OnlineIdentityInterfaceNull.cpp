// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Private/NullOSS/OnlineIdentityInterfaceNull.h"

#include "OnlineSubsystemRedpointEOS/Private/NullOSS/UniqueNetIdNull.h"

#define ONLINE_ERROR_NAMESPACE "EOS"

EOS_ENABLE_STRICT_WARNINGS

bool FOnlineIdentityInterfaceNull::Login(int32 LocalUserNum, const FOnlineAccountCredentials &AccountCredentials)
{
    this->OnLoginCompleteDelegates->Broadcast(
        LocalUserNum,
        false,
        *FUniqueNetIdRedpointNull::InvalidId(),
        TEXT("The EOS plugin is currently operating in NULL mode because you didn't have Product Name or Product "
             "Version set when the game or editor started, or because this operation is running inside a commandlet. "
             "Ensure your Project Settings are correct, restart the editor and try again."));
    return true;
}

bool FOnlineIdentityInterfaceNull::Logout(int32 LocalUserNum)
{
    this->OnLogoutCompleteDelegates->Broadcast(LocalUserNum, false);
    return true;
}

bool FOnlineIdentityInterfaceNull::AutoLogin(int32 LocalUserNum)
{
    return this->Login(LocalUserNum, FOnlineAccountCredentials());
}

TSharedPtr<FUserOnlineAccount> FOnlineIdentityInterfaceNull::GetUserAccount(const FUniqueNetId &UserId) const
{
    return nullptr;
}

TArray<TSharedPtr<FUserOnlineAccount>> FOnlineIdentityInterfaceNull::GetAllUserAccounts() const
{
    return TArray<TSharedPtr<FUserOnlineAccount>>();
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityInterfaceNull::GetUniquePlayerId(int32 LocalUserNum) const
{
    return nullptr;
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityInterfaceNull::CreateUniquePlayerId(uint8 *Bytes, int32 Size)
{
    FString Data;
    Data.Empty(Size);
    for (int i = 0; i < Size; i++)
    {
        if (Bytes[i] == 0)
        {
            break;
        }
        Data += (ANSICHAR)Bytes[i];
    }
    return FUniqueNetIdRedpointNull::ParseFromString(Data);
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityInterfaceNull::CreateUniquePlayerId(const FString &Str)
{
    return FUniqueNetIdRedpointNull::ParseFromString(Str);
}

ELoginStatus::Type FOnlineIdentityInterfaceNull::GetLoginStatus(int32 LocalUserNum) const
{
    return ELoginStatus::NotLoggedIn;
}

ELoginStatus::Type FOnlineIdentityInterfaceNull::GetLoginStatus(const FUniqueNetId &UserId) const
{
    return ELoginStatus::NotLoggedIn;
}

FString FOnlineIdentityInterfaceNull::GetPlayerNickname(int32 LocalUserNum) const
{
    return TEXT("");
}

FString FOnlineIdentityInterfaceNull::GetPlayerNickname(const FUniqueNetId &UserId) const
{
    return TEXT("");
}

FString FOnlineIdentityInterfaceNull::GetAuthToken(int32 LocalUserNum) const
{
    return TEXT("");
}

void FOnlineIdentityInterfaceNull::RevokeAuthToken(
    const FUniqueNetId &LocalUserId,
    const FOnRevokeAuthTokenCompleteDelegate &Delegate)
{
    Delegate.ExecuteIfBound(LocalUserId, ONLINE_ERROR(EOnlineErrorResult::NotImplemented));
}

void FOnlineIdentityInterfaceNull::GetUserPrivilege(
    const FUniqueNetId &LocalUserId,
    EUserPrivileges::Type Privilege,
    const FOnGetUserPrivilegeCompleteDelegate &Delegate)
{
    Delegate.ExecuteIfBound(LocalUserId, (EUserPrivileges::Type)0, 1);
}

FPlatformUserId FOnlineIdentityInterfaceNull::GetPlatformUserIdFromUniqueNetId(const FUniqueNetId &UniqueNetId) const
{
    return PLATFORMUSERID_NONE;
}

FString FOnlineIdentityInterfaceNull::GetAuthType() const
{
    return TEXT("");
}

EOS_DISABLE_STRICT_WARNINGS