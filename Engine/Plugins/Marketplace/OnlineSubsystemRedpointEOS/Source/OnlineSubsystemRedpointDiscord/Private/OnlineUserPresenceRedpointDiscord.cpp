// Copyright June Rhodes. All Rights Reserved.

#include "OnlineUserPresenceRedpointDiscord.h"

#if EOS_DISCORD_ENABLED

FOnlineUserPresenceRedpointDiscord::FOnlineUserPresenceRedpointDiscord()
{
    this->bIsOnline = false;
    this->bIsPlaying = false;
    this->bIsPlayingThisGame = false;
    this->bIsJoinable = false;
    this->bHasVoiceSupport = false;
    this->LastOnline = FDateTime::MinValue();
    this->Status.State = EOnlinePresenceState::Offline;
    this->Status.StatusStr = TEXT("");
}

void FOnlineUserPresenceRedpointDiscord::UpdateFromPresence(const discord::Presence &InNewPresence)
{
    this->SessionId = nullptr;
    this->bIsOnline = InNewPresence.GetStatus() != discord::Status::Offline;
    this->bIsPlaying = false;
    this->bIsPlayingThisGame = false;
    this->bIsJoinable = false;
    this->bHasVoiceSupport = false;
    this->LastOnline = FDateTime::MinValue();
    switch (InNewPresence.GetStatus())
    {
    case discord::Status::Online:
        this->Status.State = EOnlinePresenceState::Online;
        break;
    case discord::Status::Idle:
        this->Status.State = EOnlinePresenceState::Away;
        break;
    case discord::Status::DoNotDisturb:
        this->Status.State = EOnlinePresenceState::DoNotDisturb;
        break;
    case discord::Status::Offline:
    default:
        this->Status.State = EOnlinePresenceState::Offline;
        break;
    }
    this->Status.StatusStr = UTF8_TO_TCHAR(InNewPresence.GetActivity().GetDetails());
}

#endif