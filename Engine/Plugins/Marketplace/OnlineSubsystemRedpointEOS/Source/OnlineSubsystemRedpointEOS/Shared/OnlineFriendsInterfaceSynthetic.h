// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSDefines.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSRuntimePlatform.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineFriendsInterfaceEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineUserEOS.h"
#include "RedpointEOSInterfaces/Private/Interfaces/OnlineAvatarInterface.h"
#include <functional>

EOS_ENABLE_STRICT_WARNINGS

struct FFriendInfo
{
public:
    bool HasExternalInfo;
    bool IsCachedFromFriendDatabase;
    FExternalAccountIdInfo ExternalInfo;
    FName SubsystemName;
    TSharedPtr<FOnlineFriend> FriendRef;
    int32 SubsystemIndex;
    FString FetchedAvatarUrl;
};

typedef TSharedPtr<FFriendInfo> FUnresolvedFriendEntry;
typedef TTuple<EOS_ProductUserId, TSharedPtr<FFriendInfo>> TResolvedFriendEntry;
typedef TTuple<EOS_EExternalAccountType, TArray<FUnresolvedFriendEntry>> FUnresolvedFriendEntries;
typedef TTuple<EOS_EExternalAccountType, TArray<TResolvedFriendEntry>> TResolvedFriendEntries;

namespace OnlineRedpointEOS
{
namespace Friends
{
/**
 * The number of minutes before trying to send a P2P friend message to a target user again.
 */
constexpr int RetryMinutes = 10;
}; // namespace Friends
}; // namespace OnlineRedpointEOS

class ONLINESUBSYSTEMREDPOINTEOS_API FOnlineFriendsInterfaceSynthetic
    : public IOnlineFriends,
      public TSharedFromThis<FOnlineFriendsInterfaceSynthetic, ESPMode::ThreadSafe>
{
private:
    EOS_HPlatform EOSPlatform;
    EOS_HConnect EOSConnect;

    struct FWrappedFriendsInterface
    {
        FName SubsystemName;
        TWeakPtr<IOnlineIdentity, ESPMode::ThreadSafe> OnlineIdentity;
        TWeakPtr<IOnlineFriends, ESPMode::ThreadSafe> OnlineFriends;
        TWeakPtr<IOnlineAvatar, ESPMode::ThreadSafe> OnlineAvatar;
    };

    TSharedPtr<class IEOSRuntimePlatform> RuntimePlatform;
    TSharedRef<class FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> Identity;
    TSharedRef<class FOnlineUserCloudInterfaceEOS, ESPMode::ThreadSafe> UserCloud;
    TSharedRef<class FEOSMessagingHub> MessagingHub;
    TSharedPtr<class FEOSUserFactory, ESPMode::ThreadSafe> UserFactory;
    TArray<FWrappedFriendsInterface> OnlineFriends;
    TArray<TTuple<TWeakPtr<IOnlineFriends, ESPMode::ThreadSafe>, int, FDelegateHandle>> OnFriendsChangeDelegates;
    bool bInitialized;
    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
    IOnlineSubsystemPtr SubsystemEAS;
    TSharedRef<class FEOSConfig> Config;
    FName InstanceName;
    FUTickerDelegateHandle TickHandle;
    FDelegateHandle MessagingHubHandle;
    TMap<int32, bool> PendingReadFriendsListFromPropagation;

    struct FLocalUserFriendState
    {
    public:
        TSharedRef<class FFriendDatabase> FriendDatabase;
        TUserIdMap<TSharedPtr<class FOnlineFriendSynthetic>> FriendCacheById;
        TArray<TSharedRef<FOnlineFriend>> FriendCacheByIndex;
        TArray<TSharedRef<FOnlineRecentPlayer>> RecentPlayerCache;
        bool bDidQueryRecentPlayers;
        TArray<TSharedRef<FOnlineBlockedPlayer>> BlockedPlayersCache;
        bool bDidQueryBlockedPlayers;

        FLocalUserFriendState(
            const TSharedRef<const FUniqueNetIdEOS> &InLocalUserId,
            const TSharedRef<class FOnlineUserCloudInterfaceEOS, ESPMode::ThreadSafe> &InUserCloudImpl);
        UE_NONCOPYABLE(FLocalUserFriendState);
        ~FLocalUserFriendState() = default;
    };
    TMap<int32, TSharedPtr<FLocalUserFriendState>> FriendState;

    FString MutateListName(const FWrappedFriendsInterface &Friends, FString ListName);

    bool Op_ReadFriendsList(
        int32 LocalUserNum,
        const FString &ListName,
        FName InSubsystemName,
        const TWeakPtr<IOnlineIdentity, ESPMode::ThreadSafe> &InIdentity,
        const TWeakPtr<IOnlineFriends, ESPMode::ThreadSafe> &InFriends,
        const std::function<void(bool OutValue)> &OnDone);
    void Dn_ReadFriendsList(
        int32 LocalUserNum,
        const FString& ListName,
        const FOnReadFriendsListComplete &Delegate,
        const TArray<bool> &OutValues);
    bool Op_GetFriendsList(
        int32 LocalUserNum,
        const FString &ListName,
        const TWeakPtr<IOnlineIdentity, ESPMode::ThreadSafe> &InIdentity,
        const TWeakPtr<IOnlineFriends, ESPMode::ThreadSafe> &InFriends,
        const std::function<void(TArray<TSharedRef<FOnlineFriend>> OutValue)> &OnDone);
    void Dn_GetFriendsList(
        int32 LocalUserNum,
        const FString& ListName,
        const FOnReadFriendsListComplete &Delegate,
        TArray<TArray<TSharedRef<FOnlineFriend>>> OutValues);
    bool Op_ResolveAvatarForFriends(
        int32 LocalUserNum,
        const TSharedPtr<FFriendInfo> &Friend,
        const std::function<void(bool)> &OnDone);
    void Dn_ResolveAvatarForFriends(
        int32 LocalUserNum,
        const FString &ListName,
        const FOnReadFriendsListComplete &Delegate,
        const TArray<TArray<TSharedRef<FOnlineFriend>>> &OutValues,
        TArray<TSharedPtr<FFriendInfo>> FriendInfos,
        const TUserIdMap<TSharedPtr<FFriendInfo>> &UserIdToFriendInfo);
    void Dn_FriendDatabaseFlushed(
        bool bWasSuccessful,
        int32 LocalUserNum,
        FString ListName,
        FOnReadFriendsListComplete Delegate,
        TArray<TArray<TSharedRef<FOnlineFriend>>> OutValues,
        TArray<TSharedPtr<FFriendInfo>> FriendInfos);
    bool Op_ResolveFriendsListByType(
        int32 LocalUserNum,
        const FUnresolvedFriendEntries &EntriesByType,
        const std::function<void(TResolvedFriendEntries)> &OnDone);
    void Dn_ResolveFriendsListByType(
        int32 LocalUserNum,
        const FString& ListName,
        const FOnReadFriendsListComplete &Delegate,
        const TArray<TResolvedFriendEntries> &Results);
    void Dn_ResolveEOSAccount(
        int32 LocalUserNum,
        const FString &ListName,
        const FOnReadFriendsListComplete &Delegate,
        TMap<EOS_ProductUserId, TArray<TResolvedFriendEntry>> FriendRefsByExternalId,
        const TArray<TSharedPtr<FOnlineUserEOS>> &ResolvedEOSAccounts,
        const TArray<TSharedRef<const FUniqueNetIdEOS>> &FriendsFromDb);

    void InitIfNeeded();

    void OnMessageReceived(
        const FUniqueNetIdEOS &SenderId,
        const FUniqueNetIdEOS &ReceiverId,
        const FString &MessageType,
        const FString &MessageData);
    bool Tick(float DeltaSeconds);

    void RunReadFriendsListForPropagation(int32 LocalUserNum);

public:
    FOnlineFriendsInterfaceSynthetic(
        FName InInstanceName,
        EOS_HPlatform InPlatform,
        const TSharedRef<class FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> &InIdentity,
        const TSharedRef<class FOnlineUserCloudInterfaceEOS, ESPMode::ThreadSafe> &InUserCloudImpl,
        const TSharedRef<class FEOSMessagingHub> &InMessagingHub,
        const IOnlineSubsystemPtr &InSubsystemEAS,
        const TSharedRef<class IEOSRuntimePlatform> &InRuntimePlatform,
        const TSharedRef<class FEOSConfig> &InConfig,
        const TSharedRef<class FEOSUserFactory, ESPMode::ThreadSafe> &InUserFactory);
    UE_NONCOPYABLE(FOnlineFriendsInterfaceSynthetic);
    ~FOnlineFriendsInterfaceSynthetic();
    void RegisterEvents();

    /** Resolves the native ID to an EOS ID if possible, otherwise returns nullptr. */
    TSharedPtr<const FUniqueNetIdEOS> TryResolveNativeIdToEOS(const FUniqueNetId &UserId) const;

    virtual bool ReadFriendsList(
        int32 LocalUserNum,
        const FString &ListName,
        const FOnReadFriendsListComplete &Delegate = FOnReadFriendsListComplete()) override;
    virtual bool DeleteFriendsList(
        int32 LocalUserNum,
        const FString &ListName,
        const FOnDeleteFriendsListComplete &Delegate = FOnDeleteFriendsListComplete()) override;

    virtual bool SendInvite(
        int32 LocalUserNum,
        const FUniqueNetId &FriendId,
        const FString &ListName,
        const FOnSendInviteComplete &Delegate = FOnSendInviteComplete()) override;
    virtual bool AcceptInvite(
        int32 LocalUserNum,
        const FUniqueNetId &FriendId,
        const FString &ListName,
        const FOnAcceptInviteComplete &Delegate = FOnAcceptInviteComplete()) override;
    virtual bool RejectInvite(int32 LocalUserNum, const FUniqueNetId &FriendId, const FString &ListName) override;

    virtual void SetFriendAlias(
        int32 LocalUserNum,
        const FUniqueNetId &FriendId,
        const FString &ListName,
        const FString &Alias,
        const FOnSetFriendAliasComplete &Delegate = FOnSetFriendAliasComplete()) override;
    virtual void DeleteFriendAlias(
        int32 LocalUserNum,
        const FUniqueNetId &FriendId,
        const FString &ListName,
        const FOnDeleteFriendAliasComplete &Delegate = FOnDeleteFriendAliasComplete()) override;
    virtual bool DeleteFriend(int32 LocalUserNum, const FUniqueNetId &FriendId, const FString &ListName) override;

    virtual bool GetFriendsList(
        int32 LocalUserNum,
        const FString &ListName,
        TArray<TSharedRef<FOnlineFriend>> &OutFriends) override;
    virtual TSharedPtr<FOnlineFriend> GetFriend(
        int32 LocalUserNum,
        const FUniqueNetId &FriendId,
        const FString &ListName) override;
    virtual bool IsFriend(int32 LocalUserNum, const FUniqueNetId &FriendId, const FString &ListName) override;

    virtual void AddRecentPlayers(
        const FUniqueNetId &UserId,
        const TArray<FReportPlayedWithUser> &InRecentPlayers,
        const FString &ListName,
        const FOnAddRecentPlayersComplete &InCompletionDelegate) override;
    virtual bool QueryRecentPlayers(const FUniqueNetId &UserId, const FString &Namespace) override;
    virtual bool GetRecentPlayers(
        const FUniqueNetId &UserId,
        const FString &Namespace,
        TArray<TSharedRef<FOnlineRecentPlayer>> &OutRecentPlayers) override;
    virtual void DumpRecentPlayers() const override;

    virtual bool BlockPlayer(int32 LocalUserNum, const FUniqueNetId &PlayerId) override;
    virtual bool UnblockPlayer(int32 LocalUserNum, const FUniqueNetId &PlayerId) override;
    virtual bool QueryBlockedPlayers(const FUniqueNetId &UserId) override;
    virtual bool GetBlockedPlayers(
        const FUniqueNetId &UserId,
        TArray<TSharedRef<FOnlineBlockedPlayer>> &OutBlockedPlayers) override;
    virtual void DumpBlockedPlayers() const override;

    virtual void QueryFriendSettings(const FUniqueNetId &LocalUserId, FOnSettingsOperationComplete Delegate) override;
#if !defined(UE_5_0_OR_LATER)
    virtual void UpdateFriendSettings(
        const FUniqueNetId &LocalUserId,
        const FFriendSettings &NewSettings,
        FOnSettingsOperationComplete Delegate) override;
#endif // #if !defined(UE_5_0_OR_LATER)
    virtual bool QueryFriendSettings(
        const FUniqueNetId &UserId,
        const FString &Source,
        const FOnQueryFriendSettingsComplete &Delegate = FOnQueryFriendSettingsComplete()) override;
    virtual bool GetFriendSettings(
        const FUniqueNetId &UserId,
        TMap<FString, TSharedRef<FOnlineFriendSettingsSourceData>> &OutSettings) override;
    virtual bool SetFriendSettings(
        const FUniqueNetId &UserId,
        const FString &Source,
        bool bNeverShowAgain,
        const FOnSetFriendSettingsComplete &Delegate = FOnSetFriendSettingsComplete()) override;
};

EOS_DISABLE_STRICT_WARNINGS