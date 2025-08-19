// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineUserPresenceEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"

EOS_ENABLE_STRICT_WARNINGS

FOnlineUserPresenceEAS::FOnlineUserPresenceEAS(EOS_Presence_Info *InInitialPresence, const FString &InOurProductId)
{
    this->OurProductId = InOurProductId;

    if (InInitialPresence != nullptr)
    {
        this->UpdateFromPresence(InInitialPresence);
    }
    else
    {
        // Otherwise initialize to offline status. This is used when the friends list is being read for the first time,
        // and the friends interface needs to construct friend objects. When constructing a friend object, we must have
        // a shared FOnlineUserPresenceEAS for it to use, which the presence interface will later update once presence
        // gets the initial presence information about each of the friends.
        this->bIsOnline = false;
        this->bIsPlaying = false;
        this->bIsPlayingThisGame = false;
        this->bIsJoinable = false;
        this->bHasVoiceSupport = false;
        this->LastOnline = FDateTime::MinValue();
        this->Status.State = EOnlinePresenceState::Offline;
        this->Status.StatusStr = TEXT("");
    }
}

void FOnlineUserPresenceEAS::UpdateFromPresence(EOS_Presence_Info *InNewPresence)
{
    this->SessionId = nullptr;
    this->bIsOnline = InNewPresence->Status != EOS_Presence_EStatus::EOS_PS_Offline;
    this->bIsPlaying =
        InNewPresence->ProductId != nullptr && !FString(ANSI_TO_TCHAR(InNewPresence->ProductId)).IsEmpty();
    this->bIsPlayingThisGame =
        InNewPresence->ProductId != nullptr && FString(ANSI_TO_TCHAR(InNewPresence->ProductId)) == this->OurProductId;
    this->bIsJoinable = false;
    this->bHasVoiceSupport = false;
    this->LastOnline = FDateTime::MinValue();
    switch (InNewPresence->Status)
    {
    case EOS_Presence_EStatus::EOS_PS_Online:
        this->Status.State = EOnlinePresenceState::Online;
        break;
    case EOS_Presence_EStatus::EOS_PS_Away:
        this->Status.State = EOnlinePresenceState::Away;
        break;
    case EOS_Presence_EStatus::EOS_PS_ExtendedAway:
        this->Status.State = EOnlinePresenceState::ExtendedAway;
        break;
    case EOS_Presence_EStatus::EOS_PS_DoNotDisturb:
        this->Status.State = EOnlinePresenceState::DoNotDisturb;
        break;
    case EOS_Presence_EStatus::EOS_PS_Offline:
    default:
        this->Status.State = EOnlinePresenceState::Offline;
        break;
    }
    this->Status.StatusStr = EOSString_Presence_RichTextValue::FromUtf8String(InNewPresence->RichText);
    for (int32_t i = 0; i < InNewPresence->RecordsCount; i++)
    {
        auto Key = EOSString_Presence_DataRecord_Key::FromAnsiString(InNewPresence->Records[i].Key);
        auto Value = EOSString_Presence_DataRecord_Value::FromAnsiString(InNewPresence->Records[i].Value);
        this->Status.Properties.Add(Key, Value);
    }
    // TODO: Do we need to expose the following fields somehow?
    // - UserId
    // - ProductId
    // - ProductVersion
    // - Platform
    // - ProductName
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION