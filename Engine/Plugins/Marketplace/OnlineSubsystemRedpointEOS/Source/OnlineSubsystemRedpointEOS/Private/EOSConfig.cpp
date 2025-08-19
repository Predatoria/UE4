// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"

#include "Engine/Engine.h"
#include "Modules/ModuleManager.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "UObject/UObjectIterator.h"

EOS_ENABLE_STRICT_WARNINGS

void FEOSConfig::TryLoadDependentModules()
{
    FModuleManager &ModuleManager = FModuleManager::Get();

    // Load modules that are needed by delegated subsystems.
    TArray<FString> SubsystemList;
    GetDelegatedSubsystemsString().ParseIntoArray(SubsystemList, TEXT(","), true);
    for (const auto &SubsystemName : SubsystemList)
    {
        FString ModuleName = FString::Printf(TEXT("OnlineSubsystem%s"), *SubsystemName);
        auto Module = ModuleManager.LoadModule(FName(*ModuleName));
        if (Module == nullptr)
        {
            UE_LOG(LogEOS, Warning, TEXT("Unable to load module for delegated subsystem: %s"), *ModuleName);
        }
    }

    // Try to load the platform extension module for the current platform.
    {
#if (PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX || PLATFORM_IOS || PLATFORM_TVOS || PLATFORM_ANDROID) &&       \
    !PLATFORM_IS_EXTENSION
        ModuleManager.LoadModule(FName("RedpointEOSPlatformDefault"));
#else
        auto PlatformName = FName(PREPROCESSOR_TO_STRING(PREPROCESSOR_JOIN(RedpointEOSPlatform, PLATFORM_HEADER_NAME)));
        verifyf(
            ModuleManager.LoadModule(PlatformName) != nullptr,
            TEXT("If this load operation fails, then we could not load the native platform extension. For non-public "
                 "platforms, you must have a native platform extension so that the plugin can load EOS."));
#endif
    }
}

FString FEOSConfigIni::GetDelegatedSubsystemsString() const
{
    return GetConfigValue(TEXT("DelegatedSubsystems"), TEXT(""));
}

FString FEOSConfigIni::GetFreeEditionLicenseKey() const
{
    return GetConfigValue(TEXT("FreeEditionLicenseKey"), TEXT(""));
}

FString FEOSConfigIni::GetEncryptionKey() const
{
    return GetConfigValue(
        TEXT("PlayerDataEncryptionKey"),
        TEXT("4dc4ad8a46823586f4044225d6cf1b1e7ee32b3d7dff1c63b6ad5807671c4a3f"));
}

FString FEOSConfigIni::GetProductName() const
{
    return GetConfigValue(TEXT("ProductName"), TEXT("Product Name Not Set"));
}

FString FEOSConfigIni::GetProductVersion() const
{
    return GetConfigValue(TEXT("ProductVersion"), TEXT("0.0.0"));
}

FString FEOSConfigIni::GetProductId() const
{
    return GetConfigValue(TEXT("ProductId"), TEXT(""));
}

FString FEOSConfigIni::GetSandboxId() const
{
    return GetConfigValue(TEXT("SandboxId"), TEXT(""));
}

FString FEOSConfigIni::GetDeploymentId() const
{
    return GetConfigValue(TEXT("DeploymentId"), TEXT(""));
}

FString FEOSConfigIni::GetClientId() const
{
    return GetConfigValue(TEXT("ClientId"), TEXT(""));
}

FString FEOSConfigIni::GetClientSecret() const
{
    return GetConfigValue(TEXT("ClientSecret"), TEXT(""));
}

FString FEOSConfigIni::GetDedicatedServerClientId() const
{
#if WITH_EDITOR
    FString Value = TEXT("");
    GConfig->GetString(TEXT("EpicOnlineServices"), TEXT("DedicatedServerClientId"), Value, GEditorIni);
    return Value;
#else
    return GetConfigValue(TEXT("DedicatedServerClientId"), TEXT(""));
#endif
}

FString FEOSConfigIni::GetDedicatedServerClientSecret() const
{
#if WITH_EDITOR
    FString Value = TEXT("");
    GConfig->GetString(TEXT("EpicOnlineServices"), TEXT("DedicatedServerClientSecret"), Value, GEditorIni);
    return Value;
#else
    return GetConfigValue(TEXT("DedicatedServerClientSecret"), TEXT(""));
#endif
}

FString FEOSConfigIni::GetDeveloperToolAddress() const
{
    return GetConfigValue(TEXT("DevAuthToolAddress"), TEXT("localhost:6300"));
}

FString FEOSConfigIni::GetDeveloperToolDefaultCredentialName() const
{
    return GetConfigValue(TEXT("DevAuthToolDefaultCredentialName"), TEXT("Context_1"));
}

FString FEOSConfigIni::GetWidgetClass(FString WidgetName, FString DefaultValue) const
{
    return GetConfigValue(FString::Printf(TEXT("WidgetClass_%s"), *WidgetName), DefaultValue);
}

EEOSApiVersion FEOSConfigIni::GetApiVersion() const
{
    FString ApiVersionStr = GetConfigValue(TEXT("ApiVersion"), TEXT(""));

    if (ApiVersionStr.IsEmpty())
    {
        // Pick latest for backwards compatibility.
        return (EEOSApiVersion)0;
    }

    // It's not safe to use FindObject here, because the UObject system might not have initialized yet. Instead we used
    // the provided FOREACH_ENUM_EEOSAPIVERSION macro to enumerate through all the available options and string compare
    // them.
    static int32 LastSelectedApiVersion = -1;
#define EEOS_API_VERSION_EVALUATE(op)                                                                                  \
    {                                                                                                                  \
        FString CandidateStr = FString(TEXT(#op)).Mid(FString(TEXT("EEOSApiVersion::")).Len());                        \
        if (CandidateStr == ApiVersionStr)                                                                             \
        {                                                                                                              \
            if (LastSelectedApiVersion != (int32)(op))                                                                 \
            {                                                                                                          \
                UE_LOG(                                                                                                \
                    LogEOS,                                                                                            \
                    Verbose,                                                                                           \
                    TEXT("Using API version '%s' because that is what is selected in configuration."),                 \
                    *CandidateStr);                                                                                    \
                LastSelectedApiVersion = (int32)(op);                                                                  \
            }                                                                                                          \
            return op;                                                                                                 \
        }                                                                                                              \
    }
    FOREACH_ENUM_EEOSAPIVERSION(EEOS_API_VERSION_EVALUATE);

    UE_LOG(
        LogEOS,
        Error,
        TEXT("The ApiVersion in your configuration '%s' is not a valid option. Defaulting to latest version."),
        *ApiVersionStr);
    return (EEOSApiVersion)0;
}

bool FEOSConfigIni::IsAutomatedTesting() const
{
    return false;
}

FName FEOSConfigIni::GetAuthenticationGraph() const
{
    return FName(*GetConfigValue(TEXT("AuthenticationGraph"), TEXT("Default")));
}

FName FEOSConfigIni::GetEditorAuthenticationGraph() const
{
    return FName(*GetConfigValue(TEXT("EditorAuthenticationGraph"), TEXT("Default")));
}

FName FEOSConfigIni::GetCrossPlatformAccountProvider() const
{
    return FName(*GetConfigValue(TEXT("CrossPlatformAccountProvider"), TEXT("None")));
}

bool FEOSConfigIni::GetRequireCrossPlatformAccount() const
{
    return GetConfigValue(TEXT("RequireCrossPlatformAccount"), false);
}

EPresenceAdvertisementType FEOSConfigIni::GetPresenceAdvertisementType() const
{
    auto PresenceType = GetConfigValue(TEXT("PresenceAdvertises"), TEXT("Party"));
    if (PresenceType == TEXT("Party"))
    {
        return EPresenceAdvertisementType::Party;
    }
    else if (PresenceType == TEXT("Session"))
    {
        return EPresenceAdvertisementType::Session;
    }
    else
    {
        return EPresenceAdvertisementType::None;
    }
}

bool FEOSConfigIni::GetPersistentLoginEnabled() const
{
    return !GetConfigValue(TEXT("DisablePersistentLogin"), false);
}

bool FEOSConfigIni::GetRequireEpicGamesLauncher() const
{
    return GetConfigValue(TEXT("RequireEpicGamesLauncher"), false);
}

FString FEOSConfigIni::GetSimpleFirstPartyLoginUrl() const
{
    return GetConfigValue(TEXT("SimpleFirstPartyLoginUrl"), TEXT(""));
}

EPartyJoinabilityConstraint FEOSConfigIni::GetPartyJoinabilityConstraint() const
{
    auto Constraint = GetConfigValue(TEXT("PartyJoinabilityConstraint"), TEXT(""));
    if (Constraint == TEXT("AllowPlayersInMultipleParties"))
    {
        return EPartyJoinabilityConstraint::AllowPlayersInMultipleParties;
    }
    else if (Constraint == TEXT("IgnoreInvitesIfAlreadyInParty"))
    {
        return EPartyJoinabilityConstraint::IgnoreInvitesIfAlreadyInParty;
    }

    // No option or invalid option set.
    return EPartyJoinabilityConstraint::AllowPlayersInMultipleParties;
}

bool FEOSConfigIni::GetEnableSanctionChecks() const
{
    switch (GetNetworkAuthenticationMode())
    {
    case ENetworkAuthenticationMode::IDToken:
        return true;
    case ENetworkAuthenticationMode::Off:
        return false;
    case ENetworkAuthenticationMode::UserCredentials:
    default:
        return GetConfigValue(TEXT("EnableSanctionChecks"), false);
    }
}

bool FEOSConfigIni::GetEnableIdentityChecksOnListenServers() const
{
    switch (GetNetworkAuthenticationMode())
    {
    case ENetworkAuthenticationMode::IDToken:
        return true;
    case ENetworkAuthenticationMode::Off:
        return false;
    case ENetworkAuthenticationMode::UserCredentials:
    default:
        return GetConfigValue(TEXT("EnableIdentityChecksOnListenServers"), true);
    }
}

bool FEOSConfigIni::GetEnableTrustedDedicatedServers() const
{
    switch (GetNetworkAuthenticationMode())
    {
    case ENetworkAuthenticationMode::IDToken:
        return true;
    case ENetworkAuthenticationMode::Off:
        return false;
    case ENetworkAuthenticationMode::UserCredentials:
    default:
        return GetConfigValue(TEXT("EnableTrustedDedicatedServers"), true);
    }
}

bool FEOSConfigIni::GetEnableAutomaticEncryptionOnTrustedDedicatedServers() const
{
    return GetConfigValue(TEXT("EnableAutomaticEncryptionOnTrustedDedicatedServers"), true);
}

EDedicatedServersDistributionMode FEOSConfigIni::GetDedicatedServerDistributionMode() const
{
#if WITH_EDITOR
    FString Value = TEXT("DevelopersAndPlayers");
    GConfig->GetString(TEXT("EpicOnlineServices"), TEXT("DedicatedServerDistributionMode"), Value, GEditorIni);
    if (Value == TEXT("DevelopersOnly"))
    {
        return EDedicatedServersDistributionMode::DevelopersOnly;
    }
    else if (Value == TEXT("DevelopersAndPlayers"))
    {
        return EDedicatedServersDistributionMode::DevelopersAndPlayers;
    }
    else
    {
        return EDedicatedServersDistributionMode::PlayersOnly;
    }
#else
    checkf(
        false,
        TEXT("GetDedicatedServerDistributionMode should not be called outside editor code or a WITH_EDITOR block!"));
    return EDedicatedServersDistributionMode::PlayersOnly;
#endif
}

FString FEOSConfigIni::GetDedicatedServerPublicKey() const
{
    return GetConfigValue(TEXT("DedicatedServerPublicKey"), TEXT(""));
}

FString FEOSConfigIni::GetDedicatedServerPrivateKey() const
{
#if WITH_EDITOR
    FString Value = TEXT("");
    GConfig->GetString(TEXT("EpicOnlineServices"), TEXT("DedicatedServerPrivateKey"), Value, GEditorIni);
    return Value;
#else
    return GetConfigValue(TEXT("DedicatedServerPrivateKey"), TEXT(""));
#endif
}

bool FEOSConfigIni::GetEnableAntiCheat() const
{
    switch (GetNetworkAuthenticationMode())
    {
    case ENetworkAuthenticationMode::Off:
        return false;
    case ENetworkAuthenticationMode::IDToken:
    case ENetworkAuthenticationMode::UserCredentials:
    default:
        return GetConfigValue(TEXT("EnableAntiCheat"), false);
    }
}

FString FEOSConfigIni::GetTrustedClientPublicKey() const
{
    return GetConfigValue(TEXT("TrustedClientPublicKey"), TEXT(""));
}

FString FEOSConfigIni::GetTrustedClientPrivateKey() const
{
    return GetConfigValue(TEXT("TrustedClientPrivateKey"), TEXT(""));
}

bool FEOSConfigIni::GetEnableVoiceChatEchoInParties() const
{
    return GetConfigValue(TEXT("EnableVoiceChatEchoInParties"), false);
}

bool FEOSConfigIni::GetEnableVoiceChatPlatformAECByDefault() const
{
    return GetConfigValue(TEXT("EnableVoiceChatPlatformAECByDefault"), false);
}

static TArray<TTuple<FString, EStatTypingRule>> ParseStatTypingRules(const TArray<FString>& Value)
{
    TArray<TTuple<FString, EStatTypingRule>> Results;
    for (const FString &V : Value)
    {
        TArray<FString> Components;
        V.ParseIntoArray(Components, TEXT(":"));
        if (Components.Num() == 2)
        {
            EStatTypingRule Rule = EStatTypingRule::Int32;
            if (Components[1] == TEXT("Int32"))
            {
                Rule = EStatTypingRule::Int32;
            }
            else if (Components[1] == TEXT("Bool"))
            {
                Rule = EStatTypingRule::Bool;
            }
            else if (Components[1] == TEXT("FloatTruncated"))
            {
                Rule = EStatTypingRule::FloatTruncated;
            }
            else if (Components[1] == TEXT("FloatEncoded"))
            {
                Rule = EStatTypingRule::FloatEncoded;
            }
            else if (Components[1] == TEXT("DoubleEncoded"))
            {
                Rule = EStatTypingRule::DoubleEncoded;
            }
            Results.Add(TTuple<FString, EStatTypingRule>(Components[0], Rule));
        }
    }
    return Results;
}

TArray<TTuple<FString, EStatTypingRule>> FEOSConfigIni::GetStatTypingRules() const
{
    TArray<FString> Value;
    GConfig->GetArray(TEXT("EpicOnlineServices"), TEXT("StatTypingRules"), Value, GEngineIni);
    return ParseStatTypingRules(Value);
}

bool FEOSConfigIni::GetAcceptStatWriteRequestsFromServers() const
{
    return GetConfigValue(TEXT("AcceptStatWriteRequestsFromServers"), false);
}

ENetworkAuthenticationMode FEOSConfigIni::GetNetworkAuthenticationMode() const
{
    auto AuthMode = GetConfigValue(TEXT("NetworkAuthenticationMode"), TEXT("Default"));
    if (AuthMode == TEXT("IDToken"))
    {
        return ENetworkAuthenticationMode::IDToken;
    }
    else if (AuthMode == TEXT("UserCredentials"))
    {
        return ENetworkAuthenticationMode::UserCredentials;
    }
    else if (AuthMode == TEXT("Off"))
    {
        return ENetworkAuthenticationMode::Off;
    }
    else // including "Default"
    {
        if (EOS_ApiVersionIsAtLeast(GetApiVersion(), EEOSApiVersion::v2022_05_20))
        {
            return ENetworkAuthenticationMode::IDToken;
        }
        else
        {
            return ENetworkAuthenticationMode::UserCredentials;
        }
    }
}

FString FEOSConfigIni::GetConfigValue(const FString &Key, const FString &DefaultValue) const
{
    return GetConfigValue(Key, *DefaultValue);
}

FString FEOSConfigIni::GetConfigValue(const FString &Key, const TCHAR *DefaultValue) const
{
    FString Value = DefaultValue;
    GConfig->GetString(TEXT("EpicOnlineServices"), *Key, Value, GEngineIni);
    return Value;
}

bool FEOSConfigIni::GetConfigValue(const FString &Key, bool DefaultValue) const
{
    bool Value = DefaultValue;
    GConfig->GetBool(TEXT("EpicOnlineServices"), *Key, Value, GEngineIni);
    return Value;
}

EOS_DISABLE_STRICT_WARNINGS