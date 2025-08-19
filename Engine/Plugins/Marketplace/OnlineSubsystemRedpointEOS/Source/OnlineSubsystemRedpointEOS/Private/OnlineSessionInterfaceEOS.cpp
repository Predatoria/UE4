// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/OnlineSessionInterfaceEOS.h"

#include "Engine/Engine.h"
#include "OnlineBeaconHost.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/IInternetAddrEOS.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/ISocketSubsystemEOS.h"
#include "OnlineSubsystemRedpointEOS/Private/Orchestrator/IServerOrchestrator.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/CrossPlatformAccountId.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGamesCrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSErrorConv.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSListenTracker.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSNetDriver.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineFriendSynthetic.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineIdentityInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/SyntheticPartySessionManager.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOSSession.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "SocketSubsystem.h"

EOS_ENABLE_STRICT_WARNINGS

#define EOS_WELL_KNOWN_ATTRIBUTE_ADDRESS_BOUND "AddressBound"
#define EOS_WELL_KNOWN_ATTRIBUTE_ADDRESS_DEV "AddressDev"
#define EOS_WELL_KNOWN_ATTRIBUTE_BUSESPRESENCE "__EOS_bUsesPresence"
#define EOS_WELL_KNOWN_ATTRIBUTE_BLISTENING "__EOS_bListening"
#define EOS_WELL_KNOWN_ATTRIBUTE_OVERRIDEADDRESSBOUND "__EOS_OverrideAddressBound"

class FOnlineSessionInfoEOS : public FOnlineSessionInfo
{
private:
    TSharedPtr<FUniqueNetIdEOSSession> Id;
    EOS_HSessionDetails Handle;
    EOS_SessionDetails_Info *Info;

public:
    UE_NONCOPYABLE(FOnlineSessionInfoEOS);
    FOnlineSessionInfoEOS(EOS_HSessionDetails InHandle, EOS_SessionDetails_Info *InInfo)
        : Handle(InHandle)
        , Info(InInfo)
    {
        check(this->Handle != nullptr);
        check(this->Info != nullptr);

        this->Id = MakeShared<FUniqueNetIdEOSSession>(this->Info->SessionId);
    }
    FOnlineSessionInfoEOS(const TSharedRef<FUniqueNetIdEOSSession> &InId)
        : Id(InId)
        , Handle(nullptr)
        , Info(nullptr)
    {
    }
    ~FOnlineSessionInfoEOS()
    {
        if (this->Handle != nullptr)
        {
            EOS_SessionDetails_Release(this->Handle);
        }
        if (this->Info != nullptr)
        {
            EOS_SessionDetails_Info_Release(this->Info);
        }
    }

    EOS_HSessionDetails GetHandle() const
    {
        return this->Handle;
    }

    bool GetResolvedConnectString(FString &OutConnectionString, FName PortType) const
    {
        if (this->Info == nullptr || this->Info->HostAddress == nullptr)
        {
            return false;
        }

        int OverridePortNumber = 0;
        if (PortType.IsEqual(NAME_GamePort))
        {
            // No port override, use the port specified in the URL.
        }
        else if (PortType.IsEqual(NAME_BeaconPort))
        {
            // Override to the port specified by config for AOnlineBeaconHost.
            OverridePortNumber = GetDefault<AOnlineBeaconHost>()->ListenPort;
        }
        else if (!PortType.ToString().IsEmpty() && PortType.ToString().IsNumeric())
        {
            // Override to the port specified in PortType.
            OverridePortNumber = FCString::Atoi(*PortType.ToString());
        }
        else
        {
            // Check to see if we have a string attribute on the session
            // that starts with port_<name>.
            FString AttributeName = FString::Printf(TEXT("Port_%s"), *PortType.ToString());
            auto AttributeNameStr = EOSString_SessionModification_AttributeKey::ToAnsiString(AttributeName);
            EOS_SessionDetails_Attribute *OverrideAttribute = nullptr;
            EOS_SessionDetails_CopySessionAttributeByKeyOptions OverrideCopyOpts = {};
            OverrideCopyOpts.ApiVersion = EOS_SESSIONDETAILS_COPYSESSIONATTRIBUTEBYKEY_API_LATEST;
            OverrideCopyOpts.AttrKey = AttributeNameStr.Ptr.Get();
            if (EOS_SessionDetails_CopySessionAttributeByKey(this->Handle, &OverrideCopyOpts, &OverrideAttribute) !=
                    EOS_EResult::EOS_Success ||
                OverrideAttribute == nullptr || OverrideAttribute->Data == nullptr ||
                OverrideAttribute->Data->ValueType != EOS_ESessionAttributeType::EOS_AT_STRING)
            {
                // Could not get port override by attribute.
                UE_LOG(
                    LogEOS,
                    Warning,
                    TEXT("GetResolvedConnectString requested named port '%s', but there is no session attribute called "
                         "'%s' with a string value."),
                    *PortType.ToString(),
                    *AttributeName);
                return false;
            }
            OverridePortNumber = FCString::Atoi(*EOSString_SessionModification_AttributeStringValue::FromUtf8String(
                OverrideAttribute->Data->Value.AsUtf8));
            if (OverridePortNumber == 0)
            {
                UE_LOG(
                    LogEOS,
                    Warning,
                    TEXT("GetResolvedConnectString requested named port '%s', but the session attribute called "
                         "'%s' did not have a valid value."),
                    *PortType.ToString(),
                    *AttributeName);
                return false;
            }
        }

        FURL OldURL;
        FURL URL(&OldURL, ANSI_TO_TCHAR(this->Info->HostAddress), ETravelType::TRAVEL_Absolute);
        FString Address = FString(ANSI_TO_TCHAR(this->Info->HostAddress));
        if (URL.Host.EndsWith(INTERNET_ADDR_EOS_P2P_DOMAIN_SUFFIX))
        {
            if (OverridePortNumber == 0)
            {
                // This is a P2P address, so it's already complete.
                OutConnectionString = Address;
            }
            else
            {
                // We need to modify the server port component of the P2P address.
                URL.Port = OverridePortNumber;
                OutConnectionString = URL.GetHostPortString();
                return true;
            }
            return true;
        }

        // Get the AddressUnreal attribute, which contains port information and the connection string.
        EOS_SessionDetails_Attribute *Attribute = nullptr;
        EOS_SessionDetails_CopySessionAttributeByKeyOptions CopyOpts = {};
        CopyOpts.ApiVersion = EOS_SESSIONDETAILS_COPYSESSIONATTRIBUTEBYKEY_API_LATEST;
        CopyOpts.AttrKey = EOS_WELL_KNOWN_ATTRIBUTE_ADDRESS_BOUND;
        if (EOS_SessionDetails_CopySessionAttributeByKey(this->Handle, &CopyOpts, &Attribute) !=
                EOS_EResult::EOS_Success ||
            Attribute->Data == nullptr || Attribute->Data->ValueType != EOS_ESessionAttributeType::EOS_AT_STRING)
        {
            // Could not get any additional information, return the HostAddress field as-is.
            OutConnectionString = Address;
            return true;
        }
        FString PortURLRaw = UTF8_TO_TCHAR(Attribute->Data->Value.AsUtf8);
        EOS_SessionDetails_Attribute_Release(Attribute);

        // Merge the EOS provided IP address and the port from AddressUnreal.
        TSharedPtr<FInternetAddr> PortURLAddr = ISocketSubsystem::Get()->CreateInternetAddr();
        TSharedPtr<FInternetAddr> HostURLAddr = ISocketSubsystem::Get()->CreateInternetAddr();
        bool bPortValid, bHostValid;
        PortURLAddr->SetIp(*PortURLRaw, bPortValid);
        HostURLAddr->SetIp(*Address, bHostValid);
        int ActivePortNumber = PortURLAddr->GetPort();
        if (OverridePortNumber != 0)
        {
            ActivePortNumber = OverridePortNumber;
        }
        HostURLAddr->SetPort(ActivePortNumber);

        OutConnectionString = HostURLAddr->ToString(true);

#if defined(EOS_SUPPORT_MULTI_IP_RESOLUTION)
        if (!FParse::Param(FCommandLine::Get(), TEXT("emulateeosshipping")))
        {
            // We support multiple IP addresses in our resolution because we're running a development build.
            // Lookup the dev addresses attribute, and append it onto our host URL.
            CopyOpts.AttrKey = EOS_WELL_KNOWN_ATTRIBUTE_ADDRESS_DEV;
            if (EOS_SessionDetails_CopySessionAttributeByKey(this->Handle, &CopyOpts, &Attribute) !=
                    EOS_EResult::EOS_Success ||
                Attribute->Data == nullptr || Attribute->Data->ValueType != EOS_ESessionAttributeType::EOS_AT_STRING)
            {
                // Could not get development addresses. Return what we've already got.
                return true;
            }
            FString DevelopmentAddresses = UTF8_TO_TCHAR(Attribute->Data->Value.AsUtf8);
            EOS_SessionDetails_Attribute_Release(Attribute);

            if (DevelopmentAddresses.Len() > 0)
            {
                OutConnectionString = FString::Printf(
                    TEXT("%s,%s:%d"),
                    *HostURLAddr->ToString(false),
                    *DevelopmentAddresses,
                    ActivePortNumber);
            }
        }
#endif

        return true;
    }

    virtual const FUniqueNetId &GetSessionId() const override
    {
        return this->Id.ToSharedRef().Get();
    }

    virtual const uint8 *GetBytes() const override
    {
        return this->Id->GetBytes();
    }

    virtual int32 GetSize() const override
    {
        return this->Id->GetSize();
    }

    virtual bool IsValid() const override
    {
        return this->Id->IsValid();
    }

    virtual FString ToString() const override
    {
        return this->Id->ToString();
    }

    virtual FString ToDebugString() const override
    {
        return this->Id->ToDebugString();
    }
};

class FOnlineSessionSearchResultEOS : public FOnlineSessionSearchResult
{
public:
    static FOnlineSessionSearchResultEOS CreateInvalid()
    {
        return FOnlineSessionSearchResultEOS();
    }

    static FOnlineSessionSearchResultEOS CreateFromDetails(EOS_HSessionDetails InHandle)
    {
        check(InHandle != nullptr);

        EOS_SessionDetails_CopyInfoOptions CopyInfoOpts = {};
        CopyInfoOpts.ApiVersion = EOS_SESSIONDETAILS_COPYINFO_API_LATEST;

        EOS_SessionDetails_Info *SessionDetailsInfo = nullptr;
        if (EOS_SessionDetails_CopyInfo(InHandle, &CopyInfoOpts, &SessionDetailsInfo) != EOS_EResult::EOS_Success)
        {
            EOS_SessionDetails_Release(InHandle);
            return FOnlineSessionSearchResultEOS();
        }

        check(SessionDetailsInfo != nullptr);
        check(SessionDetailsInfo->Settings != nullptr);

        // Read bUsesPresence from attribute data if possible.
        bool bUsesPresence = false;
        {
            EOS_SessionDetails_Attribute *PresenceAttribute = nullptr;
            EOS_SessionDetails_CopySessionAttributeByKeyOptions CopyByKeyOpts = {};
            CopyByKeyOpts.ApiVersion = EOS_SESSIONDETAILS_COPYSESSIONATTRIBUTEBYKEY_API_LATEST;
            CopyByKeyOpts.AttrKey = EOS_WELL_KNOWN_ATTRIBUTE_BUSESPRESENCE;
            if (EOS_SessionDetails_CopySessionAttributeByKey(InHandle, &CopyByKeyOpts, &PresenceAttribute) ==
                EOS_EResult::EOS_Success)
            {
                if (PresenceAttribute->Data != nullptr &&
                    PresenceAttribute->Data->ValueType == EOS_ESessionAttributeType::EOS_AT_BOOLEAN)
                {
                    bUsesPresence = PresenceAttribute->Data->Value.AsBool == EOS_TRUE;
                }
            }
            if (PresenceAttribute != nullptr)
            {
                EOS_SessionDetails_Attribute_Release(PresenceAttribute);
                PresenceAttribute = nullptr;
            }
        }

        FOnlineSessionSettings Settings;
        Settings.NumPublicConnections = SessionDetailsInfo->Settings->NumPublicConnections;
        Settings.NumPrivateConnections = 0;
        Settings.bShouldAdvertise = SessionDetailsInfo->Settings->PermissionLevel ==
                                    EOS_EOnlineSessionPermissionLevel::EOS_OSPF_PublicAdvertised;
        Settings.bAllowJoinInProgress = SessionDetailsInfo->Settings->bAllowJoinInProgress == EOS_TRUE;
        Settings.bIsLANMatch = false;
        Settings.bIsDedicated =
            !EOSString_SessionModification_HostAddress::FromAnsiString(SessionDetailsInfo->HostAddress)
                 .EndsWith(FString::Printf(TEXT(".%s"), INTERNET_ADDR_EOS_P2P_DOMAIN_SUFFIX));
        Settings.bUsesStats = false;
        Settings.bAllowInvites = SessionDetailsInfo->Settings->bInvitesAllowed == EOS_TRUE;
        Settings.bUsesPresence = bUsesPresence;
        Settings.bAllowJoinViaPresence = SessionDetailsInfo->Settings->PermissionLevel ==
                                             EOS_EOnlineSessionPermissionLevel::EOS_OSPF_JoinViaPresence ||
                                         SessionDetailsInfo->Settings->PermissionLevel ==
                                             EOS_EOnlineSessionPermissionLevel::EOS_OSPF_PublicAdvertised;
        Settings.bAllowJoinViaPresenceFriendsOnly = SessionDetailsInfo->Settings->PermissionLevel ==
                                                    EOS_EOnlineSessionPermissionLevel::EOS_OSPF_JoinViaPresence;
        Settings.bAntiCheatProtected = false;
        Settings.BuildUniqueId = 0; // @todo: build ID filtering

        EOS_SessionDetails_GetSessionAttributeCountOptions CountOpts = {};
        CountOpts.ApiVersion = EOS_SESSIONDETAILS_GETSESSIONATTRIBUTECOUNT_API_LATEST;
        uint32_t AttributeCount = EOS_SessionDetails_GetSessionAttributeCount(InHandle, &CountOpts);
        for (uint32_t i = 0; i < AttributeCount; i++)
        {
            EOS_SessionDetails_CopySessionAttributeByIndexOptions CopyOpts = {};
            CopyOpts.ApiVersion = EOS_SESSIONDETAILS_COPYSESSIONATTRIBUTEBYINDEX_API_LATEST;
            CopyOpts.AttrIndex = i;

            EOS_SessionDetails_Attribute *Attribute = nullptr;
            if (EOS_SessionDetails_CopySessionAttributeByIndex(InHandle, &CopyOpts, &Attribute) !=
                EOS_EResult::EOS_Success)
            {
                UE_LOG(LogEOS, Warning, TEXT("Unable to copy session attribute at index %u"), i);
                continue;
            }

            FName Key = FName(ANSI_TO_TCHAR(Attribute->Data->Key));
            EOnlineDataAdvertisementType::Type AdvertType =
                Attribute->AdvertisementType == EOS_ESessionAttributeAdvertisementType::EOS_SAAT_Advertise
                    ? EOnlineDataAdvertisementType::ViaOnlineService
                    : EOnlineDataAdvertisementType::DontAdvertise;
            FOnlineSessionSetting Value;
            bool ValueValid = true;
            switch (Attribute->Data->ValueType)
            {
            case EOS_ESessionAttributeType::EOS_AT_BOOLEAN:
                Value = FOnlineSessionSetting(Attribute->Data->Value.AsBool == EOS_TRUE, AdvertType);
                break;
            case EOS_ESessionAttributeType::EOS_AT_DOUBLE:
                Value = FOnlineSessionSetting(Attribute->Data->Value.AsDouble, AdvertType);
                break;
            case EOS_ESessionAttributeType::EOS_AT_INT64:
                Value = FOnlineSessionSetting((int64)Attribute->Data->Value.AsInt64, AdvertType);
                break;
            case EOS_ESessionAttributeType::EOS_AT_STRING:
                Value = FOnlineSessionSetting(FString(UTF8_TO_TCHAR(Attribute->Data->Value.AsUtf8)), AdvertType);
                break;
            default:
                UE_LOG(LogEOS, Warning, TEXT("Unable to copy session attribute at index %u"), i);
                ValueValid = false;
                break;
            }
            if (!ValueValid)
            {
                EOS_SessionDetails_Attribute_Release(Attribute);
                continue;
            }

            Settings.Settings.Add(Key, Value);
            EOS_SessionDetails_Attribute_Release(Attribute);
        }

        FOnlineSessionSearchResultEOS Result;
        // EOS does not provide "owning users" for sessions, but it has to be non-null for
        // the session result to be valid. So we just always set it to the dedicated server ID.
        Result.Session.OwningUserId = FUniqueNetIdEOS::DedicatedServerId();
        Result.Session.OwningUserName = 0;
        Result.Session.SessionSettings = Settings;
        Result.Session.SessionInfo = MakeShared<FOnlineSessionInfoEOS>(InHandle, SessionDetailsInfo);
        Result.Session.NumOpenPrivateConnections = 0;
        Result.Session.NumOpenPublicConnections = SessionDetailsInfo->NumOpenPublicConnections;
        Result.PingInMs = 0;
        return Result;
    }
};

FOnlineSessionInterfaceEOS::FOnlineSessionInterfaceEOS(
    EOS_HPlatform InPlatform,
    TSharedPtr<FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> InIdentity,
    const TSharedPtr<IOnlineFriends, ESPMode::ThreadSafe> &InFriends,
#if EOS_HAS_ORCHESTRATORS
    const TSharedPtr<class IServerOrchestrator> &InServerOrchestrator,
#endif // #if EOS_HAS_ORCHESTRATORS
    const TSharedRef<class FEOSConfig> &InConfig)
    : EOSSessions(EOS_Platform_GetSessionsInterface(InPlatform))
    , EOSMetrics(EOS_Platform_GetMetricsInterface(InPlatform))
    , EOSUI(EOS_Platform_GetUIInterface(InPlatform))
    , Identity(MoveTemp(InIdentity))
#if EOS_HAS_AUTHENTICATION
    , Friends(InFriends)
    , SyntheticPartySessionManager(nullptr)
#endif // #if EOS_HAS_AUTHENTICATION
#if EOS_HAS_ORCHESTRATORS
    , ServerOrchestrator(InServerOrchestrator)
#endif // #if EOS_HAS_ORCHESTRATORS
    , Config(InConfig)
    , SessionLock()
    , Sessions()
    , ListenTracker(MakeShared<FEOSListenTracker>())
    , Unregister_JoinSessionAccepted()
    , Unregister_SessionInviteAccepted()
    , Unregister_SessionInviteReceived()
{
    check(this->EOSSessions != nullptr);
    check(this->EOSMetrics != nullptr);
    check(this->EOSUI != nullptr);
};

void FOnlineSessionInterfaceEOS::Handle_SessionAddressChanged(
    EOS_ProductUserId ProductUserId,
    const TSharedRef<const FInternetAddr> &InternetAddr,
    const TArray<TSharedPtr<FInternetAddr>> &DeveloperInternetAddrs)
{
    auto TargetUserIdEOS =
        ProductUserId == nullptr ? FUniqueNetIdEOS::DedicatedServerId() : MakeShared<FUniqueNetIdEOS>(ProductUserId);

    FScopeLock ScopeLock(&this->SessionLock);
    for (int32 SearchIndex = 0; SearchIndex < this->Sessions.Num(); SearchIndex++)
    {
        auto OwnerUserId = this->Sessions[SearchIndex].OwningUserId;
        if (!OwnerUserId.IsValid() || !OwnerUserId->GetType().IsEqual(REDPOINT_EOS_SUBSYSTEM))
        {
            continue;
        }

        if (*StaticCastSharedPtr<const FUniqueNetIdEOS>(OwnerUserId) != *TargetUserIdEOS)
        {
            continue;
        }

        // We found a session that is impacted by the address change, perform an UpdateSession operation
        // to set it's HostAddress.

        auto SessionNameAnsi =
            EOSString_SessionModification_SessionName::ToAnsiString(this->Sessions[SearchIndex].SessionName.ToString());

        EOS_Sessions_UpdateSessionModificationOptions ModOpts = {};
        ModOpts.ApiVersion = EOS_SESSIONS_UPDATESESSIONMODIFICATION_API_LATEST;
        ModOpts.SessionName = SessionNameAnsi.Ptr.Get();

        EOS_HSessionModification ModHandle = {};
        if (EOS_Sessions_UpdateSessionModification(this->EOSSessions, &ModOpts, &ModHandle) != EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("Handle_SessionAddressChanged: Could not create session modification handle to update address"));
            continue;
        }

        bool bIsPeerToPeerSession;
        EOS_EResult ApplyResult = this->ApplyConnectionSettingsToModificationHandle(
            InternetAddr,
            DeveloperInternetAddrs,
            ModHandle,
            this->Sessions[SearchIndex].SessionSettings,
            bIsPeerToPeerSession);
        if (ApplyResult != EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("Handle_SessionAddressChanged: Could not apply settings to session handle (%s)"),
                ANSI_TO_TCHAR(EOS_EResult_ToString(ApplyResult)));
            EOS_SessionModification_Release(ModHandle);
            continue;
        }

        EOS_Sessions_UpdateSessionOptions UpdateOpts = {};
        UpdateOpts.ApiVersion = EOS_SESSIONS_UPDATESESSION_API_LATEST;
        UpdateOpts.SessionModificationHandle = ModHandle;
        EOSRunOperation<EOS_HSessions, EOS_Sessions_UpdateSessionOptions, EOS_Sessions_UpdateSessionCallbackInfo>(
            this->EOSSessions,
            &UpdateOpts,
            EOS_Sessions_UpdateSession,
            [WeakThis = GetWeakThis(this), ModHandle](const EOS_Sessions_UpdateSessionCallbackInfo *Info) {
                EOS_SessionModification_Release(ModHandle);

                if (Info->ResultCode != EOS_EResult::EOS_Success)
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("Handle_SessionAddressChanged: Failed to update host address, got error %s"),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Info->ResultCode)));
                    return;
                }

                UE_LOG(LogEOS, Verbose, TEXT("Handle_SessionAddressChanged: Updated host address successfully"));
            });
    }
}

void FOnlineSessionInterfaceEOS::Handle_SessionAddressClosed(EOS_ProductUserId ProductUserId)
{
    auto TargetUserIdEOS =
        ProductUserId == nullptr ? FUniqueNetIdEOS::DedicatedServerId() : MakeShared<FUniqueNetIdEOS>(ProductUserId);

    FScopeLock ScopeLock(&this->SessionLock);
    for (int32 SearchIndex = 0; SearchIndex < this->Sessions.Num(); SearchIndex++)
    {
        auto OwnerUserId = this->Sessions[SearchIndex].OwningUserId;
        if (!OwnerUserId.IsValid() || !OwnerUserId->GetType().IsEqual(REDPOINT_EOS_SUBSYSTEM))
        {
            continue;
        }

        if (*StaticCastSharedPtr<const FUniqueNetIdEOS>(OwnerUserId) != *TargetUserIdEOS)
        {
            continue;
        }

        // We found a session that is impacted by the address change, perform an UpdateSession operation
        // to remove it's listening status.

        auto SessionNameAnsi =
            EOSString_SessionModification_SessionName::ToAnsiString(this->Sessions[SearchIndex].SessionName.ToString());

        EOS_Sessions_UpdateSessionModificationOptions ModOpts = {};
        ModOpts.ApiVersion = EOS_SESSIONS_UPDATESESSIONMODIFICATION_API_LATEST;
        ModOpts.SessionName = SessionNameAnsi.Ptr.Get();

        EOS_HSessionModification ModHandle = {};
        if (EOS_Sessions_UpdateSessionModification(this->EOSSessions, &ModOpts, &ModHandle) != EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("Handle_SessionAddressClosed: Could not create session modification handle to update listening "
                     "status"));
            continue;
        }

        // Mark the session as *not* listening so clients will no longer discover it in search results.
        {
            EOS_Sessions_AttributeData Attribute = {};
            Attribute.ApiVersion = EOS_SESSIONS_SESSIONATTRIBUTEDATA_API_LATEST;
            Attribute.Key = EOS_WELL_KNOWN_ATTRIBUTE_BLISTENING;
            Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_BOOLEAN;
            Attribute.Value.AsBool = EOS_FALSE;

            EOS_SessionModification_AddAttributeOptions AddOpts = {};
            AddOpts.ApiVersion = EOS_SESSIONMODIFICATION_ADDATTRIBUTE_API_LATEST;
            AddOpts.AdvertisementType = EOS_ESessionAttributeAdvertisementType::EOS_SAAT_Advertise;
            AddOpts.SessionAttribute = &Attribute;
            auto Result = EOS_SessionModification_AddAttribute(ModHandle, &AddOpts);
            if (Result != EOS_EResult::EOS_Success)
            {
                UE_LOG(
                    LogEOS,
                    Error,
                    TEXT("Handle_SessionAddressClosed: Can not update session as no longer listening for connections"));
                return;
            }
        }

        EOS_Sessions_UpdateSessionOptions UpdateOpts = {};
        UpdateOpts.ApiVersion = EOS_SESSIONS_UPDATESESSION_API_LATEST;
        UpdateOpts.SessionModificationHandle = ModHandle;
        EOSRunOperation<EOS_HSessions, EOS_Sessions_UpdateSessionOptions, EOS_Sessions_UpdateSessionCallbackInfo>(
            this->EOSSessions,
            &UpdateOpts,
            EOS_Sessions_UpdateSession,
            [WeakThis = GetWeakThis(this), ModHandle](const EOS_Sessions_UpdateSessionCallbackInfo *Info) {
                EOS_SessionModification_Release(ModHandle);

                if (Info->ResultCode != EOS_EResult::EOS_Success)
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("Handle_SessionAddressClosed: Failed to update host address, got error %s"),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Info->ResultCode)));
                    return;
                }

                UE_LOG(LogEOS, Verbose, TEXT("Handle_SessionAddressClosed: Updated host address successfully"));
            });
    }
}

void FOnlineSessionInterfaceEOS::RegisterEvents()
{
    this->ListenTracker->OnChanged.AddThreadSafeSP(this, &FOnlineSessionInterfaceEOS::Handle_SessionAddressChanged);
    this->ListenTracker->OnClosed.AddThreadSafeSP(this, &FOnlineSessionInterfaceEOS::Handle_SessionAddressClosed);

    EOS_Sessions_AddNotifyJoinSessionAcceptedOptions OptsJoinSessionAccepted = {};
    OptsJoinSessionAccepted.ApiVersion = EOS_SESSIONS_ADDNOTIFYJOINSESSIONACCEPTED_API_LATEST;
    EOS_Sessions_AddNotifySessionInviteAcceptedOptions OptsSessionInviteAccepted = {};
    OptsSessionInviteAccepted.ApiVersion = EOS_SESSIONS_ADDNOTIFYSESSIONINVITEACCEPTED_API_LATEST;
    EOS_Sessions_AddNotifySessionInviteReceivedOptions OptsSessionInviteReceived = {};
    OptsSessionInviteReceived.ApiVersion = EOS_SESSIONS_ADDNOTIFYSESSIONINVITERECEIVED_API_LATEST;

    this->Unregister_JoinSessionAccepted = EOSRegisterEvent<
        EOS_HSessions,
        EOS_Sessions_AddNotifyJoinSessionAcceptedOptions,
        EOS_Sessions_JoinSessionAcceptedCallbackInfo>(
        this->EOSSessions,
        &OptsJoinSessionAccepted,
        EOS_Sessions_AddNotifyJoinSessionAccepted,
        EOS_Sessions_RemoveNotifyJoinSessionAccepted,
        [WeakThis = GetWeakThis(this)](const EOS_Sessions_JoinSessionAcceptedCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                This->Handle_JoinSessionAccepted(Data);
            }
        });
    this->Unregister_SessionInviteAccepted = EOSRegisterEvent<
        EOS_HSessions,
        EOS_Sessions_AddNotifySessionInviteAcceptedOptions,
        EOS_Sessions_SessionInviteAcceptedCallbackInfo>(
        this->EOSSessions,
        &OptsSessionInviteAccepted,
        EOS_Sessions_AddNotifySessionInviteAccepted,
        EOS_Sessions_RemoveNotifySessionInviteAccepted,
        [WeakThis = GetWeakThis(this)](const EOS_Sessions_SessionInviteAcceptedCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                This->Handle_SessionInviteAccepted(Data);
            }
        });
    this->Unregister_SessionInviteReceived = EOSRegisterEvent<
        EOS_HSessions,
        EOS_Sessions_AddNotifySessionInviteReceivedOptions,
        EOS_Sessions_SessionInviteReceivedCallbackInfo>(
        this->EOSSessions,
        &OptsSessionInviteReceived,
        EOS_Sessions_AddNotifySessionInviteReceived,
        EOS_Sessions_RemoveNotifySessionInviteReceived,
        [WeakThis = GetWeakThis(this)](const EOS_Sessions_SessionInviteReceivedCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                This->Handle_SessionInviteReceived(Data);
            }
        });
}

void FOnlineSessionInterfaceEOS::Handle_JoinSessionAccepted(const EOS_Sessions_JoinSessionAcceptedCallbackInfo *Data)
{
    // We have joined a session via the overlay.

    UE_LOG(LogEOS, Verbose, TEXT("Received JoinSessionAccepted event from EOS Sessions system"));

    EOS_HSessionDetails Details = {};
    EOS_Sessions_CopySessionHandleByUiEventIdOptions CopyOpts = {};
    CopyOpts.ApiVersion = EOS_SESSIONS_COPYSESSIONHANDLEBYUIEVENTID_API_LATEST;
    CopyOpts.UiEventId = Data->UiEventId;
    auto Result = EOS_Sessions_CopySessionHandleByUiEventId(this->EOSSessions, &CopyOpts, &Details);
    if (Result != EOS_EResult::EOS_Success)
    {
        EOS_UI_AcknowledgeEventIdOptions AckOpts = {};
        AckOpts.ApiVersion = EOS_UI_ACKNOWLEDGECORRELATIONID_API_LATEST;
        AckOpts.Result = Result;
        AckOpts.UiEventId = Data->UiEventId;
        EOS_UI_AcknowledgeEventId(this->EOSUI, &AckOpts);
        return;
    }

    FOnlineSessionSearchResultEOS SearchResult = FOnlineSessionSearchResultEOS::CreateFromDetails(Details);

    TSharedPtr<FUniqueNetIdEOS> LocalUserEOS = MakeShared<FUniqueNetIdEOS>(Data->LocalUserId);

    int32 LocalUserNum = 0;
    if (!StaticCastSharedPtr<FOnlineIdentityInterfaceEOS, IOnlineIdentity, ESPMode::ThreadSafe>(this->Identity)
             ->GetLocalUserNum(*LocalUserEOS, LocalUserNum))
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Join session notification in JoinSessionAccepted was not for a locally logged in user"));
        EOS_UI_AcknowledgeEventIdOptions AckOpts = {};
        AckOpts.ApiVersion = EOS_UI_ACKNOWLEDGECORRELATIONID_API_LATEST;
        AckOpts.Result = EOS_EResult::EOS_UnexpectedError;
        AckOpts.UiEventId = Data->UiEventId;
        EOS_UI_AcknowledgeEventId(this->EOSUI, &AckOpts);
        return;
    }

    this->TriggerOnSessionUserInviteAcceptedDelegates(true, LocalUserNum, LocalUserEOS, SearchResult);

    UE_LOG(LogEOS, Verbose, TEXT("Acknowledging join session accepted event after firing OnSessionUserInviteAccepted"));
    EOS_UI_AcknowledgeEventIdOptions AckOpts = {};
    AckOpts.ApiVersion = EOS_UI_ACKNOWLEDGECORRELATIONID_API_LATEST;
    AckOpts.Result = EOS_EResult::EOS_Success;
    AckOpts.UiEventId = Data->UiEventId;
    EOS_UI_AcknowledgeEventId(this->EOSUI, &AckOpts);
}

void FOnlineSessionInterfaceEOS::Handle_SessionInviteAccepted(
    const EOS_Sessions_SessionInviteAcceptedCallbackInfo *Data)
{
    // We have accepted an invite from another user via the overlay.

    UE_LOG(
        LogEOS,
        Verbose,
        TEXT("Received SessionInviteAccepted event from EOS Sessions system: %s"),
        ANSI_TO_TCHAR(Data->InviteId));

    EOS_HSessionDetails Details = {};
    EOS_Sessions_CopySessionHandleByInviteIdOptions Opts = {};
    Opts.ApiVersion = EOS_SESSIONS_COPYSESSIONHANDLEBYINVITEID_API_LATEST;
    Opts.InviteId = Data->InviteId;
    if (EOS_Sessions_CopySessionHandleByInviteId(this->EOSSessions, &Opts, &Details) != EOS_EResult::EOS_Success)
    {
        UE_LOG(LogEOS, Error, TEXT("Failed to retrieve session details from invite ID during SessionInviteAccepted"));
        return;
    }

    FOnlineSessionSearchResultEOS SearchResult = FOnlineSessionSearchResultEOS::CreateFromDetails(Details);

    TSharedPtr<FUniqueNetIdEOS> TargetUserEOS = MakeShared<FUniqueNetIdEOS>(Data->TargetUserId);
    TSharedPtr<FUniqueNetIdEOS> LocalUserEOS = MakeShared<FUniqueNetIdEOS>(Data->LocalUserId);

    int32 LocalUserNum = 0;
    if (!StaticCastSharedPtr<FOnlineIdentityInterfaceEOS, IOnlineIdentity, ESPMode::ThreadSafe>(this->Identity)
             ->GetLocalUserNum(*LocalUserEOS, LocalUserNum))
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Accepted session invite in SessionInviteAccepted was not for a locally logged in user"));
        return;
    }

    this->TriggerOnSessionUserInviteAcceptedDelegates(true, LocalUserNum, LocalUserEOS, SearchResult);
}

void FOnlineSessionInterfaceEOS::Handle_SessionInviteReceived(
    const EOS_Sessions_SessionInviteReceivedCallbackInfo *Data)
{
    // We have received an invite from another user.

    UE_LOG(
        LogEOS,
        Verbose,
        TEXT("Received SessionInviteReceived event from EOS Sessions system: %s"),
        ANSI_TO_TCHAR(Data->InviteId));

    EOS_HSessionDetails Details = {};
    EOS_Sessions_CopySessionHandleByInviteIdOptions Opts = {};
    Opts.ApiVersion = EOS_SESSIONS_COPYSESSIONHANDLEBYINVITEID_API_LATEST;
    Opts.InviteId = Data->InviteId;
    if (EOS_Sessions_CopySessionHandleByInviteId(this->EOSSessions, &Opts, &Details) != EOS_EResult::EOS_Success)
    {
        UE_LOG(LogEOS, Error, TEXT("Failed to retrieve session details from invite ID during SessionInviteReceived"));
        return;
    }

    FOnlineSessionSearchResultEOS SearchResult = FOnlineSessionSearchResultEOS::CreateFromDetails(Details);

    TSharedPtr<FUniqueNetIdEOS> SentUserEOS = MakeShared<FUniqueNetIdEOS>(Data->TargetUserId);
    TSharedPtr<FUniqueNetIdEOS> ReceivedUserEOS = MakeShared<FUniqueNetIdEOS>(Data->LocalUserId);

    int32 LocalUserNum = 0;
    if (!StaticCastSharedPtr<FOnlineIdentityInterfaceEOS, IOnlineIdentity, ESPMode::ThreadSafe>(this->Identity)
             ->GetLocalUserNum(*ReceivedUserEOS, LocalUserNum))
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Accepted session invite in SessionInviteReceived was not for a locally logged in user"));
        return;
    }

    this->TriggerOnSessionInviteReceivedDelegates(*ReceivedUserEOS, *SentUserEOS, TEXT(""), SearchResult);
}

#if EOS_HAS_AUTHENTICATION
bool FOnlineSessionInterfaceEOS::ShouldHaveSyntheticSession(const FOnlineSessionSettings &SessionSettings) const
{
    if (this->Config->GetPresenceAdvertisementType() != EPresenceAdvertisementType::Session)
    {
        // Game does not advertise sessions via presence.
        return false;
    }

    return SessionSettings.bUsesPresence;
}
#endif // #if EOS_HAS_AUTHENTICATION

void FOnlineSessionInterfaceEOS::RegisterListeningAddress(
    EOS_ProductUserId InProductUserId,
    TSharedRef<const FInternetAddr> InInternetAddr,
    TArray<TSharedPtr<FInternetAddr>> InDeveloperInternetAddrs)
{
    this->ListenTracker->Register(InProductUserId, MoveTemp(InInternetAddr), MoveTemp(InDeveloperInternetAddrs));
}

void FOnlineSessionInterfaceEOS::DeregisterListeningAddress(
    EOS_ProductUserId InProductUserId,
    TSharedRef<const FInternetAddr> InInternetAddr)
{
    this->ListenTracker->Deregister(InProductUserId, MoveTemp(InInternetAddr));
}

FNamedOnlineSession *FOnlineSessionInterfaceEOS::AddNamedSession(
    FName SessionName,
    const FOnlineSessionSettings &SessionSettings)
{
    FScopeLock ScopeLock(&this->SessionLock);
    return new (this->Sessions) FNamedOnlineSession(SessionName, SessionSettings);
}

FNamedOnlineSession *FOnlineSessionInterfaceEOS::AddNamedSession(FName SessionName, const FOnlineSession &Session)
{
    FScopeLock ScopeLock(&this->SessionLock);
    return new (this->Sessions) FNamedOnlineSession(SessionName, Session);
}

TSharedPtr<const FUniqueNetId> FOnlineSessionInterfaceEOS::CreateSessionIdFromString(const FString &SessionIdStr)
{
    return MakeShared<FUniqueNetIdEOSSession>(SessionIdStr);
}

FNamedOnlineSession *FOnlineSessionInterfaceEOS::GetNamedSession(FName SessionName)
{
    FScopeLock ScopeLock(&this->SessionLock);
    for (int32 SearchIndex = 0; SearchIndex < this->Sessions.Num(); SearchIndex++)
    {
        if (this->Sessions[SearchIndex].SessionName == SessionName)
        {
            return &this->Sessions[SearchIndex];
        }
    }

    return nullptr;
}

void FOnlineSessionInterfaceEOS::RemoveNamedSession(FName SessionName)
{
    FScopeLock ScopeLock(&this->SessionLock);
    for (int32 SearchIndex = 0; SearchIndex < this->Sessions.Num(); SearchIndex++)
    {
        if (this->Sessions[SearchIndex].SessionName == SessionName)
        {
            this->Sessions.RemoveAtSwap(SearchIndex);
            return;
        }
    }
}

bool FOnlineSessionInterfaceEOS::HasPresenceSession()
{
    FScopeLock ScopeLock(&this->SessionLock);
    for (int32 SearchIndex = 0; SearchIndex < this->Sessions.Num(); SearchIndex++)
    {
        if (this->Sessions[SearchIndex].SessionSettings.bUsesPresence)
        {
            return true;
        }
    }

    return false;
}

EOnlineSessionState::Type FOnlineSessionInterfaceEOS::GetSessionState(FName SessionName) const
{
    FScopeLock ScopeLock(&this->SessionLock);
    for (int32 SearchIndex = 0; SearchIndex < this->Sessions.Num(); SearchIndex++)
    {
        if (this->Sessions[SearchIndex].SessionName == SessionName)
        {
            return this->Sessions[SearchIndex].SessionState;
        }
    }

    return EOnlineSessionState::NoSession;
}

bool FOnlineSessionInterfaceEOS::CreateSession(
    int32 HostingPlayerNum,
    FName SessionName,
    const FOnlineSessionSettings &NewSessionSettings)
{
    auto Id = this->Identity->GetUniquePlayerId(HostingPlayerNum);
    if (!Id.IsValid())
    {
        UE_LOG(LogEOS, Error, TEXT("HostingPlayerNum provided to CreateSession does not have online identity."));
        return false;
    }

    return this->CreateSession(Id.ToSharedRef().Get(), SessionName, NewSessionSettings);
}

FString FOnlineSessionInterfaceEOS::GetBucketId(const FOnlineSessionSettings &SessionSettings)
{
    auto GameMode = SessionSettings.Settings.Contains(SETTING_GAMEMODE)
                        ? SessionSettings.Settings[SETTING_GAMEMODE].Data.ToString()
                        : TEXT("<None>");
    auto Region = SessionSettings.Settings.Contains(SETTING_REGION)
                      ? SessionSettings.Settings[SETTING_REGION].Data.ToString()
                      : TEXT("<None>");
    auto MapName = SessionSettings.Settings.Contains(SETTING_MAPNAME)
                       ? SessionSettings.Settings[SETTING_MAPNAME].Data.ToString()
                       : TEXT("<None>");

    FString BucketId = FString::Printf(TEXT("%s:%s:%s"), *GameMode, *Region, *MapName);
    if (BucketId.Len() > 60)
    {
        // The EOS backend limits bucket IDs to 60 characters.
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("The bucket ID '%s' is too long for the EOS backend, which limits bucket IDs to a maximum of 60 "
                 "characters. The bucket ID '%s' will be used instead."),
            *BucketId,
            *BucketId.Mid(0, 60));
        BucketId = BucketId.Mid(0, 60);
    }
    return BucketId;
}

EOS_EResult FOnlineSessionInterfaceEOS::ApplyConnectionSettingsToModificationHandle(
    const TSharedRef<const FInternetAddr> &InternetAddr,
    const TArray<TSharedPtr<FInternetAddr>> &DeveloperInternetAddrs,
    EOS_HSessionModification Handle,
    const FOnlineSessionSettings &SessionSettings,
    bool &bIsPeerToPeerAddress)
{
    FString HostAddressStr = InternetAddr->ToString(true);
    TArray<FString> DeveloperAddressesArray;
    for (const auto &DevAddr : DeveloperInternetAddrs)
    {
        if (DevAddr.IsValid())
        {
            DeveloperAddressesArray.Add(DevAddr->ToString(false));
        }
    }
    FString DeveloperAddressesStr = FString::Join(DeveloperAddressesArray, TEXT(","));
    auto HostAddressAnsi = EOSString_SessionModification_HostAddress::ToAnsiString(HostAddressStr);
    auto HostAddressUtf8 = EOSString_SessionModification_AttributeStringValue::ToUtf8String(HostAddressStr);
    auto DeveloperAddressesUtf8 =
        EOSString_SessionModification_AttributeStringValue::ToUtf8String(DeveloperAddressesStr);
    if (InternetAddr->GetProtocolType().IsEqual(REDPOINT_EOS_SUBSYSTEM))
    {
        // If this is a P2P connection, we just override the host address entirely.
        EOS_SessionModification_SetHostAddressOptions Opts = {};
        Opts.ApiVersion = EOS_SESSIONMODIFICATION_SETHOSTADDRESS_API_LATEST;
        Opts.HostAddress = HostAddressAnsi.Ptr.Get();
        auto Result = EOS_SessionModification_SetHostAddress(Handle, &Opts);
        if (Result != EOS_EResult::EOS_Success)
        {
            return Result;
        }
        bIsPeerToPeerAddress = true;
    }
    else
    {
        // Check to see if the user has overridden the port that we should expose
        // in the AddressBound attribute. We only use this attribute if the EOS_IGNORE_ORCHESTRATOR_PORT
        // environment variable is not enabled (which can be used to prevent server orchestrators overriding
        // the main game port for in-cluster testing).
        FString OverrideAddress = TEXT("");
        bool bUseOverrideAddress = false;
        FString IgnoreOverridePort = FPlatformMisc::GetEnvironmentVariable(TEXT("EOS_IGNORE_ORCHESTRATOR_PORT"));
        if (IgnoreOverridePort != TEXT("true"))
        {
            if (SessionSettings.Settings.Contains(FName(EOS_WELL_KNOWN_ATTRIBUTE_OVERRIDEADDRESSBOUND)))
            {
                SessionSettings.Settings.Find(FName(EOS_WELL_KNOWN_ATTRIBUTE_OVERRIDEADDRESSBOUND))
                    ->Data.GetValue(OverrideAddress);
                bUseOverrideAddress = true;
            }
        }
        auto OverrideAddressBound = FTCHARToUTF8(*OverrideAddress);

        // If this is an IP-based connection, we explicitly *don't* set the HostAddress, because
        // we want EOS to populate that field with the true public IP address. But we still need
        // to include own address information so we can compute the port to connect to (which EOS
        // does not know about).
        EOS_Sessions_AttributeData Attribute = {};
        Attribute.ApiVersion = EOS_SESSIONS_SESSIONATTRIBUTEDATA_API_LATEST;
        Attribute.Key = EOS_WELL_KNOWN_ATTRIBUTE_ADDRESS_BOUND;
        Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_STRING;
        Attribute.Value.AsUtf8 =
            bUseOverrideAddress ? ((const char *)OverrideAddressBound.Get()) : HostAddressUtf8.GetAsChar();

        EOS_SessionModification_AddAttributeOptions AddOpts = {};
        AddOpts.ApiVersion = EOS_SESSIONMODIFICATION_ADDATTRIBUTE_API_LATEST;
        AddOpts.AdvertisementType = EOS_ESessionAttributeAdvertisementType::EOS_SAAT_Advertise;
        AddOpts.SessionAttribute = &Attribute;
        auto Result = EOS_SessionModification_AddAttribute(Handle, &AddOpts);
        if (Result != EOS_EResult::EOS_Success)
        {
            return Result;
        }

        // If the EOS_OVERRIDE_HOST_IP environment variable is set, this forcibly overrides any IP
        // detection that the EOS backend will do for IP-based connections. The value in this environment
        // variable will be stored in the Address attribute. The port detection provided by server
        // orchestrators will still apply.
        FString OverrideHostIP = FPlatformMisc::GetEnvironmentVariable(TEXT("EOS_OVERRIDE_HOST_IP"));
        if (!OverrideHostIP.IsEmpty())
        {
            UE_LOG(LogEOS, Verbose, TEXT("Using host IP from EOS_OVERRIDE_HOST_IP: %s"), *OverrideHostIP);
            auto OverrideHostAddressAnsi = EOSString_SessionModification_HostAddress::ToAnsiString(OverrideHostIP);
            EOS_SessionModification_SetHostAddressOptions Opts = {};
            Opts.ApiVersion = EOS_SESSIONMODIFICATION_SETHOSTADDRESS_API_LATEST;
            Opts.HostAddress = OverrideHostAddressAnsi.Ptr.Get();
            auto OverrideResult = EOS_SessionModification_SetHostAddress(Handle, &Opts);
            if (OverrideResult != EOS_EResult::EOS_Success)
            {
                UE_LOG(
                    LogEOS,
                    Error,
                    TEXT("Failed to set host address based on EOS_OVERRIDE_HOST_IP environment variable: %s"),
                    *ConvertError(OverrideResult).ToLogString());
                return OverrideResult;
            }
        }
        // This is an "else if", since we don't want to do any development IP resolution if the
        // host IP is being provided by an environment variable.
#if defined(EOS_SUPPORT_MULTI_IP_RESOLUTION)
        else if (!FParse::Param(FCommandLine::Get(), TEXT("emulateeosshipping")))
        {
            // If we have at least one developer address, set the "AddressDev" attribute as well. This
            // field is used so that play-in-editor and other machines on the same local area network
            // can connect to a dedicated server during development.
            if (DeveloperInternetAddrs.Num() > 0)
            {
                EOS_Sessions_AttributeData DevAttribute = {};
                DevAttribute.ApiVersion = EOS_SESSIONS_SESSIONATTRIBUTEDATA_API_LATEST;
                DevAttribute.Key = EOS_WELL_KNOWN_ATTRIBUTE_ADDRESS_DEV;
                DevAttribute.ValueType = EOS_ESessionAttributeType::EOS_AT_STRING;
                DevAttribute.Value.AsUtf8 = DeveloperAddressesUtf8.GetAsChar();

                EOS_SessionModification_AddAttributeOptions DevAddOpts = {};
                DevAddOpts.ApiVersion = EOS_SESSIONMODIFICATION_ADDATTRIBUTE_API_LATEST;
                DevAddOpts.AdvertisementType = EOS_ESessionAttributeAdvertisementType::EOS_SAAT_Advertise;
                DevAddOpts.SessionAttribute = &DevAttribute;
                auto DevResult = EOS_SessionModification_AddAttribute(Handle, &DevAddOpts);
                if (DevResult != EOS_EResult::EOS_Success)
                {
                    return DevResult;
                }
            }
        }
#endif
    }

    // Mark the session as listening so we can discover it in search results.
    {
        EOS_Sessions_AttributeData Attribute = {};
        Attribute.ApiVersion = EOS_SESSIONS_SESSIONATTRIBUTEDATA_API_LATEST;
        Attribute.Key = EOS_WELL_KNOWN_ATTRIBUTE_BLISTENING;
        Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_BOOLEAN;
        Attribute.Value.AsBool = EOS_TRUE;

        EOS_SessionModification_AddAttributeOptions AddOpts = {};
        AddOpts.ApiVersion = EOS_SESSIONMODIFICATION_ADDATTRIBUTE_API_LATEST;
        AddOpts.AdvertisementType = EOS_ESessionAttributeAdvertisementType::EOS_SAAT_Advertise;
        AddOpts.SessionAttribute = &Attribute;
        auto Result = EOS_SessionModification_AddAttribute(Handle, &AddOpts);
        if (Result != EOS_EResult::EOS_Success)
        {
            return Result;
        }
    }

    return EOS_EResult::EOS_Success;
}

EOS_EResult FOnlineSessionInterfaceEOS::ApplySettingsToModificationHandle(
    const TSharedPtr<const FUniqueNetIdEOS> &HostingUserId,
    const FOnlineSessionSettings &SessionSettings,
    EOS_HSessionModification Handle,
    const FOnlineSessionSettings *ExistingSessionSettings)
{
    // Try to update the host address with information passed in through the net driver.
    EOS_ProductUserId ProductUserIdKey =
        HostingUserId->IsDedicatedServer() ? nullptr : HostingUserId->GetProductUserId();
    bool bIsPeerToPeerAddress = false;
    TSharedPtr<const FInternetAddr> HostAddress;
    TArray<TSharedPtr<FInternetAddr>> DeveloperAddresses;
    if (this->ListenTracker->Get(ProductUserIdKey, HostAddress, DeveloperAddresses))
    {
        EOS_EResult ConnectionSettingsResult = this->ApplyConnectionSettingsToModificationHandle(
            HostAddress.ToSharedRef(),
            DeveloperAddresses,
            Handle,
            SessionSettings,
            bIsPeerToPeerAddress);
        if (ConnectionSettingsResult != EOS_EResult::EOS_Success)
        {
            return ConnectionSettingsResult;
        }
    }
    else
    {
        // Make it clearer for developers when this situation is caused by not using the
        // EOS net driver (due to misconfiguration). Detect if the EOS net driver has been
        // set as the networking driver properly.
        if (GEngine != nullptr)
        {
            bool bDidFindEOSNetDriver = false;
            for (auto Definition : GEngine->NetDriverDefinitions)
            {
                if (Definition.DefName.IsEqual(FName(TEXT("GameNetDriver"))) &&
                    Definition.DriverClassName.IsEqual(FName(TEXT("/Script/OnlineSubsystemRedpointEOS.EOSNetDriver"))))
                {
                    bDidFindEOSNetDriver = true;
                    break;
                }
            }
            if (!bDidFindEOSNetDriver)
            {
                // Do not emit this error during automated testing, because it will cause session tests to
                // fail unnecessarily.
                if (!this->Config->IsAutomatedTesting())
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT(
                            "You are not using the EOS networking driver, so the host address can not be set correctly "
                            "during the CreateSession or UpdateSession call. In addition, players will not be able "
                            "connect over P2P. Fix your configuration to correctly set up networking: "
                            "https://docs.redpoint.games/eos-online-subsystem/docs/"
                            "core_configuration#enabling-epic-online-services"));
                }
            }
        }

        // NOTE: We no longer emit a warning now, since creating a session before starting the server is now
        // a valid operation to do.

        // Mark the session as *not* listening so clients won't discover it in search results.
        {
            EOS_Sessions_AttributeData Attribute = {};
            Attribute.ApiVersion = EOS_SESSIONS_SESSIONATTRIBUTEDATA_API_LATEST;
            Attribute.Key = EOS_WELL_KNOWN_ATTRIBUTE_BLISTENING;
            Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_BOOLEAN;
            Attribute.Value.AsBool = EOS_FALSE;

            EOS_SessionModification_AddAttributeOptions AddOpts = {};
            AddOpts.ApiVersion = EOS_SESSIONMODIFICATION_ADDATTRIBUTE_API_LATEST;
            AddOpts.AdvertisementType = EOS_ESessionAttributeAdvertisementType::EOS_SAAT_Advertise;
            AddOpts.SessionAttribute = &Attribute;
            auto Result = EOS_SessionModification_AddAttribute(Handle, &AddOpts);
            if (Result != EOS_EResult::EOS_Success)
            {
                UE_LOG(
                    LogEOS,
                    Error,
                    TEXT("EOS_SessionModification_AddAttribute operation failed when setting '%s' attribute to %s "
                         "(result code: %s)"),
                    ANSI_TO_TCHAR(EOS_WELL_KNOWN_ATTRIBUTE_BLISTENING),
                    TEXT("false"),
                    ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
                return Result;
            }
        }
    }

    {
        auto BucketIdAnsi = EOSString_SessionModification_BucketId::ToAnsiString(this->GetBucketId(SessionSettings));

        EOS_SessionModification_SetBucketIdOptions Opts = {};
        Opts.ApiVersion = EOS_SESSIONMODIFICATION_SETBUCKETID_API_LATEST;
        Opts.BucketId = BucketIdAnsi.Ptr.Get();
        auto Result = EOS_SessionModification_SetBucketId(Handle, &Opts);
        if (Result != EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT(
                    "EOS_SessionModification_SetBucketId operation failed when setting to bucket ID '%s' (result code: "
                    "%s)"),
                *this->GetBucketId(SessionSettings),
                ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
            return Result;
        }
    }

    {
        EOS_SessionModification_SetMaxPlayersOptions Opts = {};
        Opts.ApiVersion = EOS_SESSIONMODIFICATION_SETMAXPLAYERS_API_LATEST;
        Opts.MaxPlayers = SessionSettings.NumPublicConnections;
        auto Result = EOS_SessionModification_SetMaxPlayers(Handle, &Opts);
        if (Result != EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("EOS_SessionModification_SetMaxPlayers operation failed when setting number of players to %d "
                     "(result code: "
                     "%s)"),
                SessionSettings.NumPublicConnections,
                ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
            return Result;
        }
    }

    {
        EOS_SessionModification_SetJoinInProgressAllowedOptions Opts = {};
        Opts.ApiVersion = EOS_SESSIONMODIFICATION_SETJOININPROGRESSALLOWED_API_LATEST;
        Opts.bAllowJoinInProgress = SessionSettings.bAllowJoinInProgress;
        auto Result = EOS_SessionModification_SetJoinInProgressAllowed(Handle, &Opts);
        if (Result != EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("EOS_SessionModification_SetJoinInProgressAllowed operation failed when join in progress to %s "
                     "(result code: "
                     "%s)"),
                SessionSettings.bAllowJoinInProgress ? TEXT("true") : TEXT("false"),
                ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
            return Result;
        }
    }

    {
        EOS_SessionModification_SetPermissionLevelOptions Opts = {};
        Opts.ApiVersion = EOS_SESSIONMODIFICATION_SETPERMISSIONLEVEL_API_LATEST;
        if (SessionSettings.bShouldAdvertise)
        {
            Opts.PermissionLevel = EOS_EOnlineSessionPermissionLevel::EOS_OSPF_PublicAdvertised;
        }
        else if (SessionSettings.bAllowJoinViaPresence)
        {
            Opts.PermissionLevel = EOS_EOnlineSessionPermissionLevel::EOS_OSPF_JoinViaPresence;
        }
        else if (SessionSettings.bAllowInvites)
        // NOLINTNEXTLINE(bugprone-branch-clone)
        {
            Opts.PermissionLevel = EOS_EOnlineSessionPermissionLevel::EOS_OSPF_InviteOnly;
        }
        else
        {
            // Default to invite only. If we don't do this, it would default to PublicAdvertised which is definitely not
            // what you want if bShouldAdvertise is false.
            Opts.PermissionLevel = EOS_EOnlineSessionPermissionLevel::EOS_OSPF_InviteOnly;
        }
        auto Result = EOS_SessionModification_SetPermissionLevel(Handle, &Opts);
        if (Result != EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("EOS_SessionModification_SetPermissionLevel operation failed when setting permission level to %d "
                     "(result code: "
                     "%s)"),
                Opts.PermissionLevel,
                ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
            return Result;
        }
    }

    {
        EOS_SessionModification_SetInvitesAllowedOptions Opts = {};
        Opts.ApiVersion = EOS_SESSIONMODIFICATION_SETINVITESALLOWED_API_LATEST;
        Opts.bInvitesAllowed = SessionSettings.bAllowInvites;
        auto Result = EOS_SessionModification_SetInvitesAllowed(Handle, &Opts);
        if (Result != EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("EOS_SessionModification_SetInvitesAllowed operation failed when setting invites allowed to %s "
                     "(result code: "
                     "%s)"),
                Opts.bInvitesAllowed ? TEXT("true") : TEXT("false"),
                ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
            return Result;
        }
    }

    if (ExistingSessionSettings == nullptr)
    {
        // We only set __EOS_bUsesPresence on create, because EOS does not actually support
        // changing this setting at the API level later.

        EOS_Sessions_AttributeData Attribute = {};
        Attribute.ApiVersion = EOS_SESSIONS_SESSIONATTRIBUTEDATA_API_LATEST;
        Attribute.Key = EOS_WELL_KNOWN_ATTRIBUTE_BUSESPRESENCE;
        Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_BOOLEAN;
        Attribute.Value.AsBool = SessionSettings.bUsesPresence ? EOS_TRUE : EOS_FALSE;

        EOS_SessionModification_AddAttributeOptions AddOpts = {};
        AddOpts.ApiVersion = EOS_SESSIONMODIFICATION_ADDATTRIBUTE_API_LATEST;
        AddOpts.AdvertisementType = EOS_ESessionAttributeAdvertisementType::EOS_SAAT_Advertise;
        AddOpts.SessionAttribute = &Attribute;

        auto Result = EOS_SessionModification_AddAttribute(Handle, &AddOpts);
        if (Result != EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("EOS_SessionModification_AddAttribute operation failed when setting '%s' attribute to %s (result "
                     "code: "
                     "%s)"),
                ANSI_TO_TCHAR(EOS_WELL_KNOWN_ATTRIBUTE_BUSESPRESENCE),
                SessionSettings.bUsesPresence ? TEXT("true") : TEXT("false"),
                ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
            return Result;
        }
    }

    for (const auto &Setting : SessionSettings.Settings)
    {
        auto KeyStr = EOSString_SessionModification_AttributeKey::ToAnsiString(Setting.Key.ToString());
        auto ValueStr = EOSString_SessionModification_AttributeStringValue::ToUtf8String(Setting.Value.Data.ToString());

        // Set attribute value.
        EOS_Sessions_AttributeData Attribute = {};
        Attribute.ApiVersion = EOS_SESSIONS_SESSIONATTRIBUTEDATA_API_LATEST;
        Attribute.Key = KeyStr.Ptr.Get();
        switch (Setting.Value.Data.GetType())
        {
        case EOnlineKeyValuePairDataType::Bool:
            Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_BOOLEAN;
            bool BoolVal;
            Setting.Value.Data.GetValue(BoolVal);
            Attribute.Value.AsBool = BoolVal ? EOS_TRUE : EOS_FALSE;
            break;
        case EOnlineKeyValuePairDataType::Int64:
            Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_INT64;
            int64 Int64Val;
            Setting.Value.Data.GetValue(Int64Val);
            Attribute.Value.AsInt64 = Int64Val;
            break;
        case EOnlineKeyValuePairDataType::Double:
            Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_DOUBLE;
            Setting.Value.Data.GetValue(Attribute.Value.AsDouble);
            break;
        case EOnlineKeyValuePairDataType::Float:
            Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_DOUBLE;
            {
                float FloatTmp;
                Setting.Value.Data.GetValue(FloatTmp);
                Attribute.Value.AsDouble = FloatTmp;
            }
            break;
        case EOnlineKeyValuePairDataType::String:
            Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_STRING;
            Attribute.Value.AsUtf8 = ValueStr.GetAsChar();
            if (ValueStr.Ptr.Length() == 0)
            {
                UE_LOG(
                    LogEOS,
                    Warning,
                    TEXT("EOS_SessionModification_AddAttribute called for string attribute '%s', but the string value "
                         "has a length of 0 - this will probably fail!"),
                    ANSI_TO_TCHAR(Attribute.Key));
            }
            break;
        default:
            UE_LOG(
                LogEOS,
                Error,
                TEXT("ApplySettingsToModificationHandle: Unsupported data type %s for search parameter %s"),
                Setting.Value.Data.GetTypeString(),
                *Setting.Key.ToString());
            return EOS_EResult::EOS_InvalidParameters;
        }

        EOS_SessionModification_AddAttributeOptions AddOpts = {};
        AddOpts.ApiVersion = EOS_SESSIONMODIFICATION_ADDATTRIBUTE_API_LATEST;
        AddOpts.AdvertisementType =
            Setting.Value.AdvertisementType >= EOnlineDataAdvertisementType::Type::ViaOnlineService
                ? EOS_ESessionAttributeAdvertisementType::EOS_SAAT_Advertise
                : EOS_ESessionAttributeAdvertisementType::EOS_SAAT_DontAdvertise;
        AddOpts.SessionAttribute = &Attribute;

        auto Result = EOS_SessionModification_AddAttribute(Handle, &AddOpts);
        if (Result != EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("EOS_SessionModification_AddAttribute operation failed when setting '%s' attribute (result "
                     "code: "
                     "%s)"),
                ANSI_TO_TCHAR(Attribute.Key),
                ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
            return Result;
        }
    }

    if (ExistingSessionSettings != nullptr)
    {
        for (const auto &OldSetting : ExistingSessionSettings->Settings)
        {
            if (!SessionSettings.Settings.Contains(OldSetting.Key))
            {
                auto KeyStr = EOSString_SessionModification_AttributeKey::ToAnsiString(OldSetting.Key.ToString());

                EOS_SessionModification_RemoveAttributeOptions RemoveOpts = {};
                RemoveOpts.ApiVersion = EOS_SESSIONMODIFICATION_REMOVEATTRIBUTE_API_LATEST;
                RemoveOpts.Key = KeyStr.Ptr.Get();

                auto Result = EOS_SessionModification_RemoveAttribute(Handle, &RemoveOpts);
                if (Result != EOS_EResult::EOS_Success)
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("EOS_SessionModification_RemoveAttribute operation failed when setting '%s' attribute "
                             "(result "
                             "code: "
                             "%s)"),
                        ANSI_TO_TCHAR(RemoveOpts.Key),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
                    return Result;
                }
            }
        }
    }

    return EOS_EResult::EOS_Success;
}

void FOnlineSessionInterfaceEOS::MetricsSend_BeginPlayerSession(
    const FUniqueNetIdEOS &UserId,
    const FString &SessionId,
    const FString &GameServerAddress)
{
    auto ProductUserIdAnsi = EOSString_ProductUserId::ToAnsiString(UserId.GetProductUserId());
    auto UserAccount = this->Identity->GetUserAccount(UserId);
    FString DisplayName = TEXT("");
    if (UserAccount.IsValid())
    {
        DisplayName = UserAccount->GetDisplayName();
    }
    auto DisplayNameUtf8 = EOSString_UserInfo_DisplayName::ToUtf8String(DisplayName);
    auto SessionIdAnsi = StringCast<ANSICHAR>(*SessionId);
    auto GameServerAddressAnsi = StringCast<ANSICHAR>(*GameServerAddress);

#if EOS_HAS_AUTHENTICATION
    if (!UserId.IsDedicatedServer())
    {
        EOS_Metrics_BeginPlayerSessionOptions MetricsOpts = {};
        MetricsOpts.ApiVersion = EOS_METRICS_BEGINPLAYERSESSION_API_LATEST;
        TSharedPtr<const FCrossPlatformAccountId> CrossPlatformAccountId =
            this->Identity->GetCrossPlatformAccountId(UserId);
        if (CrossPlatformAccountId.IsValid() && CrossPlatformAccountId->GetType() == EPIC_GAMES_ACCOUNT_ID)
        {
            EOS_EpicAccountId EpicAccountId =
                StaticCastSharedPtr<const FEpicGamesCrossPlatformAccountId>(CrossPlatformAccountId)->GetEpicAccountId();
            if (EOSString_EpicAccountId::IsValid(EpicAccountId))
            {
                MetricsOpts.AccountIdType = EOS_EMetricsAccountIdType::EOS_MAIT_Epic;
                MetricsOpts.AccountId.Epic = EpicAccountId;
            }
            else
            {
                MetricsOpts.AccountIdType = EOS_EMetricsAccountIdType::EOS_MAIT_External;
                MetricsOpts.AccountId.External = ProductUserIdAnsi.Ptr.Get();
            }
        }
        else
        {
            MetricsOpts.AccountIdType = EOS_EMetricsAccountIdType::EOS_MAIT_External;
            MetricsOpts.AccountId.External = ProductUserIdAnsi.Ptr.Get();
        }
        MetricsOpts.ControllerType = EOS_EUserControllerType::EOS_UCT_Unknown;
        MetricsOpts.DisplayName = DisplayNameUtf8.GetAsChar();
        MetricsOpts.GameSessionId = SessionIdAnsi.Get();
        MetricsOpts.ServerIp = GameServerAddressAnsi.Get();
        auto MetricsResult = EOS_Metrics_BeginPlayerSession(this->EOSMetrics, &MetricsOpts);
        if (MetricsResult != EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("EOS_Metrics_BeginPlayerSession failed: %s"),
                ANSI_TO_TCHAR(EOS_EResult_ToString(MetricsResult)));
        }
    }
#endif // #if EOS_HAS_AUTHENTICATION
}

void FOnlineSessionInterfaceEOS::MetricsSend_EndPlayerSession(const FUniqueNetIdEOS &UserId)
{
    auto ProductUserIdAnsi = EOSString_ProductUserId::ToAnsiString(UserId.GetProductUserId());

#if EOS_HAS_AUTHENTICATION
    if (!UserId.IsDedicatedServer())
    {
        EOS_Metrics_EndPlayerSessionOptions MetricsOpts = {};
        MetricsOpts.ApiVersion = EOS_METRICS_ENDPLAYERSESSION_API_LATEST;
        TSharedPtr<const FCrossPlatformAccountId> CrossPlatformAccountId =
            this->Identity->GetCrossPlatformAccountId(UserId);
        if (CrossPlatformAccountId.IsValid() && CrossPlatformAccountId->GetType() == EPIC_GAMES_ACCOUNT_ID)
        {
            EOS_EpicAccountId EpicAccountId =
                StaticCastSharedPtr<const FEpicGamesCrossPlatformAccountId>(CrossPlatformAccountId)->GetEpicAccountId();
            if (EOSString_EpicAccountId::IsValid(EpicAccountId))
            {
                MetricsOpts.AccountIdType = EOS_EMetricsAccountIdType::EOS_MAIT_Epic;
                MetricsOpts.AccountId.Epic = EpicAccountId;
            }
            else
            {
                MetricsOpts.AccountIdType = EOS_EMetricsAccountIdType::EOS_MAIT_External;
                MetricsOpts.AccountId.External = ProductUserIdAnsi.Ptr.Get();
            }
        }
        else
        {
            MetricsOpts.AccountIdType = EOS_EMetricsAccountIdType::EOS_MAIT_External;
            MetricsOpts.AccountId.External = ProductUserIdAnsi.Ptr.Get();
        }
        auto MetricsResult = EOS_Metrics_EndPlayerSession(this->EOSMetrics, &MetricsOpts);
        if (MetricsResult != EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("EOS_Metrics_EndPlayerSession failed: %s"),
                ANSI_TO_TCHAR(EOS_EResult_ToString(MetricsResult)));
        }
    }
#endif // #if EOS_HAS_AUTHENTICATION
}

bool FOnlineSessionInterfaceEOS::CreateSession(
    const FUniqueNetId &HostingPlayerId,
    FName SessionName,
    const FOnlineSessionSettings &NewSessionSettings)
{
#if EOS_HAS_ORCHESTRATORS
    if (this->ServerOrchestrator.IsValid())
    {
        if (HostingPlayerId.GetType() != REDPOINT_EOS_SUBSYSTEM)
        {
            UE_LOG(LogEOS, Error, TEXT("CreateSession: Hosting user ID was invalid"));
            return false;
        }

        // Get the port definitions from the server orchestrator, and update the session settings
        // based on it.
        this->ServerOrchestrator->GetPortMappings(FServerOrchestratorGetPortMappingsComplete::CreateLambda(
            [WeakThis = GetWeakThis(this),
             HostingPlayerId = HostingPlayerId.AsShared(),
             SessionName,
             NewSessionSettings = MakeShared<FOnlineSessionSettings>(NewSessionSettings)](
                const TArray<FServerOrchestratorPortMapping> &PortMappings) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    for (const auto &PortMapping : PortMappings)
                    {
                        if (PortMapping.GetPortName().IsEqual(NAME_GamePort) ||
                            PortMapping.GetPortName().IsEqual(FName("game")) ||
                            PortMapping.GetPortName().IsEqual(FName("default")))
                        {
                            NewSessionSettings->Settings.Add(
                                EOS_WELL_KNOWN_ATTRIBUTE_OVERRIDEADDRESSBOUND,
                                FOnlineSessionSetting(
                                    FString::Printf(TEXT("0.0.0.0:%d"), PortMapping.GetPortValue()),
                                    EOnlineDataAdvertisementType::ViaOnlineService));
                        }
                        else
                        {
                            NewSessionSettings->Settings.Add(
                                FName(*FString::Printf(TEXT("Port_%s"), *PortMapping.GetPortName().ToString())),
                                FOnlineSessionSetting(
                                    FString::Printf(TEXT("%d"), PortMapping.GetPortValue()),
                                    EOnlineDataAdvertisementType::ViaOnlineService));
                        }
                    }

                    This->CreateSessionInternal(*HostingPlayerId, SessionName, *NewSessionSettings);
                }
            }));
        return true;
    }
    else
    {
        return CreateSessionInternal(HostingPlayerId, SessionName, NewSessionSettings);
    }
#else
    return CreateSessionInternal(HostingPlayerId, SessionName, NewSessionSettings);
#endif // #if EOS_HAS_ORCHESTRATORS
}

bool FOnlineSessionInterfaceEOS::CreateSessionInternal(
    const FUniqueNetId &HostingPlayerId,
    FName SessionName,
    const FOnlineSessionSettings &NewSessionSettings)
{
    FNamedOnlineSession *Session = this->GetNamedSession(SessionName);
    if (Session == nullptr)
    {
        Session = AddNamedSession(SessionName, NewSessionSettings);
        Session->SessionState = EOnlineSessionState::Creating;
        Session->OwningUserId = HostingPlayerId.AsShared();
        Session->LocalOwnerId = HostingPlayerId.AsShared();

        if (HostingPlayerId.GetType() != REDPOINT_EOS_SUBSYSTEM)
        {
            RemoveNamedSession(SessionName);
            UE_LOG(LogEOS, Error, TEXT("CreateSession: Hosting user ID was invalid"));
            return false;
        }

        auto EOSHostingUser = StaticCastSharedRef<const FUniqueNetIdEOS>(HostingPlayerId.AsShared());
        if (!EOSHostingUser->HasValidProductUserId())
        {
            RemoveNamedSession(SessionName);
            UE_LOG(LogEOS, Error, TEXT("CreateSession: Hosting user ID was invalid"));
            return false;
        }

        auto SessionNameAnsi = EOSString_SessionModification_SessionName::ToAnsiString(SessionName.ToString());
        auto BucketIdAnsi = EOSString_SessionModification_BucketId::ToAnsiString(GetBucketId(NewSessionSettings));

        EOS_Sessions_CreateSessionModificationOptions ModOpts = {};
        ModOpts.ApiVersion = EOS_SESSIONS_CREATESESSIONMODIFICATION_API_LATEST;
        ModOpts.SessionName = SessionNameAnsi.Ptr.Get();
        ModOpts.BucketId = BucketIdAnsi.Ptr.Get();
        ModOpts.MaxPlayers = NewSessionSettings.NumPublicConnections;
        ModOpts.LocalUserId = EOSHostingUser->GetProductUserId();
        if (this->Config->GetPresenceAdvertisementType() == EPresenceAdvertisementType::Session)
        {
            ModOpts.bPresenceEnabled = NewSessionSettings.bUsesPresence;
        }
        else
        {
            ModOpts.bPresenceEnabled = false;
        }

        EOS_HSessionModification ModHandle = {};
        EOS_EResult ModResult = EOS_Sessions_CreateSessionModification(this->EOSSessions, &ModOpts, &ModHandle);
        if (ModResult != EOS_EResult::EOS_Success)
        {
            RemoveNamedSession(SessionName);
            UE_LOG(
                LogEOS,
                Error,
                TEXT("CreateSession: Could not create session modification handle: %s"),
                ANSI_TO_TCHAR(EOS_EResult_ToString(ModResult)));
            return false;
        }

        EOS_EResult ApplyResult =
            this->ApplySettingsToModificationHandle(EOSHostingUser, NewSessionSettings, ModHandle, nullptr);
        if (ApplyResult != EOS_EResult::EOS_Success)
        {
            RemoveNamedSession(SessionName);
            UE_LOG(
                LogEOS,
                Error,
                TEXT("CreateSession: Could not apply settings to session handle (%s)"),
                ANSI_TO_TCHAR(EOS_EResult_ToString(ApplyResult)));
            return false;
        }

        EOS_Sessions_UpdateSessionOptions UpdateOpts = {};
        UpdateOpts.ApiVersion = EOS_SESSIONS_UPDATESESSION_API_LATEST;
        UpdateOpts.SessionModificationHandle = ModHandle;
        EOSRunOperation<EOS_HSessions, EOS_Sessions_UpdateSessionOptions, EOS_Sessions_UpdateSessionCallbackInfo>(
            this->EOSSessions,
            &UpdateOpts,
            EOS_Sessions_UpdateSession,
            [WeakThis = GetWeakThis(this), ModHandle, SessionName, Session, EOSHostingUser](
                const EOS_Sessions_UpdateSessionCallbackInfo *Info) {
                EOS_SessionModification_Release(ModHandle);

                if (auto This = PinWeakThis(WeakThis))
                {
                    if (Info->ResultCode != EOS_EResult::EOS_Success)
                    {
                        This->RemoveNamedSession(SessionName);
                        UE_LOG(
                            LogEOS,
                            Error,
                            TEXT("CreateSession: Failed with error %s"),
                            ANSI_TO_TCHAR(EOS_EResult_ToString(Info->ResultCode)));
                        This->TriggerOnCreateSessionCompleteDelegates(SessionName, false);
                        return;
                    }

                    auto SessionId = MakeShared<FUniqueNetIdEOSSession>(ANSI_TO_TCHAR(Info->SessionId));

                    std::function<void(const FOnlineError &Error)> Finalize = [WeakThis = GetWeakThis(This),
                                                                               Session,
                                                                               SessionId,
                                                                               SessionName,
                                                                               Info,
                                                                               EOSHostingUser](
                                                                                  const FOnlineError &Error) {
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            if (!Error.bSucceeded)
                            {
                                UE_LOG(
                                    LogEOS,
                                    Warning,
                                    TEXT("CreateSession: One or more optional operations failed: %s"),
                                    *Error.ToLogString());
                            }

                            UE_LOG(
                                LogEOS,
                                Verbose,
                                TEXT("CreateSession: Successfully created session '%s' with ID '%s'"),
                                *SessionName.ToString(),
                                *SessionId->ToString());
                            Session->SessionState = EOnlineSessionState::Pending;
                            Session->SessionInfo = MakeShared<FOnlineSessionInfoEOS>(SessionId);
                            This->TriggerOnCreateSessionCompleteDelegates(SessionName, true);
                            This->MetricsSend_BeginPlayerSession(*EOSHostingUser, Session->GetSessionIdStr(), TEXT(""));
                        }
                    };

#if EOS_HAS_ORCHESTRATORS
                    if (This->ServerOrchestrator.IsValid())
                    {
                        Finalize = [WeakThis = GetWeakThis(This),
                                    Finalize,
                                    SessionName,
                                    SessionId = SessionId->ToString()](const FOnlineError &Error) {
                            if (auto This = PinWeakThis(WeakThis))
                            {
                                if (This->ServerOrchestrator.IsValid())
                                {
                                    This->ServerOrchestrator->ServingTraffic(
                                        SessionName,
                                        SessionId,
                                        FServerOrchestratorServingTrafficComplete::CreateLambda([Finalize, Error]() {
                                            Finalize(Error);
                                        }));
                                }
                                else
                                {
                                    Finalize(Error);
                                }
                            }
                        };
                    }
#endif // #if EOS_HAS_ORCHESTRATORS

#if EOS_HAS_AUTHENTICATION
                    if (This->ShouldHaveSyntheticSession(Session->SessionSettings))
                    {
                        if (TSharedPtr<FSyntheticPartySessionManager> SPM = This->SyntheticPartySessionManager.Pin())
                        {
                            SPM->CreateSyntheticSession(
                                SessionId,
                                FSyntheticPartySessionOnComplete::CreateLambda(Finalize));
                        }
                        else
                        {
                            Finalize(OnlineRedpointEOS::Errors::Success());
                        }
                    }
                    else
                    {
                        Finalize(OnlineRedpointEOS::Errors::Success());
                    }
#else
                    Finalize(OnlineRedpointEOS::Errors::Success());
#endif // #if EOS_HAS_AUTHENTICATION

                    return;
                }
            });
        return true;
    }
    else
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("CreateSession: Failed because a session with the name %s already exists."),
            *SessionName.ToString());
        return false;
    }
}

bool FOnlineSessionInterfaceEOS::StartSession(FName SessionName)
{
    FNamedOnlineSession *Session = this->GetNamedSession(SessionName);
    if (Session == nullptr)
    {
        UE_LOG(LogEOS, Error, TEXT("StartSession: Called with non-existant session."));
        return false;
    }

    auto SessionNameAnsi = EOSString_SessionModification_SessionName::ToAnsiString(SessionName.ToString());

    EOS_Sessions_StartSessionOptions Opts = {};
    Opts.ApiVersion = EOS_SESSIONS_STARTSESSION_API_LATEST;
    Opts.SessionName = SessionNameAnsi.Ptr.Get();

    EOSRunOperation<EOS_HSessions, EOS_Sessions_StartSessionOptions, EOS_Sessions_StartSessionCallbackInfo>(
        this->EOSSessions,
        &Opts,
        EOS_Sessions_StartSession,
        [WeakThis = GetWeakThis(this), SessionName](const EOS_Sessions_StartSessionCallbackInfo *Info) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Info->ResultCode != EOS_EResult::EOS_Success)
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("StartSession: Failed with error %s"),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Info->ResultCode)));
                }

#if EOS_HAS_ORCHESTRATORS
                if (This->ServerOrchestrator.IsValid() && Info->ResultCode == EOS_EResult::EOS_Success)
                {
                    This->ServerOrchestrator->SessionStarted(FServerOrchestratorSessionStartedComplete::CreateLambda(
                        [WeakThis = GetWeakThis(This), SessionName, ResultCode = Info->ResultCode]() {
                            if (auto This = PinWeakThis(WeakThis))
                            {
                                This->TriggerOnStartSessionCompleteDelegates(
                                    SessionName,
                                    ResultCode == EOS_EResult::EOS_Success);
                            }
                        }));
                }
                else
                {
                    This->TriggerOnStartSessionCompleteDelegates(
                        SessionName,
                        Info->ResultCode == EOS_EResult::EOS_Success);
                }
#else
                This->TriggerOnStartSessionCompleteDelegates(SessionName, Info->ResultCode == EOS_EResult::EOS_Success);
#endif // #if EOS_HAS_ORCHESTRATORS
            }
        });
    return true;
}

bool FOnlineSessionInterfaceEOS::UpdateSession(
    FName SessionName,
    FOnlineSessionSettings &UpdatedSessionSettings,
    bool bShouldRefreshOnlineData)
{
#if EOS_HAS_ORCHESTRATORS
    if (this->ServerOrchestrator.IsValid())
    {
        // Get the port definitions from the server orchestrator, and update the session settings
        // based on it.
        this->ServerOrchestrator->GetPortMappings(FServerOrchestratorGetPortMappingsComplete::CreateLambda(
            [WeakThis = GetWeakThis(this),
             SessionName,
             UpdatedSessionSettings = MakeShared<FOnlineSessionSettings>(UpdatedSessionSettings),
             bShouldRefreshOnlineData](const TArray<FServerOrchestratorPortMapping> &PortMappings) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    for (const auto &PortMapping : PortMappings)
                    {
                        if (PortMapping.GetPortName().IsEqual(NAME_GamePort) ||
                            PortMapping.GetPortName().IsEqual(FName("game")) ||
                            PortMapping.GetPortName().IsEqual(FName("default")))
                        {
                            UpdatedSessionSettings->Settings.Add(
                                EOS_WELL_KNOWN_ATTRIBUTE_OVERRIDEADDRESSBOUND,
                                FOnlineSessionSetting(
                                    FString::Printf(TEXT("0.0.0.0:%d"), PortMapping.GetPortValue()),
                                    EOnlineDataAdvertisementType::ViaOnlineService));
                        }
                        else
                        {
                            UpdatedSessionSettings->Settings.Add(
                                FName(*FString::Printf(TEXT("Port_%s"), *PortMapping.GetPortName().ToString())),
                                FOnlineSessionSetting(
                                    FString::Printf(TEXT("%d"), PortMapping.GetPortValue()),
                                    EOnlineDataAdvertisementType::ViaOnlineService));
                        }
                    }

                    This->UpdateSessionInternal(SessionName, *UpdatedSessionSettings, bShouldRefreshOnlineData);
                }
            }));
        return true;
    }
    else
    {
        return UpdateSessionInternal(SessionName, UpdatedSessionSettings, bShouldRefreshOnlineData);
    }
#else
    return UpdateSessionInternal(SessionName, UpdatedSessionSettings, bShouldRefreshOnlineData);
#endif // #if EOS_HAS_ORCHESTRATORS
}

bool FOnlineSessionInterfaceEOS::UpdateSessionInternal(
    FName SessionName,
    FOnlineSessionSettings &UpdatedSessionSettings,
    bool bShouldRefreshOnlineData)
{
    FNamedOnlineSession *Session = this->GetNamedSession(SessionName);
    if (Session == nullptr)
    {
        UE_LOG(LogEOS, Error, TEXT("UpdateSession: Called with non-existant session."));
        return false;
    }

    if (!Session->OwningUserId.IsValid() || Session->OwningUserId->GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("UpdateSession: Hosting user ID was invalid"));
        return false;
    }

    auto EOSHostingUser = StaticCastSharedRef<const FUniqueNetIdEOS>(Session->OwningUserId.ToSharedRef());
    if (!EOSHostingUser->HasValidProductUserId())
    {
        UE_LOG(LogEOS, Error, TEXT("UpdateSession: Hosting user ID was invalid"));
        return false;
    }

    auto SessionNameAnsi = EOSString_SessionModification_SessionName::ToAnsiString(SessionName.ToString());

    EOS_Sessions_UpdateSessionModificationOptions ModOpts = {};
    ModOpts.ApiVersion = EOS_SESSIONS_UPDATESESSIONMODIFICATION_API_LATEST;
    ModOpts.SessionName = SessionNameAnsi.Ptr.Get();

    EOS_HSessionModification ModHandle = {};
    if (EOS_Sessions_UpdateSessionModification(this->EOSSessions, &ModOpts, &ModHandle) != EOS_EResult::EOS_Success)
    {
        UE_LOG(LogEOS, Error, TEXT("UpdateSession: Could not create session modification handle"));
        return false;
    }

    EOS_EResult ApplyResult = this->ApplySettingsToModificationHandle(
        EOSHostingUser,
        UpdatedSessionSettings,
        ModHandle,
        &Session->SessionSettings);
    if (ApplyResult != EOS_EResult::EOS_Success)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("UpdateSession: Could not apply settings to session handle (%s)"),
            ANSI_TO_TCHAR(EOS_EResult_ToString(ApplyResult)));
        return false;
    }

    EOS_Sessions_UpdateSessionOptions UpdateOpts = {};
    UpdateOpts.ApiVersion = EOS_SESSIONS_UPDATESESSION_API_LATEST;
    UpdateOpts.SessionModificationHandle = ModHandle;
    EOSRunOperation<EOS_HSessions, EOS_Sessions_UpdateSessionOptions, EOS_Sessions_UpdateSessionCallbackInfo>(
        this->EOSSessions,
        &UpdateOpts,
        EOS_Sessions_UpdateSession,
        [WeakThis = GetWeakThis(this), ModHandle, SessionName, Session](
            const EOS_Sessions_UpdateSessionCallbackInfo *Info) {
            EOS_SessionModification_Release(ModHandle);

            if (auto This = PinWeakThis(WeakThis))
            {
                if (Info->ResultCode != EOS_EResult::EOS_Success)
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("UpdateSession: Failed with error %s"),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Info->ResultCode)));
                    This->TriggerOnUpdateSessionCompleteDelegates(SessionName, false);
                    return;
                }

                UE_LOG(
                    LogEOS,
                    Verbose,
                    TEXT("UpdateSession: Successfully updated session '%s' with ID '%s'"),
                    *SessionName.ToString(),
                    ANSI_TO_TCHAR(Info->SessionId));
                Session->SessionState = EOnlineSessionState::Pending;
                Session->SessionInfo = MakeShared<FOnlineSessionInfoEOS>(
                    MakeShared<FUniqueNetIdEOSSession>(ANSI_TO_TCHAR(Info->SessionId)));
                This->TriggerOnUpdateSessionCompleteDelegates(SessionName, true);
                return;
            }
        });
    return true;
}

bool FOnlineSessionInterfaceEOS::EndSession(FName SessionName)
{
#if EOS_HAS_ORCHESTRATORS
    if (this->ServerOrchestrator.IsValid())
    {
        if (this->ServerOrchestrator->ShouldDestroySessionOnEndSession())
        {
            UE_LOG(
                LogEOS,
                Verbose,
                TEXT("Forwarding EndSession call to DestroySession, as this is required by the current server "
                     "orchestrator."));
            return this->DestroySession(
                SessionName,
                FOnDestroySessionCompleteDelegate::CreateLambda(
                    [WeakThis = GetWeakThis(this), SessionName](FName DestroyedSessionName, bool bWasSuccessful) {
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            if (SessionName == DestroyedSessionName)
                            {
                                // Forward EndSession event so that OSB nodes work correctly.
                                This->TriggerOnEndSessionCompleteDelegates(DestroyedSessionName, bWasSuccessful);
                            }
                        }
                    }));
        }
    }
#endif

    FNamedOnlineSession *Session = this->GetNamedSession(SessionName);
    if (Session == nullptr)
    {
        UE_LOG(LogEOS, Error, TEXT("EndSession: Called with non-existant session."));
        return false;
    }

    auto SessionNameAnsi = EOSString_SessionModification_SessionName::ToAnsiString(SessionName.ToString());

    EOS_Sessions_EndSessionOptions Opts = {};
    Opts.ApiVersion = EOS_SESSIONS_ENDSESSION_API_LATEST;
    Opts.SessionName = SessionNameAnsi.Ptr.Get();

    EOSRunOperation<EOS_HSessions, EOS_Sessions_EndSessionOptions, EOS_Sessions_EndSessionCallbackInfo>(
        this->EOSSessions,
        &Opts,
        EOS_Sessions_EndSession,
        [WeakThis = GetWeakThis(this), SessionName](const EOS_Sessions_EndSessionCallbackInfo *Info) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Info->ResultCode != EOS_EResult::EOS_Success)
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("EndSession: Failed with error %s"),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Info->ResultCode)));
                }

#if EOS_HAS_ORCHESTRATORS
                if (This->ServerOrchestrator.IsValid() && Info->ResultCode == EOS_EResult::EOS_Success)
                {
                    This->ServerOrchestrator->Shutdown(FServerOrchestratorShutdownComplete::CreateLambda(
                        [WeakThis = GetWeakThis(This), SessionName, ResultCode = Info->ResultCode]() {
                            if (auto This = PinWeakThis(WeakThis))
                            {
                                This->TriggerOnEndSessionCompleteDelegates(
                                    SessionName,
                                    ResultCode == EOS_EResult::EOS_Success);
                            }
                        }));
                }
                else
                {
                    This->TriggerOnEndSessionCompleteDelegates(
                        SessionName,
                        Info->ResultCode == EOS_EResult::EOS_Success);
                }
#else
                This->TriggerOnEndSessionCompleteDelegates(SessionName, Info->ResultCode == EOS_EResult::EOS_Success);
#endif // #if EOS_HAS_ORCHESTRATORS
            }
        });
    return true;
}

bool FOnlineSessionInterfaceEOS::DestroySession(
    FName SessionName,
    const FOnDestroySessionCompleteDelegate &CompletionDelegate)
{
    FNamedOnlineSession *Session = this->GetNamedSession(SessionName);
    if (Session == nullptr)
    {
        UE_LOG(LogEOS, Error, TEXT("DestroySession: Called with non-existant session."));
        return false;
    }

    auto SessionNameAnsi = EOSString_SessionModification_SessionName::ToAnsiString(SessionName.ToString());

    EOS_Sessions_DestroySessionOptions Opts = {};
    Opts.ApiVersion = EOS_SESSIONS_DESTROYSESSION_API_LATEST;
    Opts.SessionName = SessionNameAnsi.Ptr.Get();

    TSharedPtr<const FUniqueNetIdEOS> UserIdEOS = StaticCastSharedPtr<const FUniqueNetIdEOS>(Session->LocalOwnerId);
    auto SessionId = StaticCastSharedRef<const FUniqueNetIdEOSSession>(Session->SessionInfo->GetSessionId().AsShared());

    EOSRunOperation<EOS_HSessions, EOS_Sessions_DestroySessionOptions, EOS_Sessions_DestroySessionCallbackInfo>(
        this->EOSSessions,
        &Opts,
        EOS_Sessions_DestroySession,
        [WeakThis = GetWeakThis(this), SessionName, SessionId, UserIdEOS, CompletionDelegate](
            const EOS_Sessions_DestroySessionCallbackInfo *Info) {
            if (auto This = PinWeakThis(WeakThis))
            {
                This->MetricsSend_EndPlayerSession(*UserIdEOS);

                if (Info->ResultCode != EOS_EResult::EOS_Success)
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("DestroySession: Failed with error %s"),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Info->ResultCode)));
                }

                std::function<void(const FOnlineError &)> Finalize = [WeakThis = GetWeakThis(This),
                                                                      SessionName,
                                                                      ResultCode = Info->ResultCode,
                                                                      CompletionDelegate](const FOnlineError &Error) {
                    if (auto This = PinWeakThis(WeakThis))
                    {
                        if (!Error.bSucceeded)
                        {
                            UE_LOG(
                                LogEOS,
                                Warning,
                                TEXT("DestroySession: One or more optional operations failed: %s"),
                                *Error.ToLogString());
                        }

                        This->RemoveNamedSession(SessionName);
                        This->TriggerOnDestroySessionCompleteDelegates(
                            SessionName,
                            ResultCode == EOS_EResult::EOS_Success);
                        CompletionDelegate.ExecuteIfBound(SessionName, ResultCode == EOS_EResult::EOS_Success);
                    }
                };

#if EOS_HAS_ORCHESTRATORS
                if (This->ServerOrchestrator.IsValid())
                {
                    Finalize = [Finalize, WeakThis = GetWeakThis(This)](const FOnlineError &Error) {
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            This->ServerOrchestrator->Shutdown(FServerOrchestratorShutdownComplete::CreateLambda(
                                [WeakThis = GetWeakThis(This), Finalize, Error]() {
                                    if (auto This = PinWeakThis(WeakThis))
                                    {
                                        Finalize(Error);
                                    }
                                }));
                        }
                    };
                }
#endif // #if EOS_HAS_ORCHESTRATORS

#if EOS_HAS_AUTHENTICATION
                if (auto SPM = This->SyntheticPartySessionManager.Pin())
                {
                    if (SPM->HasSyntheticSession(SessionId))
                    {
                        SPM->DeleteSyntheticSession(
                            SessionId,
                            FSyntheticPartySessionOnComplete::CreateLambda(Finalize));
                    }
                    else
                    {
                        Finalize(OnlineRedpointEOS::Errors::Success());
                    }
                }
                else
                {
                    Finalize(OnlineRedpointEOS::Errors::Success());
                }
#else
                Finalize(OnlineRedpointEOS::Errors::Success());
#endif // #if EOS_HAS_AUTHENTICATION
            }
        });
    return true;
}

bool FOnlineSessionInterfaceEOS::IsPlayerInSession(FName SessionName, const FUniqueNetId &UniqueId)
{
    FNamedOnlineSession *Session = this->GetNamedSession(SessionName);
    if (Session == nullptr)
    {
        UE_LOG(LogEOS, Error, TEXT("IsPlayerInSession: Called with non-existant session."));
        return false;
    }

    if (UniqueId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("IsPlayerInSession: Target user ID was invalid"));
        return false;
    }

    auto EOSTargetUser = StaticCastSharedRef<const FUniqueNetIdEOS>(UniqueId.AsShared());
    if (!EOSTargetUser->HasValidProductUserId())
    {
        UE_LOG(LogEOS, Error, TEXT("IsPlayerInSession: Target user ID was invalid"));
        return false;
    }

    auto SessionNameAnsi = EOSString_SessionModification_SessionName::ToAnsiString(SessionName.ToString());

    EOS_Sessions_IsUserInSessionOptions Opts = {};
    Opts.ApiVersion = EOS_SESSIONS_ISUSERINSESSION_API_LATEST;
    Opts.SessionName = SessionNameAnsi.Ptr.Get();
    Opts.TargetUserId = EOSTargetUser->GetProductUserId();

    EOS_EResult Result = EOS_Sessions_IsUserInSession(this->EOSSessions, &Opts);
    switch (Result)
    {
    case EOS_EResult::EOS_Success:
        return true;
    case EOS_EResult::EOS_NotFound:
        return false;
    default:
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("IsPlayerInSession: Unexpected error %s"),
            ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
        return false;
    }
}

bool FOnlineSessionInterfaceEOS::StartMatchmaking(
    const TArray<TSharedRef<const FUniqueNetId>> &LocalPlayers,
    FName SessionName,
    const FOnlineSessionSettings &NewSessionSettings,
    TSharedRef<FOnlineSessionSearch> &SearchSettings)
{
    UE_LOG(
        LogEOS,
        Warning,
        TEXT("StartMatchmaking is not supported on this platform. Use FindSessions or FindSessionById."));
    this->TriggerOnMatchmakingCompleteDelegates(SessionName, false);
    return false;
}

bool FOnlineSessionInterfaceEOS::CancelMatchmaking(int32 SearchingPlayerNum, FName SessionName)
{
    auto Id = this->Identity->GetUniquePlayerId(SearchingPlayerNum);
    if (!Id.IsValid())
    {
        UE_LOG(LogEOS, Error, TEXT("SearchingPlayerNum provided to CancelMatchmaking does not have online identity."));
        return false;
    }

    return this->CancelMatchmaking(Id.ToSharedRef().Get(), SessionName);
}

bool FOnlineSessionInterfaceEOS::CancelMatchmaking(const FUniqueNetId &SearchingPlayerId, FName SessionName)
{
    UE_LOG(LogEOS, Warning, TEXT("CancelMatchmaking is not supported on this platform. Use CancelFindSessions."));
    this->TriggerOnCancelMatchmakingCompleteDelegates(SessionName, false);
    return false;
}

bool FOnlineSessionInterfaceEOS::FindSessions(
    int32 SearchingPlayerNum,
    const TSharedRef<FOnlineSessionSearch> &SearchSettings)
{
    auto Id = this->Identity->GetUniquePlayerId(SearchingPlayerNum);
    if (!Id.IsValid())
    {
        UE_LOG(LogEOS, Error, TEXT("SearchingPlayerNum provided to FindSessions does not have online identity."));
        return false;
    }

    return this->FindSessions(Id.ToSharedRef().Get(), SearchSettings);
}

bool FOnlineSessionInterfaceEOS::FindSessions(
    const FUniqueNetId &SearchingPlayerId,
    const TSharedRef<FOnlineSessionSearch> &SearchSettings)
{
    SearchSettings->SearchState = EOnlineAsyncTaskState::NotStarted;

    if (SearchingPlayerId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        SearchSettings->SearchState = EOnlineAsyncTaskState::Failed;
        UE_LOG(LogEOS, Error, TEXT("FindSessions: Searching user ID was invalid"));
        return false;
    }

    auto EOSSearchingUser = StaticCastSharedRef<const FUniqueNetIdEOS>(SearchingPlayerId.AsShared());
    if (!EOSSearchingUser->HasValidProductUserId())
    {
        SearchSettings->SearchState = EOnlineAsyncTaskState::Failed;
        UE_LOG(LogEOS, Error, TEXT("FindSessions: Searching user ID was invalid"));
        return false;
    }

    EOS_Sessions_CreateSessionSearchOptions SearchOpts = {};
    SearchOpts.ApiVersion = EOS_SESSIONS_CREATESESSIONSEARCH_API_LATEST;
    SearchOpts.MaxSearchResults = SearchSettings->MaxSearchResults;

    EOS_HSessionSearch SearchHandle = {};
    if (EOS_Sessions_CreateSessionSearch(this->EOSSessions, &SearchOpts, &SearchHandle) != EOS_EResult::EOS_Success)
    {
        SearchSettings->SearchState = EOnlineAsyncTaskState::Failed;
        UE_LOG(LogEOS, Error, TEXT("FindSessions: Failed to create search handle"));
        return false;
    }

    bool bDidFilterListening = false;
    bool bDidFilterMinSlotsAvailable = false;

    for (const auto &Filter : SearchSettings->QuerySettings.SearchParams)
    {
#if defined(UE_4_27_OR_LATER)
        if (Filter.Key.IsEqual(SEARCH_PRESENCE) || Filter.Key.IsEqual(SEARCH_LOBBIES))
#else
        if (Filter.Key.IsEqual(SEARCH_PRESENCE))
#endif
        {
            // These are magic search keys that are only used by the Steam subsystem and
            // the Epic implementation of EOS. Ignore them in our plugin so that example
            // code written by Epic (Lyra) is able to search sessions correctly.
            continue;
        }

        if (Filter.Key.ToString() == EOS_WELL_KNOWN_ATTRIBUTE_BLISTENING)
        {
            bDidFilterListening = true;
        }

        if (Filter.Key.ToString() == EOS_SESSIONS_SEARCH_MINSLOTSAVAILABLE)
        {
            bDidFilterMinSlotsAvailable = true;
        }

        EOS_SessionSearch_SetParameterOptions ParamOpts = {};
        ParamOpts.ApiVersion = EOS_SESSIONSEARCH_SETPARAMETER_API_LATEST;

        // Set comparison operator.
        switch (Filter.Value.ComparisonOp)
        {
        case EOnlineComparisonOp::Equals:
            ParamOpts.ComparisonOp = EOS_EOnlineComparisonOp::EOS_CO_EQUAL;
            break;
        case EOnlineComparisonOp::NotEquals:
            ParamOpts.ComparisonOp = EOS_EOnlineComparisonOp::EOS_CO_NOTEQUAL;
            break;
        case EOnlineComparisonOp::GreaterThan:
            ParamOpts.ComparisonOp = EOS_EOnlineComparisonOp::EOS_CO_GREATERTHAN;
            break;
        case EOnlineComparisonOp::GreaterThanEquals:
            ParamOpts.ComparisonOp = EOS_EOnlineComparisonOp::EOS_CO_GREATERTHANOREQUAL;
            break;
        case EOnlineComparisonOp::LessThan:
            ParamOpts.ComparisonOp = EOS_EOnlineComparisonOp::EOS_CO_LESSTHAN;
            break;
        case EOnlineComparisonOp::LessThanEquals:
            ParamOpts.ComparisonOp = EOS_EOnlineComparisonOp::EOS_CO_LESSTHANOREQUAL;
            break;
        case EOnlineComparisonOp::Near:
            ParamOpts.ComparisonOp = EOS_EOnlineComparisonOp::EOS_CO_DISTANCE;
            break;
        case EOnlineComparisonOp::In:
            ParamOpts.ComparisonOp = EOS_EOnlineComparisonOp::EOS_CO_ANYOF;
            break;
        case EOnlineComparisonOp::NotIn:
            ParamOpts.ComparisonOp = EOS_EOnlineComparisonOp::EOS_CO_NOTANYOF;
            break;
        default:
            SearchSettings->SearchState = EOnlineAsyncTaskState::Failed;
            UE_LOG(
                LogEOS,
                Error,
                TEXT("FindSessions: Invalid comparison operation for search parameter %s"),
                *Filter.Key.ToString());
            return false;
        }

        auto KeyStr = EOSString_SessionModification_AttributeKey::ToAnsiString(Filter.Key.ToString());
        auto ValueStr = EOSString_SessionModification_AttributeStringValue::ToUtf8String(Filter.Value.Data.ToString());

        // Set attribute value.
        EOS_Sessions_AttributeData Attribute = {};
        Attribute.ApiVersion = EOS_SESSIONS_SESSIONATTRIBUTEDATA_API_LATEST;
        Attribute.Key = KeyStr.Ptr.Get();
        switch (Filter.Value.Data.GetType())
        {
        case EOnlineKeyValuePairDataType::Bool:
            Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_BOOLEAN;
            bool BoolVal;
            Filter.Value.Data.GetValue(BoolVal);
            Attribute.Value.AsBool = BoolVal ? EOS_TRUE : EOS_FALSE;
            break;
        case EOnlineKeyValuePairDataType::Int64:
            Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_INT64;
            int64 Int64Val;
            Filter.Value.Data.GetValue(Int64Val);
            Attribute.Value.AsInt64 = Int64Val;
            break;
        case EOnlineKeyValuePairDataType::Double:
            Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_DOUBLE;
            Filter.Value.Data.GetValue(Attribute.Value.AsDouble);
            break;
        case EOnlineKeyValuePairDataType::Float:
            Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_DOUBLE;
            {
                float FloatTmp;
                Filter.Value.Data.GetValue(FloatTmp);
                Attribute.Value.AsDouble = FloatTmp;
            }
            break;
        case EOnlineKeyValuePairDataType::String:
            Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_STRING;
            Attribute.Value.AsUtf8 = ValueStr.GetAsChar();
            break;
        case EOnlineKeyValuePairDataType::Empty:
            if (Filter.Key.ToString() == EOS_WELL_KNOWN_ATTRIBUTE_BLISTENING)
            {
                // Developer wants to include both listening and non-listening sessions.
                break;
            }
            else
            {
                // Fallthrough as this is unhandled for anything other than EOS_WELL_KNOWN_ATTRIBUTE_BLISTENING.
            }
        default:
            SearchSettings->SearchState = EOnlineAsyncTaskState::Failed;
            UE_LOG(
                LogEOS,
                Error,
                TEXT("FindSessions: Unsupported data type %s for search parameter %s"),
                Filter.Value.Data.GetTypeString(),
                *Filter.Key.ToString());
            return false;
        }

        ParamOpts.Parameter = &Attribute;

        // Set search parameter.
        EOS_EResult SetParamResult = EOS_SessionSearch_SetParameter(SearchHandle, &ParamOpts);
        if (SetParamResult != EOS_EResult::EOS_Success)
        {
            SearchSettings->SearchState = EOnlineAsyncTaskState::Failed;
            UE_LOG(
                LogEOS,
                Error,
                TEXT("FindSessions: Failed to set search parameter %s due to error %s"),
                *Filter.Key.ToString(),
                ANSI_TO_TCHAR(EOS_EResult_ToString(SetParamResult)));
            return false;
        }
    }

    if (!bDidFilterListening)
    {
        // Exclude non-listening sessions from search results.
        EOS_Sessions_AttributeData Attribute = {};
        Attribute.ApiVersion = EOS_SESSIONS_SESSIONATTRIBUTEDATA_API_LATEST;
        Attribute.Key = EOS_WELL_KNOWN_ATTRIBUTE_BLISTENING;
        Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_BOOLEAN;
        Attribute.Value.AsBool = EOS_TRUE;

        EOS_SessionSearch_SetParameterOptions ParamOpts = {};
        ParamOpts.ApiVersion = EOS_SESSIONSEARCH_SETPARAMETER_API_LATEST;
        ParamOpts.Parameter = &Attribute;
        ParamOpts.ComparisonOp = EOS_EOnlineComparisonOp::EOS_CO_EQUAL;

        EOS_EResult SetParamResult = EOS_SessionSearch_SetParameter(SearchHandle, &ParamOpts);
        if (SetParamResult != EOS_EResult::EOS_Success)
        {
            SearchSettings->SearchState = EOnlineAsyncTaskState::Failed;
            UE_LOG(
                LogEOS,
                Error,
                TEXT("FindSessions: Unable to set listening filter (got result %s)"),
                ANSI_TO_TCHAR(EOS_EResult_ToString(SetParamResult)));
            return false;
        }
    }

    if (!bDidFilterMinSlotsAvailable &&
        EOS_ApiVersionIsAtLeast(this->Config->GetApiVersion(), EEOSApiVersion::v2022_02_11))
    {
        // Require at least one slot by default. This has long been assumed to be the behaviour, based on
        // the comments in the EOS SDK. But in actuality, if you don't specify "minslotsavailable" as a
        // filter, the SDK will return you sessions that are full.
        //
        // Make our behaviour match the expectation of users.
        EOS_Sessions_AttributeData Attribute = {};
        Attribute.ApiVersion = EOS_SESSIONS_SESSIONATTRIBUTEDATA_API_LATEST;
        Attribute.Key = EOS_SESSIONS_SEARCH_MINSLOTSAVAILABLE;
        Attribute.ValueType = EOS_ESessionAttributeType::EOS_AT_INT64;
        Attribute.Value.AsInt64 = 1;

        EOS_SessionSearch_SetParameterOptions ParamOpts = {};
        ParamOpts.ApiVersion = EOS_SESSIONSEARCH_SETPARAMETER_API_LATEST;
        ParamOpts.Parameter = &Attribute;
        ParamOpts.ComparisonOp = EOS_EOnlineComparisonOp::EOS_CO_GREATERTHANOREQUAL;

        EOS_EResult SetParamResult = EOS_SessionSearch_SetParameter(SearchHandle, &ParamOpts);
        if (SetParamResult != EOS_EResult::EOS_Success)
        {
            SearchSettings->SearchState = EOnlineAsyncTaskState::Failed;
            UE_LOG(
                LogEOS,
                Error,
                TEXT("FindSessions: Unable to set minslotsavailable filter (got result %s)"),
                ANSI_TO_TCHAR(EOS_EResult_ToString(SetParamResult)));
            return false;
        }
    }

    SearchSettings->SearchState = EOnlineAsyncTaskState::InProgress;

    EOS_SessionSearch_FindOptions Opts = {};
    Opts.ApiVersion = EOS_SESSIONSEARCH_FIND_API_LATEST;
    Opts.LocalUserId = EOSSearchingUser->GetProductUserId();

    EOSRunOperation<EOS_HSessionSearch, EOS_SessionSearch_FindOptions, EOS_SessionSearch_FindCallbackInfo>(
        SearchHandle,
        &Opts,
        EOS_SessionSearch_Find,
        [WeakThis = GetWeakThis(this), SearchHandle, SearchSettings](const EOS_SessionSearch_FindCallbackInfo *Info) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Info->ResultCode != EOS_EResult::EOS_Success)
                {
                    EOS_SessionSearch_Release(SearchHandle);
                    SearchSettings->SearchState = EOnlineAsyncTaskState::Failed;
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("FindSessions: Search failed with result code %s"),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Info->ResultCode)));
                    This->TriggerOnFindSessionsCompleteDelegates(false);
                    return;
                }

                // Get the number of results.
                EOS_SessionSearch_GetSearchResultCountOptions CountOpts = {};
                CountOpts.ApiVersion = EOS_SESSIONSEARCH_GETSEARCHRESULTCOUNT_API_LATEST;
                uint32_t ResultCount = EOS_SessionSearch_GetSearchResultCount(SearchHandle, &CountOpts);

                // Iterate through the search results and copy them all into the search settings.
                for (uint32_t i = 0; i < ResultCount; i++)
                {
                    EOS_SessionSearch_CopySearchResultByIndexOptions CopyOpts = {};
                    CopyOpts.ApiVersion = EOS_SESSIONSEARCH_COPYSEARCHRESULTBYINDEX_API_LATEST;
                    CopyOpts.SessionIndex = i;

                    EOS_HSessionDetails ResultHandle = {};
                    if (EOS_SessionSearch_CopySearchResultByIndex(SearchHandle, &CopyOpts, &ResultHandle) !=
                        EOS_EResult::EOS_Success)
                    {
                        EOS_SessionSearch_Release(SearchHandle);
                        SearchSettings->SearchState = EOnlineAsyncTaskState::Failed;
                        UE_LOG(LogEOS, Error, TEXT("FindSessions: Failed to copy search result at index %u"), i);
                        This->TriggerOnFindSessionsCompleteDelegates(false);
                        return;
                    }

                    // Add the result.
                    SearchSettings->SearchResults.Add(FOnlineSessionSearchResultEOS::CreateFromDetails(ResultHandle));
                }

                EOS_SessionSearch_Release(SearchHandle);
                SearchSettings->SearchState = EOnlineAsyncTaskState::Done;
                This->TriggerOnFindSessionsCompleteDelegates(true);
            }
        });
    return true;
}

bool FOnlineSessionInterfaceEOS::FindSessionById(
    const FUniqueNetId &SearchingUserId,
    const FUniqueNetId &SessionId,
    const FUniqueNetId &FriendId,
    const FOnSingleSessionResultCompleteDelegate &CompletionDelegate)
{
    if (SearchingUserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("FindSessionById: Searching user ID was invalid"));
        return false;
    }

    auto EOSSearchingUser = StaticCastSharedRef<const FUniqueNetIdEOS>(SearchingUserId.AsShared());
    if (!EOSSearchingUser->HasValidProductUserId())
    {
        UE_LOG(LogEOS, Error, TEXT("FindSessionById: Searching user ID was invalid"));
        return false;
    }

    if (SessionId.GetType() != REDPOINT_EOS_SUBSYSTEM_SESSION)
    {
        UE_LOG(LogEOS, Error, TEXT("FindSessionById: Session ID was invalid"));
        return false;
    }

    auto EOSSessionId = StaticCastSharedRef<const FUniqueNetIdEOSSession>(SessionId.AsShared());
    if (!EOSSessionId->IsValid())
    {
        UE_LOG(LogEOS, Error, TEXT("FindSessionById: Session ID was invalid"));
        return false;
    }

    EOS_Sessions_CreateSessionSearchOptions SearchOpts = {};
    SearchOpts.ApiVersion = EOS_SESSIONS_CREATESESSIONSEARCH_API_LATEST;
    SearchOpts.MaxSearchResults = 1;

    EOS_HSessionSearch SearchHandle = {};
    if (EOS_Sessions_CreateSessionSearch(this->EOSSessions, &SearchOpts, &SearchHandle) != EOS_EResult::EOS_Success)
    {
        UE_LOG(LogEOS, Error, TEXT("FindSessionById: Failed to create search handle"));
        return false;
    }

    auto SessionIdRaw = EOSString_SessionModification_SessionId::ToAnsiString(EOSSessionId->GetSessionId());

    EOS_SessionSearch_SetSessionIdOptions IdOpts = {};
    IdOpts.ApiVersion = EOS_SESSIONSEARCH_SETSESSIONID_API_LATEST;
    IdOpts.SessionId = SessionIdRaw.Ptr.Get();

    if (EOS_SessionSearch_SetSessionId(SearchHandle, &IdOpts) != EOS_EResult::EOS_Success)
    {
        EOS_SessionSearch_Release(SearchHandle);
        UE_LOG(LogEOS, Error, TEXT("FindSessionById: Failed to set session ID for search"));
        return false;
    }

    EOS_SessionSearch_FindOptions Opts = {};
    Opts.ApiVersion = EOS_SESSIONSEARCH_FIND_API_LATEST;
    Opts.LocalUserId = EOSSearchingUser->GetProductUserId();

    EOSRunOperation<EOS_HSessionSearch, EOS_SessionSearch_FindOptions, EOS_SessionSearch_FindCallbackInfo>(
        SearchHandle,
        &Opts,
        EOS_SessionSearch_Find,
        [WeakThis = GetWeakThis(this), CompletionDelegate, SearchHandle, EOSSearchingUser](
            const EOS_SessionSearch_FindCallbackInfo *Info) {
            if (auto This = PinWeakThis(WeakThis))
            {
                int32 LocalUserNum = 0;
                if (!StaticCastSharedPtr<FOnlineIdentityInterfaceEOS, IOnlineIdentity, ESPMode::ThreadSafe>(
                         This->Identity)
                         ->GetLocalUserNum(*EOSSearchingUser, LocalUserNum))
                {
                    UE_LOG(LogEOS, Error, TEXT("Searching user was not signed it at time of response"));
                    EOS_SessionSearch_Release(SearchHandle);
                    CompletionDelegate.ExecuteIfBound(
                        LocalUserNum,
                        false,
                        FOnlineSessionSearchResultEOS::CreateInvalid());
                    return;
                }

                EOS_SessionSearch_GetSearchResultCountOptions CountOpts = {};
                CountOpts.ApiVersion = EOS_SESSIONSEARCH_GETSEARCHRESULTCOUNT_API_LATEST;

                if (Info->ResultCode != EOS_EResult::EOS_Success ||
                    EOS_SessionSearch_GetSearchResultCount(SearchHandle, &CountOpts) != 1)
                {
                    EOS_SessionSearch_Release(SearchHandle);
                    CompletionDelegate.ExecuteIfBound(
                        LocalUserNum,
                        false,
                        FOnlineSessionSearchResultEOS::CreateInvalid());
                    return;
                }

                EOS_SessionSearch_CopySearchResultByIndexOptions CopyOpts = {};
                CopyOpts.ApiVersion = EOS_SESSIONSEARCH_COPYSEARCHRESULTBYINDEX_API_LATEST;
                CopyOpts.SessionIndex = 0;

                EOS_HSessionDetails ResultHandle = {};
                if (EOS_SessionSearch_CopySearchResultByIndex(SearchHandle, &CopyOpts, &ResultHandle) !=
                    EOS_EResult::EOS_Success)
                {
                    EOS_SessionSearch_Release(SearchHandle);
                    CompletionDelegate.ExecuteIfBound(
                        LocalUserNum,
                        false,
                        FOnlineSessionSearchResultEOS::CreateInvalid());
                    return;
                }

                EOS_SessionSearch_Release(SearchHandle);
                CompletionDelegate.ExecuteIfBound(
                    LocalUserNum,
                    true,
                    FOnlineSessionSearchResultEOS::CreateFromDetails(ResultHandle));
            }
        });
    return true;
}

bool FOnlineSessionInterfaceEOS::CancelFindSessions()
{
    UE_LOG(LogEOS, Error, TEXT("CancelFindSessions not implemented."));
    return false;
}

bool FOnlineSessionInterfaceEOS::PingSearchResults(const FOnlineSessionSearchResult &SearchResult)
{
    UE_LOG(LogEOS, Error, TEXT("PingSearchResults not implemented."));
    return false;
}

bool FOnlineSessionInterfaceEOS::JoinSession(
    int32 LocalUserNum,
    FName SessionName,
    const FOnlineSessionSearchResult &DesiredSession)
{
    auto Id = this->Identity->GetUniquePlayerId(LocalUserNum);
    if (!Id.IsValid())
    {
        UE_LOG(LogEOS, Error, TEXT("SearchingPlayerNum provided to FindSessions does not have online identity."));
        return false;
    }

    return this->JoinSession(Id.ToSharedRef().Get(), SessionName, DesiredSession);
}

bool FOnlineSessionInterfaceEOS::JoinSession(
    const FUniqueNetId &LocalUserId,
    FName SessionName,
    const FOnlineSessionSearchResult &DesiredSession)
{
    return this->JoinSession(LocalUserId, SessionName, DesiredSession, FOnJoinSessionCompleteDelegate());
}

bool FOnlineSessionInterfaceEOS::JoinSession(
    const FUniqueNetId &LocalUserId,
    FName SessionName,
    const FOnlineSessionSearchResult &DesiredSession,
    const FOnJoinSessionCompleteDelegate &OnComplete)
{
    if (!DesiredSession.Session.SessionInfo.IsValid())
    {
        auto Error = OnlineRedpointEOS::Errors::Sessions::InvalidSession(
            TEXT("FOnlineSessionInterfaceEOS::JoinSession"),
            TEXT("The DesiredSession parameter does not point to a valid session. If you got this session value from "
                 "FindSessionById, then the session was not found."));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Error.ToLogString());
        this->TriggerOnJoinSessionCompleteDelegates(SessionName, ConvertErrorTo_EOnJoinSessionCompleteResult(Error));
        OnComplete.ExecuteIfBound(SessionName, ConvertErrorTo_EOnJoinSessionCompleteResult(Error));
        return true;
    }

    FNamedOnlineSession *Session = this->GetNamedSession(SessionName);
    if (Session == nullptr)
    {
        Session = AddNamedSession(SessionName, DesiredSession.Session);
        Session->LocalOwnerId = LocalUserId.AsShared();
        Session->OwningUserId = nullptr;

        auto SessionNameAnsi = EOSString_SessionModification_SessionName::ToAnsiString(SessionName.ToString());

        auto EOSJoiningUser = StaticCastSharedRef<const FUniqueNetIdEOS>(LocalUserId.AsShared());
        if (!EOSJoiningUser->HasValidProductUserId())
        {
            RemoveNamedSession(SessionName);
            auto Error = OnlineRedpointEOS::Errors::InvalidUser(
                TEXT("FOnlineSessionInterfaceEOS::JoinSession"),
                TEXT("The local user ID was invalid."));
            UE_LOG(LogEOS, Error, TEXT("%s"), *Error.ToLogString());
            this->TriggerOnJoinSessionCompleteDelegates(
                SessionName,
                ConvertErrorTo_EOnJoinSessionCompleteResult(Error));
            OnComplete.ExecuteIfBound(SessionName, ConvertErrorTo_EOnJoinSessionCompleteResult(Error));
            return true;
        }

        auto SessionInfoEOS = StaticCastSharedPtr<FOnlineSessionInfoEOS>(DesiredSession.Session.SessionInfo);
        EOS_HSessionDetails Handle = SessionInfoEOS->GetHandle();

        EOS_Sessions_JoinSessionOptions Opts = {};
        Opts.ApiVersion = EOS_SESSIONS_JOINSESSION_API_LATEST;
        Opts.SessionName = SessionNameAnsi.Ptr.Get();
        Opts.SessionHandle = Handle;
        Opts.LocalUserId = EOSJoiningUser->GetProductUserId();
        if (this->Config->GetPresenceAdvertisementType() == EPresenceAdvertisementType::Session)
        {
            // This setting is inherited from the host of the session.
            Opts.bPresenceEnabled = DesiredSession.Session.SessionSettings.bUsesPresence;
        }
        else
        {
            Opts.bPresenceEnabled = false;
        }

        FString GameServerAddr;
        TSharedPtr<FOnlineSessionInfoEOS> EOSInfo = StaticCastSharedPtr<FOnlineSessionInfoEOS>(Session->SessionInfo);
        if (!EOSInfo->GetResolvedConnectString(GameServerAddr, NAME_GamePort))
        {
            GameServerAddr = TEXT("");
        }

        EOSRunOperation<EOS_HSessions, EOS_Sessions_JoinSessionOptions, EOS_Sessions_JoinSessionCallbackInfo>(
            this->EOSSessions,
            &Opts,
            EOS_Sessions_JoinSession,
            [WeakThis = GetWeakThis(this), SessionName, Session, EOSJoiningUser, GameServerAddr, OnComplete](
                const EOS_Sessions_JoinSessionCallbackInfo *Info) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    if (Info->ResultCode != EOS_EResult::EOS_Success)
                    {
                        This->RemoveNamedSession(SessionName);
                        auto Error = ConvertError(
                            TEXT("FOnlineSessionInterfaceEOS::JoinSession"),
                            TEXT("The local user ID was invalid."),
                            Info->ResultCode);
                        UE_LOG(LogEOS, Error, TEXT("%s"), *Error.ToLogString());
                        This->TriggerOnJoinSessionCompleteDelegates(
                            SessionName,
                            ConvertErrorTo_EOnJoinSessionCompleteResult(Error));
                        OnComplete.ExecuteIfBound(SessionName, ConvertErrorTo_EOnJoinSessionCompleteResult(Error));
                        return;
                    }

                    auto SessionId = StaticCastSharedRef<const FUniqueNetIdEOSSession>(
                        Session->SessionInfo->GetSessionId().AsShared());

                    auto Finalize = [WeakThis = GetWeakThis(This),
                                     SessionName,
                                     OnComplete,
                                     EOSJoiningUser,
                                     GameServerAddr,
                                     Session](const FOnlineError &Error) {
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            if (!Error.bSucceeded)
                            {
                                UE_LOG(
                                    LogEOS,
                                    Warning,
                                    TEXT("JoinSession: One or more optional operations failed: %s"),
                                    *Error.ToLogString());
                            }

                            UE_LOG(
                                LogEOS,
                                Verbose,
                                TEXT("JoinSession: Successfully joined session '%s'"),
                                *SessionName.ToString());
                            This->TriggerOnJoinSessionCompleteDelegates(
                                SessionName,
                                EOnJoinSessionCompleteResult::Success);
                            OnComplete.ExecuteIfBound(SessionName, EOnJoinSessionCompleteResult::Success);
                            This->MetricsSend_BeginPlayerSession(
                                *EOSJoiningUser,
                                Session->GetSessionIdStr(),
                                *GameServerAddr);
                        }
                    };

#if EOS_HAS_AUTHENTICATION
                    if (This->ShouldHaveSyntheticSession(Session->SessionSettings))
                    {
                        if (TSharedPtr<FSyntheticPartySessionManager> SPM = This->SyntheticPartySessionManager.Pin())
                        {
                            SPM->CreateSyntheticSession(
                                SessionId,
                                FSyntheticPartySessionOnComplete::CreateLambda(Finalize));
                        }
                        else
                        {
                            Finalize(OnlineRedpointEOS::Errors::Success());
                        }
                    }
                    else
                    {
                        Finalize(OnlineRedpointEOS::Errors::Success());
                    }
#else
                    Finalize(OnlineRedpointEOS::Errors::Success());
#endif // #if EOS_HAS_AUTHENTICATION

                    return;
                }
            });
        return true;
    }
    else
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("JoinSession: Failed because a session with the name %s already exists."),
            *SessionName.ToString());
        this->TriggerOnJoinSessionCompleteDelegates(SessionName, EOnJoinSessionCompleteResult::AlreadyInSession);
        OnComplete.ExecuteIfBound(SessionName, EOnJoinSessionCompleteResult::AlreadyInSession);
        return true;
    }
}

bool FOnlineSessionInterfaceEOS::FindFriendSession(int32 LocalUserNum, const FUniqueNetId &Friend)
{
    auto Id = this->Identity->GetUniquePlayerId(LocalUserNum);
    if (!Id.IsValid())
    {
        UE_LOG(LogEOS, Error, TEXT("LocalUserNum provided to FindFriendSession does not have online identity."));
        return false;
    }

    return this->FindFriendSession(Id.ToSharedRef().Get(), Friend);
}

bool FOnlineSessionInterfaceEOS::FindFriendSession(const FUniqueNetId &LocalUserId, const FUniqueNetId &Friend)
{
    if (LocalUserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("FindFriendSession: Local user ID was invalid"));
        return false;
    }

    auto EOSSearchingUser = StaticCastSharedRef<const FUniqueNetIdEOS>(LocalUserId.AsShared());
    if (!EOSSearchingUser->HasValidProductUserId())
    {
        UE_LOG(LogEOS, Error, TEXT("FindFriendSession: Local user ID was invalid"));
        return false;
    }

    if (Friend.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("FindFriendSession: Friend user ID was invalid"));
        return false;
    }

    auto EOSTargetUser = StaticCastSharedRef<const FUniqueNetIdEOS>(Friend.AsShared());
    if (!EOSTargetUser->HasValidProductUserId())
    {
        UE_LOG(LogEOS, Error, TEXT("FindFriendSession: Friend user ID was invalid"));
        return false;
    }

    EOS_Sessions_CreateSessionSearchOptions SearchOpts = {};
    SearchOpts.ApiVersion = EOS_SESSIONS_CREATESESSIONSEARCH_API_LATEST;
    SearchOpts.MaxSearchResults = 1;

    EOS_HSessionSearch SearchHandle = {};
    if (EOS_Sessions_CreateSessionSearch(this->EOSSessions, &SearchOpts, &SearchHandle) != EOS_EResult::EOS_Success)
    {
        UE_LOG(LogEOS, Error, TEXT("FindFriendSession: Failed to create search handle"));
        return false;
    }

    EOS_SessionSearch_SetTargetUserIdOptions IdOpts = {};
    IdOpts.ApiVersion = EOS_SESSIONSEARCH_SETTARGETUSERID_API_LATEST;
    IdOpts.TargetUserId = EOSTargetUser->GetProductUserId();

    if (EOS_SessionSearch_SetTargetUserId(SearchHandle, &IdOpts) != EOS_EResult::EOS_Success)
    {
        EOS_SessionSearch_Release(SearchHandle);
        UE_LOG(LogEOS, Error, TEXT("FindFriendSession: Failed to set target user ID for search"));
        return false;
    }

    EOS_SessionSearch_FindOptions Opts = {};
    Opts.ApiVersion = EOS_SESSIONSEARCH_FIND_API_LATEST;
    Opts.LocalUserId = EOSSearchingUser->GetProductUserId();

    EOSRunOperation<EOS_HSessionSearch, EOS_SessionSearch_FindOptions, EOS_SessionSearch_FindCallbackInfo>(
        SearchHandle,
        &Opts,
        EOS_SessionSearch_Find,
        [WeakThis = GetWeakThis(this), SearchHandle, EOSSearchingUser](const EOS_SessionSearch_FindCallbackInfo *Info) {
            if (auto This = PinWeakThis(WeakThis))
            {
                int32 LocalUserNum = 0;
                if (!StaticCastSharedPtr<FOnlineIdentityInterfaceEOS, IOnlineIdentity, ESPMode::ThreadSafe>(
                         This->Identity)
                         ->GetLocalUserNum(*EOSSearchingUser, LocalUserNum))
                {
                    UE_LOG(LogEOS, Error, TEXT("Searching user was not signed it at time of response"));
                    EOS_SessionSearch_Release(SearchHandle);
                    This->TriggerOnFindFriendSessionCompleteDelegates(
                        LocalUserNum,
                        false,
                        TArray<FOnlineSessionSearchResult>());
                    return;
                }

                EOS_SessionSearch_GetSearchResultCountOptions CountOpts = {};
                CountOpts.ApiVersion = EOS_SESSIONSEARCH_GETSEARCHRESULTCOUNT_API_LATEST;

                if (Info->ResultCode != EOS_EResult::EOS_Success ||
                    EOS_SessionSearch_GetSearchResultCount(SearchHandle, &CountOpts) != 1)
                {
                    EOS_SessionSearch_Release(SearchHandle);
                    This->TriggerOnFindFriendSessionCompleteDelegates(
                        LocalUserNum,
                        false,
                        TArray<FOnlineSessionSearchResult>());
                    return;
                }

                EOS_SessionSearch_CopySearchResultByIndexOptions CopyOpts = {};
                CopyOpts.ApiVersion = EOS_SESSIONSEARCH_COPYSEARCHRESULTBYINDEX_API_LATEST;
                CopyOpts.SessionIndex = 0;

                EOS_HSessionDetails ResultHandle = {};
                if (EOS_SessionSearch_CopySearchResultByIndex(SearchHandle, &CopyOpts, &ResultHandle) !=
                    EOS_EResult::EOS_Success)
                {
                    EOS_SessionSearch_Release(SearchHandle);
                    This->TriggerOnFindFriendSessionCompleteDelegates(
                        LocalUserNum,
                        false,
                        TArray<FOnlineSessionSearchResult>());
                    return;
                }

                EOS_SessionSearch_Release(SearchHandle);

                TArray<FOnlineSessionSearchResult> Results;
                Results.Add(FOnlineSessionSearchResultEOS::CreateFromDetails(ResultHandle));
                This->TriggerOnFindFriendSessionCompleteDelegates(LocalUserNum, true, Results);
            }
        });
    return true;
}

bool FOnlineSessionInterfaceEOS::FindFriendSession(
    const FUniqueNetId &LocalUserId,
    const TArray<TSharedRef<const FUniqueNetId>> &FriendList)
{
    if (FriendList.Num() != 1)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("FindFriendSession with multiple friends is not supported. You must pass exactly one friend."));
        return false;
    }

    return this->FindFriendSession(LocalUserId, *FriendList[0]);
}

bool FOnlineSessionInterfaceEOS::SendSessionInviteToFriend(
    int32 LocalUserNum,
    FName SessionName,
    const FUniqueNetId &Friend)
{
#if EOS_HAS_AUTHENTICATION
    auto Id = this->Identity->GetUniquePlayerId(LocalUserNum);
    if (!Id.IsValid())
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("LocalUserNum provided to SendSessionInviteToFriend does not have online identity."));
        return false;
    }

    return this->SendSessionInviteToFriend(Id.ToSharedRef().Get(), SessionName, Friend);
#else
    UE_LOG(LogEOS, Error, TEXT("Sending session invites to friends is not supported on dedicated servers."));
    return false;
#endif // #if EOS_HAS_AUTHENTICATION
}

bool FOnlineSessionInterfaceEOS::SendSessionInviteToFriend(
    const FUniqueNetId &LocalUserId,
    FName SessionName,
    const FUniqueNetId &Friend)
{
#if EOS_HAS_AUTHENTICATION
    if (!this->Friends.IsValid())
    {
        UE_LOG(LogEOS, Error, TEXT("Sending session invites to friends is not supported on dedicated servers."));
        return false;
    }

    FNamedOnlineSession *Session = this->GetNamedSession(SessionName);
    if (Session == nullptr)
    {
        FOnlineError Err = OnlineRedpointEOS::Errors::NotFound(
            LocalUserId,
            SessionName.ToString(),
            TEXT("FOnlineSessionInterfaceEOS::SendSessionInviteToFriend"),
            TEXT("There is not session with the specified local session name"));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
        return false;
    }

    auto SessionId = StaticCastSharedRef<const FUniqueNetIdEOSSession>(Session->SessionInfo->GetSessionId().AsShared());

    if (LocalUserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        FOnlineError Err = OnlineRedpointEOS::Errors::InvalidUser(
            LocalUserId,
            SessionName.ToString(),
            TEXT("FOnlineSessionInterfaceEOS::SendSessionInviteToFriend"),
            TEXT("Specified local user was not an EOS user"));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
        return false;
    }

    auto LocalUserIdRef = StaticCastSharedRef<const FUniqueNetIdEOS>(LocalUserId.AsShared());
    if (!LocalUserIdRef->HasValidProductUserId())
    {
        FOnlineError Err = OnlineRedpointEOS::Errors::InvalidUser(
            LocalUserId,
            SessionName.ToString(),
            TEXT("FOnlineSessionInterfaceEOS::SendSessionInviteToFriend"),
            TEXT("Specified local user was not an EOS user"));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
        return false;
    }

    FSyntheticPartySessionOnComplete EmptyHandler =
        FSyntheticPartySessionOnComplete::CreateLambda([](const FOnlineError &Error) {
            if (!Error.bSucceeded)
            {
                UE_LOG(LogEOS, Warning, TEXT("%s"), *Error.ToLogString());
            }
        });

    if (Friend.GetType().IsEqual(REDPOINT_EOS_SUBSYSTEM))
    {
        // Check to see if the target is a synthetic friend. If they are, send over the wrapped subsystems as well.
        int32 LocalUserNum;
        if (this->Identity->GetLocalUserNum(LocalUserId, LocalUserNum))
        {
            TSharedPtr<FOnlineFriend> FriendObj = this->Friends->GetFriend(LocalUserNum, Friend, TEXT(""));
            if (FriendObj.IsValid())
            {
                FString _;
                if (FriendObj->GetUserAttribute(TEXT("eosSynthetic.subsystemNames"), _))
                {
                    if (auto SPM = this->SyntheticPartySessionManager.Pin())
                    {
                        if (SPM.IsValid())
                        {
                            // This is a synthetic friend. Cast to FOnlineFriendSynthetic so we can access
                            // GetWrappedFriends() directly.
                            const TMap<FName, TSharedPtr<FOnlineFriend>> &WrappedFriends =
                                StaticCastSharedPtr<FOnlineFriendSynthetic>(FriendObj)->GetWrappedFriends();
                            for (const auto &WrappedFriend : WrappedFriends)
                            {
                                SPM->SendInvitationToSession(
                                    LocalUserNum,
                                    SessionId,
                                    WrappedFriend.Value->GetUserId(),
                                    EmptyHandler);
                            }
                        }
                        else
                        {
                            UE_LOG(
                                LogEOS,
                                Warning,
                                TEXT("SendSessionInviteToFriend: Target was synthetic friend, but synthetic session "
                                     "manager was not "
                                     "valid. Unable to send invitation over native subsystems."));
                        }
                    }
                    else
                    {
                        UE_LOG(
                            LogEOS,
                            Warning,
                            TEXT("SendSessionInviteToFriend: Target was synthetic friend, but synthetic session "
                                 "manager was not "
                                 "valid. Unable to send invitation over native subsystems."));
                    }
                }
            }
        }

        // Now also send over EOS as well so that the game can receive a notification of the incoming invitation.
        auto RecipientIdRef = StaticCastSharedRef<const FUniqueNetIdEOS>(Friend.AsShared());
        if (!RecipientIdRef->HasValidProductUserId())
        {
            // We succeeded sending over any native subsystems though, so this is a success.
            return true;
        }

        // Send the invitation directly over EOS.
        EOS_Sessions_SendInviteOptions Opts = {};
        Opts.ApiVersion = EOS_SESSIONS_SENDINVITE_API_LATEST;
        EOSString_SessionModification_SessionName::AllocateToCharBuffer(SessionName.ToString(), Opts.SessionName);
        Opts.LocalUserId = LocalUserIdRef->GetProductUserId();
        Opts.TargetUserId = RecipientIdRef->GetProductUserId();

        EOSRunOperation<EOS_HSessions, EOS_Sessions_SendInviteOptions, EOS_Sessions_SendInviteCallbackInfo>(
            this->EOSSessions,
            &Opts,
            *EOS_Sessions_SendInvite,
            [WeakThis = GetWeakThis(this), LocalUserIdRef, SessionName](
                const EOS_Sessions_SendInviteCallbackInfo *Info) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    if (Info->ResultCode != EOS_EResult::EOS_Success)
                    {
                        FOnlineError Err = ConvertError(
                            *LocalUserIdRef,
                            SessionName.ToString(),
                            TEXT("FOnlineSessionInterfaceEOS::SendSessionInviteToFriend"),
                            TEXT("The EOS callback returned an error code, but we couldn't propagate this to the game "
                                 "code because the SendInvite API for sessions in Unreal Engine is synchronous (and "
                                 "the EOS API is asynchronous)."),
                            Info->ResultCode);
                        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
                    }
                }
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
                    SPM->SendInvitationToSession(LocalUserNum, SessionId, Friend.AsShared(), EmptyHandler);
                    return true;
                }
                else
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("SendSessionInviteToFriend: Unable to get local user ID when sending to native friend."));
                }
            }
        }

        UE_LOG(
            LogEOS,
            Error,
            TEXT("SendSessionInviteToFriend: Target was native friend, but synthetic session manager was not "
                 "valid. Unable to send invitation at all."));
        return false;
    }
#else
    UE_LOG(LogEOS, Error, TEXT("Sending session invites to friends is not supported on dedicated servers."));
    return false;
#endif // #if EOS_HAS_AUTHENTICATION
}

bool FOnlineSessionInterfaceEOS::SendSessionInviteToFriends(
    int32 LocalUserNum,
    FName SessionName,
    const TArray<TSharedRef<const FUniqueNetId>> &FriendsArr)
{
#if EOS_HAS_AUTHENTICATION
    auto Id = this->Identity->GetUniquePlayerId(LocalUserNum);
    if (!Id.IsValid())
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("LocalUserNum provided to SendSessionInviteToFriend does not have online identity."));
        return false;
    }

    return this->SendSessionInviteToFriends(Id.ToSharedRef().Get(), SessionName, FriendsArr);
#else
    UE_LOG(LogEOS, Error, TEXT("Sending session invites to friends is not supported on dedicated servers."));
    return false;
#endif // #if EOS_HAS_AUTHENTICATION
}

bool FOnlineSessionInterfaceEOS::SendSessionInviteToFriends(
    const FUniqueNetId &LocalUserId,
    FName SessionName,
    const TArray<TSharedRef<const FUniqueNetId>> &FriendsArr)
{
#if EOS_HAS_AUTHENTICATION
    if (LocalUserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        FOnlineError Err = OnlineRedpointEOS::Errors::InvalidUser(
            LocalUserId,
            SessionName.ToString(),
            TEXT("FOnlineSessionInterfaceEOS::SendSessionInviteToFriends"),
            TEXT("Specified local user was not an EOS user"));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
        return true;
    }

    bool bAllSuccess = true;
    for (const auto &Friend : FriendsArr)
    {
        if (!this->SendSessionInviteToFriend(LocalUserId, SessionName, *Friend))
        {
            bAllSuccess = false;
        }
    }
    return bAllSuccess;
#else
    UE_LOG(LogEOS, Error, TEXT("Sending session invites to friends is not supported on dedicated servers."));
    return false;
#endif // #if EOS_HAS_AUTHENTICATION
}

bool FOnlineSessionInterfaceEOS::GetResolvedConnectString(FName SessionName, FString &ConnectInfo, FName PortType)
{
    FNamedOnlineSession *Session = this->GetNamedSession(SessionName);
    if (Session == nullptr)
    {
        UE_LOG(LogEOS, Error, TEXT("GetResolvedConnectString: Session '%s' does not exist."), *SessionName.ToString());
        return false;
    }

    if (!Session->SessionInfo.IsValid())
    {
        UE_LOG(LogEOS, Error, TEXT("GetResolvedConnectString: Session does not have valid session info."));
        return false;
    }

    if (Session->SessionInfo->GetSessionId().GetType() != REDPOINT_EOS_SUBSYSTEM_SESSION)
    {
        UE_LOG(LogEOS, Error, TEXT("GetResolvedConnectString: Session is not from the EOS subsystem."));
        return false;
    }

    TSharedPtr<FOnlineSessionInfoEOS> EOSInfo = StaticCastSharedPtr<FOnlineSessionInfoEOS>(Session->SessionInfo);
    return EOSInfo->GetResolvedConnectString(ConnectInfo, PortType);
}

bool FOnlineSessionInterfaceEOS::GetResolvedConnectString(
    const class FOnlineSessionSearchResult &SearchResult,
    FName PortType,
    FString &ConnectInfo)
{
    if (!SearchResult.IsSessionInfoValid() || !SearchResult.Session.SessionInfo.IsValid())
    {
        UE_LOG(LogEOS, Error, TEXT("GetResolvedConnectString: Search result does not have valid session info."));
        return false;
    }

    if (SearchResult.Session.SessionInfo->GetSessionId().GetType() != REDPOINT_EOS_SUBSYSTEM_SESSION)
    {
        UE_LOG(LogEOS, Error, TEXT("GetResolvedConnectString: Search result is not from the EOS subsystem."));
        return false;
    }

    TSharedPtr<FOnlineSessionInfoEOS> EOSInfo =
        StaticCastSharedPtr<FOnlineSessionInfoEOS>(SearchResult.Session.SessionInfo);
    return EOSInfo->GetResolvedConnectString(ConnectInfo, PortType);
}

FOnlineSessionSettings *FOnlineSessionInterfaceEOS::GetSessionSettings(FName SessionName)
{
    auto Session = this->GetNamedSession(SessionName);
    if (Session == nullptr)
    {
        return nullptr;
    }

    return &Session->SessionSettings;
}

bool FOnlineSessionInterfaceEOS::RegisterPlayer(FName SessionName, const FUniqueNetId &PlayerId, bool bWasInvited)
{
    TArray<TSharedRef<const FUniqueNetId>> Players;
    Players.Add(PlayerId.AsShared());
    return this->RegisterPlayers(SessionName, Players, bWasInvited);
}

bool FOnlineSessionInterfaceEOS::RegisterPlayers(
    FName SessionName,
    const TArray<TSharedRef<const FUniqueNetId>> &Players,
    bool bWasInvited)
{
    auto SessionNameAnsi = EOSString_SessionModification_SessionName::ToAnsiString(SessionName.ToString());

    for (const TSharedRef<const FUniqueNetId> &PlayerId : Players)
    {
        if (PlayerId->GetType() != REDPOINT_EOS_SUBSYSTEM)
        {
            UE_LOG(LogEOS, Error, TEXT("RegisterPlayers: Invalid player ID type."));
            return false;
        }
    }

    EOS_Sessions_RegisterPlayersOptions Opts = {};
    Opts.ApiVersion = EOS_SESSIONS_REGISTERPLAYERS_API_LATEST;
    Opts.SessionName = SessionNameAnsi.Ptr.Get();
    EOSString_ProductUserId::AllocateToIdListViaAccessor<TSharedRef<const FUniqueNetId>>(
        Players,
        [](const TSharedRef<const FUniqueNetId> &Player) {
            TSharedRef<const FUniqueNetIdEOS> PlayerIdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(Player);
            return PlayerIdEOS->GetProductUserId();
        },
        Opts.PlayersToRegisterCount,
        Opts.PlayersToRegister);

    EOSRunOperation<EOS_HSessions, EOS_Sessions_RegisterPlayersOptions, EOS_Sessions_RegisterPlayersCallbackInfo>(
        this->EOSSessions,
        &Opts,
        EOS_Sessions_RegisterPlayers,
        [WeakThis = GetWeakThis(this), SessionName, Players, Opts](
            const EOS_Sessions_RegisterPlayersCallbackInfo *Info) {
            if (auto This = PinWeakThis(WeakThis))
            {
                EOSString_ProductUserId::FreeFromIdListConst(Opts.PlayersToRegisterCount, Opts.PlayersToRegister);

                if (Info->ResultCode == EOS_EResult::EOS_Success)
                {
                    FNamedOnlineSession *Session = This->GetNamedSession(SessionName);
                    if (Session != nullptr)
                    {
                        for (const TSharedRef<const FUniqueNetId> &PlayerId : Players)
                        {
                            Session->RegisteredPlayers.AddUnique(PlayerId);
                        }
                    }
                }
                else
                {
                    FString ErrorMessage = ConvertError(
                                               TEXT("FOnlineSessionInterfaceEOS::RegisterPlayers"),
                                               FString::Printf(
                                                   TEXT("Failed to register one or more players for session '%s'."),
                                                   *SessionName.ToString()),
                                               Info->ResultCode)
                                               .ToLogString();
                    if (SessionName.IsEqual(NAME_GameSession))
                    {
                        UE_LOG(LogEOS, Warning, TEXT("%s"), *ErrorMessage);
                    }
                    else
                    {
                        UE_LOG(LogEOS, Error, TEXT("%s"), *ErrorMessage);
                    }
                }

                This->TriggerOnRegisterPlayersCompleteDelegates(
                    SessionName,
                    Players,
                    Info->ResultCode == EOS_EResult::EOS_Success || Info->ResultCode == EOS_EResult::EOS_NoChange);
            }
        });
    return true;
}

bool FOnlineSessionInterfaceEOS::UnregisterPlayer(FName SessionName, const FUniqueNetId &PlayerId)
{
    TArray<TSharedRef<const FUniqueNetId>> Players;
    Players.Add(PlayerId.AsShared());
    return this->UnregisterPlayers(SessionName, Players);
}

bool FOnlineSessionInterfaceEOS::UnregisterPlayers(
    FName SessionName,
    const TArray<TSharedRef<const FUniqueNetId>> &Players)
{
    auto SessionNameAnsi = EOSString_SessionModification_SessionName::ToAnsiString(SessionName.ToString());

    for (const TSharedRef<const FUniqueNetId> &PlayerId : Players)
    {
        if (PlayerId->GetType() != REDPOINT_EOS_SUBSYSTEM)
        {
            UE_LOG(LogEOS, Error, TEXT("UnregisterPlayers: Invalid player ID type."));
            return false;
        }
    }

    EOS_Sessions_UnregisterPlayersOptions Opts = {};
    Opts.ApiVersion = EOS_SESSIONS_UNREGISTERPLAYERS_API_LATEST;
    Opts.SessionName = SessionNameAnsi.Ptr.Get();
    EOSString_ProductUserId::AllocateToIdListViaAccessor<TSharedRef<const FUniqueNetId>>(
        Players,
        [](const TSharedRef<const FUniqueNetId> &Player) {
            TSharedRef<const FUniqueNetIdEOS> PlayerIdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(Player);
            return PlayerIdEOS->GetProductUserId();
        },
        Opts.PlayersToUnregisterCount,
        Opts.PlayersToUnregister);

    EOSRunOperation<EOS_HSessions, EOS_Sessions_UnregisterPlayersOptions, EOS_Sessions_UnregisterPlayersCallbackInfo>(
        this->EOSSessions,
        &Opts,
        EOS_Sessions_UnregisterPlayers,
        [WeakThis = GetWeakThis(this), SessionName, Players, Opts](
            const EOS_Sessions_UnregisterPlayersCallbackInfo *Info) {
            if (auto This = PinWeakThis(WeakThis))
            {
                EOSString_ProductUserId::FreeFromIdListConst(Opts.PlayersToUnregisterCount, Opts.PlayersToUnregister);

                if (Info->ResultCode == EOS_EResult::EOS_Success)
                {
                    FNamedOnlineSession *Session = This->GetNamedSession(SessionName);
                    if (Session != nullptr)
                    {
                        for (const TSharedRef<const FUniqueNetId> &PlayerId : Players)
                        {
                            Session->RegisteredPlayers.Remove(PlayerId);
                        }
                    }
                }
                else
                {
                    FString ErrorMessage = ConvertError(
                                               TEXT("FOnlineSessionInterfaceEOS::UnregisterPlayers"),
                                               FString::Printf(
                                                   TEXT("Failed to unregister one or more players for session '%s'."),
                                                   *SessionName.ToString()),
                                               Info->ResultCode)
                                               .ToLogString();
                    if (SessionName.IsEqual(NAME_GameSession))
                    {
                        UE_LOG(LogEOS, Warning, TEXT("%s"), *ErrorMessage);
                    }
                    else
                    {
                        UE_LOG(LogEOS, Error, TEXT("%s"), *ErrorMessage);
                    }
                }

                This->TriggerOnUnregisterPlayersCompleteDelegates(
                    SessionName,
                    Players,
                    Info->ResultCode == EOS_EResult::EOS_Success || Info->ResultCode == EOS_EResult::EOS_NoChange);
            }
        });
    return true;
}

void FOnlineSessionInterfaceEOS::RegisterLocalPlayer(
    const FUniqueNetId &PlayerId,
    FName SessionName,
    const FOnRegisterLocalPlayerCompleteDelegate &Delegate)
{
    // Not used.
    Delegate.ExecuteIfBound(PlayerId, EOnJoinSessionCompleteResult::Success);
}

void FOnlineSessionInterfaceEOS::UnregisterLocalPlayer(
    const FUniqueNetId &PlayerId,
    FName SessionName,
    const FOnUnregisterLocalPlayerCompleteDelegate &Delegate)
{
    // Not used.
    Delegate.ExecuteIfBound(PlayerId, true);
}

int32 FOnlineSessionInterfaceEOS::GetNumSessions()
{
    return this->Sessions.Num();
}

void FOnlineSessionInterfaceEOS::DumpSessionState()
{
}

EOS_DISABLE_STRICT_WARNINGS