// Copyright June Rhodes. All Rights Reserved.

#include "NetworkingEOSConfigurer.h"

#include "../PlatformHelpers.h"
#include "Misc/Base64.h"
#include "RedpointLibHydrogen.h"

void FNetworkingEOSConfigurer::InitDefaults(FEOSConfigurerContext &Context, UOnlineSubsystemEOSEditorConfig *Instance)
{
    Instance->EnableAntiCheat = false;
    Instance->TrustedPlatforms.Empty();
    Instance->TrustedClientPrivateKey.Empty();
    Instance->TrustedClientPublicKey.Empty();
    Instance->NetworkAuthenticationMode = ENetworkAuthenticationMode::Default;
    Instance->EnableSanctionChecks = true;
    Instance->EnableIdentityChecksOnListenServers = true;
    Instance->DedicatedServerDistributionMode = EDedicatedServersDistributionMode::DevelopersAndPlayers;
    Instance->EnableTrustedDedicatedServers = true;
    Instance->EnableAutomaticEncryptionOnTrustedDedicatedServers = true;
}

void FNetworkingEOSConfigurer::Load(
    FEOSConfigurerContext &Context,
    FEOSConfigurationReader &Reader,
    UOnlineSubsystemEOSEditorConfig *Instance)
{
    Reader.GetBool(TEXT("EnableAntiCheat"), Instance->EnableAntiCheat);
    Reader.GetString(TEXT("TrustedClientPublicKey"), Instance->TrustedClientPublicKey);
    Reader.GetString(
        TEXT("TrustedClientPrivateKey"),
        Instance->TrustedClientPrivateKey,
        DefaultEOSSection,
        EEOSConfigurationFileType::TrustedClient);

    // Determine trusted clients based on which INI files have the private key.
    {
        TSet<FName> TrustedPlatformsSet;
        FString PlatformClientPrivateKey;
        for (const auto &PlatformName : GetConfidentialPlatformNames())
        {
            if (Reader.GetString(
                    TEXT("TrustedClientPrivateKey"),
                    PlatformClientPrivateKey,
                    DefaultEOSSection,
                    EEOSConfigurationFileType::Engine,
                    PlatformName))
            {
                if (!PlatformClientPrivateKey.IsEmpty() &&
                    PlatformClientPrivateKey == Instance->TrustedClientPrivateKey)
                {
                    TrustedPlatformsSet.Add(PlatformName);
                }
            }
        }
        Instance->TrustedPlatforms.Empty();
        for (const auto &TrustedPlatform : TrustedPlatformsSet)
        {
            Instance->TrustedPlatforms.Add(TrustedPlatform);
        }
    }

    Reader.GetEnum<ENetworkAuthenticationMode>(
        TEXT("NetworkAuthenticationMode"),
        TEXT("ENetworkAuthenticationMode"),
        Instance->NetworkAuthenticationMode,
        DefaultEOSSection);

    Reader.GetBool(TEXT("EnableSanctionChecks"), Instance->EnableSanctionChecks);
    Reader.GetBool(TEXT("EnableIdentityChecksOnListenServers"), Instance->EnableIdentityChecksOnListenServers);

    Reader.GetEnum<EDedicatedServersDistributionMode>(
        TEXT("DedicatedServerDistributionMode"),
        TEXT("EDedicatedServersDistributionMode"),
        Instance->DedicatedServerDistributionMode,
        DefaultEOSSection,
        EEOSConfigurationFileType::Editor);

    Reader.GetString(TEXT("DedicatedServerPublicKey"), Instance->DedicatedServerPublicKey);
    Reader.GetBool(TEXT("EnableTrustedDedicatedServers"), Instance->EnableTrustedDedicatedServers);
    Reader.GetBool(
        TEXT("EnableAutomaticEncryptionOnTrustedDedicatedServers"),
        Instance->EnableAutomaticEncryptionOnTrustedDedicatedServers);
    Reader.GetString(
        TEXT("DedicatedServerClientId"),
        Instance->DedicatedServerClientId,
        DefaultEOSSection,
        EEOSConfigurationFileType::Editor);
    Reader.GetString(
        TEXT("DedicatedServerClientSecret"),
        Instance->DedicatedServerClientSecret,
        DefaultEOSSection,
        EEOSConfigurationFileType::Editor);
    Reader.GetString(
        TEXT("DedicatedServerPrivateKey"),
        Instance->DedicatedServerPrivateKey,
        DefaultEOSSection,
        EEOSConfigurationFileType::Editor);
}

bool FNetworkingEOSConfigurer::Validate(FEOSConfigurerContext &Context, UOnlineSubsystemEOSEditorConfig *Instance)
{
    bool bDidModify = false;

    if (Instance->DedicatedServerDistributionMode == EDedicatedServersDistributionMode::PlayersOnly)
    {
        Instance->EnableTrustedDedicatedServers = false;
        bDidModify = true;
    }

    if (Instance->DedicatedServerPublicKey.IsEmpty() && Instance->DedicatedServerPrivateKey.IsEmpty() &&
        Instance->DedicatedServerDistributionMode != EDedicatedServersDistributionMode::PlayersOnly)
    {
        // Generate a dedicated server public/private key pair.
        hydro_sign_keypair server_key_pair;
        hydro_sign_keygen(&server_key_pair);

        TArray<uint8> PrivateKey(server_key_pair.sk, sizeof(server_key_pair.sk));
        TArray<uint8> PublicKey(server_key_pair.pk, sizeof(server_key_pair.pk));

        Instance->DedicatedServerPrivateKey = FBase64::Encode(PrivateKey);
        Instance->DedicatedServerPublicKey = FBase64::Encode(PublicKey);

        // Mark as modified.
        bDidModify = true;
    }

    if (Instance->EnableAntiCheat &&
        (Instance->TrustedClientPrivateKey.IsEmpty() || Instance->TrustedClientPublicKey.IsEmpty()))
    {
        // Generate a trusted client public/private key pair.
        hydro_sign_keypair client_key_pair;
        hydro_sign_keygen(&client_key_pair);

        TArray<uint8> PrivateKey(client_key_pair.sk, sizeof(client_key_pair.sk));
        TArray<uint8> PublicKey(client_key_pair.pk, sizeof(client_key_pair.pk));

        Instance->TrustedClientPrivateKey = FBase64::Encode(PrivateKey);
        Instance->TrustedClientPublicKey = FBase64::Encode(PublicKey);

        // Mark as modified.
        bDidModify = true;
    }

    return bDidModify;
}

void FNetworkingEOSConfigurer::Save(
    FEOSConfigurerContext &Context,
    FEOSConfigurationWriter &Writer,
    UOnlineSubsystemEOSEditorConfig *Instance)
{
    Writer.SetBool(TEXT("EnableAntiCheat"), Instance->EnableAntiCheat);
    Writer.SetString(TEXT("TrustedClientPublicKey"), Instance->TrustedClientPublicKey);
    Writer.SetString(
        TEXT("TrustedClientPrivateKey"),
        Instance->TrustedClientPrivateKey,
        DefaultEOSSection,
        EEOSConfigurationFileType::TrustedClient);

    {
        for (const auto &PlatformName : GetConfidentialPlatformNames())
        {
            if (Instance->TrustedPlatforms.Contains(PlatformName))
            {
                Writer.SetString(
                    TEXT("TrustedClientPrivateKey"),
                    Instance->TrustedClientPrivateKey,
                    DefaultEOSSection,
                    EEOSConfigurationFileType::Engine,
                    PlatformName);
            }
            else
            {
                Writer.Remove(
                    TEXT("TrustedClientPrivateKey"),
                    DefaultEOSSection,
                    EEOSConfigurationFileType::Engine,
                    PlatformName);
            }
        }
    }

    Writer.SetEnum<ENetworkAuthenticationMode>(
        TEXT("NetworkAuthenticationMode"),
        TEXT("ENetworkAuthenticationMode"),
        Instance->NetworkAuthenticationMode,
        DefaultEOSSection);

    Writer.SetBool(TEXT("EnableSanctionChecks"), Instance->EnableSanctionChecks);
    Writer.SetBool(TEXT("EnableIdentityChecksOnListenServers"), Instance->EnableIdentityChecksOnListenServers);

    Writer.SetEnum<EDedicatedServersDistributionMode>(
        TEXT("DedicatedServerDistributionMode"),
        TEXT("EDedicatedServersDistributionMode"),
        Instance->DedicatedServerDistributionMode,
        DefaultEOSSection,
        EEOSConfigurationFileType::Editor);

    Writer.SetString(TEXT("DedicatedServerPublicKey"), Instance->DedicatedServerPublicKey);
    Writer.SetBool(TEXT("EnableTrustedDedicatedServers"), Instance->EnableTrustedDedicatedServers);
    Writer.SetBool(
        TEXT("EnableAutomaticEncryptionOnTrustedDedicatedServers"),
        Instance->EnableAutomaticEncryptionOnTrustedDedicatedServers);

    bool bTrustedServersEnabled = Instance->NetworkAuthenticationMode == ENetworkAuthenticationMode::IDToken ||
                                  (Instance->NetworkAuthenticationMode == ENetworkAuthenticationMode::UserCredentials &&
                                   Instance->EnableTrustedDedicatedServers) ||
                                  (Instance->NetworkAuthenticationMode == ENetworkAuthenticationMode::Default &&
                                   EOS_ApiVersionIsAtLeast(Instance->ApiVersion, EEOSApiVersion::v2022_05_20));

    if (Context.bAutomaticallyConfigureEngineLevelSettings)
    {
        if (Instance->EnableAutomaticEncryptionOnTrustedDedicatedServers && bTrustedServersEnabled)
        {
            Writer.SetString(
                TEXT("EncryptionComponent"),
                TEXT("AESGCMHandlerComponent"),
                TEXT("PacketHandlerComponents"),
                EEOSConfigurationFileType::Engine);
        }
        else if (!Instance->EnableAutomaticEncryptionOnTrustedDedicatedServers && bTrustedServersEnabled)
        {
            // Leave user override.
        }
        else
        {
            Writer.Remove(
                TEXT("EncryptionComponent"),
                TEXT("PacketHandlerComponents"),
                EEOSConfigurationFileType::Engine);
        }
    }

    Writer.SetString(
        TEXT("DedicatedServerClientId"),
        Instance->DedicatedServerClientId,
        DefaultEOSSection,
        EEOSConfigurationFileType::Editor);
    Writer.SetString(
        TEXT("DedicatedServerClientSecret"),
        Instance->DedicatedServerClientSecret,
        DefaultEOSSection,
        EEOSConfigurationFileType::Editor);
    Writer.SetString(
        TEXT("DedicatedServerPrivateKey"),
        Instance->DedicatedServerPrivateKey,
        DefaultEOSSection,
        EEOSConfigurationFileType::Editor);

    if (Instance->DedicatedServerDistributionMode == EDedicatedServersDistributionMode::DevelopersOnly)
    {
        Writer.SetString(
            TEXT("DedicatedServerClientId"),
            Instance->DedicatedServerClientId,
            DefaultEOSSection,
            EEOSConfigurationFileType::DedicatedServer);
        Writer.SetString(
            TEXT("DedicatedServerClientSecret"),
            Instance->DedicatedServerClientSecret,
            DefaultEOSSection,
            EEOSConfigurationFileType::DedicatedServer);
        Writer.SetString(
            TEXT("DedicatedServerPrivateKey"),
            Instance->DedicatedServerPrivateKey,
            DefaultEOSSection,
            EEOSConfigurationFileType::DedicatedServer);
    }
    else
    {
        Writer.Remove(TEXT("DedicatedServerClientId"), DefaultEOSSection, EEOSConfigurationFileType::DedicatedServer);
        Writer.Remove(
            TEXT("DedicatedServerClientSecret"),
            DefaultEOSSection,
            EEOSConfigurationFileType::DedicatedServer);
        Writer.Remove(TEXT("DedicatedServerPrivateKey"), DefaultEOSSection, EEOSConfigurationFileType::DedicatedServer);
    }

    // Remove legacy values for dedicated servers.
    Writer.Remove(TEXT("ClientId"), DefaultEOSSection, EEOSConfigurationFileType::DedicatedServer);
    Writer.Remove(TEXT("ClientSecret"), DefaultEOSSection, EEOSConfigurationFileType::DedicatedServer);
    Writer.Remove(TEXT("DedicatedServerActAsClientId"), DefaultEOSSection, EEOSConfigurationFileType::DedicatedServer);
    Writer.Remove(
        TEXT("DedicatedServerActAsClientSecret"),
        DefaultEOSSection,
        EEOSConfigurationFileType::DedicatedServer);
}