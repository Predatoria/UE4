// Copyright June Rhodes. All Rights Reserved.

#include "VoiceChatEOSConfigurer.h"

void FVoiceChatEOSConfigurer::InitDefaults(FEOSConfigurerContext &Context, UOnlineSubsystemEOSEditorConfig *Instance)
{
    Instance->EnableVoiceChatEchoInParties = false;
    Instance->EnableVoiceChatPlatformAECByDefault = false;
}

void FVoiceChatEOSConfigurer::Load(
    FEOSConfigurerContext &Context,
    FEOSConfigurationReader &Reader,
    UOnlineSubsystemEOSEditorConfig *Instance)
{
    Reader.GetBool(TEXT("EnableVoiceChatEchoInParties"), Instance->EnableVoiceChatEchoInParties);
    Reader.GetBool(TEXT("EnableVoiceChatPlatformAECByDefault"), Instance->EnableVoiceChatPlatformAECByDefault);
}

void FVoiceChatEOSConfigurer::Save(
    FEOSConfigurerContext &Context,
    FEOSConfigurationWriter &Writer,
    UOnlineSubsystemEOSEditorConfig *Instance)
{
    Writer.SetBool(TEXT("EnableVoiceChatEchoInParties"), Instance->EnableVoiceChatEchoInParties);
    Writer.SetBool(TEXT("EnableVoiceChatPlatformAECByDefault"), Instance->EnableVoiceChatPlatformAECByDefault);
}