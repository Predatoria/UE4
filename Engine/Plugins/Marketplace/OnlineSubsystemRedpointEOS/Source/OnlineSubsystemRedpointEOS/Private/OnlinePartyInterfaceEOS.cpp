// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/OnlinePartyInterfaceEOS.h"

#include "Containers/StringConv.h"
#include "Containers/Ticker.h"
#include "Dom/JsonObject.h"
#include "Interfaces/OnlinePartyInterface.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSErrorConv.h"
#include "OnlineSubsystemRedpointEOS/Shared/HeapLambda.h"
#include "OnlineSubsystemRedpointEOS/Shared/MultiOperation.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineFriendSynthetic.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineIdentityInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/SyntheticPartySessionManager.h"
#include "OnlineSubsystemRedpointEOS/Shared/VoiceChat/VoiceManager.h"
#include "OnlineSubsystemRedpointEOS/Shared/VoiceChat/VoiceManagerLocalUser.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include <functional>

EOS_ENABLE_STRICT_WARNINGS

#define PARTY_TYPE_ID_ANSI "PartyTypeId"
#define PARTY_TYPE_ID_TCHAR TEXT("PartyTypeId")

FOnlinePartyJoinInfoEOS::FOnlinePartyJoinInfoEOS(
    EOS_HLobbyDetails InLobbyDetails,
    TSharedPtr<const FUniqueNetId> InSenderId,
    const FString &InSenderDisplayName,
    const FString &InInviteId)
    : LobbyId()
    , LobbyInfo(nullptr)
    , Empty()
    , LobbyHandle(InLobbyDetails)
    , SourcePlatform("EOS")
    ,
    PlatformData("")
    ,
    PartyTypeId()
    , SenderId(MoveTemp(InSenderId))
    , SenderDisplayName(InSenderDisplayName)
    , InviteId(InInviteId)
{
    EOS_LobbyDetails_CopyInfoOptions Opts = {};
    Opts.ApiVersion = EOS_LOBBYDETAILS_COPYINFO_API_LATEST;

    check(InLobbyDetails != nullptr);
    verify(EOS_LobbyDetails_CopyInfo(InLobbyDetails, &Opts, &this->LobbyInfo) == EOS_EResult::EOS_Success);

    this->LobbyId = MakeShared<FOnlinePartyIdEOS>(this->LobbyInfo->LobbyId);

    EOS_LobbyDetails_CopyAttributeByKeyOptions CopyOpts = {};
    CopyOpts.ApiVersion = EOS_LOBBYDETAILS_COPYATTRIBUTEBYKEY_API_LATEST;
    CopyOpts.AttrKey = PARTY_TYPE_ID_ANSI;

    EOS_Lobby_Attribute *Attr = nullptr;
    if (EOS_LobbyDetails_CopyAttributeByKey(this->LobbyHandle, &CopyOpts, &Attr) != EOS_EResult::EOS_Success)
    {
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("Assuming primary party type ID for join info, since the PartyTypeId attribute was not found on "
                 "the lobby"));
        this->PartyTypeId = FOnlinePartySystemEOS::GetPrimaryPartyTypeId();
    }
    else if (Attr->Data->ValueType != EOS_ELobbyAttributeType::EOS_AT_INT64)
    {
        EOS_Lobby_Attribute_Release(Attr);
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("Assuming primary party type ID for join info, since the PartyTypeId attribute was not int64"));
        this->PartyTypeId = FOnlinePartySystemEOS::GetPrimaryPartyTypeId();
    }
    else
    {
        this->PartyTypeId = FOnlinePartyTypeId((const FOnlinePartyTypeId::TInternalType)Attr->Data->Value.AsInt64);
        EOS_Lobby_Attribute_Release(Attr);
    }
}

FOnlinePartyJoinInfoEOS::~FOnlinePartyJoinInfoEOS()
{
    if (this->LobbyInfo != nullptr)
    {
        EOS_LobbyDetails_Info_Release(this->LobbyInfo);
        this->LobbyInfo = nullptr;
    }

    if (this->LobbyHandle != nullptr)
    {
        EOS_LobbyDetails_Release(this->LobbyHandle);
        this->LobbyHandle = nullptr;
    }
}

bool FOnlinePartyJoinInfoEOS::GetLobbyHandle(EOS_HLobbyDetails &OutLobbyHandle) const
{
    OutLobbyHandle = this->LobbyHandle;
    return true;
}

TSharedPtr<const FOnlinePartyJoinInfoEOS> FOnlinePartyJoinInfoEOS::GetEOSJoinInfo(const IOnlinePartyJoinInfo &PartyInfo)
{
    if (PartyInfo.GetSourcePlatform() == TEXT("EOS"))
    {
        return StaticCastSharedRef<const FOnlinePartyJoinInfoEOS>(PartyInfo.AsShared());
    }

    return nullptr;
}

FString FOnlinePartyJoinInfoEOS::GetInviteId() const
{
    return this->InviteId;
}

bool FOnlinePartyJoinInfoEOS::IsValid() const
{
    return true;
}

TSharedRef<const FOnlinePartyId> FOnlinePartyJoinInfoEOS::GetPartyId() const
{
    return this->LobbyId.ToSharedRef();
}

FOnlinePartyTypeId FOnlinePartyJoinInfoEOS::GetPartyTypeId() const
{
    return this->PartyTypeId;
}

TSharedRef<const FUniqueNetId> FOnlinePartyJoinInfoEOS::GetSourceUserId() const
{
    if (this->SenderId.IsValid())
    {
        return this->SenderId.ToSharedRef();
    }

    return FUniqueNetIdEOS::InvalidId();
}

const FString &FOnlinePartyJoinInfoEOS::GetSourceDisplayName() const
{
    return this->SenderDisplayName;
}

const FString &FOnlinePartyJoinInfoEOS::GetSourcePlatform() const
{
    return this->SourcePlatform;
}

const FString &FOnlinePartyJoinInfoEOS::GetPlatformData() const
{
    return this->PlatformData;
}

bool FOnlinePartyJoinInfoEOS::HasKey() const
{
    return false;
}

bool FOnlinePartyJoinInfoEOS::HasPassword() const
{
    return false;
}

bool FOnlinePartyJoinInfoEOS::IsAcceptingMembers() const
{
    return true;
}

bool FOnlinePartyJoinInfoEOS::IsPartyOfOne() const
{
    return false;
}

int32 FOnlinePartyJoinInfoEOS::GetNotAcceptingReason() const
{
    return 0;
}

const FString &FOnlinePartyJoinInfoEOS::GetAppId() const
{
    return this->Empty;
}

const FString &FOnlinePartyJoinInfoEOS::GetBuildId() const
{
    return this->Empty;
}

bool FOnlinePartyJoinInfoEOS::CanJoin() const
{
    return true;
}

bool FOnlinePartyJoinInfoEOS::CanJoinWithPassword() const
{
    return true;
}

bool FOnlinePartyJoinInfoEOS::CanRequestAnInvite() const
{
    return true;
}

FOnlinePartyJoinInfoEOSUnresolved::FOnlinePartyJoinInfoEOSUnresolved(EOS_LobbyId InLobbyId)
{
    this->LobbyId = MakeShared<FOnlinePartyIdEOS>(InLobbyId);
    this->SourcePlatform = TEXT("EOSUnresolved");
}

TSharedPtr<const FOnlinePartyJoinInfoEOSUnresolved> FOnlinePartyJoinInfoEOSUnresolved::GetEOSUnresolvedJoinInfo(
    const IOnlinePartyJoinInfo &PartyInfo)
{
    if (PartyInfo.GetSourcePlatform() == TEXT("EOSUnresolved"))
    {
        return StaticCastSharedRef<const FOnlinePartyJoinInfoEOSUnresolved>(PartyInfo.AsShared());
    }

    return nullptr;
}

bool FOnlinePartyJoinInfoEOSUnresolved::IsValid() const
{
    return true;
}

TSharedRef<const FOnlinePartyId> FOnlinePartyJoinInfoEOSUnresolved::GetPartyId() const
{
    return this->LobbyId.ToSharedRef();
}

FOnlinePartyTypeId FOnlinePartyJoinInfoEOSUnresolved::GetPartyTypeId() const
{
    return (FOnlinePartyTypeId)0;
}

TSharedRef<const FUniqueNetId> FOnlinePartyJoinInfoEOSUnresolved::GetSourceUserId() const
{
    return FUniqueNetIdEOS::InvalidId();
}

const FString &FOnlinePartyJoinInfoEOSUnresolved::GetSourceDisplayName() const
{
    return this->Empty;
}

const FString &FOnlinePartyJoinInfoEOSUnresolved::GetSourcePlatform() const
{
    return this->SourcePlatform;
}

const FString &FOnlinePartyJoinInfoEOSUnresolved::GetPlatformData() const
{
    return this->Empty;
}

bool FOnlinePartyJoinInfoEOSUnresolved::HasKey() const
{
    return false;
}

bool FOnlinePartyJoinInfoEOSUnresolved::HasPassword() const
{
    return false;
}

bool FOnlinePartyJoinInfoEOSUnresolved::IsAcceptingMembers() const
{
    return true;
}

bool FOnlinePartyJoinInfoEOSUnresolved::IsPartyOfOne() const
{
    return false;
}

int32 FOnlinePartyJoinInfoEOSUnresolved::GetNotAcceptingReason() const
{
    return 0;
}

const FString &FOnlinePartyJoinInfoEOSUnresolved::GetAppId() const
{
    return this->Empty;
}

const FString &FOnlinePartyJoinInfoEOSUnresolved::GetBuildId() const
{
    return this->Empty;
}

bool FOnlinePartyJoinInfoEOSUnresolved::CanJoin() const
{
    return true;
}

bool FOnlinePartyJoinInfoEOSUnresolved::CanJoinWithPassword() const
{
    return false;
}

bool FOnlinePartyJoinInfoEOSUnresolved::CanRequestAnInvite() const
{
    return false;
}

const TSharedRef<const FOnlinePartyId> FOnlinePartyEOS::ResolvePartyIdAndInit(EOS_HLobbyDetails InLobbyHandle)
{
    this->LobbyHandle = InLobbyHandle;

    EOS_LobbyDetails_CopyInfoOptions Opts = {};
    Opts.ApiVersion = EOS_LOBBYDETAILS_COPYINFO_API_LATEST;

    EOS_LobbyDetails_CopyInfo(this->LobbyHandle, &Opts, &this->LobbyInfo);

    return MakeShared<FOnlinePartyIdEOS>(this->LobbyInfo->LobbyId);
}

const FOnlinePartyTypeId FOnlinePartyEOS::ResolvePartyTypeId(EOS_HLobbyDetails InLobbyHandle)
{
    EOS_LobbyDetails_CopyAttributeByKeyOptions CopyOpts = {};
    CopyOpts.ApiVersion = EOS_LOBBYDETAILS_COPYATTRIBUTEBYKEY_API_LATEST;
    CopyOpts.AttrKey = PARTY_TYPE_ID_ANSI;

    EOS_Lobby_Attribute *Attr = nullptr;
    if (EOS_LobbyDetails_CopyAttributeByKey(InLobbyHandle, &CopyOpts, &Attr) != EOS_EResult::EOS_Success)
    {
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("Assuming primary party type ID for join info, since the PartyTypeId attribute was not found on "
                 "the lobby"));
        return FOnlinePartySystemEOS::GetPrimaryPartyTypeId();
    }

    if (Attr->Data->ValueType != EOS_ELobbyAttributeType::EOS_AT_INT64)
    {
        EOS_Lobby_Attribute_Release(Attr);
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("Assuming primary party type ID for join info, since the PartyTypeId attribute was not int64"));
        return FOnlinePartySystemEOS::GetPrimaryPartyTypeId();
    }

    auto Result = FOnlinePartyTypeId((const FOnlinePartyTypeId::TInternalType)Attr->Data->Value.AsInt64);
    EOS_Lobby_Attribute_Release(Attr);
    return Result;
}

// The ResolvePartyIdAndInit function initializes LobbyInfo and LobbyHandle, which the
// clang-tidy rule can't detect. We can't put default initializers for those variables
// in the initializer list, since those would run after the base class constructor is
// executed.
// NOLINTNEXTLINE(unreal-field-not-initialized-in-constructor)
FOnlinePartyEOS::FOnlinePartyEOS(
    EOS_HPlatform InPlatform,
    EOS_HLobbyDetails InLobbyHandle,
    TSharedRef<const FUniqueNetIdEOS> InEventSendingUserId,
    TWeakPtr<class FOnlinePartySystemEOS, ESPMode::ThreadSafe> InPartySystem,
    TWeakPtr<FEOSUserFactory, ESPMode::ThreadSafe> InUserFactory,
    bool bInRTCEnabled,
    const TSharedPtr<FPartyConfiguration> &PartyConfigurationFromCreate)
    : FOnlineParty(ResolvePartyIdAndInit(InLobbyHandle), ResolvePartyTypeId(InLobbyHandle))
    , EOSPlatform(InPlatform)
    , EOSLobby(EOS_Platform_GetLobbyInterface(InPlatform))
    , PartySystem(MoveTemp(InPartySystem))
    , UserFactory(MoveTemp(InUserFactory))
    , LocalUserId(MoveTemp(InEventSendingUserId))
    , bRTCEnabled(bInRTCEnabled)
    , bReadyForEvents(false)
    , Config(nullptr)
    , Members()
    , Attributes()
{
    check(this->EOSPlatform != nullptr);
    check(this->EOSLobby != nullptr);

    check(this->LocalUserId->GetType() == REDPOINT_EOS_SUBSYSTEM);

    EOS_LobbyDetails_GetMemberCountOptions CountOpts = {};
    CountOpts.ApiVersion = EOS_LOBBYDETAILS_GETMEMBERCOUNT_API_LATEST;
    uint32_t MemberCount = EOS_LobbyDetails_GetMemberCount(this->LobbyHandle, &CountOpts);

    TArray<TSharedRef<const FUniqueNetIdEOS>> MemberIds;
    for (uint32_t i = 0; i < MemberCount; i++)
    {
        EOS_LobbyDetails_GetMemberByIndexOptions MemOpts = {};
        MemOpts.ApiVersion = EOS_LOBBYDETAILS_GETMEMBERBYINDEX_API_LATEST;
        MemOpts.MemberIndex = i;
        auto UserId = EOS_LobbyDetails_GetMemberByIndex(this->LobbyHandle, &MemOpts);

        MemberIds.Add(MakeShared<FUniqueNetIdEOS>(UserId));
    }

    check(this->UserFactory.IsValid());
    TSharedPtr<FEOSUserFactory, ESPMode::ThreadSafe> Factory = this->UserFactory.Pin();
    TUserIdMap<TSharedPtr<FOnlinePartyMemberEOS>> UnresolvedMembers =
        FOnlinePartyMemberEOS::GetUnresolved(Factory.ToSharedRef(), this->LocalUserId, MemberIds);

    for (const auto &KV : UnresolvedMembers)
    {
        this->Members.Add(KV.Value);
        KV.Value->SetMemberConnectionStatus(EMemberConnectionStatus::Connected);

        // Read latest member attributes.
        KV.Value->PartyMemberAttributes = this->ReadMemberAttributes(*KV.Value->GetUserIdEOS());
    }

    // Set basic details.
    this->LeaderId = MakeShared<FUniqueNetIdEOS>(this->LobbyInfo->LobbyOwnerUserId);
    this->State = EPartyState::Active;
    this->PreviousState = EPartyState::CreatePending;
    this->RoomId = TEXT("");

    if (PartyConfigurationFromCreate.IsValid())
    {
        // Use the party configuration as the host created it.
        this->Config = PartyConfigurationFromCreate.ToSharedRef();
    }
    else
    {
        auto ConfigWritable = MakeShared<FPartyConfiguration>();
        ConfigWritable->PresencePermissions = PartySystemPermissions::EPermissionType::Anyone;
        ConfigWritable->InvitePermissions = PartySystemPermissions::EPermissionType::Leader;
        ConfigWritable->bIsAcceptingMembers = true;
        ConfigWritable->bChatEnabled = false;
        ConfigWritable->MaxMembers = this->LobbyInfo->MaxMembers;
        switch (this->LobbyInfo->PermissionLevel)
        {
        case EOS_ELobbyPermissionLevel::EOS_LPL_PUBLICADVERTISED:
            ConfigWritable->InvitePermissions = PartySystemPermissions::EPermissionType::Anyone;
            break;
        case EOS_ELobbyPermissionLevel::EOS_LPL_JOINVIAPRESENCE:
            ConfigWritable->InvitePermissions = PartySystemPermissions::EPermissionType::Friends;
            break;
        case EOS_ELobbyPermissionLevel::EOS_LPL_INVITEONLY:
            ConfigWritable->InvitePermissions = PartySystemPermissions::EPermissionType::Leader;
            break;
        }
#if EOS_VERSION_AT_LEAST(1, 11, 0)
        if (!this->LobbyInfo->bAllowInvites)
        {
            ConfigWritable->InvitePermissions = PartySystemPermissions::EPermissionType::Noone;
            ConfigWritable->bIsAcceptingMembers = false;
        }
#endif
#if EOS_VERSION_AT_LEAST(1, 13, 0) && defined(EOS_VOICE_CHAT_SUPPORTED)
        if (this->LobbyInfo->bRTCRoomEnabled)
        {
            ConfigWritable->bChatEnabled = true;
        }
#endif
        this->Config = ConfigWritable;
    }

    // Read latest lobby attributes.
    this->Attributes = this->ReadLobbyAttributes();
}

void FOnlinePartyEOS::UpdateWithLatestInfo()
{
    if (this->LobbyInfo != nullptr)
    {
        EOS_LobbyDetails_Info_Release(this->LobbyInfo);
        this->LobbyInfo = nullptr;
    }

    EOS_LobbyDetails_CopyInfoOptions Opts = {};
    Opts.ApiVersion = EOS_LOBBYDETAILS_COPYINFO_API_LATEST;
    EOS_LobbyDetails_CopyInfo(this->LobbyHandle, &Opts, &this->LobbyInfo);

    // ... and sync configuration.
    auto ConfigWritable = MakeShared<FPartyConfiguration>(*this->Config);
    ConfigWritable->PresencePermissions = PartySystemPermissions::EPermissionType::Anyone;
    ConfigWritable->InvitePermissions = PartySystemPermissions::EPermissionType::Leader;
    ConfigWritable->bIsAcceptingMembers = true;
    ConfigWritable->bChatEnabled = false;
    ConfigWritable->MaxMembers = this->LobbyInfo->MaxMembers;
    switch (this->LobbyInfo->PermissionLevel)
    {
    case EOS_ELobbyPermissionLevel::EOS_LPL_PUBLICADVERTISED:
        ConfigWritable->InvitePermissions = PartySystemPermissions::EPermissionType::Anyone;
        break;
    case EOS_ELobbyPermissionLevel::EOS_LPL_JOINVIAPRESENCE:
        ConfigWritable->InvitePermissions = PartySystemPermissions::EPermissionType::Friends;
        break;
    case EOS_ELobbyPermissionLevel::EOS_LPL_INVITEONLY:
        ConfigWritable->InvitePermissions = PartySystemPermissions::EPermissionType::Leader;
        break;
    }
#if EOS_VERSION_AT_LEAST(1, 11, 0)
    if (!this->LobbyInfo->bAllowInvites)
    {
        ConfigWritable->InvitePermissions = PartySystemPermissions::EPermissionType::Noone;
        ConfigWritable->bIsAcceptingMembers = false;
    }
#endif
#if EOS_VERSION_AT_LEAST(1, 13, 0) && defined(EOS_VOICE_CHAT_SUPPORTED)
    if (this->LobbyInfo->bRTCRoomEnabled)
    {
        ConfigWritable->bChatEnabled = true;
    }
#endif
    this->Config = ConfigWritable;
}

FOnlineKeyValuePairs<FString, FVariantData> FOnlinePartyEOS::ReadMemberAttributes(const FUniqueNetIdEOS &MemberId) const
{
    EOS_LobbyDetails_GetMemberAttributeCountOptions CountOpts = {};
    CountOpts.ApiVersion = EOS_LOBBYDETAILS_GETMEMBERATTRIBUTECOUNT_API_LATEST;
    CountOpts.TargetUserId = MemberId.GetProductUserId();
    auto AttributeCount = EOS_LobbyDetails_GetMemberAttributeCount(this->LobbyHandle, &CountOpts);
    FOnlineKeyValuePairs<FString, FVariantData> LatestAttributes;
    for (uint32_t i = 0; i < AttributeCount; i++)
    {
        EOS_LobbyDetails_CopyMemberAttributeByIndexOptions CopyOpts = {};
        CopyOpts.ApiVersion = EOS_LOBBYDETAILS_COPYATTRIBUTEBYINDEX_API_LATEST;
        CopyOpts.TargetUserId = MemberId.GetProductUserId();
        CopyOpts.AttrIndex = i;

        EOS_Lobby_Attribute *Attribute = nullptr;
        if (EOS_LobbyDetails_CopyMemberAttributeByIndex(this->LobbyHandle, &CopyOpts, &Attribute) ==
            EOS_EResult::EOS_Success)
        {
            if (Attribute->Data != nullptr)
            {
                if (Attribute->Data->ValueType == EOS_ELobbyAttributeType::EOS_AT_STRING)
                {
                    LatestAttributes.Add(
                        UTF8_TO_TCHAR(Attribute->Data->Key),
                        UTF8_TO_TCHAR(Attribute->Data->Value.AsUtf8));
                }
                if (Attribute->Data->ValueType == EOS_ELobbyAttributeType::EOS_AT_INT64)
                {
                    LatestAttributes.Add(UTF8_TO_TCHAR(Attribute->Data->Key), (int64)Attribute->Data->Value.AsInt64);
                }
                if (Attribute->Data->ValueType == EOS_ELobbyAttributeType::EOS_AT_DOUBLE)
                {
                    LatestAttributes.Add(UTF8_TO_TCHAR(Attribute->Data->Key), (double)Attribute->Data->Value.AsDouble);
                }
                if (Attribute->Data->ValueType == EOS_ELobbyAttributeType::EOS_AT_BOOLEAN)
                {
                    LatestAttributes.Add(
                        UTF8_TO_TCHAR(Attribute->Data->Key),
                        Attribute->Data->Value.AsBool == EOS_TRUE);
                }
            }
            EOS_Lobby_Attribute_Release(Attribute);
        }
    }
    return LatestAttributes;
}

FOnlineKeyValuePairs<FString, FVariantData> FOnlinePartyEOS::ReadLobbyAttributes() const
{
    // Retrieve the latest attributes.
    EOS_LobbyDetails_GetAttributeCountOptions CountOpts = {};
    CountOpts.ApiVersion = EOS_LOBBYDETAILS_GETATTRIBUTECOUNT_API_LATEST;
    auto AttributeCount = EOS_LobbyDetails_GetAttributeCount(this->LobbyHandle, &CountOpts);
    FOnlineKeyValuePairs<FString, FVariantData> LatestAttributes;
    for (uint32_t i = 0; i < AttributeCount; i++)
    {
        EOS_LobbyDetails_CopyAttributeByIndexOptions CopyOpts = {};
        CopyOpts.ApiVersion = EOS_LOBBYDETAILS_COPYATTRIBUTEBYINDEX_API_LATEST;
        CopyOpts.AttrIndex = i;

        EOS_Lobby_Attribute *Attribute = nullptr;
        if (EOS_LobbyDetails_CopyAttributeByIndex(this->LobbyHandle, &CopyOpts, &Attribute) == EOS_EResult::EOS_Success)
        {
            if (Attribute->Data != nullptr)
            {
                if (Attribute->Data->ValueType == EOS_ELobbyAttributeType::EOS_AT_STRING)
                {
                    LatestAttributes.Add(
                        UTF8_TO_TCHAR(Attribute->Data->Key),
                        UTF8_TO_TCHAR(Attribute->Data->Value.AsUtf8));
                }
                if (Attribute->Data->ValueType == EOS_ELobbyAttributeType::EOS_AT_INT64)
                {
                    LatestAttributes.Add(UTF8_TO_TCHAR(Attribute->Data->Key), (int64)Attribute->Data->Value.AsInt64);
                }
                if (Attribute->Data->ValueType == EOS_ELobbyAttributeType::EOS_AT_DOUBLE)
                {
                    LatestAttributes.Add(UTF8_TO_TCHAR(Attribute->Data->Key), (double)Attribute->Data->Value.AsDouble);
                }
                if (Attribute->Data->ValueType == EOS_ELobbyAttributeType::EOS_AT_BOOLEAN)
                {
                    LatestAttributes.Add(
                        UTF8_TO_TCHAR(Attribute->Data->Key),
                        Attribute->Data->Value.AsBool == EOS_TRUE);
                }
            }
            EOS_Lobby_Attribute_Release(Attribute);
        }
    }

    return LatestAttributes;
}

bool FOnlinePartyEOS::CanLocalUserInvite(const FUniqueNetId &) const
{
    return true;
}

bool FOnlinePartyEOS::IsJoinable() const
{
    return true;
}

TSharedRef<const FPartyConfiguration> FOnlinePartyEOS::GetConfiguration() const
{
    return this->Config.ToSharedRef();
}

bool FOnlinePartyEOS::IsRTCEnabled() const
{
    return this->bRTCEnabled;
}

FOnlinePartyEOS::~FOnlinePartyEOS()
{
    if (this->LobbyInfo != nullptr)
    {
        EOS_LobbyDetails_Info_Release(this->LobbyInfo);
    }

    EOS_LobbyDetails_Release(this->LobbyHandle);
}

FOnlinePartySystemEOS::FOnlinePartySystemEOS(
    EOS_HPlatform InPlatform,
    const TSharedRef<class FEOSConfig> &InConfig,
    const TSharedRef<FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> &InIdentity,
    const TSharedRef<IOnlineFriends, ESPMode::ThreadSafe> &InFriends,
    const TSharedRef<FEOSUserFactory, ESPMode::ThreadSafe> &InUserFactory
#if defined(EOS_VOICE_CHAT_SUPPORTED)
    ,
    const TSharedRef<class FEOSVoiceManager> &InVoiceManager
#endif
    )
    : EOSPlatform(InPlatform)
    , EOSConnect(EOS_Platform_GetConnectInterface(InPlatform))
    , EOSLobby(EOS_Platform_GetLobbyInterface(InPlatform))
    , EOSUI(EOS_Platform_GetUIInterface(InPlatform))
    , Config(InConfig)
    , Identity(InIdentity)
    , Friends(InFriends)
    , UserFactory(InUserFactory)
    ,
#if defined(EOS_VOICE_CHAT_SUPPORTED)
    VoiceManager(InVoiceManager)
    ,
#endif // #if defined(EOS_VOICE_CHAT_SUPPORTED)
    EventDeduplication()
    , JoinedParties()
    , PendingInvites()
    , SyntheticPartySessionManager()
    , CreateJoinMutex()
    , Unregister_JoinLobbyAccepted()
    , Unregister_LobbyInviteAccepted()
    , Unregister_LobbyInviteReceived()
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    , Unregister_LobbyInviteRejected()
#endif
    , Unregister_LobbyMemberStatusReceived()
    , Unregister_LobbyMemberUpdateReceived()
    , Unregister_LobbyUpdateReceived()
{
    check(this->EOSPlatform != nullptr);
    check(this->EOSConnect != nullptr);
    check(this->EOSLobby != nullptr);
    check(this->EOSUI != nullptr);
}

void FOnlinePartySystemEOS::RegisterEvents()
{
    EOS_Lobby_AddNotifyJoinLobbyAcceptedOptions OptsJoinLobbyAccepted = {};
    OptsJoinLobbyAccepted.ApiVersion = EOS_LOBBY_ADDNOTIFYJOINLOBBYACCEPTED_API_LATEST;
    EOS_Lobby_AddNotifyLobbyInviteAcceptedOptions OptsLobbyInviteAccepted = {};
    OptsLobbyInviteAccepted.ApiVersion = EOS_LOBBY_ADDNOTIFYLOBBYINVITEACCEPTED_API_LATEST;
    EOS_Lobby_AddNotifyLobbyInviteReceivedOptions OptsLobbyInviteReceived = {};
    OptsLobbyInviteReceived.ApiVersion = EOS_LOBBY_ADDNOTIFYLOBBYINVITERECEIVED_API_LATEST;
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    EOS_Lobby_AddNotifyLobbyInviteRejectedOptions OptsLobbyInviteRejected = {};
    OptsLobbyInviteRejected.ApiVersion = EOS_LOBBY_ADDNOTIFYLOBBYINVITEREJECTED_API_LATEST;
#endif
    EOS_Lobby_AddNotifyLobbyMemberStatusReceivedOptions OptsLobbyMemberStatus = {};
    OptsLobbyMemberStatus.ApiVersion = EOS_LOBBY_ADDNOTIFYLOBBYMEMBERSTATUSRECEIVED_API_LATEST;
    EOS_Lobby_AddNotifyLobbyMemberUpdateReceivedOptions OptsLobbyMemberUpdate = {};
    OptsLobbyMemberUpdate.ApiVersion = EOS_LOBBY_ADDNOTIFYLOBBYMEMBERUPDATERECEIVED_API_LATEST;
    EOS_Lobby_AddNotifyLobbyUpdateReceivedOptions OptsLobbyUpdate = {};
    OptsLobbyUpdate.ApiVersion = EOS_LOBBY_ADDNOTIFYLOBBYUPDATERECEIVED_API_LATEST;

    this->Unregister_JoinLobbyAccepted = EOSRegisterEvent<
        EOS_HLobby,
        EOS_Lobby_AddNotifyJoinLobbyAcceptedOptions,
        EOS_Lobby_JoinLobbyAcceptedCallbackInfo>(
        this->EOSLobby,
        &OptsJoinLobbyAccepted,
        EOS_Lobby_AddNotifyJoinLobbyAccepted,
        EOS_Lobby_RemoveNotifyJoinLobbyAccepted,
        [WeakThis = GetWeakThis(this)](const EOS_Lobby_JoinLobbyAcceptedCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                This->Handle_JoinLobbyAccepted(Data);
            }
        });
    this->Unregister_LobbyInviteAccepted = EOSRegisterEvent<
        EOS_HLobby,
        EOS_Lobby_AddNotifyLobbyInviteAcceptedOptions,
        EOS_Lobby_LobbyInviteAcceptedCallbackInfo>(
        this->EOSLobby,
        &OptsLobbyInviteAccepted,
        EOS_Lobby_AddNotifyLobbyInviteAccepted,
        EOS_Lobby_RemoveNotifyLobbyInviteAccepted,
        [WeakThis = GetWeakThis(this)](const EOS_Lobby_LobbyInviteAcceptedCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                This->Handle_LobbyInviteAccepted(Data);
            }
        });
    this->Unregister_LobbyInviteReceived = EOSRegisterEvent<
        EOS_HLobby,
        EOS_Lobby_AddNotifyLobbyInviteReceivedOptions,
        EOS_Lobby_LobbyInviteReceivedCallbackInfo>(
        this->EOSLobby,
        &OptsLobbyInviteReceived,
        EOS_Lobby_AddNotifyLobbyInviteReceived,
        EOS_Lobby_RemoveNotifyLobbyInviteReceived,
        [WeakThis = GetWeakThis(this)](const EOS_Lobby_LobbyInviteReceivedCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                This->Handle_LobbyInviteReceived(Data);
            }
        });
#if EOS_VERSION_AT_LEAST(1, 15, 0)
    this->Unregister_LobbyInviteRejected = EOSRegisterEvent<
        EOS_HLobby,
        EOS_Lobby_AddNotifyLobbyInviteRejectedOptions,
        EOS_Lobby_LobbyInviteRejectedCallbackInfo>(
        this->EOSLobby,
        &OptsLobbyInviteRejected,
        EOS_Lobby_AddNotifyLobbyInviteRejected,
        EOS_Lobby_RemoveNotifyLobbyInviteRejected,
        [WeakThis = GetWeakThis(this)](const EOS_Lobby_LobbyInviteRejectedCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                This->Handle_LobbyInviteRejected(Data);
            }
        });
#endif
    this->Unregister_LobbyMemberStatusReceived = EOSRegisterEvent<
        EOS_HLobby,
        EOS_Lobby_AddNotifyLobbyMemberStatusReceivedOptions,
        EOS_Lobby_LobbyMemberStatusReceivedCallbackInfo>(
        this->EOSLobby,
        &OptsLobbyMemberStatus,
        EOS_Lobby_AddNotifyLobbyMemberStatusReceived,
        EOS_Lobby_RemoveNotifyLobbyMemberStatusReceived,
        [WeakThis = GetWeakThis(this)](const EOS_Lobby_LobbyMemberStatusReceivedCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                This->Handle_LobbyMemberStatusReceived(Data);
            }
        });
    this->Unregister_LobbyMemberUpdateReceived = EOSRegisterEvent<
        EOS_HLobby,
        EOS_Lobby_AddNotifyLobbyMemberUpdateReceivedOptions,
        EOS_Lobby_LobbyMemberUpdateReceivedCallbackInfo>(
        this->EOSLobby,
        &OptsLobbyMemberUpdate,
        EOS_Lobby_AddNotifyLobbyMemberUpdateReceived,
        EOS_Lobby_RemoveNotifyLobbyMemberUpdateReceived,
        [WeakThis = GetWeakThis(this)](const EOS_Lobby_LobbyMemberUpdateReceivedCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                This->Handle_LobbyMemberUpdateReceived(Data);
            }
        });
    this->Unregister_LobbyUpdateReceived = EOSRegisterEvent<
        EOS_HLobby,
        EOS_Lobby_AddNotifyLobbyUpdateReceivedOptions,
        EOS_Lobby_LobbyUpdateReceivedCallbackInfo>(
        this->EOSLobby,
        &OptsLobbyUpdate,
        EOS_Lobby_AddNotifyLobbyUpdateReceived,
        EOS_Lobby_RemoveNotifyLobbyUpdateReceived,
        [WeakThis = GetWeakThis(this)](const EOS_Lobby_LobbyUpdateReceivedCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                This->Handle_LobbyUpdateReceived(Data);
            }
        });
}

void FOnlinePartySystemEOS::Tick()
{
    this->EventDeduplication.Empty();
}

void FOnlinePartySystemEOS::Handle_JoinLobbyAccepted(const EOS_Lobby_JoinLobbyAcceptedCallbackInfo *Data)
{
    // We have joined a party directly via the social overlay.

    UE_LOG(LogEOS, Verbose, TEXT("Received JoinLobbyAccepted event from EOS Lobbies system"));

    EOS_HLobbyDetails LobbyHandle = {};

    EOS_Lobby_CopyLobbyDetailsHandleByUiEventIdOptions CopyOpts = {};
    CopyOpts.ApiVersion = EOS_LOBBY_COPYLOBBYDETAILSHANDLEBYUIEVENTID_API_LATEST;
    CopyOpts.UiEventId = Data->UiEventId;
    auto Result = EOS_Lobby_CopyLobbyDetailsHandleByUiEventId(this->EOSLobby, &CopyOpts, &LobbyHandle);
    if (Result != EOS_EResult::EOS_Success)
    {
        EOS_UI_AcknowledgeEventIdOptions AckOpts = {};
        AckOpts.ApiVersion = EOS_UI_ACKNOWLEDGECORRELATIONID_API_LATEST;
        AckOpts.Result = Result;
        AckOpts.UiEventId = Data->UiEventId;
        EOS_UI_AcknowledgeEventId(this->EOSUI, &AckOpts);
        return;
    }

    // Got handle, now pass it into JoinLobby and then propagate the result of the join
    // lobby operation back to the UI.
    auto LocalUser = MakeShared<FUniqueNetIdEOS>(Data->LocalUserId);
    auto JoinInfo = MakeShared<FOnlinePartyJoinInfoEOS>(LobbyHandle);
    this->JoinParty(
        LocalUser.Get(),
        JoinInfo.Get(),
        FOnJoinPartyComplete::CreateLambda([WeakThis = GetWeakThis(this), Data](
                                               const FUniqueNetId &LocalUserId,
                                               const FOnlinePartyId &PartyId,
                                               const EJoinPartyCompletionResult Result,
                                               const int32 NotApprovedReason) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Result != EJoinPartyCompletionResult::Succeeded)
                {
                    EOS_UI_AcknowledgeEventIdOptions AckOpts = {};
                    AckOpts.ApiVersion = EOS_UI_ACKNOWLEDGECORRELATIONID_API_LATEST;
                    AckOpts.Result = EOS_EResult::EOS_UnexpectedError;
                    AckOpts.UiEventId = Data->UiEventId;
                    EOS_UI_AcknowledgeEventId(This->EOSUI, &AckOpts);
                    return;
                }

                {
                    EOS_UI_AcknowledgeEventIdOptions AckOpts = {};
                    AckOpts.ApiVersion = EOS_UI_ACKNOWLEDGECORRELATIONID_API_LATEST;
                    AckOpts.Result = EOS_EResult::EOS_Success;
                    AckOpts.UiEventId = Data->UiEventId;
                    EOS_UI_AcknowledgeEventId(This->EOSUI, &AckOpts);
                    return;
                }
            }
        }));
}

void FOnlinePartySystemEOS::Handle_LobbyInviteAccepted(const EOS_Lobby_LobbyInviteAcceptedCallbackInfo *Data)
{
    // We have accepted an invite from another user via the social overlay.

    UE_LOG(
        LogEOS,
        Verbose,
        TEXT("Received LobbyInviteAccepted event from EOS Lobbies system: %s"),
        ANSI_TO_TCHAR(Data->InviteId));

    EOS_HLobbyDetails Handle = {};
    EOS_Lobby_CopyLobbyDetailsHandleByInviteIdOptions Opts = {};
    Opts.ApiVersion = EOS_LOBBY_COPYLOBBYDETAILSHANDLEBYINVITEID_API_LATEST;
    Opts.InviteId = Data->InviteId;
    if (EOS_Lobby_CopyLobbyDetailsHandleByInviteId(this->EOSLobby, &Opts, &Handle) != EOS_EResult::EOS_Success)
    {
        UE_LOG(LogEOS, Error, TEXT("Failed to retrieve lobby details from invite ID during LobbyInviteReceived"));
        return;
    }

    auto JoinInfo = MakeShared<FOnlinePartyJoinInfoEOS>(Handle);

    TSharedPtr<FUniqueNetIdEOS> TargetUserEOS = MakeShared<FUniqueNetIdEOS>(Data->TargetUserId);
    TSharedPtr<FUniqueNetIdEOS> LocalUserEOS = MakeShared<FUniqueNetIdEOS>(Data->LocalUserId);

    // Try to perform the join.

    this->JoinParty(
        LocalUserEOS.ToSharedRef().Get(),
        JoinInfo.Get(),
        FOnJoinPartyComplete::CreateLambda([WeakThis = GetWeakThis(this), LocalUserEOS, JoinInfo, TargetUserEOS](
                                               const FUniqueNetId &LocalUserId,
                                               const FOnlinePartyId &PartyId,
                                               const EJoinPartyCompletionResult Result,
                                               const int32 NotApprovedReason) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Result != EJoinPartyCompletionResult::Succeeded)
                {
                    UE_LOG(LogEOS, Error, TEXT("Failed to join lobby that was accepted through the UI"));
                    return;
                }

                // Remove any pending invites that match, since the user accepted through the overlay.

                if (!This->PendingInvites.Contains(*LocalUserEOS))
                {
                    This->PendingInvites.Add(*LocalUserEOS, TArray<IOnlinePartyJoinInfoConstRef>());
                }
                TArray<IOnlinePartyJoinInfoConstRef> JoinInfosToRemove;
                for (const auto &CandidateJoinInfo : This->PendingInvites[*LocalUserEOS])
                {
                    if (CandidateJoinInfo->GetPartyId().Get() == JoinInfo->GetPartyId().Get())
                    {
                        JoinInfosToRemove.Add(CandidateJoinInfo);
                    }
                }
                for (const auto &RemoveJoinInfo : JoinInfosToRemove)
                {
                    This->PendingInvites[*LocalUserEOS].Remove(RemoveJoinInfo);
                }

#if defined(UE_5_1_OR_LATER)
                This->TriggerOnPartyInviteRemovedExDelegates(
                    LocalUserEOS.ToSharedRef().Get(),
                    JoinInfo.Get(),
                    EPartyInvitationRemovedReason::Accepted);
#else
                This->TriggerOnPartyInviteRemovedDelegates(
                    LocalUserEOS.ToSharedRef().Get(),
                    JoinInfo->GetPartyId().Get(),
                    TargetUserEOS.ToSharedRef().Get(),
                    EPartyInvitationRemovedReason::Accepted);
#endif
                This->TriggerOnPartyInvitesChangedDelegates(LocalUserEOS.ToSharedRef().Get());
            }
        }));
}

void FOnlinePartySystemEOS::Handle_LobbyInviteReceived(const EOS_Lobby_LobbyInviteReceivedCallbackInfo *Data)
{
    // We have received an invite from another user.

    UE_LOG(
        LogEOS,
        Verbose,
        TEXT("Received LobbyInviteReceived event from EOS Lobbies system: %s"),
        ANSI_TO_TCHAR(Data->InviteId));

    FString InviteIdStr = ANSI_TO_TCHAR(Data->InviteId);

    EOS_HLobbyDetails Handle = {};
    EOS_Lobby_CopyLobbyDetailsHandleByInviteIdOptions Opts = {};
    Opts.ApiVersion = EOS_LOBBY_COPYLOBBYDETAILSHANDLEBYINVITEID_API_LATEST;
    Opts.InviteId = Data->InviteId;
    if (EOS_Lobby_CopyLobbyDetailsHandleByInviteId(this->EOSLobby, &Opts, &Handle) != EOS_EResult::EOS_Success)
    {
        UE_LOG(LogEOS, Error, TEXT("Failed to retrieve lobby details from invite ID during LobbyInviteReceived"));
        return;
    }

    // Look up the sender's display name.
    EOS_Connect_QueryProductUserIdMappingsOptions DNOpts = {};
    DNOpts.ApiVersion = EOS_CONNECT_QUERYPRODUCTUSERIDMAPPINGS_API_LATEST;
    DNOpts.LocalUserId = Data->LocalUserId;
    DNOpts.ProductUserIds = (EOS_ProductUserId *)&Data->TargetUserId;
    DNOpts.ProductUserIdCount = 1;

    TSharedPtr<FUniqueNetIdEOS> TargetUserEOS = MakeShared<FUniqueNetIdEOS>(Data->TargetUserId);
    TSharedPtr<FUniqueNetIdEOS> LocalUserEOS = MakeShared<FUniqueNetIdEOS>(Data->LocalUserId);

    EOSRunOperation<
        EOS_HConnect,
        EOS_Connect_QueryProductUserIdMappingsOptions,
        EOS_Connect_QueryProductUserIdMappingsCallbackInfo>(
        this->EOSConnect,
        &DNOpts,
        EOS_Connect_QueryProductUserIdMappings,
        [WeakThis = GetWeakThis(this), TargetUserEOS, Handle, LocalUserEOS, InviteIdStr](
            const EOS_Connect_QueryProductUserIdMappingsCallbackInfo *Info) {
            if (auto This = PinWeakThis(WeakThis))
            {
                // Fetch the sender name if possible.
                FString SenderDisplayName = TEXT("");
                if (Info->ResultCode == EOS_EResult::EOS_Success)
                {
                    EOS_Connect_CopyProductUserInfoOptions CopyOpts = {};
                    CopyOpts.ApiVersion = EOS_CONNECT_COPYPRODUCTUSERINFO_API_LATEST;
                    CopyOpts.TargetUserId = TargetUserEOS->GetProductUserId();

                    EOS_Connect_ExternalAccountInfo *ExternalAccountInfo = nullptr;
                    auto Result = EOS_Connect_CopyProductUserInfo(This->EOSConnect, &CopyOpts, &ExternalAccountInfo);
                    if (Result == EOS_EResult::EOS_Success)
                    {
                        SenderDisplayName = FString(ANSI_TO_TCHAR(ExternalAccountInfo->DisplayName));
                    }
                    EOS_Connect_ExternalAccountInfo_Release(ExternalAccountInfo);
                }

                // Get the lobby ID out of the handle so we can check if the user is already in it.
                EOS_LobbyDetails_Info *LobbyInfo = nullptr;
                EOS_LobbyDetails_CopyInfoOptions CopyInfoOpts = {};
                CopyInfoOpts.ApiVersion = EOS_LOBBYDETAILS_COPYINFO_API_LATEST;
                verify(EOS_LobbyDetails_CopyInfo(Handle, &CopyInfoOpts, &LobbyInfo) == EOS_EResult::EOS_Success);
                auto LobbyIdEOS = MakeShared<FOnlinePartyIdEOS>(LobbyInfo->LobbyId);
                EOS_LobbyDetails_Info_Release(LobbyInfo);

                // Ensure this user has a list of pending invites so we can add to it.
                if (!This->PendingInvites.Contains(*LocalUserEOS))
                {
                    This->PendingInvites.Add(*LocalUserEOS, TArray<IOnlinePartyJoinInfoConstRef>());
                }

                // If the user already has an invite to this party, ignore this
                // invite (this is possible if the game calls RestoreInvites).
                bool bInviteFound = false;
                for (const auto &Invite : This->PendingInvites[*LocalUserEOS])
                {
                    auto InviteEOS = StaticCastSharedRef<const FOnlinePartyJoinInfoEOS>(Invite);
                    if (*InviteEOS->GetPartyId() == *LobbyIdEOS)
                    {
                        bInviteFound = true;
                        break;
                    }
                }
                if (bInviteFound)
                {
                    // User already has an invite, don't add.
                    return;
                }

                // Create a join info.
                auto JoinInfo =
                    MakeShared<FOnlinePartyJoinInfoEOS>(Handle, TargetUserEOS, SenderDisplayName, InviteIdStr);

                // If the local user has already joined the party, there's no need
                // to add this invite.
                if (This->IsLocalUserInParty(*LobbyIdEOS, *LocalUserEOS))
                {
                    return;
                }

                // Add to the list of pending invites.
                checkf(
                    !JoinInfo->GetInviteId().IsEmpty(),
                    TEXT("InviteId must not be empty for invite added to PendingInvites array."));
                This->PendingInvites[*LocalUserEOS].Add(JoinInfo);
#if defined(UE_5_1_OR_LATER)
                This->TriggerOnPartyInviteReceivedExDelegates(LocalUserEOS.ToSharedRef().Get(), JoinInfo.Get());
#else
                This->TriggerOnPartyInviteReceivedDelegates(
                    LocalUserEOS.ToSharedRef().Get(),
                    JoinInfo->GetPartyId().Get(),
                    TargetUserEOS.ToSharedRef().Get());
#endif
                This->TriggerOnPartyInvitesChangedDelegates(LocalUserEOS.ToSharedRef().Get());
            }
        });
}

#if EOS_VERSION_AT_LEAST(1, 15, 0)
void FOnlinePartySystemEOS::Handle_LobbyInviteRejected(const EOS_Lobby_LobbyInviteRejectedCallbackInfo *Data)
{
    // We have rejected an invite from another user via the social overlay.

    UE_LOG(
        LogEOS,
        Verbose,
        TEXT("Received LobbyInviteRejected event from EOS Lobbies system: %s"),
        ANSI_TO_TCHAR(Data->InviteId));

    TSharedRef<const FOnlinePartyIdEOS> LobbyId = MakeShared<FOnlinePartyIdEOS>(Data->LobbyId);
    TSharedRef<const FUniqueNetIdEOS> EOSUser = MakeShared<FUniqueNetIdEOS>(Data->LocalUserId);

    if (!this->PendingInvites.Contains(*EOSUser))
    {
        return;
    }

    TArray<IOnlinePartyJoinInfoConstRef> InvitesToRemove;
    for (const auto &PendingInvite : this->PendingInvites[*EOSUser])
    {
        if (*PendingInvite->GetPartyId() == *LobbyId)
        {
            InvitesToRemove.Add(PendingInvite);
        }
    }
    bool bInvitesChanged = false;
    for (const auto &PendingInvite : InvitesToRemove)
    {
        if (this->PendingInvites[*EOSUser].Remove(PendingInvite) > 0)
        {
            bInvitesChanged = true;
#if defined(UE_5_1_OR_LATER)
            this->TriggerOnPartyInviteRemovedExDelegates(
                *EOSUser,
                PendingInvite.Get(),
                EPartyInvitationRemovedReason::Declined);
#else
            this->TriggerOnPartyInviteRemovedDelegates(
                *EOSUser,
                *PendingInvite->GetPartyId(),
                *PendingInvite->GetSourceUserId(),
                EPartyInvitationRemovedReason::Declined);
#endif
        }
    }
    if (bInvitesChanged)
    {
        this->TriggerOnPartyInvitesChangedDelegates(*EOSUser);
    }
}
#endif

void FOnlinePartySystemEOS::Handle_LobbyMemberStatusReceived(
    const EOS_Lobby_LobbyMemberStatusReceivedCallbackInfo *Data)
{
    this->MemberStatusChanged(Data->LobbyId, Data->TargetUserId, Data->CurrentStatus);
}

void FOnlinePartySystemEOS::Handle_LobbyMemberUpdateReceived(
    const EOS_Lobby_LobbyMemberUpdateReceivedCallbackInfo *Data)
{
    auto TargetUser = MakeShared<FUniqueNetIdEOS>(Data->TargetUserId);
    auto PartyId = MakeShared<FOnlinePartyIdEOS>(Data->LobbyId);

    FOnlinePartyRecordedEvent Ev(
        EOnlinePartyRecordedEventType::LobbyMemberUpdateReceived,
        TargetUser,
        PartyId,
        EOS_ELobbyMemberStatus::EOS_LMS_LEFT);
    if (this->EventDeduplication.Contains(Ev))
    {
        return;
    }
    this->EventDeduplication.Add(Ev);

    UE_LOG(
        LogEOS,
        Verbose,
        TEXT("PartyInterface: LobbyMemberUpdateReceived: (Lobby) %s (Member) %s"),
        *PartyId->ToString(),
        *TargetUser->ToString());

    for (const auto &KV : TUserIdMap<TArray<TSharedPtr<FOnlinePartyEOS>>>(this->JoinedParties))
    {
        const auto &OwnerUserId = KV.Key;

        for (const auto &Party : TArray<TSharedPtr<FOnlinePartyEOS>>(KV.Value))
        {
            if (*PartyId != *Party->PartyId)
            {
                continue;
            }

            checkf(
                *OwnerUserId == *Party->LocalUserId,
                TEXT("Expected key of JoinedParties map to match the party's LocalUserId."));

            for (const auto &Member : Party->Members)
            {
                if (*Member->GetUserIdEOS() == *TargetUser)
                {
                    // Read and apply new attributes.
                    FOnlineKeyValuePairs<FString, FVariantData> OldAttributes = Member->PartyMemberAttributes;
                    Member->PartyMemberAttributes = Party->ReadMemberAttributes(*TargetUser);

                    // Fire events.
                    for (const auto &AttrKV : Member->PartyMemberAttributes)
                    {
                        if (!OldAttributes.Contains(AttrKV.Key))
                        {
                            Member->OnMemberAttributeChanged()
                                .Broadcast(*TargetUser, AttrKV.Key, AttrKV.Value.ToString(), TEXT(""));
                        }
                        else if (OldAttributes[AttrKV.Key] != AttrKV.Value)
                        {
                            Member->OnMemberAttributeChanged().Broadcast(
                                *TargetUser,
                                AttrKV.Key,
                                AttrKV.Value.ToString(),
                                OldAttributes[AttrKV.Key].ToString());
                        }
                    }
                    for (const auto &AttrKV : OldAttributes)
                    {
                        if (!Member->PartyMemberAttributes.Contains(AttrKV.Key))
                        {
                            Member->OnMemberAttributeChanged()
                                .Broadcast(*TargetUser, AttrKV.Key, TEXT(""), OldAttributes[AttrKV.Key].ToString());
                        }
                    }

                    TSharedPtr<FOnlinePartyData> PartyData = MakeShared<FOnlinePartyData>();
                    PartyData->GetKeyValAttrs() = Member->PartyMemberAttributes;
                    if (Party->bReadyForEvents)
                    {
                        this->TriggerOnPartyMemberDataReceivedDelegates(
                            *OwnerUserId,
                            *Party->PartyId,
                            *TargetUser,
                            DefaultPartyDataNamespace,
                            *PartyData);
                    }
                }
            }
        }
    }
}

void FOnlinePartySystemEOS::Handle_LobbyUpdateReceived(const EOS_Lobby_LobbyUpdateReceivedCallbackInfo *Data)
{
    UE_LOG(LogEOS, Verbose, TEXT("PartyInterface: LobbyUpdateReceived: (Lobby) %s"), ANSI_TO_TCHAR(Data->LobbyId));

    for (const auto &KV : TUserIdMap<TArray<TSharedPtr<FOnlinePartyEOS>>>(this->JoinedParties))
    {
        const auto &OwnerUserId = KV.Key;

        for (const auto &Party : TArray<TSharedPtr<FOnlinePartyEOS>>(KV.Value))
        {
            if (FString(ANSI_TO_TCHAR(Data->LobbyId)) != FString(ANSI_TO_TCHAR(Party->LobbyInfo->LobbyId)))
            {
                continue;
            }

            checkf(
                *OwnerUserId == *Party->LocalUserId,
                TEXT("Expected key of JoinedParties map to match the party's LocalUserId."));

            // Update with latest attributes.
            Party->Attributes = Party->ReadLobbyAttributes();

            // Update with the latest configuration.
            Party->UpdateWithLatestInfo();

            // Create or delete the synthetic party if required (when we are a client in a party).
            bool bShouldHaveSyntheticParty =
                this->ShouldHaveSyntheticParty(Party->PartyTypeId, *Party->GetConfiguration());
            if (TSharedPtr<FSyntheticPartySessionManager> SPM = this->SyntheticPartySessionManager.Pin())
            {
                bool bHasSyntheticParty = SPM->HasSyntheticParty(Party->PartyId);
                if (bShouldHaveSyntheticParty && !bHasSyntheticParty)
                {
                    SPM->CreateSyntheticParty(
                        Party->PartyId,
                        FSyntheticPartySessionOnComplete::CreateLambda(
                            [LobbyIdStr = (FString)(ANSI_TO_TCHAR(Data->LobbyId))](const FOnlineError &Error) {
                                if (!Error.bSucceeded)
                                {
                                    UE_LOG(
                                        LogEOS,
                                        Error,
                                        TEXT("PartyInterface: LobbyUpdateReceived: (Lobby) %s: %s"),
                                        *LobbyIdStr,
                                        *Error.ToLogString());
                                }
                            }));
                }
                else if (!bShouldHaveSyntheticParty && bHasSyntheticParty)
                {
                    SPM->DeleteSyntheticParty(
                        Party->PartyId,
                        FSyntheticPartySessionOnComplete::CreateLambda(
                            [LobbyIdStr = (FString)(ANSI_TO_TCHAR(Data->LobbyId))](const FOnlineError &Error) {
                                if (!Error.bSucceeded)
                                {
                                    UE_LOG(
                                        LogEOS,
                                        Error,
                                        TEXT("PartyInterface: LobbyUpdateReceived: (Lobby) %s: %s"),
                                        *LobbyIdStr,
                                        *Error.ToLogString());
                                }
                            }));
                }
            }

            // Fire events.
            TSharedPtr<FOnlinePartyData> PartyData = MakeShared<FOnlinePartyData>();
            PartyData->GetKeyValAttrs() = Party->Attributes;
            if (Party->bReadyForEvents)
            {
                this->TriggerOnPartyDataReceivedDelegates(
                    *OwnerUserId,
                    *Party->PartyId,
                    DefaultPartyDataNamespace,
                    *PartyData);
                this->TriggerOnPartyConfigChangedDelegates(*OwnerUserId, *Party->PartyId, *Party->GetConfiguration());
            }
        }
    }
}

void FOnlinePartySystemEOS::MemberStatusChanged(
    EOS_LobbyId InLobbyId,
    EOS_ProductUserId InTargetUserId,
    EOS_ELobbyMemberStatus InCurrentStatus)
{
    auto TargetUser = MakeShared<FUniqueNetIdEOS>(InTargetUserId);
    auto PartyId = MakeShared<FOnlinePartyIdEOS>(InLobbyId);

    FOnlinePartyRecordedEvent Ev(
        EOnlinePartyRecordedEventType::LobbyMemberStatusReceived,
        TargetUser,
        PartyId,
        InCurrentStatus);
    if (this->EventDeduplication.Contains(Ev))
    {
        return;
    }
    this->EventDeduplication.Add(Ev);

    FString NewStatus = TEXT("Unknown");
    switch (InCurrentStatus)
    {
    case EOS_ELobbyMemberStatus::EOS_LMS_CLOSED:
        NewStatus = TEXT("Closed");
        break;
    case EOS_ELobbyMemberStatus::EOS_LMS_DISCONNECTED:
        NewStatus = TEXT("Disconnected");
        break;
    case EOS_ELobbyMemberStatus::EOS_LMS_JOINED:
        NewStatus = TEXT("Joined");
        break;
    case EOS_ELobbyMemberStatus::EOS_LMS_KICKED:
        NewStatus = TEXT("Kicked");
        break;
    case EOS_ELobbyMemberStatus::EOS_LMS_LEFT:
        NewStatus = TEXT("Left");
        break;
    case EOS_ELobbyMemberStatus::EOS_LMS_PROMOTED:
        NewStatus = TEXT("Promoted");
        break;
    }
    UE_LOG(
        LogEOS,
        Verbose,
        TEXT("PartyInterface: LobbyMemberStatusReceived: (Lobby) %s (Member) %s (Status) %s"),
        *PartyId->ToString(),
        *TargetUser->ToString(),
        *NewStatus);

    for (const auto &KV : TUserIdMap<TArray<TSharedPtr<FOnlinePartyEOS>>>(this->JoinedParties))
    {
        const auto &OwnerUserId = KV.Key;

        for (const auto &Party : TArray<TSharedPtr<FOnlinePartyEOS>>(KV.Value))
        {
            if (*PartyId != *Party->PartyId)
            {
                continue;
            }

            checkf(
                *OwnerUserId == *Party->LocalUserId,
                TEXT("Expected key of JoinedParties map to match the party's LocalUserId."));

            switch (InCurrentStatus)
            {
            case EOS_ELobbyMemberStatus::EOS_LMS_JOINED: {
                for (const auto &ExistingMember : Party->Members)
                {
                    if (*ExistingMember->GetUserId() == *TargetUser)
                    {
                        // User is already in party, ignore this event. Sometimes EOS even sends join events
                        // over multiple frames, which our event deduplication won't catch.
                        UE_LOG(
                            LogEOS,
                            Verbose,
                            TEXT("Detected EOS sent a duplicate EOS_LMS_JOINED event for user %s in party %s. Ignoring "
                                 "it."),
                            *TargetUser->ToString(),
                            *PartyId->ToString());
                        return;
                    }
                }

                TArray<TSharedRef<const FUniqueNetIdEOS>> MemberIds;
                MemberIds.Add(TargetUser);
                TSharedPtr<FOnlinePartyMemberEOS> NewMember = FOnlinePartyMemberEOS::GetUnresolved(
                    this->UserFactory.ToSharedRef(),
                    StaticCastSharedRef<const FUniqueNetIdEOS>(OwnerUserId),
                    MemberIds)[*TargetUser];

                NewMember->PartyMemberAttributes = Party->ReadMemberAttributes(*TargetUser);
                Party->Members.Add(NewMember);
                NewMember->SetMemberConnectionStatus(EMemberConnectionStatus::Connected);
                if (Party->bReadyForEvents)
                {
                    this->TriggerOnPartyMemberJoinedDelegates(*OwnerUserId, *Party->PartyId, *TargetUser);
                }
                break;
            }
            case EOS_ELobbyMemberStatus::EOS_LMS_LEFT:
            case EOS_ELobbyMemberStatus::EOS_LMS_DISCONNECTED:
            case EOS_ELobbyMemberStatus::EOS_LMS_KICKED:
            case EOS_ELobbyMemberStatus::EOS_LMS_CLOSED: {
                TSharedPtr<FOnlinePartyMemberEOS> FoundMember = nullptr;
                for (const auto &Member : Party->Members)
                {
                    if (*Member->GetUserIdEOS() == *TargetUser)
                    {
                        FoundMember = Member;
                        break;
                    }
                }
                if (FoundMember == nullptr)
                {
                    // Ignore.
                    break;
                }
                Party->Members.Remove(FoundMember);
                FoundMember->SetMemberConnectionStatus(EMemberConnectionStatus::Disconnected);

                // Once the member entry is gone, we must also remove the party from the user's "joined parties" list if
                // they're the local user. This ensures that GetJoinedParties and GetPartyMember have a consistent state
                // when the events are firing.
                if (*OwnerUserId == *FoundMember->GetUserId())
                {
                    auto &LocalUsersParties = this->JoinedParties[*OwnerUserId];
                    for (int i = LocalUsersParties.Num() - 1; i >= 0; i--)
                    {
                        if (*LocalUsersParties[i]->PartyId == *Party->PartyId)
                        {
                            UE_LOG(
                                LogEOS,
                                Verbose,
                                TEXT("Handle_LobbyMemberStatusReceived: Removing party %s at index %d because it "
                                     "has been left"),
                                *Party->PartyId->ToString(),
                                i);
                            LocalUsersParties.RemoveAt(i);
                        }
                    }
                }

                EMemberExitedReason Reason = EMemberExitedReason::Unknown;
                if (InCurrentStatus == EOS_ELobbyMemberStatus::EOS_LMS_LEFT)
                {
                    Reason = EMemberExitedReason::Left;
                }
                else if (InCurrentStatus == EOS_ELobbyMemberStatus::EOS_LMS_KICKED)
                {
                    Reason = EMemberExitedReason::Kicked;
                }
                if (Party->bReadyForEvents)
                {
                    this->TriggerOnPartyMemberExitedDelegates(
                        *OwnerUserId,
                        *Party->PartyId,
                        *FoundMember->GetUserId(),
                        Reason);
                }
                if (*OwnerUserId == *FoundMember->GetUserId())
                {
                    // Also fire PartyExited event.
                    UE_LOG(
                        LogEOS,
                        Verbose,
                        TEXT("Handle_LobbyMemberStatusReceived: Invoking OnPartyExited event for local user ID %s"),
                        *FoundMember->GetUserId()->ToString());
                    if (Party->bReadyForEvents)
                    {
                        this->TriggerOnPartyExitedDelegates(*FoundMember->GetUserId(), *Party->PartyId);
                    }
                }
                break;
            }
            case EOS_ELobbyMemberStatus::EOS_LMS_PROMOTED: {
                Party->LeaderId = TargetUser;
                if (Party->bReadyForEvents)
                {
                    this->TriggerOnPartyMemberPromotedDelegates(*OwnerUserId, *Party->PartyId, *Party->LeaderId);
                }
                break;
            }
            }
        }
    }
}

void FOnlinePartySystemEOS::RestoreParties(
    const FUniqueNetId &LocalUserId,
    const FOnRestorePartiesComplete &CompletionDelegate)
{
    if (LocalUserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        CompletionDelegate.ExecuteIfBound(LocalUserId, OnlineRedpointEOS::Errors::InvalidUser(LocalUserId));
        return;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(LocalUserId.AsShared());
    if (!EOSUser->HasValidProductUserId())
    {
        CompletionDelegate.ExecuteIfBound(LocalUserId, OnlineRedpointEOS::Errors::InvalidUser(LocalUserId));
        return;
    }

    EOS_HLobbySearch LobbySearch = {};
    EOS_Lobby_CreateLobbySearchOptions SearchOpts = {};
    SearchOpts.ApiVersion = EOS_LOBBY_CREATELOBBYSEARCH_API_LATEST;
    SearchOpts.MaxResults = 1;
    EOS_EResult LobbySearchResult = EOS_Lobby_CreateLobbySearch(this->EOSLobby, &SearchOpts, &LobbySearch);
    if (LobbySearchResult != EOS_EResult::EOS_Success)
    {
        CompletionDelegate.ExecuteIfBound(
            LocalUserId,
            ConvertError(
                TEXT("EOS_Lobby_CreateLobbySearch"),
                TEXT("Unable to search for parties to restore"),
                LobbySearchResult));
        return;
    }

    EOS_LobbySearch_SetTargetUserIdOptions IdOpts = {};
    IdOpts.ApiVersion = EOS_LOBBYSEARCH_SETTARGETUSERID_API_LATEST;
    IdOpts.TargetUserId = EOSUser->GetProductUserId();
    EOS_EResult SetTargetResult = EOS_LobbySearch_SetTargetUserId(LobbySearch, &IdOpts);
    if (SetTargetResult != EOS_EResult::EOS_Success)
    {
        CompletionDelegate.ExecuteIfBound(
            LocalUserId,
            ConvertError(
                TEXT("EOS_LobbySearch_SetTargetUserId"),
                TEXT("Unable to set target user ID for parties to restore"),
                LobbySearchResult));
        return;
    }

    // todo: we should support restoring multiple parties, even though it doesn't
    // make sense for most games...

    EOS_LobbySearch_SetMaxResultsOptions MaxOpts = {};
    MaxOpts.ApiVersion = EOS_LOBBYSEARCH_SETMAXRESULTS_API_LATEST;
    MaxOpts.MaxResults = 1;
    EOS_EResult SetMaxResultsResult = EOS_LobbySearch_SetMaxResults(LobbySearch, &MaxOpts);
    if (SetMaxResultsResult != EOS_EResult::EOS_Success)
    {
        CompletionDelegate.ExecuteIfBound(
            LocalUserId,
            ConvertError(
                TEXT("EOS_LobbySearch_SetMaxResults"),
                TEXT("Unable to set max results for parties to restore"),
                SetMaxResultsResult));
        return;
    }

    EOS_LobbySearch_FindOptions FindOpts = {};
    FindOpts.ApiVersion = EOS_LOBBYSEARCH_FIND_API_LATEST;
    FindOpts.LocalUserId = EOSUser->GetProductUserId();
    EOSRunOperation<EOS_HLobbySearch, EOS_LobbySearch_FindOptions, EOS_LobbySearch_FindCallbackInfo>(
        LobbySearch,
        &FindOpts,
        *EOS_LobbySearch_Find,
        [WeakThis = GetWeakThis(this), CompletionDelegate, LobbySearch, EOSUser](
            const EOS_LobbySearch_FindCallbackInfo *Info) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Info->ResultCode != EOS_EResult::EOS_Success)
                {
                    CompletionDelegate.ExecuteIfBound(
                        EOSUser.Get(),
                        ConvertError(
                            TEXT("EOS_LobbySearch_Find"),
                            TEXT("Unable to execute search for parties to restore"),
                            Info->ResultCode));
                    return;
                }

                EOS_LobbySearch_GetSearchResultCountOptions CountOpts = {};
                CountOpts.ApiVersion = EOS_LOBBYSEARCH_GETSEARCHRESULTCOUNT_API_LATEST;
                auto Count = EOS_LobbySearch_GetSearchResultCount(LobbySearch, &CountOpts);
                if (Count == 0)
                {
                    // This is fine, this just means the user has no party to rejoin into.
                    EOS_LobbySearch_Release(LobbySearch);
                    CompletionDelegate.ExecuteIfBound(EOSUser.Get(), OnlineRedpointEOS::Errors::Success());
                    return;
                }

                EOS_HLobbyDetails SearchLobbyHandle = {};
                EOS_LobbySearch_CopySearchResultByIndexOptions CopyOpts = {};
                CopyOpts.ApiVersion = EOS_LOBBYSEARCH_COPYSEARCHRESULTBYINDEX_API_LATEST;
                CopyOpts.LobbyIndex = 0;
                EOS_EResult CopyResult =
                    EOS_LobbySearch_CopySearchResultByIndex(LobbySearch, &CopyOpts, &SearchLobbyHandle);
                if (CopyResult != EOS_EResult::EOS_Success)
                {
                    EOS_LobbySearch_Release(LobbySearch);
                    CompletionDelegate.ExecuteIfBound(
                        EOSUser.Get(),
                        ConvertError(
                            TEXT("EOS_LobbySearch_CopySearchResultByIndex"),
                            TEXT("Unable to copy search result for parties to restore"),
                            CopyResult));
                    return;
                }

                EOS_LobbySearch_Release(LobbySearch);

                // Automatically join the party.
                auto JoinInfo = MakeShared<FOnlinePartyJoinInfoEOS>(SearchLobbyHandle);
                This->JoinParty(
                    EOSUser.Get(),
                    JoinInfo.Get(),
                    FOnJoinPartyComplete::CreateLambda([CompletionDelegate, EOSUser](
                                                           const FUniqueNetId &LocalUserId,
                                                           const FOnlinePartyId &PartyId,
                                                           const EJoinPartyCompletionResult Result,
                                                           const int32 NotApprovedReason) {
                        if (Result == EJoinPartyCompletionResult::Succeeded)
                        {
                            CompletionDelegate.ExecuteIfBound(EOSUser.Get(), OnlineRedpointEOS::Errors::Success());
                            return;
                        }

                        CompletionDelegate.ExecuteIfBound(
                            EOSUser.Get(),
                            OnlineRedpointEOS::Errors::ServiceFailure(
                                *EOSUser,
                                PartyId.ToString(),
                                TEXT("JoinParty"),
                                TEXT("Unable to join party during party restoration")));
                    }));
            }
        });
}

void FOnlinePartySystemEOS::RestoreInvites(
    const FUniqueNetId &LocalUserId,
    const FOnRestoreInvitesComplete &CompletionDelegate)
{
    if (LocalUserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        CompletionDelegate.ExecuteIfBound(LocalUserId, OnlineRedpointEOS::Errors::InvalidUser(LocalUserId));
        return;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(LocalUserId.AsShared());
    if (!EOSUser->HasValidProductUserId())
    {
        CompletionDelegate.ExecuteIfBound(LocalUserId, OnlineRedpointEOS::Errors::InvalidUser(LocalUserId));
        return;
    }

    EOS_Lobby_QueryInvitesOptions Opts = {};
    Opts.ApiVersion = EOS_LOBBY_QUERYINVITES_API_LATEST;
    Opts.LocalUserId = EOSUser->GetProductUserId();

    EOSRunOperation<EOS_HLobby, EOS_Lobby_QueryInvitesOptions, EOS_Lobby_QueryInvitesCallbackInfo>(
        this->EOSLobby,
        &Opts,
        EOS_Lobby_QueryInvites,
        [WeakThis = GetWeakThis(this), CompletionDelegate, EOSUser](const EOS_Lobby_QueryInvitesCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Data->ResultCode == EOS_EResult::EOS_AlreadyPending)
                {
                    // Another RestoreInvites operation is in progress. You can't run RestoreInvites in parallel for
                    // multiple local users, so we wait until the other local user is done by trying again with a
                    // little delay.
                    FUTicker::GetCoreTicker().AddTicker(
                        FTickerDelegate::CreateLambda(
                            [WeakThis = GetWeakThis(This), EOSUser, CompletionDelegate](float DeltaSeconds) {
                                if (auto This = PinWeakThis(WeakThis))
                                {
                                    This->RestoreInvites(*EOSUser, CompletionDelegate);
                                }
                                return false;
                            }),
                        5.0f);
                    return;
                }

                if (Data->ResultCode != EOS_EResult::EOS_Success)
                {
                    CompletionDelegate.ExecuteIfBound(
                        EOSUser.Get(),
                        ConvertError(
                            TEXT("EOS_Lobby_QueryInvites"),
                            TEXT("Failed to query invites"),
                            Data->ResultCode));
                    return;
                }

                EOS_Lobby_GetInviteCountOptions CountOpts = {};
                CountOpts.ApiVersion = EOS_LOBBY_GETINVITECOUNT_API_LATEST;
                CountOpts.LocalUserId = EOSUser->GetProductUserId();
                uint32_t Count = EOS_Lobby_GetInviteCount(This->EOSLobby, &CountOpts);

                bool bInvitesChanged = false;
                for (uint32_t i = 0; i < Count; i++)
                {
                    // Copy invite ID out.
                    EOS_Lobby_GetInviteIdByIndexOptions GetOpts = {};
                    GetOpts.ApiVersion = EOS_LOBBY_GETINVITEIDBYINDEX_API_LATEST;
                    GetOpts.LocalUserId = EOSUser->GetProductUserId();
                    GetOpts.Index = i;
                    char *Buffer = nullptr;
                    int32 BufferLen = 0;
                    EOSString_Lobby_InviteId::AllocateEmptyCharBuffer(Buffer, BufferLen);
                    if (EOS_Lobby_GetInviteIdByIndex(This->EOSLobby, &GetOpts, Buffer, &BufferLen) !=
                        EOS_EResult::EOS_Success)
                    {
                        UE_LOG(LogEOS, Warning, TEXT("Unable to get invite ID by index during RestoreInvites"));
                        EOSString_Lobby_InviteId::FreeFromCharBuffer(Buffer);
                        continue;
                    }

                    // Copy the lobby details from the invite ID.
                    EOS_Lobby_CopyLobbyDetailsHandleByInviteIdOptions CopyOpts = {};
                    CopyOpts.ApiVersion = EOS_LOBBY_COPYLOBBYDETAILSHANDLEBYINVITEID_API_LATEST;
                    CopyOpts.InviteId = Buffer;
                    EOS_HLobbyDetails InviteLobbyDetails = {};
                    if (EOS_Lobby_CopyLobbyDetailsHandleByInviteId(This->EOSLobby, &CopyOpts, &InviteLobbyDetails) !=
                        EOS_EResult::EOS_Success)
                    {
                        UE_LOG(
                            LogEOS,
                            Warning,
                            TEXT("Unable to copy lobby details by invite ID during RestoreInvites"));
                        EOSString_Lobby_InviteId::FreeFromCharBuffer(Buffer);
                        continue;
                    }

                    // Convert and free char buffer of invite ID.
                    FString InviteId = EOSString_Lobby_InviteId::FromAnsiString(Buffer);
                    EOSString_Lobby_InviteId::FreeFromCharBuffer(Buffer);

                    // Ensure this user has a list of pending invites so we can add to it.
                    if (!This->PendingInvites.Contains(*EOSUser))
                    {
                        This->PendingInvites.Add(*EOSUser, TArray<IOnlinePartyJoinInfoConstRef>());
                    }

                    // If the invite is already in the list of pending invites, ignore it.
                    bool bInviteAlreadyExists = false;
                    for (const auto &PendingInvite : This->PendingInvites[*EOSUser])
                    {
                        auto PendingInviteEOS = StaticCastSharedRef<const FOnlinePartyJoinInfoEOS>(PendingInvite);
                        if (PendingInviteEOS->GetInviteId() == InviteId)
                        {
                            bInviteAlreadyExists = true;
                            break;
                        }
                    }
                    if (bInviteAlreadyExists)
                    {
                        EOS_LobbyDetails_Release(InviteLobbyDetails);
                        continue;
                    }

                    // Copy the info so we can get the lobby owner. We can't obtain the
                    // original sender of the invite, so we just use the current owner instead.
                    EOS_LobbyDetails_CopyInfoOptions CopyInfoOpts = {};
                    EOS_LobbyDetails_Info *LobbyInfo = nullptr;
                    CopyInfoOpts.ApiVersion = EOS_LOBBYDETAILS_COPYINFO_API_LATEST;
                    verify(
                        EOS_LobbyDetails_CopyInfo(InviteLobbyDetails, &CopyInfoOpts, &LobbyInfo) ==
                        EOS_EResult::EOS_Success);
                    auto LobbyOwnerEOS = MakeShared<FUniqueNetIdEOS>(LobbyInfo->LobbyOwnerUserId);
                    EOS_LobbyDetails_Info_Release(LobbyInfo);

                    // Create the join info.
                    auto JoinInfo =
                        MakeShared<FOnlinePartyJoinInfoEOS>(InviteLobbyDetails, LobbyOwnerEOS, TEXT(""), InviteId);

                    // If the local user has already joined the party, there's no need
                    // to add this invite.
                    if (This->IsLocalUserInParty(*JoinInfo->GetPartyId(), *EOSUser))
                    {
                        continue;
                    }

                    // Create the join info and add it to the list of pending invites.
                    checkf(
                        !JoinInfo->GetInviteId().IsEmpty(),
                        TEXT("InviteId must not be empty for invite added to PendingInvites array."));
                    This->PendingInvites[*EOSUser].Add(JoinInfo);
#if defined(UE_5_1_OR_LATER)
                    This->TriggerOnPartyInviteReceivedExDelegates(*EOSUser, JoinInfo.Get());
#else
                    This->TriggerOnPartyInviteReceivedDelegates(
                        *EOSUser,
                        JoinInfo->GetPartyId().Get(),
                        *JoinInfo->GetSourceUserId());
#endif
                    bInvitesChanged = true;
                }
                if (bInvitesChanged)
                {
                    This->TriggerOnPartyInvitesChangedDelegates(*EOSUser);
                }

                CompletionDelegate.ExecuteIfBound(EOSUser.Get(), OnlineRedpointEOS::Errors::Success());
            }
        });
}

void FOnlinePartySystemEOS::CleanupParties(
    const FUniqueNetId &LocalUserId,
    const FOnCleanupPartiesComplete &CompletionDelegate)
{
    CompletionDelegate.ExecuteIfBound(LocalUserId, OnlineRedpointEOS::Errors::NotImplemented(LocalUserId));
}

bool FOnlinePartySystemEOS::CreateParty(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyTypeId PartyTypeId,
    const FPartyConfiguration &PartyConfig,
    const FOnCreatePartyComplete &Delegate)
{
    if (LocalUserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        const FOnlineError Err = OnlineRedpointEOS::Errors::InvalidUser(
            TEXT("FOnlinePartySystemEOS::CreateParty"),
            FString::Printf(
                TEXT("The local user ID is not an EOS user ID (got a user ID with type '%s')."),
                *LocalUserId.GetType().ToString()));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
        return false;
    }

    auto LocalUserIdRef = LocalUserId.AsShared();

    // We want to make a copy so that changes to the FPartyConfiguration (if it's a shared reference)
    // don't take effect while we're in the middle of performing our operations, and so we can store it
    // later against the party object without the user being able to directly modify it.
    TSharedRef<FPartyConfiguration> PartyConfigCopy = MakeShared<FPartyConfiguration>(PartyConfig);

    CreateJoinMutex.Run([WeakThis = GetWeakThis(this), LocalUserIdRef, Delegate, PartyConfigCopy, PartyTypeId](
                            const std::function<void()> &MutexRelease) {
        if (auto This = PinWeakThis(WeakThis))
        {
            if (LocalUserIdRef.Get().GetType() != REDPOINT_EOS_SUBSYSTEM)
            {
                const FOnlineError Err = OnlineRedpointEOS::Errors::InvalidUser(
                    TEXT("FOnlinePartySystemEOS::CreateParty"),
                    TEXT("The local user ID is not an EOS user ID."));
                UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
                Delegate.ExecuteIfBound(
                    LocalUserIdRef.Get(),
                    nullptr,
                    ConvertErrorTo_ECreatePartyCompletionResult(Err));
                MutexRelease();
                return;
            }

            auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(LocalUserIdRef);
            if (!EOSUser->HasValidProductUserId())
            {
                const FOnlineError Err = OnlineRedpointEOS::Errors::InvalidUser(
                    TEXT("FOnlinePartySystemEOS::CreateParty"),
                    TEXT("The local user ID does not have a valid EOS product user ID."));
                UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
                Delegate.ExecuteIfBound(
                    LocalUserIdRef.Get(),
                    nullptr,
                    ConvertErrorTo_ECreatePartyCompletionResult(Err));
                MutexRelease();
                return;
            }

#if !defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING
            bool bEnableEcho = This->Config->GetEnableVoiceChatEchoInParties();
#else
            bool bEnableEcho = false;
#endif

            EOS_Lobby_CreateLobbyOptions Opts = {};
            Opts.ApiVersion = EOS_LOBBY_CREATELOBBY_API_LATEST;
            Opts.LocalUserId = EOSUser->GetProductUserId();
            Opts.MaxLobbyMembers = PartyConfigCopy->MaxMembers;
#if EOS_VERSION_AT_LEAST(1, 11, 0)
            Opts.BucketId = "Default";
            Opts.bAllowInvites =
                PartyConfigCopy->bIsAcceptingMembers
                    ? (PartyConfigCopy->InvitePermissions != PartySystemPermissions::EPermissionType::Noone)
                    : false;
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
            Opts.bDisableHostMigration = PartyConfigCopy->bShouldRemoveOnDisconnection ? EOS_TRUE : EOS_FALSE;
#endif
#if EOS_VERSION_AT_LEAST(1, 13, 0) && defined(EOS_VOICE_CHAT_SUPPORTED)
            Opts.bEnableRTCRoom = PartyConfigCopy->bChatEnabled ? EOS_TRUE : EOS_FALSE;
            EOS_Lobby_LocalRTCOptions RTCOpts = {};
            RTCOpts.ApiVersion = EOS_LOBBY_LOCALRTCOPTIONS_API_LATEST;
            RTCOpts.Flags = bEnableEcho ? EOS_RTC_JOINROOMFLAGS_ENABLE_ECHO : 0x0;
            RTCOpts.bUseManualAudioInput = EOS_FALSE;  // @todo: Needs to come from Project Settings.
            RTCOpts.bUseManualAudioOutput = EOS_FALSE; // @todo: Needs to come from Project Settings.
#if EOS_VERSION_AT_LEAST(1, 13, 1)
            RTCOpts.bLocalAudioDeviceInputStartsMuted = EOS_FALSE;
#else
            RTCOpts.bAudioOutputStartsMuted = EOS_FALSE;
#endif
            Opts.LocalRTCOptions = PartyConfigCopy->bChatEnabled ? &RTCOpts : nullptr;
#endif
            if (This->Config->GetPresenceAdvertisementType() == EPresenceAdvertisementType::Party)
            {
                Opts.bPresenceEnabled = PartyTypeId == This->GetPrimaryPartyTypeId();
            }
            else
            {
                Opts.bPresenceEnabled = false;
            }
            if (PartyConfigCopy->bIsAcceptingMembers)
            {
                switch (PartyConfigCopy->InvitePermissions)
                {
                case PartySystemPermissions::EPermissionType::Anyone:
                    Opts.PermissionLevel = EOS_ELobbyPermissionLevel::EOS_LPL_PUBLICADVERTISED;
                    break;
                case PartySystemPermissions::EPermissionType::Friends:
                    Opts.PermissionLevel = EOS_ELobbyPermissionLevel::EOS_LPL_JOINVIAPRESENCE;
                    break;
                case PartySystemPermissions::EPermissionType::Leader:
                case PartySystemPermissions::EPermissionType::Noone:
                default:
                    Opts.PermissionLevel = EOS_ELobbyPermissionLevel::EOS_LPL_INVITEONLY;
                    break;
                }
            }
            else
            {
                Opts.PermissionLevel = EOS_ELobbyPermissionLevel::EOS_LPL_INVITEONLY;
            }

            EOSRunOperation<EOS_HLobby, EOS_Lobby_CreateLobbyOptions, EOS_Lobby_CreateLobbyCallbackInfo>(
                This->EOSLobby,
                &Opts,
                *EOS_Lobby_CreateLobby,
                [WeakThis = GetWeakThis(This),
                 Delegate,
                 LocalUserIdRef,
                 MutexRelease,
                 EOSUser,
                 PartyTypeId,
                 bVoiceChatEnabled = PartyConfigCopy->bChatEnabled,
                 bEnableEcho,
                 PartyConfigCopy](const EOS_Lobby_CreateLobbyCallbackInfo *Info) {
                    if (auto This = PinWeakThis(WeakThis))
                    {
                        if (Info->ResultCode != EOS_EResult::EOS_Success)
                        {
                            FOnlineError Err = ConvertError(
                                TEXT("FOnlinePartySystemEOS::CreateParty"),
                                TEXT("Failed to create EOS lobby for party"),
                                Info->ResultCode);
                            UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
                            Delegate.ExecuteIfBound(
                                LocalUserIdRef.Get(),
                                nullptr,
                                ConvertErrorTo_ECreatePartyCompletionResult(Err));
                            MutexRelease();
                            return;
                        }

#if EOS_VERSION_AT_LEAST(1, 13, 0) && defined(EOS_VOICE_CHAT_SUPPORTED)
                        if (bVoiceChatEnabled)
                        {
                            TSharedPtr<FEOSVoiceManagerLocalUser> VoiceUser =
                                This->VoiceManager->GetLocalUser(*EOSUser);
                            if (VoiceUser.IsValid())
                            {
                                EOS_Lobby_GetRTCRoomNameOptions GetRTCRoomNameOpts = {};
                                GetRTCRoomNameOpts.ApiVersion = EOS_LOBBY_GETRTCROOMNAME_API_LATEST;
                                GetRTCRoomNameOpts.LocalUserId = EOSUser->GetProductUserId();
                                GetRTCRoomNameOpts.LobbyId = Info->LobbyId;

                                FString RoomName;
                                EOS_EResult GetRoomNameResult = EOSString_AnsiUnlimited::
                                    FromDynamicLengthApiCall<EOS_HLobby, EOS_Lobby_GetRTCRoomNameOptions>(
                                        This->EOSLobby,
                                        &GetRTCRoomNameOpts,
                                        EOS_Lobby_GetRTCRoomName,
                                        RoomName);
                                if (GetRoomNameResult == EOS_EResult::EOS_Success)
                                {
                                    VoiceUser->RegisterLobbyChannel(
                                        RoomName,
                                        bEnableEcho ? EVoiceChatChannelType::Echo
                                                    : EVoiceChatChannelType::NonPositional);
                                }
                            }
                        }
#endif

                        EOS_HLobbyModification Mod = {};

                        EOS_Lobby_UpdateLobbyModificationOptions ModOpts = {};
                        ModOpts.ApiVersion = EOS_LOBBY_UPDATELOBBYMODIFICATION_API_LATEST;
                        ModOpts.LobbyId = Info->LobbyId;
                        ModOpts.LocalUserId = EOSUser->GetProductUserId();

                        EOS_EResult LobbyModResult = EOS_Lobby_UpdateLobbyModification(This->EOSLobby, &ModOpts, &Mod);
                        if (LobbyModResult != EOS_EResult::EOS_Success)
                        {
                            FOnlineError Err = ConvertError(
                                TEXT("FOnlinePartySystemEOS::CreateParty"),
                                TEXT("Failed to create 'update lobby modification' for party"),
                                LobbyModResult);
                            UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
                            // @todo: Clean up lobby.
                            check(false /* failed to get handle after creating lobby! */);
                        }

                        {
                            EOS_Lobby_AttributeData AttrData = {};
                            AttrData.ApiVersion = EOS_LOBBY_ATTRIBUTEDATA_API_LATEST;
                            AttrData.Key = PARTY_TYPE_ID_ANSI;
                            AttrData.ValueType = EOS_ELobbyAttributeType::EOS_AT_INT64;
                            AttrData.Value.AsInt64 = PartyTypeId.GetValue();

                            EOS_LobbyModification_AddAttributeOptions AddOpts = {};
                            AddOpts.ApiVersion = EOS_LOBBYMODIFICATION_ADDATTRIBUTE_API_LATEST;
                            AddOpts.Attribute = &AttrData;
                            AddOpts.Visibility = EOS_ELobbyAttributeVisibility::EOS_LAT_PUBLIC;
                            LobbyModResult = EOS_LobbyModification_AddAttribute(Mod, &AddOpts);
                            if (LobbyModResult != EOS_EResult::EOS_Success)
                            {
                                EOS_LobbyModification_Release(Mod);
                                FOnlineError Err = ConvertError(
                                    TEXT("FOnlinePartySystemEOS::CreateParty"),
                                    FString::Printf(
                                        TEXT("Failed to add attribute '%s' to EOS lobby modification for party"),
                                        PARTY_TYPE_ID_TCHAR),
                                    LobbyModResult);
                                UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
                                // @todo: Clean up lobby.
                                check(false /* failed to get handle after creating lobby! */);
                            }
                        }

                        EOS_Lobby_UpdateLobbyOptions Opts = {};
                        Opts.ApiVersion = EOS_LOBBY_UPDATELOBBY_API_LATEST;
                        Opts.LobbyModificationHandle = Mod;

                        EOSRunOperation<EOS_HLobby, EOS_Lobby_UpdateLobbyOptions, EOS_Lobby_UpdateLobbyCallbackInfo>(
                            This->EOSLobby,
                            &Opts,
                            *EOS_Lobby_UpdateLobby,
                            [WeakThis = GetWeakThis(This),
                             Mod,
                             EOSUser,
                             LocalUserIdRef,
                             Delegate,
                             MutexRelease,
                             bVoiceChatEnabled,
                             PartyConfigCopy,
                             PartyTypeId](const EOS_Lobby_UpdateLobbyCallbackInfo *Info) {
                                EOS_LobbyModification_Release(Mod);

                                if (auto This = PinWeakThis(WeakThis))
                                {
                                    if (Info->ResultCode != EOS_EResult::EOS_Success)
                                    {
                                        FOnlineError Err = ConvertError(
                                            TEXT("FOnlinePartySystemEOS::CreateParty"),
                                            TEXT("Failed to update EOS lobby for party"),
                                            Info->ResultCode);
                                        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());

                                        // @todo: Clean up lobby.
                                        check(false /* failed to update lobby during creation! */);
                                    }

                                    EOS_HLobbyDetails LobbyDetails = {};
                                    EOS_Lobby_CopyLobbyDetailsHandleOptions HandleOpts = {};
                                    HandleOpts.ApiVersion = EOS_LOBBY_COPYLOBBYDETAILSHANDLE_API_LATEST;
                                    HandleOpts.LobbyId = Info->LobbyId;
                                    HandleOpts.LocalUserId = EOSUser->GetProductUserId();
                                    EOS_EResult CopyDetailsResult =
                                        EOS_Lobby_CopyLobbyDetailsHandle(This->EOSLobby, &HandleOpts, &LobbyDetails);
                                    if (CopyDetailsResult != EOS_EResult::EOS_Success)
                                    {
                                        FOnlineError Err = ConvertError(
                                            TEXT("FOnlinePartySystemEOS::CreateParty"),
                                            TEXT(
                                                "Failed to copy lobby details after creating/updating lobby for party"),
                                            CopyDetailsResult);
                                        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());

                                        // @todo: Clean up lobby.
                                        check(false /* failed to get handle after creating lobby! */);
                                    }

                                    auto PartyId = MakeShared<FOnlinePartyIdEOS>(Info->LobbyId);
                                    auto Party = MakeShared<FOnlinePartyEOS>(
                                        This->EOSPlatform,
                                        LobbyDetails,
                                        StaticCastSharedRef<const FUniqueNetIdEOS>(LocalUserIdRef),
                                        This->AsShared(),
                                        This->UserFactory,
                                        bVoiceChatEnabled,
                                        PartyConfigCopy);

                                    auto Finalize = [WeakThis = GetWeakThis(This),
                                                     Party,
                                                     LocalUserIdRef,
                                                     PartyId,
                                                     Delegate,
                                                     MutexRelease](const FOnlineError &Error) {
                                        if (auto This = PinWeakThis(WeakThis))
                                        {
                                            if (!Error.bSucceeded)
                                            {
                                                UE_LOG(LogEOS, Warning, TEXT("%s"), *Error.ToLogString());
                                            }

                                            UE_LOG(LogEOS, Verbose, TEXT("CreateParty: Added party to parties list"));
                                            if (!This->JoinedParties.Contains(*LocalUserIdRef))
                                            {
                                                This->JoinedParties.Add(
                                                    *LocalUserIdRef,
                                                    TArray<TSharedPtr<FOnlinePartyEOS>>());
                                            }
                                            This->JoinedParties[*LocalUserIdRef].Add(Party);

                                            // Fire party join event (because we created it).
                                            This->TriggerOnPartyJoinedDelegates(LocalUserIdRef.Get(), PartyId.Get());

                                            // Allow party-related events to fire now.
                                            Party->bReadyForEvents = true;

                                            Delegate.ExecuteIfBound(
                                                LocalUserIdRef.Get(),
                                                PartyId,
                                                ECreatePartyCompletionResult::Succeeded);
                                            MutexRelease();
                                            return;
                                        }
                                    };

                                    if (This->ShouldHaveSyntheticParty(PartyTypeId, *PartyConfigCopy))
                                    {
                                        if (TSharedPtr<FSyntheticPartySessionManager> SPM =
                                                This->SyntheticPartySessionManager.Pin())
                                        {
                                            SPM->CreateSyntheticParty(
                                                PartyId,
                                                FSyntheticPartySessionOnComplete::CreateLambda(Finalize));
                                        }
                                        else
                                        {
                                            // No synthetic party manager available.
                                            Finalize(OnlineRedpointEOS::Errors::Success());
                                        }
                                    }
                                    else
                                    {
                                        // Party should not have synthetic party.
                                        Finalize(OnlineRedpointEOS::Errors::Success());
                                    }
                                }
                            });
                    }
                });
        }
    });
    return true;
}

bool FOnlinePartySystemEOS::UpdateParty(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FPartyConfiguration &PartyConfig,
    bool bShouldRegenerateReservationKey,
    const FOnUpdatePartyComplete &Delegate)
{
    if (LocalUserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        FOnlineError Err = OnlineRedpointEOS::Errors::InvalidUser(
            LocalUserId,
            PartyId.ToString(),
            TEXT("FOnlinePartySystemEOS::UpdateParty"),
            TEXT("Specified local user was not an EOS user"));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
        Delegate.ExecuteIfBound(LocalUserId, PartyId, ConvertErrorTo_EUpdateConfigCompletionResult(Err));
        return true;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(LocalUserId.AsShared());
    if (!EOSUser->HasValidProductUserId())
    {
        FOnlineError Err = OnlineRedpointEOS::Errors::InvalidUser(
            LocalUserId,
            PartyId.ToString(),
            TEXT("FOnlinePartySystemEOS::UpdateParty"),
            TEXT("Specified local user did not have a valid EOS ID"));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
        Delegate.ExecuteIfBound(LocalUserId, PartyId, ConvertErrorTo_EUpdateConfigCompletionResult(Err));
        return true;
    }

    TSharedPtr<FOnlineParty> Party = nullptr;
    for (const auto &PartyRef : this->JoinedParties[LocalUserId])
    {
        if (*PartyRef->PartyId == PartyId)
        {
            Party = PartyRef;
            break;
        }
    }
    if (!Party.IsValid())
    {
        FOnlineError Err = OnlineRedpointEOS::Errors::NotFound(
            LocalUserId,
            PartyId.ToString(),
            TEXT("FOnlinePartySystemEOS::UpdateParty"),
            TEXT("Specified local user is not joined to the specified party"));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
        Delegate.ExecuteIfBound(LocalUserId, PartyId, ConvertErrorTo_EUpdateConfigCompletionResult(Err));
        return true;
    }

    EOS_HLobbyModification Mod = {};

    EOS_Lobby_UpdateLobbyModificationOptions ModOpts = {};
    ModOpts.ApiVersion = EOS_LOBBY_UPDATELOBBYMODIFICATION_API_LATEST;
    ModOpts.LobbyId = ((const FOnlinePartyIdEOS &)PartyId).GetLobbyId();
    ModOpts.LocalUserId = EOSUser->GetProductUserId();
    EOS_EResult UpdateModResult = EOS_Lobby_UpdateLobbyModification(this->EOSLobby, &ModOpts, &Mod);
    if (UpdateModResult != EOS_EResult::EOS_Success)
    {
        FOnlineError Err = ConvertError(
            LocalUserId,
            PartyId.ToString(),
            TEXT("EOS_Lobby_UpdateLobbyModification"),
            TEXT("Unable to create update lobby modification for further operations"),
            UpdateModResult);
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
        Delegate.ExecuteIfBound(LocalUserId, PartyId, ConvertErrorTo_EUpdateConfigCompletionResult(Err));
        return true;
    }

    {
        EOS_LobbyModification_SetMaxMembersOptions SetOpts = {};
        SetOpts.ApiVersion = EOS_LOBBYMODIFICATION_SETMAXMEMBERS_API_LATEST;
        SetOpts.MaxMembers = PartyConfig.MaxMembers;
        EOS_EResult SetMaxMembersResult = EOS_LobbyModification_SetMaxMembers(Mod, &SetOpts);
        if (SetMaxMembersResult != EOS_EResult::EOS_Success)
        {
            FOnlineError Err = ConvertError(
                LocalUserId,
                PartyId.ToString(),
                TEXT("EOS_LobbyModification_SetMaxMembers"),
                FString::Printf(TEXT("Unable to set max lobby members to %d"), PartyConfig.MaxMembers),
                SetMaxMembersResult);
            UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
            Delegate.ExecuteIfBound(LocalUserId, PartyId, ConvertErrorTo_EUpdateConfigCompletionResult(Err));
            return true;
        }
    }

    {
        EOS_LobbyModification_SetPermissionLevelOptions SetOpts = {};
        SetOpts.ApiVersion = EOS_LOBBYMODIFICATION_SETPERMISSIONLEVEL_API_LATEST;
        if (PartyConfig.bIsAcceptingMembers)
        {
            switch (PartyConfig.InvitePermissions)
            {
            case PartySystemPermissions::EPermissionType::Anyone:
                SetOpts.PermissionLevel = EOS_ELobbyPermissionLevel::EOS_LPL_PUBLICADVERTISED;
                break;
            case PartySystemPermissions::EPermissionType::Friends:
                SetOpts.PermissionLevel = EOS_ELobbyPermissionLevel::EOS_LPL_JOINVIAPRESENCE;
                break;
            case PartySystemPermissions::EPermissionType::Leader:
            case PartySystemPermissions::EPermissionType::Noone:
            default:
                SetOpts.PermissionLevel = EOS_ELobbyPermissionLevel::EOS_LPL_INVITEONLY;
                break;
            }
        }
        else
        {
            SetOpts.PermissionLevel = EOS_ELobbyPermissionLevel::EOS_LPL_INVITEONLY;
        }
        EOS_EResult PermissionsResult = EOS_LobbyModification_SetPermissionLevel(Mod, &SetOpts);
        if (PermissionsResult != EOS_EResult::EOS_Success)
        {
            FOnlineError Err = ConvertError(
                LocalUserId,
                PartyId.ToString(),
                TEXT("EOS_LobbyModification_SetPermissionLevel"),
                TEXT("Unable to set lobby permissions"),
                PermissionsResult);
            UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
            Delegate.ExecuteIfBound(LocalUserId, PartyId, ConvertErrorTo_EUpdateConfigCompletionResult(Err));
            return true;
        }
    }

#if EOS_VERSION_AT_LEAST(1, 11, 0)
    {
        EOS_LobbyModification_SetInvitesAllowedOptions SetOpts = {};
        SetOpts.ApiVersion = EOS_LOBBYMODIFICATION_SETINVITESALLOWED_API_LATEST;
        if (PartyConfig.bIsAcceptingMembers)
        {
            SetOpts.bInvitesAllowed = (PartyConfig.InvitePermissions != PartySystemPermissions::EPermissionType::Noone);
        }
        else
        {
            SetOpts.bInvitesAllowed = false;
        }
        EOS_EResult InvitesResult = EOS_LobbyModification_SetInvitesAllowed(Mod, &SetOpts);
        if (InvitesResult != EOS_EResult::EOS_Success)
        {
            FOnlineError Err = ConvertError(
                LocalUserId,
                PartyId.ToString(),
                TEXT("EOS_LobbyModification_SetInvitesAllowed"),
                TEXT("Unable to set lobby invites setting"),
                InvitesResult);
            UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
            Delegate.ExecuteIfBound(LocalUserId, PartyId, ConvertErrorTo_EUpdateConfigCompletionResult(Err));
            return true;
        }
    }
#endif

    EOS_Lobby_UpdateLobbyOptions Opts = {};
    Opts.ApiVersion = EOS_LOBBY_UPDATELOBBY_API_LATEST;
    Opts.LobbyModificationHandle = Mod;

    auto LocalUserIdRef = LocalUserId.AsShared();
    auto PartyIdRef = PartyId.AsShared();

    bool bShouldHaveSyntheticParty = this->ShouldHaveSyntheticParty(Party->PartyTypeId, PartyConfig);

    EOSRunOperation<EOS_HLobby, EOS_Lobby_UpdateLobbyOptions, EOS_Lobby_UpdateLobbyCallbackInfo>(
        this->EOSLobby,
        &Opts,
        *EOS_Lobby_UpdateLobby,
        [WeakThis = GetWeakThis(this), Mod, Delegate, LocalUserIdRef, PartyIdRef, bShouldHaveSyntheticParty](
            const EOS_Lobby_UpdateLobbyCallbackInfo *Info) {
            EOS_LobbyModification_Release(Mod);

            if (auto This = PinWeakThis(WeakThis))
            {
                if (Info->ResultCode != EOS_EResult::EOS_Success)
                {
                    FOnlineError Err = ConvertError(
                        *LocalUserIdRef,
                        PartyIdRef->ToString(),
                        TEXT("EOS_Lobby_UpdateLobby"),
                        TEXT("Unable to update lobby"),
                        Info->ResultCode);
                    UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
                    Delegate.ExecuteIfBound(
                        LocalUserIdRef.Get(),
                        PartyIdRef.Get(),
                        ConvertErrorTo_EUpdateConfigCompletionResult(Err));
                    return;
                }

                if (TSharedPtr<FSyntheticPartySessionManager> SPM = This->SyntheticPartySessionManager.Pin())
                {
                    bool bHasSyntheticParty = SPM->HasSyntheticParty(PartyIdRef);
                    if (bShouldHaveSyntheticParty && !bHasSyntheticParty)
                    {
                        SPM->CreateSyntheticParty(
                            PartyIdRef,
                            FSyntheticPartySessionOnComplete::CreateLambda(
                                [Delegate, LocalUserIdRef, PartyIdRef](const FOnlineError &Error) {
                                    Delegate.ExecuteIfBound(
                                        LocalUserIdRef.Get(),
                                        PartyIdRef.Get(),
                                        ConvertErrorTo_EUpdateConfigCompletionResult(Error));
                                }));
                        return;
                    }
                    else if (!bShouldHaveSyntheticParty && bHasSyntheticParty)
                    {
                        SPM->DeleteSyntheticParty(
                            PartyIdRef,
                            FSyntheticPartySessionOnComplete::CreateLambda(
                                [Delegate, LocalUserIdRef, PartyIdRef](const FOnlineError &Error) {
                                    Delegate.ExecuteIfBound(
                                        LocalUserIdRef.Get(),
                                        PartyIdRef.Get(),
                                        ConvertErrorTo_EUpdateConfigCompletionResult(Error));
                                }));
                        return;
                    }
                }

                // NOTE: If adding more logic here, SPM callbacks above must also be updated.

                Delegate.ExecuteIfBound(
                    LocalUserIdRef.Get(),
                    PartyIdRef.Get(),
                    EUpdateConfigCompletionResult::Succeeded);
            }
        });
    return true;
}

bool FOnlinePartySystemEOS::JoinParty(
    const FUniqueNetId &LocalUserId,
    const IOnlinePartyJoinInfo &OnlinePartyJoinInfo,
    const FOnJoinPartyComplete &Delegate)
{
    if (LocalUserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        Delegate.ExecuteIfBound(
            LocalUserId,
            *OnlinePartyJoinInfo.GetPartyId(),
            EJoinPartyCompletionResult::UnknownClientFailure,
            0);
        return false;
    }

    auto LocalUserIdRef = StaticCastSharedRef<const FUniqueNetIdEOS>(LocalUserId.AsShared());
    if (!LocalUserIdRef->HasValidProductUserId())
    {
        Delegate.ExecuteIfBound(
            LocalUserId,
            *OnlinePartyJoinInfo.GetPartyId(),
            EJoinPartyCompletionResult::UnknownClientFailure,
            0);
        return false;
    }

    auto OnlinePartyJoinInfoRef = OnlinePartyJoinInfo.AsShared();
    auto OnlinePartyJoinInfoEOSBase = StaticCastSharedRef<const IOnlinePartyJoinInfoEOS>(OnlinePartyJoinInfoRef);

    if (OnlinePartyJoinInfoEOSBase->IsUnresolved())
    {
        auto EOSUnresolvedJoinInfo =
            FOnlinePartyJoinInfoEOSUnresolved::GetEOSUnresolvedJoinInfo(OnlinePartyJoinInfoRef.Get());

        auto UnresolvedLobbyId = StaticCastSharedRef<const FOnlinePartyIdEOS>(EOSUnresolvedJoinInfo->GetPartyId());

        // First try to get the handle directly via EOS_Lobby_CopyLobbyDetailsHandle. This works if the SDK still has
        // the handle in memory for the lobby.
        {
            EOS_HLobbyDetails LobbyDetails = nullptr;
            EOS_Lobby_CopyLobbyDetailsHandleOptions CopyOpts = {};
            CopyOpts.ApiVersion = EOS_LOBBY_COPYLOBBYDETAILSHANDLE_API_LATEST;
            CopyOpts.LobbyId = UnresolvedLobbyId->GetLobbyId();
            CopyOpts.LocalUserId = LocalUserIdRef->GetProductUserId();
            auto Result = EOS_Lobby_CopyLobbyDetailsHandle(this->EOSLobby, &CopyOpts, &LobbyDetails);
            if (Result == EOS_EResult::EOS_Success)
            {
                auto ResolvedInfo = MakeShared<FOnlinePartyJoinInfoEOS>(LobbyDetails);
                return JoinParty(*LocalUserIdRef, *ResolvedInfo, Delegate);
            }
            if (LobbyDetails != nullptr)
            {
                EOS_LobbyDetails_Release(LobbyDetails);
                LobbyDetails = nullptr;
            }
        }

        // We must perform a search for the party to create the actual FOnlinePartyJoinInfoEOS, then re-invoke JoinParty
        // with the resolved data.
        {
            EOS_HLobbySearch LobbySearch = {};
            EOS_Lobby_CreateLobbySearchOptions SearchOpts = {};
            SearchOpts.ApiVersion = EOS_LOBBY_CREATELOBBYSEARCH_API_LATEST;
            SearchOpts.MaxResults = 1;
            EOS_EResult CreateSearchResult = EOS_Lobby_CreateLobbySearch(this->EOSLobby, &SearchOpts, &LobbySearch);
            if (CreateSearchResult != EOS_EResult::EOS_Success)
            {
                UE_LOG(LogEOS, Error, TEXT("Unable to create search for looking up party by ID"));
                Delegate.ExecuteIfBound(
                    LocalUserId,
                    *OnlinePartyJoinInfo.GetPartyId(),
                    EJoinPartyCompletionResult::UnknownClientFailure,
                    0);
                return true;
            }

            EOS_LobbySearch_SetLobbyIdOptions IdOpts = {};
            IdOpts.ApiVersion = EOS_LOBBYSEARCH_SETLOBBYID_API_LATEST;
            IdOpts.LobbyId = UnresolvedLobbyId->GetLobbyId();
            EOS_EResult SearchSetIdResult = EOS_LobbySearch_SetLobbyId(LobbySearch, &IdOpts);
            if (SearchSetIdResult != EOS_EResult::EOS_Success)
            {
                EOS_LobbySearch_Release(LobbySearch);
                UE_LOG(LogEOS, Error, TEXT("Unable to execute EOS_LobbySearch_SetLobbyId for looking up party by ID"));
                Delegate.ExecuteIfBound(
                    LocalUserId,
                    *OnlinePartyJoinInfo.GetPartyId(),
                    EJoinPartyCompletionResult::UnknownClientFailure,
                    0);
                return true;
            }

            EOS_LobbySearch_FindOptions FindOpts = {};
            FindOpts.ApiVersion = EOS_LOBBYSEARCH_FIND_API_LATEST;
            FindOpts.LocalUserId = LocalUserIdRef->GetProductUserId();
            EOSRunOperation<EOS_HLobbySearch, EOS_LobbySearch_FindOptions, EOS_LobbySearch_FindCallbackInfo>(
                LobbySearch,
                &FindOpts,
                EOS_LobbySearch_Find,
                [WeakThis = GetWeakThis(this),
                 LobbySearch,
                 Delegate,
                 LocalUserIdRef,
                 OnlinePartyJoinInfoRef,
                 UnresolvedLobbyId](const EOS_LobbySearch_FindCallbackInfo *Info) {
                    if (auto This = PinWeakThis(WeakThis))
                    {
                        if (Info->ResultCode != EOS_EResult::EOS_Success)
                        {
                            EOS_LobbySearch_Release(LobbySearch);

                            FOnlineError Err = ConvertError(
                                *LocalUserIdRef,
                                UnresolvedLobbyId->ToString(),
                                TEXT("EOS_LobbySearch_Find"),
                                TEXT("Lobby search operation that occurred while calling JoinParty failed; unable to "
                                     "locate party based on it's ID"),
                                Info->ResultCode);
                            UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
                            Delegate.ExecuteIfBound(
                                *LocalUserIdRef,
                                *OnlinePartyJoinInfoRef->GetPartyId(),
                                ConvertErrorTo_EJoinPartyCompletionResult(Err),
                                0);
                            return;
                        }

                        EOS_LobbySearch_GetSearchResultCountOptions CountOpts = {};
                        CountOpts.ApiVersion = EOS_LOBBYSEARCH_GETSEARCHRESULTCOUNT_API_LATEST;
                        if (EOS_LobbySearch_GetSearchResultCount(LobbySearch, &CountOpts) < 1)
                        {
                            EOS_LobbySearch_Release(LobbySearch);

                            FOnlineError Err = OnlineRedpointEOS::Errors::NotFound(
                                *LocalUserIdRef,
                                UnresolvedLobbyId->ToString(),
                                TEXT("EOS_LobbySearch_GetSearchResultCount"),
                                TEXT("Lobby search returned less than 1 result during JoinParty call; unable to locate "
                                     "party based on it's ID. Make sure the party's InvitePermissions are set to "
                                     "Anyone (public)."));
                            UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
                            Delegate.ExecuteIfBound(
                                *LocalUserIdRef,
                                *OnlinePartyJoinInfoRef->GetPartyId(),
                                ConvertErrorTo_EJoinPartyCompletionResult(Err),
                                0);
                            return;
                        }

                        EOS_HLobbyDetails LobbyDetails = nullptr;
                        EOS_LobbySearch_CopySearchResultByIndexOptions CopyOpts = {};
                        CopyOpts.ApiVersion = EOS_LOBBYSEARCH_COPYSEARCHRESULTBYINDEX_API_LATEST;
                        CopyOpts.LobbyIndex = 0;
                        EOS_EResult CopyResult =
                            EOS_LobbySearch_CopySearchResultByIndex(LobbySearch, &CopyOpts, &LobbyDetails);
                        if (CopyResult != EOS_EResult::EOS_Success)
                        {
                            if (LobbyDetails != nullptr)
                            {
                                EOS_LobbyDetails_Release(LobbyDetails);
                            }

                            EOS_LobbySearch_Release(LobbySearch);
                            UE_LOG(
                                LogEOS,
                                Error,
                                TEXT("Unable to copy lobby handle from party search result."),
                                ANSI_TO_TCHAR(EOS_EResult_ToString(Info->ResultCode)));
                            Delegate.ExecuteIfBound(
                                *LocalUserIdRef,
                                *OnlinePartyJoinInfoRef->GetPartyId(),
                                EJoinPartyCompletionResult::UnknownClientFailure,
                                0);
                            return;
                        }

                        EOS_LobbySearch_Release(LobbySearch);

                        auto ResolvedInfo = MakeShared<FOnlinePartyJoinInfoEOS>(LobbyDetails);
                        if (!This->JoinParty(*LocalUserIdRef, *ResolvedInfo, Delegate))
                        {
                            UE_LOG(
                                LogEOS,
                                Error,
                                TEXT("JoinParty operation did not start on resolved party ID."),
                                ANSI_TO_TCHAR(EOS_EResult_ToString(Info->ResultCode)));
                            Delegate.ExecuteIfBound(
                                *LocalUserIdRef,
                                *OnlinePartyJoinInfoRef->GetPartyId(),
                                EJoinPartyCompletionResult::UnknownClientFailure,
                                0);
                        }
                    }
                });
            return true;
        }
    }

    CreateJoinMutex.Run([WeakThis = GetWeakThis(this),
                         OnlinePartyJoinInfoRef,
                         OnlinePartyJoinInfoEOSBase,
                         Delegate,
                         LocalUserIdRef](const std::function<void()> &MutexRelease) {
        if (auto This = PinWeakThis(WeakThis))
        {
            auto EOSJoinInfo = FOnlinePartyJoinInfoEOS::GetEOSJoinInfo(OnlinePartyJoinInfoRef.Get());
            if (EOSJoinInfo == nullptr)
            {
                UE_LOG(LogEOS, Error, TEXT("JoinParty: EOSJoinInfo is not valid"));
                Delegate.ExecuteIfBound(
                    LocalUserIdRef.Get(),
                    OnlinePartyJoinInfoRef.Get().GetPartyId().Get(),
                    EJoinPartyCompletionResult::UnknownClientFailure,
                    0);
                MutexRelease();
                return;
            }

            if (LocalUserIdRef.Get().GetType() != REDPOINT_EOS_SUBSYSTEM)
            {
                UE_LOG(LogEOS, Error, TEXT("JoinParty: Local user ID type is not EOS"));
                Delegate.ExecuteIfBound(
                    LocalUserIdRef.Get(),
                    OnlinePartyJoinInfoRef.Get().GetPartyId().Get(),
                    EJoinPartyCompletionResult::UnknownClientFailure,
                    0);
                MutexRelease();
                return;
            }

            auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(LocalUserIdRef.Get().AsShared());
            if (!EOSUser->HasValidProductUserId())
            {
                UE_LOG(LogEOS, Error, TEXT("JoinParty: Local user does not have a valid product user ID"));
                Delegate.ExecuteIfBound(
                    LocalUserIdRef.Get(),
                    OnlinePartyJoinInfoRef.Get().GetPartyId().Get(),
                    EJoinPartyCompletionResult::UnknownClientFailure,
                    0);
                MutexRelease();
                return;
            }

            auto EOSPartyId = StaticCastSharedRef<const FOnlinePartyIdEOS>(OnlinePartyJoinInfoRef.Get().GetPartyId());

            auto JoinWithLobbyDetails = [WeakThis = GetWeakThis(This),
                                         EOSJoinInfo,
                                         EOSUser,
                                         Delegate,
                                         LocalUserIdRef,
                                         EOSPartyId,
                                         MutexRelease](EOS_HLobbyDetails InLobbyDetails, bool ReleaseHandle) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    EOS_Lobby_JoinLobbyOptions Opts = {};
                    Opts.ApiVersion = EOS_LOBBY_JOINLOBBY_API_LATEST;
                    if (This->Config->GetPresenceAdvertisementType() == EPresenceAdvertisementType::Party)
                    {
                        Opts.bPresenceEnabled =
                            EOSJoinInfo.ToSharedRef().Get().GetPartyTypeId() == This->GetPrimaryPartyTypeId();
                    }
                    else
                    {
                        Opts.bPresenceEnabled = false;
                    }
                    Opts.LocalUserId = EOSUser->GetProductUserId();
                    Opts.LobbyDetailsHandle = InLobbyDetails;

                    EOSRunOperation<EOS_HLobby, EOS_Lobby_JoinLobbyOptions, EOS_Lobby_JoinLobbyCallbackInfo>(
                        This->EOSLobby,
                        &Opts,
                        *EOS_Lobby_JoinLobby,
                        [WeakThis = GetWeakThis(This),
                         ReleaseHandle,
                         InLobbyDetails,
                         Delegate,
                         LocalUserIdRef,
                         EOSPartyId,
                         MutexRelease,
                         EOSUser,
                         EOSJoinInfo](const EOS_Lobby_JoinLobbyCallbackInfo *Info) {
                            if (ReleaseHandle)
                            {
                                EOS_LobbyDetails_Release(InLobbyDetails);
                            }

                            if (auto This = PinWeakThis(WeakThis))
                            {
                                if (Info->ResultCode != EOS_EResult::EOS_Success)
                                {
                                    FOnlineError Err = ConvertError(
                                        *EOSUser,
                                        EOSPartyId->ToString(),
                                        TEXT("EOS_Lobby_JoinLobby"),
                                        TEXT("Failed to join the party because the backend returned an error"),
                                        Info->ResultCode);
                                    UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
                                    Delegate.ExecuteIfBound(
                                        LocalUserIdRef.Get(),
                                        EOSPartyId.Get(),
                                        ConvertErrorTo_EJoinPartyCompletionResult(Err),
                                        0);
                                    MutexRelease();
                                    return;
                                }

                                bool bVoiceChatEnabled = false;
#if EOS_VERSION_AT_LEAST(1, 13, 0) && defined(EOS_VOICE_CHAT_SUPPORTED)
#if !defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING
                                bool bEnableEcho = This->Config->GetEnableVoiceChatEchoInParties();
#else
                                bool bEnableEcho = false;
#endif
                                // Just try to get the RTC room name; if it succeeds then this lobby is RTC enabled.
                                {
                                    TSharedPtr<FEOSVoiceManagerLocalUser> VoiceUser =
                                        This->VoiceManager->GetLocalUser(*EOSUser);
                                    if (VoiceUser.IsValid())
                                    {
                                        EOS_Lobby_GetRTCRoomNameOptions GetRTCRoomNameOpts = {};
                                        GetRTCRoomNameOpts.ApiVersion = EOS_LOBBY_GETRTCROOMNAME_API_LATEST;
                                        GetRTCRoomNameOpts.LocalUserId = EOSUser->GetProductUserId();
                                        GetRTCRoomNameOpts.LobbyId = Info->LobbyId;

                                        FString RoomName;
                                        EOS_EResult GetRoomNameResult = EOSString_AnsiUnlimited::
                                            FromDynamicLengthApiCall<EOS_HLobby, EOS_Lobby_GetRTCRoomNameOptions>(
                                                This->EOSLobby,
                                                &GetRTCRoomNameOpts,
                                                EOS_Lobby_GetRTCRoomName,
                                                RoomName);
                                        if (GetRoomNameResult == EOS_EResult::EOS_Success)
                                        {
                                            bVoiceChatEnabled = true;
                                            VoiceUser->RegisterLobbyChannel(
                                                RoomName,
                                                bEnableEcho ? EVoiceChatChannelType::Echo
                                                            : EVoiceChatChannelType::NonPositional);
                                        }
                                    }
                                }
#endif

                                EOS_HLobbyDetails LobbyDetails = {};
                                EOS_Lobby_CopyLobbyDetailsHandleOptions HandleOpts = {};
                                HandleOpts.ApiVersion = EOS_LOBBY_COPYLOBBYDETAILSHANDLE_API_LATEST;
                                HandleOpts.LobbyId = Info->LobbyId;
                                HandleOpts.LocalUserId = EOSUser->GetProductUserId();
                                EOS_EResult GetLobbyDetailsResult =
                                    EOS_Lobby_CopyLobbyDetailsHandle(This->EOSLobby, &HandleOpts, &LobbyDetails);
                                if (GetLobbyDetailsResult != EOS_EResult::EOS_Success)
                                {
                                    EOS_Lobby_LeaveLobbyOptions Opts = {};
                                    Opts.ApiVersion = EOS_LOBBY_LEAVELOBBY_API_LATEST;
                                    Opts.LocalUserId = EOSUser->GetProductUserId();
                                    Opts.LobbyId = EOSPartyId->GetLobbyId();

                                    // Leave lobby to clean up.
                                    EOSRunOperation<
                                        EOS_HLobby,
                                        EOS_Lobby_LeaveLobbyOptions,
                                        EOS_Lobby_LeaveLobbyCallbackInfo>(
                                        This->EOSLobby,
                                        &Opts,
                                        *EOS_Lobby_LeaveLobby,
                                        [WeakThis = GetWeakThis(This), Delegate, LocalUserIdRef, EOSPartyId](
                                            const EOS_Lobby_LeaveLobbyCallbackInfo *Info) {
                                            if (auto This = PinWeakThis(WeakThis))
                                            {
                                                if (Info->ResultCode != EOS_EResult::EOS_Success)
                                                {
                                                    UE_LOG(
                                                        LogEOS,
                                                        Error,
                                                        TEXT("JoinLobby: Error when performing EOS_Lobby_LeaveLobby "
                                                             "operation due to partially failed join operation: %s! "
                                                             "Expect the party system to be in an inconsistent state!"),
                                                        ANSI_TO_TCHAR(EOS_EResult_ToString(Info->ResultCode)));
                                                    return;
                                                }
                                            }
                                        });

                                    if (Info->ResultCode == EOS_EResult::EOS_NotFound)
                                    {
                                        UE_LOG(
                                            LogEOS,
                                            Error,
                                            TEXT("JoinParty: Error when performing EOS_Lobby_CopyLobbyDetailsHandle "
                                                 "operation: %s (this can happen if you were immediately kicked from "
                                                 "the party)"),
                                            ANSI_TO_TCHAR(EOS_EResult_ToString(GetLobbyDetailsResult)));
                                    }
                                    else
                                    {
                                        UE_LOG(
                                            LogEOS,
                                            Error,
                                            TEXT("JoinParty: Error when performing EOS_Lobby_CopyLobbyDetailsHandle "
                                                 "operation: %s"),
                                            ANSI_TO_TCHAR(EOS_EResult_ToString(GetLobbyDetailsResult)));
                                    }
                                    Delegate.ExecuteIfBound(
                                        LocalUserIdRef.Get(),
                                        EOSPartyId.Get(),
                                        EJoinPartyCompletionResult::UnknownClientFailure,
                                        0);
                                    MutexRelease();
                                    return;
                                }

                                auto PartyId = MakeShared<FOnlinePartyIdEOS>(Info->LobbyId);
                                auto Party = MakeShared<FOnlinePartyEOS>(
                                    This->EOSPlatform,
                                    LobbyDetails,
                                    LocalUserIdRef,
                                    This->AsShared(),
                                    This->UserFactory,
                                    bVoiceChatEnabled,
                                    nullptr);

                                auto Finalize = [WeakThis = GetWeakThis(This),
                                                 Party,
                                                 EOSJoinInfo,
                                                 LocalUserIdRef,
                                                 PartyId,
                                                 Delegate,
                                                 EOSPartyId,
                                                 MutexRelease](const FOnlineError &Error) {
                                    if (auto This = PinWeakThis(WeakThis))
                                    {
                                        if (!Error.bSucceeded)
                                        {
                                            UE_LOG(
                                                LogEOS,
                                                Warning,
                                                TEXT("JoinParty: One or more optional operations failed: %s"),
                                                *Error.ToLogString());
                                        }

                                        UE_LOG(LogEOS, Verbose, TEXT("JoinParty: Added party to parties list"));
                                        if (!This->JoinedParties.Contains(*LocalUserIdRef))
                                        {
                                            This->JoinedParties.Add(
                                                *LocalUserIdRef,
                                                TArray<TSharedPtr<FOnlinePartyEOS>>());
                                        }
                                        This->JoinedParties[*LocalUserIdRef].Add(Party);

                                        // We've "consumed" this join info, so search through pending invites and
                                        // remove them all.
                                        bool bInvitesChanged = false;
                                        if (This->PendingInvites.Contains(*LocalUserIdRef))
                                        {
                                            for (int i = This->PendingInvites[*LocalUserIdRef].Num() - 1; i >= 0; i--)
                                            {
                                                auto PendingInvite = This->PendingInvites[*LocalUserIdRef][i];
                                                if (*PendingInvite->GetPartyId() == *EOSJoinInfo->GetPartyId())
                                                {
                                                    This->PendingInvites[*LocalUserIdRef].RemoveAt(i);
#if defined(UE_5_1_OR_LATER)
                                                    This->TriggerOnPartyInviteRemovedExDelegates(
                                                        *LocalUserIdRef,
                                                        *PendingInvite,
                                                        EPartyInvitationRemovedReason::Accepted);
#else
                                                    This->TriggerOnPartyInviteRemovedDelegates(
                                                        *LocalUserIdRef,
                                                        *PendingInvite->GetPartyId(),
                                                        *PendingInvite->GetSourceUserId(),
                                                        EPartyInvitationRemovedReason::Accepted);
#endif
                                                    bInvitesChanged = true;
                                                }
                                            }
                                            if (This->PendingInvites[*LocalUserIdRef].Num() == 0)
                                            {
                                                This->PendingInvites.Remove(*LocalUserIdRef);
                                            }
                                        }
                                        if (bInvitesChanged)
                                        {
                                            This->TriggerOnPartyInvitesChangedDelegates(LocalUserIdRef.Get());
                                        }

                                        // Fire party join event (because we joined it).
                                        This->TriggerOnPartyJoinedDelegates(LocalUserIdRef.Get(), PartyId.Get());

                                        // Allow party-related events to fire now.
                                        Party->bReadyForEvents = true;

                                        UE_LOG(LogEOS, Verbose, TEXT("JoinParty: Succeeded"));
                                        Delegate.ExecuteIfBound(
                                            LocalUserIdRef.Get(),
                                            EOSPartyId.Get(),
                                            EJoinPartyCompletionResult::Succeeded,
                                            0);
                                        MutexRelease();
                                        return;
                                    }
                                };

                                if (This->ShouldHaveSyntheticParty(Party->PartyTypeId, *Party->GetConfiguration()))
                                {
                                    if (TSharedPtr<FSyntheticPartySessionManager> SPM =
                                            This->SyntheticPartySessionManager.Pin())
                                    {
                                        SPM->CreateSyntheticParty(
                                            PartyId,
                                            FSyntheticPartySessionOnComplete::CreateLambda(Finalize));
                                    }
                                    else
                                    {
                                        // No synthetic party manager available.
                                        Finalize(OnlineRedpointEOS::Errors::Success());
                                    }
                                }
                                else
                                {
                                    // This party doesn't have cross-platform presence.
                                    Finalize(OnlineRedpointEOS::Errors::Success());
                                }
                            }
                        });
                }
            };

            auto LeaveLobbyWithPresenceIfNecessary = [WeakThis = GetWeakThis(This),
                                                      EOSUser,
                                                      EOSJoinInfo,
                                                      JoinWithLobbyDetails,
                                                      Delegate,
                                                      LocalUserIdRef,
                                                      EOSPartyId,
                                                      MutexRelease](
                                                         EOS_HLobbyDetails InLobbyDetails,
                                                         bool ReleaseHandle) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    auto ExistingPartyOfType =
                        This->GetParty(EOSUser.Get(), EOSJoinInfo.ToSharedRef().Get().GetPartyTypeId());
                    if (!(ExistingPartyOfType != nullptr &&
                          ExistingPartyOfType->PartyTypeId.GetValue() ==
                              FOnlinePartySystemEOS::GetPrimaryPartyTypeId().GetValue()))
                    {
                        // No need to leave an existing party because there's no type conflict.
                        JoinWithLobbyDetails(InLobbyDetails, ReleaseHandle);
                        return;
                    }

                    UE_LOG(
                        LogEOS,
                        Verbose,
                        TEXT("FOnlinePartySystemEOS::DoLeaveParty is being called because there can only be one "
                             "presence party"));
                    // We have to leave our existing lobby before we can join the new one, because
                    // the new lobby will also have presence enabled, and we are currently in a party
                    // that has presence.
                    if (!This->LeaveParty(
                            EOSUser.Get(),
                            ExistingPartyOfType->PartyId.Get(),
                            FOnLeavePartyComplete::CreateLambda([Delegate,
                                                                 LocalUserIdRef,
                                                                 EOSPartyId,
                                                                 MutexRelease,
                                                                 JoinWithLobbyDetails,
                                                                 InLobbyDetails,
                                                                 ReleaseHandle](
                                                                    const FUniqueNetId &LocalUserIdUnused,
                                                                    const FOnlinePartyId &PartyIdUnused,
                                                                    const ELeavePartyCompletionResult Result) {
                                if (Result != ELeavePartyCompletionResult::Succeeded)
                                {
                                    UE_LOG(
                                        LogEOS,
                                        Error,
                                        TEXT("JoinParty: Error when performing LeaveLobby operation (see above logs)"));

                                    // We won't be able to join the new lobby successfully.
                                    Delegate.ExecuteIfBound(
                                        LocalUserIdRef.Get(),
                                        EOSPartyId.Get(),
                                        EJoinPartyCompletionResult::AlreadyInPartyOfSpecifiedType,
                                        0);
                                    MutexRelease();
                                    return;
                                }

                                // Otherwise, we have left our old party and can now join the new one.
                                JoinWithLobbyDetails(InLobbyDetails, ReleaseHandle);
                            })))
                    {
                        // We couldn't leave the lobby, so fail.
                        UE_LOG(
                            LogEOS,
                            Error,
                            TEXT("JoinParty: Error when performing LeaveLobby operation (call failed)"));
                        Delegate.ExecuteIfBound(
                            LocalUserIdRef.Get(),
                            EOSPartyId.Get(),
                            EJoinPartyCompletionResult::AlreadyInPartyOfSpecifiedType,
                            0);
                        MutexRelease();
                        return;
                    }
                }
            };

            EOS_HLobbyDetails LobbyDetails = {};
            if (EOSJoinInfo->GetLobbyHandle(LobbyDetails))
            {
                // We can just join directly using the handle in the join info.
                LeaveLobbyWithPresenceIfNecessary(LobbyDetails, false);
                return;
            }

            // No handle, so we'll need to search for it based on the lobby ID.
            TSharedRef<const FOnlinePartyId> PartyIdRef = EOSJoinInfo->GetPartyId();
            TSharedRef<const FOnlinePartyIdEOS> IdOptsLobbyId =
                StaticCastSharedRef<const FOnlinePartyIdEOS>(PartyIdRef);
            This->LookupPartyById(
                IdOptsLobbyId->GetLobbyId(),
                EOSUser->GetProductUserId(),
                [Delegate, LocalUserIdRef, EOSPartyId, MutexRelease, LeaveLobbyWithPresenceIfNecessary](
                    EOS_HLobbyDetails SearchLobbyHandle) {
                    if (SearchLobbyHandle == nullptr)
                    {
                        UE_LOG(LogEOS, Error, TEXT("JoinParty: Failed to resolve party ID to lobby handle"));
                        Delegate.ExecuteIfBound(
                            LocalUserIdRef.Get(),
                            EOSPartyId.Get(),
                            EJoinPartyCompletionResult::UnknownClientFailure,
                            0);
                        MutexRelease();
                        return;
                    }

                    LeaveLobbyWithPresenceIfNecessary(SearchLobbyHandle, true);
                });
        }
    });
    return true;
}

#if defined(EOS_PARTY_SYSTEM_REQUEST_TO_JOIN_PARTY)

void FOnlinePartySystemEOS::RequestToJoinParty(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyTypeId PartyTypeId,
    const FPartyInvitationRecipient &Recipient,
    const FOnRequestToJoinPartyComplete &Delegate)
{
    UE_LOG(
        LogEOS,
        Error,
        TEXT("%s"),
        *OnlineRedpointEOS::Errors::NotImplemented(
             LocalUserId,
             TEXT("FOnlinePartySystemEOS::QueryPartyJoinability"),
             TEXT("RequestToJoinParty is not supported."))
             .ToLogString());
    Delegate.ExecuteIfBound(
        LocalUserId,
        LocalUserId,
        FDateTime::MinValue(),
        ERequestToJoinPartyCompletionResult::UnknownInternalFailure);
}

void FOnlinePartySystemEOS::ClearRequestToJoinParty(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FUniqueNetId &Sender,
    EPartyRequestToJoinRemovedReason Reason)
{
    UE_LOG(
        LogEOS,
        Error,
        TEXT("%s"),
        *OnlineRedpointEOS::Errors::NotImplemented(
             LocalUserId,
             TEXT("FOnlinePartySystemEOS::ClearRequestToJoinParty"),
             TEXT("ClearRequestToJoinParty is not supported."))
             .ToLogString());
}

#endif // #if defined(EOS_PARTY_SYSTEM_REQUEST_TO_JOIN_PARTY)

void FOnlinePartySystemEOS::LookupPartyById(
    EOS_LobbyId InId,
    EOS_ProductUserId InSearchingUser,
    const std::function<void(EOS_HLobbyDetails Handle)> &OnDone)
{
    EOS_HLobbySearch LobbySearch = {};
    EOS_Lobby_CreateLobbySearchOptions SearchOpts = {};
    SearchOpts.ApiVersion = EOS_LOBBY_CREATELOBBYSEARCH_API_LATEST;
    SearchOpts.MaxResults = 1;
    if (EOS_Lobby_CreateLobbySearch(this->EOSLobby, &SearchOpts, &LobbySearch) != EOS_EResult::EOS_Success)
    {
        OnDone(nullptr);
        return;
    }

    EOS_LobbySearch_SetLobbyIdOptions IdOpts = {};
    IdOpts.ApiVersion = EOS_LOBBYSEARCH_SETLOBBYID_API_LATEST;
    IdOpts.LobbyId = InId;
    if (EOS_LobbySearch_SetLobbyId(LobbySearch, &IdOpts) != EOS_EResult::EOS_Success)
    {
        OnDone(nullptr);
        return;
    }

    EOS_LobbySearch_FindOptions FindOpts = {};
    FindOpts.ApiVersion = EOS_LOBBYSEARCH_FIND_API_LATEST;
    FindOpts.LocalUserId = InSearchingUser;
    EOSRunOperation<EOS_HLobbySearch, EOS_LobbySearch_FindOptions, EOS_LobbySearch_FindCallbackInfo>(
        LobbySearch,
        &FindOpts,
        *EOS_LobbySearch_Find,
        [LobbySearch, OnDone](const EOS_LobbySearch_FindCallbackInfo *Info) {
            if (Info->ResultCode != EOS_EResult::EOS_Success)
            {
                EOS_LobbySearch_Release(LobbySearch);
                OnDone(nullptr);
                return;
            }

            EOS_LobbySearch_GetSearchResultCountOptions CountOpts = {};
            CountOpts.ApiVersion = EOS_LOBBYSEARCH_GETSEARCHRESULTCOUNT_API_LATEST;
            auto Count = EOS_LobbySearch_GetSearchResultCount(LobbySearch, &CountOpts);
            if (Count == 0)
            {
                EOS_LobbySearch_Release(LobbySearch);
                OnDone(nullptr);
                return;
            }

            EOS_HLobbyDetails SearchLobbyHandle = {};
            EOS_LobbySearch_CopySearchResultByIndexOptions CopyOpts = {};
            CopyOpts.ApiVersion = EOS_LOBBYSEARCH_COPYSEARCHRESULTBYINDEX_API_LATEST;
            CopyOpts.LobbyIndex = 0;
            if (EOS_LobbySearch_CopySearchResultByIndex(LobbySearch, &CopyOpts, &SearchLobbyHandle) !=
                EOS_EResult::EOS_Success)
            {
                EOS_LobbySearch_Release(LobbySearch);
                OnDone(nullptr);
                return;
            }

            EOS_LobbySearch_Release(LobbySearch);
            OnDone(SearchLobbyHandle);
        });
}

bool FOnlinePartySystemEOS::JIPFromWithinParty(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FUniqueNetId &PartyLeaderId)
{
    UE_LOG(LogEOS, Error, TEXT("JIPFromWithinParty not supported"));
    return false;
}

#if !defined(UE_5_1_OR_LATER)
void FOnlinePartySystemEOS::QueryPartyJoinability(
    const FUniqueNetId &LocalUserId,
    const IOnlinePartyJoinInfo &OnlinePartyJoinInfo,
    const FOnQueryPartyJoinabilityComplete &Delegate)
{
    UE_LOG(LogEOS, Error, TEXT("QueryPartyJoinability not supported"));
    Delegate.ExecuteIfBound(
        LocalUserId,
        OnlinePartyJoinInfo.GetPartyId().Get(),
        EJoinPartyCompletionResult::UnknownClientFailure,
        0);
}
#endif

#if defined(EOS_PARTY_SYSTEM_HAS_QUERY_PARTY_JOINABILITY_EX)
void FOnlinePartySystemEOS::QueryPartyJoinability(
    const FUniqueNetId &LocalUserId,
    const IOnlinePartyJoinInfo &OnlinePartyJoinInfo,
    const FOnQueryPartyJoinabilityCompleteEx &Delegate)
{
    UE_LOG(LogEOS, Error, TEXT("QueryPartyJoinability not supported"));
    FQueryPartyJoinabilityResult Result;
    Result.Result = OnlineRedpointEOS::Errors::NotImplemented(LocalUserId);
    Delegate.ExecuteIfBound(LocalUserId, OnlinePartyJoinInfo.GetPartyId().Get(), Result);
}
#endif

bool FOnlinePartySystemEOS::RejoinParty(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FOnlinePartyTypeId &PartyTypeId,
    const TArray<TSharedRef<const FUniqueNetId>> &FormerMembers,
    const FOnJoinPartyComplete &Delegate)
{
    UE_LOG(LogEOS, Error, TEXT("RejoinParty not supported"));
    return false;
}

bool FOnlinePartySystemEOS::LeaveParty(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FOnLeavePartyComplete &Delegate)
{
    UE_LOG(
        LogEOS,
        Verbose,
        TEXT("FOnlinePartySystemEOS::LeaveParty was called to have user %s leave party %s"),
        *LocalUserId.ToString(),
        *PartyId.ToString());
    return this->LeaveParty(LocalUserId, PartyId, false, Delegate);
}

bool FOnlinePartySystemEOS::LeaveParty(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    bool bSynchronizeLeave,
    const FOnLeavePartyComplete &Delegate)
{
    UE_LOG(
        LogEOS,
        Verbose,
        TEXT("FOnlinePartySystemEOS::LeaveParty was called to have user %s leave party %s"),
        *LocalUserId.ToString(),
        *PartyId.ToString());
    if (auto SPM = this->SyntheticPartySessionManager.Pin())
    {
        auto LocalUserIdRef = LocalUserId.AsShared();
        auto PartyIdRef = PartyId.AsShared();
        if (SPM->HasSyntheticParty(PartyId.AsShared()))
        {
            SPM->DeleteSyntheticParty(
                PartyId.AsShared(),
                FSyntheticPartySessionOnComplete::CreateLambda([WeakThis = GetWeakThis(this),
                                                                LocalUserIdRef,
                                                                PartyIdRef,
                                                                bSynchronizeLeave,
                                                                Delegate](const FOnlineError &Error) {
                    if (auto This = PinWeakThis(WeakThis))
                    {
                        if (!Error.bSucceeded)
                        {
                            UE_LOG(
                                LogEOS,
                                Warning,
                                TEXT("FOnlinePartySystemEOS::LeaveParty: An optional operation failed: %s"),
                                *Error.ToLogString());
                        }

                        if (!This->DoLeaveParty(LocalUserIdRef.Get(), PartyIdRef.Get(), bSynchronizeLeave, Delegate))
                        {
                            Delegate.ExecuteIfBound(
                                LocalUserIdRef.Get(),
                                PartyIdRef.Get(),
                                ELeavePartyCompletionResult::UnknownInternalFailure);
                        }
                    }
                }));
            return true;
        }
    }

    return this->DoLeaveParty(LocalUserId, PartyId, bSynchronizeLeave, Delegate);
}

bool FOnlinePartySystemEOS::DoLeaveParty(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    bool bSynchronizeLeave,
    const FOnLeavePartyComplete &Delegate)
{
    UE_LOG(
        LogEOS,
        Verbose,
        TEXT("FOnlinePartySystemEOS::DoLeaveParty was called to have user %s leave party %s"),
        *LocalUserId.ToString(),
        *PartyId.ToString());
    if (LocalUserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("DoLeaveParty: Local user ID type is not EOS"));
        Delegate.ExecuteIfBound(LocalUserId, PartyId, ELeavePartyCompletionResult::UnknownClientFailure);
        return true;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(LocalUserId.AsShared());
    if (!EOSUser->HasValidProductUserId())
    {
        UE_LOG(LogEOS, Error, TEXT("DoLeaveParty: Local EOS user does not have a valid product user ID"));
        Delegate.ExecuteIfBound(LocalUserId, PartyId, ELeavePartyCompletionResult::UnknownClientFailure);
        return true;
    }

    auto LocalUserIdRef = LocalUserId.AsShared();
    auto EOSPartyId = StaticCastSharedRef<const FOnlinePartyIdEOS>(PartyId.AsShared());

    FString RTCRoomName;
#if EOS_VERSION_AT_LEAST(1, 13, 0) && defined(EOS_VOICE_CHAT_SUPPORTED)
    // Before we leave, try to get the RTC room name. If we can get it, then we need to unregister this RTC channel
    // after we leave successfully.
    {
        EOS_Lobby_GetRTCRoomNameOptions GetRTCRoomNameOpts = {};
        GetRTCRoomNameOpts.ApiVersion = EOS_LOBBY_GETRTCROOMNAME_API_LATEST;
        GetRTCRoomNameOpts.LocalUserId = EOSUser->GetProductUserId();
        GetRTCRoomNameOpts.LobbyId = EOSPartyId->GetLobbyId();

        FString RoomName;
        EOS_EResult GetRoomNameResult =
            EOSString_AnsiUnlimited::FromDynamicLengthApiCall<EOS_HLobby, EOS_Lobby_GetRTCRoomNameOptions>(
                this->EOSLobby,
                &GetRTCRoomNameOpts,
                EOS_Lobby_GetRTCRoomName,
                RTCRoomName);
        if (GetRoomNameResult != EOS_EResult::EOS_Success)
        {
            RTCRoomName = TEXT("");
        }
    }
#endif

    EOS_Lobby_LeaveLobbyOptions Opts = {};
    Opts.ApiVersion = EOS_LOBBY_LEAVELOBBY_API_LATEST;
    Opts.LocalUserId = EOSUser->GetProductUserId();
    Opts.LobbyId = EOSPartyId->GetLobbyId();

    EOSRunOperation<EOS_HLobby, EOS_Lobby_LeaveLobbyOptions, EOS_Lobby_LeaveLobbyCallbackInfo>(
        this->EOSLobby,
        &Opts,
        *EOS_Lobby_LeaveLobby,
        [WeakThis = GetWeakThis(this), Delegate, LocalUserIdRef, EOSUser, EOSPartyId, RTCRoomName](
            const EOS_Lobby_LeaveLobbyCallbackInfo *Info) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Info->ResultCode != EOS_EResult::EOS_Success)
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("DoLeaveParty: Error when performing EOS_Lobby_LeaveLobby operation: %s"),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Info->ResultCode)));
                    Delegate.ExecuteIfBound(
                        LocalUserIdRef.Get(),
                        EOSPartyId.Get(),
                        ELeavePartyCompletionResult::UnknownClientFailure);
                    return;
                }

                // We do not get an EOS_LMS_LEFT event for ourself when we leave a party manually, so emulate the event
                // as needed.
                for (const auto &Party : TArray<TSharedPtr<FOnlinePartyEOS>>(This->JoinedParties[*LocalUserIdRef]))
                {
                    if (*Party->PartyId == *EOSPartyId)
                    {
                        This->MemberStatusChanged(
                            EOSPartyId->GetLobbyId(),
                            EOSUser->GetProductUserId(),
                            EOS_ELobbyMemberStatus::EOS_LMS_LEFT);
#if EOS_VERSION_AT_LEAST(1, 13, 0) && defined(EOS_VOICE_CHAT_SUPPORTED)
                        if (Party->IsRTCEnabled() && !RTCRoomName.IsEmpty())
                        {
                            TSharedPtr<FEOSVoiceManagerLocalUser> VoiceUser =
                                This->VoiceManager->GetLocalUser(*EOSUser);
                            if (VoiceUser.IsValid())
                            {
                                VoiceUser->UnregisterLobbyChannel(RTCRoomName);
                            }
                        }
#endif
                    }
                }

                UE_LOG(LogEOS, Verbose, TEXT("DoLeaveParty: Party left successfully"));
                Delegate.ExecuteIfBound(LocalUserIdRef.Get(), EOSPartyId.Get(), ELeavePartyCompletionResult::Succeeded);
            }
        });
    return true;
}

bool FOnlinePartySystemEOS::IsLocalUserInParty(const FOnlinePartyId &LobbyId, const FUniqueNetId &LocalUserId) const
{
    if (!this->JoinedParties.Contains(LocalUserId))
    {
        return false;
    }

    for (const auto &Party : this->JoinedParties[LocalUserId])
    {
        if (*Party->PartyId == LobbyId)
        {
            return true;
        }
    }

    return false;
}

bool FOnlinePartySystemEOS::ShouldHaveSyntheticParty(
    const FOnlinePartyTypeId &PartyTypeId,
    const FPartyConfiguration &PartyConfiguration) const
{
    if (this->Config->GetPresenceAdvertisementType() != EPresenceAdvertisementType::Party)
    {
        // Game does not advertise parties via presence.
        return false;
    }

    if (!PartyConfiguration.bIsAcceptingMembers)
    {
        // This party isn't accepting members.
        return false;
    }

    if (PartyConfiguration.InvitePermissions != PartySystemPermissions::EPermissionType::Anyone)
    {
        // This party is not publicly accessible via presence, so cross-platform invites can't work.
        return false;
    }

    if (PartyTypeId != this->GetPrimaryPartyTypeId())
    {
        // This isn't the primary party, so don't advertise it.
        return false;
    }

    // This party should be advertised cross-platform.
    return true;
}

bool FOnlinePartySystemEOS::ApproveJoinRequest(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FUniqueNetId &RecipientId,
    bool bIsApproved,
    int32 DeniedResultCode)
{
    UE_LOG(LogEOS, Error, TEXT("ApproveJoinRequest not supported!"));
    return false;
}

bool FOnlinePartySystemEOS::ApproveJIPRequest(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FUniqueNetId &RecipientId,
    bool bIsApproved,
    int32 DeniedResultCode)
{
    UE_LOG(LogEOS, Error, TEXT("ApproveJIPRequest not supported!"));
    return false;
}

#if !defined(UE_5_1_OR_LATER)
void FOnlinePartySystemEOS::RespondToQueryJoinability(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FUniqueNetId &RecipientId,
    bool bCanJoin,
    int32 DeniedResultCode)
{
    UE_LOG(LogEOS, Error, TEXT("RespondToQueryJoinability not supported!"));
}
#endif

#if defined(EOS_PARTY_SYSTEM_HAS_RESPOND_TO_QUERY_JOINABILITY)
void FOnlinePartySystemEOS::RespondToQueryJoinability(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FUniqueNetId &RecipientId,
    bool bCanJoin,
    int32 DeniedResultCode,
    FOnlinePartyDataConstPtr PartyData)
{
    UE_LOG(LogEOS, Error, TEXT("RespondToQueryJoinability not supported!"));
}
#endif

bool FOnlinePartySystemEOS::SendInvitation(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FPartyInvitationRecipient &Recipient,
    const FOnSendPartyInvitationComplete &Delegate)
{
    if (LocalUserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        Delegate.ExecuteIfBound(
            LocalUserId,
            PartyId,
            Recipient.Id.Get(),
            ESendPartyInvitationCompletionResult::NotLoggedIn);
        return true;
    }

    auto LocalUserIdRef = StaticCastSharedRef<const FUniqueNetIdEOS>(LocalUserId.AsShared());
    if (!LocalUserIdRef->HasValidProductUserId())
    {
        Delegate.ExecuteIfBound(
            LocalUserId,
            PartyId,
            Recipient.Id.Get(),
            ESendPartyInvitationCompletionResult::NotLoggedIn);
        return true;
    }

    auto EOSPartyId = StaticCastSharedRef<const FOnlinePartyIdEOS>(PartyId.AsShared());

    // Make a list of invocations so we can FMultiOperation over them, to ensure we only fire the callback delegate
    // once.
    TArray<std::function<void(const FSyntheticPartySessionOnComplete &OnDone)>> Invocations;

    if (Recipient.Id->GetType().IsEqual(REDPOINT_EOS_SUBSYSTEM))
    {
        // Check to see if the target is a synthetic friend. If they are, send over the wrapped subsystems as well.
        int32 LocalUserNum;
        if (this->Identity->GetLocalUserNum(LocalUserId, LocalUserNum))
        {
            TSharedPtr<FOnlineFriend> Friend = this->Friends->GetFriend(LocalUserNum, *Recipient.Id, TEXT(""));
            if (Friend.IsValid())
            {
                FString _;
                if (Friend->GetUserAttribute(TEXT("eosSynthetic.subsystemNames"), _))
                {
                    if (auto SPM = this->SyntheticPartySessionManager.Pin())
                    {
                        if (SPM.IsValid())
                        {
                            // This is a synthetic friend. Cast to FOnlineFriendSynthetic so we can access
                            // GetWrappedFriends() directly.
                            const TMap<FName, TSharedPtr<FOnlineFriend>> &WrappedFriends =
                                StaticCastSharedPtr<FOnlineFriendSynthetic>(Friend)->GetWrappedFriends();
                            for (const auto &WrappedFriend : WrappedFriends)
                            {
                                Invocations.Add([SPM,
                                                 LocalUserNum,
                                                 PartyId = PartyId.AsShared(),
                                                 FriendId = WrappedFriend.Value->GetUserId()](
                                                    const FSyntheticPartySessionOnComplete &OnDone) {
                                    SPM->SendInvitationToParty(LocalUserNum, PartyId, FriendId, OnDone);
                                });
                            }
                        }
                        else
                        {
                            UE_LOG(
                                LogEOS,
                                Warning,
                                TEXT("SendInvitation: Target was synthetic friend, but synthetic party manager was not "
                                     "valid. Unable to send invitation over native subsystems."));
                        }
                    }
                    else
                    {
                        UE_LOG(
                            LogEOS,
                            Warning,
                            TEXT("SendInvitation: Target was synthetic friend, but synthetic party manager was not "
                                 "valid. Unable to send invitation over native subsystems."));
                    }
                }
            }
        }

        // Now also send over EOS as well so that the game can receive a notification of the incoming invitation.
        auto RecipientIdRef = StaticCastSharedRef<const FUniqueNetIdEOS>(Recipient.Id);
        if (RecipientIdRef->HasValidProductUserId())
        {
            Invocations.Add([WeakThis = GetWeakThis(this), EOSPartyId, LocalUserIdRef, RecipientIdRef](
                                const FSyntheticPartySessionOnComplete &OnDone) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    // This is an EOS user, so send the invitation directly.
                    EOS_Lobby_SendInviteOptions Opts = {};
                    Opts.ApiVersion = EOS_LOBBY_SENDINVITE_API_LATEST;
                    Opts.LobbyId = EOSPartyId->GetLobbyId();
                    Opts.LocalUserId = LocalUserIdRef->GetProductUserId();
                    Opts.TargetUserId = RecipientIdRef->GetProductUserId();

                    int *AttemptCount = new int(0);
                    FHeapLambda<EHeapLambdaFlags::OneShotCleanup> CleanupCounter;
                    CleanupCounter = [AttemptCount]() {
                        delete AttemptCount;
                    };

                    FHeapLambda<EHeapLambdaFlags::None> AttemptSend;
                    AttemptSend = [WeakThis = GetWeakThis(This),
                                   AttemptSend,
                                   AttemptCount,
                                   CleanupCounter,
                                   Opts,
                                   OnDone,
                                   LocalUserIdRef,
                                   EOSPartyId,
                                   RecipientIdRef]() {
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            EOSRunOperation<EOS_HLobby, EOS_Lobby_SendInviteOptions, EOS_Lobby_SendInviteCallbackInfo>(
                                This->EOSLobby,
                                &Opts,
                                *EOS_Lobby_SendInvite,
                                [WeakThis = GetWeakThis(This),
                                 AttemptSend,
                                 AttemptCount,
                                 CleanupCounter,
                                 OnDone,
                                 LocalUserIdRef,
                                 EOSPartyId,
                                 RecipientIdRef](const EOS_Lobby_SendInviteCallbackInfo *Info) {
                                    if (auto This = PinWeakThis(WeakThis))
                                    {
                                        if (Info->ResultCode == EOS_EResult::EOS_NotFound)
                                        {
                                            FOnlinePartyConstPtr PartyRef =
                                                This->GetParty(*LocalUserIdRef, *EOSPartyId);
                                            if (PartyRef.IsValid())
                                            {
                                                // We are in this party, so we *know* it exists.
                                                if (*AttemptCount < 3)
                                                {
                                                    UE_LOG(
                                                        LogEOS,
                                                        Verbose,
                                                        TEXT("SendInvitation could not find party on backend, trying "
                                                             "exponential "
                                                             "backoff attempt #%d..."),
                                                        (*AttemptCount) + 1);

                                                    // This is an eventual consistency issue and we must retry later.
                                                    (*AttemptCount)++;
                                                    FUTicker::GetCoreTicker().AddTicker(
                                                        FTickerDelegate::CreateLambda([WeakThis = GetWeakThis(This),
                                                                                       AttemptSend](float DeltaTime) {
                                                            if (auto This = PinWeakThis(WeakThis))
                                                            {
                                                                AttemptSend();
                                                            }
                                                            return false;
                                                        }),
                                                        2.0f * (float)(*AttemptCount));
                                                    return;
                                                }

                                                // Otherwise, we are out of retries. Fallthrough to the != EOS_Success
                                                // case below.
                                            }
                                        }

                                        if (Info->ResultCode != EOS_EResult::EOS_Success)
                                        {
                                            UE_LOG(
                                                LogEOS,
                                                Error,
                                                TEXT("SendInvitation failed: %s"),
                                                ANSI_TO_TCHAR(EOS_EResult_ToString(Info->ResultCode)));
                                            OnDone.ExecuteIfBound(ConvertError(
                                                TEXT("FOnlinePartySystemEOS::SendInvitation"),
                                                TEXT("SendInvitation to EOS user account failed."),
                                                Info->ResultCode));
                                            CleanupCounter();
                                            return;
                                        }

                                        OnDone.ExecuteIfBound(OnlineRedpointEOS::Errors::Success());
                                        CleanupCounter();
                                    }
                                });
                        }
                    };
                    AttemptSend();
                }
            });
        }

        if (Invocations.Num() == 0)
        {
            Delegate.ExecuteIfBound(
                LocalUserId,
                PartyId,
                Recipient.Id.Get(),
                ESendPartyInvitationCompletionResult::UnknownInternalFailure);
            return true;
        }

        FMultiOperation<std::function<void(const FSyntheticPartySessionOnComplete &OnDone)>, FOnlineError>::Run(
            Invocations,
            [](const std::function<void(const FSyntheticPartySessionOnComplete &OnDone)> &Handler,
               const std::function<void(FOnlineError)> &OnDone) {
                Handler(FSyntheticPartySessionOnComplete::CreateLambda([OnDone](const FOnlineError &Error) {
                    OnDone(Error);
                }));
                return true;
            },
            [Delegate, LocalUserIdRef, EOSPartyId, RecipientIdRef](const TArray<FOnlineError> &Results) {
                for (const auto &Result : Results)
                {
                    if (!Result.bSucceeded)
                    {
                        Delegate.ExecuteIfBound(
                            *LocalUserIdRef,
                            *EOSPartyId,
                            *RecipientIdRef,
                            ConvertErrorTo_ESendPartyInvitationCompletionResult(Result));
                        return;
                    }
                }

                Delegate.ExecuteIfBound(
                    *LocalUserIdRef,
                    *EOSPartyId,
                    *RecipientIdRef,
                    ESendPartyInvitationCompletionResult::Succeeded);
            });
        return true;
    }
    else
    {
        if (auto SPM = this->SyntheticPartySessionManager.Pin())
        {
            if (SPM.IsValid())
            {
                int32 LocalUserNum;
                if (this->Identity->GetLocalUserNum(LocalUserId, LocalUserNum))
                {
                    SPM->SendInvitationToParty(
                        LocalUserNum,
                        PartyId.AsShared(),
                        Recipient.Id,
                        FSyntheticPartySessionOnComplete::CreateLambda(
                            [WeakThis = GetWeakThis(this),
                             Delegate,
                             EOSPartyId,
                             LocalUserIdRef,
                             RecipientIdRef = Recipient.Id](const FOnlineError &Error) {
                                if (auto This = PinWeakThis(WeakThis))
                                {
                                    if (!Error.bSucceeded)
                                    {
                                        Delegate.ExecuteIfBound(
                                            *LocalUserIdRef,
                                            *EOSPartyId,
                                            *RecipientIdRef,
                                            ConvertErrorTo_ESendPartyInvitationCompletionResult(Error));
                                    }
                                    else
                                    {
                                        Delegate.ExecuteIfBound(
                                            *LocalUserIdRef,
                                            *EOSPartyId,
                                            *RecipientIdRef,
                                            ESendPartyInvitationCompletionResult::Succeeded);
                                    }
                                }
                            }));
                    return true;
                }
                else
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("SendInvitation: Unable to get local user ID when sending to native friend."));
                }
            }
        }

        UE_LOG(
            LogEOS,
            Error,
            TEXT("SendInvitation: Target was native friend, but synthetic party manager was not "
                 "valid. Unable to send invitation at all."));
        return false;
    }
}

#if defined(UE_5_1_OR_LATER)
void FOnlinePartySystemEOS::CancelInvitation(
    const FUniqueNetId &LocalUserId,
    const FUniqueNetId &TargetUserId,
    const FOnlinePartyId &PartyId,
    const FOnCancelPartyInvitationComplete &Delegate)
{
    UE_LOG(LogEOS, Error, TEXT("CancelInvitation not supported!"));
    Delegate.ExecuteIfBound(LocalUserId, PartyId, TargetUserId, OnlineRedpointEOS::Errors::NotImplemented());
}
#endif

bool FOnlinePartySystemEOS::RejectInvitation(const FUniqueNetId &LocalUserId, const FUniqueNetId &SenderId)
{
    if (!this->PendingInvites.Contains(LocalUserId))
    {
        // No invites anyway.
        return true;
    }

    if (LocalUserId.GetType() != REDPOINT_EOS_SUBSYSTEM || SenderId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("RejectInvitation: Local user ID or sender user ID were not EOS user IDs"));
        return false;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(LocalUserId.AsShared());
    if (!EOSUser->HasValidProductUserId())
    {
        UE_LOG(LogEOS, Error, TEXT("RejectInvitation: Local user ID did not have product user ID"));
        return false;
    }

    auto EOSSender = StaticCastSharedRef<const FUniqueNetIdEOS>(SenderId.AsShared());
    if (!EOSSender->HasValidProductUserId())
    {
        UE_LOG(LogEOS, Error, TEXT("RejectInvitation: Sender user ID did not have product user ID"));
        return false;
    }

    // Explicitly make a copy of the invites list (instead of auto&) so that
    // we don't assert when the "lobby invite rejected" event fires.
    auto Invites = this->PendingInvites[LocalUserId];
    TArray<IOnlinePartyJoinInfoConstRef> InvitesToRemove;
    for (const auto &PendingInvite : Invites)
    {
        auto SourceUser = PendingInvite->GetSourceUserId();
        if (SourceUser->GetType() == REDPOINT_EOS_SUBSYSTEM)
        {
            auto EOSSourceUser = StaticCastSharedRef<const FUniqueNetIdEOS>(SourceUser);
            if (EOSSourceUser.Get() == *EOSSender)
            {
                auto PendingInviteEOS = StaticCastSharedRef<const FOnlinePartyJoinInfoEOS>(PendingInvite);
                auto InviteId = StringCast<ANSICHAR>(*PendingInviteEOS->GetInviteId());

                InvitesToRemove.Add(PendingInvite);

                EOS_Lobby_RejectInviteOptions RejectOpts = {};
                RejectOpts.ApiVersion = EOS_LOBBY_REJECTINVITE_API_LATEST;
                RejectOpts.LocalUserId = EOSUser->GetProductUserId();
                RejectOpts.InviteId = InviteId.Get();
                checkf(
                    EOSString_ProductUserId::IsValid(RejectOpts.LocalUserId),
                    TEXT("LocalUserId passed into EOS_Lobby_RejectInviteOptions was invalid, even though we verified "
                         "it earlier during the RejectInvitation call (via synchronous RejectInvitation)!"));
                checkf(
                    !FString(ANSI_TO_TCHAR(RejectOpts.InviteId)).TrimStartAndEnd().IsEmpty(),
                    TEXT("InviteId was empty when RejectInvite was called (via synchronous RejectInvitation)!"));
                EOSRunOperation<EOS_HLobby, EOS_Lobby_RejectInviteOptions, EOS_Lobby_RejectInviteCallbackInfo>(
                    this->EOSLobby,
                    &RejectOpts,
                    EOS_Lobby_RejectInvite,
                    [WeakThis = GetWeakThis(this)](const EOS_Lobby_RejectInviteCallbackInfo *Info) {
                        // The RejectInvitation call calls EOS_Lobby_RejectInvite and leaves it to run in the
                        // background. If the online subsystem shuts down while this background call is still
                        // starting (this is common in the unit tests), then the product user ID in the LocalUserId
                        // field will already be freed. This in turn will cause the result code of this call to be
                        // EOS_InvalidParameter.
                        //
                        // Since the party interface does not provide an asynchronous RejectInvitation call, we use
                        // GetWeakThis/PinWeakThis to check if the subsystem is still alive; if it isn't, then we can
                        // safely ignore any error we get here.
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            if (Info->ResultCode != EOS_EResult::EOS_Success &&
                                Info->ResultCode != EOS_EResult::EOS_NotFound)
                            {
                                UE_LOG(
                                    LogEOS,
                                    Warning,
                                    TEXT("Rejecting invite failed, but we already returned true assuming it was "
                                         "rejected (got result code %s)"),
                                    ANSI_TO_TCHAR(EOS_EResult_ToString(Info->ResultCode)));
                            }
                        }
                    });
            }
        }
    }
    bool bInvitesChanged = false;
    for (const auto &PendingInvite : InvitesToRemove)
    {
        if (Invites.Remove(PendingInvite) > 0)
        {
            bInvitesChanged = true;
#if defined(UE_5_1_OR_LATER)
            this->TriggerOnPartyInviteRemovedExDelegates(
                *EOSUser,
                *PendingInvite,
                EPartyInvitationRemovedReason::Declined);
#else
            this->TriggerOnPartyInviteRemovedDelegates(
                *EOSUser,
                *PendingInvite->GetPartyId(),
                *EOSSender,
                EPartyInvitationRemovedReason::Declined);
#endif
        }
    }
    if (bInvitesChanged)
    {
        this->TriggerOnPartyInvitesChangedDelegates(*EOSUser);
    }
    return true;
}

bool FOnlinePartySystemEOS::RejectInvitation(
    const FUniqueNetId &LocalUserId,
    const IOnlinePartyJoinInfo &OnlinePartyJoinInfo,
    const FOnRejectPartyInvitationComplete &Delegate)
{
    if (!this->PendingInvites.Contains(LocalUserId))
    {
        // No invites anyway.
        return true;
    }

    if (LocalUserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("RejectInvitation: Local user ID was not an EOS user ID"));
        return false;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(LocalUserId.AsShared());
    if (!EOSUser->HasValidProductUserId())
    {
        UE_LOG(LogEOS, Error, TEXT("RejectInvitation: Local user ID did not have product user ID"));
        return false;
    }

    auto EOSJoinInfo = FOnlinePartyJoinInfoEOS::GetEOSJoinInfo(OnlinePartyJoinInfo);
    if (EOSJoinInfo == nullptr)
    {
        UE_LOG(LogEOS, Error, TEXT("RejectInvitation: Join info is not valid"));
        return false;
    }

    auto &Invites = this->PendingInvites[LocalUserId];
    for (const auto &PendingInvite : Invites)
    {
        if (*PendingInvite->GetPartyId() == *EOSJoinInfo->GetPartyId())
        {
            auto PendingInviteEOS = StaticCastSharedRef<const FOnlinePartyJoinInfoEOS>(PendingInvite);
            auto InviteId = StringCast<ANSICHAR>(*PendingInviteEOS->GetInviteId());

            EOS_Lobby_RejectInviteOptions RejectOpts = {};
            RejectOpts.ApiVersion = EOS_LOBBY_REJECTINVITE_API_LATEST;
            RejectOpts.LocalUserId = EOSUser->GetProductUserId();
            RejectOpts.InviteId = InviteId.Get();
            checkf(
                EOSString_ProductUserId::IsValid(RejectOpts.LocalUserId),
                TEXT("LocalUserId passed into EOS_Lobby_RejectInviteOptions was invalid, even though we verified "
                     "it earlier during the RejectInvitation call (via asynchronous RejectInvitation)!"));
            checkf(
                !FString(ANSI_TO_TCHAR(RejectOpts.InviteId)).TrimStartAndEnd().IsEmpty(),
                TEXT("InviteId was empty when RejectInvite was called (via asynchronous RejectInvitation)!"));
            EOSRunOperation<EOS_HLobby, EOS_Lobby_RejectInviteOptions, EOS_Lobby_RejectInviteCallbackInfo>(
                this->EOSLobby,
                &RejectOpts,
                EOS_Lobby_RejectInvite,
                [WeakThis = GetWeakThis(this), Delegate, EOSUser, PendingInviteEOS, PendingInvite](
                    const EOS_Lobby_RejectInviteCallbackInfo *Info) {
                    if (auto This = PinWeakThis(WeakThis))
                    {
                        if (Info->ResultCode != EOS_EResult::EOS_Success &&
                            Info->ResultCode != EOS_EResult::EOS_NotFound)
                        {
                            Delegate.ExecuteIfBound(
                                *EOSUser,
                                *PendingInviteEOS->GetPartyId(),
                                ERejectPartyInvitationCompletionResult::UnknownInternalFailure);
                            UE_LOG(
                                LogEOS,
                                Error,
                                TEXT("Rejecting invite failed (got result code %s)"),
                                ANSI_TO_TCHAR(EOS_EResult_ToString(Info->ResultCode)));
                            return;
                        }

                        This->PendingInvites[*EOSUser].Remove(PendingInvite);
#if defined(UE_5_1_OR_LATER)
                        This->TriggerOnPartyInviteRemovedExDelegates(
                            *EOSUser,
                            *PendingInvite,
                            EPartyInvitationRemovedReason::Declined);
#else
                        This->TriggerOnPartyInviteRemovedDelegates(
                            *EOSUser,
                            *PendingInvite->GetPartyId(),
                            *PendingInvite->GetSourceUserId(),
                            EPartyInvitationRemovedReason::Declined);
#endif
                        This->TriggerOnPartyInvitesChangedDelegates(*EOSUser);

                        Delegate.ExecuteIfBound(
                            *EOSUser,
                            *PendingInviteEOS->GetPartyId(),
                            ERejectPartyInvitationCompletionResult::Succeeded);
                    }
                });
        }
    }
    return true;
}

void FOnlinePartySystemEOS::ClearInvitations(
    const FUniqueNetId &LocalUserId,
    const FUniqueNetId &SenderId,
    const FOnlinePartyId *PartyId)
{
    UE_LOG(LogEOS, Error, TEXT("ClearInvitations not supported!"));
}

bool FOnlinePartySystemEOS::KickMember(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FUniqueNetId &TargetMemberId,
    const FOnKickPartyMemberComplete &Delegate)
{
    if (LocalUserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        Delegate
            .ExecuteIfBound(LocalUserId, PartyId, TargetMemberId, EKickMemberCompletionResult::UnknownClientFailure);
        return true;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(LocalUserId.AsShared());
    if (!EOSUser->HasValidProductUserId())
    {
        Delegate
            .ExecuteIfBound(LocalUserId, PartyId, TargetMemberId, EKickMemberCompletionResult::UnknownClientFailure);
        return true;
    }

    auto EOSTarget = StaticCastSharedRef<const FUniqueNetIdEOS>(TargetMemberId.AsShared());
    if (!EOSTarget->HasValidProductUserId())
    {
        Delegate
            .ExecuteIfBound(LocalUserId, PartyId, TargetMemberId, EKickMemberCompletionResult::UnknownClientFailure);
        return true;
    }

    auto LocalUserIdRef = LocalUserId.AsShared();
    auto TargetUserIdRef = TargetMemberId.AsShared();
    auto EOSPartyId = StaticCastSharedRef<const FOnlinePartyIdEOS>(PartyId.AsShared());

    EOS_Lobby_KickMemberOptions Opts = {};
    Opts.ApiVersion = EOS_LOBBY_KICKMEMBER_API_LATEST;
    Opts.LocalUserId = EOSUser->GetProductUserId();
    Opts.LobbyId = EOSPartyId->GetLobbyId();
    Opts.TargetUserId = EOSTarget->GetProductUserId();

    EOSRunOperation<EOS_HLobby, EOS_Lobby_KickMemberOptions, EOS_Lobby_KickMemberCallbackInfo>(
        this->EOSLobby,
        &Opts,
        *EOS_Lobby_KickMember,
        [Delegate, LocalUserIdRef, EOSPartyId, TargetUserIdRef](const EOS_Lobby_KickMemberCallbackInfo *Info) {
            if (Info->ResultCode != EOS_EResult::EOS_Success)
            {
                Delegate.ExecuteIfBound(
                    LocalUserIdRef.Get(),
                    EOSPartyId.Get(),
                    TargetUserIdRef.Get(),
                    EKickMemberCompletionResult::UnknownClientFailure);
                return;
            }

            Delegate.ExecuteIfBound(
                LocalUserIdRef.Get(),
                EOSPartyId.Get(),
                TargetUserIdRef.Get(),
                EKickMemberCompletionResult::Succeeded);
        });
    return true;
}

bool FOnlinePartySystemEOS::PromoteMember(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FUniqueNetId &TargetMemberId,
    const FOnPromotePartyMemberComplete &Delegate)
{
    if (LocalUserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        Delegate
            .ExecuteIfBound(LocalUserId, PartyId, TargetMemberId, EPromoteMemberCompletionResult::UnknownClientFailure);
        return true;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(LocalUserId.AsShared());
    if (!EOSUser->HasValidProductUserId())
    {
        Delegate
            .ExecuteIfBound(LocalUserId, PartyId, TargetMemberId, EPromoteMemberCompletionResult::UnknownClientFailure);
        return true;
    }

    auto EOSTarget = StaticCastSharedRef<const FUniqueNetIdEOS>(TargetMemberId.AsShared());
    if (!EOSTarget->HasValidProductUserId())
    {
        Delegate
            .ExecuteIfBound(LocalUserId, PartyId, TargetMemberId, EPromoteMemberCompletionResult::UnknownClientFailure);
        return true;
    }

    auto LocalUserIdRef = LocalUserId.AsShared();
    auto TargetUserIdRef = TargetMemberId.AsShared();
    auto EOSPartyId = StaticCastSharedRef<const FOnlinePartyIdEOS>(PartyId.AsShared());

    EOS_Lobby_PromoteMemberOptions Opts = {};
    Opts.ApiVersion = EOS_LOBBY_PROMOTEMEMBER_API_LATEST;
    Opts.LocalUserId = EOSUser->GetProductUserId();
    Opts.LobbyId = EOSPartyId->GetLobbyId();
    Opts.TargetUserId = EOSTarget->GetProductUserId();

    EOSRunOperation<EOS_HLobby, EOS_Lobby_PromoteMemberOptions, EOS_Lobby_PromoteMemberCallbackInfo>(
        this->EOSLobby,
        &Opts,
        *EOS_Lobby_PromoteMember,
        [Delegate, LocalUserIdRef, EOSPartyId, TargetUserIdRef](const EOS_Lobby_PromoteMemberCallbackInfo *Info) {
            if (Info->ResultCode != EOS_EResult::EOS_Success)
            {
                Delegate.ExecuteIfBound(
                    LocalUserIdRef.Get(),
                    EOSPartyId.Get(),
                    TargetUserIdRef.Get(),
                    EPromoteMemberCompletionResult::UnknownClientFailure);
                return;
            }

            Delegate.ExecuteIfBound(
                LocalUserIdRef.Get(),
                EOSPartyId.Get(),
                TargetUserIdRef.Get(),
                EPromoteMemberCompletionResult::Succeeded);
        });
    return false;
}

bool FOnlinePartySystemEOS::UpdatePartyData(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FName &Namespace,
    const FOnlinePartyData &PartyData)
{
    if (!Namespace.IsEqual(DefaultPartyDataNamespace) && !Namespace.IsEqual(NAME_None))
    {
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("%s"),
            *OnlineRedpointEOS::Errors::InvalidRequest(
                 LocalUserId,
                 *PartyId.ToString(),
                 ANSI_TO_TCHAR(__FUNCTION__),
                 TEXT("EOS does not support namespaced party data. The namespace parameter will be ignored. You should use "
                     "pass DefaultPartyDataNamespace / NAME_Default as the namespace (or 'Default' from blueprints) to "
                     "hide this warning."))
                 .ToLogString());
    }

    if (LocalUserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("%s"),
            *OnlineRedpointEOS::Errors::InvalidUser(
                 LocalUserId,
                 *PartyId.ToString(),
                 ANSI_TO_TCHAR(__FUNCTION__),
                 TEXT("Local user ID is not an EOS user"))
                 .ToLogString());
        return false;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(LocalUserId.AsShared());
    if (!EOSUser->HasValidProductUserId())
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("%s"),
            *OnlineRedpointEOS::Errors::InvalidUser(
                 LocalUserId,
                 *PartyId.ToString(),
                 ANSI_TO_TCHAR(__FUNCTION__),
                 TEXT("Local EOS user does not have a valid product user ID"))
                 .ToLogString());
        return false;
    }

    if (!PartyData.DoesSharedInstanceExist())
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("%s"),
            *OnlineRedpointEOS::Errors::InvalidRequest(
                 LocalUserId,
                 *PartyId.ToString(),
                 ANSI_TO_TCHAR(__FUNCTION__),
                 TEXT("The party data must be a shared object; create it with MakeShared<> instead."))
                 .ToLogString());
        return false;
    }

    EOS_HLobbyModification Mod = {};

    EOS_Lobby_UpdateLobbyModificationOptions ModOpts = {};
    ModOpts.ApiVersion = EOS_LOBBY_UPDATELOBBYMODIFICATION_API_LATEST;
    ModOpts.LobbyId = ((const FOnlinePartyIdEOS &)PartyId).GetLobbyId();
    ModOpts.LocalUserId = EOSUser->GetProductUserId();

    if (EOS_Lobby_UpdateLobbyModification(this->EOSLobby, &ModOpts, &Mod) != EOS_EResult::EOS_Success)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("%s"),
            *OnlineRedpointEOS::Errors::UnexpectedError(
                 LocalUserId,
                 *PartyId.ToString(),
                 ANSI_TO_TCHAR(__FUNCTION__),
                 TEXT("Could not get UpdateLobbyModification"))
                 .ToLogString());
        return false;
    }

    TSharedRef<const FOnlinePartyData> PartyDataRef = PartyData.AsShared();

    FOnlineKeyValuePairs<FString, FVariantData> DirtyAttrs;
    TArray<FString> RemovedAttrs;
    PartyDataRef->GetDirtyKeyValAttrs(DirtyAttrs, RemovedAttrs);
    for (const auto &KV : DirtyAttrs)
    {
        if (KV.Key.ToUpper() == FString(PARTY_TYPE_ID_TCHAR).ToUpper())
        {
            // This is a reserved key - you can not set it through UpdatePartyData.
            continue;
        }

        auto KeyStr = EOSString_LobbyModification_AttributeKey::ToAnsiString(KV.Key);
        auto ValueStr = EOSString_LobbyModification_AttributeStringValue::ToUtf8String(KV.Value.ToString());

        EOS_Lobby_AttributeData Attribute = {};
        Attribute.ApiVersion = EOS_LOBBY_ATTRIBUTEDATA_API_LATEST;
        Attribute.Key = KeyStr.Ptr.Get();

        switch (KV.Value.GetType())
        {
        case EOnlineKeyValuePairDataType::Bool:
            Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_BOOLEAN;
            bool BoolVal;
            KV.Value.GetValue(BoolVal);
            Attribute.Value.AsBool = BoolVal ? EOS_TRUE : EOS_FALSE;
            break;
        case EOnlineKeyValuePairDataType::Int64:
            Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_INT64;
            int64 Int64Val;
            KV.Value.GetValue(Int64Val);
            Attribute.Value.AsInt64 = Int64Val;
            break;
        case EOnlineKeyValuePairDataType::Double:
            Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_DOUBLE;
            double DoubleVal;
            KV.Value.GetValue(DoubleVal);
            Attribute.Value.AsDouble = DoubleVal;
            break;
        case EOnlineKeyValuePairDataType::String:
            Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_STRING;
            Attribute.Value.AsUtf8 = ValueStr.GetAsChar();
            break;
        default:
            UE_LOG(
                LogEOS,
                Error,
                TEXT("%s"),
                *OnlineRedpointEOS::Errors::InvalidRequest(
                     LocalUserId,
                     *PartyId.ToString(),
                     ANSI_TO_TCHAR(__FUNCTION__),
                     TEXT("Invalid attribute data (unsupported type)"))
                     .ToLogString());
            return false;
        }

        EOS_LobbyModification_AddAttributeOptions AddOpts = {};
        AddOpts.ApiVersion = EOS_LOBBYMODIFICATION_ADDATTRIBUTE_API_LATEST;
        AddOpts.Attribute = &Attribute;
        AddOpts.Visibility = EOS_ELobbyAttributeVisibility::EOS_LAT_PUBLIC;
        if (EOS_LobbyModification_AddAttribute(Mod, &AddOpts) != EOS_EResult::EOS_Success)
        {
            EOS_LobbyModification_Release(Mod);
            UE_LOG(
                LogEOS,
                Error,
                TEXT("%s"),
                *OnlineRedpointEOS::Errors::UnexpectedError(
                     LocalUserId,
                     *PartyId.ToString(),
                     ANSI_TO_TCHAR(__FUNCTION__),
                     TEXT("Call to EOS_LobbyModification_AddAttribute failed"))
                     .ToLogString());
            return false;
        }
    }
    for (const auto &Key : RemovedAttrs)
    {
        if (Key.ToUpper() == FString(PARTY_TYPE_ID_TCHAR).ToUpper())
        {
            // This is a reserved key - you can not remove it through UpdatePartyData.
            continue;
        }

        auto KeyStr = EOSString_LobbyModification_AttributeKey::ToAnsiString(Key);

        EOS_LobbyModification_RemoveAttributeOptions RemoveOpts = {};
        RemoveOpts.ApiVersion = EOS_LOBBYMODIFICATION_REMOVEATTRIBUTE_API_LATEST;
        RemoveOpts.Key = KeyStr.Ptr.Get();
        if (EOS_LobbyModification_RemoveAttribute(Mod, &RemoveOpts) != EOS_EResult::EOS_Success)
        {
            EOS_LobbyModification_Release(Mod);
            UE_LOG(
                LogEOS,
                Error,
                TEXT("%s"),
                *OnlineRedpointEOS::Errors::UnexpectedError(
                     LocalUserId,
                     *PartyId.ToString(),
                     ANSI_TO_TCHAR(__FUNCTION__),
                     TEXT("Call to EOS_LobbyModification_RemoveAttribute failed"))
                     .ToLogString());
            return false;
        }
    }

    EOS_Lobby_UpdateLobbyOptions Opts = {};
    Opts.ApiVersion = EOS_LOBBY_UPDATELOBBY_API_LATEST;
    Opts.LobbyModificationHandle = Mod;

    auto LocalUserIdRef = LocalUserId.AsShared();
    auto PartyIdRef = PartyId.AsShared();

    EOSRunOperation<EOS_HLobby, EOS_Lobby_UpdateLobbyOptions, EOS_Lobby_UpdateLobbyCallbackInfo>(
        this->EOSLobby,
        &Opts,
        *EOS_Lobby_UpdateLobby,
        [WeakThis = GetWeakThis(this), Mod, LocalUserIdRef, PartyIdRef, PartyDataRef](
            const EOS_Lobby_UpdateLobbyCallbackInfo *Info) {
            EOS_LobbyModification_Release(Mod);

            if (auto This = PinWeakThis(WeakThis))
            {
                if (Info->ResultCode != EOS_EResult::EOS_Success &&
                    Info->ResultCode != EOS_EResult::EOS_NoChange)
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("%s"),
                        *ConvertError(
                             *LocalUserIdRef,
                             *PartyIdRef->ToString(),
                             ANSI_TO_TCHAR(__FUNCTION__),
                             Info->ResultCode)
                             .ToLogString());
                    return;
                }
                else if (Info->ResultCode == EOS_EResult::EOS_NoChange)
                {
                    UE_LOG(
                        LogEOS,
                        Warning,
                        TEXT("%s"),
                        *ConvertError(
                             *LocalUserIdRef,
                             *PartyIdRef->ToString(),
                             ANSI_TO_TCHAR(__FUNCTION__),
                             TEXT("The UpdatePartyData operation did not change any data on the party, "
                                  "which means the OnPartyDataReceived event will not be fired."),
                             Info->ResultCode)
                             .ToLogString());
                }

                // Otherwise success (the party being updated will cause the OnPartyDataReceived event to fire).
                ConstCastSharedRef<FOnlinePartyData>(PartyDataRef)->ClearDirty();
            }
        });
    return true;
}

bool FOnlinePartySystemEOS::UpdatePartyMemberData(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FName &Namespace,
    const FOnlinePartyData &PartyMemberData)
{
    if (!Namespace.IsEqual(DefaultPartyDataNamespace) && !Namespace.IsEqual(NAME_None))
    {
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("%s"),
            *OnlineRedpointEOS::Errors::InvalidRequest(
                 LocalUserId,
                 *PartyId.ToString(),
                 ANSI_TO_TCHAR(__FUNCTION__),
                 TEXT("EOS does not support namespaced party member data. The namespace parameter will be ignored. You "
                      "should use pass DefaultPartyDataNamespace / NAME_Default as the namespace (or 'Default' from "
                      "blueprints) to hide this warning."))
                 .ToLogString());
    }

    if (LocalUserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("%s"),
            *OnlineRedpointEOS::Errors::InvalidUser(
                 LocalUserId,
                 *PartyId.ToString(),
                 ANSI_TO_TCHAR(__FUNCTION__),
                 TEXT("Local user ID is not an EOS user"))
                 .ToLogString());
        return false;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(LocalUserId.AsShared());
    if (!EOSUser->HasValidProductUserId())
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("%s"),
            *OnlineRedpointEOS::Errors::InvalidUser(
                 LocalUserId,
                 *PartyId.ToString(),
                 ANSI_TO_TCHAR(__FUNCTION__),
                 TEXT("Local EOS user does not have a valid product user ID"))
                 .ToLogString());
        return false;
    }

    EOS_HLobbyModification Mod = {};

    EOS_Lobby_UpdateLobbyModificationOptions ModOpts = {};
    ModOpts.ApiVersion = EOS_LOBBY_UPDATELOBBYMODIFICATION_API_LATEST;
    ModOpts.LobbyId = ((const FOnlinePartyIdEOS &)PartyId).GetLobbyId();
    ModOpts.LocalUserId = EOSUser->GetProductUserId();

    if (EOS_Lobby_UpdateLobbyModification(this->EOSLobby, &ModOpts, &Mod) != EOS_EResult::EOS_Success)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("%s"),
            *OnlineRedpointEOS::Errors::UnexpectedError(
                 LocalUserId,
                 *PartyId.ToString(),
                 ANSI_TO_TCHAR(__FUNCTION__),
                 TEXT("Could not get UpdateLobbyModification"))
                 .ToLogString());
        return false;
    }

    TSharedRef<const FOnlinePartyData> PartyDataRef = PartyMemberData.AsShared();

    FOnlineKeyValuePairs<FString, FVariantData> DirtyAttrs;
    TArray<FString> RemovedAttrs;
    PartyDataRef->GetDirtyKeyValAttrs(DirtyAttrs, RemovedAttrs);
    for (const auto &KV : DirtyAttrs)
    {
        if (KV.Key.ToUpper() == FString(PARTY_TYPE_ID_TCHAR).ToUpper())
        {
            // This is a reserved key - you can not set it through UpdatePartyData.
            continue;
        }

        auto KeyStr = EOSString_LobbyModification_AttributeKey::ToAnsiString(KV.Key);
        auto ValueStr = EOSString_LobbyModification_AttributeStringValue::ToUtf8String(KV.Value.ToString());

        EOS_Lobby_AttributeData Attribute = {};
        Attribute.ApiVersion = EOS_LOBBY_ATTRIBUTEDATA_API_LATEST;
        Attribute.Key = KeyStr.Ptr.Get();

        switch (KV.Value.GetType())
        {
        case EOnlineKeyValuePairDataType::Bool:
            Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_BOOLEAN;
            bool BoolVal;
            KV.Value.GetValue(BoolVal);
            Attribute.Value.AsBool = BoolVal ? EOS_TRUE : EOS_FALSE;
            break;
        case EOnlineKeyValuePairDataType::Int64:
            Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_INT64;
            int64 Int64Val;
            KV.Value.GetValue(Int64Val);
            Attribute.Value.AsInt64 = Int64Val;
            break;
        case EOnlineKeyValuePairDataType::Double:
            Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_DOUBLE;
            double DoubleVal;
            KV.Value.GetValue(DoubleVal);
            Attribute.Value.AsDouble = DoubleVal;
            break;
        case EOnlineKeyValuePairDataType::String:
            Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_STRING;
            Attribute.Value.AsUtf8 = ValueStr.GetAsChar();
            break;
        default:
            UE_LOG(
                LogEOS,
                Error,
                TEXT("%s"),
                *OnlineRedpointEOS::Errors::InvalidRequest(
                     LocalUserId,
                     *PartyId.ToString(),
                     ANSI_TO_TCHAR(__FUNCTION__),
                     TEXT("Invalid attribute data (unsupported type)"))
                     .ToLogString());
            return false;
        }

        EOS_LobbyModification_AddMemberAttributeOptions AddOpts = {};
        AddOpts.ApiVersion = EOS_LOBBYMODIFICATION_ADDMEMBERATTRIBUTE_API_LATEST;
        AddOpts.Attribute = &Attribute;
        AddOpts.Visibility = EOS_ELobbyAttributeVisibility::EOS_LAT_PUBLIC;
        if (EOS_LobbyModification_AddMemberAttribute(Mod, &AddOpts) != EOS_EResult::EOS_Success)
        {
            EOS_LobbyModification_Release(Mod);
            UE_LOG(
                LogEOS,
                Error,
                TEXT("%s"),
                *OnlineRedpointEOS::Errors::UnexpectedError(
                     LocalUserId,
                     *PartyId.ToString(),
                     ANSI_TO_TCHAR(__FUNCTION__),
                     TEXT("Call to EOS_LobbyModification_AddMemberAttribute failed"))
                     .ToLogString());
            return false;
        }
    }
    for (const auto &Key : RemovedAttrs)
    {
        if (Key.ToUpper() == FString(PARTY_TYPE_ID_TCHAR).ToUpper())
        {
            // This is a reserved key - you can not remove it through UpdatePartyData.
            continue;
        }

        auto KeyStr = EOSString_LobbyModification_AttributeKey::ToAnsiString(Key);

        EOS_LobbyModification_RemoveMemberAttributeOptions RemoveOpts = {};
        RemoveOpts.ApiVersion = EOS_LOBBYMODIFICATION_REMOVEMEMBERATTRIBUTE_API_LATEST;
        RemoveOpts.Key = KeyStr.Ptr.Get();
        if (EOS_LobbyModification_RemoveMemberAttribute(Mod, &RemoveOpts) != EOS_EResult::EOS_Success)
        {
            EOS_LobbyModification_Release(Mod);
            UE_LOG(
                LogEOS,
                Error,
                TEXT("%s"),
                *OnlineRedpointEOS::Errors::UnexpectedError(
                     LocalUserId,
                     *PartyId.ToString(),
                     ANSI_TO_TCHAR(__FUNCTION__),
                     TEXT("Call to EOS_LobbyModification_RemoveMemberAttribute failed"))
                     .ToLogString());
            return false;
        }
    }

    EOS_Lobby_UpdateLobbyOptions Opts = {};
    Opts.ApiVersion = EOS_LOBBY_UPDATELOBBY_API_LATEST;
    Opts.LobbyModificationHandle = Mod;

    auto LocalUserIdRef = LocalUserId.AsShared();
    auto PartyIdRef = PartyId.AsShared();

    EOSRunOperation<EOS_HLobby, EOS_Lobby_UpdateLobbyOptions, EOS_Lobby_UpdateLobbyCallbackInfo>(
        this->EOSLobby,
        &Opts,
        *EOS_Lobby_UpdateLobby,
        [WeakThis = GetWeakThis(this), Mod, LocalUserIdRef, PartyIdRef, PartyDataRef](
            const EOS_Lobby_UpdateLobbyCallbackInfo *Info) {
            EOS_LobbyModification_Release(Mod);

            if (auto This = PinWeakThis(WeakThis))
            {
                if (Info->ResultCode != EOS_EResult::EOS_Success &&
                    Info->ResultCode != EOS_EResult::EOS_NoChange)
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("%s"),
                        *ConvertError(
                             *LocalUserIdRef,
                             *PartyIdRef->ToString(),
                             ANSI_TO_TCHAR(__FUNCTION__),
                             Info->ResultCode)
                             .ToLogString());
                    return;
                }
                else if (Info->ResultCode == EOS_EResult::EOS_NoChange)
                {
                    UE_LOG(
                        LogEOS,
                        Warning,
                        TEXT("%s"),
                        *ConvertError(
                             *LocalUserIdRef,
                             *PartyIdRef->ToString(),
                             ANSI_TO_TCHAR(__FUNCTION__),
                             TEXT("The UpdatePartyMemberData operation did not change any data on the party member, which means the OnPartyMemberDataReceived event will not be fired."),
                             Info->ResultCode)
                             .ToLogString());
                }

                // Otherwise success (the member being updated will cause the OnPartyMemberDataReceived event to
                // fire).
                ConstCastSharedRef<FOnlinePartyData>(PartyDataRef)->ClearDirty();
            }
        });
    return true;
}

bool FOnlinePartySystemEOS::IsMemberLeader(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FUniqueNetId &MemberId) const
{
    if (!this->JoinedParties.Contains(LocalUserId))
    {
        return false;
    }

    for (const auto &Party : this->JoinedParties[LocalUserId])
    {
        if (Party->PartyId.Get() == PartyId)
        {
            return Party->LeaderId.ToSharedRef().Get() == MemberId;
        }
    }

    UE_LOG(LogEOS, Error, TEXT("IsMemberLeader called for unknown party!"));
    return false;
}

uint32 FOnlinePartySystemEOS::GetPartyMemberCount(const FUniqueNetId &LocalUserId, const FOnlinePartyId &PartyId) const
{
    if (!this->JoinedParties.Contains(LocalUserId))
    {
        return false;
    }

    for (const auto &Party : this->JoinedParties[LocalUserId])
    {
        if (Party->PartyId.Get() == PartyId)
        {
            return Party->Members.Num();
        }
    }

    UE_LOG(LogEOS, Error, TEXT("GetPartyMemberCount called for unknown party!"));
    return 0;
}

FOnlinePartyConstPtr FOnlinePartySystemEOS::GetParty(const FUniqueNetId &LocalUserId, const FOnlinePartyId &PartyId)
    const
{
    if (!this->JoinedParties.Contains(LocalUserId))
    {
        return nullptr;
    }

    for (const auto &Party : this->JoinedParties[LocalUserId])
    {
        if (Party->PartyId.Get() == PartyId)
        {
            for (const auto &Member : Party->Members)
            {
                if (Member->GetUserId().Get() == LocalUserId)
                {
                    return Party;
                }
            }

            return nullptr;
        }
    }

    UE_LOG(LogEOS, Error, TEXT("GetParty called for unknown party!"));
    return nullptr;
}

FOnlinePartyConstPtr FOnlinePartySystemEOS::GetParty(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyTypeId &PartyTypeId) const
{
    if (!this->JoinedParties.Contains(LocalUserId))
    {
        return nullptr;
    }

    for (const auto &Party : this->JoinedParties[LocalUserId])
    {
        if (Party->PartyTypeId.GetValue() == PartyTypeId.GetValue())
        {
            for (const auto &Member : Party->Members)
            {
                if (Member->GetUserId().Get() == LocalUserId)
                {
                    return Party;
                }
            }

            return nullptr;
        }
    }

    return nullptr;
}

FOnlinePartyMemberConstPtr FOnlinePartySystemEOS::GetPartyMember(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FUniqueNetId &MemberId) const
{
    if (!this->JoinedParties.Contains(LocalUserId))
    {
        return nullptr;
    }

    for (const auto &Party : this->JoinedParties[LocalUserId])
    {
        if (Party->PartyId.Get() == PartyId)
        {
            for (const auto &Member : Party->Members)
            {
                if (Member->GetUserId().Get() == MemberId)
                {
                    return Member;
                }
            }

            UE_LOG(LogEOS, Error, TEXT("GetPartyMember called for unknown party member!"));
            return nullptr;
        }
    }

    UE_LOG(LogEOS, Error, TEXT("GetPartyMember called for unknown party!"));
    return nullptr;
}

FOnlinePartyDataConstPtr FOnlinePartySystemEOS::GetPartyData(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId
    ,
    const FName &Namespace
) const
{
    if (!Namespace.IsEqual(DefaultPartyDataNamespace) && !Namespace.IsEqual(NAME_None))
    {
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("EOS does not support namespaced party data. The namespace parameter will be ignored. You should use "
                 "pass DefaultPartyDataNamespace / NAME_Default as the namespace (or 'Default' from blueprints) to "
                 "hide this warning."));
    }

    if (!this->JoinedParties.Contains(LocalUserId))
    {
        return nullptr;
    }

    for (const auto &Party : this->JoinedParties[LocalUserId])
    {
        if (Party->PartyId.Get() == PartyId)
        {
            TSharedPtr<FOnlinePartyData> PartyData = MakeShared<FOnlinePartyData>();
            PartyData->GetKeyValAttrs() = Party->Attributes;
            return PartyData;
        }
    }

    UE_LOG(LogEOS, Error, TEXT("GetPartyData called for unknown party!"));
    return nullptr;
}

FOnlinePartyDataConstPtr FOnlinePartySystemEOS::GetPartyMemberData(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FUniqueNetId &MemberId
    ,
    const FName &Namespace
) const
{
    if (!Namespace.IsEqual(DefaultPartyDataNamespace) && !Namespace.IsEqual(NAME_None))
    {
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("EOS does not support namespaced party member data. The namespace parameter will be ignored. You "
                 "should use "
                 "pass DefaultPartyDataNamespace / NAME_Default as the namespace (or 'Default' from blueprints) to "
                 "hide this warning."));
    }

    if (!this->JoinedParties.Contains(LocalUserId))
    {
        return nullptr;
    }

    for (const auto &Party : this->JoinedParties[LocalUserId])
    {
        if (Party->PartyId.Get() == PartyId)
        {
            for (const auto &Member : Party->Members)
            {
                if (Member->GetUserId().Get() == MemberId)
                {
                    TSharedPtr<FOnlinePartyData> PartyMemberData = MakeShared<FOnlinePartyData>();
                    PartyMemberData->GetKeyValAttrs() = Member->PartyMemberAttributes;
                    return PartyMemberData;
                }
            }

            UE_LOG(LogEOS, Error, TEXT("GetPartyMemberData called for unknown party member!"));
            return nullptr;
        }
    }

    UE_LOG(LogEOS, Error, TEXT("GetPartyMemberData called for unknown party!"));
    return nullptr;
}

IOnlinePartyJoinInfoConstPtr FOnlinePartySystemEOS::GetAdvertisedParty(
    const FUniqueNetId &LocalUserId,
    const FUniqueNetId &UserId,
    const FOnlinePartyTypeId PartyTypeId) const
{
    UE_LOG(LogEOS, Error, TEXT("GetAdvertisedParty not supported!"));
    return nullptr;
}

bool FOnlinePartySystemEOS::GetJoinedParties(
    const FUniqueNetId &LocalUserId,
    TArray<TSharedRef<const FOnlinePartyId>> &OutPartyIdArray) const
{
    if (!this->JoinedParties.Contains(LocalUserId))
    {
        UE_LOG(LogEOS, Verbose, TEXT("GetJoinedParties: There are 0 parties present"));
        OutPartyIdArray.Empty();
        return true;
    }

    UE_LOG(
        LogEOS,
        Verbose,
        TEXT("GetJoinedParties: There are %d parties present"),
        this->JoinedParties[LocalUserId].Num());
    for (const auto &Party : this->JoinedParties[LocalUserId])
    {
        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("GetJoinedParties: Party %s has %d members"),
            *Party->PartyId->ToString(),
            Party->Members.Num());

        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("GetJoinedParties: Added party to result list"),
            *Party->PartyId->ToString(),
            Party->Members.Num());
        OutPartyIdArray.Add(Party->PartyId);
    }

    return true;
}

bool FOnlinePartySystemEOS::GetPartyMembers(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    TArray<FOnlinePartyMemberConstRef> &OutPartyMembersArray) const
{
    if (!this->JoinedParties.Contains(LocalUserId))
    {
        OutPartyMembersArray.Empty();
        return false;
    }

    for (const auto &Party : this->JoinedParties[LocalUserId])
    {
        if (Party->PartyId.Get() == PartyId)
        {
            for (const auto &Member : Party->Members)
            {
                OutPartyMembersArray.Add(Member.ToSharedRef());
            }

            return true;
        }
    }

    UE_LOG(LogEOS, Error, TEXT("GetPartyMembers called for unknown party!"));
    return false;
}

bool FOnlinePartySystemEOS::GetPendingInvites(
    const FUniqueNetId &LocalUserId,
    TArray<IOnlinePartyJoinInfoConstRef> &OutPendingInvitesArray) const
{
    for (const auto &KV : this->PendingInvites)
    {
        if (*KV.Key == LocalUserId)
        {
            OutPendingInvitesArray = KV.Value;
            return true;
        }
    }

    // User has no pending invites.
    OutPendingInvitesArray = TArray<IOnlinePartyJoinInfoConstRef>();
    return true;
}

bool FOnlinePartySystemEOS::GetPendingJoinRequests(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    TArray<IOnlinePartyPendingJoinRequestInfoConstRef> &OutPendingJoinRequestArray) const
{
    UE_LOG(LogEOS, Error, TEXT("GetPendingJoinRequests is not supported!"));
    return false;
}

bool FOnlinePartySystemEOS::GetPendingInvitedUsers(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    TArray<TSharedRef<const FUniqueNetId>> &OutPendingInvitedUserArray) const
{
    UE_LOG(LogEOS, Error, TEXT("GetPendingInvitedUsers is not supported!"));
    return false;
}

#if defined(EOS_PARTY_SYSTEM_REQUEST_TO_JOIN_PARTY)

bool FOnlinePartySystemEOS::GetPendingRequestsToJoin(
    const FUniqueNetId &LocalUserId,
    TArray<IOnlinePartyRequestToJoinInfoConstRef> &OutRequestsToJoin) const
{
    UE_LOG(
        LogEOS,
        Error,
        TEXT("%s"),
        *OnlineRedpointEOS::Errors::NotImplemented(
             LocalUserId,
             TEXT("FOnlinePartySystemEOS::GetPendingRequestsToJoin"),
             TEXT("GetPendingRequestsToJoin is not supported."))
             .ToLogString());
    return false;
}

#endif // #if defined(EOS_PARTY_SYSTEM_REQUEST_TO_JOIN_PARTY)

FString FOnlinePartySystemEOS::MakeJoinInfoJson(const FUniqueNetId &LocalUserId, const FOnlinePartyId &PartyId)
{
    if (LocalUserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        return TEXT("");
    }

    auto LocalUserIdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(LocalUserId.AsShared());
    auto PartyIdEOS = StaticCastSharedRef<const FOnlinePartyIdEOS>(PartyId.AsShared());

    TSharedRef<FJsonObject> Json = MakeShared<FJsonObject>();
    Json->SetStringField(TEXT("LocalUserId"), LocalUserIdEOS->GetProductUserIdString());
    Json->SetStringField(TEXT("PartyId"), EOSString_LobbyId::ToString(PartyIdEOS->GetLobbyId()));

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(Json, Writer);
    return OutputString;
}

IOnlinePartyJoinInfoConstPtr FOnlinePartySystemEOS::MakeJoinInfoFromJson(const FString &JoinInfoJson)
{
    TSharedPtr<FJsonObject> Json;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JoinInfoJson);
    FJsonSerializer::Deserialize(*Reader, Json);

    if (!Json->HasField("LocalUserId") || !Json->HasField("PartyId"))
    {
        return nullptr;
    }

    FString LocalUserIdStr = Json->GetStringField("LocalUserId");
    FString PartyIdStr = Json->GetStringField("PartyId");

    // Uhhh, we should handle this cast better...
    auto LobbyIdAnsi = StringCast<ANSICHAR>(*PartyIdStr);
    return MakeShared<FOnlinePartyJoinInfoEOSUnresolved>(LobbyIdAnsi.Get());
}

FString FOnlinePartySystemEOS::MakeTokenFromJoinInfo(const IOnlinePartyJoinInfo &JoinInfo) const
{
    return TEXT("");
}

IOnlinePartyJoinInfoConstPtr FOnlinePartySystemEOS::MakeJoinInfoFromToken(const FString &Token) const
{
    return nullptr;
}

IOnlinePartyJoinInfoConstPtr FOnlinePartySystemEOS::ConsumePendingCommandLineInvite()
{
    return nullptr;
}

void FOnlinePartySystemEOS::DumpPartyState()
{
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
