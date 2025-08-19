// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "Containers/StringConv.h"
#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "Delegates/DelegateCombinations.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlinePartyInterface.h"
#include "OnlineSubsystemRedpointEOS/Shared/AsyncMutex.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSDefines.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlinePartyIdEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineUserEOS.h"

EOS_ENABLE_STRICT_WARNINGS

class FOnlinePartyEOS;
class FOnlinePartySystemEOS;

class FOnlinePartyMemberEOS : public TBaseUserImplementation<FOnlinePartyMemberEOS, FOnlinePartyMember>
{
    friend class TBaseUserImplementation<FOnlinePartyMemberEOS, FOnlinePartyMember>;
    friend class FOnlinePartyEOS;

    // Supported:
    // - OnMemberConnectionStatusChanged
    // - OnMemberAttributeChanged

private:
    FOnlinePartyMemberEOS()
        : TBaseUserImplementation<FOnlinePartyMemberEOS, FOnlinePartyMember>(){};

public:
    FOnlineKeyValuePairs<FString, FVariantData> PartyMemberAttributes;

    UE_NONCOPYABLE(FOnlinePartyMemberEOS);
};

class FOnlinePartyEOS : public FOnlineParty
{
    friend class FOnlinePartySystemEOS;

private:
    EOS_HPlatform EOSPlatform;
    EOS_HLobby EOSLobby;
    TWeakPtr<class FOnlinePartySystemEOS, ESPMode::ThreadSafe> PartySystem;
    TWeakPtr<FEOSUserFactory, ESPMode::ThreadSafe> UserFactory;
    TSharedRef<const FUniqueNetIdEOS> LocalUserId;
    bool bRTCEnabled;
    bool bReadyForEvents;

    const TSharedRef<const FOnlinePartyId> ResolvePartyIdAndInit(EOS_HLobbyDetails InLobbyHandle);
    const FOnlinePartyTypeId ResolvePartyTypeId(EOS_HLobbyDetails InLobbyHandle);
    EOS_HLobbyDetails LobbyHandle;
    EOS_LobbyDetails_Info *LobbyInfo;
    TSharedPtr<const FPartyConfiguration> Config;

    FOnlineKeyValuePairs<FString, FVariantData> ReadMemberAttributes(const FUniqueNetIdEOS &MemberId) const;
    FOnlineKeyValuePairs<FString, FVariantData> ReadLobbyAttributes() const;

    TArray<TSharedPtr<FOnlinePartyMemberEOS>> Members;
    FOnlineKeyValuePairs<FString, FVariantData> Attributes;

public:
    FOnlinePartyEOS(
        EOS_HPlatform InPlatform,
        EOS_HLobbyDetails InLobbyHandle,
        TSharedRef<const FUniqueNetIdEOS> InEventSendingUserId,
        TWeakPtr<class FOnlinePartySystemEOS, ESPMode::ThreadSafe> InPartySystem,
        TWeakPtr<FEOSUserFactory, ESPMode::ThreadSafe> InUserFactory,
        bool bInRTCEnabled,
        const TSharedPtr<FPartyConfiguration> &PartyConfigurationFromCreate);

    FOnlinePartyEOS() = delete;
    UE_NONCOPYABLE(FOnlinePartyEOS);
    ~FOnlinePartyEOS();

    bool CanLocalUserInvite(const FUniqueNetId &) const override;
    bool IsJoinable() const override;
    TSharedRef<const FPartyConfiguration> GetConfiguration() const override;
    bool IsRTCEnabled() const;

    void UpdateWithLatestInfo();
};

class IOnlinePartyJoinInfoEOS : public IOnlinePartyJoinInfo
{
public:
    IOnlinePartyJoinInfoEOS() = default;
    UE_NONCOPYABLE(IOnlinePartyJoinInfoEOS);
    virtual bool IsUnresolved() const = 0;
};

class FOnlinePartyJoinInfoEOS : public IOnlinePartyJoinInfoEOS
{
private:
    TSharedPtr<FOnlinePartyIdEOS> LobbyId;
    EOS_LobbyDetails_Info *LobbyInfo;
    FString Empty;
    EOS_HLobbyDetails LobbyHandle;
    FString SourcePlatform;
    FString PlatformData;
    FOnlinePartyTypeId PartyTypeId;
    TSharedPtr<const FUniqueNetId> SenderId;
    FString SenderDisplayName;
    FString InviteId;

public:
    FOnlinePartyJoinInfoEOS() = delete;
    FOnlinePartyJoinInfoEOS(
        EOS_HLobbyDetails LobbyDetails,
        TSharedPtr<const FUniqueNetId> SenderId = nullptr,
        const FString &SenderDisplayName = TEXT(""),
        const FString &InviteId = TEXT(""));
    UE_NONCOPYABLE(FOnlinePartyJoinInfoEOS);
    ~FOnlinePartyJoinInfoEOS();

    virtual bool GetLobbyHandle(EOS_HLobbyDetails &OutLobbyHandle) const;
    static TSharedPtr<const FOnlinePartyJoinInfoEOS> GetEOSJoinInfo(const IOnlinePartyJoinInfo &PartyInfo);
    virtual FString GetInviteId() const;

    virtual bool IsValid() const override;
    virtual TSharedRef<const FOnlinePartyId> GetPartyId() const override;
    virtual FOnlinePartyTypeId GetPartyTypeId() const override;
    virtual TSharedRef<const FUniqueNetId> GetSourceUserId() const override;
    virtual const FString &GetSourceDisplayName() const override;
    virtual const FString &GetSourcePlatform() const override;
    virtual const FString &GetPlatformData() const override;
    virtual bool HasKey() const override;
    virtual bool HasPassword() const override;
    virtual bool IsAcceptingMembers() const override;
    virtual bool IsPartyOfOne() const override;
    virtual int32 GetNotAcceptingReason() const override;
    virtual const FString &GetAppId() const override;
    virtual const FString &GetBuildId() const override;
    virtual bool CanJoin() const override;
    virtual bool CanJoinWithPassword() const override;
    virtual bool CanRequestAnInvite() const override;

    virtual bool IsUnresolved() const override
    {
        return false;
    }
};

class FOnlinePartyJoinInfoEOSUnresolved : public IOnlinePartyJoinInfoEOS
{
private:
    TSharedPtr<FOnlinePartyIdEOS> LobbyId;
    FString Empty;
    FString SourcePlatform;

public:
    FOnlinePartyJoinInfoEOSUnresolved() = delete;
    FOnlinePartyJoinInfoEOSUnresolved(EOS_LobbyId InLobbyId);
    UE_NONCOPYABLE(FOnlinePartyJoinInfoEOSUnresolved);
    ~FOnlinePartyJoinInfoEOSUnresolved(){};

    static TSharedPtr<const FOnlinePartyJoinInfoEOSUnresolved> GetEOSUnresolvedJoinInfo(
        const IOnlinePartyJoinInfo &PartyInfo);

    virtual bool IsValid() const override;
    virtual TSharedRef<const FOnlinePartyId> GetPartyId() const override;
    virtual FOnlinePartyTypeId GetPartyTypeId() const override;
    virtual TSharedRef<const FUniqueNetId> GetSourceUserId() const override;
    virtual const FString &GetSourceDisplayName() const override;
    virtual const FString &GetSourcePlatform() const override;
    virtual const FString &GetPlatformData() const override;
    virtual bool HasKey() const override;
    virtual bool HasPassword() const override;
    virtual bool IsAcceptingMembers() const override;
    virtual bool IsPartyOfOne() const override;
    virtual int32 GetNotAcceptingReason() const override;
    virtual const FString &GetAppId() const override;
    virtual const FString &GetBuildId() const override;
    virtual bool CanJoin() const override;
    virtual bool CanJoinWithPassword() const override;
    virtual bool CanRequestAnInvite() const override;

    virtual bool IsUnresolved() const override
    {
        return true;
    }
};

enum class EOnlinePartyRecordedEventType : uint8
{
    LobbyMemberStatusReceived,
    LobbyMemberUpdateReceived,
    LobbyUpdateReceived,
};

class FOnlinePartyRecordedEvent
{
private:
    EOnlinePartyRecordedEventType EventType;
    TSharedRef<const FUniqueNetId> UserId;
    TSharedRef<const FOnlinePartyId> PartyId;
    EOS_ELobbyMemberStatus MemberStatus;

public:
    FOnlinePartyRecordedEvent(
        EOnlinePartyRecordedEventType InEventType,
        TSharedRef<const FUniqueNetId> InUserId,
        TSharedRef<const FOnlinePartyId> InPartyId,
        EOS_ELobbyMemberStatus InMemberStatus)
        : EventType(InEventType)
        , UserId(MoveTemp(InUserId))
        , PartyId(MoveTemp(InPartyId))
        , MemberStatus(InMemberStatus)
    {
    }

    friend bool operator==(const FOnlinePartyRecordedEvent &LHS, const FOnlinePartyRecordedEvent &RHS)
    {
        return GetTypeHash(LHS) == GetTypeHash(RHS);
    }

    friend inline uint32 GetTypeHash(const FOnlinePartyRecordedEvent &Value)
    {
        return GetTypeHash((int)Value.EventType) * GetTypeHash(*Value.UserId) * GetTypeHash(*Value.PartyId) *
               GetTypeHash((int)Value.MemberStatus);
    }
};

class FOnlinePartySystemEOS : public IOnlinePartySystem,
                              public TSharedFromThis<FOnlinePartySystemEOS, ESPMode::ThreadSafe>
{
    friend class FOnlineSubsystemEOS;
    friend class FOnlinePartyEOS;

private:
    EOS_HPlatform EOSPlatform;
    EOS_HConnect EOSConnect;
    EOS_HLobby EOSLobby;
    EOS_HUI EOSUI;
    TSharedPtr<class FEOSConfig> Config;
    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
    TSharedPtr<class FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> Identity;
    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
    TSharedPtr<IOnlineFriends, ESPMode::ThreadSafe> Friends;
    TSharedPtr<FEOSUserFactory, ESPMode::ThreadSafe> UserFactory;
#if defined(EOS_VOICE_CHAT_SUPPORTED)
    TSharedPtr<class FEOSVoiceManager> VoiceManager;
#endif
    TSet<FOnlinePartyRecordedEvent> EventDeduplication;

    void RegisterEvents();
    void Tick();

    TUserIdMap<TArray<TSharedPtr<FOnlinePartyEOS>>> JoinedParties;
    TUserIdMap<TArray<IOnlinePartyJoinInfoConstRef>> PendingInvites;
    TWeakPtr<class FSyntheticPartySessionManager> SyntheticPartySessionManager;
    FAsyncMutex CreateJoinMutex;

    TSharedPtr<EOSEventHandle<EOS_Lobby_JoinLobbyAcceptedCallbackInfo>> Unregister_JoinLobbyAccepted;
    TSharedPtr<EOSEventHandle<EOS_Lobby_LobbyInviteAcceptedCallbackInfo>> Unregister_LobbyInviteAccepted;
    TSharedPtr<EOSEventHandle<EOS_Lobby_LobbyInviteReceivedCallbackInfo>> Unregister_LobbyInviteReceived;
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    TSharedPtr<EOSEventHandle<EOS_Lobby_LobbyInviteRejectedCallbackInfo>> Unregister_LobbyInviteRejected;
#endif
    TSharedPtr<EOSEventHandle<EOS_Lobby_LobbyMemberStatusReceivedCallbackInfo>> Unregister_LobbyMemberStatusReceived;
    TSharedPtr<EOSEventHandle<EOS_Lobby_LobbyMemberUpdateReceivedCallbackInfo>> Unregister_LobbyMemberUpdateReceived;
    TSharedPtr<EOSEventHandle<EOS_Lobby_LobbyUpdateReceivedCallbackInfo>> Unregister_LobbyUpdateReceived;

    void Handle_JoinLobbyAccepted(const EOS_Lobby_JoinLobbyAcceptedCallbackInfo *Data);
    void Handle_LobbyInviteAccepted(const EOS_Lobby_LobbyInviteAcceptedCallbackInfo *Data);
    void Handle_LobbyInviteReceived(const EOS_Lobby_LobbyInviteReceivedCallbackInfo *Data);
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    void Handle_LobbyInviteRejected(const EOS_Lobby_LobbyInviteRejectedCallbackInfo *Data);
#endif

    void Handle_LobbyMemberStatusReceived(const EOS_Lobby_LobbyMemberStatusReceivedCallbackInfo *Data);
    void Handle_LobbyMemberUpdateReceived(const EOS_Lobby_LobbyMemberUpdateReceivedCallbackInfo *Data);
    void Handle_LobbyUpdateReceived(const EOS_Lobby_LobbyUpdateReceivedCallbackInfo *Data);

    // DoLeaveParty needs to call into logic that Handle_LobbyMemberStatusReceived would otherwise handle, so the common
    // behaviour is contained here.
    void MemberStatusChanged(
        EOS_LobbyId InLobbyId,
        EOS_ProductUserId InTargetUserId,
        EOS_ELobbyMemberStatus InCurrentStatus);

    bool DoLeaveParty(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        bool bSynchronizeLeave,
        const FOnLeavePartyComplete &Delegate);

    bool IsLocalUserInParty(const FOnlinePartyId &LobbyId, const FUniqueNetId &LocalUserId) const;
    bool ShouldHaveSyntheticParty(const FOnlinePartyTypeId &PartyTypeId, const FPartyConfiguration &PartyConfiguration)
        const;

    bool SerializePartyAndPartyConfiguration(
        EOS_HLobbyModification Mod,
        const FOnlinePartyTypeId PartyTypeId,
        const FPartyConfiguration &LatestPartyConfiguration) const;

public:
    FOnlinePartySystemEOS(
        EOS_HPlatform InPlatform,
        const TSharedRef<class FEOSConfig> &InConfig,
        const TSharedRef<class FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> &InIdentity,
        const TSharedRef<IOnlineFriends, ESPMode::ThreadSafe> &InFriends,
        const TSharedRef<FEOSUserFactory, ESPMode::ThreadSafe> &InUserFactory
#if defined(EOS_VOICE_CHAT_SUPPORTED)
        ,
        const TSharedRef<class FEOSVoiceManager> &InVoiceManager
#endif
    );
    UE_NONCOPYABLE(FOnlinePartySystemEOS);
    void SetSyntheticPartySessionManager(
        const TSharedPtr<class FSyntheticPartySessionManager> &InSyntheticPartySessionManager)
    {
        this->SyntheticPartySessionManager = InSyntheticPartySessionManager;
    }

    void LookupPartyById(
        EOS_LobbyId InId,
        EOS_ProductUserId InSearchingUser,
        const std::function<void(EOS_HLobbyDetails Handle)> &OnDone);

    // Supported:
    // - OnPartyJoined
    // - OnPartyExited
    // - OnPartyInvitesChanged
    // - OnPartyInviteReceived
    // - OnPartyInviteRemoved
    // - OnPartyDataReceivedConst
    // - OnPartyMemberPromoted
    // - OnPartyMemberExited
    // - OnPartyMemberJoined
    // - OnPartyMemberDataReceivedConst
    //
    // Not Supported:
    // - OnPartyStateChanged
    // - OnPartyJIP
    // - OnPartyPromotionLockoutChanged
    // - OnPartyConfigChangedConst
    // - OnPartyInviteRequestReceived
    // - OnPartyInviteResponseReceived
    // - OnPartyGroupJoinRequestReceived
    // - OnPartyJIPRequestReceived
    // - OnQueryPartyJoinabilityGroupReceived
    // - OnFillPartyJoinRequestData
    // - OnPartyAnalyticsEvent
    // - OnPartySystemStateChange

    virtual void RestoreParties(const FUniqueNetId &LocalUserId, const FOnRestorePartiesComplete &CompletionDelegate)
        override;
    virtual void RestoreInvites(const FUniqueNetId &LocalUserId, const FOnRestoreInvitesComplete &CompletionDelegate)
        override;
    virtual void CleanupParties(const FUniqueNetId &LocalUserId, const FOnCleanupPartiesComplete &CompletionDelegate)
        override;
    virtual bool CreateParty(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyTypeId PartyTypeId,
        const FPartyConfiguration &PartyConfig,
        const FOnCreatePartyComplete &Delegate = FOnCreatePartyComplete()) override;
    virtual bool UpdateParty(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FPartyConfiguration &PartyConfig,
        bool bShouldRegenerateReservationKey = false,
        const FOnUpdatePartyComplete &Delegate = FOnUpdatePartyComplete()) override;
    virtual bool JoinParty(
        const FUniqueNetId &LocalUserId,
        const IOnlinePartyJoinInfo &OnlinePartyJoinInfo,
        const FOnJoinPartyComplete &Delegate = FOnJoinPartyComplete()) override;
#if defined(EOS_PARTY_SYSTEM_REQUEST_TO_JOIN_PARTY)
    virtual void RequestToJoinParty(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyTypeId PartyTypeId,
        const FPartyInvitationRecipient &Recipient,
        const FOnRequestToJoinPartyComplete &Delegate = FOnRequestToJoinPartyComplete()) override;
    virtual void ClearRequestToJoinParty(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &Sender,
        EPartyRequestToJoinRemovedReason Reason) override;
#endif // #if defined(EOS_PARTY_SYSTEM_REQUEST_TO_JOIN_PARTY)
    virtual bool JIPFromWithinParty(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &PartyLeaderId) override;
#if !defined(UE_5_1_OR_LATER)
    virtual void QueryPartyJoinability(
        const FUniqueNetId &LocalUserId,
        const IOnlinePartyJoinInfo &OnlinePartyJoinInfo,
        const FOnQueryPartyJoinabilityComplete &Delegate = FOnQueryPartyJoinabilityComplete()) override;
#endif
#if defined(EOS_PARTY_SYSTEM_HAS_QUERY_PARTY_JOINABILITY_EX)
    virtual void QueryPartyJoinability(
        const FUniqueNetId &LocalUserId,
        const IOnlinePartyJoinInfo &OnlinePartyJoinInfo,
        const FOnQueryPartyJoinabilityCompleteEx &Delegate = FOnQueryPartyJoinabilityCompleteEx()) override;
#endif
    virtual bool RejoinParty(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FOnlinePartyTypeId &PartyTypeId,
        const TArray<TSharedRef<const FUniqueNetId>> &FormerMembers,
        const FOnJoinPartyComplete &Delegate = FOnJoinPartyComplete()) override;
    virtual bool LeaveParty(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FOnLeavePartyComplete &Delegate = FOnLeavePartyComplete()) override;
    virtual bool LeaveParty(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        bool bSynchronizeLeave,
        const FOnLeavePartyComplete &Delegate = FOnLeavePartyComplete()) override;
    virtual bool ApproveJoinRequest(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &RecipientId,
        bool bIsApproved,
        int32 DeniedResultCode = 0) override;
    virtual bool ApproveJIPRequest(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &RecipientId,
        bool bIsApproved,
        int32 DeniedResultCode = 0) override;
#if !defined(UE_5_1_OR_LATER)
    virtual void RespondToQueryJoinability(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &RecipientId,
        bool bCanJoin,
        int32 DeniedResultCode = 0) override;
#endif
#if defined(EOS_PARTY_SYSTEM_HAS_RESPOND_TO_QUERY_JOINABILITY)
    virtual void RespondToQueryJoinability(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &RecipientId,
        bool bCanJoin,
        int32 DeniedResultCode,
        FOnlinePartyDataConstPtr PartyData) override;
#endif
    virtual bool SendInvitation(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FPartyInvitationRecipient &Recipient,
        const FOnSendPartyInvitationComplete &Delegate = FOnSendPartyInvitationComplete()) override;
#if defined(UE_5_1_OR_LATER)
    virtual void CancelInvitation(
        const FUniqueNetId &LocalUserId,
        const FUniqueNetId &TargetUserId,
        const FOnlinePartyId &PartyId,
        const FOnCancelPartyInvitationComplete &Delegate = FOnCancelPartyInvitationComplete()) override;
#endif
    virtual bool RejectInvitation(const FUniqueNetId &LocalUserId, const FUniqueNetId &SenderId) override;
    /** An EOS-specific extension that allows you to be aware of when the invite is actually rejected (since it is an
     * asynchronous call in EOS). */
    virtual bool RejectInvitation(
        const FUniqueNetId &LocalUserId,
        const IOnlinePartyJoinInfo &OnlinePartyJoinInfo,
        const FOnRejectPartyInvitationComplete &Delegate);
    virtual void ClearInvitations(
        const FUniqueNetId &LocalUserId,
        const FUniqueNetId &SenderId,
        const FOnlinePartyId *PartyId = nullptr) override;
    virtual bool KickMember(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &TargetMemberId,
        const FOnKickPartyMemberComplete &Delegate = FOnKickPartyMemberComplete()) override;
    virtual bool PromoteMember(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &TargetMemberId,
        const FOnPromotePartyMemberComplete &Delegate = FOnPromotePartyMemberComplete()) override;
    virtual bool UpdatePartyData(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FName &Namespace,
        const FOnlinePartyData &PartyData) override;
    virtual bool UpdatePartyMemberData(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FName &Namespace,
        const FOnlinePartyData &PartyMemberData) override;
    virtual bool IsMemberLeader(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &MemberId) const override;
    virtual uint32 GetPartyMemberCount(const FUniqueNetId &LocalUserId, const FOnlinePartyId &PartyId) const override;
    virtual FOnlinePartyConstPtr GetParty(const FUniqueNetId &LocalUserId, const FOnlinePartyId &PartyId)
        const override;
    virtual FOnlinePartyConstPtr GetParty(const FUniqueNetId &LocalUserId, const FOnlinePartyTypeId &PartyTypeId)
        const override;
    virtual FOnlinePartyMemberConstPtr GetPartyMember(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &MemberId) const override;
    virtual FOnlinePartyDataConstPtr GetPartyData(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FName &Namespace
    ) const override;
    virtual FOnlinePartyDataConstPtr GetPartyMemberData(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &MemberId,
        const FName &Namespace
    ) const override;
    virtual IOnlinePartyJoinInfoConstPtr GetAdvertisedParty(
        const FUniqueNetId &LocalUserId,
        const FUniqueNetId &UserId,
        const FOnlinePartyTypeId PartyTypeId) const override;
    virtual bool GetJoinedParties(
        const FUniqueNetId &LocalUserId,
        TArray<TSharedRef<const FOnlinePartyId>> &OutPartyIdArray) const override;
    virtual bool GetPartyMembers(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        TArray<FOnlinePartyMemberConstRef> &OutPartyMembersArray) const override;
    virtual bool GetPendingInvites(
        const FUniqueNetId &LocalUserId,
        TArray<IOnlinePartyJoinInfoConstRef> &OutPendingInvitesArray) const override;
    virtual bool GetPendingJoinRequests(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        TArray<IOnlinePartyPendingJoinRequestInfoConstRef> &OutPendingJoinRequestArray) const override;
    virtual bool GetPendingInvitedUsers(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        TArray<TSharedRef<const FUniqueNetId>> &OutPendingInvitedUserArray) const override;
#if defined(EOS_PARTY_SYSTEM_REQUEST_TO_JOIN_PARTY)
    virtual bool GetPendingRequestsToJoin(
        const FUniqueNetId &LocalUserId,
        TArray<IOnlinePartyRequestToJoinInfoConstRef> &OutRequestsToJoin) const override;
#endif // #if defined (EOS_PARTY_SYSTEM_REQUEST_TO_JOIN_PARTY)

    virtual FString MakeJoinInfoJson(const FUniqueNetId &LocalUserId, const FOnlinePartyId &PartyId) override;
    virtual IOnlinePartyJoinInfoConstPtr MakeJoinInfoFromJson(const FString &JoinInfoJson) override;
    virtual FString MakeTokenFromJoinInfo(const IOnlinePartyJoinInfo &JoinInfo) const override;
    virtual IOnlinePartyJoinInfoConstPtr MakeJoinInfoFromToken(const FString &Token) const override;

    virtual IOnlinePartyJoinInfoConstPtr ConsumePendingCommandLineInvite() override;

    virtual void DumpPartyState() override;
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
