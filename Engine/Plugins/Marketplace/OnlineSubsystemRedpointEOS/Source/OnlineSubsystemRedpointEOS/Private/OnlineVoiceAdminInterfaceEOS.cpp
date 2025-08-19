// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Private/OnlineVoiceAdminInterfaceEOS.h"

#if defined(EOS_VOICE_CHAT_SUPPORTED)

#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSErrorConv.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

FOnlineVoiceAdminInterfaceEOS::FOnlineVoiceAdminInterfaceEOS(EOS_HRTCAdmin InRTCAdmin)
    : EOSRTCAdmin(InRTCAdmin)
{
    checkf(
        this->EOSRTCAdmin != nullptr,
        TEXT("FOnlineVoiceAdminInterfaceEOS must not be passed null EOS RTCAdmin interface"));
}

bool FOnlineVoiceAdminInterfaceEOS::CreateChannelCredentials(
    const FUniqueNetId &LocalUserId,
    const FString &ChannelName,
    const TArray<TSharedRef<const FUniqueNetId>> &TargetUserIds,
    FOnVoiceAdminCreateChannelCredentialsComplete OnComplete)
{
    if (LocalUserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        OnComplete.ExecuteIfBound(
            OnlineRedpointEOS::Errors::InvalidUser(),
            LocalUserId,
            TArray<FVoiceAdminChannelCredentials>());
        return true;
    }

    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(LocalUserId.AsShared());
    if (!EOSUser->HasValidProductUserId() && !EOSUser->IsDedicatedServer())
    {
        OnComplete.ExecuteIfBound(
            OnlineRedpointEOS::Errors::InvalidUser(),
            LocalUserId,
            TArray<FVoiceAdminChannelCredentials>());
        return true;
    }

    EOS_RTCAdmin_QueryJoinRoomTokenOptions Opts = {};
    Opts.ApiVersion = EOS_RTCADMIN_QUERYJOINROOMTOKEN_API_LATEST;
    if (EOSUser->IsDedicatedServer())
    {
        Opts.LocalUserId = nullptr;
    }
    else
    {
        Opts.LocalUserId = EOSUser->GetProductUserId();
    }
    EOSString_AnsiUnlimited::AllocateToCharBuffer(ChannelName, Opts.RoomName);
    EOSString_ProductUserId::AllocateToIdListViaAccessor<TSharedRef<const FUniqueNetId>>(
        TargetUserIds,
        [](const TSharedRef<const FUniqueNetId> &TargetUserId) -> EOS_ProductUserId {
            if (TargetUserId->GetType() == REDPOINT_EOS_SUBSYSTEM)
            {
                return StaticCastSharedRef<const FUniqueNetIdEOS>(TargetUserId)->GetProductUserId();
            }
            return nullptr;
        },
        Opts.TargetUserIdsCount,
        Opts.TargetUserIds);
    Opts.TargetUserIpAddresses = nullptr;

    EOSRunOperation<
        EOS_HRTCAdmin,
        EOS_RTCAdmin_QueryJoinRoomTokenOptions,
        EOS_RTCAdmin_QueryJoinRoomTokenCompleteCallbackInfo>(
        this->EOSRTCAdmin,
        &Opts,
        &EOS_RTCAdmin_QueryJoinRoomToken,
        [WeakThis = GetWeakThis(this), Opts, EOSUser, OnComplete](
            const EOS_RTCAdmin_QueryJoinRoomTokenCompleteCallbackInfo *Data) {
            EOSString_ProductUserId::FreeFromIdListConst(Opts.TargetUserIdsCount, Opts.TargetUserIds);
            EOSString_AnsiUnlimited::FreeFromCharBufferConst(Opts.RoomName);

            if (auto This = PinWeakThis(WeakThis))
            {
                if (Data->ResultCode != EOS_EResult::EOS_Success)
                {
                    UE_LOG(
                        LogEOS,
                        Warning,
                        TEXT("Failed to retrieve RTC user tokens, got result %s"),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
                    OnComplete.ExecuteIfBound(
                        ConvertError(
                            TEXT("EOS_RTCAdmin_QueryJoinRoomToken"),
                            TEXT("Failed to retrieve RTC user tokens"),
                            Data->ResultCode),
                        *EOSUser,
                        TArray<FVoiceAdminChannelCredentials>());
                    return;
                }

                TArray<FVoiceAdminChannelCredentials> Results;
                for (uint32_t i = 0; i < Data->TokenCount; i++)
                {
                    EOS_RTCAdmin_UserToken *UserToken = nullptr;

                    EOS_RTCAdmin_CopyUserTokenByIndexOptions CopyOpts = {};
                    CopyOpts.ApiVersion = EOS_RTCADMIN_COPYUSERTOKENBYINDEX_API_LATEST;
                    CopyOpts.QueryId = Data->QueryId;
                    CopyOpts.UserTokenIndex = i;
                    EOS_EResult CopyResult =
                        EOS_RTCAdmin_CopyUserTokenByIndex(This->EOSRTCAdmin, &CopyOpts, &UserToken);
                    if (CopyResult == EOS_EResult::EOS_Success)
                    {
                        FVoiceAdminChannelCredentials Creds = {};
                        Creds.TargetUserId = MakeShared<FUniqueNetIdEOS>(UserToken->ProductUserId);
                        Creds.PlayerName = Creds.TargetUserId->ToString();
                        Creds.ChannelCredentials = FString::Printf(
                            TEXT("%s?token=%s"),
                            *EOSString_AnsiUnlimited::FromAnsiString(Data->ClientBaseUrl),
                            *EOSString_AnsiUnlimited::FromAnsiString(UserToken->Token));
                        Results.Add(Creds);
                    }
                    else
                    {
                        UE_LOG(
                            LogEOS,
                            Warning,
                            TEXT("Unable to copy RTC user token at index %d, got result %s"),
                            i,
                            ANSI_TO_TCHAR(EOS_EResult_ToString(CopyResult)));
                    }
                    if (UserToken != nullptr)
                    {
                        EOS_RTCAdmin_UserToken_Release(UserToken);
                    }
                }
                OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::Success(), *EOSUser, Results);
            }
        });
    return true;
}

bool FOnlineVoiceAdminInterfaceEOS::KickParticipant(
    const FUniqueNetId &LocalUserId,
    const FString &ChannelName,
    const FUniqueNetId &TargetUserId,
    FOnVoiceAdminKickParticipantComplete OnComplete)
{
    if (LocalUserId.GetType() != REDPOINT_EOS_SUBSYSTEM || TargetUserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::InvalidUser(), LocalUserId, TargetUserId);
        return true;
    }

    auto EOSLocalUser = StaticCastSharedRef<const FUniqueNetIdEOS>(LocalUserId.AsShared());
    auto EOSTargetUser = StaticCastSharedRef<const FUniqueNetIdEOS>(TargetUserId.AsShared());
    if (!EOSLocalUser->HasValidProductUserId() || !EOSTargetUser->HasValidProductUserId())
    {
        OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::InvalidUser(), LocalUserId, TargetUserId);
        return true;
    }

    EOS_RTCAdmin_KickOptions Opts = {};
    Opts.ApiVersion = EOS_RTCADMIN_KICK_API_LATEST;
    EOSString_AnsiUnlimited::AllocateToCharBuffer(ChannelName, Opts.RoomName);
    Opts.TargetUserId = EOSTargetUser->GetProductUserId();

    EOSRunOperation<EOS_HRTCAdmin, EOS_RTCAdmin_KickOptions, EOS_RTCAdmin_KickCompleteCallbackInfo>(
        this->EOSRTCAdmin,
        &Opts,
        &EOS_RTCAdmin_Kick,
        [WeakThis = GetWeakThis(this), Opts, EOSLocalUser, EOSTargetUser, OnComplete](
            const EOS_RTCAdmin_KickCompleteCallbackInfo *Data) {
            EOSString_AnsiUnlimited::FreeFromCharBufferConst(Opts.RoomName);

            if (auto This = PinWeakThis(WeakThis))
            {
                if (Data->ResultCode != EOS_EResult::EOS_Success)
                {
                    UE_LOG(
                        LogEOS,
                        Warning,
                        TEXT("Failed to kick RTC user, got result %s"),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
                    OnComplete.ExecuteIfBound(
                        ConvertError(TEXT("EOS_RTCAdmin_Kick"), TEXT("Failed to kick RTC user"), Data->ResultCode),
                        *EOSLocalUser,
                        *EOSTargetUser);
                }
                else
                {
                    OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::Success(), *EOSLocalUser, *EOSTargetUser);
                }
            }
        });
    return true;
}

bool FOnlineVoiceAdminInterfaceEOS::SetParticipantHardMute(
    const FUniqueNetId &LocalUserId,
    const FString &ChannelName,
    const FUniqueNetId &TargetUserId,
    bool bMuted,
    FOnVoiceAdminSetParticipantHardMuteComplete OnComplete)
{
    if (LocalUserId.GetType() != REDPOINT_EOS_SUBSYSTEM || TargetUserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::InvalidUser(), LocalUserId, TargetUserId, bMuted);
        return true;
    }

    auto EOSLocalUser = StaticCastSharedRef<const FUniqueNetIdEOS>(LocalUserId.AsShared());
    auto EOSTargetUser = StaticCastSharedRef<const FUniqueNetIdEOS>(TargetUserId.AsShared());
    if (!EOSLocalUser->HasValidProductUserId() || !EOSTargetUser->HasValidProductUserId())
    {
        OnComplete.ExecuteIfBound(OnlineRedpointEOS::Errors::InvalidUser(), LocalUserId, TargetUserId, bMuted);
        return true;
    }

    EOS_RTCAdmin_SetParticipantHardMuteOptions Opts = {};
    Opts.ApiVersion = EOS_RTCADMIN_SETPARTICIPANTHARDMUTE_API_LATEST;
    Opts.bMute = bMuted;
    EOSString_AnsiUnlimited::AllocateToCharBuffer(ChannelName, Opts.RoomName);
    Opts.TargetUserId = EOSTargetUser->GetProductUserId();

    EOSRunOperation<
        EOS_HRTCAdmin,
        EOS_RTCAdmin_SetParticipantHardMuteOptions,
        EOS_RTCAdmin_SetParticipantHardMuteCompleteCallbackInfo>(
        this->EOSRTCAdmin,
        &Opts,
        &EOS_RTCAdmin_SetParticipantHardMute,
        [WeakThis = GetWeakThis(this), Opts, EOSLocalUser, EOSTargetUser, bMuted, OnComplete](
            const EOS_RTCAdmin_SetParticipantHardMuteCompleteCallbackInfo *Data) {
            EOSString_AnsiUnlimited::FreeFromCharBufferConst(Opts.RoomName);

            if (auto This = PinWeakThis(WeakThis))
            {
                if (Data->ResultCode != EOS_EResult::EOS_Success)
                {
                    UE_LOG(
                        LogEOS,
                        Warning,
                        TEXT("Failed to hard mute/unmute RTC user, got result %s"),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
                    OnComplete.ExecuteIfBound(
                        ConvertError(
                            TEXT("EOS_RTCAdmin_SetParticipantHardMute"),
                            TEXT("Failed to hard mute/unmute RTC user"),
                            Data->ResultCode),
                        *EOSLocalUser,
                        *EOSTargetUser,
                        bMuted);
                }
                else
                {
                    OnComplete
                        .ExecuteIfBound(OnlineRedpointEOS::Errors::Success(), *EOSLocalUser, *EOSTargetUser, bMuted);
                }
            }
        });
    return true;
}

EOS_DISABLE_STRICT_WARNINGS

#endif
