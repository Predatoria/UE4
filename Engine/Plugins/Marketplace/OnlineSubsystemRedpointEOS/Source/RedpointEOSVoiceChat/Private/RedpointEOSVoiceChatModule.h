// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Logging/LogMacros.h"
#include "Modules/ModuleManager.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

DECLARE_LOG_CATEGORY_EXTERN(LogEOSVoiceChat, All, All);

class FRedpointEOSVoiceChatModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

#if defined(EOS_VOICE_CHAT_SUPPORTED)
private:
    TUniquePtr<class FRedpointEOSVoiceChat> VoiceChat;
#endif // #if defined(EOS_VOICE_CHAT_SUPPORTED)
};