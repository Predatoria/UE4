// Copyright June Rhodes. All Rights Reserved.

#include "RedpointEOSVoiceChat.h"

#if defined(EOS_VOICE_CHAT_SUPPORTED)

#include "RedpointEOSVoiceChatModule.h"
#include "RedpointEOSVoiceChatUser.h"

FRedpointEOSVoiceChat::FRedpointEOSVoiceChat()
    : PrimaryUser(MakeShared<FRedpointEOSVoiceChatUser>())
{
}

// IVoiceChat Interface

void FRedpointEOSVoiceChat::LogoutAndReleaseAllUsers()
{
    if (this->PrimaryUser->IsLoggedIn())
    {
        this->PrimaryUser->Logout(FOnVoiceChatLogoutCompleteDelegate());
    }
    for (const auto &User : this->AdditionalUsers)
    {
        if (User->IsLoggedIn())
        {
            User->Logout(FOnVoiceChatLogoutCompleteDelegate());
        }
    }
    this->AdditionalUsers.Empty();
}

bool FRedpointEOSVoiceChat::Initialize()
{
    UE_LOG(
        LogEOSVoiceChat,
        Warning,
        TEXT("IVoiceChat::Initialize called, but this operation is unnecessary because IsInitialized already returns "
             "true."));
    return true;
}

bool FRedpointEOSVoiceChat::Uninitialize()
{
    UE_LOG(
        LogEOSVoiceChat,
        Warning,
        TEXT("IVoiceChat::Uninitialize called, but this operation is not supported for EOS Voice Chat."));
    return false;
}

void FRedpointEOSVoiceChat::Initialize(const FOnVoiceChatInitializeCompleteDelegate &Delegate)
{
    Delegate.ExecuteIfBound(FVoiceChatResult(EVoiceChatResult::Success));
}

void FRedpointEOSVoiceChat::Uninitialize(const FOnVoiceChatUninitializeCompleteDelegate &Delegate)
{
    Delegate.ExecuteIfBound(FVoiceChatResult(
        EVoiceChatResult::InvalidArgument,
        TEXT("UninitializeNotPermitted"),
        TEXT("EOS voice chat does not support uninitialization at a global level.")));
}

bool FRedpointEOSVoiceChat::IsInitialized() const
{
    // We're always initialized, as it is IVoiceChatUser::Login that connects to the subsystem.
    return true;
}

void FRedpointEOSVoiceChat::Connect(const FOnVoiceChatConnectCompleteDelegate &Delegate)
{
    Delegate.ExecuteIfBound(FVoiceChatResult(EVoiceChatResult::Success));
}

void FRedpointEOSVoiceChat::Disconnect(const FOnVoiceChatDisconnectCompleteDelegate &Delegate)
{
    Delegate.ExecuteIfBound(FVoiceChatResult(
        EVoiceChatResult::InvalidArgument,
        TEXT("DisconnectNotPermitted"),
        TEXT("EOS voice chat does not support disconnection at a global level.")));
}

bool FRedpointEOSVoiceChat::IsConnecting() const
{
    // We're always "connected" because we don't have this information at a global level.
    return false;
}

bool FRedpointEOSVoiceChat::IsConnected() const
{
    // We're always "connected" because we don't have this information at a global level.
    return true;
}

IVoiceChatUser *FRedpointEOSVoiceChat::CreateUser()
{
    TSharedRef<FRedpointEOSVoiceChatUser> User = MakeShared<FRedpointEOSVoiceChatUser>();
    this->AdditionalUsers.Add(User);
    return &User.Get();
}

void FRedpointEOSVoiceChat::ReleaseUser(IVoiceChatUser *VoiceChatUser)
{
    for (int i = this->AdditionalUsers.Num() - 1; i >= 0; i--)
    {
        if (&this->AdditionalUsers[i].Get() == VoiceChatUser)
        {
            this->AdditionalUsers.RemoveAt(i);
        }
    }
}

// IVoiceChat Events

FOnVoiceChatConnectedDelegate &FRedpointEOSVoiceChat::OnVoiceChatConnected()
{
    return this->OnVoiceChatConnectedDelegate;
}

FOnVoiceChatDisconnectedDelegate &FRedpointEOSVoiceChat::OnVoiceChatDisconnected()
{
    return this->OnVoiceChatDisconnectedDelegate;
}

FOnVoiceChatReconnectedDelegate &FRedpointEOSVoiceChat::OnVoiceChatReconnected()
{
    return this->OnVoiceChatReconnectedDelegate;
}

// IVoiceChatUser Interface

void FRedpointEOSVoiceChat::SetSetting(const FString &Name, const FString &Value)
{
    this->PrimaryUser->SetSetting(Name, Value);
}

FString FRedpointEOSVoiceChat::GetSetting(const FString &Name)
{
    return this->PrimaryUser->GetSetting(Name);
}

void FRedpointEOSVoiceChat::SetAudioInputVolume(float Volume)
{
    this->PrimaryUser->SetAudioInputVolume(Volume);
}

void FRedpointEOSVoiceChat::SetAudioOutputVolume(float Volume)
{
    this->PrimaryUser->SetAudioOutputVolume(Volume);
}

float FRedpointEOSVoiceChat::GetAudioInputVolume() const
{
    return this->PrimaryUser->GetAudioInputVolume();
}

float FRedpointEOSVoiceChat::GetAudioOutputVolume() const
{
    return this->PrimaryUser->GetAudioOutputVolume();
}

void FRedpointEOSVoiceChat::SetAudioInputDeviceMuted(bool bIsMuted)
{
    this->PrimaryUser->SetAudioInputDeviceMuted(bIsMuted);
}

void FRedpointEOSVoiceChat::SetAudioOutputDeviceMuted(bool bIsMuted)
{
    this->PrimaryUser->SetAudioOutputDeviceMuted(bIsMuted);
}

bool FRedpointEOSVoiceChat::GetAudioInputDeviceMuted() const
{
    return this->PrimaryUser->GetAudioInputDeviceMuted();
}

bool FRedpointEOSVoiceChat::GetAudioOutputDeviceMuted() const
{
    return this->PrimaryUser->GetAudioOutputDeviceMuted();
}

TArray<FVoiceChatDeviceInfo> FRedpointEOSVoiceChat::GetAvailableInputDeviceInfos() const
{
    return this->PrimaryUser->GetAvailableInputDeviceInfos();
}

TArray<FVoiceChatDeviceInfo> FRedpointEOSVoiceChat::GetAvailableOutputDeviceInfos() const
{
    return this->PrimaryUser->GetAvailableOutputDeviceInfos();
}

void FRedpointEOSVoiceChat::SetInputDeviceId(const FString &InputDeviceId)
{
    return this->PrimaryUser->SetInputDeviceId(InputDeviceId);
}

void FRedpointEOSVoiceChat::SetOutputDeviceId(const FString &OutputDeviceId)
{
    return this->PrimaryUser->SetOutputDeviceId(OutputDeviceId);
}

FVoiceChatDeviceInfo FRedpointEOSVoiceChat::GetInputDeviceInfo() const
{
    return this->PrimaryUser->GetInputDeviceInfo();
}

FVoiceChatDeviceInfo FRedpointEOSVoiceChat::GetOutputDeviceInfo() const
{
    return this->PrimaryUser->GetOutputDeviceInfo();
}

FVoiceChatDeviceInfo FRedpointEOSVoiceChat::GetDefaultInputDeviceInfo() const
{
    return this->PrimaryUser->GetDefaultInputDeviceInfo();
}

FVoiceChatDeviceInfo FRedpointEOSVoiceChat::GetDefaultOutputDeviceInfo() const
{
    return this->PrimaryUser->GetDefaultOutputDeviceInfo();
}

void FRedpointEOSVoiceChat::Login(
    FPlatformUserId PlatformId,
    const FString &PlayerName,
    const FString &Credentials,
    const FOnVoiceChatLoginCompleteDelegate &Delegate)
{
    this->PrimaryUser->Login(PlatformId, PlayerName, Credentials, Delegate);
}

void FRedpointEOSVoiceChat::Logout(const FOnVoiceChatLogoutCompleteDelegate &Delegate)
{
    this->PrimaryUser->Logout(Delegate);
}

bool FRedpointEOSVoiceChat::IsLoggingIn() const
{
    return this->PrimaryUser->IsLoggingIn();
}

bool FRedpointEOSVoiceChat::IsLoggedIn() const
{
    return this->PrimaryUser->IsLoggedIn();
}

FString FRedpointEOSVoiceChat::GetLoggedInPlayerName() const
{
    return this->PrimaryUser->GetLoggedInPlayerName();
}

void FRedpointEOSVoiceChat::BlockPlayers(const TArray<FString> &PlayerNames)
{
    this->PrimaryUser->BlockPlayers(PlayerNames);
}

void FRedpointEOSVoiceChat::UnblockPlayers(const TArray<FString> &PlayerNames)
{
    this->PrimaryUser->UnblockPlayers(PlayerNames);
}

void FRedpointEOSVoiceChat::JoinChannel(
    const FString &ChannelName,
    const FString &ChannelCredentials,
    EVoiceChatChannelType ChannelType,
    const FOnVoiceChatChannelJoinCompleteDelegate &Delegate,
    TOptional<FVoiceChatChannel3dProperties> Channel3dProperties)
{
    this->PrimaryUser->JoinChannel(ChannelName, ChannelCredentials, ChannelType, Delegate, Channel3dProperties);
}

void FRedpointEOSVoiceChat::LeaveChannel(
    const FString &ChannelName,
    const FOnVoiceChatChannelLeaveCompleteDelegate &Delegate)
{
    this->PrimaryUser->LeaveChannel(ChannelName, Delegate);
}

void FRedpointEOSVoiceChat::Set3DPosition(
    const FString &ChannelName,
    const FVector &SpeakerPosition,
    const FVector &ListenerPosition,
    const FVector &ListenerForwardDirection,
    const FVector &ListenerUpDirection)
{
    this->PrimaryUser
        ->Set3DPosition(ChannelName, SpeakerPosition, ListenerPosition, ListenerForwardDirection, ListenerUpDirection);
}

TArray<FString> FRedpointEOSVoiceChat::GetChannels() const
{
    return this->PrimaryUser->GetChannels();
}

TArray<FString> FRedpointEOSVoiceChat::GetPlayersInChannel(const FString &ChannelName) const
{
    return this->PrimaryUser->GetPlayersInChannel(ChannelName);
}

EVoiceChatChannelType FRedpointEOSVoiceChat::GetChannelType(const FString &ChannelName) const
{
    return this->PrimaryUser->GetChannelType(ChannelName);
}

bool FRedpointEOSVoiceChat::IsPlayerTalking(const FString &PlayerName) const
{
    return this->PrimaryUser->IsPlayerTalking(PlayerName);
}

void FRedpointEOSVoiceChat::SetPlayerMuted(const FString &PlayerName, bool bMuted)
{
    this->PrimaryUser->SetPlayerMuted(PlayerName, bMuted);
}

bool FRedpointEOSVoiceChat::IsPlayerMuted(const FString &PlayerName) const
{
    return this->PrimaryUser->IsPlayerMuted(PlayerName);
}

#if defined(UE_5_0_OR_LATER)

void FRedpointEOSVoiceChat::SetChannelPlayerMuted(
    const FString &ChannelName,
    const FString &PlayerName,
    bool bAudioMuted)
{
    this->PrimaryUser->SetChannelPlayerMuted(ChannelName, PlayerName, bAudioMuted);
}

bool FRedpointEOSVoiceChat::IsChannelPlayerMuted(const FString &ChannelName, const FString &PlayerName) const
{
    return this->PrimaryUser->IsChannelPlayerMuted(ChannelName, PlayerName);
}

#endif // #if defined(UE_5_0_OR_LATER)

void FRedpointEOSVoiceChat::SetPlayerVolume(const FString &PlayerName, float Volume)
{
    this->PrimaryUser->SetPlayerVolume(PlayerName, Volume);
}

float FRedpointEOSVoiceChat::GetPlayerVolume(const FString &PlayerName) const
{
    return this->PrimaryUser->GetPlayerVolume(PlayerName);
}

void FRedpointEOSVoiceChat::TransmitToAllChannels()
{
    this->PrimaryUser->TransmitToAllChannels();
}

void FRedpointEOSVoiceChat::TransmitToNoChannels()
{
    this->PrimaryUser->TransmitToNoChannels();
}

void FRedpointEOSVoiceChat::TransmitToSpecificChannel(const FString &ChannelName)
{
    this->PrimaryUser->TransmitToSpecificChannel(ChannelName);
}

EVoiceChatTransmitMode FRedpointEOSVoiceChat::GetTransmitMode() const
{
    return this->PrimaryUser->GetTransmitMode();
}

FString FRedpointEOSVoiceChat::GetTransmitChannel() const
{
    return this->PrimaryUser->GetTransmitChannel();
}

FDelegateHandle FRedpointEOSVoiceChat::StartRecording(
    const FOnVoiceChatRecordSamplesAvailableDelegate::FDelegate &Delegate)
{
    return this->PrimaryUser->StartRecording(Delegate);
}

void FRedpointEOSVoiceChat::StopRecording(FDelegateHandle Handle)
{
    this->PrimaryUser->StopRecording(Handle);
}

FString FRedpointEOSVoiceChat::InsecureGetLoginToken(const FString &PlayerName)
{
    return this->PrimaryUser->InsecureGetLoginToken(PlayerName);
}

FString FRedpointEOSVoiceChat::InsecureGetJoinToken(
    const FString &ChannelName,
    EVoiceChatChannelType ChannelType,
    TOptional<FVoiceChatChannel3dProperties> Channel3dProperties)
{
    return this->PrimaryUser->InsecureGetJoinToken(ChannelName, ChannelType, Channel3dProperties);
}

// IVoiceChatUser Events

FOnVoiceChatAvailableAudioDevicesChangedDelegate &FRedpointEOSVoiceChat::OnVoiceChatAvailableAudioDevicesChanged()
{
    return this->PrimaryUser->OnVoiceChatAvailableAudioDevicesChanged();
}

FOnVoiceChatLoggedInDelegate &FRedpointEOSVoiceChat::OnVoiceChatLoggedIn()
{
    return this->PrimaryUser->OnVoiceChatLoggedIn();
}

FOnVoiceChatLoggedOutDelegate &FRedpointEOSVoiceChat::OnVoiceChatLoggedOut()
{
    return this->PrimaryUser->OnVoiceChatLoggedOut();
}

FOnVoiceChatChannelJoinedDelegate &FRedpointEOSVoiceChat::OnVoiceChatChannelJoined()
{
    return this->PrimaryUser->OnVoiceChatChannelJoined();
}

FOnVoiceChatChannelExitedDelegate &FRedpointEOSVoiceChat::OnVoiceChatChannelExited()
{
    return this->PrimaryUser->OnVoiceChatChannelExited();
}

FOnVoiceChatCallStatsUpdatedDelegate &FRedpointEOSVoiceChat::OnVoiceChatCallStatsUpdated()
{
    return this->PrimaryUser->OnVoiceChatCallStatsUpdated();
}

FOnVoiceChatPlayerAddedDelegate &FRedpointEOSVoiceChat::OnVoiceChatPlayerAdded()
{
    return this->PrimaryUser->OnVoiceChatPlayerAdded();
}

FOnVoiceChatPlayerRemovedDelegate &FRedpointEOSVoiceChat::OnVoiceChatPlayerRemoved()
{
    return this->PrimaryUser->OnVoiceChatPlayerRemoved();
}

FOnVoiceChatPlayerTalkingUpdatedDelegate &FRedpointEOSVoiceChat::OnVoiceChatPlayerTalkingUpdated()
{
    return this->PrimaryUser->OnVoiceChatPlayerTalkingUpdated();
}

FOnVoiceChatPlayerMuteUpdatedDelegate &FRedpointEOSVoiceChat::OnVoiceChatPlayerMuteUpdated()
{
    return this->PrimaryUser->OnVoiceChatPlayerMuteUpdated();
}

FOnVoiceChatPlayerVolumeUpdatedDelegate &FRedpointEOSVoiceChat::OnVoiceChatPlayerVolumeUpdated()
{
    return this->PrimaryUser->OnVoiceChatPlayerVolumeUpdated();
}

FDelegateHandle FRedpointEOSVoiceChat::RegisterOnVoiceChatAfterCaptureAudioReadDelegate(
    const FOnVoiceChatAfterCaptureAudioReadDelegate::FDelegate &Delegate)
{
    return this->PrimaryUser->RegisterOnVoiceChatAfterCaptureAudioReadDelegate(Delegate);
}

void FRedpointEOSVoiceChat::UnregisterOnVoiceChatAfterCaptureAudioReadDelegate(FDelegateHandle Handle)
{
    this->PrimaryUser->UnregisterOnVoiceChatAfterCaptureAudioReadDelegate(Handle);
}

FDelegateHandle FRedpointEOSVoiceChat::RegisterOnVoiceChatBeforeCaptureAudioSentDelegate(
    const FOnVoiceChatBeforeCaptureAudioSentDelegate::FDelegate &Delegate)
{
    return this->PrimaryUser->RegisterOnVoiceChatBeforeCaptureAudioSentDelegate(Delegate);
}

void FRedpointEOSVoiceChat::UnregisterOnVoiceChatBeforeCaptureAudioSentDelegate(FDelegateHandle Handle)
{
    this->PrimaryUser->UnregisterOnVoiceChatBeforeCaptureAudioSentDelegate(Handle);
}

FDelegateHandle FRedpointEOSVoiceChat::RegisterOnVoiceChatBeforeRecvAudioRenderedDelegate(
    const FOnVoiceChatBeforeRecvAudioRenderedDelegate::FDelegate &Delegate)
{
    return this->PrimaryUser->RegisterOnVoiceChatBeforeRecvAudioRenderedDelegate(Delegate);
}

void FRedpointEOSVoiceChat::UnregisterOnVoiceChatBeforeRecvAudioRenderedDelegate(FDelegateHandle Handle)
{
    this->PrimaryUser->UnregisterOnVoiceChatBeforeRecvAudioRenderedDelegate(Handle);
}

#endif // #if defined(EOS_VOICE_CHAT_SUPPORTED)