// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineUserInterfaceEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/UniqueNetIdEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

FOnlineUserInterfaceEAS::FOnlineUserInterfaceEAS(
    EOS_HPlatform InPlatform,
    TSharedRef<const FOnlineIdentityInterfaceEAS, ESPMode::ThreadSafe> InIdentity)
    : EOSUserInfo(EOS_Platform_GetUserInfoInterface(InPlatform))
    , Identity(MoveTemp(InIdentity))
{
}

bool FOnlineUserInterfaceEAS::QueryUserInfo(int32 LocalUserNum, const TArray<TSharedRef<const FUniqueNetId>> &UserIds)
{
    return false;
}

bool FOnlineUserInterfaceEAS::GetAllUserInfo(int32 LocalUserNum, TArray<TSharedRef<class FOnlineUser>> &OutUsers)
{
    return false;
}

TSharedPtr<FOnlineUser> FOnlineUserInterfaceEAS::GetUserInfo(int32 LocalUserNum, const class FUniqueNetId &UserId)
{
    return nullptr;
}

bool FOnlineUserInterfaceEAS::QueryUserIdMapping(
    const FUniqueNetId &UserId,
    const FString &DisplayNameOrEmail,
    const FOnQueryUserMappingComplete &Delegate)
{
    if (UserId.GetType() != REDPOINT_EAS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("QueryUserIdMapping: Querying user is not logged an Epic account"));
        return false;
    }

    auto QueryingUserId = StaticCastSharedRef<const FUniqueNetIdEAS>(UserId.AsShared());
    EOS_EpicAccountId EpicAccountId = QueryingUserId->GetEpicAccountId();
    if (EOSString_EpicAccountId::IsNone(EpicAccountId))
    {
        UE_LOG(LogEOS, Error, TEXT("QueryUserIdMapping: Querying user is not logged into an Epic Games account"));
        return false;
    }

    auto DisplayNameStr = EOSString_QueryUserInfoByDisplayName_DisplayName::ToUtf8String(DisplayNameOrEmail);

    EOS_UserInfo_QueryUserInfoByDisplayNameOptions Opts = {};
    Opts.ApiVersion = EOS_USERINFO_QUERYUSERINFOBYDISPLAYNAME_API_LATEST;
    Opts.LocalUserId = EpicAccountId;
    Opts.DisplayName = DisplayNameStr.GetAsChar();

    EOSRunOperation<
        EOS_HUserInfo,
        EOS_UserInfo_QueryUserInfoByDisplayNameOptions,
        EOS_UserInfo_QueryUserInfoByDisplayNameCallbackInfo>(
        this->EOSUserInfo,
        &Opts,
        EOS_UserInfo_QueryUserInfoByDisplayName,
        [WeakThis = GetWeakThis(this), Delegate, QueryingUserId, DisplayNameOrEmail](
            const EOS_UserInfo_QueryUserInfoByDisplayNameCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Data->ResultCode != EOS_EResult::EOS_Success)
                {
                    Delegate.ExecuteIfBound(
                        false,
                        *QueryingUserId,
                        DisplayNameOrEmail,
                        *FUniqueNetIdEAS::InvalidId(),
                        FString::Printf(
                            TEXT("EOS_UserInfo_QueryUserInfoByDisplayName failed: %s"),
                            ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode))));
                    return;
                }

                Delegate.ExecuteIfBound(
                    true,
                    *QueryingUserId,
                    DisplayNameOrEmail,
                    *MakeShared<FUniqueNetIdEAS>(Data->TargetUserId),
                    TEXT(""));
            }
        });
    return true;
}

bool FOnlineUserInterfaceEAS::QueryExternalIdMappings(
    const FUniqueNetId &UserId,
    const FExternalIdQueryOptions &QueryOptions,
    const TArray<FString> &ExternalIds,
    const FOnQueryExternalIdMappingsComplete &Delegate)
{
    return false;
}

void FOnlineUserInterfaceEAS::GetExternalIdMappings(
    const FExternalIdQueryOptions &QueryOptions,
    const TArray<FString> &ExternalIds,
    TArray<TSharedPtr<const FUniqueNetId>> &OutIds)
{
}

TSharedPtr<const FUniqueNetId> FOnlineUserInterfaceEAS::GetExternalIdMapping(
    const FExternalIdQueryOptions &QueryOptions,
    const FString &ExternalId)
{
    return nullptr;
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION