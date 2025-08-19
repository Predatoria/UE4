// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "Interfaces/OnlineUserInterface.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineIdentityInterfaceEAS.h"

EOS_ENABLE_STRICT_WARNINGS

class FOnlineUserInterfaceEAS : public IOnlineUser, public TSharedFromThis<FOnlineUserInterfaceEAS, ESPMode::ThreadSafe>
{
private:
    EOS_HUserInfo EOSUserInfo;
    TSharedRef<const FOnlineIdentityInterfaceEAS, ESPMode::ThreadSafe> Identity;

public:
    FOnlineUserInterfaceEAS(
        EOS_HPlatform InPlatform,
        TSharedRef<const FOnlineIdentityInterfaceEAS, ESPMode::ThreadSafe> InIdentity);
    UE_NONCOPYABLE(FOnlineUserInterfaceEAS);

    virtual bool QueryUserInfo(int32 LocalUserNum, const TArray<TSharedRef<const FUniqueNetId>> &UserIds) override;
    virtual bool GetAllUserInfo(int32 LocalUserNum, TArray<TSharedRef<class FOnlineUser>> &OutUsers) override;
    virtual TSharedPtr<FOnlineUser> GetUserInfo(int32 LocalUserNum, const class FUniqueNetId &UserId) override;
    virtual bool QueryUserIdMapping(
        const FUniqueNetId &UserId,
        const FString &DisplayNameOrEmail,
        const FOnQueryUserMappingComplete &Delegate = FOnQueryUserMappingComplete()) override;
    virtual bool QueryExternalIdMappings(
        const FUniqueNetId &UserId,
        const FExternalIdQueryOptions &QueryOptions,
        const TArray<FString> &ExternalIds,
        const FOnQueryExternalIdMappingsComplete &Delegate = FOnQueryExternalIdMappingsComplete()) override;
    virtual void GetExternalIdMappings(
        const FExternalIdQueryOptions &QueryOptions,
        const TArray<FString> &ExternalIds,
        TArray<TSharedPtr<const FUniqueNetId>> &OutIds) override;
    virtual TSharedPtr<const FUniqueNetId> GetExternalIdMapping(
        const FExternalIdQueryOptions &QueryOptions,
        const FString &ExternalId) override;
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION