// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGamesCrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/UniqueNetIdEAS.h"

EOS_ENABLE_STRICT_WARNINGS

class FOnlineIdentityInterfaceEAS : public IOnlineIdentity,
                                    public TSharedFromThis<FOnlineIdentityInterfaceEAS, ESPMode::ThreadSafe>
{
    friend class FOnlineIdentityInterfaceEOS;

private:
    TMap<int32, TSharedPtr<const FUniqueNetIdEAS>> SignedInUsers;

    // These functions are called by FOnlineIdentityInterfaceEOS whenever a local user is signed in or out, and they are
    // using an Epic account as their cross-platform account. The interface is otherwise a read-only implementation and
    // only exists so the other implemented interfaces can use it (instead of referencing the EOS side of things
    // directly).
    void UserSignedInWithEpicId(
        int32 InLocalUserNum,
        const TSharedPtr<const FEpicGamesCrossPlatformAccountId> &InCrossPlatformAccountId);
    void UserSignedOut(int32 InLocalUserNum);

public:
    FOnlineIdentityInterfaceEAS() = default;
    UE_NONCOPYABLE(FOnlineIdentityInterfaceEAS);

    virtual bool Login(int32 LocalUserNum, const FOnlineAccountCredentials &AccountCredentials) override;
    virtual bool Logout(int32 LocalUserNum) override;
    virtual bool AutoLogin(int32 LocalUserNum) override;
    virtual TSharedPtr<FUserOnlineAccount> GetUserAccount(const FUniqueNetId &UserId) const override;
    virtual TArray<TSharedPtr<FUserOnlineAccount>> GetAllUserAccounts() const override;
    virtual TSharedPtr<const FUniqueNetId> GetUniquePlayerId(int32 LocalUserNum) const override;
    virtual bool GetLocalUserNum(const FUniqueNetId &UniqueNetId, int32 &OutLocalUserNum) const;
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

    /** Returns the Epic account ID of the given local player, by their local user num. */
    virtual EOS_EpicAccountId GetEpicAccountId(int32 LocalUserNum) const;
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION