// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/VoiceChat/VoiceManagerLocalUser.h"

#if defined(EOS_VOICE_CHAT_SUPPORTED)

#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/MultiOperation.h"
#include "OnlineSubsystemRedpointEOS/Shared/VoiceChat/VoiceManager.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"

FEOSVoiceManagerLocalUser::FEOSVoiceManagerLocalUserJoinedChannel::FEOSVoiceManagerLocalUserJoinedChannel(
    const TSharedRef<FEOSVoiceManagerLocalUser> &InOwner,
    EOS_HRTC InRTC,
    EOS_HRTCAudio InRTCAudio,
    const FString &InRoomName,
    EVoiceChatChannelType InChannelType)
    : Owner(InOwner)
    , EOSRTC(InRTC)
    , EOSRTCAudio(InRTCAudio)
    , RoomName(InRoomName)
    , RoomNameStr(nullptr)
    , ChannelType(InChannelType)
    , Participants()
    , OnDisconnected()
    , OnParticipantStatusChanged()
    , OnParticipantUpdated()
    , OnAudioInputState()
    , OnAudioOutputState()
{
    verify(EOSString_AnsiUnlimited::AllocateToCharBuffer(InRoomName, this->RoomNameStr) == EOS_EResult::EOS_Success);
}

void FEOSVoiceManagerLocalUser::FEOSVoiceManagerLocalUserJoinedChannel::RegisterEvents()
{
    EOS_RTC_AddNotifyDisconnectedOptions DisconnectOpts = {};
    DisconnectOpts.ApiVersion = EOS_RTC_ADDNOTIFYDISCONNECTED_API_LATEST;
    DisconnectOpts.LocalUserId = this->Owner->LocalUserId->GetProductUserId();
    DisconnectOpts.RoomName = this->RoomNameStr;
    this->OnDisconnected =
        EOSRegisterEvent<EOS_HRTC, EOS_RTC_AddNotifyDisconnectedOptions, EOS_RTC_DisconnectedCallbackInfo>(
            this->EOSRTC,
            &DisconnectOpts,
            EOS_RTC_AddNotifyDisconnected,
            EOS_RTC_RemoveNotifyDisconnected,
            [WeakThis = GetWeakThis(this)](const EOS_RTC_DisconnectedCallbackInfo *Data) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    if (Data->ResultCode == EOS_EResult::EOS_Success)
                    {
                        // Room left because of a call to LeaveRoom or because an RTC-enabled lobby was left.
                        This->Owner->OnVoiceChatChannelExited().Broadcast(
                            This->RoomName,
                            FVoiceChatResult(EVoiceChatResult::Success));
                        This->Owner->JoinedChannels.Remove(This->RoomName);
                    }
                    else if (Data->ResultCode == EOS_EResult::EOS_NoConnection)
                    // NOLINTNEXTLINE(bugprone-branch-clone)
                    {
                        // Temporary interruption.
                        // @todo: Will EOS automatically reconnect here?
                    }
                    else if (Data->ResultCode == EOS_EResult::EOS_RTC_UserKicked)
                    {
                        // User was kicked from the RTC lobby.
                        This->Owner->OnVoiceChatChannelExited().Broadcast(
                            This->RoomName,
                            FVoiceChatResult(
                                EVoiceChatResult::NotPermitted,
                                TEXT("KickedByServer"),
                                TEXT("You were kicked from voice chat by the server.")));
                        This->Owner->JoinedChannels.Remove(This->RoomName);
                    }
                    else if (Data->ResultCode == EOS_EResult::EOS_ServiceFailure)
                    {
                        // Temporary interruption.
                        // @todo: Will EOS automatically reconnect here?
                    }
                    else
                    {
                        // Unknown.
                        // @todo: Are we actually disconnected from the room here?
                    }
                }
            });

    EOS_RTC_AddNotifyParticipantStatusChangedOptions StatusOpts = {};
    StatusOpts.ApiVersion = EOS_RTC_ADDNOTIFYPARTICIPANTSTATUSCHANGED_API_LATEST;
    StatusOpts.LocalUserId = this->Owner->LocalUserId->GetProductUserId();
    StatusOpts.RoomName = this->RoomNameStr;
    this->OnParticipantStatusChanged = EOSRegisterEvent<
        EOS_HRTC,
        EOS_RTC_AddNotifyParticipantStatusChangedOptions,
        EOS_RTC_ParticipantStatusChangedCallbackInfo>(
        this->EOSRTC,
        &StatusOpts,
        EOS_RTC_AddNotifyParticipantStatusChanged,
        EOS_RTC_RemoveNotifyParticipantStatusChanged,
        [WeakThis = GetWeakThis(this)](const EOS_RTC_ParticipantStatusChangedCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Data->ParticipantStatus == EOS_ERTCParticipantStatus::EOS_RTCPS_Joined)
                {
                    TSharedRef<const FUniqueNetIdEOS> RemoteUserId = MakeShared<FUniqueNetIdEOS>(Data->ParticipantId);
                    // @todo: Grab metadata and find some way of surfacing it...
                    This->Participants.Add(
                        *RemoteUserId,
                        MakeShared<FEOSVoiceManagerLocalUserRemoteUser>(RemoteUserId));
                    This->Owner->OnVoiceChatPlayerAdded().Broadcast(This->RoomName, RemoteUserId->ToString());
                }
                else if (Data->ParticipantStatus == EOS_ERTCParticipantStatus::EOS_RTCPS_Left)
                {
                    TSharedRef<const FUniqueNetIdEOS> RemoteUserId = MakeShared<FUniqueNetIdEOS>(Data->ParticipantId);
                    This->Participants.Remove(*RemoteUserId);
                    This->Owner->OnVoiceChatPlayerRemoved().Broadcast(This->RoomName, RemoteUserId->ToString());
                }
            }
        });

    EOS_RTCAudio_AddNotifyParticipantUpdatedOptions UpdatedOpts = {};
    UpdatedOpts.ApiVersion = EOS_RTCAUDIO_ADDNOTIFYPARTICIPANTUPDATED_API_LATEST;
    UpdatedOpts.LocalUserId = this->Owner->LocalUserId->GetProductUserId();
    UpdatedOpts.RoomName = this->RoomNameStr;
    this->OnParticipantUpdated = EOSRegisterEvent<
        EOS_HRTCAudio,
        EOS_RTCAudio_AddNotifyParticipantUpdatedOptions,
        EOS_RTCAudio_ParticipantUpdatedCallbackInfo>(
        this->EOSRTCAudio,
        &UpdatedOpts,
        EOS_RTCAudio_AddNotifyParticipantUpdated,
        EOS_RTCAudio_RemoveNotifyParticipantUpdated,
        [WeakThis = GetWeakThis(this)](const EOS_RTCAudio_ParticipantUpdatedCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                TSharedRef<const FUniqueNetIdEOS> RemoteUserId = MakeShared<FUniqueNetIdEOS>(Data->ParticipantId);
                if (This->Participants.Contains(*RemoteUserId))
                {
                    auto &RemoteUser = This->Participants[*RemoteUserId];
                    bool bSpeaking = Data->bSpeaking == EOS_TRUE;
                    if (RemoteUser->bIsTalking != bSpeaking)
                    {
                        RemoteUser->bIsTalking = bSpeaking;
                        This->Owner->OnVoiceChatPlayerTalkingUpdated().Broadcast(
                            This->RoomName,
                            RemoteUserId->ToString(),
                            RemoteUser->bIsTalking);
                    }
                    bool bIsMuted = Data->AudioStatus != EOS_ERTCAudioStatus::EOS_RTCAS_Enabled;
                    if (RemoteUser->bIsMuted != bIsMuted)
                    {
                        FString AudioStatusString = TEXT("Unknown");
                        switch (Data->AudioStatus)
                        {
                        case EOS_ERTCAudioStatus::EOS_RTCAS_Enabled:
                            AudioStatusString = TEXT("Enabled");
                            break;
                        case EOS_ERTCAudioStatus::EOS_RTCAS_Disabled:
                            AudioStatusString = TEXT("Disabled");
                            break;
                        case EOS_ERTCAudioStatus::EOS_RTCAS_AdminDisabled:
                            AudioStatusString = TEXT("AdminDisabled");
                            break;
                        case EOS_ERTCAudioStatus::EOS_RTCAS_NotListeningDisabled:
                            AudioStatusString = TEXT("NotListeningDisabled");
                            break;
                        case EOS_ERTCAudioStatus::EOS_RTCAS_Unsupported:
                            AudioStatusString = TEXT("Unsupported");
                            break;
                        }
                        UE_LOG(
                            LogEOS,
                            Verbose,
                            TEXT("Voice chat participant status updated. Channel: '%s', Player: '%s', Audio Status: "
                                 "'%s'"),
                            *This->RoomName,
                            *RemoteUserId->ToString(),
                            *AudioStatusString);

                        RemoteUser->bIsTalking = bIsMuted;
                        This->Owner->OnVoiceChatPlayerMuteUpdated().Broadcast(
                            This->RoomName,
                            RemoteUserId->ToString(),
                            bIsMuted);
                    }
                }
            }
        });

    EOS_RTCAudio_AddNotifyAudioInputStateOptions InputStateOpts = {};
    InputStateOpts.ApiVersion = EOS_RTCAUDIO_ADDNOTIFYAUDIOINPUTSTATE_API_LATEST;
    InputStateOpts.LocalUserId = this->Owner->LocalUserId->GetProductUserId();
    InputStateOpts.RoomName = this->RoomNameStr;
    this->OnAudioInputState = EOSRegisterEvent<
        EOS_HRTCAudio,
        EOS_RTCAudio_AddNotifyAudioInputStateOptions,
        EOS_RTCAudio_AudioInputStateCallbackInfo>(
        this->EOSRTCAudio,
        &InputStateOpts,
        EOS_RTCAudio_AddNotifyAudioInputState,
        EOS_RTCAudio_RemoveNotifyAudioInputState,
        [WeakThis = GetWeakThis(this)](const EOS_RTCAudio_AudioInputStateCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
            }
        });

    EOS_RTCAudio_AddNotifyAudioOutputStateOptions OutputStateOpts = {};
    OutputStateOpts.ApiVersion = EOS_RTCAUDIO_ADDNOTIFYAUDIOOUTPUTSTATE_API_LATEST;
    OutputStateOpts.LocalUserId = this->Owner->LocalUserId->GetProductUserId();
    OutputStateOpts.RoomName = this->RoomNameStr;
    this->OnAudioOutputState = EOSRegisterEvent<
        EOS_HRTCAudio,
        EOS_RTCAudio_AddNotifyAudioOutputStateOptions,
        EOS_RTCAudio_AudioOutputStateCallbackInfo>(
        this->EOSRTCAudio,
        &OutputStateOpts,
        EOS_RTCAudio_AddNotifyAudioOutputState,
        EOS_RTCAudio_RemoveNotifyAudioOutputState,
        [WeakThis = GetWeakThis(this)](const EOS_RTCAudio_AudioOutputStateCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
            }
        });
}

TSharedRef<FEOSVoiceManagerLocalUser::FEOSVoiceManagerLocalUserJoinedChannel> FEOSVoiceManagerLocalUser::
    FEOSVoiceManagerLocalUserJoinedChannel::New(
        const TSharedRef<FEOSVoiceManagerLocalUser> &InOwner,
        EOS_HRTC InRTC,
        EOS_HRTCAudio InRTCAudio,
        const FString &InRoomName,
        EVoiceChatChannelType InChannelType)
{
    TSharedRef<FEOSVoiceManagerLocalUserJoinedChannel> Channel =
        MakeShared<FEOSVoiceManagerLocalUserJoinedChannel>(InOwner, InRTC, InRTCAudio, InRoomName, InChannelType);
    Channel->RegisterEvents();
    return Channel;
}

FEOSVoiceManagerLocalUser::FEOSVoiceManagerLocalUserJoinedChannel::~FEOSVoiceManagerLocalUserJoinedChannel()
{
    EOSString_AnsiUnlimited::FreeFromCharBuffer(this->RoomNameStr);
}

bool FEOSVoiceManagerLocalUser::SyncInputDeviceSettings()
{
    FString DeviceId;
    if (this->bHasSetCurrentInputDevice)
    {
        DeviceId = this->CurrentInputDevice.Id;
    }
    else
    {
        DeviceId = this->GetDefaultInputDeviceInfo().Id;
    }

    EOS_RTCAudio_SetAudioInputSettingsOptions Opts = {};
    Opts.ApiVersion = EOS_RTCAUDIO_SETAUDIOINPUTSETTINGS_API_LATEST;
    Opts.LocalUserId = this->LocalUserId->GetProductUserId();
    EOSString_AnsiUnlimited::AllocateToCharBuffer(DeviceId, Opts.DeviceId);
    // InputVolume divided by two, since EOS RTC uses a range of 0.0 - 100.0f to represent silence - 2x gain.
    Opts.Volume = this->bInputMuted ? 0.0f : (this->InputVolume / 2);
    if (this->Settings.Contains("PlatformAECEnabled"))
    {
        if (this->Settings["PlatformAECEnabled"] == "true")
        {
            Opts.bPlatformAEC = EOS_TRUE;
        }
        else
        {
            Opts.bPlatformAEC = EOS_FALSE;
        }
    }
    else
    {
        Opts.bPlatformAEC = this->VoiceManager->Config->GetEnableVoiceChatPlatformAECByDefault();
    }

    EOS_EResult Result = EOS_RTCAudio_SetAudioInputSettings(this->EOSRTCAudio, &Opts);
    EOSString_AnsiUnlimited::FreeFromCharBuffer(Opts.DeviceId);
    if (Result != EOS_EResult::EOS_Success)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Failed to set audio input options. Got result: %s"),
            ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
        return false;
    }
    return true;
}

bool FEOSVoiceManagerLocalUser::SyncOutputDeviceSettings()
{
    FString DeviceId;
    if (this->bHasSetCurrentOutputDevice)
    {
        DeviceId = this->CurrentOutputDevice.Id;
    }
    else
    {
        DeviceId = this->GetDefaultOutputDeviceInfo().Id;
    }

    EOS_RTCAudio_SetAudioOutputSettingsOptions Opts = {};
    Opts.ApiVersion = EOS_RTCAUDIO_SETAUDIOOUTPUTSETTINGS_API_LATEST;
    Opts.LocalUserId = this->LocalUserId->GetProductUserId();
    EOSString_AnsiUnlimited::AllocateToCharBuffer(DeviceId, Opts.DeviceId);
    // OutputVolume divided by two, since EOS RTC uses a range of 0.0 - 100.0f to represent silence - 2x gain.
    Opts.Volume = this->bOutputMuted ? 0.0f : (this->OutputVolume / 2);

    EOS_EResult Result = EOS_RTCAudio_SetAudioOutputSettings(this->EOSRTCAudio, &Opts);
    EOSString_AnsiUnlimited::FreeFromCharBuffer(Opts.DeviceId);
    if (Result != EOS_EResult::EOS_Success)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Failed to set audio output options. Got result: %s"),
            ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
        return false;
    }
    return true;
}

bool FEOSVoiceManagerLocalUser::SyncTransmitMode()
{
    if (this->bIsSyncingTransmit)
    {
        // We're already syncing transmit (since it's an asynchronous operation).
        // Ask the transmit sync to run again after it finishes.
        this->bHasPendingTransmitSync = true;
        return true;
    }

    this->bIsSyncingTransmit = true;
    this->bHasPendingTransmitSync = false;

    bool bTransmitToAllChannelsCached = this->bTransmitToAllChannels;
    TSet<FString> TransmitChannelNamesCached = TSet<FString>(this->TransmitChannelNames);

    TArray<FString> ChannelNames;
    this->JoinedChannels.GetKeys(ChannelNames);
    FMultiOperation<FString, bool>::Run(
        ChannelNames,
        [WeakThis = GetWeakThis(this), bTransmitToAllChannelsCached, TransmitChannelNamesCached](
            const FString &InChannelName,
            const std::function<void(bool OutValue)> &OnDone) -> bool {
            if (auto This = PinWeakThis(WeakThis))
            {
                bool bEnabled = bTransmitToAllChannelsCached || TransmitChannelNamesCached.Contains(InChannelName);

                EOS_RTCAudio_UpdateSendingOptions Opts = {};
                Opts.ApiVersion = EOS_RTCAUDIO_UPDATESENDING_API_LATEST;
                Opts.LocalUserId = This->LocalUserId->GetProductUserId();
                EOSString_AnsiUnlimited::AllocateToCharBuffer(InChannelName, Opts.RoomName);
                Opts.AudioStatus =
                    bEnabled ? EOS_ERTCAudioStatus::EOS_RTCAS_Enabled : EOS_ERTCAudioStatus::EOS_RTCAS_Disabled;
                EOSRunOperation<
                    EOS_HRTCAudio,
                    EOS_RTCAudio_UpdateSendingOptions,
                    EOS_RTCAudio_UpdateSendingCallbackInfo>(
                    This->EOSRTCAudio,
                    &Opts,
                    &EOS_RTCAudio_UpdateSending,
                    [WeakThis = GetWeakThis(This), OnDone](const EOS_RTCAudio_UpdateSendingCallbackInfo *Info) {
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            if (Info->ResultCode != EOS_EResult::EOS_Success)
                            {
                                UE_LOG(
                                    LogEOS,
                                    Error,
                                    TEXT("EOS_RTCAudio_UpdateSending operation failed when trying to sync transmit "
                                         "modes. User may be sending to fewer or greater channels than intended. "
                                         "Result code: %s"),
                                    ANSI_TO_TCHAR(EOS_EResult_ToString(Info->ResultCode)));
                            }

                            OnDone(true);
                        }
                    });
                return true;
            }
            return false;
        },
        [WeakThis = GetWeakThis(this)](const TArray<bool> &OutResults) {
            if (auto This = PinWeakThis(WeakThis))
            {
                This->bIsSyncingTransmit = false;

                // Kick off another sync operation if we need to.
                if (This->bHasPendingTransmitSync)
                {
                    This->SyncTransmitMode();
                }
            }
        });
    return true;
}

FEOSVoiceManagerLocalUser::FEOSVoiceManagerLocalUser(
    EOS_HPlatform InPlatform,
    TSharedRef<FEOSVoiceManager> InVoiceManager,
    TSharedRef<const FUniqueNetIdEOS> InLocalUserId)
    : EOSRTC(EOS_Platform_GetRTCInterface(InPlatform))
    , EOSRTCAudio(EOS_RTC_GetAudioInterface(EOSRTC))
    , VoiceManager(MoveTemp(InVoiceManager))
    , LocalUserId(MoveTemp(InLocalUserId))
    , Settings()
    , bHasSetCurrentInputDevice(false)
    , CurrentInputDevice()
    , bHasSetCurrentOutputDevice(false)
    , CurrentOutputDevice()
    , OnAudioDevicesChanged(nullptr)
    , bTransmitToAllChannels(true)
    , TransmitChannelNames()
    , bIsSyncingTransmit(false)
    , bHasPendingTransmitSync(false)
    , InputVolume(100.0f)
    , OutputVolume(100.0f)
    , bInputMuted(false)
    , bOutputMuted(false)
{
}

void FEOSVoiceManagerLocalUser::RegisterEvents()
{
    EOS_RTCAudio_AddNotifyAudioDevicesChangedOptions Opts = {};
    Opts.ApiVersion = EOS_RTCAUDIO_ADDNOTIFYAUDIODEVICESCHANGED_API_LATEST;
    this->OnAudioDevicesChanged = EOSRegisterEvent<
        EOS_HRTCAudio,
        EOS_RTCAudio_AddNotifyAudioDevicesChangedOptions,
        EOS_RTCAudio_AudioDevicesChangedCallbackInfo>(
        this->EOSRTCAudio,
        &Opts,
        EOS_RTCAudio_AddNotifyAudioDevicesChanged,
        EOS_RTCAudio_RemoveNotifyAudioDevicesChanged,
        [WeakThis = GetWeakThis(this)](const EOS_RTCAudio_AudioDevicesChangedCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                This->OnVoiceChatAvailableAudioDevicesChanged().Broadcast();
            }
        });
}

void FEOSVoiceManagerLocalUser::SetSetting(const FString &Name, const FString &Value)
{
    this->Settings.Add(Name, Value);
    if (Name == "PlatformAECEnabled")
    {
        this->SyncInputDeviceSettings();
    }
}

FString FEOSVoiceManagerLocalUser::GetSetting(const FString &Name)
{
    if (this->Settings.Contains(Name))
    {
        return this->Settings[Name];
    }
    return TEXT("");
}

void FEOSVoiceManagerLocalUser::SetAudioInputVolume(float Volume)
{
    float OldVolume = this->InputVolume;
    this->InputVolume = Volume;
    if (this->InputVolume < 0.0f)
    {
        this->InputVolume = 0.0f;
    }
    if (this->InputVolume > 200.0f)
    {
        this->InputVolume = 200.0f;
    }
    if (!this->SyncInputDeviceSettings())
    {
        this->InputVolume = OldVolume;
    }
}

void FEOSVoiceManagerLocalUser::SetAudioOutputVolume(float Volume)
{
    float OldVolume = this->OutputVolume;
    this->OutputVolume = Volume;
    if (this->OutputVolume < 0.0f)
    {
        this->OutputVolume = 0.0f;
    }
    if (this->OutputVolume > 200.0f)
    {
        this->OutputVolume = 200.0f;
    }
    if (!this->SyncOutputDeviceSettings())
    {
        this->OutputVolume = OldVolume;
    }
}

float FEOSVoiceManagerLocalUser::GetAudioInputVolume() const
{
    return this->InputVolume;
}

float FEOSVoiceManagerLocalUser::GetAudioOutputVolume() const
{
    return this->OutputVolume;
}

void FEOSVoiceManagerLocalUser::SetAudioInputDeviceMuted(bool bIsMuted)
{
    bool bOldMuted = this->bInputMuted;
    this->bInputMuted = bIsMuted;
    if (!this->SyncInputDeviceSettings())
    {
        this->bInputMuted = bOldMuted;
    }
}

void FEOSVoiceManagerLocalUser::SetAudioOutputDeviceMuted(bool bIsMuted)
{
    bool bOldMuted = this->bOutputMuted;
    this->bOutputMuted = bIsMuted;
    if (!this->SyncOutputDeviceSettings())
    {
        this->bOutputMuted = bOldMuted;
    }
}

bool FEOSVoiceManagerLocalUser::GetAudioInputDeviceMuted() const
{
    return this->bInputMuted;
}

bool FEOSVoiceManagerLocalUser::GetAudioOutputDeviceMuted() const
{
    return this->bOutputMuted;
}

TArray<FEOSVoiceDeviceInfo> FEOSVoiceManagerLocalUser::GetAvailableInputDeviceInfos() const
{
    TArray<FEOSVoiceDeviceInfo> Results;

    EOS_RTCAudio_GetAudioInputDevicesCountOptions CountOpts = {};
    CountOpts.ApiVersion = EOS_RTCAUDIO_GETAUDIOINPUTDEVICESCOUNT_API_LATEST;

    uint32_t DeviceCount = EOS_RTCAudio_GetAudioInputDevicesCount(this->EOSRTCAudio, &CountOpts);
    for (uint32_t i = 0; i < DeviceCount; i++)
    {
        EOS_RTCAudio_GetAudioInputDeviceByIndexOptions CopyOpts = {};
        CopyOpts.ApiVersion = EOS_RTCAUDIO_GETAUDIOINPUTDEVICEBYINDEX_API_LATEST;
        CopyOpts.DeviceInfoIndex = i;

        const EOS_RTCAudio_AudioInputDeviceInfo *DeviceInfo =
            EOS_RTCAudio_GetAudioInputDeviceByIndex(this->EOSRTCAudio, &CopyOpts);
        if (DeviceInfo != nullptr)
        {
            FEOSVoiceDeviceInfo Entry = {};
            Entry.Id = EOSString_AnsiUnlimited::FromAnsiString(DeviceInfo->DeviceId);
            Entry.DisplayName = EOSString_Utf8Unlimited::FromUtf8String(DeviceInfo->DeviceName);
            Entry.bIsDefaultDevice = DeviceInfo->bDefaultDevice ? EOS_TRUE : EOS_FALSE;
            Results.Add(Entry);
        }
    }

    return Results;
}

TArray<FEOSVoiceDeviceInfo> FEOSVoiceManagerLocalUser::GetAvailableOutputDeviceInfos() const
{
    TArray<FEOSVoiceDeviceInfo> Results;

    EOS_RTCAudio_GetAudioOutputDevicesCountOptions CountOpts = {};
    CountOpts.ApiVersion = EOS_RTCAUDIO_GETAUDIOOUTPUTDEVICESCOUNT_API_LATEST;

    uint32_t DeviceCount = EOS_RTCAudio_GetAudioOutputDevicesCount(this->EOSRTCAudio, &CountOpts);
    for (uint32_t i = 0; i < DeviceCount; i++)
    {
        EOS_RTCAudio_GetAudioOutputDeviceByIndexOptions CopyOpts = {};
        CopyOpts.ApiVersion = EOS_RTCAUDIO_GETAUDIOOUTPUTDEVICEBYINDEX_API_LATEST;
        CopyOpts.DeviceInfoIndex = i;

        const EOS_RTCAudio_AudioOutputDeviceInfo *DeviceInfo =
            EOS_RTCAudio_GetAudioOutputDeviceByIndex(this->EOSRTCAudio, &CopyOpts);
        if (DeviceInfo != nullptr)
        {
            FEOSVoiceDeviceInfo Entry = {};
            Entry.Id = EOSString_AnsiUnlimited::FromAnsiString(DeviceInfo->DeviceId);
            Entry.DisplayName = EOSString_Utf8Unlimited::FromUtf8String(DeviceInfo->DeviceName);
            Entry.bIsDefaultDevice = DeviceInfo->bDefaultDevice ? EOS_TRUE : EOS_FALSE;
            Results.Add(Entry);
        }
    }

    return Results;
}

void FEOSVoiceManagerLocalUser::SetInputDeviceId(const FString &InputDeviceId)
{
    bool bFoundTargetInputDevice = false;
    FEOSVoiceDeviceInfo TargetInputDeviceId;
    TArray<FEOSVoiceDeviceInfo> Results = this->GetAvailableInputDeviceInfos();
    for (const auto &Result : Results)
    {
        if (Result.Id == InputDeviceId)
        {
            TargetInputDeviceId = Result;
            bFoundTargetInputDevice = true;
            break;
        }
    }
    if (!bFoundTargetInputDevice)
    {
        UE_LOG(LogEOS, Error, TEXT("Failed to set audio input options. No such audio input device."));
        return;
    }

    FEOSVoiceDeviceInfo OldDevice = this->CurrentInputDevice;
    bool bOldHasSet = this->bHasSetCurrentInputDevice;
    this->CurrentInputDevice = TargetInputDeviceId;
    this->bHasSetCurrentInputDevice = true;
    if (!this->SyncInputDeviceSettings())
    {
        this->CurrentInputDevice = OldDevice;
        this->bHasSetCurrentInputDevice = bOldHasSet;
    }
}

void FEOSVoiceManagerLocalUser::SetOutputDeviceId(const FString &OutputDeviceId)
{
    bool bFoundTargetOutputDevice = false;
    FEOSVoiceDeviceInfo TargetOutputDeviceId;
    TArray<FEOSVoiceDeviceInfo> Results = this->GetAvailableOutputDeviceInfos();
    for (const auto &Result : Results)
    {
        if (Result.Id == OutputDeviceId)
        {
            TargetOutputDeviceId = Result;
            bFoundTargetOutputDevice = true;
            break;
        }
    }
    if (!bFoundTargetOutputDevice)
    {
        UE_LOG(LogEOS, Error, TEXT("Failed to set audio output options. No such audio output device."));
        return;
    }

    FEOSVoiceDeviceInfo OldDevice = this->CurrentOutputDevice;
    bool bOldHasSet = this->bHasSetCurrentOutputDevice;
    this->CurrentOutputDevice = TargetOutputDeviceId;
    this->bHasSetCurrentOutputDevice = true;
    if (!this->SyncOutputDeviceSettings())
    {
        this->CurrentOutputDevice = OldDevice;
        this->bHasSetCurrentOutputDevice = bOldHasSet;
    }
}

FEOSVoiceDeviceInfo FEOSVoiceManagerLocalUser::GetInputDeviceInfo() const
{
    if (this->bHasSetCurrentInputDevice)
    {
        return this->CurrentInputDevice;
    }
    return this->GetDefaultInputDeviceInfo();
}

FEOSVoiceDeviceInfo FEOSVoiceManagerLocalUser::GetOutputDeviceInfo() const
{
    if (this->bHasSetCurrentOutputDevice)
    {
        return this->CurrentOutputDevice;
    }
    return this->GetDefaultOutputDeviceInfo();
}

FEOSVoiceDeviceInfo FEOSVoiceManagerLocalUser::GetDefaultInputDeviceInfo() const
{
    TArray<FEOSVoiceDeviceInfo> Results = this->GetAvailableInputDeviceInfos();
    for (const auto &Result : Results)
    {
        if (Result.bIsDefaultDevice)
        {
            return Result;
        }
    }
    return FEOSVoiceDeviceInfo();
}

FEOSVoiceDeviceInfo FEOSVoiceManagerLocalUser::GetDefaultOutputDeviceInfo() const
{
    TArray<FEOSVoiceDeviceInfo> Results = this->GetAvailableOutputDeviceInfos();
    for (const auto &Result : Results)
    {
        if (Result.bIsDefaultDevice)
        {
            return Result;
        }
    }
    return FEOSVoiceDeviceInfo();
}

void FEOSVoiceManagerLocalUser::SetPlayersBlockState(const TArray<FString> &PlayerNames, bool bBlocked)
{
    for (const auto &PlayerName : PlayerNames)
    {
        TSharedPtr<const FUniqueNetIdEOS> ParticipantId = FUniqueNetIdEOS::ParseFromString(PlayerName);
        if (ParticipantId.IsValid())
        {
            for (const auto &Channel : this->JoinedChannels)
            {
                if (Channel.Value->Participants.Contains(*ParticipantId))
                {
                    EOS_RTC_BlockParticipantOptions Opts = {};
                    Opts.ApiVersion = EOS_RTC_BLOCKPARTICIPANT_API_LATEST;
                    Opts.bBlocked = bBlocked;
                    Opts.LocalUserId = this->LocalUserId->GetProductUserId();
                    Opts.ParticipantId = ParticipantId->GetProductUserId();
                    EOSString_AnsiUnlimited::AllocateToCharBuffer(Channel.Value->RoomName, Opts.RoomName);

                    EOSRunOperation<EOS_HRTC, EOS_RTC_BlockParticipantOptions, EOS_RTC_BlockParticipantCallbackInfo>(
                        this->EOSRTC,
                        &Opts,
                        &EOS_RTC_BlockParticipant,
                        [ParticipantId, bBlocked, RoomName = Channel.Value->RoomName, RoomNameBuffer = Opts.RoomName](
                            const EOS_RTC_BlockParticipantCallbackInfo *Data) {
                            EOSString_AnsiUnlimited::FreeFromCharBufferConst(RoomNameBuffer);
                            if (Data->ResultCode != EOS_EResult::EOS_Success)
                            {
                                UE_LOG(
                                    LogEOS,
                                    Warning,
                                    TEXT("EOS_RTC_BlockParticipant operation (%s) for remote user '%s' in room '%s' "
                                         "failed "
                                         "with result code %s"),
                                    bBlocked ? TEXT("blocking") : TEXT("unblocking"),
                                    *ParticipantId->ToString(),
                                    *RoomName,
                                    ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
                            }
                        });
                }
            }
        }
    }
}

void FEOSVoiceManagerLocalUser::BlockPlayers(const TArray<FString> &PlayerNames)
{
    this->SetPlayersBlockState(PlayerNames, true);
}

void FEOSVoiceManagerLocalUser::UnblockPlayers(const TArray<FString> &PlayerNames)
{
    this->SetPlayersBlockState(PlayerNames, false);
}

void FEOSVoiceManagerLocalUser::JoinChannel(
    const FString &ChannelName,
    const FString &ChannelCredentials,
    EVoiceChatChannelType ChannelType,
    const FOnVoiceChatChannelJoinCompleteDelegate &Delegate,
    const TOptional<FVoiceChatChannel3dProperties> &Channel3dProperties)
{
    if (ChannelType == EVoiceChatChannelType::Positional)
    {
        Delegate.ExecuteIfBound(
            ChannelName,
            FVoiceChatResult(
                EVoiceChatResult::InvalidArgument,
                TEXT("PositionalChannelsNotSupported"),
                TEXT("EOS does not support positional voice chat channels yet.")));
        return;
    }

    if (this->JoinedChannels.Contains(ChannelName))
    {
        Delegate.ExecuteIfBound(
            ChannelName,
            FVoiceChatResult(
                EVoiceChatResult::InvalidArgument,
                TEXT("AlreadyJoined"),
                TEXT("This user is already connected to that voice chat channel.")));
    }

    FString ClientBaseUrl, ParticipantToken;
    ChannelCredentials
        .Split(TEXT("?token="), &ClientBaseUrl, &ParticipantToken, ESearchCase::IgnoreCase, ESearchDir::FromEnd);

    EOS_RTC_JoinRoomOptions Opts = {};
    Opts.ApiVersion = EOS_RTC_JOINROOM_API_LATEST;
    Opts.LocalUserId = this->LocalUserId->GetProductUserId();
    verify(EOSString_AnsiUnlimited::AllocateToCharBuffer(ChannelName, Opts.RoomName) == EOS_EResult::EOS_Success);
    verify(
        EOSString_AnsiUnlimited::AllocateToCharBuffer(ClientBaseUrl, Opts.ClientBaseUrl) == EOS_EResult::EOS_Success);
    verify(
        EOSString_AnsiUnlimited::AllocateToCharBuffer(ParticipantToken, Opts.ParticipantToken) ==
        EOS_EResult::EOS_Success);
    Opts.ParticipantId = nullptr;
    Opts.Flags = ChannelType == EVoiceChatChannelType::Echo ? EOS_RTC_JOINROOMFLAGS_ENABLE_ECHO : 0x0;
    Opts.bManualAudioInputEnabled = false;
    Opts.bManualAudioOutputEnabled = false;

    EOSRunOperation<EOS_HRTC, EOS_RTC_JoinRoomOptions, EOS_RTC_JoinRoomCallbackInfo>(
        this->EOSRTC,
        &Opts,
        &EOS_RTC_JoinRoom,
        [WeakThis = GetWeakThis(this), Opts, Delegate, ChannelName, ChannelType](
            const EOS_RTC_JoinRoomCallbackInfo *Data) {
            EOSString_AnsiUnlimited::FreeFromCharBufferConst(Opts.RoomName);
            EOSString_AnsiUnlimited::FreeFromCharBufferConst(Opts.ClientBaseUrl);
            EOSString_AnsiUnlimited::FreeFromCharBufferConst(Opts.ParticipantToken);

            if (auto This = PinWeakThis(WeakThis))
            {
                if (Data->ResultCode == EOS_EResult::EOS_Success)
                {
                    if (!This->JoinedChannels.Contains(ChannelName))
                    {
                        This->JoinedChannels.Add(
                            ChannelName,
                            FEOSVoiceManagerLocalUserJoinedChannel::New(
                                This.ToSharedRef(),
                                This->EOSRTC,
                                This->EOSRTCAudio,
                                ChannelName,
                                ChannelType));
                    }
                    else
                    {
                        UE_LOG(
                            LogEOS,
                            Warning,
                            TEXT("%s"),
                            *OnlineRedpointEOS::Errors::DuplicateNotAllowed(
                                 ANSI_TO_TCHAR(__FUNCTION__),
                                 TEXT("The user was already in this chat channel by the time EOS_RTC_JoinRoom "
                                      "completed successfully. The voice chat system state may be inconsistent."))
                                 .ToLogString());
                    }
                    This->SyncTransmitMode();
                    This->OnVoiceChatChannelJoined().Broadcast(ChannelName);
                    Delegate.ExecuteIfBound(ChannelName, FVoiceChatResult(EVoiceChatResult::Success));
                }
                else
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("Failed to join RTC channel: %s"),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
                    if (Data->ResultCode == EOS_EResult::EOS_NoConnection)
                    {
                        Delegate.ExecuteIfBound(ChannelName, FVoiceChatResult(EVoiceChatResult::ConnectionFailure));
                    }
                    else if (Data->ResultCode == EOS_EResult::EOS_InvalidAuth)
                    {
                        Delegate.ExecuteIfBound(ChannelName, FVoiceChatResult(EVoiceChatResult::CredentialsInvalid));
                    }
                    else if (Data->ResultCode == EOS_EResult::EOS_RTC_TooManyParticipants)
                    {
                        Delegate.ExecuteIfBound(ChannelName, FVoiceChatResult(EVoiceChatResult::NotPermitted));
                    }
                    else if (Data->ResultCode == EOS_EResult::EOS_AccessDenied)
                    {
                        Delegate.ExecuteIfBound(ChannelName, FVoiceChatResult(EVoiceChatResult::InvalidArgument));
                    }
                    else
                    {
                        Delegate.ExecuteIfBound(ChannelName, FVoiceChatResult(EVoiceChatResult::ImplementationError));
                    }
                }
            }
        });
}

void FEOSVoiceManagerLocalUser::RegisterLobbyChannel(const FString &ChannelName, EVoiceChatChannelType ChannelType)
{
    if (!this->JoinedChannels.Contains(ChannelName))
    {
        this->JoinedChannels.Add(
            ChannelName,
            FEOSVoiceManagerLocalUserJoinedChannel::New(
                this->AsShared(),
                this->EOSRTC,
                this->EOSRTCAudio,
                ChannelName,
                    ChannelType));
    }
    else
    {
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("%s"),
            *OnlineRedpointEOS::Errors::DuplicateNotAllowed(
                 ANSI_TO_TCHAR(__FUNCTION__),
                 TEXT("The user was already in this chat channel by the time RegisterLobbyChannel "
                      "was called. The voice chat system state may be inconsistent."))
                 .ToLogString());
    }
    this->SyncTransmitMode();
    this->OnVoiceChatChannelJoined().Broadcast(ChannelName);
}

void FEOSVoiceManagerLocalUser::LeaveChannel(
    const FString &ChannelName,
    const FOnVoiceChatChannelLeaveCompleteDelegate &Delegate)
{
    EOS_RTC_LeaveRoomOptions Opts = {};
    Opts.ApiVersion = EOS_RTC_LEAVEROOM_API_LATEST;
    Opts.LocalUserId = this->LocalUserId->GetProductUserId();
    verify(EOSString_AnsiUnlimited::AllocateToCharBuffer(ChannelName, Opts.RoomName) == EOS_EResult::EOS_Success);

    EOSRunOperation<EOS_HRTC, EOS_RTC_LeaveRoomOptions, EOS_RTC_LeaveRoomCallbackInfo>(
        this->EOSRTC,
        &Opts,
        &EOS_RTC_LeaveRoom,
        [WeakThis = GetWeakThis(this), Opts, Delegate, ChannelName](const EOS_RTC_LeaveRoomCallbackInfo *Data) {
            EOSString_AnsiUnlimited::FreeFromCharBufferConst(Opts.RoomName);

            if (auto This = PinWeakThis(WeakThis))
            {
                if (Data->ResultCode == EOS_EResult::EOS_Success)
                {
                    if (This->JoinedChannels.Contains(ChannelName))
                    {
                        This->JoinedChannels.Remove(ChannelName);
                        This->OnVoiceChatChannelExited().Broadcast(
                            ChannelName,
                            FVoiceChatResult(EVoiceChatResult::Success));
                    }
                    Delegate.ExecuteIfBound(ChannelName, FVoiceChatResult(EVoiceChatResult::Success));
                }
                else
                {
                    UE_LOG(
                        LogEOS,
                        Error,
                        TEXT("Failed to leave RTC channel: %s"),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
                    if (Data->ResultCode == EOS_EResult::EOS_AccessDenied)
                    {
                        Delegate.ExecuteIfBound(ChannelName, FVoiceChatResult(EVoiceChatResult::InvalidArgument));
                    }
                    else
                    {
                        Delegate.ExecuteIfBound(ChannelName, FVoiceChatResult(EVoiceChatResult::ImplementationError));
                    }
                }
            }
        });
}

void FEOSVoiceManagerLocalUser::UnregisterLobbyChannel(const FString &ChannelName)
{
    if (this->JoinedChannels.Contains(ChannelName))
    {
        this->JoinedChannels.Remove(ChannelName);
        this->OnVoiceChatChannelExited().Broadcast(ChannelName, FVoiceChatResult(EVoiceChatResult::Success));
    }
}

void FEOSVoiceManagerLocalUser::Set3DPosition(
    const FString &ChannelName,
    const FVector &SpeakerPosition,
    const FVector &ListenerPosition,
    const FVector &ListenerForwardDirection,
    const FVector &ListenerUpDirection)
{
    UE_LOG(
        LogEOS,
        Error,
        TEXT("EOS does not support calling Set3DPosition, because positional voice channels are not supported yet."));
}

TArray<FString> FEOSVoiceManagerLocalUser::GetChannels()
{
    TArray<FString> Keys;
    this->JoinedChannels.GenerateKeyArray(Keys);
    return Keys;
}

const TArray<FString> FEOSVoiceManagerLocalUser::GetPlayersInChannel(const FString &ChannelName) const
{
    TArray<FString> PlayerIds;
    if (this->JoinedChannels.Contains(ChannelName))
    {
        for (const auto &RemoteUser : this->JoinedChannels[ChannelName]->Participants)
        {
            PlayerIds.Add(RemoteUser.Key->ToString());
        }
    }
    return PlayerIds;
}

EVoiceChatChannelType FEOSVoiceManagerLocalUser::GetChannelType(const FString &ChannelName) const
{
    if (this->JoinedChannels.Contains(ChannelName))
    {
        return this->JoinedChannels[ChannelName]->ChannelType;
    }

    return EVoiceChatChannelType::NonPositional;
}

bool FEOSVoiceManagerLocalUser::IsPlayerTalking(const FString &PlayerName) const
{
    if (PlayerName == this->LocalUserId->ToString())
    {
        // @todo: We might need to get this locally, if the local player doesn't count as a participant.
    }

    TSharedPtr<const FUniqueNetIdEOS> ParticipantId = FUniqueNetIdEOS::ParseFromString(PlayerName);
    if (ParticipantId.IsValid())
    {
        for (const auto &Channel : this->JoinedChannels)
        {
            if (Channel.Value->Participants.Contains(*ParticipantId))
            {
                if (Channel.Value->Participants[*ParticipantId]->bIsTalking)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void FEOSVoiceManagerLocalUser::SetPlayerMuted(const FString &PlayerName, bool bMuted)
{
    TSharedPtr<const FUniqueNetIdEOS> ParticipantId = FUniqueNetIdEOS::ParseFromString(PlayerName);
    if (ParticipantId.IsValid())
    {
        if (*ParticipantId == *this->LocalUserId)
        {
            // We're trying to mute ourselves, not a remote player.
            this->SetAudioInputDeviceMuted(true);
            return;
        }

        for (const auto &Channel : this->JoinedChannels)
        {
            if (Channel.Value->Participants.Contains(*ParticipantId))
            {
                EOS_RTCAudio_UpdateReceivingOptions Opts = {};
                Opts.ApiVersion = EOS_RTCAUDIO_UPDATERECEIVING_API_LATEST;
                Opts.LocalUserId = this->LocalUserId->GetProductUserId();
                Opts.ParticipantId = ParticipantId->GetProductUserId();
                Opts.RoomName = Channel.Value->RoomNameStr;
                Opts.bAudioEnabled = bMuted ? EOS_FALSE : EOS_TRUE;

                EOSRunOperation<
                    EOS_HRTCAudio,
                    EOS_RTCAudio_UpdateReceivingOptions,
                    EOS_RTCAudio_UpdateReceivingCallbackInfo>(
                    this->EOSRTCAudio,
                    &Opts,
                    &EOS_RTCAudio_UpdateReceiving,
                    [bMuted, ParticipantId, RoomName = Channel.Key](
                        const EOS_RTCAudio_UpdateReceivingCallbackInfo *Data) {
                        if (Data->ResultCode != EOS_EResult::EOS_Success)
                        {
                            UE_LOG(
                                LogEOS,
                                Warning,
                                TEXT("EOS_RTCAudio_UpdateReceiving operation (%s) for remote user '%s' in room '%s' "
                                     "failed "
                                     "with result code %s"),
                                bMuted ? TEXT("muting") : TEXT("unmuting"),
                                *ParticipantId->ToString(),
                                *RoomName,
                                ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
                        }
                    });
            }
        }
    }
}

bool FEOSVoiceManagerLocalUser::IsPlayerMuted(const FString &PlayerName) const
{
    TSharedPtr<const FUniqueNetIdEOS> ParticipantId = FUniqueNetIdEOS::ParseFromString(PlayerName);
    if (ParticipantId.IsValid())
    {
        if (*ParticipantId == *this->LocalUserId)
        {
            // We're trying to check if we're muted ourselves, not a remote player.
            return this->GetAudioInputDeviceMuted();
        }

        for (const auto &Channel : this->JoinedChannels)
        {
            if (Channel.Value->Participants.Contains(*ParticipantId))
            {
                if (Channel.Value->Participants[*ParticipantId]->bIsMuted)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void FEOSVoiceManagerLocalUser::SetPlayerVolume(const FString &PlayerName, float Volume)
{
    if (Volume == 0.0f)
    {
        this->SetPlayerMuted(PlayerName, true);
    }
    else
    {
        this->SetPlayerMuted(PlayerName, false);
    }
}

float FEOSVoiceManagerLocalUser::GetPlayerVolume(const FString &PlayerName) const
{
    return this->IsPlayerMuted(PlayerName) ? 0.0f : 100.0f;
}

void FEOSVoiceManagerLocalUser::TransmitToAllChannels()
{
    this->bTransmitToAllChannels = true;
    this->TransmitChannelNames.Empty();
    this->SyncTransmitMode();
}

void FEOSVoiceManagerLocalUser::TransmitToNoChannels()
{
    this->bTransmitToAllChannels = false;
    this->TransmitChannelNames.Empty();
    this->SyncTransmitMode();
}

void FEOSVoiceManagerLocalUser::TransmitToSpecificChannel(const FString &ChannelName)
{
    this->bTransmitToAllChannels = false;
    this->TransmitChannelNames.Add(ChannelName);
    this->SyncTransmitMode();
}

EVoiceChatTransmitMode FEOSVoiceManagerLocalUser::GetTransmitMode() const
{
    if (this->bTransmitToAllChannels)
    {
        return EVoiceChatTransmitMode::All;
    }
    else if (this->TransmitChannelNames.Num() == 0)
    {
        return EVoiceChatTransmitMode::None;
    }
    else
    {
        return EVoiceChatTransmitMode::Channel;
    }
}

FString FEOSVoiceManagerLocalUser::GetTransmitChannel() const
{
    if (this->bTransmitToAllChannels)
    {
        return TEXT("");
    }

    return FString::Join(this->TransmitChannelNames, TEXT(","));
}

FOnVoiceChatAvailableAudioDevicesChangedDelegate &FEOSVoiceManagerLocalUser::OnVoiceChatAvailableAudioDevicesChanged()
{
    return this->OnVoiceChatAvailableAudioDevicesChangedDelegate;
}

FOnVoiceChatChannelJoinedDelegate &FEOSVoiceManagerLocalUser::OnVoiceChatChannelJoined()
{
    return this->OnVoiceChatChannelJoinedDelegate;
}

FOnVoiceChatChannelExitedDelegate &FEOSVoiceManagerLocalUser::OnVoiceChatChannelExited()
{
    return this->OnVoiceChatChannelExitedDelegate;
}

FOnVoiceChatCallStatsUpdatedDelegate &FEOSVoiceManagerLocalUser::OnVoiceChatCallStatsUpdated()
{
    return this->OnVoiceChatCallStatsUpdatedDelegate;
}

FOnVoiceChatPlayerAddedDelegate &FEOSVoiceManagerLocalUser::OnVoiceChatPlayerAdded()
{
    return this->OnVoiceChatPlayerAddedDelegate;
}

FOnVoiceChatPlayerRemovedDelegate &FEOSVoiceManagerLocalUser::OnVoiceChatPlayerRemoved()
{
    return this->OnVoiceChatPlayerRemovedDelegate;
}

FOnVoiceChatPlayerTalkingUpdatedDelegate &FEOSVoiceManagerLocalUser::OnVoiceChatPlayerTalkingUpdated()
{
    return this->OnVoiceChatPlayerTalkingUpdatedDelegate;
}

FOnVoiceChatPlayerMuteUpdatedDelegate &FEOSVoiceManagerLocalUser::OnVoiceChatPlayerMuteUpdated()
{
    return this->OnVoiceChatPlayerMuteUpdatedDelegate;
}

FOnVoiceChatPlayerVolumeUpdatedDelegate &FEOSVoiceManagerLocalUser::OnVoiceChatPlayerVolumeUpdated()
{
    return this->OnVoiceChatPlayerVolumeUpdatedDelegate;
}

#endif