// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "UniqueNetIdRedpointItchIo.h"

#if EOS_ITCH_IO_ENABLED

class FUserOnlineAccountRedpointItchIo : public FUserOnlineAccount
{
private:
    TSharedRef<const FUniqueNetIdRedpointItchIo> UserId;
    FString CoverUrl;
    FString Username;
    FString DisplayName;

public:
    UE_NONCOPYABLE(FUserOnlineAccountRedpointItchIo);
    FUserOnlineAccountRedpointItchIo(
        TSharedRef<const FUniqueNetIdRedpointItchIo> InUserId,
        FString InDisplayName,
        FString InUsername,
        FString InCoverUrl)
        : UserId(MoveTemp(InUserId))
        , CoverUrl(MoveTemp(InCoverUrl))
        , Username(MoveTemp(InUsername))
        , DisplayName(MoveTemp(InDisplayName)){};
    virtual ~FUserOnlineAccountRedpointItchIo(){};

    virtual TSharedRef<const FUniqueNetId> GetUserId() const override;
    virtual FString GetRealName() const override;
    virtual FString GetDisplayName(const FString &Platform = FString()) const override;
    virtual bool GetUserAttribute(const FString &AttrName, FString &OutAttrValue) const override;
    virtual FString GetAccessToken() const override;
    virtual bool GetAuthAttribute(const FString &AttrName, FString &OutAttrValue) const override;
    virtual bool SetUserAttribute(const FString &AttrName, const FString &AttrValue) override;
};

class FOnlineIdentityInterfaceRedpointItchIo
    : public IOnlineIdentity,
      public TSharedFromThis<FOnlineIdentityInterfaceRedpointItchIo, ESPMode::ThreadSafe>
{
private:
    TSharedPtr<const FUniqueNetIdRedpointItchIo> AuthenticatedUserId;
    TSharedPtr<FUserOnlineAccountRedpointItchIo> AuthenticatedUserAccount;
    bool bLoggedIn;

public:
    FOnlineIdentityInterfaceRedpointItchIo();
    virtual ~FOnlineIdentityInterfaceRedpointItchIo();
    UE_NONCOPYABLE(FOnlineIdentityInterfaceRedpointItchIo);

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