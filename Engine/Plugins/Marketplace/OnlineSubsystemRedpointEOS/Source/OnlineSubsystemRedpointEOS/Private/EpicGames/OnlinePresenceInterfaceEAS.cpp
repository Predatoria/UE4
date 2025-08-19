// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlinePresenceInterfaceEAS.h"

#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineFriendsInterfaceEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineUserPresenceEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/UniqueNetIdEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/MultiOperation.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

void FOnlinePresenceInterfaceEAS::ConnectFriendsToPresence()
{
    this->Friends->Presence = this->AsShared();
}

void FOnlinePresenceInterfaceEAS::DisconnectFriendsFromPresence()
{
    this->Friends->Presence.Reset();
}

void FOnlinePresenceInterfaceEAS::RegisterEvents()
{
    EOS_Presence_AddNotifyOnPresenceChangedOptions PresenceChangedOpts = {};
    PresenceChangedOpts.ApiVersion = EOS_PRESENCE_ADDNOTIFYONPRESENCECHANGED_API_LATEST;

    this->Unregister_PresenceChanged = EOSRegisterEvent<
        EOS_HPresence,
        EOS_Presence_AddNotifyOnPresenceChangedOptions,
        EOS_Presence_PresenceChangedCallbackInfo>(
        this->EOSPresence,
        &PresenceChangedOpts,
        EOS_Presence_AddNotifyOnPresenceChanged,
        EOS_Presence_RemoveNotifyOnPresenceChanged,
        [WeakThis = GetWeakThis(this)](const EOS_Presence_PresenceChangedCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                EOS_Presence_CopyPresenceOptions CopyPresenceOpts = {};
                CopyPresenceOpts.ApiVersion = EOS_PRESENCE_COPYPRESENCE_API_LATEST;
                CopyPresenceOpts.LocalUserId = Data->LocalUserId;
                CopyPresenceOpts.TargetUserId = Data->PresenceUserId;
                EOS_Presence_Info *PresenceInfo = nullptr;
                EOS_EResult CopyResult = EOS_Presence_CopyPresence(This->EOSPresence, &CopyPresenceOpts, &PresenceInfo);
                if (CopyResult == EOS_EResult::EOS_Success)
                {
                    TSharedRef<const FUniqueNetIdEAS> UserWhoseStatusWasUpdatedId =
                        MakeShared<FUniqueNetIdEAS>(Data->PresenceUserId);
                    This->GetOrCreatePresenceInfoForEpicId(UserWhoseStatusWasUpdatedId)
                        ->UpdateFromPresence(PresenceInfo);
                    EOS_Presence_Info_Release(PresenceInfo);

                    TArray<TSharedRef<FOnlineUserPresence>> PresenceArray;
                    PresenceArray.Add(This->PresenceByEpicId[*UserWhoseStatusWasUpdatedId].ToSharedRef());
                    This->TriggerOnPresenceArrayUpdatedDelegates(*UserWhoseStatusWasUpdatedId, PresenceArray);
                    This->TriggerOnPresenceReceivedDelegates(
                        *UserWhoseStatusWasUpdatedId,
                        This->PresenceByEpicId[*UserWhoseStatusWasUpdatedId].ToSharedRef());
                }
                else
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("OnPresenceChanged: EOS_Presence_CopyPresence failed for local user %s, "
                             "target "
                             "user "
                             "%s, with error code %s"),
                        *EOSString_EpicAccountId::ToAnsiString(Data->LocalUserId).StrTCHAR,
                        *EOSString_EpicAccountId::ToAnsiString(Data->PresenceUserId).StrTCHAR,
                        ANSI_TO_TCHAR(EOS_EResult_ToString(CopyResult)));
                }
            }
        });
}

FOnlinePresenceInterfaceEAS::FOnlinePresenceInterfaceEAS(
    EOS_HPlatform InPlatform,
    const TSharedRef<FOnlineIdentityInterfaceEAS, ESPMode::ThreadSafe> &InIdentity,
    const TSharedRef<FOnlineFriendsInterfaceEAS, ESPMode::ThreadSafe> &InFriends,
    const TSharedRef<const FEOSConfig> &InConfig)
    : EOSPresence(EOS_Platform_GetPresenceInterface(InPlatform))
    , EOSConnect(EOS_Platform_GetConnectInterface(InPlatform))
    , Identity(InIdentity)
    , Friends(InFriends)
    , Config(InConfig)
    , PresenceByEpicId()
    , Unregister_PresenceChanged()
{
    check(this->EOSPresence != nullptr);
    check(this->EOSConnect != nullptr);
}

TSharedPtr<FOnlineUserPresenceEAS> FOnlinePresenceInterfaceEAS::GetOrCreatePresenceInfoForEpicId(
    const TSharedRef<const FUniqueNetIdEAS> &InEpicId)
{
    if (this->PresenceByEpicId.Contains(*InEpicId))
    {
        return this->PresenceByEpicId[*InEpicId];
    }

    this->PresenceByEpicId.Add(*InEpicId, MakeShared<FOnlineUserPresenceEAS>(nullptr, this->Config->GetProductId()));
    return this->PresenceByEpicId[*InEpicId];
}

void FOnlinePresenceInterfaceEAS::SetPresence(
    const FUniqueNetId &User,
    const FOnlineUserPresenceStatus &Status,
    const FOnPresenceTaskCompleteDelegate &Delegate)
{
    if (User.GetType() != REDPOINT_EAS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("SetPresence: User ID is not an Epic account ID."));
        Delegate.ExecuteIfBound(User, false);
        return;
    }

    auto EpicUser = StaticCastSharedRef<const FUniqueNetIdEAS>(User.AsShared());

    EOS_Presence_CreatePresenceModificationOptions ModOpts = {};
    ModOpts.ApiVersion = EOS_PRESENCE_CREATEPRESENCEMODIFICATION_API_LATEST;
    ModOpts.LocalUserId = EpicUser->GetEpicAccountId();

    EOS_HPresenceModification Modification = {};

    EOS_EResult LastResult = {};
    if ((LastResult = EOS_Presence_CreatePresenceModification(this->EOSPresence, &ModOpts, &Modification)) !=
        EOS_EResult::EOS_Success)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("SetPresence: EOS_Presence_CreatePresenceModification failed with error code %s"),
            ANSI_TO_TCHAR(EOS_EResult_ToString(LastResult)));
        Delegate.ExecuteIfBound(User, false);
        return;
    }

    {
        EOS_PresenceModification_SetStatusOptions Opts = {};
        Opts.ApiVersion = EOS_PRESENCE_SETSTATUS_API_LATEST;
        switch (Status.State)
        {
        case EOnlinePresenceState::Offline:
            Opts.Status = EOS_Presence_EStatus::EOS_PS_Offline;
            break;
        case EOnlinePresenceState::Away:
            Opts.Status = EOS_Presence_EStatus::EOS_PS_Away;
            break;
        case EOnlinePresenceState::ExtendedAway:
            Opts.Status = EOS_Presence_EStatus::EOS_PS_ExtendedAway;
            break;
        case EOnlinePresenceState::DoNotDisturb:
            Opts.Status = EOS_Presence_EStatus::EOS_PS_DoNotDisturb;
            break;
        case EOnlinePresenceState::Online:
        case EOnlinePresenceState::Chat:
        default:
            Opts.Status = EOS_Presence_EStatus::EOS_PS_Online;
            break;
        }
        if ((LastResult = EOS_PresenceModification_SetStatus(Modification, &Opts)) != EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("SetPresence: EOS_PresenceModification_SetStatus failed with error code %s"),
                ANSI_TO_TCHAR(EOS_EResult_ToString(LastResult)));
            Delegate.ExecuteIfBound(User, false);
            return;
        }
    }

    {
        auto StatusChr = EOSString_Presence_RichTextValue::ToUtf8String(Status.StatusStr);

        EOS_PresenceModification_SetRawRichTextOptions Opts = {};
        Opts.ApiVersion = EOS_PRESENCE_SETRAWRICHTEXT_API_LATEST;
        Opts.RichText = StatusChr.GetAsChar();
        if ((LastResult = EOS_PresenceModification_SetRawRichText(Modification, &Opts)) != EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("SetPresence: EOS_PresenceModification_SetRawRichText failed with error code %s"),
                ANSI_TO_TCHAR(EOS_EResult_ToString(LastResult)));
            Delegate.ExecuteIfBound(User, false);
            return;
        }
    }

    if (Status.Properties.Num() > 0)
    {
        struct RecordData
        {
            const char *Key;
            const char *Value;
        };
        TArray<RecordData> RecordDatas;
        for (const auto &KV : Status.Properties)
        {
            if (KV.Key.StartsWith(TEXT("JoinInfo_")))
            {
                // Special key for indicating join information, ignore for general properties.
                continue;
            }

            RecordData RecordData = {};
            EOS_EResult KeyResult = EOSString_Presence_DataRecord_Key::AllocateToCharBuffer(KV.Key, RecordData.Key);
            EOS_EResult ValueResult =
                EOSString_Presence_DataRecord_Value::AllocateToCharBuffer(KV.Value.ToString(), RecordData.Value);
            if (KeyResult != EOS_EResult::EOS_Success || ValueResult != EOS_EResult::EOS_Success)
            {
                EOSString_Presence_DataRecord_Key::FreeFromCharBuffer(RecordData.Key);
                EOSString_Presence_DataRecord_Value::FreeFromCharBuffer(RecordData.Value);
                continue;
            }
            RecordDatas.Add(RecordData);
        }

        auto Len = RecordDatas.Num();
        EOS_Presence_DataRecord *Records =
            (EOS_Presence_DataRecord *)FMemory::MallocZeroed(sizeof(EOS_Presence_DataRecord) * Len);
        auto i = 0;
        TArray<const char *> AllocatedKeyMemTemp;
        TArray<const char *> AllocatedValueMemTemp;
        for (auto RecordData : RecordDatas)
        {
            Records[i].ApiVersion = EOS_PRESENCE_DATARECORD_API_LATEST;
            Records[i].Key = RecordData.Key;
            Records[i].Value = RecordData.Value;

            AllocatedKeyMemTemp.Add(Records[i].Key);
            AllocatedValueMemTemp.Add(Records[i].Value);

            i++;
        }

        EOS_PresenceModification_SetDataOptions Opts = {};
        Opts.ApiVersion = EOS_PRESENCE_SETDATA_API_LATEST;
        Opts.Records = Records;
        Opts.RecordsCount = Len;
        auto Result = EOS_PresenceModification_SetData(Modification, &Opts);
        for (auto Mem : AllocatedKeyMemTemp)
        {
            EOSString_Presence_DataRecord_Key::FreeFromCharBuffer(Mem);
        }
        for (auto Mem : AllocatedValueMemTemp)
        {
            EOSString_Presence_DataRecord_Value::FreeFromCharBuffer(Mem);
        }
        if (Result != EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("SetPresence: EOS_PresenceModification_SetData failed with error code %s"),
                ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
            Delegate.ExecuteIfBound(User, false);
            return;
        }
    }

    // @todo: Implement delete when we've got a cache of the user's current presence.

    {
        FString JoinInfoStr = TEXT("");
        for (const auto &KV : Status.Properties)
        {
            if (KV.Key == TEXT("JoinInfo_JoinData"))
            {
                JoinInfoStr = KV.Value.ToString();
                break;
            }
        }

        EOS_EResult Result = {};
        if (JoinInfoStr != TEXT(""))
        {
            // Set join information.
            auto JoinInfoAnsi = StringCast<ANSICHAR>(*JoinInfoStr);
            EOS_PresenceModification_SetJoinInfoOptions Opts = {};
            Opts.ApiVersion = EOS_PRESENCEMODIFICATION_SETJOININFO_API_LATEST;
            Opts.JoinInfo = JoinInfoAnsi.Get();
            Result = EOS_PresenceModification_SetJoinInfo(Modification, &Opts);
        }
        else
        {
            // The EOS SDK does not allow you to clear join information.
            Result = EOS_EResult::EOS_Success;
        }

        if (Result != EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("SetPresence: EOS_PresenceModification_SetJoinInfo failed with error code %s"),
                ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
            Delegate.ExecuteIfBound(User, false);
            return;
        }
    }

    auto UserRef = User.AsShared();

    EOS_Presence_SetPresenceOptions SetOpts = {};
    SetOpts.ApiVersion = EOS_PRESENCE_SETPRESENCE_API_LATEST;
    SetOpts.LocalUserId = EpicUser->GetEpicAccountId();
    SetOpts.PresenceModificationHandle = Modification;
    EOSRunOperation<EOS_HPresence, EOS_Presence_SetPresenceOptions, EOS_Presence_SetPresenceCallbackInfo>(
        this->EOSPresence,
        &SetOpts,
        *EOS_Presence_SetPresence,
        [Modification, Delegate, UserRef](const EOS_Presence_SetPresenceCallbackInfo *Info) {
            EOS_PresenceModification_Release(Modification);

            if (Info->ResultCode != EOS_EResult::EOS_Success)
            {
                UE_LOG(
                    LogEOS,
                    Error,
                    TEXT("SetPresence: EOS_Presence_SetPresence failed with error code %s"),
                    ANSI_TO_TCHAR(EOS_EResult_ToString(Info->ResultCode)));
            }

            Delegate.ExecuteIfBound(UserRef.Get(), Info->ResultCode == EOS_EResult::EOS_Success);
        });
}

void FOnlinePresenceInterfaceEAS::QueryPresence(
    const FUniqueNetId &User,
    const FOnPresenceTaskCompleteDelegate &Delegate)
{
    if (User.GetType() != REDPOINT_EAS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("QueryPresence: LocalUserId is not an Epic user."));
        Delegate.ExecuteIfBound(User, false);
        return;
    }

    // Find the local user num for the given user, then find all their friends, and then propagate the call to
    // QueryPresence with a list of their friend IDs.
    int32 LocalUserNum;
    if (!this->Identity->GetLocalUserNum(User, LocalUserNum))
    {
        UE_LOG(LogEOS, Error, TEXT("QueryPresence: User parameter is not the user ID of a locally logged in user"));
        Delegate.ExecuteIfBound(User, false);
        return;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(User.AsShared());

    if (!this->Friends->ReadFriendsList(
            LocalUserNum,
            TEXT(""),
            FOnReadFriendsListComplete::CreateLambda([WeakThis = GetWeakThis(this), Delegate, EOSUser](
                                                         int32 LocalUserNum,
                                                         bool bWasSuccessful,
                                                         const FString &ListName,
                                                         const FString &ErrorStr) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    if (!bWasSuccessful)
                    {
                        UE_LOG(LogEOS, Error, TEXT("QueryPresence: ReadFriendsList operation failed: %s"), *ErrorStr);
                        Delegate.ExecuteIfBound(*EOSUser, false);
                        return;
                    }

                    TArray<TSharedRef<const FUniqueNetId>> FriendUserIds;
                    TArray<TSharedRef<FOnlineFriend>> OnlineFriends;
                    verify(This->Friends->GetFriendsList(LocalUserNum, ListName, OnlineFriends));
                    for (const auto &Friend : OnlineFriends)
                    {
                        FriendUserIds.Add(Friend->GetUserId());
                    }

                    This->QueryPresence(*EOSUser, FriendUserIds, Delegate);
                }
            })))
    {
        UE_LOG(LogEOS, Error, TEXT("QueryPresence: ReadFriendsList failed to execute"));
        Delegate.ExecuteIfBound(User, false);
        return;
    }
}

void FOnlinePresenceInterfaceEAS::QueryPresence(
    const FUniqueNetId &LocalUserId,
    const TArray<TSharedRef<const FUniqueNetId>> &UserIds,
    const FOnPresenceTaskCompleteDelegate &Delegate)
{
    if (LocalUserId.GetType() != REDPOINT_EAS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("QueryPresence: LocalUserId is not an Epic user."));
        Delegate.ExecuteIfBound(LocalUserId, false);
        return;
    }

    auto EASUser = StaticCastSharedRef<const FUniqueNetIdEAS>(LocalUserId.AsShared());
    if (!EASUser->HasValidEpicAccountId())
    {
        UE_LOG(LogEOS, Error, TEXT("QueryPresence: LocalUserId is not an EOS user signed into an Epic Games account."));
        Delegate.ExecuteIfBound(LocalUserId, false);
        return;
    }

    // Filter the user IDs to only select Epic accounts.
    TArray<TSharedRef<const FUniqueNetIdEAS>> EpicUserIds;
    for (const auto &UserId : UserIds)
    {
        if (UserId->GetType() == REDPOINT_EAS_SUBSYSTEM)
        {
            EpicUserIds.Add(StaticCastSharedRef<const FUniqueNetIdEAS>(UserId));
        }
    }

    FMultiOperation<TSharedRef<const FUniqueNetIdEAS>, bool>::Run(
        EpicUserIds,
        [WeakThis = GetWeakThis(this), EASUser](
            const TSharedRef<const FUniqueNetIdEAS> &InTargetUserId,
            const std::function<void(bool OutValue)> &OnDone) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (!InTargetUserId->HasValidEpicAccountId())
                {
                    return false;
                }

                EOS_Presence_HasPresenceOptions HasPresenceOpts = {};
                HasPresenceOpts.ApiVersion = EOS_PRESENCE_HASPRESENCE_API_LATEST;
                HasPresenceOpts.LocalUserId = EASUser->GetEpicAccountId();
                HasPresenceOpts.TargetUserId = InTargetUserId->GetEpicAccountId();
                if (EOS_Presence_HasPresence(This->EOSPresence, &HasPresenceOpts) == EOS_TRUE &&
                    This->PresenceByEpicId.Contains(*InTargetUserId))
                {
                    // We already have cached presence information for this user (which will be kept in sync
                    // by the NotifyOnPresenceChanged event).
                    OnDone(true);
                    return true;
                }

                EOS_Presence_QueryPresenceOptions QueryPresenceOpts = {};
                QueryPresenceOpts.ApiVersion = EOS_PRESENCE_QUERYPRESENCE_API_LATEST;
                QueryPresenceOpts.LocalUserId = EASUser->GetEpicAccountId();
                QueryPresenceOpts.TargetUserId = InTargetUserId->GetEpicAccountId();
                EOSRunOperation<
                    EOS_HPresence,
                    EOS_Presence_QueryPresenceOptions,
                    EOS_Presence_QueryPresenceCallbackInfo>(
                    This->EOSPresence,
                    &QueryPresenceOpts,
                    *EOS_Presence_QueryPresence,
                    [WeakThis = GetWeakThis(This), EASUser, InTargetUserId, OnDone](
                        const EOS_Presence_QueryPresenceCallbackInfo *Info) {
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            if (Info->ResultCode == EOS_EResult::EOS_Success)
                            {
                                EOS_Presence_CopyPresenceOptions CopyPresenceOpts = {};
                                CopyPresenceOpts.ApiVersion = EOS_PRESENCE_COPYPRESENCE_API_LATEST;
                                CopyPresenceOpts.LocalUserId = EASUser->GetEpicAccountId();
                                CopyPresenceOpts.TargetUserId = InTargetUserId->GetEpicAccountId();
                                EOS_Presence_Info *PresenceInfo = nullptr;
                                EOS_EResult CopyResult =
                                    EOS_Presence_CopyPresence(This->EOSPresence, &CopyPresenceOpts, &PresenceInfo);
                                if (CopyResult == EOS_EResult::EOS_Success)
                                {
                                    This->GetOrCreatePresenceInfoForEpicId(InTargetUserId)
                                        ->UpdateFromPresence(PresenceInfo);
                                    EOS_Presence_Info_Release(PresenceInfo);

                                    TArray<TSharedRef<FOnlineUserPresence>> PresenceArray;
                                    PresenceArray.Add(This->PresenceByEpicId[*InTargetUserId].ToSharedRef());
                                    This->TriggerOnPresenceArrayUpdatedDelegates(*InTargetUserId, PresenceArray);
                                    This->TriggerOnPresenceReceivedDelegates(
                                        *InTargetUserId,
                                        This->PresenceByEpicId[*InTargetUserId].ToSharedRef());
                                    OnDone(true);
                                }
                                else
                                {
                                    UE_LOG(
                                        LogEOS,
                                        Error,
                                        TEXT("QueryPresence: EOS_Presence_CopyPresence failed for local "
                                             "user "
                                             "%s, "
                                             "target "
                                             "user "
                                             "%s, with error code %s"),
                                        *EASUser->ToString(),
                                        *InTargetUserId->ToString(),
                                        ANSI_TO_TCHAR(EOS_EResult_ToString(CopyResult)));
                                    OnDone(false);
                                }
                            }
                            else
                            {
                                UE_LOG(
                                    LogEOS,
                                    Error,
                                    TEXT("QueryPresence: EOS_Presence_QueryPresence failed for local user "
                                         "%s, "
                                         "target "
                                         "user "
                                         "%s, with error code %s"),
                                    *EASUser->ToString(),
                                    *InTargetUserId->ToString(),
                                    ANSI_TO_TCHAR(EOS_EResult_ToString(Info->ResultCode)));
                                OnDone(false);
                            }
                        }
                        else
                        {
                            OnDone(false);
                        }
                    });
                return true;
            }
            else
            {
                return false;
            }
        },
        [WeakThis = GetWeakThis(this), EASUser, Delegate](const TArray<bool> &OutValues) {
            for (auto Result : OutValues)
            {
                if (Result)
                {
                    Delegate.ExecuteIfBound(*EASUser, true);
                    return;
                }
            }
            Delegate.ExecuteIfBound(*EASUser, false);
        });
}

EOnlineCachedResult::Type FOnlinePresenceInterfaceEAS::GetCachedPresence(
    const FUniqueNetId &User,
    TSharedPtr<FOnlineUserPresence> &OutPresence)
{
    if (this->PresenceByEpicId.Contains(User))
    {
        OutPresence = this->PresenceByEpicId[User];
        return EOnlineCachedResult::Success;
    }

    OutPresence = nullptr;
    return EOnlineCachedResult::NotFound;
}

EOnlineCachedResult::Type FOnlinePresenceInterfaceEAS::GetCachedPresenceForApp(
    const FUniqueNetId &LocalUserId,
    const FUniqueNetId &User,
    const FString &AppId,
    TSharedPtr<FOnlineUserPresence> &OutPresence)
{
    return this->GetCachedPresence(User, OutPresence);
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION