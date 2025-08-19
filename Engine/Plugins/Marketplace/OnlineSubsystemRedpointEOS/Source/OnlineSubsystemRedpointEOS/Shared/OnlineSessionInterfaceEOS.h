// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPAddress.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineIdentityInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FOnlineSessionInterfaceEOS
    : public IOnlineSession,
      public TSharedFromThis<FOnlineSessionInterfaceEOS, ESPMode::ThreadSafe>
{
    friend class FOnlineSubsystemEOS;
    friend class FCleanShutdown;

private:
    EOS_HSessions EOSSessions;
    EOS_HMetrics EOSMetrics;
    EOS_HUI EOSUI;

    TSharedPtr<FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> Identity;
#if EOS_HAS_AUTHENTICATION
    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
    TSharedPtr<IOnlineFriends, ESPMode::ThreadSafe> Friends;
    TWeakPtr<class FSyntheticPartySessionManager> SyntheticPartySessionManager;
#endif // #if EOS_HAS_AUTHENTICATION
#if EOS_HAS_ORCHESTRATORS
    TSharedPtr<class IServerOrchestrator> ServerOrchestrator;
#endif // #if EOS_HAS_ORCHESTRATORS
    TSharedPtr<class FEOSConfig> Config;

    /** Critical sections for thread safe operation of session lists */
    mutable FCriticalSection SessionLock;

    /** Current session settings */
    TArray<FNamedOnlineSession> Sessions;

    FString GetBucketId(const FOnlineSessionSettings &SessionSettings);
    EOS_EResult ApplyConnectionSettingsToModificationHandle(
        const TSharedRef<const FInternetAddr> &InternetAddr,
        const TArray<TSharedPtr<FInternetAddr>> &DeveloperInternetAddrs,
        EOS_HSessionModification Handle,
        const FOnlineSessionSettings &SessionSettings,
        bool &bIsPeerToPeerAddress);
    EOS_EResult ApplySettingsToModificationHandle(
        const TSharedPtr<const FUniqueNetIdEOS> &HostingUserId,
        const FOnlineSessionSettings &SessionSettings,
        EOS_HSessionModification Handle,
        const FOnlineSessionSettings *ExistingSessionSettings);

    TSharedRef<class FEOSListenTracker> ListenTracker;

    void MetricsSend_BeginPlayerSession(
        const FUniqueNetIdEOS &UserId,
        const FString &SessionId,
        const FString &GameServerAddress);
    void MetricsSend_EndPlayerSession(const FUniqueNetIdEOS &UserId);

    void Handle_SessionAddressChanged(
        EOS_ProductUserId ProductUserId,
        const TSharedRef<const FInternetAddr> &InternetAddr,
        const TArray<TSharedPtr<FInternetAddr>> &DeveloperInternetAddrs);
    void Handle_SessionAddressClosed(EOS_ProductUserId ProductUserId);

    void RegisterEvents();

    TSharedPtr<EOSEventHandle<EOS_Sessions_JoinSessionAcceptedCallbackInfo>> Unregister_JoinSessionAccepted;
    TSharedPtr<EOSEventHandle<EOS_Sessions_SessionInviteAcceptedCallbackInfo>> Unregister_SessionInviteAccepted;
    TSharedPtr<EOSEventHandle<EOS_Sessions_SessionInviteReceivedCallbackInfo>> Unregister_SessionInviteReceived;

    void Handle_JoinSessionAccepted(const EOS_Sessions_JoinSessionAcceptedCallbackInfo *Data);
    void Handle_SessionInviteAccepted(const EOS_Sessions_SessionInviteAcceptedCallbackInfo *Data);
    void Handle_SessionInviteReceived(const EOS_Sessions_SessionInviteReceivedCallbackInfo *Data);

#if EOS_HAS_AUTHENTICATION
    bool ShouldHaveSyntheticSession(const FOnlineSessionSettings &SessionSettings) const;
#endif // #if EOS_HAS_AUTHENTICATION

    bool CreateSessionInternal(
        const FUniqueNetId &HostingPlayerId,
        FName SessionName,
        const FOnlineSessionSettings &NewSessionSettings);
    bool UpdateSessionInternal(
        FName SessionName,
        FOnlineSessionSettings &UpdatedSessionSettings,
        bool bShouldRefreshOnlineData);

public:
    FOnlineSessionInterfaceEOS(
        EOS_HPlatform InPlatform,
        TSharedPtr<FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> InIdentity,
        const TSharedPtr<IOnlineFriends, ESPMode::ThreadSafe> &InFriends,
#if EOS_HAS_ORCHESTRATORS
        const TSharedPtr<class IServerOrchestrator> &InServerOrchestrator,
#endif // #if EOS_HAS_ORCHESTRATORS
        const TSharedRef<class FEOSConfig> &InConfig);
    UE_NONCOPYABLE(FOnlineSessionInterfaceEOS);
#if EOS_HAS_AUTHENTICATION
    void SetSyntheticPartySessionManager(
        const TSharedPtr<class FSyntheticPartySessionManager> &InSyntheticPartySessionManager)
    {
        this->SyntheticPartySessionManager = InSyntheticPartySessionManager;
    }
#endif // #if EOS_HAS_AUTHENTICATION

    void RegisterListeningAddress(
        EOS_ProductUserId InProductUserId,
        TSharedRef<const FInternetAddr> InInternetAddr,
        TArray<TSharedPtr<FInternetAddr>> InDeveloperInternetAddrs);
    void DeregisterListeningAddress(EOS_ProductUserId InProductUserId, TSharedRef<const FInternetAddr> InInternetAddr);

    virtual FNamedOnlineSession *AddNamedSession(FName SessionName, const FOnlineSessionSettings &SessionSettings)
        override;
    virtual FNamedOnlineSession *AddNamedSession(FName SessionName, const FOnlineSession &Session) override;

    virtual TSharedPtr<const FUniqueNetId> CreateSessionIdFromString(const FString &SessionIdStr) override;
    virtual FNamedOnlineSession *GetNamedSession(FName SessionName) override;
    virtual void RemoveNamedSession(FName SessionName) override;
    virtual bool HasPresenceSession() override;
    virtual EOnlineSessionState::Type GetSessionState(FName SessionName) const override;
    virtual bool CreateSession(
        int32 HostingPlayerNum,
        FName SessionName,
        const FOnlineSessionSettings &NewSessionSettings) override;
    virtual bool CreateSession(
        const FUniqueNetId &HostingPlayerId,
        FName SessionName,
        const FOnlineSessionSettings &NewSessionSettings) override;
    virtual bool StartSession(FName SessionName) override;
    virtual bool UpdateSession(
        FName SessionName,
        FOnlineSessionSettings &UpdatedSessionSettings,
        bool bShouldRefreshOnlineData = true) override;
    virtual bool EndSession(FName SessionName) override;
    virtual bool DestroySession(
        FName SessionName,
        const FOnDestroySessionCompleteDelegate &CompletionDelegate = FOnDestroySessionCompleteDelegate()) override;
    virtual bool IsPlayerInSession(FName SessionName, const FUniqueNetId &UniqueId) override;
    virtual bool StartMatchmaking(
        const TArray<TSharedRef<const FUniqueNetId>> &LocalPlayers,
        FName SessionName,
        const FOnlineSessionSettings &NewSessionSettings,
        TSharedRef<FOnlineSessionSearch> &SearchSettings) override;
    virtual bool CancelMatchmaking(int32 SearchingPlayerNum, FName SessionName) override;
    virtual bool CancelMatchmaking(const FUniqueNetId &SearchingPlayerId, FName SessionName) override;
    virtual bool FindSessions(int32 SearchingPlayerNum, const TSharedRef<FOnlineSessionSearch> &SearchSettings)
        override;
    virtual bool FindSessions(
        const FUniqueNetId &SearchingPlayerId,
        const TSharedRef<FOnlineSessionSearch> &SearchSettings) override;
    virtual bool FindSessionById(
        const FUniqueNetId &SearchingUserId,
        const FUniqueNetId &SessionId,
        const FUniqueNetId &FriendId,
        const FOnSingleSessionResultCompleteDelegate &CompletionDelegate) override;
    virtual bool CancelFindSessions() override;
    virtual bool PingSearchResults(const FOnlineSessionSearchResult &SearchResult) override;
    virtual bool JoinSession(int32 LocalUserNum, FName SessionName, const FOnlineSessionSearchResult &DesiredSession)
        override;
    virtual bool JoinSession(
        const FUniqueNetId &LocalUserId,
        FName SessionName,
        const FOnlineSessionSearchResult &DesiredSession) override;
    virtual bool JoinSession(
        const FUniqueNetId &LocalUserId,
        FName SessionName,
        const FOnlineSessionSearchResult &DesiredSession,
        const FOnJoinSessionCompleteDelegate &OnComplete);
    virtual bool FindFriendSession(int32 LocalUserNum, const FUniqueNetId &Friend) override;
    virtual bool FindFriendSession(const FUniqueNetId &LocalUserId, const FUniqueNetId &Friend) override;
    virtual bool FindFriendSession(
        const FUniqueNetId &LocalUserId,
        const TArray<TSharedRef<const FUniqueNetId>> &FriendList) override;
    virtual bool SendSessionInviteToFriend(int32 LocalUserNum, FName SessionName, const FUniqueNetId &Friend) override;
    virtual bool SendSessionInviteToFriend(
        const FUniqueNetId &LocalUserId,
        FName SessionName,
        const FUniqueNetId &Friend) override;
    virtual bool SendSessionInviteToFriends(
        int32 LocalUserNum,
        FName SessionName,
        const TArray<TSharedRef<const FUniqueNetId>> &Friends) override;
    virtual bool SendSessionInviteToFriends(
        const FUniqueNetId &LocalUserId,
        FName SessionName,
        const TArray<TSharedRef<const FUniqueNetId>> &Friends) override;
    virtual bool GetResolvedConnectString(FName SessionName, FString &ConnectInfo, FName PortType = NAME_GamePort)
        override;
    virtual bool GetResolvedConnectString(
        const class FOnlineSessionSearchResult &SearchResult,
        FName PortType,
        FString &ConnectInfo) override;
    virtual FOnlineSessionSettings *GetSessionSettings(FName SessionName) override;
    virtual bool RegisterPlayer(FName SessionName, const FUniqueNetId &PlayerId, bool bWasInvited) override;
    virtual bool RegisterPlayers(
        FName SessionName,
        const TArray<TSharedRef<const FUniqueNetId>> &Players,
        bool bWasInvited = false) override;
    virtual bool UnregisterPlayer(FName SessionName, const FUniqueNetId &PlayerId) override;
    virtual bool UnregisterPlayers(FName SessionName, const TArray<TSharedRef<const FUniqueNetId>> &Players) override;
    virtual void RegisterLocalPlayer(
        const FUniqueNetId &PlayerId,
        FName SessionName,
        const FOnRegisterLocalPlayerCompleteDelegate &Delegate) override;
    virtual void UnregisterLocalPlayer(
        const FUniqueNetId &PlayerId,
        FName SessionName,
        const FOnUnregisterLocalPlayerCompleteDelegate &Delegate) override;
    virtual int32 GetNumSessions() override;
    virtual void DumpSessionState() override;
};

EOS_DISABLE_STRICT_WARNINGS