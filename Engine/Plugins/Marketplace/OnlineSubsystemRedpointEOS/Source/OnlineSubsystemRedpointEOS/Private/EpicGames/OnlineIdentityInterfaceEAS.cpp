// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineIdentityInterfaceEAS.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"

EOS_ENABLE_STRICT_WARNINGS

void FOnlineIdentityInterfaceEAS::UserSignedInWithEpicId(
    int32 InLocalUserNum,
    const TSharedPtr<const FEpicGamesCrossPlatformAccountId> &InCrossPlatformAccountId)
{
    this->SignedInUsers.Add(InLocalUserNum, MakeShared<FUniqueNetIdEAS>(InCrossPlatformAccountId->GetEpicAccountId()));
}

void FOnlineIdentityInterfaceEAS::UserSignedOut(int32 InLocalUserNum)
{
    this->SignedInUsers.Remove(InLocalUserNum);
}

bool FOnlineIdentityInterfaceEAS::Login(int32 LocalUserNum, const FOnlineAccountCredentials &AccountCredentials)
{
    return false;
}

bool FOnlineIdentityInterfaceEAS::Logout(int32 LocalUserNum)
{
    return false;
}

bool FOnlineIdentityInterfaceEAS::AutoLogin(int32 LocalUserNum)
{
    return false;
}

TSharedPtr<FUserOnlineAccount> FOnlineIdentityInterfaceEAS::GetUserAccount(const FUniqueNetId &UserId) const
{
    return nullptr;
}

TArray<TSharedPtr<FUserOnlineAccount>> FOnlineIdentityInterfaceEAS::GetAllUserAccounts() const
{
    return TArray<TSharedPtr<FUserOnlineAccount>>();
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityInterfaceEAS::GetUniquePlayerId(int32 LocalUserNum) const
{
    if (this->SignedInUsers.Contains(LocalUserNum))
    {
        return this->SignedInUsers[LocalUserNum];
    }

    return nullptr;
}

bool FOnlineIdentityInterfaceEAS::GetLocalUserNum(const FUniqueNetId &UniqueNetId, int32 &OutLocalUserNum) const
{
    for (const auto &KV : this->SignedInUsers)
    {
        if (KV.Value.IsValid() && *KV.Value == UniqueNetId)
        {
            OutLocalUserNum = KV.Key;
            return true;
        }
    }

    OutLocalUserNum = 0;
    return false;
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityInterfaceEAS::CreateUniquePlayerId(uint8 *Bytes, int32 Size)
{
    FString Data = BytesToString(Bytes, Size);
    return FUniqueNetIdEAS::ParseFromString(Data);
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityInterfaceEAS::CreateUniquePlayerId(const FString &Str)
{
    return FUniqueNetIdEAS::ParseFromString(Str);
}

ELoginStatus::Type FOnlineIdentityInterfaceEAS::GetLoginStatus(int32 LocalUserNum) const
{
    if (this->SignedInUsers.Contains(LocalUserNum))
    {
        return ELoginStatus::LoggedIn;
    }

    return ELoginStatus::NotLoggedIn;
}

ELoginStatus::Type FOnlineIdentityInterfaceEAS::GetLoginStatus(const FUniqueNetId &UserId) const
{
    for (const auto &KV : this->SignedInUsers)
    {
        if (*KV.Value == UserId)
        {
            return ELoginStatus::LoggedIn;
        }
    }

    return ELoginStatus::NotLoggedIn;
}

FString FOnlineIdentityInterfaceEAS::GetPlayerNickname(int32 LocalUserNum) const
{
    return TEXT("");
}

FString FOnlineIdentityInterfaceEAS::GetPlayerNickname(const FUniqueNetId &UserId) const
{
    return TEXT("");
}

FString FOnlineIdentityInterfaceEAS::GetAuthToken(int32 LocalUserNum) const
{
    return TEXT("");
}

void FOnlineIdentityInterfaceEAS::RevokeAuthToken(
    const FUniqueNetId &LocalUserId,
    const FOnRevokeAuthTokenCompleteDelegate &Delegate)
{
    Delegate.ExecuteIfBound(LocalUserId, OnlineRedpointEOS::Errors::NotImplemented());
}

void FOnlineIdentityInterfaceEAS::GetUserPrivilege(
    const FUniqueNetId &LocalUserId,
    EUserPrivileges::Type Privilege,
    const FOnGetUserPrivilegeCompleteDelegate &Delegate)
{
    Delegate.ExecuteIfBound(LocalUserId, Privilege, 0);
}

FPlatformUserId FOnlineIdentityInterfaceEAS::GetPlatformUserIdFromUniqueNetId(const FUniqueNetId &UniqueNetId) const
{
#if defined(UE_5_0_OR_LATER)
    return PLATFORMUSERID_NONE;
#else
    return 0;
#endif // #if defined(UE_5_0_OR_LATER)
}

FString FOnlineIdentityInterfaceEAS::GetAuthType() const
{
    return TEXT("");
}

/** Returns the Epic account ID of the given local player, by their local user num. */
EOS_EpicAccountId FOnlineIdentityInterfaceEAS::GetEpicAccountId(int32 LocalUserNum) const
{
    if (this->SignedInUsers.Contains(LocalUserNum))
    {
        return this->SignedInUsers[LocalUserNum]->GetEpicAccountId();
    }

    return nullptr;
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION