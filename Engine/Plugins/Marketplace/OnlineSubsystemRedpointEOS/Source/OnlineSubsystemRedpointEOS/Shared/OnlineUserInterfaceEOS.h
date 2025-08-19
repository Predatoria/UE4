// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EOSCommon.h"
#include "Interfaces/OnlineUserInterface.h"
#include "OnlineUserEOS.h"
#include "UniqueNetIdEOS.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FOnlineUserInterfaceEOS
    : public IOnlineUser,
      public TSharedFromThis<FOnlineUserInterfaceEOS, ESPMode::ThreadSafe>
{
    friend class FOnlineSubsystemEOS;

private:
    typedef FString FExternalPlatformName;
    typedef FString FExternalUserId;

    EOS_HPlatform EOSPlatform;
    EOS_HUserInfo EOSUserInfo;
    EOS_HConnect EOSConnect;
    TSharedPtr<class FOnlineSubsystemRedpointEAS, ESPMode::ThreadSafe> SubsystemEAS;
    TSharedPtr<class FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> Identity;
    TSharedPtr<class FEOSUserFactory, ESPMode::ThreadSafe> UserFactory;
    TMap<int32, TUserIdMap<TSharedPtr<FOnlineUser>>> CachedUsers;
    TMap<FExternalPlatformName, TMap<FExternalUserId, TSharedPtr<const FUniqueNetId>>> CachedMappings;

public:
    FOnlineUserInterfaceEOS(
        EOS_HPlatform InPlatform,
        TSharedPtr<class FOnlineSubsystemRedpointEAS, ESPMode::ThreadSafe> InSubsystemEAS,
        TSharedPtr<class FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> InIdentity,
        const TSharedRef<class FEOSUserFactory, ESPMode::ThreadSafe> &InUserFactory);
    UE_NONCOPYABLE(FOnlineUserInterfaceEOS);
    virtual ~FOnlineUserInterfaceEOS();

    virtual bool QueryUserInfo(int32 LocalUserNum, const TArray<TSharedRef<const FUniqueNetId>> &UserIds) override;

    virtual bool GetAllUserInfo(int32 LocalUserNum, TArray<TSharedRef<FOnlineUser>> &OutUsers) override;

    virtual TSharedPtr<FOnlineUser> GetUserInfo(int32 LocalUserNum, const FUniqueNetId &UserId) override;

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