// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if EOS_DISCORD_ENABLED

#include "DiscordGameSDK.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

/**
 * This isn't a real session implementation; it implements *just enough* of the API in order for synthetic parties to
 * work.
 */
class FOnlineSessionInterfaceRedpointDiscord
    : public IOnlineSession,
      public TSharedFromThis<FOnlineSessionInterfaceRedpointDiscord, ESPMode::ThreadSafe>
{
private:
    TSharedRef<discord::Core> Instance;
    mutable FCriticalSection SessionLock;
    TArray<FNamedOnlineSession> Sessions;

    discord::Event<discord::User const &>::Token OnActivityJoinRequestHandle;
    discord::Event<const char *>::Token OnActivityJoinHandle;

protected:
    virtual class FNamedOnlineSession *AddNamedSession(FName SessionName, const FOnlineSessionSettings &SessionSettings)
        override;
    virtual class FNamedOnlineSession *AddNamedSession(FName SessionName, const FOnlineSession &Session) override;

public:
    FOnlineSessionInterfaceRedpointDiscord(TSharedRef<discord::Core> InInstance)
        : Instance(MoveTemp(InInstance))
        , SessionLock()
        , Sessions()
        , OnActivityJoinRequestHandle()
        , OnActivityJoinHandle(){};
    UE_NONCOPYABLE(FOnlineSessionInterfaceRedpointDiscord);
    virtual ~FOnlineSessionInterfaceRedpointDiscord();

    void RegisterEvents();

    virtual TSharedPtr<const FUniqueNetId> CreateSessionIdFromString(const FString &SessionIdStr) override;
    virtual class FNamedOnlineSession *GetNamedSession(FName SessionName) override;
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

#endif