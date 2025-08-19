// Copyright June Rhodes. All Rights Reserved.

#include "RedpointEOSVoiceChatModule.h"

#include "RedpointEOSVoiceChat.h"

DEFINE_LOG_CATEGORY(LogEOSVoiceChat);

void FRedpointEOSVoiceChatModule::StartupModule()
{
#if defined(EOS_VOICE_CHAT_SUPPORTED)
    this->VoiceChat = MakeUnique<FRedpointEOSVoiceChat>();

    if (this->VoiceChat.IsValid())
    {
        IModularFeatures::Get().RegisterModularFeature(TEXT("VoiceChat"), this->VoiceChat.Get());
    }
#endif // #if defined(EOS_VOICE_CHAT_SUPPORTED)
}

void FRedpointEOSVoiceChatModule::ShutdownModule()
{
#if defined(EOS_VOICE_CHAT_SUPPORTED)
    if (this->VoiceChat.IsValid())
    {
        IModularFeatures::Get().UnregisterModularFeature(TEXT("VoiceChat"), this->VoiceChat.Get());
        this->VoiceChat->LogoutAndReleaseAllUsers();
        this->VoiceChat.Reset();
    }
#endif // #if defined(EOS_VOICE_CHAT_SUPPORTED)
}

IMPLEMENT_MODULE(FRedpointEOSVoiceChatModule, RedpointEOSVoiceChat)