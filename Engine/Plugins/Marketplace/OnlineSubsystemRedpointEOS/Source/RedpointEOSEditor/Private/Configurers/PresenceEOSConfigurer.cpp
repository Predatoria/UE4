// Copyright June Rhodes. All Rights Reserved.

#include "PresenceEOSConfigurer.h"

void FPresenceEOSConfigurer::InitDefaults(FEOSConfigurerContext &Context, UOnlineSubsystemEOSEditorConfig *Instance)
{
}

void FPresenceEOSConfigurer::Load(
    FEOSConfigurerContext &Context,
    FEOSConfigurationReader &Reader,
    UOnlineSubsystemEOSEditorConfig *Instance)
{
    Reader.GetEnum<EPresenceAdvertisementType>(
        TEXT("PresenceAdvertises"),
        TEXT("EPresenceAdvertisementType"),
        Instance->PresenceAdvertises);
}

void FPresenceEOSConfigurer::Save(
    FEOSConfigurerContext &Context,
    FEOSConfigurationWriter &Writer,
    UOnlineSubsystemEOSEditorConfig *Instance)
{
    Writer.SetEnum<EPresenceAdvertisementType>(
        TEXT("PresenceAdvertises"),
        TEXT("EPresenceAdvertisementType"),
        Instance->PresenceAdvertises);
}