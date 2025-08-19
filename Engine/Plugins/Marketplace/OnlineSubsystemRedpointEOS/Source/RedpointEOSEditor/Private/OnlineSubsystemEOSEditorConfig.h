// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ISettingsSection.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"

#include "OnlineSubsystemEOSEditorConfig.generated.h"

USTRUCT()
struct FStatTypingRule
{
    GENERATED_BODY()

public:
    FStatTypingRule()
        : StatName()
        , Type(EStatTypingRule::Int32){};

    /**
     * The name of the stat to match. This can contain wildcard (*) characters.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Stats")
    FString StatName;

    /**
     * The typing to apply to the stat, which tells the plugin how to store the stat in the EOS backend.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Stats")
    EStatTypingRule Type = EStatTypingRule::Int32;
};

/**
 * Configuration for Epic Online Services. The developer portal is located at https://dev.epicgames.com/portal/en-US/.
 */
UCLASS(Config = Engine, DefaultConfig)
class UOnlineSubsystemEOSEditorConfig : public UObject
{
    GENERATED_BODY()

public:
    UOnlineSubsystemEOSEditorConfig();

    virtual void LoadEOSConfig();
    virtual void SaveEOSConfig();

    static const FName GetAuthenticationGraphPropertyName();
    static const FName GetEditorAuthenticationGraphPropertyName();
    static const FName GetCrossPlatformAccountProviderPropertyName();

    /**
     * The product name to send with requests to Epic Online Services. This can be any arbitrary string, but should
     * probably be the name of your game.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Core Configuration")
    FString ProductName;

    /**
     * The product version to send with requests to Epic Online Services. If unsure, you can leave this as the default.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Core Configuration")
    FString ProductVersion;

    /**
     * The product ID. You can find this under "Product Settings" in the portal.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Core Configuration")
    FString ProductId;

    /**
     * The sandbox ID. You can find this under "Product Settings" -> "Sandboxes" in the portal. If you are not an Epic
     * Games Store developer, this should be the ID of the Live sandbox.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Core Configuration")
    FString SandboxId;

    /**
     * The deployment ID. You can find this under "Product Settings" -> "Sandboxes" -> (SANDBOX NAME) -> "Deployments"
     * in the portal.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Core Configuration")
    FString DeploymentId;

    /**
     * The client ID. You can find this under "Product Settings" -> "Client Credentials" (bottom of the page)
     * in the portal.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Core Configuration")
    FString ClientId;

    /**
     * The client secret. You can find this under "Product Settings" -> "Client Credentials" (bottom of the page) ->
     * "..." -> "Client Details" -> "Secret key" in the portal.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Core Configuration")
    FString ClientSecret;

    /**
     * Require the Epic Games Launcher to launch the game. This option should only be turned on for games that ship on
     * the Epic Games Store.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Core Configuration")
    bool RequireEpicGamesLauncher;

    /**
     * The authentication graph to use for this application. Refer to
     * https://docs.redpoint.games/eos-online-subsystem/docs/identity_configuration for a list of available options.
     * This defaults to "Default", whose behaviour may change with API versions.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Authentication")
    FName AuthenticationGraph;

    /**
     * The authentication graph to use when running this application in the editor.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Authentication")
    FName EditorAuthenticationGraph;

    /**
     * The cross-platform account provider. This will typically be either "None" or "EpicGames" but you can define your
     * own first-party, cross-platform account systems in C++ and leverage the existing authentication logic across
     * platforms.
     *
     * If this is set to "None", then you will get the same behaviour as the NoEAS option that was previously available.
     * If this is set to "EpicGames", then you'll get either EASOptional or EASRequired depending on the
     * RequireCrossPlatformAccount setting.
     *
     * Refer to https://docs.redpoint.games/eos-online-subsystem/docs/identity_configuration for a list of available
     * options.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Authentication")
    FName CrossPlatformAccountProvider;

    /**
     * Require the user to have linked a cross-platform account in order to play the game.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Authentication")
    bool RequireCrossPlatformAccount;

    /**
     * The user interface to use when the player is being prompted to choose between signing into an existing account or
     * creating a new account.
     */
    UPROPERTY(
        EditAnywhere,
        Config,
        Category = "Authentication|User Interface",
        NoClear,
        meta =
            (DisplayName = "On Sign In or Create Account",
             MetaClass = "UserWidget",
             MustImplement = "EOSUserInterface_SignInOrCreateAccount"))
    FSoftClassPath WidgetClass_SignInOrCreateAccount;

    /**
     * The user interface to use when the player is being prompted to obtain and enter a PIN code (used for
     * authentication on consoles).
     */
    UPROPERTY(
        EditAnywhere,
        Config,
        Category = "Authentication|User Interface",
        NoClear,
        meta =
            (DisplayName = "On Enter Device PIN Code",
             MetaClass = "UserWidget",
             MustImplement = "EOSUserInterface_EnterDevicePinCode"))
    FSoftClassPath WidgetClass_EnterDevicePinCode;

    /**
     * If true, player logins will not be persistently saved when players sign in with an Epic Games account. This
     * setting should only be used for testing purposes. You can find out more information about this option in the
     * documentation: https://docs.redpoint.games/eos-online-subsystem/docs/identity_configuration
     */
    UPROPERTY(EditAnywhere, Config, Category = "Cross Platform Account Provider: Epic Games")
    bool DisablePersistentLogin;

    /**
     * The login URL that you have set up for Simple First Party authentication. Follow the documentation on how to
     * configure this.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Cross Platform Account Provider: Simple First Party")
    FString SimpleFirstPartyLoginUrl;

    /**
     * Specifies the host and port as "host:port" that the plugin will use to connect to the Developer Authentication
     * Tool. Refer to the documentation for more information about this setting:
     * https://docs.redpoint.games/eos-online-subsystem/docs/identity_dev_auth_tool_usage
     */
    UPROPERTY(EditAnywhere, Config, Category = "Authentication Graph: Monolithic")
    FString DevAuthToolAddress;

    /**
     * Specifies the default credential name that the plugin will try to use in the Developer Authentication
     * Tool if the credential for the play-in-editor context didn't work. Refer to the documentation for more
     * information about this setting:
     * https://docs.redpoint.games/eos-online-subsystem/docs/identity_dev_auth_tool_usage
     */
    UPROPERTY(EditAnywhere, Config, Category = "Authentication Graph: Monolithic")
    FString DevAuthToolDefaultCredentialName;

    /**
     * The encryption key used to encrypt and decrypt data from Player Data Storage and Title Storage. This should be
     * a 64-character hexadecimal string; see the EOS documentation for more information:
     * https://dev.epicgames.com/docs/services/en-US/Interfaces/PlayerDataStorage/index.html
     */
    UPROPERTY(EditAnywhere, Config, Category = "Storage")
    FString PlayerDataEncryptionKey;

    /**
     * In EOS, you can only have one object advertised with presence; this can either be a party or a session, but not
     * both. This configuration option determines what gets advertised in the presence system.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Presence")
    EPresenceAdvertisementType PresenceAdvertises;

    /**
     * A comma-separated list of subsystems that friends will be included from (in addition to Epic Account Services).
     * Refer to the documentation for more information on this setting:
     * https://docs.redpoint.games/eos-online-subsystem/docs/friends_delegated_subsystems
     */
    UPROPERTY(EditAnywhere, Config, Category = "Cross-Platform")
    TArray<FString> DelegatedSubsystems;

    /**
     * Constrains the effect of accepting cross-platform party invites by a player. This setting has no effect on your
     * game code calling JoinParty; it only affects synthetic party invites that were sent and accepted over the native
     * platform.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Cross-Platform")
    EPartyJoinabilityConstraint PartyJoinabilityConstraint;

    /**
     * When new versions of the plugin are released, sometimes the default behaviour changes. When you first use the
     * plugin, it sets the current latest API version in the configuration so that when you upgrade, the behaviour does
     * not change for you until you decide to upgrade.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Compatibility")
    EEOSApiVersion ApiVersion;

    /**
     * Sets the network authentication mode. This controls how players are authenticated with both dedicated and
     * listen servers. You can't use Anti-Cheat without turning on some type of network authentication.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Networking")
    ENetworkAuthenticationMode NetworkAuthenticationMode;

    /**
     * Turn this on to enable Anti-Cheat. Enabling anti-cheat will also turn on identity checks on listen servers and
     * enable trusted dedicated servers.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Networking")
    bool EnableAntiCheat;

    /**
     * A list of trusted platforms.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Networking")
    TSet<FName> TrustedPlatforms;

    /**
     * The public signing key for trusted clients (such as consoles). This is used to by other clients and servers to
     * verify that the connecting player is allowed to join without Anti-Cheat.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Networking")
    FString TrustedClientPublicKey;

    /**
     * The private signing key for trusted clients (such as consoles). This is used to sign a proof, enabling the player
     * to connect to an Anti-Cheat protected server without running Anti-Cheat.
     *
     * This value is only set in Build/NoRedist/EOSTrustedClient.ini and in each
     * Platforms/<Platform>/Config/<Platform>Engine.ini for the trusted platforms chosen in Project Settings.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Networking")
    FString TrustedClientPrivateKey;

    /**
     * If enabled, listen servers and trusted dedicated servers will check the connecting player for any BAN sanctions.
     *
     * Turning this option on requires creating a custom policy in the Epic Games Dev Portal with permissions to check
     * sanctions of other players. If you don't set up the custom policy properly, players will not be able to connect.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Networking")
    bool EnableSanctionChecks = true; 

    /**
     * If enabled, listen servers will check the product user ID of connecting players to ensure that they exist as a
     * real account. No credentials are sent, so this doesn't prevent impersonation, but it does prevent players from
     * using junk IDs.
     *
     * This option is required for Anti-Cheat to work.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Networking")
    bool EnableIdentityChecksOnListenServers = true;

    /**
     * Specifies how dedicated servers are distributed for this project. Depending on how dedicated servers are
     * distributed changes how the server IDs, secrets and private keys are stored in configuration.
     *
     * This configuration option is stored in DefaultEditor.ini, not DefaultEngine.ini. The value is not present in
     * packaged versions of the game.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Networking")
    EDedicatedServersDistributionMode DedicatedServerDistributionMode;

    /**
     * If enabled, connecting players will be verified when they connect to dedicated servers using their EOS Connect
     * token. Trusted dedicated servers will also be able to write to Player Data Storage on a connected player's
     * behalf.
     *
     * Trusted dedicated servers required the client-server connection to be encrypted. You can turn on automatic
     * encryption below or manually configure network encryption in your project.
     *
     * This option is required for Anti-Cheat to work.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Networking")
    bool EnableTrustedDedicatedServers;

    /**
     * If enabled, player connections to trusted dedicated servers will automatically negotiate an AES-GCM encrypted
     * connection using the public and private keys that have been set up below. This is vastly simpler than setting up
     * encryption manually, and is suitable for almost all games. The only cases where you might want to configure
     * encryption manually is if you have platform-specific requirements that dictate an encryption algorithm other than
     * AES-GCM.
     *
     * If this option is off, but trusted dedicated servers is on, you MUST configure encryption manually. Trusted
     * dedicated servers can not work without connections being encrypted.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Networking")
    bool EnableAutomaticEncryptionOnTrustedDedicatedServers;

    /**
     * The client ID to use when running as a dedicated server. This value should only ever be set in
     * DedicatedServerEngine.ini to prevent it from being shipped with game clients.
     *
     * For additional security, you can leave this option blank and pass in the client ID on the command line (refer to
     * the documentation).
     */
    UPROPERTY(EditAnywhere, Config, Category = "Networking")
    FString DedicatedServerClientId;

    /**
     * The client secret to use when running as a dedicated server. This value should only ever be set in
     * DedicatedServerEngine.ini to prevent it from being shipped with game clients.
     *
     * For additional security, you can leave this option blank and pass in the client secret on the command line (refer
     * to the documentation).
     */
    UPROPERTY(EditAnywhere, Config, Category = "Networking")
    FString DedicatedServerClientSecret;

    /**
     * The public signing key for dedicated servers. This is used by clients to verify that network
     * commands are actually being sent by trusted dedicated servers. This value is set into DefaultEngine.ini and
     * should be present on all clients.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Networking")
    FString DedicatedServerPublicKey;

    /**
     * The private signing key of dedicated servers. This value should only ever be set in
     * DedicatedServerEngine.ini to prevent it from being shipped with game clients.
     *
     * For additional security, you can leave this option blank and pass in the private key on the command line (refer
     * to the documentation).
     */
    UPROPERTY(EditAnywhere, Config, Category = "Networking")
    FString DedicatedServerPrivateKey;

    /**
     * When enabled, the voice chat system will echo back audio input through your speakers so you can test voice chat
     * without setting up multiple machines.
     *
     * This option only applies to parties with voice chat enabled, and only takes effect in non-Shipping builds.
     */
    UPROPERTY(
        EditAnywhere,
        Config,
        Category = "Voice Chat",
        meta = (DisplayName = "Enable Echo in Parties (Development Only)"))
    bool EnableVoiceChatEchoInParties;

    /**
     * When enabled, the voice chat system will enable noise cancellation by default, if SetSetting has not been called
     * on the IVoiceChatUser to explicitly state otherwise.
     */
    UPROPERTY(
        EditAnywhere,
        Config,
        Category = "Voice Chat",
        meta = (DisplayName = "Enable Platform AEC (Noise Cancellation) By Default"))
    bool EnableVoiceChatPlatformAECByDefault;

    /**
     * Controls how the plugin stores stats in the EOS backend. The EOS backend itself only stores int32 values, so if
     * you want to store floating-point values, you need to configure them here.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Stats")
    TArray<FStatTypingRule> TypingRules;

    /**
     * If true, clients will accept "write stat" requests from listen and dedicated servers. The EOS SDK currently has
     * a bug that prevents you from turning on and using the `ingestForAnyUser` policy, so this workaround allows
     * servers to at least write stats for users that are currently connected to them.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Stats")
    bool AcceptStatWriteRequestsFromServers;
};

UCLASS(Config = Engine, DefaultConfig)
class UOnlineSubsystemEOSEditorConfigFreeEdition : public UOnlineSubsystemEOSEditorConfig
{
    GENERATED_BODY()

public:
    /**
     * The license key for EOS Online Subsystem (Free Edition). You can obtain a license key from
     * https://licensing.redpoint.games/get/eos-online-subsystem-free/
     */
    UPROPERTY(EditAnywhere, Config, Category = "Free Edition")
    FString FreeEditionLicenseKey;
};

class FOnlineSubsystemEOSEditorConfig
{
private:
    TWeakObjectPtr<UOnlineSubsystemEOSEditorConfig> Config;
    ISettingsSectionPtr SettingsSection;

    bool HandleSettingsSaved();

public:
    void RegisterSettings();
    void UnregisterSettings();
};
