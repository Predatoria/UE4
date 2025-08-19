// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/OnlineUserInterfaceEOS.h"

#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineSubsystemRedpointEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/MultiOperation.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineIdentityInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineUserEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

FOnlineUserInterfaceEOS::FOnlineUserInterfaceEOS(
    EOS_HPlatform InPlatform,
    TSharedPtr<class FOnlineSubsystemRedpointEAS, ESPMode::ThreadSafe> InSubsystemEAS,
    TSharedPtr<FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> InIdentity,
    const TSharedRef<class FEOSUserFactory, ESPMode::ThreadSafe> &InUserFactory)
    : EOSPlatform(InPlatform)
    , EOSUserInfo(EOS_Platform_GetUserInfoInterface(InPlatform))
    , EOSConnect(EOS_Platform_GetConnectInterface(InPlatform))
    , SubsystemEAS(MoveTemp(InSubsystemEAS))
    , Identity(MoveTemp(InIdentity))
    , UserFactory(InUserFactory)
    , CachedUsers()
    , CachedMappings()
{
}

FOnlineUserInterfaceEOS::~FOnlineUserInterfaceEOS()
{
}

bool FOnlineUserInterfaceEOS::QueryUserInfo(int32 LocalUserNum, const TArray<TSharedRef<const FUniqueNetId>> &UserIds)
{
    if (this->Identity->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
    {
        UE_LOG(LogEOS, Error, TEXT("QueryUserInfo: Local user is not logged in"));
        return false;
    }

    auto QueryingUserId = this->Identity->GetUniquePlayerId(LocalUserNum);
    if (QueryingUserId->GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("QueryUserInfo: Local user is not using the Epic subsystem"));
        return false;
    }

    TSharedRef<const FUniqueNetIdEOS> QueryingUserIdEOS =
        StaticCastSharedRef<const FUniqueNetIdEOS>(QueryingUserId.ToSharedRef());
    TArray<TSharedRef<const FUniqueNetIdEOS>> UserIdsEOS;
    for (const auto &UserId : UserIds)
    {
        if (UserId->GetType() == REDPOINT_EOS_SUBSYSTEM)
        {
            UserIdsEOS.Add(StaticCastSharedRef<const FUniqueNetIdEOS>(UserId));
        }
    }

    FOnlineUserEOS::Get(
        this->UserFactory.ToSharedRef(),
        QueryingUserIdEOS,
        UserIdsEOS,
        [WeakThis = GetWeakThis(this), LocalUserNum](const TUserIdMap<TSharedPtr<FOnlineUserEOS>> &Users) {
            if (auto This = PinWeakThis(WeakThis))
            {
                TArray<TSharedRef<const FUniqueNetId>> CachedUserIds;
                for (const auto &KV : Users)
                {
                    if (!KV.Value.IsValid() || !KV.Value->IsValid())
                    {
                        continue;
                    }

                    if (!This->CachedUsers.Contains(LocalUserNum))
                    {
                        This->CachedUsers.Add(LocalUserNum, TUserIdMap<TSharedPtr<FOnlineUser>>());
                    }
                    This->CachedUsers[LocalUserNum].Add(*KV.Key, KV.Value);
                    CachedUserIds.Add(KV.Key);
                }

                This->TriggerOnQueryUserInfoCompleteDelegates(LocalUserNum, true, CachedUserIds, TEXT(""));
            }
        });
    return true;
}

bool FOnlineUserInterfaceEOS::GetAllUserInfo(int32 LocalUserNum, TArray<TSharedRef<FOnlineUser>> &OutUsers)
{
    if (!this->CachedUsers.Contains(LocalUserNum))
    {
        UE_LOG(LogEOS, Error, TEXT("GetAllUserInfo: No users have been cached for this user"));
        return false;
    }
    for (const auto &KV : this->CachedUsers[LocalUserNum])
    {
        OutUsers.Add(KV.Value.ToSharedRef());
    }
    return true;
}

TSharedPtr<FOnlineUser> FOnlineUserInterfaceEOS::GetUserInfo(int32 LocalUserNum, const FUniqueNetId &UserId)
{
    if (!this->CachedUsers.Contains(LocalUserNum))
    {
        return nullptr;
    }

    if (UserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        return nullptr;
    }

    if (!this->CachedUsers[LocalUserNum].Contains(UserId))
    {
        return nullptr;
    }

    return this->CachedUsers[LocalUserNum][UserId];
}

bool FOnlineUserInterfaceEOS::QueryUserIdMapping(
    const FUniqueNetId &UserId,
    const FString &DisplayNameOrEmail,
    const FOnQueryUserMappingComplete &Delegate)
{
#if EOS_HAS_AUTHENTICATION
    if (!this->SubsystemEAS.IsValid())
    {
        UE_LOG(LogEOS, Error, TEXT("QueryUserIdMapping: This function can not be used on a dedicated server"));
        return false;
    }

    if (UserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("Expected local user ID to be an EOS user."));
        return false;
    }

    int32 LocalUserNum;
    if (this->Identity->GetLocalUserNum(UserId, LocalUserNum))
    {
        TSharedPtr<const FUniqueNetId> UserIdEAS =
            this->SubsystemEAS->GetIdentityInterface()->GetUniquePlayerId(LocalUserNum);
        if (UserIdEAS.IsValid())
        {
            return this->SubsystemEAS->GetUserInterface()->QueryUserIdMapping(
                *UserIdEAS,
                DisplayNameOrEmail,
                FOnQueryUserMappingComplete::CreateLambda(
                    [WeakThis = GetWeakThis(this),
                     Delegate,
                     UserIdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared())](
                        bool bWasSuccessful,
                        const FUniqueNetId &UserId,
                        const FString &DisplayNameOrEmail,
                        const FUniqueNetId &FoundUserId,
                        const FString &Error) {
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            if (!bWasSuccessful)
                            {
                                Delegate.ExecuteIfBound(
                                    bWasSuccessful,
                                    *UserIdEOS,
                                    DisplayNameOrEmail,
                                    *FUniqueNetIdEOS::InvalidId(),
                                    Error);
                                return;
                            }

                            EOS_EpicAccountId TargetUserId =
                                StaticCastSharedRef<const FUniqueNetIdEAS>(FoundUserId.AsShared())->GetEpicAccountId();

                            // Now we must resolve the EOS_EpicAccountId into an EOS_ProductUserId, or the
                            // FUniqueNetIdEOS can't be valid.
                            EOS_Connect_QueryExternalAccountMappingsOptions LookupOpts = {};
                            LookupOpts.ApiVersion = EOS_CONNECT_QUERYEXTERNALACCOUNTMAPPINGS_API_LATEST;
                            LookupOpts.AccountIdType = EOS_EExternalAccountType::EOS_EAT_EPIC;
                            LookupOpts.LocalUserId = UserIdEOS->GetProductUserId();
                            TArray<EOS_EpicAccountId> TargetIds;
                            TargetIds.Add(TargetUserId);
                            EOSString_EpicAccountId::AllocateToCharList(
                                TargetIds,
                                LookupOpts.ExternalAccountIdCount,
                                LookupOpts.ExternalAccountIds);

                            EOSRunOperation<
                                EOS_HConnect,
                                EOS_Connect_QueryExternalAccountMappingsOptions,
                                EOS_Connect_QueryExternalAccountMappingsCallbackInfo>(
                                This->EOSConnect,
                                &LookupOpts,
                                EOS_Connect_QueryExternalAccountMappings,
                                [WeakThis = GetWeakThis(This),
                                 Delegate,
                                 UserIdEOS,
                                 DisplayNameOrEmail,
                                 LookupOpts,
                                 TargetUserId](const EOS_Connect_QueryExternalAccountMappingsCallbackInfo *Data) {
                                    EOSString_EpicAccountId::FreeFromCharListConst(
                                        LookupOpts.ExternalAccountIdCount,
                                        LookupOpts.ExternalAccountIds);

                                    if (auto This = PinWeakThis(WeakThis))
                                    {
                                        if (Data->ResultCode != EOS_EResult::EOS_Success)
                                        {
                                            Delegate.ExecuteIfBound(
                                                false,
                                                *UserIdEOS,
                                                DisplayNameOrEmail,
                                                *FUniqueNetIdEOS::InvalidId(),
                                                FString::Printf(
                                                    TEXT("EOS_Connect_QueryExternalAccountMappings failed: %s"),
                                                    ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode))));
                                            return;
                                        }

                                        EOSString_EpicAccountId::AnsiString AnsiBuffer =
                                            EOSString_EpicAccountId::ToAnsiString(TargetUserId);
                                        if (AnsiBuffer.Result != EOS_EResult::EOS_Success)
                                        {
                                            Delegate.ExecuteIfBound(
                                                false,
                                                *UserIdEOS,
                                                DisplayNameOrEmail,
                                                *FUniqueNetIdEOS::InvalidId(),
                                                TEXT("EOS_Connect_GetExternalAccountMapping failed: Could not "
                                                     "stringify the Epic "
                                                     "account ID"));
                                            return;
                                        }

                                        EOS_Connect_GetExternalAccountMappingsOptions ExternalOpts = {};
                                        ExternalOpts.ApiVersion = EOS_CONNECT_GETEXTERNALACCOUNTMAPPINGS_API_LATEST;
                                        ExternalOpts.AccountIdType = EOS_EExternalAccountType::EOS_EAT_EPIC;
                                        ExternalOpts.LocalUserId = UserIdEOS->GetProductUserId();
                                        ExternalOpts.TargetExternalUserId = AnsiBuffer.Ptr.Get();

                                        auto ProductId =
                                            EOS_Connect_GetExternalAccountMapping(This->EOSConnect, &ExternalOpts);
                                        if (!EOSString_ProductUserId::IsValid(ProductId))
                                        {
                                            Delegate.ExecuteIfBound(
                                                false,
                                                *UserIdEOS,
                                                DisplayNameOrEmail,
                                                *FUniqueNetIdEOS::InvalidId(),
                                                TEXT("EOS_Connect_GetExternalAccountMapping failed: Resolved product "
                                                     "user ID is "
                                                     "not valid"));
                                            return;
                                        }

                                        Delegate.ExecuteIfBound(
                                            true,
                                            *UserIdEOS,
                                            DisplayNameOrEmail,
                                            *MakeShared<FUniqueNetIdEOS>(ProductId),
                                            TEXT(""));
                                    }
                                });
                        }
                    }));
        }
    }
    UE_LOG(LogEOS, Error, TEXT("QueryUserIdMapping: Local user is not signed into an Epic Games account"));
    return false;
#else
    UE_LOG(LogEOS, Error, TEXT("QueryUserIdMapping: This function can not be used on a dedicated server"));
    return false;
#endif // #if EOS_HAS_AUTHENTICATION
}

EOS_EExternalAccountType GetExternalAccountType(const FString &InType)
{
    if (InType.ToUpper() == TEXT("EPIC"))
    {
        return EOS_EExternalAccountType::EOS_EAT_EPIC;
    }
    else if (InType.ToUpper() == TEXT("STEAM"))
    {
        return EOS_EExternalAccountType::EOS_EAT_STEAM;
    }
    else if (InType.ToUpper() == TEXT("PSN"))
    {
        return EOS_EExternalAccountType::EOS_EAT_PSN;
    }
    else if (InType.ToUpper() == TEXT("XBL"))
    {
        return EOS_EExternalAccountType::EOS_EAT_XBL;
    }
    else if (InType.ToUpper() == TEXT("DISCORD"))
    {
        return EOS_EExternalAccountType::EOS_EAT_DISCORD;
    }
    else if (InType.ToUpper() == TEXT("GOG"))
    {
        return EOS_EExternalAccountType::EOS_EAT_GOG;
    }
    else if (InType.ToUpper() == TEXT("NINTENDO"))
    {
        return EOS_EExternalAccountType::EOS_EAT_NINTENDO;
    }
    else if (InType.ToUpper() == TEXT("UPLAY"))
    {
        return EOS_EExternalAccountType::EOS_EAT_UPLAY;
    }
    else if (InType.ToUpper() == TEXT("OPENID"))
    {
        return EOS_EExternalAccountType::EOS_EAT_OPENID;
    }
    else if (InType.ToUpper() == TEXT("APPLE"))
    {
        return EOS_EExternalAccountType::EOS_EAT_APPLE;
    }

    return (EOS_EExternalAccountType)-1;
}

bool FOnlineUserInterfaceEOS::QueryExternalIdMappings(
    const FUniqueNetId &QueryingUserId,
    const FExternalIdQueryOptions &QueryOptions,
    const TArray<FString> &ExternalIds,
    const FOnQueryExternalIdMappingsComplete &Delegate)
{
    if (QueryingUserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        Delegate.ExecuteIfBound(
            false,
            QueryingUserId,
            QueryOptions,
            ExternalIds,
            TEXT("Querying user ID is not an EOS user ID"));
        return true;
    }

    if (QueryOptions.bLookupByDisplayName)
    {
        Delegate.ExecuteIfBound(
            false,
            QueryingUserId,
            QueryOptions,
            ExternalIds,
            TEXT("bLookupByDisplayName must be false"));
        return true;
    }

    auto QueryingUserIdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(QueryingUserId.AsShared());
    auto QueryType = GetExternalAccountType(QueryOptions.AuthType);
    if (QueryType == (EOS_EExternalAccountType)-1)
    {
        Delegate.ExecuteIfBound(
            false,
            QueryingUserId,
            QueryOptions,
            ExternalIds,
            TEXT("AuthType in QueryOptions is invalid"));
        return true;
    }

    EOS_Connect_QueryExternalAccountMappingsOptions LookupOpts = {};
    LookupOpts.ApiVersion = EOS_CONNECT_QUERYEXTERNALACCOUNTMAPPINGS_API_LATEST;
    LookupOpts.AccountIdType = QueryType;
    LookupOpts.LocalUserId = QueryingUserIdEOS->GetProductUserId();
    EOSString_Connect_ExternalAccountId::AllocateToCharList(
        ExternalIds,
        LookupOpts.ExternalAccountIdCount,
        LookupOpts.ExternalAccountIds);

    EOSRunOperation<
        EOS_HConnect,
        EOS_Connect_QueryExternalAccountMappingsOptions,
        EOS_Connect_QueryExternalAccountMappingsCallbackInfo>(
        this->EOSConnect,
        &LookupOpts,
        EOS_Connect_QueryExternalAccountMappings,
        [WeakThis = GetWeakThis(this), LookupOpts, Delegate, QueryingUserIdEOS, QueryOptions, ExternalIds, QueryType](
            const EOS_Connect_QueryExternalAccountMappingsCallbackInfo *Data) {
            EOSString_Connect_ExternalAccountId::FreeFromCharListConst(
                LookupOpts.ExternalAccountIdCount,
                LookupOpts.ExternalAccountIds);

            if (auto This = PinWeakThis(WeakThis))
            {
                if (Data->ResultCode != EOS_EResult::EOS_Success)
                {
                    Delegate.ExecuteIfBound(
                        false,
                        *QueryingUserIdEOS,
                        QueryOptions,
                        ExternalIds,
                        FString::Printf(
                            TEXT("EOS_Connect_QueryExternalAccountMappings options: %s"),
                            ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode))));
                    return;
                }

                auto MappingKey = QueryOptions.AuthType.ToUpper();
                if (!This->CachedMappings.Contains(MappingKey))
                {
                    This->CachedMappings.Add(MappingKey, TMap<FString, TSharedPtr<const FUniqueNetId>>());
                }

                for (const FString &ExternalId : ExternalIds)
                {
                    auto ExternalIdStr = EOSString_Connect_ExternalAccountId::ToAnsiString(ExternalId);

                    EOS_Connect_GetExternalAccountMappingsOptions Opts = {};
                    Opts.ApiVersion = EOS_CONNECT_GETEXTERNALACCOUNTMAPPINGS_API_LATEST;
                    Opts.AccountIdType = QueryType;
                    Opts.LocalUserId = QueryingUserIdEOS->GetProductUserId();
                    Opts.TargetExternalUserId = ExternalIdStr.Ptr.Get();

                    EOS_ProductUserId ResultId = EOS_Connect_GetExternalAccountMapping(This->EOSConnect, &Opts);
                    if (EOSString_ProductUserId::IsValid(ResultId))
                    {
                        This->CachedMappings[MappingKey].Add(ExternalId, MakeShared<const FUniqueNetIdEOS>(ResultId));
                    }
                }

                Delegate.ExecuteIfBound(true, *QueryingUserIdEOS, QueryOptions, ExternalIds, TEXT(""));
            }
        });
    return true;
}

void FOnlineUserInterfaceEOS::GetExternalIdMappings(
    const FExternalIdQueryOptions &QueryOptions,
    const TArray<FString> &ExternalIds,
    TArray<TSharedPtr<const FUniqueNetId>> &OutIds)
{
    if (QueryOptions.bLookupByDisplayName)
    {
        UE_LOG(LogEOS, Error, TEXT("bLookupByDisplayName must be false"));
        return;
    }

    auto MappingKey = QueryOptions.AuthType.ToUpper();
    if (!this->CachedMappings.Contains(MappingKey))
    {
        UE_LOG(LogEOS, Error, TEXT("QueryExternalIdMappings has not been called"));
        return;
    }

    OutIds = TArray<TSharedPtr<const FUniqueNetId>>();
    for (const auto &ExternalId : ExternalIds)
    {
        if (this->CachedMappings[MappingKey].Contains(ExternalId))
        {
            OutIds.Add(this->CachedMappings[MappingKey][ExternalId]);
        }
        else
        {
            OutIds.Add(nullptr);
        }
    }
}

TSharedPtr<const FUniqueNetId> FOnlineUserInterfaceEOS::GetExternalIdMapping(
    const FExternalIdQueryOptions &QueryOptions,
    const FString &ExternalId)
{
    if (QueryOptions.bLookupByDisplayName)
    {
        UE_LOG(LogEOS, Error, TEXT("bLookupByDisplayName must be false"));
        return nullptr;
    }

    auto MappingKey = QueryOptions.AuthType.ToUpper();
    if (!this->CachedMappings.Contains(MappingKey))
    {
        UE_LOG(LogEOS, Error, TEXT("QueryExternalIdMappings has not been called"));
        return nullptr;
    }

    if (this->CachedMappings[MappingKey].Contains(ExternalId))
    {
        return this->CachedMappings[MappingKey][ExternalId];
    }

    return nullptr;
}

EOS_DISABLE_STRICT_WARNINGS