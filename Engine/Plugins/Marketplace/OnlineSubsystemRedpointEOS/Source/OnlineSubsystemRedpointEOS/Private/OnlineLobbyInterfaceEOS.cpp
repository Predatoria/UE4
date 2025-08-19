// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Private/OnlineLobbyInterfaceEOS.h"

#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSErrorConv.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlinePartyIdEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/VoiceChat/VoiceManager.h"
#include "OnlineSubsystemRedpointEOS/Shared/VoiceChat/VoiceManagerLocalUser.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

class FOnlineLobbyEOS : public FOnlineLobby
{
public:
    FOnlineLobbyEOS() = default;
    UE_NONCOPYABLE(FOnlineLobbyEOS);
};

class FOnlineLobbyTransactionEOS : public FOnlineLobbyTransaction
{
public:
    FOnlineLobbyTransactionEOS() = default;
    UE_NONCOPYABLE(FOnlineLobbyTransactionEOS);
};

class FOnlineLobbyMemberTransactionEOS : public FOnlineLobbyMemberTransaction
{
public:
    FOnlineLobbyMemberTransactionEOS() = default;
    UE_NONCOPYABLE(FOnlineLobbyMemberTransactionEOS);
};

void FOnlineLobbyInterfaceEOS::Internal_AddMemberToLobby(const FUniqueNetId &UserId, const FOnlineLobbyId &LobbyId)
{
    auto LobbyIdStr = LobbyId.ToString();
    if (!this->ConnectedMembers.Contains(LobbyIdStr))
    {
        this->ConnectedMembers.Add(LobbyIdStr, TArray<TSharedPtr<const FUniqueNetId>>());
    }
    for (const auto &ExistingMember : this->ConnectedMembers[LobbyIdStr])
    {
        if (*ExistingMember == UserId)
        {
            return;
        }
    }
    this->ConnectedMembers[LobbyIdStr].Add(UserId.AsShared());
}

void FOnlineLobbyInterfaceEOS::Internal_RemoveMemberFromLobby(const FUniqueNetId &UserId, const FOnlineLobbyId &LobbyId)
{
    auto LobbyIdStr = LobbyId.ToString();
    if (!this->ConnectedMembers.Contains(LobbyIdStr))
    {
        return;
    }
    for (auto i = this->ConnectedMembers[LobbyIdStr].Num() - 1; i >= 0; i--)
    {
        if (*this->ConnectedMembers[LobbyIdStr][i] == UserId)
        {
            this->ConnectedMembers[LobbyIdStr].RemoveAt(i);
        }
    }
    if (this->ConnectedMembers[LobbyIdStr].Num() == 0)
    {
        this->ConnectedMembers.Remove(LobbyIdStr);
    }
}

void FOnlineLobbyInterfaceEOS::Handle_LobbyMemberStatusReceived(
    const EOS_Lobby_LobbyMemberStatusReceivedCallbackInfo *Data)
{
    FString Status = TEXT("Unknown");
    switch (Data->CurrentStatus)
    {
    case EOS_ELobbyMemberStatus::EOS_LMS_JOINED:
        Status = TEXT("Joined");
        break;
    case EOS_ELobbyMemberStatus::EOS_LMS_CLOSED:
        Status = TEXT("Closed");
        break;
    case EOS_ELobbyMemberStatus::EOS_LMS_LEFT:
        Status = TEXT("Left");
        break;
    case EOS_ELobbyMemberStatus::EOS_LMS_DISCONNECTED:
        Status = TEXT("Disconnected");
        break;
    case EOS_ELobbyMemberStatus::EOS_LMS_KICKED:
        Status = TEXT("Kicked");
        break;
    case EOS_ELobbyMemberStatus::EOS_LMS_PROMOTED:
        Status = TEXT("Promoted");
        break;
    }

    auto LobbyIdEOS = MakeShared<FOnlinePartyIdEOS>(Data->LobbyId);
    auto TargetUserIdEOS = MakeShared<FUniqueNetIdEOS>(Data->TargetUserId);

    UE_LOG(
        LogEOS,
        Verbose,
        TEXT("LobbyInterface: LobbyMemberStatusReceived: (Lobby) %s (Member) %s (Status) %s"),
        *LobbyIdEOS->ToString(),
        *TargetUserIdEOS->ToString(),
        *Status);

#if defined(EOS_ENABLE_STATE_DIAGNOSTICS) && EOS_ENABLE_STATE_DIAGNOSTICS
    this->DumpLocalLobbyAttributeState(Data->TargetUserId, Data->LobbyId);
#endif

    auto LobbyIdStr = EOSString_LobbyId::ToString(Data->LobbyId);
    if (this->ConnectedMembers.Contains(LobbyIdStr))
    {
        for (const auto &Member : this->ConnectedMembers[LobbyIdStr])
        {
            switch (Data->CurrentStatus)
            {
            case EOS_ELobbyMemberStatus::EOS_LMS_JOINED:
                if (*Member != *TargetUserIdEOS)
                {
                    this->TriggerOnMemberConnectDelegates(*Member, *LobbyIdEOS, *TargetUserIdEOS);
                }
                break;
            case EOS_ELobbyMemberStatus::EOS_LMS_CLOSED:
                if (*Member == *TargetUserIdEOS)
                {
                    this->TriggerOnLobbyDeleteDelegates(*Member, *LobbyIdEOS);
                }
                break;
            case EOS_ELobbyMemberStatus::EOS_LMS_LEFT:
            case EOS_ELobbyMemberStatus::EOS_LMS_DISCONNECTED:
            case EOS_ELobbyMemberStatus::EOS_LMS_KICKED:
                this->TriggerOnMemberDisconnectDelegates(
                    *Member,
                    *LobbyIdEOS,
                    *TargetUserIdEOS,
                    Data->CurrentStatus == EOS_ELobbyMemberStatus::EOS_LMS_KICKED);
                break;
            case EOS_ELobbyMemberStatus::EOS_LMS_PROMOTED:
                // IOnlineLobby does not care about promotion.
                break;
            }
        }
    }
    switch (Data->CurrentStatus)
    {
    case EOS_ELobbyMemberStatus::EOS_LMS_CLOSED:
    case EOS_ELobbyMemberStatus::EOS_LMS_LEFT:
    case EOS_ELobbyMemberStatus::EOS_LMS_DISCONNECTED:
    case EOS_ELobbyMemberStatus::EOS_LMS_KICKED:
        this->Internal_RemoveMemberFromLobby(*TargetUserIdEOS, *LobbyIdEOS);
        break;
    }
}

void FOnlineLobbyInterfaceEOS::Handle_LobbyMemberUpdateReceived(
    const EOS_Lobby_LobbyMemberUpdateReceivedCallbackInfo *Data)
{
    auto LobbyIdEOS = MakeShared<FOnlinePartyIdEOS>(Data->LobbyId);
    auto TargetUserIdEOS = MakeShared<FUniqueNetIdEOS>(Data->TargetUserId);

    UE_LOG(
        LogEOS,
        Verbose,
        TEXT("LobbyInterface: LobbyMemberUpdateReceived: (Lobby) %s (Member) %s"),
        *LobbyIdEOS->ToString(),
        *TargetUserIdEOS->ToString());

#if defined(EOS_ENABLE_STATE_DIAGNOSTICS) && EOS_ENABLE_STATE_DIAGNOSTICS
    this->DumpLocalLobbyAttributeState(Data->TargetUserId, Data->LobbyId);
#endif

    auto LobbyIdStr = EOSString_LobbyId::ToString(Data->LobbyId);
    if (this->ConnectedMembers.Contains(LobbyIdStr))
    {
        for (const auto &Member : this->ConnectedMembers[LobbyIdStr])
        {
            this->TriggerOnMemberUpdateDelegates(*Member, *LobbyIdEOS, *TargetUserIdEOS);
        }
    }
}

#if defined(EOS_ENABLE_STATE_DIAGNOSTICS) && EOS_ENABLE_STATE_DIAGNOSTICS

void FOnlineLobbyInterfaceEOS::DumpLocalLobbyAttributeState(EOS_ProductUserId LocalUserId, EOS_LobbyId InLobbyId) const
{
    EOS_HLobbyDetails LobbyDetails = {};

    EOS_Lobby_CopyLobbyDetailsHandleOptions HandleOpts = {};
    HandleOpts.ApiVersion = EOS_LOBBY_COPYLOBBYDETAILSHANDLE_API_LATEST;
    HandleOpts.LobbyId = InLobbyId;
    HandleOpts.LocalUserId = LocalUserId;
    EOS_EResult HandleResult = EOS_Lobby_CopyLobbyDetailsHandle(this->EOSLobby, &HandleOpts, &LobbyDetails);
    if (HandleResult == EOS_EResult::EOS_Success)
    {
        this->DumpLobbyAttributeState(LocalUserId, InLobbyId, LobbyDetails);
        EOS_LobbyDetails_Release(LobbyDetails);
    }
    else
    {
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("LobbyInterface: (Lobby) %s: Couldn't copy lobby details handle for lobby, so can't dump "
                 "attributes to "
                 "log: %s"),
            ANSI_TO_TCHAR(InLobbyId),
            *ConvertError(HandleResult).ToLogString());
    }
}

void FOnlineLobbyInterfaceEOS::DumpLobbyAttributeState(
    EOS_ProductUserId LocalUserId,
    EOS_LobbyId InLobbyId,
    EOS_HLobbyDetails InLobbyDetails) const
{
    UE_LOG(LogEOS, Verbose, TEXT("LobbyInterface: --- Lobby State: %s (Begin) ---"), ANSI_TO_TCHAR(InLobbyId));
    UE_LOG(LogEOS, Verbose, TEXT("LobbyInterface: Info:"));

    EOS_LobbyDetails_Info *Info = {};
    EOS_LobbyDetails_CopyInfoOptions InfoOpts = {};
    InfoOpts.ApiVersion = EOS_LOBBYDETAILS_COPYINFO_API_LATEST;
    EOS_EResult InfoResult = EOS_LobbyDetails_CopyInfo(InLobbyDetails, &InfoOpts, &Info);
    if (InfoResult == EOS_EResult::EOS_Success)
    {
        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("LobbyInterface:   Bucket ID: %s"),
            *EOSString_SessionModification_BucketId::FromAnsiString(Info->BucketId));
        UE_LOG(LogEOS, Verbose, TEXT("LobbyInterface:   Max Members: %u"), Info->MaxMembers);
        UE_LOG(LogEOS, Verbose, TEXT("LobbyInterface:   Available Slots: %u"), Info->AvailableSlots);
        switch (Info->PermissionLevel)
        {
        case EOS_ELobbyPermissionLevel::EOS_LPL_INVITEONLY:
            UE_LOG(LogEOS, Verbose, TEXT("LobbyInterface:   Permission Level: Invite Only"));
            break;
        case EOS_ELobbyPermissionLevel::EOS_LPL_JOINVIAPRESENCE:
            UE_LOG(LogEOS, Verbose, TEXT("LobbyInterface:   Permission Level: Join Via Presence"));
            break;
        case EOS_ELobbyPermissionLevel::EOS_LPL_PUBLICADVERTISED:
            UE_LOG(LogEOS, Verbose, TEXT("LobbyInterface:   Permission Level: Public Advertised"));
            break;
        }
        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("LobbyInterface:   Allow Invites: %s"),
            Info->bAllowInvites ? TEXT("True") : TEXT("False"));
        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("LobbyInterface:   Allow Host Migration: %u"),
            Info->bAllowHostMigration ? TEXT("True") : TEXT("False"));
        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("LobbyInterface:   RTC Room Enabled: %s"),
            Info->bRTCRoomEnabled ? TEXT("True") : TEXT("False"));

        EOS_LobbyDetails_Info_Release(Info);
    }
    else
    {
        UE_LOG(LogEOS, Warning, TEXT("LobbyInterface:   Error: %s"), *ConvertError(InfoResult).ToLogString());
    }

    UE_LOG(LogEOS, Verbose, TEXT("LobbyInterface: Attributes:"));

    {
        EOS_LobbyDetails_GetAttributeCountOptions CountOpts = {};
        CountOpts.ApiVersion = EOS_LOBBYDETAILS_GETATTRIBUTECOUNT_API_LATEST;
        uint32_t Count = EOS_LobbyDetails_GetAttributeCount(InLobbyDetails, &CountOpts);
        for (uint32_t AttrIdx = 0; AttrIdx < Count; AttrIdx++)
        {
            EOS_LobbyDetails_CopyAttributeByIndexOptions CopyOpts = {};
            CopyOpts.ApiVersion = EOS_LOBBYDETAILS_COPYATTRIBUTEBYINDEX_API_LATEST;
            CopyOpts.AttrIndex = AttrIdx;

            EOS_Lobby_Attribute *Attr = {};
            EOS_EResult CopyResult = EOS_LobbyDetails_CopyAttributeByIndex(InLobbyDetails, &CopyOpts, &Attr);
            if (CopyResult == EOS_EResult::EOS_Success)
            {
                FString ValueStr;
                FString TypeStr;
                switch (Attr->Data->ValueType)
                {
                case EOS_ELobbyAttributeType::EOS_AT_BOOLEAN:
                    TypeStr = TEXT("bool");
                    ValueStr = Attr->Data->Value.AsBool ? TEXT("True") : TEXT("False");
                    break;
                case EOS_ELobbyAttributeType::EOS_AT_DOUBLE:
                    TypeStr = TEXT("double");
                    ValueStr = FString::Printf(TEXT("%f"), Attr->Data->Value.AsDouble);
                    break;
                case EOS_ELobbyAttributeType::EOS_AT_INT64:
                    TypeStr = TEXT("int64");
                    ValueStr = FString::Printf(TEXT("%lld"), Attr->Data->Value.AsInt64);
                    break;
                case EOS_ELobbyAttributeType::EOS_AT_STRING:
                    TypeStr = TEXT("string");
                    ValueStr = UTF8_TO_TCHAR(Attr->Data->Value.AsUtf8);
                    break;
                }

                UE_LOG(LogEOS, Verbose, TEXT("LobbyInterface:   - Key: %s"), UTF8_TO_TCHAR(Attr->Data->Key));
                UE_LOG(LogEOS, Verbose, TEXT("LobbyInterface:     Type: %s"), *TypeStr);
                UE_LOG(LogEOS, Verbose, TEXT("LobbyInterface:     Value: %s"), *ValueStr);

                EOS_Lobby_Attribute_Release(Attr);
            }
            else
            {
                UE_LOG(LogEOS, Warning, TEXT("LobbyInterface:   - Error: %s"), *ConvertError(CopyResult).ToLogString());
            }
        }
    }

    UE_LOG(LogEOS, Verbose, TEXT("LobbyInterface: Members:"));

    EOS_LobbyDetails_GetLobbyOwnerOptions OwnerOpts = {};
    OwnerOpts.ApiVersion = EOS_LOBBYDETAILS_GETLOBBYOWNER_API_LATEST;
    EOS_ProductUserId OwnerId = EOS_LobbyDetails_GetLobbyOwner(InLobbyDetails, &OwnerOpts);
    auto OwnerIdEOS =
        EOS_ProductUserId_IsValid(OwnerId) ? MakeShared<FUniqueNetIdEOS>(OwnerId) : FUniqueNetIdEOS::InvalidId();

    EOS_LobbyDetails_GetMemberCountOptions MemberCountOpts = {};
    MemberCountOpts.ApiVersion = EOS_LOBBYDETAILS_GETMEMBERCOUNT_API_LATEST;
    uint32_t MemberCount = EOS_LobbyDetails_GetMemberCount(InLobbyDetails, &MemberCountOpts);
    for (uint32_t MemberIdx = 0; MemberIdx < MemberCount; MemberIdx++)
    {
        FString Designation = TEXT("Member");

        EOS_LobbyDetails_GetMemberByIndexOptions GetOpts = {};
        GetOpts.ApiVersion = EOS_LOBBYDETAILS_GETMEMBERBYINDEX_API_LATEST;
        GetOpts.MemberIndex = MemberIdx;
        EOS_ProductUserId MemberId = EOS_LobbyDetails_GetMemberByIndex(InLobbyDetails, &GetOpts);
        auto MemberIdEOS = MakeShared<FUniqueNetIdEOS>(MemberId);

        if (*OwnerIdEOS == *MemberIdEOS)
        {
            Designation = TEXT("Owner");
        }

        UE_LOG(LogEOS, Verbose, TEXT("LobbyInterface:   - User ID: %s"), *MemberIdEOS->ToString());
        UE_LOG(LogEOS, Verbose, TEXT("LobbyInterface:     Type: %s"), *Designation);
        UE_LOG(LogEOS, Verbose, TEXT("LobbyInterface:     Attributes:"));

        {
            EOS_LobbyDetails_GetMemberAttributeCountOptions CountOpts = {};
            CountOpts.ApiVersion = EOS_LOBBYDETAILS_GETMEMBERATTRIBUTECOUNT_API_LATEST;
            CountOpts.TargetUserId = MemberIdEOS->GetProductUserId();
            uint32_t Count = EOS_LobbyDetails_GetMemberAttributeCount(InLobbyDetails, &CountOpts);
            for (uint32_t AttrIdx = 0; AttrIdx < Count; AttrIdx++)
            {
                EOS_LobbyDetails_CopyMemberAttributeByIndexOptions CopyOpts = {};
                CopyOpts.ApiVersion = EOS_LOBBYDETAILS_COPYMEMBERATTRIBUTEBYINDEX_API_LATEST;
                CopyOpts.AttrIndex = AttrIdx;
                CopyOpts.TargetUserId = MemberIdEOS->GetProductUserId();

                EOS_Lobby_Attribute *Attr = {};
                EOS_EResult CopyResult = EOS_LobbyDetails_CopyMemberAttributeByIndex(InLobbyDetails, &CopyOpts, &Attr);
                if (CopyResult == EOS_EResult::EOS_Success)
                {
                    FString ValueStr;
                    FString TypeStr;
                    switch (Attr->Data->ValueType)
                    {
                    case EOS_ELobbyAttributeType::EOS_AT_BOOLEAN:
                        TypeStr = TEXT("bool");
                        ValueStr = Attr->Data->Value.AsBool ? TEXT("True") : TEXT("False");
                        break;
                    case EOS_ELobbyAttributeType::EOS_AT_DOUBLE:
                        TypeStr = TEXT("double");
                        ValueStr = FString::Printf(TEXT("%f"), Attr->Data->Value.AsDouble);
                        break;
                    case EOS_ELobbyAttributeType::EOS_AT_INT64:
                        TypeStr = TEXT("int64");
                        ValueStr = FString::Printf(TEXT("%lld"), Attr->Data->Value.AsInt64);
                        break;
                    case EOS_ELobbyAttributeType::EOS_AT_STRING:
                        TypeStr = TEXT("string");
                        ValueStr = UTF8_TO_TCHAR(Attr->Data->Value.AsUtf8);
                        break;
                    }

                    UE_LOG(LogEOS, Verbose, TEXT("LobbyInterface:     - Key: %s"), UTF8_TO_TCHAR(Attr->Data->Key));
                    UE_LOG(LogEOS, Verbose, TEXT("LobbyInterface:       Type: %s"), *TypeStr);
                    UE_LOG(LogEOS, Verbose, TEXT("LobbyInterface:       Value: %s"), *ValueStr);

                    EOS_Lobby_Attribute_Release(Attr);
                }
                else
                {
                    UE_LOG(
                        LogEOS,
                        Warning,
                        TEXT("LobbyInterface:     - Error: %s"),
                        *ConvertError(CopyResult).ToLogString());
                }
            }
        }
    }

    UE_LOG(LogEOS, Verbose, TEXT("LobbyInterface: --- Lobby State: %s ( End ) ---"), ANSI_TO_TCHAR(InLobbyId));
}

#endif

void FOnlineLobbyInterfaceEOS::Handle_LobbyUpdateReceived(const EOS_Lobby_LobbyUpdateReceivedCallbackInfo *Data)
{
    UE_LOG(LogEOS, Verbose, TEXT("LobbyInterface: LobbyUpdateReceived: (Lobby) %s"), ANSI_TO_TCHAR(Data->LobbyId));

    auto LobbyIdStr = EOSString_LobbyId::ToString(Data->LobbyId);
    auto LobbyIdEOS = MakeShared<FOnlinePartyIdEOS>(Data->LobbyId);
    if (this->ConnectedMembers.Contains(LobbyIdStr))
    {
#if defined(EOS_ENABLE_STATE_DIAGNOSTICS) && EOS_ENABLE_STATE_DIAGNOSTICS
        this->DumpLocalLobbyAttributeState(
            StaticCastSharedPtr<const FUniqueNetIdEOS>(this->ConnectedMembers[LobbyIdStr][0])->GetProductUserId(),
            Data->LobbyId);
#endif

        for (const auto &Member : this->ConnectedMembers[LobbyIdStr])
        {
            UE_LOG(
                LogEOS,
                Verbose,
                TEXT("LobbyInterface: Propagating LobbyUpdateReceived for: (Lobby) %s (Member) %s"),
                *LobbyIdEOS->ToString(),
                *Member->ToString());
            this->TriggerOnLobbyUpdateDelegates(*Member, *LobbyIdEOS);
        }
    }
    else
    {
        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("LobbyInterface: There are no connected members for lobby %s, so the OnLobbyUpdate event is not being "
                 "propagated"),
            *LobbyIdEOS->ToString());
    }
}

FOnlineLobbyInterfaceEOS::FOnlineLobbyInterfaceEOS(
    EOS_HPlatform InPlatform
#if defined(EOS_VOICE_CHAT_SUPPORTED)
    ,
    const TSharedRef<class FEOSVoiceManager> &InVoiceManager
#endif
    )
    : EOSPlatform(InPlatform)
    , EOSLobby(EOS_Platform_GetLobbyInterface(InPlatform))
    ,
#if defined(EOS_VOICE_CHAT_SUPPORTED)
    VoiceManager(InVoiceManager)
    ,
#endif // #if defined(EOS_VOICE_CHAT_SUPPORTED)
    LastSearchResultsByUser()
    , ConnectedMembers()
    , Unregister_LobbyMemberStatusReceived()
    , Unregister_LobbyMemberUpdateReceived()
    , Unregister_LobbyUpdateReceived()
{
}

FOnlineLobbyInterfaceEOS::~FOnlineLobbyInterfaceEOS()
{
}

void FOnlineLobbyInterfaceEOS::RegisterEvents()
{
    EOS_Lobby_AddNotifyLobbyMemberStatusReceivedOptions OptsLobbyMemberStatus = {};
    OptsLobbyMemberStatus.ApiVersion = EOS_LOBBY_ADDNOTIFYLOBBYMEMBERSTATUSRECEIVED_API_LATEST;
    EOS_Lobby_AddNotifyLobbyMemberUpdateReceivedOptions OptsLobbyMemberUpdate = {};
    OptsLobbyMemberUpdate.ApiVersion = EOS_LOBBY_ADDNOTIFYLOBBYMEMBERUPDATERECEIVED_API_LATEST;
    EOS_Lobby_AddNotifyLobbyUpdateReceivedOptions OptsLobbyUpdate = {};
    OptsLobbyUpdate.ApiVersion = EOS_LOBBY_ADDNOTIFYLOBBYUPDATERECEIVED_API_LATEST;

    this->Unregister_LobbyMemberStatusReceived = EOSRegisterEvent<
        EOS_HLobby,
        EOS_Lobby_AddNotifyLobbyMemberStatusReceivedOptions,
        EOS_Lobby_LobbyMemberStatusReceivedCallbackInfo>(
        this->EOSLobby,
        &OptsLobbyMemberStatus,
        EOS_Lobby_AddNotifyLobbyMemberStatusReceived,
        EOS_Lobby_RemoveNotifyLobbyMemberStatusReceived,
        [WeakThis = GetWeakThis(this)](const EOS_Lobby_LobbyMemberStatusReceivedCallbackInfo *Data) {
            if (auto This = StaticCastSharedPtr<FOnlineLobbyInterfaceEOS>(PinWeakThis(WeakThis)))
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
            if (auto This = StaticCastSharedPtr<FOnlineLobbyInterfaceEOS>(PinWeakThis(WeakThis)))
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
            if (auto This = StaticCastSharedPtr<FOnlineLobbyInterfaceEOS>(PinWeakThis(WeakThis)))
            {
                This->Handle_LobbyUpdateReceived(Data);
            }
        });
}

FDateTime FOnlineLobbyInterfaceEOS::GetUtcNow()
{
    return FDateTime::UtcNow();
}

TSharedPtr<FOnlineLobbyTransaction> FOnlineLobbyInterfaceEOS::MakeCreateLobbyTransaction(const FUniqueNetId &UserId)
{
    return MakeShared<FOnlineLobbyTransactionEOS>();
}

TSharedPtr<FOnlineLobbyTransaction> FOnlineLobbyInterfaceEOS::MakeUpdateLobbyTransaction(
    const FUniqueNetId &UserId,
    const FOnlineLobbyId &LobbyId)
{
    return MakeShared<FOnlineLobbyTransactionEOS>();
}

TSharedPtr<FOnlineLobbyMemberTransaction> FOnlineLobbyInterfaceEOS::MakeUpdateLobbyMemberTransaction(
    const FUniqueNetId &UserId,
    const FOnlineLobbyId &LobbyId,
    const FUniqueNetId &MemberId)
{
    return MakeShared<FOnlineLobbyMemberTransactionEOS>();
}

bool FOnlineLobbyInterfaceEOS::CreateLobby(
    const FUniqueNetId &UserId,
    const FOnlineLobbyTransaction &Transaction,
    FOnLobbyCreateOrConnectComplete OnComplete)
{
    if (UserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::InvalidUser(), UserId, nullptr);
        return true;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared());
    if (!EOSUser->HasValidProductUserId())
    {
        OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::InvalidUser(), UserId, nullptr);
        return true;
    }

    auto EOSTransaction = Transaction.AsShared();

    EOS_Lobby_CreateLobbyOptions Opts = {};
    Opts.ApiVersion = EOS_LOBBY_CREATELOBBY_API_LATEST;
    Opts.LocalUserId = EOSUser->GetProductUserId();
    Opts.MaxLobbyMembers = EOSTransaction->Capacity.Get(4);
    Opts.PermissionLevel = EOSTransaction->Public.Get(false) ? EOS_ELobbyPermissionLevel::EOS_LPL_PUBLICADVERTISED
                                                             : EOS_ELobbyPermissionLevel::EOS_LPL_INVITEONLY;
    Opts.bPresenceEnabled = false;
#if EOS_VERSION_AT_LEAST(1, 11, 0)
    Opts.BucketId = "Default";
    Opts.bAllowInvites = true;
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    // The IOnlineLobby interface is designed around having lobbies close when the leader leaves.
    Opts.bDisableHostMigration = true;
#endif
    bool bVoiceChatEnabled = false;
    bool bEnableEcho = false;
#if EOS_VERSION_AT_LEAST(1, 13, 0) && defined(EOS_VOICE_CHAT_SUPPORTED)
    if (EOSTransaction->SetMetadata.Contains("EOSVoiceChat_Enabled") &&
        EOSTransaction->SetMetadata["EOSVoiceChat_Enabled"].GetType() == EOnlineKeyValuePairDataType::Bool)
    {
        EOSTransaction->SetMetadata["EOSVoiceChat_Enabled"].GetValue(bVoiceChatEnabled);
    }
#if !defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING
    if (EOSTransaction->SetMetadata.Contains("EOSVoiceChat_Echo") &&
        EOSTransaction->SetMetadata["EOSVoiceChat_Echo"].GetType() == EOnlineKeyValuePairDataType::Bool)
    {
        EOSTransaction->SetMetadata["EOSVoiceChat_Echo"].GetValue(bEnableEcho);
    }
#endif
    Opts.bEnableRTCRoom = bVoiceChatEnabled ? EOS_TRUE : EOS_FALSE;
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
    Opts.LocalRTCOptions = bVoiceChatEnabled ? &RTCOpts : nullptr;
#endif

    EOSRunOperation<EOS_HLobby, EOS_Lobby_CreateLobbyOptions, EOS_Lobby_CreateLobbyCallbackInfo>(
        this->EOSLobby,
        &Opts,
        &EOS_Lobby_CreateLobby,
        [WeakThis = GetWeakThis(this), OnComplete, EOSUser, EOSTransaction, bVoiceChatEnabled, bEnableEcho](
            const EOS_Lobby_CreateLobbyCallbackInfo *Info) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Info->ResultCode != EOS_EResult::EOS_Success)
                {
                    OnComplete.ExecuteIfBound(
                        ConvertError(TEXT("EOS_Lobby_CreateLobby"), TEXT("Failed to create lobby"), Info->ResultCode),
                        *EOSUser,
                        nullptr);
                    return;
                }

                auto DeleteLobbyCleanup = [This, Info, EOSUser](const std::function<void()> &OnCleanedUp) {
                    EOS_Lobby_DestroyLobbyOptions DestroyOpts = {};
                    DestroyOpts.ApiVersion = EOS_LOBBY_DESTROYLOBBY_API_LATEST;
                    DestroyOpts.LobbyId = Info->LobbyId;
                    DestroyOpts.LocalUserId = EOSUser->GetProductUserId();
                    EOSRunOperation<EOS_HLobby, EOS_Lobby_DestroyLobbyOptions, EOS_Lobby_DestroyLobbyCallbackInfo>(
                        This->EOSLobby,
                        &DestroyOpts,
                        EOS_Lobby_DestroyLobby,
                        [OnCleanedUp](const EOS_Lobby_DestroyLobbyCallbackInfo *Info) {
                            if (Info->ResultCode != EOS_EResult::EOS_Success)
                            {
                                UE_LOG(
                                    LogEOS,
                                    Error,
                                    TEXT("Failed to destroy lobby during partial creation failure: %s"),
                                    ANSI_TO_TCHAR(EOS_EResult_ToString(Info->ResultCode)));
                            }

                            OnCleanedUp();
                        });
                };

                EOS_Lobby_UpdateLobbyModificationOptions ModOpts = {};
                ModOpts.ApiVersion = EOS_LOBBY_UPDATELOBBYMODIFICATION_API_LATEST;
                ModOpts.LobbyId = Info->LobbyId;
                ModOpts.LocalUserId = EOSUser->GetProductUserId();

                EOS_HLobbyModification ModHandle = {};
                EOS_EResult ModResult = EOS_Lobby_UpdateLobbyModification(This->EOSLobby, &ModOpts, &ModHandle);
                if (ModResult != EOS_EResult::EOS_Success)
                {
                    DeleteLobbyCleanup([OnComplete, EOSUser, ModResult]() {
                        OnComplete.ExecuteIfBound(
                            ConvertError(
                                TEXT("EOS_Lobby_UpdateLobbyModification"),
                                TEXT("Failed to create lobby update modification"),
                                ModResult),
                            *EOSUser,
                            nullptr);
                    });
                    return;
                }

                // Now we need to set metadata on the new lobby, because EOS doesn't do it in a single operation.
                for (const auto &Metadata : EOSTransaction->SetMetadata)
                {
                    auto MetadataKey = EOSString_LobbyModification_AttributeKey::ToAnsiString(Metadata.Key);
                    if (MetadataKey.Result != EOS_EResult::EOS_Success)
                    {
                        DeleteLobbyCleanup([OnComplete, EOSUser, Metadata, Result = MetadataKey.Result]() {
                            OnComplete.ExecuteIfBound(
                                ConvertError(
                                    TEXT("EOSString_LobbyModification_AttributeKey::ToAnsiString"),
                                    FString::Printf(TEXT("The attribute key '%s' is not valid"), *Metadata.Key),
                                    Result),
                                *EOSUser,
                                nullptr);
                        });
                        return;
                    }

                    auto MetadataValue =
                        EOSString_LobbyModification_AttributeStringValue::ToUtf8String(Metadata.Value.ToString());
                    if (MetadataValue.Result != EOS_EResult::EOS_Success)
                    {
                        DeleteLobbyCleanup([OnComplete, EOSUser, Metadata, Result = MetadataValue.Result]() {
                            OnComplete.ExecuteIfBound(
                                ConvertError(
                                    TEXT("EOSString_LobbyModification_AttributeStringValue::ToUtf8String"),
                                    FString::Printf(TEXT("The attribute key '%s' has an invalid value"), *Metadata.Key),
                                    Result),
                                *EOSUser,
                                nullptr);
                        });
                        return;
                    }

                    EOS_Lobby_AttributeData Attr = {};
                    Attr.ApiVersion = EOS_LOBBY_ATTRIBUTEDATA_API_LATEST;
                    Attr.Key = MetadataKey.Ptr.Get();

                    switch (Metadata.Value.GetType())
                    {
                    case EOnlineKeyValuePairDataType::Bool:
                        Attr.ValueType = EOS_ELobbyAttributeType::EOS_AT_BOOLEAN;
                        bool BoolVal;
                        Metadata.Value.GetValue(BoolVal);
                        Attr.Value.AsBool = BoolVal ? EOS_TRUE : EOS_FALSE;
                        break;
                    case EOnlineKeyValuePairDataType::Int64:
                        Attr.ValueType = EOS_ELobbyAttributeType::EOS_AT_INT64;
                        int64 Int64Val;
                        Metadata.Value.GetValue(Int64Val);
                        Attr.Value.AsInt64 = Int64Val;
                        break;
                    case EOnlineKeyValuePairDataType::Double:
                        Attr.ValueType = EOS_ELobbyAttributeType::EOS_AT_DOUBLE;
                        Metadata.Value.GetValue(Attr.Value.AsDouble);
                        break;
                    case EOnlineKeyValuePairDataType::String:
                        Attr.ValueType = EOS_ELobbyAttributeType::EOS_AT_STRING;
                        Attr.Value.AsUtf8 = MetadataValue.GetAsChar();
                        break;
                    default:
                        DeleteLobbyCleanup([OnComplete, EOSUser, Metadata]() {
                            OnComplete.ExecuteIfBound(
                                ConvertError(
                                    TEXT("EOS_Lobby_AttributeData.Type"),
                                    FString::Printf(
                                        TEXT("The attribute '%s' has an unsupported type of value"),
                                        *Metadata.Key),
                                    EOS_EResult::EOS_InvalidParameters),
                                *EOSUser,
                                nullptr);
                        });
                        return;
                    }

                    EOS_LobbyModification_AddAttributeOptions AddOpts = {};
                    AddOpts.ApiVersion = EOS_LOBBYMODIFICATION_ADDATTRIBUTE_API_LATEST;
                    AddOpts.Attribute = &Attr;
                    AddOpts.Visibility = EOS_ELobbyAttributeVisibility::EOS_LAT_PUBLIC;

                    EOS_EResult AttrResult = EOS_LobbyModification_AddAttribute(ModHandle, &AddOpts);
                    if (AttrResult != EOS_EResult::EOS_Success)
                    {
                        DeleteLobbyCleanup([OnComplete, EOSUser, Metadata, AttrResult]() {
                            OnComplete.ExecuteIfBound(
                                ConvertError(
                                    TEXT("EOS_LobbyModification_AddAttribute"),
                                    FString::Printf(
                                        TEXT("Failed to add attribute '%s' to modification transaction"),
                                        *Metadata.Key),
                                    AttrResult),
                                *EOSUser,
                                nullptr);
                        });
                        return;
                    }
                }

                EOS_Lobby_UpdateLobbyOptions UpdateOpts = {};
                UpdateOpts.ApiVersion = EOS_LOBBY_UPDATELOBBY_API_LATEST;
                UpdateOpts.LobbyModificationHandle = ModHandle;

                EOSRunOperation<EOS_HLobby, EOS_Lobby_UpdateLobbyOptions, EOS_Lobby_UpdateLobbyCallbackInfo>(
                    This->EOSLobby,
                    &UpdateOpts,
                    &EOS_Lobby_UpdateLobby,
                    [WeakThis = GetWeakThis(This),
                     ModHandle,
                     DeleteLobbyCleanup,
                     OnComplete,
                     EOSUser,
                     bVoiceChatEnabled,
                     bEnableEcho](const EOS_Lobby_UpdateLobbyCallbackInfo *Info) {
                        EOS_LobbyModification_Release(ModHandle);

                        if (auto This = PinWeakThis(WeakThis))
                        {
                            if (Info->ResultCode != EOS_EResult::EOS_Success &&
                                Info->ResultCode != EOS_EResult::EOS_NoChange)
                            {
                                DeleteLobbyCleanup([OnComplete, EOSUser, Result = Info->ResultCode]() {
                                    OnComplete.ExecuteIfBound(
                                        ConvertError(
                                            TEXT("EOS_Lobby_UpdateLobby"),
                                            TEXT("Failed to update lobby to add attributes after creation"),
                                            Result),
                                        *EOSUser,
                                        nullptr);
                                });
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

                            TSharedRef<FOnlineLobbyEOS> ResultLobby = MakeShared<FOnlineLobbyEOS>();
                            ResultLobby->Id = MakeShared<FOnlinePartyIdEOS>(Info->LobbyId);
                            ResultLobby->OwnerId = MakeShared<FUniqueNetIdEOS>(EOSUser->GetProductUserId());
                            This->Internal_AddMemberToLobby(*EOSUser, *ResultLobby->Id);
                            OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::Success(), *EOSUser, ResultLobby);
                        }
                    });
            }
        });
    return true;
}

bool FOnlineLobbyInterfaceEOS::UpdateLobby(
    const FUniqueNetId &UserId,
    const FOnlineLobbyId &LobbyId,
    const FOnlineLobbyTransaction &Transaction,
    FOnLobbyOperationComplete OnComplete)
{
    if (UserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::InvalidUser(), UserId);
        return true;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared());
    if (!EOSUser->HasValidProductUserId())
    {
        OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::InvalidUser(), UserId);
        return true;
    }

    auto EOSLobbyId = StaticCastSharedRef<const FOnlinePartyIdEOS>(LobbyId.AsShared());

    auto EOSTransaction = Transaction.AsShared();

    EOS_Lobby_UpdateLobbyModificationOptions ModOpts = {};
    ModOpts.ApiVersion = EOS_LOBBY_UPDATELOBBYMODIFICATION_API_LATEST;
    ModOpts.LobbyId = EOSLobbyId->GetLobbyId();
    ModOpts.LocalUserId = EOSUser->GetProductUserId();

    EOS_HLobbyModification ModHandle = {};
    EOS_EResult ModResult = EOS_Lobby_UpdateLobbyModification(this->EOSLobby, &ModOpts, &ModHandle);
    if (ModResult != EOS_EResult::EOS_Success)
    {
        OnComplete.ExecuteIfBound(
            ConvertError(
                TEXT("EOS_Lobby_UpdateLobbyModification"),
                TEXT("Failed to create lobby update modification"),
                ModResult),
            *EOSUser);
        return true;
    }

    if (Transaction.Capacity.IsSet())
    {
        EOS_LobbyModification_SetMaxMembersOptions ModCapacity = {};
        ModCapacity.ApiVersion = EOS_LOBBYMODIFICATION_SETMAXMEMBERS_API_LATEST;
        ModCapacity.MaxMembers = Transaction.Capacity.Get(4);

        EOS_EResult CapacityResult = EOS_LobbyModification_SetMaxMembers(ModHandle, &ModCapacity);
        if (CapacityResult != EOS_EResult::EOS_Success)
        {
            OnComplete.ExecuteIfBound(
                ConvertError(
                    TEXT("EOS_LobbyModification_SetMaxMembers"),
                    TEXT("Unable to set capacity"),
                    CapacityResult),
                *EOSUser);
            return true;
        }
    }

    if (Transaction.Public.IsSet())
    {
        EOS_LobbyModification_SetPermissionLevelOptions ModPerm = {};
        ModPerm.ApiVersion = EOS_LOBBYMODIFICATION_SETPERMISSIONLEVEL_API_LATEST;
        ModPerm.PermissionLevel = Transaction.Public.Get(false) ? EOS_ELobbyPermissionLevel::EOS_LPL_PUBLICADVERTISED
                                                                : EOS_ELobbyPermissionLevel::EOS_LPL_INVITEONLY;

        EOS_EResult CapacityResult = EOS_LobbyModification_SetPermissionLevel(ModHandle, &ModPerm);
        if (CapacityResult != EOS_EResult::EOS_Success)
        {
            OnComplete.ExecuteIfBound(
                ConvertError(
                    TEXT("EOS_LobbyModification_SetPermissionLevel"),
                    TEXT("Unable to set visibility"),
                    CapacityResult),
                *EOSUser);
            return true;
        }
    }

    for (const auto &Metadata : Transaction.SetMetadata)
    {
        auto MetadataKey = EOSString_LobbyModification_AttributeKey::ToAnsiString(Metadata.Key);
        if (MetadataKey.Result != EOS_EResult::EOS_Success)
        {
            OnComplete.ExecuteIfBound(
                ConvertError(
                    TEXT("EOSString_LobbyModification_AttributeKey::ToAnsiString"),
                    FString::Printf(TEXT("The attribute key '%s' is not valid"), *Metadata.Key),
                    MetadataKey.Result),
                *EOSUser);
            return true;
        }

        auto MetadataValue = EOSString_LobbyModification_AttributeStringValue::ToUtf8String(Metadata.Value.ToString());
        if (MetadataValue.Result != EOS_EResult::EOS_Success)
        {
            OnComplete.ExecuteIfBound(
                ConvertError(
                    TEXT("EOSString_LobbyModification_AttributeStringValue::ToUtf8String"),
                    FString::Printf(TEXT("The attribute key '%s' has an invalid value"), *Metadata.Key),
                    MetadataValue.Result),
                *EOSUser);
            return true;
        }

        EOS_Lobby_AttributeData Attr = {};
        Attr.ApiVersion = EOS_LOBBY_ATTRIBUTEDATA_API_LATEST;
        Attr.Key = MetadataKey.Ptr.Get();

        switch (Metadata.Value.GetType())
        {
        case EOnlineKeyValuePairDataType::Bool:
            Attr.ValueType = EOS_ELobbyAttributeType::EOS_AT_BOOLEAN;
            bool BoolVal;
            Metadata.Value.GetValue(BoolVal);
            Attr.Value.AsBool = BoolVal ? EOS_TRUE : EOS_FALSE;
            break;
        case EOnlineKeyValuePairDataType::Int64:
            Attr.ValueType = EOS_ELobbyAttributeType::EOS_AT_INT64;
            int64 Int64Val;
            Metadata.Value.GetValue(Int64Val);
            Attr.Value.AsInt64 = Int64Val;
            break;
        case EOnlineKeyValuePairDataType::Double:
            Attr.ValueType = EOS_ELobbyAttributeType::EOS_AT_DOUBLE;
            Metadata.Value.GetValue(Attr.Value.AsDouble);
            break;
        case EOnlineKeyValuePairDataType::String:
            Attr.ValueType = EOS_ELobbyAttributeType::EOS_AT_STRING;
            Attr.Value.AsUtf8 = MetadataValue.GetAsChar();
            break;
        default:
            OnComplete.ExecuteIfBound(
                ConvertError(
                    TEXT("EOS_Lobby_AttributeData.ValueType"),
                    FString::Printf(TEXT("The attribute '%s' has an unsupported type of value"), *Metadata.Key),
                    EOS_EResult::EOS_InvalidParameters),
                *EOSUser);
            return true;
        }

        EOS_LobbyModification_AddAttributeOptions AddOpts = {};
        AddOpts.ApiVersion = EOS_LOBBYMODIFICATION_ADDATTRIBUTE_API_LATEST;
        AddOpts.Attribute = &Attr;
        AddOpts.Visibility = EOS_ELobbyAttributeVisibility::EOS_LAT_PUBLIC;

        EOS_EResult AttrResult = EOS_LobbyModification_AddAttribute(ModHandle, &AddOpts);
        if (AttrResult != EOS_EResult::EOS_Success)
        {
            OnComplete.ExecuteIfBound(
                ConvertError(
                    TEXT("EOS_LobbyModification_AddAttribute"),
                    FString::Printf(TEXT("Failed to add attribute '%s' to modification transaction"), *Metadata.Key),
                    AttrResult),
                *EOSUser);
            return true;
        }
    }

    for (const auto &Metadata : Transaction.DeleteMetadata)
    {
        auto MetadataKey = EOSString_LobbyModification_AttributeKey::ToAnsiString(Metadata);
        if (MetadataKey.Result != EOS_EResult::EOS_Success)
        {
            OnComplete.ExecuteIfBound(
                ConvertError(
                    TEXT("EOSString_LobbyModification_AttributeKey::ToAnsiString"),
                    FString::Printf(TEXT("The attribute key '%s' is not valid"), *Metadata),
                    MetadataKey.Result),
                *EOSUser);
            return true;
        }

        EOS_LobbyModification_RemoveAttributeOptions RemoveOpts = {};
        RemoveOpts.ApiVersion = EOS_LOBBYMODIFICATION_REMOVEATTRIBUTE_API_LATEST;
        RemoveOpts.Key = MetadataKey.Ptr.Get();

        EOS_EResult AttrResult = EOS_LobbyModification_RemoveAttribute(ModHandle, &RemoveOpts);
        if (AttrResult != EOS_EResult::EOS_Success)
        {
            OnComplete.ExecuteIfBound(
                ConvertError(
                    TEXT("EOSString_LobbyModification_AttributeKey::ToAnsiString"),
                    FString::Printf(TEXT("Unable to remove attribute '%s'"), *Metadata),
                    AttrResult),
                *EOSUser);
            return true;
        }
    }

    EOS_Lobby_UpdateLobbyOptions UpdateOpts = {};
    UpdateOpts.ApiVersion = EOS_LOBBY_UPDATELOBBY_API_LATEST;
    UpdateOpts.LobbyModificationHandle = ModHandle;

    EOSRunOperation<EOS_HLobby, EOS_Lobby_UpdateLobbyOptions, EOS_Lobby_UpdateLobbyCallbackInfo>(
        this->EOSLobby,
        &UpdateOpts,
        &EOS_Lobby_UpdateLobby,
        [WeakThis = GetWeakThis(this), ModHandle, EOSUser, OnComplete](const EOS_Lobby_UpdateLobbyCallbackInfo *Info) {
            EOS_LobbyModification_Release(ModHandle);

            if (auto This = PinWeakThis(WeakThis))
            {
                if (Info->ResultCode != EOS_EResult::EOS_Success && Info->ResultCode != EOS_EResult::EOS_NoChange)
                {
                    OnComplete.ExecuteIfBound(
                        ConvertError(TEXT("EOS_Lobby_UpdateLobby"), TEXT("Unable to update lobby"), Info->ResultCode),
                        *EOSUser);
                    return;
                }

                OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::Success(), *EOSUser);
            }
        });
    return true;
}

bool FOnlineLobbyInterfaceEOS::DeleteLobby(
    const FUniqueNetId &UserId,
    const FOnlineLobbyId &LobbyId,
    FOnLobbyOperationComplete OnComplete)
{
    if (UserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::InvalidUser(), UserId);
        return true;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared());
    if (!EOSUser->HasValidProductUserId())
    {
        OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::InvalidUser(), UserId);
        return true;
    }

    auto EOSLobbyId = StaticCastSharedRef<const FOnlinePartyIdEOS>(LobbyId.AsShared());

    FString RTCRoomName;
#if EOS_VERSION_AT_LEAST(1, 13, 0) && defined(EOS_VOICE_CHAT_SUPPORTED)
    // Before we leave, try to get the RTC room name. If we can get it, then we need to unregister this RTC channel
    // after we leave successfully.
    {
        EOS_Lobby_GetRTCRoomNameOptions GetRTCRoomNameOpts = {};
        GetRTCRoomNameOpts.ApiVersion = EOS_LOBBY_GETRTCROOMNAME_API_LATEST;
        GetRTCRoomNameOpts.LocalUserId = EOSUser->GetProductUserId();
        GetRTCRoomNameOpts.LobbyId = EOSLobbyId->GetLobbyId();

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

    EOS_Lobby_DestroyLobbyOptions Opts = {};
    Opts.ApiVersion = EOS_LOBBY_DESTROYLOBBY_API_LATEST;
    Opts.LocalUserId = EOSUser->GetProductUserId();
    Opts.LobbyId = EOSLobbyId->GetLobbyId();

    EOSRunOperation<EOS_HLobby, EOS_Lobby_DestroyLobbyOptions, EOS_Lobby_DestroyLobbyCallbackInfo>(
        this->EOSLobby,
        &Opts,
        &EOS_Lobby_DestroyLobby,
        [WeakThis = GetWeakThis(this), OnComplete, EOSUser, EOSLobbyId, RTCRoomName](
            const EOS_Lobby_DestroyLobbyCallbackInfo *Info) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Info->ResultCode != EOS_EResult::EOS_Success)
                {
                    OnComplete.ExecuteIfBound(
                        ConvertError(TEXT("EOS_Lobby_DestroyLobby"), TEXT("Unable to delete lobby"), Info->ResultCode),
                        *EOSUser);
                    return;
                }

                // We have to trigger this event manually for the host, as they don't receive
                // the event otherwise.
                This->TriggerOnLobbyDeleteDelegates(*EOSUser, *EOSLobbyId);
                This->Internal_RemoveMemberFromLobby(*EOSUser, *EOSLobbyId);

                OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::Success(), *EOSUser);

#if EOS_VERSION_AT_LEAST(1, 13, 0) && defined(EOS_VOICE_CHAT_SUPPORTED)
                if (!RTCRoomName.IsEmpty())
                {
                    TSharedPtr<FEOSVoiceManagerLocalUser> VoiceUser = This->VoiceManager->GetLocalUser(*EOSUser);
                    if (VoiceUser.IsValid())
                    {
                        VoiceUser->UnregisterLobbyChannel(RTCRoomName);
                    }
                }
#endif
            }
        });
    return true;
}

bool FOnlineLobbyInterfaceEOS::ConnectLobby(
    const FUniqueNetId &UserId,
    const FOnlineLobbyId &LobbyId,
    FOnLobbyCreateOrConnectComplete OnComplete)
{
    if (UserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::InvalidUser(), UserId, nullptr);
        return true;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared());
    if (!EOSUser->HasValidProductUserId())
    {
        OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::InvalidUser(), UserId, nullptr);
        return true;
    }

    auto EOSLobbyId = StaticCastSharedRef<const FOnlinePartyIdEOS>(LobbyId.AsShared());

    // Once we have the handle either via quick lookup or via full search, this connects to the lobby
    // using the obtained handle.
    auto ConnectWithHandle = [EOSUser](
                                 const TSharedRef<FOnlineLobbyInterfaceEOS, ESPMode::ThreadSafe> &This,
                                 const TSharedRef<const FUniqueNetIdEOS> &UserId,
                                 EOS_HLobbyDetails LobbyDetails,
                                 const FOnLobbyCreateOrConnectComplete &OnComplete,
                                 bool bShouldRelease) {
        EOS_Lobby_JoinLobbyOptions Opts = {};
        Opts.ApiVersion = EOS_LOBBY_JOINLOBBY_API_LATEST;
        Opts.LobbyDetailsHandle = LobbyDetails;
        Opts.LocalUserId = EOSUser->GetProductUserId();

        EOSRunOperation<EOS_HLobby, EOS_Lobby_JoinLobbyOptions, EOS_Lobby_JoinLobbyCallbackInfo>(
            This->EOSLobby,
            &Opts,
            &EOS_Lobby_JoinLobby,
            [WeakThis = GetWeakThis(This), LobbyDetails, OnComplete, EOSUser, bShouldRelease](
                const EOS_Lobby_JoinLobbyCallbackInfo *Info) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    if (Info->ResultCode != EOS_EResult::EOS_Success)
                    {
                        if (bShouldRelease)
                        {
                            EOS_LobbyDetails_Release(LobbyDetails);
                        }

                        OnComplete.ExecuteIfBound(
                            ConvertError(TEXT("EOS_Lobby_JoinLobby"), TEXT("Unable to join lobby"), Info->ResultCode),
                            *EOSUser,
                            nullptr);
                        return;
                    }

#if EOS_VERSION_AT_LEAST(1, 13, 0) && defined(EOS_VOICE_CHAT_SUPPORTED)
                    // Just try to get the RTC room name; if it succeeds then this lobby is RTC enabled.
                    {
                        TSharedPtr<FEOSVoiceManagerLocalUser> VoiceUser = This->VoiceManager->GetLocalUser(*EOSUser);
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
                                VoiceUser->RegisterLobbyChannel(RoomName, EVoiceChatChannelType::NonPositional);
                            }
                        }
                    }
#endif

#if defined(EOS_ENABLE_STATE_DIAGNOSTICS) && EOS_ENABLE_STATE_DIAGNOSTICS
                    This->DumpLocalLobbyAttributeState(EOSUser->GetProductUserId(), Info->LobbyId);
#endif

                    EOS_LobbyDetails_GetLobbyOwnerOptions OwnerOpts = {};
                    OwnerOpts.ApiVersion = EOS_LOBBYDETAILS_GETLOBBYOWNER_API_LATEST;
                    auto OwnerId = EOS_LobbyDetails_GetLobbyOwner(LobbyDetails, &OwnerOpts);

                    TSharedRef<FOnlineLobbyEOS> ResultLobby = MakeShared<FOnlineLobbyEOS>();
                    ResultLobby->Id = MakeShared<FOnlinePartyIdEOS>(Info->LobbyId);
                    ResultLobby->OwnerId = EOS_ProductUserId_IsValid(OwnerId) ? MakeShared<FUniqueNetIdEOS>(OwnerId)
                                                                              : FUniqueNetIdEOS::InvalidId();
                    This->Internal_AddMemberToLobby(*EOSUser, *ResultLobby->Id);
                    OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::Success(), *EOSUser, ResultLobby);
                }
            });
    };

    // Try to get the details handle the quick way - via CopyLobbyDetailsHandle. This only works
    // for lobbies that the EOS SDK has in memory or are in the search results.
    {
        EOS_HLobbyDetails LobbyDetails = nullptr;
        bool bShouldRelease = false;

        auto Result = CopyLobbyDetailsHandle(*EOSUser, *EOSLobbyId, LobbyDetails, bShouldRelease);
        if (Result == EOS_EResult::EOS_Success)
        {
            ConnectWithHandle(this->AsShared(), EOSUser, LobbyDetails, OnComplete, bShouldRelease);
            return true;
        }
        if (LobbyDetails != nullptr)
        {
            if (bShouldRelease)
            {
                EOS_LobbyDetails_Release(LobbyDetails);
            }
            LobbyDetails = nullptr;
        }
    }

    // We couldn't get the handle in memory, do a search by lobby ID to get it.
    {
        EOS_HLobbySearch LobbySearch = {};
        EOS_Lobby_CreateLobbySearchOptions SearchOpts = {};
        SearchOpts.ApiVersion = EOS_LOBBY_CREATELOBBYSEARCH_API_LATEST;
        SearchOpts.MaxResults = 1;
        EOS_EResult CreateSearchResult = EOS_Lobby_CreateLobbySearch(this->EOSLobby, &SearchOpts, &LobbySearch);
        if (CreateSearchResult != EOS_EResult::EOS_Success)
        {
            OnComplete.ExecuteIfBound(
                ConvertError(
                    TEXT("EOS_Lobby_CreateLobbySearch"),
                    TEXT("Unable to create search for looking up lobby"),
                    CreateSearchResult),
                UserId,
                nullptr);
            return true;
        }

        EOS_LobbySearch_SetLobbyIdOptions IdOpts = {};
        IdOpts.ApiVersion = EOS_LOBBYSEARCH_SETLOBBYID_API_LATEST;
        IdOpts.LobbyId = EOSLobbyId->GetLobbyId();
        EOS_EResult SearchSetIdResult = EOS_LobbySearch_SetLobbyId(LobbySearch, &IdOpts);
        if (SearchSetIdResult != EOS_EResult::EOS_Success)
        {
            EOS_LobbySearch_Release(LobbySearch);
            OnComplete.ExecuteIfBound(
                ConvertError(
                    TEXT("EOS_LobbySearch_SetLobbyId"),
                    TEXT("Unable to create set lobby ID on search for looking up lobby"),
                    SearchSetIdResult),
                UserId,
                nullptr);
            return true;
        }

        EOS_LobbySearch_FindOptions FindOpts = {};
        FindOpts.ApiVersion = EOS_LOBBYSEARCH_FIND_API_LATEST;
        FindOpts.LocalUserId = EOSUser->GetProductUserId();
        EOSRunOperation<EOS_HLobbySearch, EOS_LobbySearch_FindOptions, EOS_LobbySearch_FindCallbackInfo>(
            LobbySearch,
            &FindOpts,
            EOS_LobbySearch_Find,
            [WeakThis = GetWeakThis(this), EOSUser, OnComplete, LobbySearch, ConnectWithHandle](
                const EOS_LobbySearch_FindCallbackInfo *Info) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    if (Info->ResultCode != EOS_EResult::EOS_Success)
                    {
                        EOS_LobbySearch_Release(LobbySearch);
                        OnComplete.ExecuteIfBound(
                            ConvertError(
                                TEXT("EOS_LobbySearch_Find"),
                                TEXT("Unable to search for lobby"),
                                Info->ResultCode),
                            *EOSUser,
                            nullptr);
                        return;
                    }

                    EOS_LobbySearch_GetSearchResultCountOptions CountOpts = {};
                    CountOpts.ApiVersion = EOS_LOBBYSEARCH_GETSEARCHRESULTCOUNT_API_LATEST;
                    if (EOS_LobbySearch_GetSearchResultCount(LobbySearch, &CountOpts) < 1)
                    {
                        EOS_LobbySearch_Release(LobbySearch);
                        OnComplete.ExecuteIfBound(
                            ConvertError(
                                TEXT("EOS_LobbySearch_GetSearchResultCount"),
                                TEXT("Unable to find lobby (it does not exist)"),
                                EOS_EResult::EOS_NotFound),
                            *EOSUser,
                            nullptr);
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
                        OnComplete.ExecuteIfBound(
                            ConvertError(
                                TEXT("EOS_LobbySearch_CopySearchResultByIndex"),
                                TEXT("Unable to copy lobby handle from search result"),
                                CopyResult),
                            *EOSUser,
                            nullptr);
                        return;
                    }

                    EOS_LobbySearch_Release(LobbySearch);

                    ConnectWithHandle(This.ToSharedRef(), EOSUser, LobbyDetails, OnComplete, true);
                }
            });
        return true;
    }
}

bool FOnlineLobbyInterfaceEOS::DisconnectLobby(
    const FUniqueNetId &UserId,
    const FOnlineLobbyId &LobbyId,
    FOnLobbyOperationComplete OnComplete)
{
    if (UserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::InvalidUser(), UserId);
        return true;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared());
    if (!EOSUser->HasValidProductUserId())
    {
        OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::InvalidUser(), UserId);
        return true;
    }

    auto EOSLobbyId = StaticCastSharedRef<const FOnlinePartyIdEOS>(LobbyId.AsShared());

    FString RTCRoomName;
#if EOS_VERSION_AT_LEAST(1, 13, 0) && defined(EOS_VOICE_CHAT_SUPPORTED)
    // Before we leave, try to get the RTC room name. If we can get it, then we need to unregister this RTC channel
    // after we leave successfully.
    {
        EOS_Lobby_GetRTCRoomNameOptions GetRTCRoomNameOpts = {};
        GetRTCRoomNameOpts.ApiVersion = EOS_LOBBY_GETRTCROOMNAME_API_LATEST;
        GetRTCRoomNameOpts.LocalUserId = EOSUser->GetProductUserId();
        GetRTCRoomNameOpts.LobbyId = EOSLobbyId->GetLobbyId();

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
    Opts.LobbyId = EOSLobbyId->GetLobbyId();
    Opts.LocalUserId = EOSUser->GetProductUserId();
    EOSRunOperation<EOS_HLobby, EOS_Lobby_LeaveLobbyOptions, EOS_Lobby_LeaveLobbyCallbackInfo>(
        this->EOSLobby,
        &Opts,
        EOS_Lobby_LeaveLobby,
        [WeakThis = GetWeakThis(this), OnComplete, EOSUser, EOSLobbyId, RTCRoomName](
            const EOS_Lobby_LeaveLobbyCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Data->ResultCode == EOS_EResult::EOS_Success || Data->ResultCode == EOS_EResult::EOS_NotFound)
                {
                    // We have to trigger this event manually for the user, as they don't receive
                    // the event otherwise.
                    This->TriggerOnMemberDisconnectDelegates(*EOSUser, *EOSLobbyId, *EOSUser, false);
                    This->Internal_RemoveMemberFromLobby(*EOSUser, *EOSLobbyId);
                    OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::Success(), *EOSUser);
#if EOS_VERSION_AT_LEAST(1, 13, 0) && defined(EOS_VOICE_CHAT_SUPPORTED)
                    if (!RTCRoomName.IsEmpty())
                    {
                        TSharedPtr<FEOSVoiceManagerLocalUser> VoiceUser = This->VoiceManager->GetLocalUser(*EOSUser);
                        if (VoiceUser.IsValid())
                        {
                            VoiceUser->UnregisterLobbyChannel(RTCRoomName);
                        }
                    }
#endif
                }
                else
                {
                    OnComplete.ExecuteIfBound(
                        ConvertError(
                            TEXT("EOS_Lobby_LeaveLobby"),
                            TEXT("Unable to disconnect from lobby"),
                            Data->ResultCode),
                        *EOSUser);
                }
            }
        });
    return true;
}

bool FOnlineLobbyInterfaceEOS::UpdateMemberSelf(
    const FUniqueNetId &UserId,
    const FOnlineLobbyId &LobbyId,
    const FOnlineLobbyMemberTransaction &Transaction,
    FOnLobbyOperationComplete OnComplete)
{
    if (UserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::InvalidUser(), UserId);
        return true;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared());
    if (!EOSUser->HasValidProductUserId())
    {
        OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::InvalidUser(), UserId);
        return true;
    }

    auto EOSLobbyId = StaticCastSharedRef<const FOnlinePartyIdEOS>(LobbyId.AsShared());

    auto EOSTransaction = Transaction.AsShared();

    EOS_Lobby_UpdateLobbyModificationOptions ModOpts = {};
    ModOpts.ApiVersion = EOS_LOBBY_UPDATELOBBYMODIFICATION_API_LATEST;
    ModOpts.LobbyId = EOSLobbyId->GetLobbyId();
    ModOpts.LocalUserId = EOSUser->GetProductUserId();

    EOS_HLobbyModification ModHandle = {};
    EOS_EResult ModResult = EOS_Lobby_UpdateLobbyModification(this->EOSLobby, &ModOpts, &ModHandle);
    if (ModResult != EOS_EResult::EOS_Success)
    {
        OnComplete.ExecuteIfBound(
            ConvertError(
                TEXT("EOS_Lobby_UpdateLobbyModification"),
                TEXT("Failed to create lobby update modification"),
                ModResult),
            *EOSUser);
        return true;
    }

    for (const auto &Metadata : Transaction.SetMetadata)
    {
        auto MetadataKey = EOSString_LobbyModification_AttributeKey::ToAnsiString(Metadata.Key);
        if (MetadataKey.Result != EOS_EResult::EOS_Success)
        {
            OnComplete.ExecuteIfBound(
                ConvertError(
                    TEXT("EOSString_LobbyModification_AttributeKey::ToAnsiString"),
                    FString::Printf(TEXT("The attribute key '%s' is not valid"), *Metadata.Key),
                    MetadataKey.Result),
                *EOSUser);
            return true;
        }

        auto MetadataValue = EOSString_LobbyModification_AttributeStringValue::ToUtf8String(Metadata.Value.ToString());
        if (MetadataValue.Result != EOS_EResult::EOS_Success)
        {
            OnComplete.ExecuteIfBound(
                ConvertError(
                    TEXT("EOSString_LobbyModification_AttributeStringValue::ToUtf8String"),
                    FString::Printf(TEXT("The attribute key '%s' has an invalid value"), *Metadata.Key),
                    MetadataValue.Result),
                *EOSUser);
            return true;
        }

        EOS_Lobby_AttributeData Attr = {};
        Attr.ApiVersion = EOS_LOBBY_ATTRIBUTEDATA_API_LATEST;
        Attr.Key = MetadataKey.Ptr.Get();

        switch (Metadata.Value.GetType())
        {
        case EOnlineKeyValuePairDataType::Bool:
            Attr.ValueType = EOS_ELobbyAttributeType::EOS_AT_BOOLEAN;
            bool BoolVal;
            Metadata.Value.GetValue(BoolVal);
            Attr.Value.AsBool = BoolVal ? EOS_TRUE : EOS_FALSE;
            break;
        case EOnlineKeyValuePairDataType::Int64:
            Attr.ValueType = EOS_ELobbyAttributeType::EOS_AT_INT64;
            int64 Int64Val;
            Metadata.Value.GetValue(Int64Val);
            Attr.Value.AsInt64 = Int64Val;
            break;
        case EOnlineKeyValuePairDataType::Double:
            Attr.ValueType = EOS_ELobbyAttributeType::EOS_AT_DOUBLE;
            Metadata.Value.GetValue(Attr.Value.AsDouble);
            break;
        case EOnlineKeyValuePairDataType::String:
            Attr.ValueType = EOS_ELobbyAttributeType::EOS_AT_STRING;
            Attr.Value.AsUtf8 = MetadataValue.GetAsChar();
            break;
        default:
            OnComplete.ExecuteIfBound(
                ConvertError(
                    TEXT("EOS_Lobby_AttributeData.ValueType"),
                    FString::Printf(TEXT("The attribute '%s' has an unsupported type of value"), *Metadata.Key),
                    EOS_EResult::EOS_InvalidParameters),
                *EOSUser);
            return true;
        }

        EOS_LobbyModification_AddMemberAttributeOptions AddOpts = {};
        AddOpts.ApiVersion = EOS_LOBBYMODIFICATION_ADDMEMBERATTRIBUTE_API_LATEST;
        AddOpts.Attribute = &Attr;
        AddOpts.Visibility = EOS_ELobbyAttributeVisibility::EOS_LAT_PUBLIC;

        EOS_EResult AttrResult = EOS_LobbyModification_AddMemberAttribute(ModHandle, &AddOpts);
        if (AttrResult != EOS_EResult::EOS_Success)
        {
            OnComplete.ExecuteIfBound(
                ConvertError(
                    TEXT("EOS_LobbyModification_AddMemberAttribute"),
                    FString::Printf(TEXT("Failed to add attribute '%s' to modification transaction"), *Metadata.Key),
                    AttrResult),
                *EOSUser);
            return true;
        }
    }

    for (const auto &Metadata : Transaction.DeleteMetadata)
    {
        auto MetadataKey = EOSString_LobbyModification_AttributeKey::ToAnsiString(Metadata);
        if (MetadataKey.Result != EOS_EResult::EOS_Success)
        {
            OnComplete.ExecuteIfBound(
                ConvertError(
                    TEXT("EOSString_LobbyModification_AttributeKey::ToAnsiString"),
                    FString::Printf(TEXT("The attribute key '%s' is not valid"), *Metadata),
                    MetadataKey.Result),
                *EOSUser);
            return true;
        }

        EOS_LobbyModification_RemoveMemberAttributeOptions RemoveOpts = {};
        RemoveOpts.ApiVersion = EOS_LOBBYMODIFICATION_REMOVEMEMBERATTRIBUTE_API_LATEST;
        RemoveOpts.Key = MetadataKey.Ptr.Get();

        EOS_EResult AttrResult = EOS_LobbyModification_RemoveMemberAttribute(ModHandle, &RemoveOpts);
        if (AttrResult != EOS_EResult::EOS_Success)
        {
            OnComplete.ExecuteIfBound(
                ConvertError(
                    TEXT("EOS_LobbyModification_RemoveMemberAttribute"),
                    FString::Printf(TEXT("Unable to remove attribute '%s'"), *Metadata),
                    AttrResult),
                *EOSUser);
            return true;
        }
    }

    EOS_Lobby_UpdateLobbyOptions UpdateOpts = {};
    UpdateOpts.ApiVersion = EOS_LOBBY_UPDATELOBBY_API_LATEST;
    UpdateOpts.LobbyModificationHandle = ModHandle;

    EOSRunOperation<EOS_HLobby, EOS_Lobby_UpdateLobbyOptions, EOS_Lobby_UpdateLobbyCallbackInfo>(
        this->EOSLobby,
        &UpdateOpts,
        &EOS_Lobby_UpdateLobby,
        [WeakThis = GetWeakThis(this), ModHandle, EOSUser, OnComplete](const EOS_Lobby_UpdateLobbyCallbackInfo *Info) {
            EOS_LobbyModification_Release(ModHandle);

            if (auto This = PinWeakThis(WeakThis))
            {
                if (Info->ResultCode != EOS_EResult::EOS_Success && Info->ResultCode != EOS_EResult::EOS_NoChange)
                {
                    OnComplete.ExecuteIfBound(
                        ConvertError(TEXT("EOS_Lobby_UpdateLobby"), TEXT("Unable to update lobby"), Info->ResultCode),
                        *EOSUser);
                    return;
                }

                OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::Success(), *EOSUser);
            }
        });
    return true;
}

bool FOnlineLobbyInterfaceEOS::GetMemberCount(
    const FUniqueNetId &UserId,
    const FOnlineLobbyId &LobbyId,
    int32 &OutMemberCount)
{
    if (UserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        OutMemberCount = 0;
        return false;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared());
    if (!EOSUser->HasValidProductUserId())
    {
        OutMemberCount = 0;
        return false;
    }

    auto EOSLobbyId = StaticCastSharedRef<const FOnlinePartyIdEOS>(LobbyId.AsShared());

    EOS_HLobbyDetails LobbyDetails = nullptr;
    bool bShouldRelease = false;
    auto Result = CopyLobbyDetailsHandle(*EOSUser, *EOSLobbyId, LobbyDetails, bShouldRelease);
    if (Result == EOS_EResult::EOS_Success)
    {
        EOS_LobbyDetails_GetMemberCountOptions CountOpts = {};
        CountOpts.ApiVersion = EOS_LOBBYDETAILS_GETMEMBERCOUNT_API_LATEST;
        OutMemberCount = EOS_LobbyDetails_GetMemberCount(LobbyDetails, &CountOpts);
    }
    else
    {
        OutMemberCount = 0;
    }
    if (LobbyDetails != nullptr)
    {
        if (bShouldRelease)
        {
            EOS_LobbyDetails_Release(LobbyDetails);
        }
        LobbyDetails = nullptr;
    }
    return Result == EOS_EResult::EOS_Success;
}

bool FOnlineLobbyInterfaceEOS::GetMemberUserId(
    const FUniqueNetId &UserId,
    const FOnlineLobbyId &LobbyId,
    int32 MemberIndex,
    TSharedPtr<const FUniqueNetId> &OutMemberId)
{
    if (UserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        return false;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared());
    if (!EOSUser->HasValidProductUserId())
    {
        return false;
    }

    auto EOSLobbyId = StaticCastSharedRef<const FOnlinePartyIdEOS>(LobbyId.AsShared());

    EOS_HLobbyDetails LobbyDetails = nullptr;
    bool bShouldRelease = false;
    auto Result = CopyLobbyDetailsHandle(*EOSUser, *EOSLobbyId, LobbyDetails, bShouldRelease);
    if (Result == EOS_EResult::EOS_Success)
    {
        EOS_LobbyDetails_GetMemberByIndexOptions MemOpts = {};
        MemOpts.ApiVersion = EOS_LOBBYDETAILS_GETMEMBERBYINDEX_API_LATEST;
        MemOpts.MemberIndex = MemberIndex;
        EOS_ProductUserId PUID = EOS_LobbyDetails_GetMemberByIndex(LobbyDetails, &MemOpts);
        if (EOS_ProductUserId_IsValid(PUID))
        {
            OutMemberId = MakeShared<FUniqueNetIdEOS>(PUID);
            return true;
        }
    }
    if (LobbyDetails != nullptr)
    {
        if (bShouldRelease)
        {
            EOS_LobbyDetails_Release(LobbyDetails);
        }
        LobbyDetails = nullptr;
    }
    return false;
}

bool FOnlineLobbyInterfaceEOS::GetMemberMetadataValue(
    const FUniqueNetId &UserId,
    const FOnlineLobbyId &LobbyId,
    const FUniqueNetId &MemberId,
    const FString &MetadataKey,
    FVariantData &OutMetadataValue)
{
    if (UserId.GetType() != REDPOINT_EOS_SUBSYSTEM || MemberId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        return false;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared());
    if (!EOSUser->HasValidProductUserId())
    {
        return false;
    }

    auto EOSMember = StaticCastSharedRef<const FUniqueNetIdEOS>(MemberId.AsShared());
    if (!EOSMember->HasValidProductUserId())
    {
        return false;
    }

    auto EOSLobbyId = StaticCastSharedRef<const FOnlinePartyIdEOS>(LobbyId.AsShared());

    bool DidSet = false;
    EOS_Lobby_Attribute *Attr = nullptr;
    EOS_HLobbyDetails LobbyDetails = nullptr;
    bool bShouldRelease = false;
    auto Result = CopyLobbyDetailsHandle(*EOSUser, *EOSLobbyId, LobbyDetails, bShouldRelease);
    if (Result == EOS_EResult::EOS_Success)
    {
        auto MetadataKeyAnsi = EOSString_LobbyModification_AttributeKey::ToAnsiString(MetadataKey);

        EOS_LobbyDetails_CopyMemberAttributeByKeyOptions AttrOpts = {};
        AttrOpts.ApiVersion = EOS_LOBBYDETAILS_COPYMEMBERATTRIBUTEBYKEY_API_LATEST;
        AttrOpts.AttrKey = MetadataKeyAnsi.Ptr.Get();
        AttrOpts.TargetUserId = EOSMember->GetProductUserId();

        if (EOS_LobbyDetails_CopyMemberAttributeByKey(LobbyDetails, &AttrOpts, &Attr) == EOS_EResult::EOS_Success)
        {
            switch (Attr->Data->ValueType)
            {
            case EOS_ELobbyAttributeType::EOS_AT_STRING:
                OutMetadataValue = FVariantData(FString(
                    EOSString_LobbyModification_AttributeStringValue::FromUtf8String(Attr->Data->Value.AsUtf8)));
                DidSet = true;
                break;
            case EOS_ELobbyAttributeType::EOS_AT_DOUBLE:
                OutMetadataValue = FVariantData(Attr->Data->Value.AsDouble);
                DidSet = true;
                break;
            case EOS_ELobbyAttributeType::EOS_AT_INT64: {
                int64 Int64Val = Attr->Data->Value.AsInt64;
                OutMetadataValue = FVariantData(Int64Val);
                DidSet = true;
                break;
            }
            case EOS_ELobbyAttributeType::EOS_AT_BOOLEAN:
                OutMetadataValue = FVariantData(Attr->Data->Value.AsBool == EOS_TRUE);
                DidSet = true;
                break;
            }
        }
    }
    if (Attr != nullptr)
    {
        EOS_Lobby_Attribute_Release(Attr);
        Attr = nullptr;
    }
    if (LobbyDetails != nullptr)
    {
        if (bShouldRelease)
        {
            EOS_LobbyDetails_Release(LobbyDetails);
        }
        LobbyDetails = nullptr;
    }
    return DidSet;
}

bool FOnlineLobbyInterfaceEOS::Search(
    const FUniqueNetId &UserId,
    const FOnlineLobbySearchQuery &Query,
    FOnLobbySearchComplete OnComplete)
{
    if (UserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        OnComplete.ExecuteIfBound(
            OnlineRedpointEOS::Errors::InvalidUser(),
            UserId,
            TArray<TSharedRef<const FOnlineLobbyId>>());
        return true;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared());
    if (!EOSUser->HasValidProductUserId())
    {
        OnComplete.ExecuteIfBound(
            OnlineRedpointEOS::Errors::InvalidUser(),
            UserId,
            TArray<TSharedRef<const FOnlineLobbyId>>());
        return true;
    }

    EOS_HLobbySearch LobbySearch = {};

    EOS_Lobby_CreateLobbySearchOptions CreateSearchOpts = {};
    CreateSearchOpts.ApiVersion = EOS_LOBBY_CREATELOBBYSEARCH_API_LATEST;
    CreateSearchOpts.MaxResults = Query.Limit.Get(100);
    EOS_EResult CreateSearchResult = EOS_Lobby_CreateLobbySearch(this->EOSLobby, &CreateSearchOpts, &LobbySearch);
    if (CreateSearchResult != EOS_EResult::EOS_Success)
    {
        OnComplete.ExecuteIfBound(
            ConvertError(
                TEXT("EOS_Lobby_CreateLobbySearch"),
                TEXT("Unable to create lobby search"),
                CreateSearchResult),
            UserId,
            TArray<TSharedRef<const FOnlineLobbyId>>());
        return true;
    }

    for (const FOnlineLobbySearchQueryFilter &Filter : Query.Filters)
    {
        auto MetadataKey = EOSString_LobbyModification_AttributeKey::ToAnsiString(Filter.Key);
        if (MetadataKey.Result != EOS_EResult::EOS_Success)
        {
            OnComplete.ExecuteIfBound(
                ConvertError(
                    TEXT("EOSString_LobbyModification_AttributeKey::ToAnsiString"),
                    FString::Printf(TEXT("The attribute key '%s' is not valid"), *Filter.Key),
                    MetadataKey.Result),
                *EOSUser,
                TArray<TSharedRef<const FOnlineLobbyId>>());
            return true;
        }

        auto MetadataValue = EOSString_LobbyModification_AttributeStringValue::ToUtf8String(Filter.Value.ToString());
        if (MetadataValue.Result != EOS_EResult::EOS_Success)
        {
            OnComplete.ExecuteIfBound(
                ConvertError(
                    TEXT("EOSString_LobbyModification_AttributeStringValue::ToUtf8String"),
                    FString::Printf(TEXT("The attribute key '%s' has an invalid value"), *Filter.Key),
                    MetadataValue.Result),
                *EOSUser,
                TArray<TSharedRef<const FOnlineLobbyId>>());
            return true;
        }

        EOS_LobbySearch_SetParameterOptions Opts = {};
        Opts.ApiVersion = EOS_LOBBYSEARCH_SETPARAMETER_API_LATEST;
        switch (Filter.Comparison)
        {
        case EOnlineLobbySearchQueryFilterComparator::NotEqual:
            Opts.ComparisonOp = EOS_EComparisonOp::EOS_CO_NOTEQUAL;
            break;
        case EOnlineLobbySearchQueryFilterComparator::Equal:
            Opts.ComparisonOp = EOS_EComparisonOp::EOS_CO_EQUAL;
            break;
        case EOnlineLobbySearchQueryFilterComparator::GreaterThan:
            Opts.ComparisonOp = EOS_EComparisonOp::EOS_CO_GREATERTHAN;
            break;
        case EOnlineLobbySearchQueryFilterComparator::GreaterThanOrEqual:
            Opts.ComparisonOp = EOS_EComparisonOp::EOS_CO_GREATERTHANOREQUAL;
            break;
        case EOnlineLobbySearchQueryFilterComparator::LessThan:
            Opts.ComparisonOp = EOS_EComparisonOp::EOS_CO_LESSTHAN;
            break;
        case EOnlineLobbySearchQueryFilterComparator::LessThanOrEqual:
            Opts.ComparisonOp = EOS_EComparisonOp::EOS_CO_LESSTHANOREQUAL;
            break;
        case EOnlineLobbySearchQueryFilterComparator::Distance:
            Opts.ComparisonOp = EOS_EComparisonOp::EOS_CO_DISTANCE;
            break;
        }

        EOS_Lobby_AttributeData Attr = {};
        Attr.ApiVersion = EOS_LOBBY_ATTRIBUTEDATA_API_LATEST;
        Attr.Key = MetadataKey.Ptr.Get();
        switch (Filter.Value.GetType())
        {
        case EOnlineKeyValuePairDataType::Bool:
            Attr.ValueType = EOS_ELobbyAttributeType::EOS_AT_BOOLEAN;
            bool BoolVal;
            Filter.Value.GetValue(BoolVal);
            Attr.Value.AsBool = BoolVal ? EOS_TRUE : EOS_FALSE;
            break;
        case EOnlineKeyValuePairDataType::Int64:
            Attr.ValueType = EOS_ELobbyAttributeType::EOS_AT_INT64;
            int64 Int64Val;
            Filter.Value.GetValue(Int64Val);
            Attr.Value.AsInt64 = Int64Val;
            break;
        case EOnlineKeyValuePairDataType::Double:
            Attr.ValueType = EOS_ELobbyAttributeType::EOS_AT_DOUBLE;
            Filter.Value.GetValue(Attr.Value.AsDouble);
            break;
        case EOnlineKeyValuePairDataType::String:
            Attr.ValueType = EOS_ELobbyAttributeType::EOS_AT_STRING;
            Attr.Value.AsUtf8 = MetadataValue.GetAsChar();
            break;
        default:
            OnComplete.ExecuteIfBound(
                ConvertError(
                    TEXT("EOS_Lobby_AttributeData.ValueType"),
                    FString::Printf(TEXT("The attribute '%s' has an unsupported type of value"), *Filter.Key),
                    EOS_EResult::EOS_InvalidParameters),
                *EOSUser,
                TArray<TSharedRef<const FOnlineLobbyId>>());
            return true;
        }
        Opts.Parameter = &Attr;

        auto ParameterResult = EOS_LobbySearch_SetParameter(LobbySearch, &Opts);
        if (ParameterResult != EOS_EResult::EOS_Success)
        {
            OnComplete.ExecuteIfBound(
                ConvertError(
                    TEXT("EOS_LobbySearch_SetParameter"),
                    FString::Printf(TEXT("The parameter could not be set '%s' has an invalid value"), *Filter.Key),
                    ParameterResult),
                *EOSUser,
                TArray<TSharedRef<const FOnlineLobbyId>>());
            return true;
        }
    }

    EOS_LobbySearch_FindOptions FindOpts = {};
    FindOpts.ApiVersion = EOS_LOBBYSEARCH_FIND_API_LATEST;
    FindOpts.LocalUserId = EOSUser->GetProductUserId();
    EOSRunOperation<EOS_HLobbySearch, EOS_LobbySearch_FindOptions, EOS_LobbySearch_FindCallbackInfo>(
        LobbySearch,
        &FindOpts,
        &EOS_LobbySearch_Find,
        [WeakThis = GetWeakThis(this), LobbySearch, EOSUser, OnComplete](const EOS_LobbySearch_FindCallbackInfo *Info) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Info->ResultCode != EOS_EResult::EOS_Success)
                {
                    EOS_LobbySearch_Release(LobbySearch);
                    OnComplete.ExecuteIfBound(
                        ConvertError(
                            TEXT("EOS_LobbySearch_Find"),
                            TEXT("Unable to search for lobbies"),
                            Info->ResultCode),
                        *EOSUser,
                        TArray<TSharedRef<const FOnlineLobbyId>>());
                    return;
                }

                EOS_LobbySearch_GetSearchResultCountOptions CountOpts = {};
                CountOpts.ApiVersion = EOS_LOBBYSEARCH_GETSEARCHRESULTCOUNT_API_LATEST;
                uint32_t Count = EOS_LobbySearch_GetSearchResultCount(LobbySearch, &CountOpts);

                TArray<TSharedRef<const FOnlineLobbyId>> Results;

                if (!This->LastSearchResultsByUser.Contains(*EOSUser))
                {
                    This->LastSearchResultsByUser.Add(*EOSUser, TMap<FString, EOS_HLobbyDetails>());
                }
                auto &LastSearchResults = This->LastSearchResultsByUser[*EOSUser];
                for (const auto &KV : LastSearchResults)
                {
                    EOS_LobbyDetails_Release(KV.Value);
                }
                LastSearchResults.Empty();

                for (uint32_t i = 0; i < Count; i++)
                {
                    EOS_HLobbyDetails Details = nullptr;
                    bool bKeep = false;

                    EOS_LobbySearch_CopySearchResultByIndexOptions CopyOpts = {};
                    CopyOpts.ApiVersion = EOS_LOBBYSEARCH_COPYSEARCHRESULTBYINDEX_API_LATEST;
                    CopyOpts.LobbyIndex = i;
                    if (EOS_LobbySearch_CopySearchResultByIndex(LobbySearch, &CopyOpts, &Details) ==
                        EOS_EResult::EOS_Success)
                    {
                        EOS_LobbyDetails_Info *LobbyInfo = nullptr;

                        EOS_LobbyDetails_CopyInfoOptions CopyInfoOpts = {};
                        CopyInfoOpts.ApiVersion = EOS_LOBBYDETAILS_COPYINFO_API_LATEST;
                        if (EOS_LobbyDetails_CopyInfo(Details, &CopyInfoOpts, &LobbyInfo) == EOS_EResult::EOS_Success)
                        {
                            auto LobbyId = MakeShared<FOnlinePartyIdEOS>(LobbyInfo->LobbyId);
                            Results.Add(LobbyId);
                            LastSearchResults.Add(LobbyId->ToString(), Details);
                            bKeep = true;

#if defined(EOS_ENABLE_STATE_DIAGNOSTICS) && EOS_ENABLE_STATE_DIAGNOSTICS
                            This->DumpLobbyAttributeState(EOSUser->GetProductUserId(), LobbyInfo->LobbyId, Details);
#endif
                        }

                        if (Info != nullptr)
                        {
                            EOS_LobbyDetails_Info_Release(LobbyInfo);
                        }
                    }
                    if (Details != nullptr && !bKeep)
                    {
                        EOS_LobbyDetails_Release(Details);
                    }
                }

                EOS_LobbySearch_Release(LobbySearch);
                OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::Success(), *EOSUser, Results);
            }
            else
            {
                EOS_LobbySearch_Release(LobbySearch);
            }
        });
    return true;
}

EOS_EResult FOnlineLobbyInterfaceEOS::CopyLobbyDetailsHandle(
    const FUniqueNetId &UserId,
    const FOnlineLobbyId &LobbyId,
    EOS_HLobbyDetails &LobbyDetails,
    bool &bShouldRelease)
{
    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared());
    auto EOSLobbyId = StaticCastSharedRef<const FOnlinePartyIdEOS>(LobbyId.AsShared());

    EOS_Lobby_CopyLobbyDetailsHandleOptions CopyOpts = {};
    CopyOpts.ApiVersion = EOS_LOBBY_COPYLOBBYDETAILSHANDLE_API_LATEST;
    CopyOpts.LobbyId = EOSLobbyId->GetLobbyId();
    CopyOpts.LocalUserId = EOSUser->GetProductUserId();
    EOS_EResult LiveCopyResult = EOS_Lobby_CopyLobbyDetailsHandle(this->EOSLobby, &CopyOpts, &LobbyDetails);
    if (LiveCopyResult == EOS_EResult::EOS_Success)
    {
        bShouldRelease = true;
        return LiveCopyResult;
    }

    for (const auto &KV : LastSearchResultsByUser)
    {
        if (KV.Value.Contains(LobbyId.ToString()))
        {
            LobbyDetails = KV.Value[LobbyId.ToString()];
            bShouldRelease = false;
            return EOS_EResult::EOS_Success;
        }
    }

    bShouldRelease = false;
    return EOS_EResult::EOS_NotFound;
}

bool FOnlineLobbyInterfaceEOS::GetLobbyMetadataValue(
    const FUniqueNetId &UserId,
    const FOnlineLobbyId &LobbyId,
    const FString &MetadataKey,
    FVariantData &OutMetadataValue)
{
    if (UserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        return false;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared());
    if (!EOSUser->HasValidProductUserId())
    {
        return false;
    }

    auto EOSLobbyId = StaticCastSharedRef<const FOnlinePartyIdEOS>(LobbyId.AsShared());

    bool DidSet = false;
    EOS_Lobby_Attribute *Attr = nullptr;
    EOS_HLobbyDetails LobbyDetails = nullptr;
    bool bShouldRelease = false;
    auto Result = CopyLobbyDetailsHandle(*EOSUser, *EOSLobbyId, LobbyDetails, bShouldRelease);
    if (Result == EOS_EResult::EOS_Success)
    {
        auto MetadataKeyAnsi = EOSString_LobbyModification_AttributeKey::ToAnsiString(MetadataKey);

        EOS_LobbyDetails_CopyAttributeByKeyOptions AttrOpts = {};
        AttrOpts.ApiVersion = EOS_LOBBYDETAILS_COPYMEMBERATTRIBUTEBYKEY_API_LATEST;
        AttrOpts.AttrKey = MetadataKeyAnsi.Ptr.Get();

        if (EOS_LobbyDetails_CopyAttributeByKey(LobbyDetails, &AttrOpts, &Attr) == EOS_EResult::EOS_Success)
        {
            switch (Attr->Data->ValueType)
            {
            case EOS_ELobbyAttributeType::EOS_AT_STRING:
                OutMetadataValue = FVariantData(FString(
                    EOSString_LobbyModification_AttributeStringValue::FromUtf8String(Attr->Data->Value.AsUtf8)));
                DidSet = true;
                break;
            case EOS_ELobbyAttributeType::EOS_AT_DOUBLE:
                OutMetadataValue = FVariantData(Attr->Data->Value.AsDouble);
                DidSet = true;
                break;
            case EOS_ELobbyAttributeType::EOS_AT_INT64: {
                int64 Int64Val = Attr->Data->Value.AsInt64;
                OutMetadataValue = FVariantData(Int64Val);
                DidSet = true;
                break;
            }
            case EOS_ELobbyAttributeType::EOS_AT_BOOLEAN:
                OutMetadataValue = FVariantData(Attr->Data->Value.AsBool == EOS_TRUE);
                DidSet = true;
                break;
            }
        }
    }
    if (Attr != nullptr)
    {
        EOS_Lobby_Attribute_Release(Attr);
        Attr = nullptr;
    }
    if (LobbyDetails != nullptr)
    {
        if (bShouldRelease)
        {
            EOS_LobbyDetails_Release(LobbyDetails);
        }
        LobbyDetails = nullptr;
    }
    return DidSet;
}

TSharedPtr<FOnlineLobbyId> FOnlineLobbyInterfaceEOS::ParseSerializedLobbyId(const FString &InLobbyId)
{
    return MakeShared<FOnlinePartyIdEOS>(TCHAR_TO_ANSI(*InLobbyId));
}

bool FOnlineLobbyInterfaceEOS::KickMember(
    const FUniqueNetId &UserId,
    const FOnlineLobbyId &LobbyId,
    const FUniqueNetId &MemberId,
    FOnLobbyOperationComplete OnComplete)
{
    if (UserId.GetType() != REDPOINT_EOS_SUBSYSTEM || MemberId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        OnComplete.ExecuteIfBound(
            OnlineRedpointEOS::Errors::InvalidUser(
                UserId,
                LobbyId.ToString(),
                TEXT("KickMember"),
                TEXT("Local user isn't an EOS user")),
            UserId);
        return true;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared());
    if (!EOSUser->HasValidProductUserId())
    {
        OnComplete.ExecuteIfBound(
            OnlineRedpointEOS::Errors::InvalidUser(
                UserId,
                LobbyId.ToString(),
                TEXT("KickMember"),
                TEXT("Local user doesn't have a valid product user ID")),
            UserId);
        return true;
    }

    auto EOSMember = StaticCastSharedRef<const FUniqueNetIdEOS>(MemberId.AsShared());
    if (!EOSMember->HasValidProductUserId())
    {
        OnComplete.ExecuteIfBound(
            OnlineRedpointEOS::Errors::InvalidUser(
                UserId,
                LobbyId.ToString(),
                TEXT("KickMember"),
                FString::Printf(TEXT("Target member %s doesn't have a valid product user ID"), *MemberId.ToString())),
            UserId);
        return true;
    }

    auto EOSLobbyId = StaticCastSharedRef<const FOnlinePartyIdEOS>(LobbyId.AsShared());

    EOS_Lobby_KickMemberOptions KickOpts = {};
    KickOpts.ApiVersion = EOS_LOBBY_KICKMEMBER_API_LATEST;
    KickOpts.LobbyId = EOSLobbyId->GetLobbyId();
    KickOpts.LocalUserId = EOSUser->GetProductUserId();
    KickOpts.TargetUserId = EOSMember->GetProductUserId();
    EOSRunOperation<EOS_HLobby, EOS_Lobby_KickMemberOptions, EOS_Lobby_KickMemberCallbackInfo>(
        this->EOSLobby,
        &KickOpts,
        &EOS_Lobby_KickMember,
        [WeakThis = GetWeakThis(this), OnComplete, EOSUser](const EOS_Lobby_KickMemberCallbackInfo *Info) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Info->ResultCode == EOS_EResult::EOS_Success)
                {
                    OnComplete.ExecuteIfBound(FOnlineError::Success(), *EOSUser);
                }
                else
                {
                    OnComplete.ExecuteIfBound(ConvertError(Info->ResultCode), *EOSUser);
                };
            }
        });
    return true;
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
