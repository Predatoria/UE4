// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/VoiceChat/VoiceManager.h"

#if defined(EOS_VOICE_CHAT_SUPPORTED)

FEOSVoiceManager::FEOSVoiceManager(
    EOS_HPlatform InPlatform,
    TSharedRef<FEOSConfig> InConfig,
    TSharedRef<IOnlineIdentity, ESPMode::ThreadSafe> InIdentity)
    : EOSPlatform(InPlatform), Config(MoveTemp(InConfig)), Identity(MoveTemp(InIdentity)), LocalUsers()
{
}

void FEOSVoiceManager::AddLocalUser(const FUniqueNetIdEOS &UserId)
{
    this->LocalUsers.Add(
        UserId,
        MakeShared<FEOSVoiceManagerLocalUser>(
            this->EOSPlatform,
            this->AsShared(),
            StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared())));
}

void FEOSVoiceManager::RemoveLocalUser(const FUniqueNetIdEOS &UserId)
{
    if (this->LocalUsers.Contains(UserId))
    {
        // Channels have references to their owning users, so we have to clear out the joined channels so the users
        // don't stay alive due to circular references.
        this->LocalUsers[UserId]->JoinedChannels.Empty();
    }
    this->LocalUsers.Remove(UserId);
}

void FEOSVoiceManager::RemoveAllLocalUsers()
{
    for (const auto &KV : this->LocalUsers)
    {
        // Channels have references to their owning users, so we have to clear out the joined channels so the users
        // don't stay alive due to circular references.
        KV.Value->JoinedChannels.Empty();
    }
    this->LocalUsers.Empty();
}

TSharedPtr<FEOSVoiceManagerLocalUser> FEOSVoiceManager::GetLocalUser(const FUniqueNetIdEOS &UserId)
{
    if (this->LocalUsers.Contains(UserId))
    {
        return this->LocalUsers[UserId];
    }
    return nullptr;
}

#endif