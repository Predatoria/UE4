// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/OnlineVoiceInterfaceEOS.h"

EOS_ENABLE_STRICT_WARNINGS

#if defined(EOS_VOICE_CHAT_SUPPORTED)

IVoiceEnginePtr FOnlineVoiceInterfaceEOS::CreateVoiceEngine()
{
    // Unused (this is only used by FVoiceInterfaceImpl, which we don't subclass).
    return nullptr;
}

void FOnlineVoiceInterfaceEOS::ProcessMuteChangeNotification()
{
}

void FOnlineVoiceInterfaceEOS::StartNetworkedVoice(uint8 LocalUserNum)
{
    // EOS does not support push to talk at the moment.
}

void FOnlineVoiceInterfaceEOS::StopNetworkedVoice(uint8 LocalUserNum)
{
    // EOS does not support push to talk at the moment.
}

bool FOnlineVoiceInterfaceEOS::RegisterLocalTalker(uint32 LocalUserNum)
{
    // Not used.
    return true;
}

void FOnlineVoiceInterfaceEOS::RegisterLocalTalkers()
{
}

bool FOnlineVoiceInterfaceEOS::UnregisterLocalTalker(uint32 LocalUserNum)
{
    // Not used.
    return true;
}

void FOnlineVoiceInterfaceEOS::UnregisterLocalTalkers()
{
}

bool FOnlineVoiceInterfaceEOS::RegisterRemoteTalker(const FUniqueNetId &UniqueId)
{
    // Not used.
    return true;
}

bool FOnlineVoiceInterfaceEOS::UnregisterRemoteTalker(const FUniqueNetId &UniqueId)
{
    // Not used.
    return true;
}

void FOnlineVoiceInterfaceEOS::RemoveAllRemoteTalkers()
{
}

bool FOnlineVoiceInterfaceEOS::IsHeadsetPresent(uint32 LocalUserNum)
{
    if (this->IdentityEOS->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
    {
        return false;
    }

    TSharedPtr<const FUniqueNetIdEOS> UserIdEOS =
        StaticCastSharedPtr<const FUniqueNetIdEOS>(this->IdentityEOS->GetUniquePlayerId(0));
    TSharedPtr<FEOSVoiceManagerLocalUser> VoiceUser = this->VoiceManager->GetLocalUser(*UserIdEOS);
    return VoiceUser->GetAvailableOutputDeviceInfos().Num() > 0;
}

bool FOnlineVoiceInterfaceEOS::IsLocalPlayerTalking(uint32 LocalUserNum)
{
    if (this->IdentityEOS->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
    {
        return false;
    }

    TSharedPtr<const FUniqueNetIdEOS> UserIdEOS =
        StaticCastSharedPtr<const FUniqueNetIdEOS>(this->IdentityEOS->GetUniquePlayerId(0));
    TSharedPtr<FEOSVoiceManagerLocalUser> VoiceUser = this->VoiceManager->GetLocalUser(*UserIdEOS);
    return VoiceUser->IsPlayerTalking(UserIdEOS->ToString());
}

bool FOnlineVoiceInterfaceEOS::IsRemotePlayerTalking(const FUniqueNetId &UniqueId)
{
    for (const auto &Account : this->IdentityEOS->GetAllUserAccounts())
    {
        TSharedRef<const FUniqueNetIdEOS> UserIdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(Account->GetUserId());
        TSharedPtr<FEOSVoiceManagerLocalUser> VoiceUser = this->VoiceManager->GetLocalUser(*UserIdEOS);
        if (VoiceUser.IsValid())
        {
            if (VoiceUser->IsPlayerTalking(UniqueId.ToString()))
            {
                return true;
            }
        }
    }

    return false;
}

bool FOnlineVoiceInterfaceEOS::IsMuted(uint32 LocalUserNum, const FUniqueNetId &UniqueId) const
{
    if (this->IdentityEOS->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
    {
        return false;
    }

    TSharedPtr<const FUniqueNetIdEOS> UserIdEOS =
        StaticCastSharedPtr<const FUniqueNetIdEOS>(this->IdentityEOS->GetUniquePlayerId(0));
    TSharedPtr<FEOSVoiceManagerLocalUser> VoiceUser = this->VoiceManager->GetLocalUser(*UserIdEOS);
    return VoiceUser->IsPlayerMuted(UniqueId.ToString());
}

bool FOnlineVoiceInterfaceEOS::MuteRemoteTalker(uint8 LocalUserNum, const FUniqueNetId &PlayerId, bool bIsSystemWide)
{
    if (this->IdentityEOS->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
    {
        return false;
    }

    // We don't support using bIsSystemWide, because for system-wide voice mute you'd have to be a dedicated server
    // (which LocalUserNum doesn't make sense for).

    TSharedPtr<const FUniqueNetIdEOS> UserIdEOS =
        StaticCastSharedPtr<const FUniqueNetIdEOS>(this->IdentityEOS->GetUniquePlayerId(0));
    TSharedPtr<FEOSVoiceManagerLocalUser> VoiceUser = this->VoiceManager->GetLocalUser(*UserIdEOS);
    VoiceUser->SetPlayerMuted(PlayerId.ToString(), true);
    return true;
}

bool FOnlineVoiceInterfaceEOS::UnmuteRemoteTalker(uint8 LocalUserNum, const FUniqueNetId &PlayerId, bool bIsSystemWide)
{
    if (this->IdentityEOS->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
    {
        return false;
    }

    // We don't support using bIsSystemWide, because for system-wide voice mute you'd have to be a dedicated server
    // (which LocalUserNum doesn't make sense for).

    TSharedPtr<const FUniqueNetIdEOS> UserIdEOS =
        StaticCastSharedPtr<const FUniqueNetIdEOS>(this->IdentityEOS->GetUniquePlayerId(0));
    TSharedPtr<FEOSVoiceManagerLocalUser> VoiceUser = this->VoiceManager->GetLocalUser(*UserIdEOS);
    VoiceUser->SetPlayerMuted(PlayerId.ToString(), false);
    return true;
}

TSharedPtr<class FVoicePacket> FOnlineVoiceInterfaceEOS::SerializeRemotePacket(FArchive &Ar)
{
    return nullptr;
}

TSharedPtr<class FVoicePacket> FOnlineVoiceInterfaceEOS::GetLocalPacket(uint32 LocalUserNum)
{
    return nullptr;
}

int32 FOnlineVoiceInterfaceEOS::GetNumLocalTalkers()
{
    int32 LocalTalkers = 0;

    for (const auto &Account : this->IdentityEOS->GetAllUserAccounts())
    {
        TSharedRef<const FUniqueNetIdEOS> UserIdEOS = StaticCastSharedRef<const FUniqueNetIdEOS>(Account->GetUserId());
        TSharedPtr<FEOSVoiceManagerLocalUser> VoiceUser = this->VoiceManager->GetLocalUser(*UserIdEOS);
        if (VoiceUser.IsValid())
        {
            if (VoiceUser->IsPlayerTalking(UserIdEOS->ToString()))
            {
                LocalTalkers++;
            }
        }
    }

    return LocalTalkers;
}

void FOnlineVoiceInterfaceEOS::ClearVoicePackets()
{
}

void FOnlineVoiceInterfaceEOS::Tick(float DeltaTime)
{
}

FString FOnlineVoiceInterfaceEOS::GetVoiceDebugState() const
{
    return TEXT("Voice debug state not available");
}

#endif

EOS_DISABLE_STRICT_WARNINGS