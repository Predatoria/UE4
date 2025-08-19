// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EOSCommon.h"
#include "Misc/ConfigCacheIni.h"
#include "OnlineSubsystemRedpointEOS/Shared/WorldResolution.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

#ifdef ONLINESUBSYSTEMEOS_PACKAGE
#include "EOSConfig.generated.h"
#endif

EOS_ENABLE_STRICT_WARNINGS

/**
 * In EOS, you can only have one object advertised with presence; this can either be a party or a session, but not both.
 * This configuration option determines what gets advertised in the presence system.
 */
UENUM()
enum class EPresenceAdvertisementType : uint8
{
    /**
     * Neither parties nor sessions will be advertised with presence.
     */
    None,

    /**
     * Parties will be the type of joinable object advertised with presence. This
     * is the default for backwards compatibility.
     */
    Party,

    /**
     * Sessions will be the type of joinable object advertised with presence.
     */
    Session,
};

/**
 * When new versions of the plugin are released, sometimes the default behaviour changes. When you first use the plugin,
 * it sets the current latest API version in the configuration so that when you upgrade, the behaviour does not change
 * for you until you decide to upgrade.
 */
UENUM()
enum class EEOSApiVersion : uint8
{
    // NOTE: When a new version is added here, it *MUST* be added to the top of
    // the enum so it becomes value 0.

    /**
     * Behaviour as of 2022-05-20. As of this release, the default network authentication mode was set to Recommended.
     */
    v2022_05_20 UMETA(DisplayName = "2022-05-20"),

    /**
     * Behaviour as of 2022-02-11. As of this release, "minslotsavailable" in session searches is set to >= 1 by
     * default, as this was the expected behaviour for a lot of users.
     */
    v2022_02_11 UMETA(DisplayName = "2022-02-11"),
};

UENUM()
enum class EPartyJoinabilityConstraint : uint8
{
    /**
     * When a player accepts an invite from the native platform and the player is already in a party, the player will
     * join the new party in addition to the existing party they are in.  IOnlinePartySystem::GetJoinedParties will
     * return multiple parties.
     */
    AllowPlayersInMultipleParties UMETA(DisplayName = "Allow players to join multiple parties at the same time"),

    /**
     * When a player accepts an invite from the native platform and the player is already in a party, the accepted
     * invite is ignored and the IOnlinePartySystem::OnPartyInviteResponseReceived event is fired to indicate an invite
     * was ignored.
     */
    IgnoreInvitesIfAlreadyInParty UMETA(DisplayName = "Ignore accepted invites if player is already in a party")
};

UENUM()
enum class ENetworkAuthenticationMode : uint8
{
    /**
     * The networking authentication mode is selected based on your API version.
     * - 2022-05-20 defaults to IDToken.
     * - 2022-02-11 defaults to UserCredentials.
     */
    Default UMETA(DisplayName = "Default"),

    /**
     * Turns on trusted dedicated servers if relevant for the type of distribution and sanction checks. Users are
     * verified:
     * - With ID tokens when connecting to trusted dedicated servers. While you can't access Player Data Storage, it
     *   works for any kind of user, regardless of how they authenticated.
     * - Comparison of P2P sending address when connecting to listen servers over EOS P2P. Does not support split-screen
     *   connections to listen servers, as there is no way to verify the additional users on the inbound connection.
     * - Authentication is skipped when connecting to an untrusted dedicated server (typically when the distribution
     *   mode is not set to "developer only").
     * - Authentication is skipped when connecting to a listen server over IP.
     * - Connections are rejected in all other cases.
     */
    IDToken UMETA(DisplayName = "ID Token"),

    /**
     * Toggle network authentication options individually. Users are verified:
     * - With the player credentials when connecting to trusted dedicated servers. You can access Player Data Storage on
     *   the server on behalf of connected users, but authentication will only work if the player is connecting from a
     *   platform when the access tokens are not single use.
     * - By querying IUserInfo to check if the claimed account exists when connecting to listen servers. Makes no
     *   attempt to verify that the connecting user actually is that account.
     * - Connections are rejected in all other cases, such as connections to untrusted dedicated servers.
     */
    UserCredentials UMETA(DisplayName = "User Credentials (Legacy)"),

    /**
     * Network authentication is turned off. No validation of connecting players is done; anyone can connect to any
     * server. You can not use Anti-Cheat in this mode.
     */
    Off UMETA(DisplayName = "Off"),
};

UENUM()
enum class EDedicatedServersDistributionMode : uint8
{
    /**
     * In developers only mode, dedicated servers can have the client ID and secret embedded in their packaged binaries.
     * You must ensure that the binaries are never distributed to end users, otherwise they will be able to run trusted
     * dedicated servers and impersonate other users on their behalf.
     */
    DevelopersOnly UMETA(DisplayName = "Dedicated server binaries are only ever distributed and run by developers"),

    /**
     * If dedicated servers are being run by both developers and players, the dedicated server secrets will be stored in
     * Build/NoRedist/DedicatedServerEngine.ini. You must then provide the values in this file at runtime when running
     * the dedicated servers on your own infrastructure. Dedicated servers run by players will be considered untrusted,
     * and will not perform user verification checks.
     */
    DevelopersAndPlayers UMETA(DisplayName = "Both developers and players will run dedicated servers"),

    /**
     * If dedicated servers are only being run by players, this turns off all the trusted dedicated server code paths.
     */
    PlayersOnly UMETA(DisplayName = "Dedicated server binaries will be distributed to players and only run by players"),
};

UENUM()
enum class EStatTypingRule : uint8
{
    /**
     * The stat is stored as a 32-bit signed integer. This is the native stat format for EOS, and is compatible
     * with achievements and leaderboards.
     *
     * On the EOS backend, the stat can be configured as LATEST, SUM, MIN or MAX.
     */
    Int32 UMETA(DisplayName = "Int32"),

    /**
     * The boolean value is converted to a 32-bit signed integer (either a 0 or a 1). Because this value can be
     * entirely contained within a 32-bit signed integer, this stat is compatible with achievements and leaderboards.
     *
     * When configuring achievements, remember that the values for this stat can be either 0 or 1, so you'll typically
     * want to unlock the achievement on a value of 1.
     *
     * On the EOS backend, the stat can be configured as LATEST, SUM, MIN or MAX.
     */
    Bool UMETA(DisplayName = "Boolean"),

    /**
     * The floating point value is multiplied by 10,000,000, truncated and stored as an int32. This preserves
     * ordering and permits it's use in leaderboards, at the cost of range. The maximum range for a stat
     * using truncated floats is -214.7483648 to 214.7483647.
     *
     * When using this stat with achievements and leaderboards, you must remember that the values will
     * be stored with the multipler, and set achievement thresholds appropriately, and divide by 10,000,000
     * when displaying leaderboard values.
     *
     * On the EOS backend, the stat can be configured as LATEST, SUM, MIN or MAX.
     */
    FloatTruncated UMETA(DisplayName = "Float (Truncated)"),

    /**
     * The bits that make up the floating point number are treated as an int32, and this value is stored.
     * This preserves the precision of the floating point number, and allows you to store the full range of values,
     * at the cost of ordering (making it unsuitable for achievements and leaderboards). You should only read
     * these kinds of stats through QueryStats.
     *
     * On the EOS backend, the stat must be configured as LATEST.
     */
    FloatEncoded UMETA(DisplayName = "Float (Encoded)"),

    /**
     * The bits that make up the double floating point number are split into two int32 values, and these are
     * independently stored in EOS with the name <stat>_upper and <stat>_lower.
     *
     * This preserves the precision of the double floating point number, and allows you to store the full range
     * of values, but does not preserve ordering and you can't use the values in any other system (achievements
     * or leaderboards), as the value is split over two underlying stats.
     *
     * On the EOS backend, both the <stat>_upper and <stat>_lower stats must be configured as LATEST.
     */
    DoubleEncoded UMETA(DisplayName = "Double (Encoded)")
};

FORCEINLINE bool EOS_ApiVersionIsAtLeast(EEOSApiVersion InConfigVersion, EEOSApiVersion InTargetVersion)
{
    return InConfigVersion <= InTargetVersion;
}

class ONLINESUBSYSTEMREDPOINTEOS_API FEOSConfig : public TSharedFromThis<FEOSConfig>
{
    friend class FDelegatedSubsystems;

protected:
    virtual FString GetDelegatedSubsystemsString() const = 0;

public:
    FEOSConfig(){};
    virtual ~FEOSConfig(){};
    UE_NONCOPYABLE(FEOSConfig);

    virtual void TryLoadDependentModules();

    virtual FString GetFreeEditionLicenseKey() const = 0;
    virtual FString GetEncryptionKey() const = 0;
    virtual FString GetProductName() const = 0;
    virtual FString GetProductVersion() const = 0;
    virtual FString GetProductId() const = 0;
    virtual FString GetSandboxId() const = 0;
    virtual FString GetDeploymentId() const = 0;
    virtual FString GetClientId() const = 0;
    virtual FString GetClientSecret() const = 0;
    virtual FString GetDedicatedServerClientId() const = 0;
    virtual FString GetDedicatedServerClientSecret() const = 0;
    virtual FString GetDeveloperToolAddress() const = 0;
    virtual FString GetDeveloperToolDefaultCredentialName() const = 0;
    virtual FString GetWidgetClass(FString WidgetName, FString DefaultValue) const = 0;
    virtual EEOSApiVersion GetApiVersion() const = 0;
    virtual FName GetAuthenticationGraph() const = 0;
    virtual FName GetEditorAuthenticationGraph() const = 0;
    virtual FName GetCrossPlatformAccountProvider() const = 0;
    virtual bool GetRequireCrossPlatformAccount() const = 0;
    virtual EPresenceAdvertisementType GetPresenceAdvertisementType() const = 0;
    virtual bool GetPersistentLoginEnabled() const = 0;
    virtual bool IsAutomatedTesting() const = 0;
    virtual bool GetRequireEpicGamesLauncher() const = 0;
    virtual FString GetSimpleFirstPartyLoginUrl() const = 0;
    virtual EPartyJoinabilityConstraint GetPartyJoinabilityConstraint() const = 0;
    virtual bool GetEnableSanctionChecks() const = 0;
    virtual bool GetEnableIdentityChecksOnListenServers() const = 0;
    virtual bool GetEnableTrustedDedicatedServers() const = 0;
    virtual bool GetEnableAutomaticEncryptionOnTrustedDedicatedServers() const = 0;
    virtual EDedicatedServersDistributionMode GetDedicatedServerDistributionMode() const = 0;
    virtual FString GetDedicatedServerPublicKey() const = 0;
    virtual FString GetDedicatedServerPrivateKey() const = 0;
    virtual bool GetEnableAntiCheat() const = 0;
    virtual FString GetTrustedClientPublicKey() const = 0;
    virtual FString GetTrustedClientPrivateKey() const = 0;
    virtual bool GetEnableVoiceChatEchoInParties() const = 0;
    virtual bool GetEnableVoiceChatPlatformAECByDefault() const = 0;
    virtual TArray<TTuple<FString, EStatTypingRule>> GetStatTypingRules() const = 0;
    virtual bool GetAcceptStatWriteRequestsFromServers() const = 0;
    virtual ENetworkAuthenticationMode GetNetworkAuthenticationMode() const = 0;
};

class ONLINESUBSYSTEMREDPOINTEOS_API FEOSConfigIni : public FEOSConfig
{
protected:
    virtual FString GetDelegatedSubsystemsString() const override;

public:
    FEOSConfigIni(){};
    virtual ~FEOSConfigIni(){};
    UE_NONCOPYABLE(FEOSConfigIni);

    virtual FString GetFreeEditionLicenseKey() const override;
    virtual FString GetEncryptionKey() const override;
    virtual FString GetProductName() const override;
    virtual FString GetProductVersion() const override;
    virtual FString GetProductId() const override;
    virtual FString GetSandboxId() const override;
    virtual FString GetDeploymentId() const override;
    virtual FString GetClientId() const override;
    virtual FString GetClientSecret() const override;
    virtual FString GetDedicatedServerClientId() const override;
    virtual FString GetDedicatedServerClientSecret() const override;
    virtual FString GetDeveloperToolAddress() const override;
    virtual FString GetDeveloperToolDefaultCredentialName() const override;
    virtual FString GetWidgetClass(FString WidgetName, FString DefaultValue) const override;
    virtual EEOSApiVersion GetApiVersion() const override;
    virtual FName GetAuthenticationGraph() const override;
    virtual FName GetEditorAuthenticationGraph() const override;
    virtual FName GetCrossPlatformAccountProvider() const override;
    virtual bool GetRequireCrossPlatformAccount() const override;
    virtual EPresenceAdvertisementType GetPresenceAdvertisementType() const override;
    virtual bool GetPersistentLoginEnabled() const override;
    virtual bool IsAutomatedTesting() const override;
    virtual bool GetRequireEpicGamesLauncher() const override;
    virtual FString GetSimpleFirstPartyLoginUrl() const override;
    virtual EPartyJoinabilityConstraint GetPartyJoinabilityConstraint() const override;
    virtual bool GetEnableSanctionChecks() const override;
    virtual bool GetEnableIdentityChecksOnListenServers() const override;
    virtual bool GetEnableTrustedDedicatedServers() const override;
    virtual bool GetEnableAutomaticEncryptionOnTrustedDedicatedServers() const override;
    virtual EDedicatedServersDistributionMode GetDedicatedServerDistributionMode() const override;
    virtual FString GetDedicatedServerPublicKey() const override;
    virtual FString GetDedicatedServerPrivateKey() const override;
    virtual bool GetEnableAntiCheat() const override;
    virtual FString GetTrustedClientPublicKey() const override;
    virtual FString GetTrustedClientPrivateKey() const override;
    virtual bool GetEnableVoiceChatEchoInParties() const override;
    virtual bool GetEnableVoiceChatPlatformAECByDefault() const override;
    virtual TArray<TTuple<FString, EStatTypingRule>> GetStatTypingRules() const override;
    virtual bool GetAcceptStatWriteRequestsFromServers() const override;
    virtual ENetworkAuthenticationMode GetNetworkAuthenticationMode() const override;

private:
    FString GetConfigValue(const FString &Key, const FString &DefaultValue) const;
    FString GetConfigValue(const FString &Key, const TCHAR *DefaultValue) const;
    bool GetConfigValue(const FString &Key, bool DefaultValue) const;
};

EOS_DISABLE_STRICT_WARNINGS