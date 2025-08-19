// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

#if defined(EOS_VOICE_CHAT_SUPPORTED)

#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/VoiceChat/VoiceManagerLocalUser.h"

class ONLINESUBSYSTEMREDPOINTEOS_API FEOSVoiceManager : public TSharedFromThis<FEOSVoiceManager>
{
    friend class FEOSVoiceManagerLocalUser;

private:
    EOS_HPlatform EOSPlatform;
    TSharedRef<FEOSConfig> Config;
    TSharedRef<IOnlineIdentity, ESPMode::ThreadSafe> Identity;
    TUserIdMap<TSharedPtr<FEOSVoiceManagerLocalUser>> LocalUsers;

public:
    FEOSVoiceManager(
        EOS_HPlatform InPlatform,
        TSharedRef<FEOSConfig> InConfig,
        TSharedRef<IOnlineIdentity, ESPMode::ThreadSafe> InIdentity);
    UE_NONCOPYABLE(FEOSVoiceManager);
    void AddLocalUser(const FUniqueNetIdEOS &UserId);
    void RemoveLocalUser(const FUniqueNetIdEOS &UserId);
    void RemoveAllLocalUsers();
    TSharedPtr<FEOSVoiceManagerLocalUser> GetLocalUser(const FUniqueNetIdEOS &UserId);
};

#endif