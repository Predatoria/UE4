// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DiscordGameSDK.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineExternalUIInterfaceRedpointDiscord.h"
#include "UniqueNetIdRedpointDiscord.h"

#if EOS_DISCORD_ENABLED

class FOnlineIdentityInterfaceRedpointDiscord
    : public IOnlineIdentity,
      public TSharedFromThis<FOnlineIdentityInterfaceRedpointDiscord, ESPMode::ThreadSafe>
{
private:
    TSharedRef<discord::Core> Instance;
    TSharedRef<FOnlineExternalUIInterfaceRedpointDiscord, ESPMode::ThreadSafe> ExternalUI;
    TSharedPtr<const FUniqueNetIdRedpointDiscord> AuthenticatedUserId;
    TSharedPtr<class FUserOnlineAccountRedpointDiscord> AuthenticatedUserAccount;
    bool bLoggedIn;

public:
    FOnlineIdentityInterfaceRedpointDiscord(
        TSharedRef<discord::Core> InInstance,
        TSharedRef<FOnlineExternalUIInterfaceRedpointDiscord, ESPMode::ThreadSafe> InExternalUI);
    UE_NONCOPYABLE(FOnlineIdentityInterfaceRedpointDiscord);
    virtual ~FOnlineIdentityInterfaceRedpointDiscord();

    virtual bool Login(int32 LocalUserNum, const FOnlineAccountCredentials &AccountCredentials) override;
    virtual bool Logout(int32 LocalUserNum) override;
    virtual bool AutoLogin(int32 LocalUserNum) override;
    virtual TSharedPtr<FUserOnlineAccount> GetUserAccount(const FUniqueNetId &UserId) const override;
    virtual TArray<TSharedPtr<FUserOnlineAccount>> GetAllUserAccounts() const override;
    virtual TSharedPtr<const FUniqueNetId> GetUniquePlayerId(int32 LocalUserNum) const override;
    virtual TSharedPtr<const FUniqueNetId> CreateUniquePlayerId(uint8 *Bytes, int32 Size) override;
    virtual TSharedPtr<const FUniqueNetId> CreateUniquePlayerId(const FString &Str) override;
    virtual ELoginStatus::Type GetLoginStatus(int32 LocalUserNum) const override;
    virtual ELoginStatus::Type GetLoginStatus(const FUniqueNetId &UserId) const override;
    virtual FString GetPlayerNickname(int32 LocalUserNum) const override;
    virtual FString GetPlayerNickname(const FUniqueNetId &UserId) const override;
    virtual FString GetAuthToken(int32 LocalUserNum) const override;
    virtual void RevokeAuthToken(const FUniqueNetId &LocalUserId, const FOnRevokeAuthTokenCompleteDelegate &Delegate)
        override;
    virtual void GetUserPrivilege(
        const FUniqueNetId &LocalUserId,
        EUserPrivileges::Type Privilege,
        const FOnGetUserPrivilegeCompleteDelegate &Delegate) override;
    virtual FPlatformUserId GetPlatformUserIdFromUniqueNetId(const FUniqueNetId &UniqueNetId) const override;
    virtual FString GetAuthType() const override;
};

#endif