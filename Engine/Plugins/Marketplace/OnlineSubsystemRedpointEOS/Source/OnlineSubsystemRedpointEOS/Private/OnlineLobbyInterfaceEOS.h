// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSDefines.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"
#include "RedpointEOSInterfaces/Private/Interfaces/OnlineLobbyInterface.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FOnlineLobbyInterfaceEOS
    : public IOnlineLobby,
      public TSharedFromThis<FOnlineLobbyInterfaceEOS, ESPMode::ThreadSafe>
{
    friend class FOnlineSubsystemEOS;

private:
    EOS_HPlatform EOSPlatform;
    EOS_HLobby EOSLobby;
#if defined(EOS_VOICE_CHAT_SUPPORTED)
    TSharedPtr<class FEOSVoiceManager> VoiceManager;
#endif

    TUserIdMap<TMap<FString, EOS_HLobbyDetails>> LastSearchResultsByUser;

    /** This map is used so we can propagate events correctly. */
    TMap<FString, TArray<TSharedPtr<const FUniqueNetId>>> ConnectedMembers;
    void Internal_AddMemberToLobby(const FUniqueNetId &UserId, const FOnlineLobbyId &LobbyId);
    void Internal_RemoveMemberFromLobby(const FUniqueNetId &UserId, const FOnlineLobbyId &LobbyId);

    TSharedPtr<EOSEventHandle<EOS_Lobby_LobbyMemberStatusReceivedCallbackInfo>> Unregister_LobbyMemberStatusReceived;
    TSharedPtr<EOSEventHandle<EOS_Lobby_LobbyMemberUpdateReceivedCallbackInfo>> Unregister_LobbyMemberUpdateReceived;
    TSharedPtr<EOSEventHandle<EOS_Lobby_LobbyUpdateReceivedCallbackInfo>> Unregister_LobbyUpdateReceived;

    void Handle_LobbyMemberStatusReceived(const EOS_Lobby_LobbyMemberStatusReceivedCallbackInfo *Data);
    void Handle_LobbyMemberUpdateReceived(const EOS_Lobby_LobbyMemberUpdateReceivedCallbackInfo *Data);
    void Handle_LobbyUpdateReceived(const EOS_Lobby_LobbyUpdateReceivedCallbackInfo *Data);

    EOS_EResult CopyLobbyDetailsHandle(
        const FUniqueNetId &UserId,
        const FOnlineLobbyId &LobbyId,
        EOS_HLobbyDetails &LobbyDetails,
        bool &bShouldRelease);

#if defined(EOS_ENABLE_STATE_DIAGNOSTICS) && EOS_ENABLE_STATE_DIAGNOSTICS
    void DumpLocalLobbyAttributeState(EOS_ProductUserId LocalUserId, EOS_LobbyId InLobbyId) const;
    void DumpLobbyAttributeState(EOS_ProductUserId LocalUserId, EOS_LobbyId InLobbyId, EOS_HLobbyDetails InLobbyDetails)
        const;
#endif

public:
    FOnlineLobbyInterfaceEOS(
        EOS_HPlatform InPlatform
#if defined(EOS_VOICE_CHAT_SUPPORTED)
        ,
        const TSharedRef<class FEOSVoiceManager> &InVoiceManager
#endif
    );
    UE_NONCOPYABLE(FOnlineLobbyInterfaceEOS);
    ~FOnlineLobbyInterfaceEOS();
    void RegisterEvents();

    virtual FDateTime GetUtcNow() override;

    virtual TSharedPtr<FOnlineLobbyTransaction> MakeCreateLobbyTransaction(const FUniqueNetId &UserId) override;
    virtual TSharedPtr<FOnlineLobbyTransaction> MakeUpdateLobbyTransaction(
        const FUniqueNetId &UserId,
        const FOnlineLobbyId &LobbyId) override;
    virtual TSharedPtr<FOnlineLobbyMemberTransaction> MakeUpdateLobbyMemberTransaction(
        const FUniqueNetId &UserId,
        const FOnlineLobbyId &LobbyId,
        const FUniqueNetId &MemberId) override;

    virtual bool CreateLobby(
        const FUniqueNetId &UserId,
        const FOnlineLobbyTransaction &Transaction,
        FOnLobbyCreateOrConnectComplete OnComplete = FOnLobbyCreateOrConnectComplete()) override;
    virtual bool UpdateLobby(
        const FUniqueNetId &UserId,
        const FOnlineLobbyId &LobbyId,
        const FOnlineLobbyTransaction &Transaction,
        FOnLobbyOperationComplete OnComplete = FOnLobbyOperationComplete()) override;
    virtual bool DeleteLobby(
        const FUniqueNetId &UserId,
        const FOnlineLobbyId &LobbyId,
        FOnLobbyOperationComplete OnComplete = FOnLobbyOperationComplete()) override;

    virtual bool ConnectLobby(
        const FUniqueNetId &UserId,
        const FOnlineLobbyId &LobbyId,
        FOnLobbyCreateOrConnectComplete OnComplete = FOnLobbyCreateOrConnectComplete()) override;
    virtual bool DisconnectLobby(
        const FUniqueNetId &UserId,
        const FOnlineLobbyId &LobbyId,
        FOnLobbyOperationComplete OnComplete = FOnLobbyOperationComplete()) override;

    virtual bool UpdateMemberSelf(
        const FUniqueNetId &UserId,
        const FOnlineLobbyId &LobbyId,
        const FOnlineLobbyMemberTransaction &Transaction,
        FOnLobbyOperationComplete OnComplete = FOnLobbyOperationComplete()) override;

    virtual bool GetMemberCount(const FUniqueNetId &UserId, const FOnlineLobbyId &LobbyId, int32 &OutMemberCount)
        override;
    virtual bool GetMemberUserId(
        const FUniqueNetId &UserId,
        const FOnlineLobbyId &LobbyId,
        int32 MemberIndex,
        TSharedPtr<const FUniqueNetId> &OutMemberId) override;

    virtual bool GetMemberMetadataValue(
        const FUniqueNetId &UserId,
        const FOnlineLobbyId &LobbyId,
        const FUniqueNetId &MemberId,
        const FString &MetadataKey,
        FVariantData &OutMetadataValue) override;

    virtual bool Search(
        const FUniqueNetId &UserId,
        const FOnlineLobbySearchQuery &Query,
        FOnLobbySearchComplete OnComplete = FOnLobbySearchComplete()) override;

    virtual bool GetLobbyMetadataValue(
        const FUniqueNetId &UserId,
        const FOnlineLobbyId &LobbyId,
        const FString &MetadataKey,
        FVariantData &OutMetadataValue) override;

    virtual TSharedPtr<FOnlineLobbyId> ParseSerializedLobbyId(const FString &InLobbyId) override;

    virtual bool KickMember(
        const FUniqueNetId &UserId,
        const FOnlineLobbyId &LobbyId,
        const FUniqueNetId &MemberId,
        FOnLobbyOperationComplete OnComplete) override;
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
