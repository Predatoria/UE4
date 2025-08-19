// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"

#if defined(EOS_VOICE_CHAT_SUPPORTED)

#include "VoiceChat.h"

struct FEOSVoiceDeviceInfo
{
    FString DisplayName;
    FString Id;
    bool bIsDefaultDevice;
};

class ONLINESUBSYSTEMREDPOINTEOS_API FEOSVoiceManagerLocalUser : public TSharedFromThis<FEOSVoiceManagerLocalUser>
{
    friend class FEOSVoiceManager;

private:
    EOS_HRTC EOSRTC;
    EOS_HRTCAudio EOSRTCAudio;
    TSharedRef<class FEOSVoiceManager> VoiceManager;
    TSharedRef<const FUniqueNetIdEOS> LocalUserId;
    TMap<FString, FString> Settings;
    bool bHasSetCurrentInputDevice;
    FEOSVoiceDeviceInfo CurrentInputDevice;
    bool bHasSetCurrentOutputDevice;
    FEOSVoiceDeviceInfo CurrentOutputDevice;
    TSharedPtr<EOSEventHandle<EOS_RTCAudio_AudioDevicesChangedCallbackInfo>> OnAudioDevicesChanged;
    bool bTransmitToAllChannels;
    TSet<FString> TransmitChannelNames;
    bool bIsSyncingTransmit;
    bool bHasPendingTransmitSync;
    // Volumes are values between 0.0f and 200.0f.
    float InputVolume;
    float OutputVolume;
    bool bInputMuted;
    bool bOutputMuted;

    class FEOSVoiceManagerLocalUserRemoteUser
    {
    public:
        TSharedRef<const FUniqueNetIdEOS> UserId;
        FEOSVoiceManagerLocalUserRemoteUser(TSharedRef<const FUniqueNetIdEOS> InUserId)
            : UserId(MoveTemp(InUserId))
            , bIsTalking(false)
            , bIsMuted(false){};

        bool bIsTalking;
        bool bIsMuted;
    };

    class FEOSVoiceManagerLocalUserJoinedChannel : public TSharedFromThis<FEOSVoiceManagerLocalUserJoinedChannel>
    {
    private:
        void RegisterEvents();

    public:
        /** Do not use this constructor; use New instead. */
        FEOSVoiceManagerLocalUserJoinedChannel(
            const TSharedRef<FEOSVoiceManagerLocalUser> &InOwner,
            EOS_HRTC InRTC,
            EOS_HRTCAudio InRTCAudio,
            const FString &InRoomName,
            EVoiceChatChannelType InChannelType);
        UE_NONCOPYABLE(FEOSVoiceManagerLocalUserJoinedChannel);

        static TSharedRef<FEOSVoiceManagerLocalUserJoinedChannel> New(
            const TSharedRef<FEOSVoiceManagerLocalUser> &InOwner,
            EOS_HRTC InRTC,
            EOS_HRTCAudio InRTCAudio,
            const FString &InRoomName,
            EVoiceChatChannelType InChannelType);
        virtual ~FEOSVoiceManagerLocalUserJoinedChannel();

        TSharedRef<FEOSVoiceManagerLocalUser> Owner;
        EOS_HRTC EOSRTC;
        EOS_HRTCAudio EOSRTCAudio;
        FString RoomName;
        const char *RoomNameStr;
        EVoiceChatChannelType ChannelType;
        TUserIdMap<TSharedPtr<FEOSVoiceManagerLocalUserRemoteUser>> Participants;
        TSharedPtr<EOSEventHandle<EOS_RTC_DisconnectedCallbackInfo>> OnDisconnected;
        TSharedPtr<EOSEventHandle<EOS_RTC_ParticipantStatusChangedCallbackInfo>> OnParticipantStatusChanged;
        TSharedPtr<EOSEventHandle<EOS_RTCAudio_ParticipantUpdatedCallbackInfo>> OnParticipantUpdated;
        TSharedPtr<EOSEventHandle<EOS_RTCAudio_AudioInputStateCallbackInfo>> OnAudioInputState;
        TSharedPtr<EOSEventHandle<EOS_RTCAudio_AudioOutputStateCallbackInfo>> OnAudioOutputState;
        // EOS_RTCAudio_AddNotifyAudioBeforeSend
        // EOS_RTCAudio_AddNotifyAudioBeforeRender
    };

    TMap<FString, TSharedPtr<FEOSVoiceManagerLocalUserJoinedChannel>> JoinedChannels;

    void SetPlayersBlockState(const TArray<FString> &PlayerNames, bool bBlocked);

    FOnVoiceChatAvailableAudioDevicesChangedDelegate OnVoiceChatAvailableAudioDevicesChangedDelegate;
    FOnVoiceChatChannelJoinedDelegate OnVoiceChatChannelJoinedDelegate;
    FOnVoiceChatChannelExitedDelegate OnVoiceChatChannelExitedDelegate;
    FOnVoiceChatCallStatsUpdatedDelegate OnVoiceChatCallStatsUpdatedDelegate;
    FOnVoiceChatPlayerAddedDelegate OnVoiceChatPlayerAddedDelegate;
    FOnVoiceChatPlayerRemovedDelegate OnVoiceChatPlayerRemovedDelegate;
    FOnVoiceChatPlayerTalkingUpdatedDelegate OnVoiceChatPlayerTalkingUpdatedDelegate;
    FOnVoiceChatPlayerMuteUpdatedDelegate OnVoiceChatPlayerMuteUpdatedDelegate;
    FOnVoiceChatPlayerVolumeUpdatedDelegate OnVoiceChatPlayerVolumeUpdatedDelegate;

    bool SyncInputDeviceSettings();
    bool SyncOutputDeviceSettings();
    bool SyncTransmitMode();

public:
    FEOSVoiceManagerLocalUser(
        EOS_HPlatform InPlatform,
        TSharedRef<class FEOSVoiceManager> InVoiceManager,
        TSharedRef<const FUniqueNetIdEOS> InLocalUserId);
    UE_NONCOPYABLE(FEOSVoiceManagerLocalUser);
    void RegisterEvents();

    void SetSetting(const FString &Name, const FString &Value);
    FString GetSetting(const FString &Name);
    void SetAudioInputVolume(float Volume);
    void SetAudioOutputVolume(float Volume);
    float GetAudioInputVolume() const;
    float GetAudioOutputVolume() const;
    void SetAudioInputDeviceMuted(bool bIsMuted);
    void SetAudioOutputDeviceMuted(bool bIsMuted);
    bool GetAudioInputDeviceMuted() const;
    bool GetAudioOutputDeviceMuted() const;
    TArray<FEOSVoiceDeviceInfo> GetAvailableInputDeviceInfos() const;
    TArray<FEOSVoiceDeviceInfo> GetAvailableOutputDeviceInfos() const;
    void SetInputDeviceId(const FString &InputDeviceId);
    void SetOutputDeviceId(const FString &OutputDeviceId);
    FEOSVoiceDeviceInfo GetInputDeviceInfo() const;
    FEOSVoiceDeviceInfo GetOutputDeviceInfo() const;
    FEOSVoiceDeviceInfo GetDefaultInputDeviceInfo() const;
    FEOSVoiceDeviceInfo GetDefaultOutputDeviceInfo() const;
    void BlockPlayers(const TArray<FString> &PlayerNames);
    void UnblockPlayers(const TArray<FString> &PlayerNames);
    void JoinChannel(
        const FString &ChannelName,
        const FString &ChannelCredentials,
        EVoiceChatChannelType ChannelType,
        const FOnVoiceChatChannelJoinCompleteDelegate &Delegate,
        const TOptional<FVoiceChatChannel3dProperties> &Channel3dProperties);
    void RegisterLobbyChannel(const FString &ChannelName, EVoiceChatChannelType ChannelType);
    void LeaveChannel(const FString &ChannelName, const FOnVoiceChatChannelLeaveCompleteDelegate &Delegate);
    void UnregisterLobbyChannel(const FString &ChannelName);
    void Set3DPosition(
        const FString &ChannelName,
        const FVector &SpeakerPosition,
        const FVector &ListenerPosition,
        const FVector &ListenerForwardDirection,
        const FVector &ListenerUpDirection);
    TArray<FString> GetChannels();
    const TArray<FString> GetPlayersInChannel(const FString &ChannelName) const;
    EVoiceChatChannelType GetChannelType(const FString &ChannelName) const;
    bool IsPlayerTalking(const FString &PlayerName) const;
    void SetPlayerMuted(const FString &PlayerName, bool bMuted);
    bool IsPlayerMuted(const FString &PlayerName) const;
    void SetPlayerVolume(const FString &PlayerName, float Volume);
    float GetPlayerVolume(const FString &PlayerName) const;
    void TransmitToAllChannels();
    void TransmitToNoChannels();
    void TransmitToSpecificChannel(const FString &ChannelName);
    EVoiceChatTransmitMode GetTransmitMode() const;
    FString GetTransmitChannel() const;

    FOnVoiceChatAvailableAudioDevicesChangedDelegate &OnVoiceChatAvailableAudioDevicesChanged();
    FOnVoiceChatChannelJoinedDelegate &OnVoiceChatChannelJoined();
    FOnVoiceChatChannelExitedDelegate &OnVoiceChatChannelExited();
    FOnVoiceChatCallStatsUpdatedDelegate &OnVoiceChatCallStatsUpdated();
    FOnVoiceChatPlayerAddedDelegate &OnVoiceChatPlayerAdded();
    FOnVoiceChatPlayerRemovedDelegate &OnVoiceChatPlayerRemoved();
    FOnVoiceChatPlayerTalkingUpdatedDelegate &OnVoiceChatPlayerTalkingUpdated();
    FOnVoiceChatPlayerMuteUpdatedDelegate &OnVoiceChatPlayerMuteUpdated();
    FOnVoiceChatPlayerVolumeUpdatedDelegate &OnVoiceChatPlayerVolumeUpdated();
};

#endif