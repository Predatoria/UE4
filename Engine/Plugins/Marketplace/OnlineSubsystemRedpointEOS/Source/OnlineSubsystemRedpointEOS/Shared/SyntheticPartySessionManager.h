// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"

#include "Containers/Array.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlinePartyInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOSSession.h"

EOS_ENABLE_STRICT_WARNINGS

DECLARE_DELEGATE_OneParam(FSyntheticPartySessionOnComplete, const FOnlineError & /* Error */);

class FSyntheticPartySessionManager : public TSharedFromThis<FSyntheticPartySessionManager>
{
    friend class FOnlineSubsystemEOS;

private:
    void RegisterEvents();

    struct FWrappedSubsystem
    {
        FName SubsystemName;
        TWeakPtr<IOnlineIdentity, ESPMode::ThreadSafe> Identity;
        TWeakPtr<IOnlineSession, ESPMode::ThreadSafe> Session;
    };

    TSharedRef<class IEOSRuntimePlatform> RuntimePlatform;
    TSharedRef<class FEOSConfig> Config;
    TWeakPtr<class FOnlinePartySystemEOS, ESPMode::ThreadSafe> EOSPartySystem;
    TWeakPtr<class FOnlineSessionInterfaceEOS, ESPMode::ThreadSafe> EOSSession;
    TWeakPtr<class FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> EOSIdentitySystem;
    TArray<FWrappedSubsystem> WrappedSubsystems;
    TMap<TWeakPtr<IOnlineSession, ESPMode::ThreadSafe>, FDelegateHandle> SessionUserInviteAcceptedDelegateHandles;
    TMap<FString, FDelegateHandle> HandlesPendingRemoval;

    void HandlePartyInviteAccept(int32 ControllerId, const FString &EOSPartyId);
    void HandleSessionInviteAccept(int32 ControllerId, const FString &EOSSessionId);

public:
    FSyntheticPartySessionManager(
        FName InInstanceName,
        TWeakPtr<class FOnlinePartySystemEOS, ESPMode::ThreadSafe> InEOSPartySystem,
        TWeakPtr<class FOnlineSessionInterfaceEOS, ESPMode::ThreadSafe> InEOSSession,
        TWeakPtr<class FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> InEOSIdentitySystem,
        const TSharedRef<class IEOSRuntimePlatform> &InRuntimePlatform,
        const TSharedRef<class FEOSConfig> &InConfig);
    UE_NONCOPYABLE(FSyntheticPartySessionManager);
    ~FSyntheticPartySessionManager();

    void CreateSyntheticParty(
        const TSharedPtr<const FOnlinePartyId> &PartyId,
        const FSyntheticPartySessionOnComplete &OnComplete);
    void DeleteSyntheticParty(
        const TSharedPtr<const FOnlinePartyId> &PartyId,
        const FSyntheticPartySessionOnComplete &OnComplete);
    bool HasSyntheticParty(const TSharedPtr<const FOnlinePartyId> &PartyId);
    void CreateSyntheticSession(
        const TSharedPtr<const FUniqueNetIdEOSSession> &SessionId,
        const FSyntheticPartySessionOnComplete &OnComplete);
    void DeleteSyntheticSession(
        const TSharedPtr<const FUniqueNetIdEOSSession> &SessionId,
        const FSyntheticPartySessionOnComplete &OnComplete);
    bool HasSyntheticSession(const TSharedPtr<const FUniqueNetIdEOSSession> &SessionId);
    bool HasSyntheticSession(const FName &SubsystemName, const TSharedPtr<const FUniqueNetIdEOSSession> &SessionId);
    FName GetSyntheticSessionNativeSessionName(
        const FName &SubsystemName,
        const TSharedPtr<const FUniqueNetIdEOSSession> &SessionId);
    void SendInvitationToParty(
        int32 LocalUserNum,
        const TSharedPtr<const FOnlinePartyId> &PartyId,
        const TSharedPtr<const FUniqueNetId> &RecipientId,
        const FSyntheticPartySessionOnComplete &Delegate);
    void SendInvitationToSession(
        int32 LocalUserNum,
        const TSharedPtr<const FUniqueNetIdEOSSession> &SessionId,
        const TSharedPtr<const FUniqueNetId> &RecipientId,
        const FSyntheticPartySessionOnComplete &Delegate);
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION