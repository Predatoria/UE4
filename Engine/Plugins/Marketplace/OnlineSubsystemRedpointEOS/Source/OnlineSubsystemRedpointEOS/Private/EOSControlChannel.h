// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/ControlChannel.h"
#include "Interfaces/OnlineStatsInterface.h"
#include "Net/DataChannel.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthPhase.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthPhaseFailureCode.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/UserVerificationStatus.h"
#include "OnlineSubsystemRedpointEOS/Shared/AntiCheat/AntiCheat.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSControlMessages.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSDefines.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSNetDriver.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/UserIdMap.h"

#include "EOSControlChannel.generated.h"

UCLASS(transient, customConstructor)
class ONLINESUBSYSTEMREDPOINTEOS_API UEOSControlChannel : public UControlChannel
{
    friend class FQueuedLoginEntry;
    friend class FQueuedBeaconEntry;

    GENERATED_BODY()

public:
    void AddRoute(uint8 MessageType, const FAuthPhaseRoute &Route);
    TSharedPtr<class FAuthConnectionPhaseContext> GetAuthConnectionPhaseContext();
    TSharedPtr<class FAuthVerificationPhaseContext> GetAuthVerificationPhaseContext(const FUniqueNetIdRepl &InRepl);
    TSharedPtr<class FAuthLoginPhaseContext> GetAuthLoginPhaseContext(const FUniqueNetIdRepl &InRepl);
    TSharedPtr<class FAuthBeaconPhaseContext> GetAuthBeaconPhaseContext(
        const FUniqueNetIdRepl &InRepl,
        const FString &InBeaconName);

    bool bClientTrustsServer;
    TUserIdMap<bool> bRegisteredForAntiCheat;
    TUserIdMap<EUserVerificationStatus> VerificationDatabase;

private:
    TMap<uint8, FAuthPhaseRoute> Routes;

    TSharedPtr<class FAuthConnectionPhaseContext> AuthConnectionContext;
    TUserIdMap<TSharedPtr<class FAuthVerificationPhaseContext>> AuthVerificationContexts;
    TUserIdMap<TSharedPtr<class FQueuedLoginEntry>> QueuedLogins;
    TUserIdMap<TMap<FString, TSharedPtr<class FQueuedBeaconEntry>>> QueuedBeacons;

    bool bGotCachedEACSourceUserId;
    TSharedPtr<const FUniqueNetIdEOS> CachedEACSourceUserId;

    struct FOriginalParameters_NMT_Hello
    {
        uint8 IsLittleEndian;
        uint32 RemoteNetworkVersion;
        FString EncryptionToken;
#if defined(UE_5_1_OR_LATER)
        EEngineNetworkRuntimeFeatures NetworkFeatures;
#endif
    };

    void On_NMT_Hello(const FOriginalParameters_NMT_Hello &Parameters);
    void Finalize_NMT_Hello(
        EAuthPhaseFailureCode Result,
        const FString &ErrorMessage,
        FOriginalParameters_NMT_Hello Parameters);

#if EOS_VERSION_AT_LEAST(1, 12, 0)
    void On_NMT_EOS_AntiCheatMessage(
        const FUniqueNetIdRepl &SourceUserId,
        const FUniqueNetIdRepl &TargetUserId,
        const TArray<uint8> &AntiCheatData);

    FDelegateHandle AuthStatusChangedHandle;
    FDelegateHandle ActionRequiredHandle;
    void OnAntiCheatPlayerAuthStatusChanged(
        const FUniqueNetIdEOS &TargetUserId,
        EOS_EAntiCheatCommonClientAuthStatus NewAuthStatus);
    void OnAntiCheatPlayerActionRequired(
        const FUniqueNetIdEOS &TargetUserId,
        EOS_EAntiCheatCommonClientAction ClientAction,
        EOS_EAntiCheatCommonClientActionReason ActionReasonCode,
        const FString &ActionReasonDetailsString);
#endif

    void StartAuthentication(
        const FUniqueNetIdRepl &IncomingUser,
        const TSharedRef<class IQueuedEntry> &NetworkEntry,
        const FString &BeaconName,
        bool bIsBeacon);
    void OnAuthenticationComplete(
        EAuthPhaseFailureCode Result,
        const FString &ErrorMessage,
        TSharedRef<class IQueuedEntry> NetworkEntry);

    void On_NMT_Login(
        FString ClientResponse,
        FString URLString,
        FUniqueNetIdRepl IncomingUser,
        FString OnlinePlatformNameString);
    void On_NMT_BeaconJoin(FString BeaconType, FUniqueNetIdRepl IncomingUser);

public:
    UEOSControlChannel(const FObjectInitializer &ObjectInitializer = FObjectInitializer::Get());

    virtual void Init(UNetConnection *InConnection, int32 InChIndex, EChannelCreateFlags CreateFlags) override;
    virtual bool CleanUp(const bool bForDestroy, EChannelCloseReason CloseReason) override;

    virtual void ReceivedBunch(FInBunch &Bunch) override;
};
