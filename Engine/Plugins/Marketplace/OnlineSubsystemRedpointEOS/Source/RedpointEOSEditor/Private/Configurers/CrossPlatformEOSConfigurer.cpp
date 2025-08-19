// Copyright June Rhodes. All Rights Reserved.

#include "CrossPlatformEOSConfigurer.h"

void FCrossPlatformEOSConfigurer::InitDefaults(
    FEOSConfigurerContext &Context,
    UOnlineSubsystemEOSEditorConfig *Instance)
{
    Instance->PresenceAdvertises = EPresenceAdvertisementType::Party;
    Instance->PartyJoinabilityConstraint = EPartyJoinabilityConstraint::AllowPlayersInMultipleParties;
}

void FCrossPlatformEOSConfigurer::Load(
    FEOSConfigurerContext &Context,
    FEOSConfigurationReader &Reader,
    UOnlineSubsystemEOSEditorConfig *Instance)
{
    FString DelegatedSubsystems;
    if (Reader.GetString(TEXT("DelegatedSubsystems"), DelegatedSubsystems))
    {
        DelegatedSubsystems.ParseIntoArray(Instance->DelegatedSubsystems, TEXT(","), true);
    }

    Reader.GetEnum<EPartyJoinabilityConstraint>(
        TEXT("PartyJoinabilityConstraint"),
        TEXT("EPartyJoinabilityConstraint"),
        Instance->PartyJoinabilityConstraint);
}

void FCrossPlatformEOSConfigurer::Save(
    FEOSConfigurerContext &Context,
    FEOSConfigurationWriter &Writer,
    UOnlineSubsystemEOSEditorConfig *Instance)
{
    Writer.SetString(TEXT("DelegatedSubsystems"), FString::Join(Instance->DelegatedSubsystems, TEXT(",")));
    Writer.SetEnum<EPartyJoinabilityConstraint>(
        TEXT("PartyJoinabilityConstraint"),
        TEXT("EPartyJoinabilityConstraint"),
        Instance->PartyJoinabilityConstraint);
}