// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Misc/CoreDelegates.h"
#include "Misc/MessageDialog.h"
#include "Misc/Parse.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/Full/SocketSubsystemEOSFull.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/ISocketSubsystemEOS.h"
#include "OnlineSubsystemRedpointEOS/Private/OnlineAvatarInterfaceSynthetic.h"
#include "OnlineSubsystemRedpointEOS/Private/OnlineLobbyInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Private/OnlineVoiceAdminInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Private/Orchestrator/AgonesServerOrchestrator.h"
#include "OnlineSubsystemRedpointEOS/Shared/AntiCheat/EOSDedicatedServerAntiCheat.h"
#include "OnlineSubsystemRedpointEOS/Shared/AntiCheat/EOSGameAntiCheat.h"
#include "OnlineSubsystemRedpointEOS/Shared/AntiCheat/EditorAntiCheat.h"
#include "OnlineSubsystemRedpointEOS/Shared/AntiCheat/NullAntiCheat.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/CrossPlatformAccountId.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSLifecycleManager.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSMessagingHub.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSRegulatedTicker.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSRuntimePlatform.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineIdentityInterfaceEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineSubsystemRedpointEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/MultiOperation.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineAchievementsInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineEntitlementsInterfaceSynthetic.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineExternalUIInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineFriendsInterfaceSynthetic.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineIdentityInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineLeaderboardsInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlinePartyInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlinePresenceInterfaceSynthetic.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlinePurchaseInterfaceSynthetic.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSessionInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineStatsInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineStoreInterfaceV2Synthetic.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineTitleFileInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineUserCloudInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineUserEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineUserInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineVoiceInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/SyntheticPartySessionManager.h"
#include "OnlineSubsystemRedpointEOS/Shared/VoiceChat/VoiceManager.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

#define LOCTEXT_NAMESPACE "FOnlineSubsystemRedpointEOSModule"

FOnlineSubsystemEOS::FOnlineSubsystemEOS(
    FName InInstanceName,
    FOnlineSubsystemRedpointEOSModule *InModule,
    const TSharedRef<class IEOSRuntimePlatform> &InRuntimePlatform,
    const TSharedRef<FEOSConfig> &InConfig)
    : FOnlineSubsystemImplBase(REDPOINT_EOS_SUBSYSTEM, InInstanceName)
    , PlatformHandle(nullptr)
#if EOS_VERSION_AT_LEAST(1, 12, 0) && (!defined(UE_SERVER) || !UE_SERVER)
    , EOSAntiCheatClient(nullptr)
#endif
    , ActAsPlatformHandle(nullptr)
    , Module(InModule)
    , Config(InConfig)
    , RegulatedTicker(MakeShared<FEOSRegulatedTicker>(60))
    , TickerHandle()
    , RuntimePlatform(InRuntimePlatform)
    , UserFactory(nullptr)
    , AntiCheat(nullptr)
#if defined(EOS_VOICE_CHAT_SUPPORTED)
    , VoiceManager(nullptr)
#endif
#if EOS_HAS_ORCHESTRATORS
    , ServerOrchestrator(nullptr)
#endif // #if EOS_HAS_ORCHESTRATORS
#if EOS_HAS_AUTHENTICATION
    , SubsystemEAS(nullptr)
    , LobbyImpl(nullptr)
    , FriendsImpl(nullptr)
    , PresenceImpl(nullptr)
    , PartyImpl(nullptr)
    , UserCloudImpl(nullptr)
    , SyntheticPartySessionManager(nullptr)
    , AvatarImpl(nullptr)
#endif // #if EOS_HAS_AUTHENTICATION
    , SessionImpl(nullptr)
    , IdentityImpl(nullptr)
    , UserImpl(nullptr)
    , TitleFileImpl(nullptr)
    , AchievementsImpl(nullptr)
    , StatsImpl(nullptr)
    , LeaderboardsImpl(nullptr)
    , EntitlementsImpl(nullptr)
    , StoreV2Impl(nullptr)
    , PurchaseImpl(nullptr)
    , OnPreExitHandle()
    , SocketSubsystem(nullptr)
    , bConfigCanBeSwitched(true)
    , bDidEarlyDestroyForEditor(false)
{
    Module->SubsystemInstances.Add(this->InstanceName, this);
}

FOnlineSubsystemEOS::~FOnlineSubsystemEOS()
{
    if (!this->bDidEarlyDestroyForEditor)
    {
        Module->SubsystemInstances.Remove(this->InstanceName);
        Module->SubsystemInstancesTickedFlag.Remove(this->InstanceName);
    }
}

bool FOnlineSubsystemEOS::IsEnabled() const
{
    return true;
}

EOS_HPlatform FOnlineSubsystemEOS::GetPlatformInstance() const
{
    return this->PlatformHandle;
}

EOS_HPlatform FOnlineSubsystemEOS::GetActAsPlatformInstance() const
{
    return this->ActAsPlatformHandle;
}

void FOnlineSubsystemEOS::RegisterListeningAddress(
    EOS_ProductUserId InProductUserId,
    TSharedRef<const FInternetAddr> InInternetAddr,
    TArray<TSharedPtr<FInternetAddr>> InDeveloperInternetAddrs)
{
    check(this->SessionImpl.IsValid());

    StaticCastSharedPtr<FOnlineSessionInterfaceEOS>(this->SessionImpl)
        ->RegisterListeningAddress(InProductUserId, MoveTemp(InInternetAddr), MoveTemp(InDeveloperInternetAddrs));
}

void FOnlineSubsystemEOS::DeregisterListeningAddress(
    EOS_ProductUserId InProductUserId,
    TSharedRef<const FInternetAddr> InInternetAddr)
{
    check(this->SessionImpl.IsValid());

    StaticCastSharedPtr<FOnlineSessionInterfaceEOS>(this->SessionImpl)
        ->DeregisterListeningAddress(InProductUserId, MoveTemp(InInternetAddr));
}

IOnlineSessionPtr FOnlineSubsystemEOS::GetSessionInterface() const
{
    return this->SessionImpl;
}

IOnlineFriendsPtr FOnlineSubsystemEOS::GetFriendsInterface() const
{
#if EOS_HAS_AUTHENTICATION
    return this->FriendsImpl;
#else
    return nullptr;
#endif // #if EOS_HAS_AUTHENTICATION
}

IOnlineIdentityPtr FOnlineSubsystemEOS::GetIdentityInterface() const
{
    return this->IdentityImpl;
}

IOnlinePresencePtr FOnlineSubsystemEOS::GetPresenceInterface() const
{
#if EOS_HAS_AUTHENTICATION
    return this->PresenceImpl;
#else
    return nullptr;
#endif // #if EOS_HAS_AUTHENTICATION
}

IOnlinePartyPtr FOnlineSubsystemEOS::GetPartyInterface() const
{
#if EOS_HAS_AUTHENTICATION
    return this->PartyImpl;
#else
    return nullptr;
#endif // #if EOS_HAS_AUTHENTICATION
}

IOnlineUserPtr FOnlineSubsystemEOS::GetUserInterface() const
{
    return this->UserImpl;
}

IOnlineUserCloudPtr FOnlineSubsystemEOS::GetUserCloudInterface() const
{
#if EOS_HAS_AUTHENTICATION
    return this->UserCloudImpl;
#else
    return nullptr;
#endif // #if EOS_HAS_AUTHENTICATION
}

IOnlineTitleFilePtr FOnlineSubsystemEOS::GetTitleFileInterface() const
{
#if !EOS_VERSION_AT_LEAST(1, 8, 0)
    UE_LOG(LogEOS, Error, TEXT("You must compile against EOS SDK 1.8 or later to use the IOnlineTitleFile interface."));
#endif

    return this->TitleFileImpl;
}

IOnlineLeaderboardsPtr FOnlineSubsystemEOS::GetLeaderboardsInterface() const
{
    return this->LeaderboardsImpl;
}

IOnlineEntitlementsPtr FOnlineSubsystemEOS::GetEntitlementsInterface() const
{
    return this->EntitlementsImpl;
}

IOnlineStoreV2Ptr FOnlineSubsystemEOS::GetStoreV2Interface() const
{
    return this->StoreV2Impl;
}

IOnlinePurchasePtr FOnlineSubsystemEOS::GetPurchaseInterface() const
{
    return this->PurchaseImpl;
}

IOnlineAchievementsPtr FOnlineSubsystemEOS::GetAchievementsInterface() const
{
    return this->AchievementsImpl;
}

IOnlineStatsPtr FOnlineSubsystemEOS::GetStatsInterface() const
{
    return this->StatsImpl;
}

IOnlineExternalUIPtr FOnlineSubsystemEOS::GetExternalUIInterface() const
{
#if EOS_HAS_AUTHENTICATION
    return this->ExternalUIImpl;
#else
    return nullptr;
#endif // #if EOS_HAS_AUTHENTICATION
}

#if defined(EOS_VOICE_CHAT_SUPPORTED)
TSharedPtr<FEOSVoiceManager> FOnlineSubsystemEOS::GetVoiceManager() const
{
    return this->VoiceManager;
}

IOnlineVoicePtr FOnlineSubsystemEOS::GetVoiceInterface() const
{
    return this->VoiceImpl;
}
#endif

class UObject *FOnlineSubsystemEOS::GetNamedInterface(FName InterfaceName)
{
#if EOS_HAS_AUTHENTICATION
    if (InterfaceName.IsEqual(ONLINE_LOBBY_INTERFACE_NAME))
    {
        return (class UObject *)(void *)&this->LobbyImpl;
    }
    if (InterfaceName.IsEqual(ONLINE_AVATAR_INTERFACE_NAME))
    {
        return (class UObject *)(void *)&this->AvatarImpl;
    }
#endif // #if EOS_HAS_AUTHENTICATION
#if defined(EOS_VOICE_CHAT_SUPPORTED)
    if (InterfaceName.IsEqual(ONLINE_VOICE_ADMIN_INTERFACE_NAME))
    {
        return (class UObject *)(void *)&this->VoiceAdminImpl;
    }
#endif

    return FOnlineSubsystemImpl::GetNamedInterface(InterfaceName);
}

IOnlineTurnBasedPtr FOnlineSubsystemEOS::GetTurnBasedInterface() const
{
    return nullptr;
}

IOnlineTournamentPtr FOnlineSubsystemEOS::GetTournamentInterface() const
{
    return nullptr;
}

bool FOnlineSubsystemEOS::Init()
{
    EOS_SCOPE_CYCLE_COUNTER(STAT_EOSOnlineSubsystemInit);

    this->bConfigCanBeSwitched = false;

    // .. please load Steam, please! :(
    this->Config->TryLoadDependentModules();

    // Initialize the platform.

    FString CacheDirectoryStr = Module->RuntimePlatform->GetCacheDirectory();

    auto EncryptionKey = StringCast<ANSICHAR>(*this->Config->GetEncryptionKey());
    auto CacheDirectory = StringCast<ANSICHAR>(*CacheDirectoryStr);
    auto ProductId = StringCast<ANSICHAR>(*this->Config->GetProductId());
    auto SandboxId = StringCast<ANSICHAR>(*this->Config->GetSandboxId());
    auto DeploymentId = StringCast<ANSICHAR>(*this->Config->GetDeploymentId());
    auto ClientId = StringCast<ANSICHAR>(*this->Config->GetClientId());
    auto ClientSecret = StringCast<ANSICHAR>(*this->Config->GetClientSecret());
    auto DedicatedServerClientId = StringCast<ANSICHAR>(*this->Config->GetDedicatedServerClientId());
    auto DedicatedServerClientSecret = StringCast<ANSICHAR>(*this->Config->GetDedicatedServerClientSecret());

    UE_LOG(
        LogEOS,
        Verbose,
        TEXT("Initializing EOS online subsystem: InstanceName=%s, bIsServer=%s, bIsDedicated=%s, bIsTrueDedicated=%s"),
        *this->InstanceName.ToString(),
        this->IsServer() ? TEXT("true") : TEXT("false"),
        this->IsDedicated() ? TEXT("true") : TEXT("false"),
        this->IsTrueDedicatedServer() ? TEXT("true") : TEXT("false"));

    EOS_Platform_Options PlatformOptions = {};
    PlatformOptions.ApiVersion = EOS_PLATFORM_OPTIONS_API_LATEST;
    PlatformOptions.bIsServer = this->IsTrueDedicatedServer() ? EOS_TRUE : EOS_FALSE;
    PlatformOptions.EncryptionKey = EncryptionKey.Get();
    if (CacheDirectoryStr.IsEmpty())
    {
        PlatformOptions.CacheDirectory = nullptr;
    }
    else
    {
        PlatformOptions.CacheDirectory = CacheDirectory.Get();
    }
    PlatformOptions.OverrideCountryCode = nullptr;
    PlatformOptions.OverrideLocaleCode = nullptr;
#if EOS_HAS_AUTHENTICATION
    if (!this->Config->GetCrossPlatformAccountProvider().IsEqual(EPIC_GAMES_ACCOUNT_ID))
    {
        // Only enable if we can ever authenticate with Epic Games, otherwise it'll never appear anyway.
        PlatformOptions.Flags = EOS_PF_DISABLE_OVERLAY;
    }
#if WITH_EDITOR
    else if (!FString(FCommandLine::Get()).ToUpper().Contains(TEXT("-GAME")))
    {
        // Disable the overlay in editor builds, if we're not running as a standalone process.
        PlatformOptions.Flags = EOS_PF_LOADING_IN_EDITOR;
    }
#endif // #if WITH_EDITOR
    else
#endif // #if EOS_HAS_AUTHENTICATION
    {
        // Allow the default behaviour of loading the overlay.
        PlatformOptions.Flags = 0;
    }
    PlatformOptions.ProductId = ProductId.Get();
    PlatformOptions.SandboxId = SandboxId.Get();
    PlatformOptions.DeploymentId = DeploymentId.Get();
    if (this->IsTrueDedicatedServer())
    {
        PlatformOptions.ClientCredentials.ClientId = DedicatedServerClientId.Get();
        PlatformOptions.ClientCredentials.ClientSecret = DedicatedServerClientSecret.Get();
    }
    else
    {
        PlatformOptions.ClientCredentials.ClientId = ClientId.Get();
        PlatformOptions.ClientCredentials.ClientSecret = ClientSecret.Get();
    }
#if EOS_VERSION_AT_LEAST(1, 13, 0)
    PlatformOptions.RTCOptions = this->RuntimePlatform->GetRTCOptions();
#endif

    this->PlatformHandle = EOS_Platform_Create(&PlatformOptions);
    if (this->PlatformHandle == nullptr)
    {
        UE_LOG(LogEOS, Error, TEXT("Unable to initialize EOS platform."));
        if (FParse::Param(FCommandLine::Get(), TEXT("requireeos")))
        {
            checkf(
                this->PlatformHandle != nullptr,
                TEXT("Unable to initialize EOS platform and you have -REQUIREEOS on the command line."));
        }
        return false;
    }

    UE_LOG(LogEOS, Verbose, TEXT("EOS platform %p has been created."), this->PlatformHandle);

#if EOS_VERSION_AT_LEAST(1, 15, 0)
    // Set the initial application and network status from the runtime platform.
    Module->RuntimePlatform->SetLifecycleForNewPlatformInstance(this->PlatformHandle);
#endif

#if EOS_VERSION_AT_LEAST(1, 12, 0) && (!defined(UE_SERVER) || !UE_SERVER)
    this->EOSAntiCheatClient = EOS_Platform_GetAntiCheatClientInterface(this->PlatformHandle);
#endif

#if !WITH_EDITOR
    if (this->Config->GetRequireEpicGamesLauncher())
    {
        EOS_EResult RestartResult = EOS_Platform_CheckForLauncherAndRestart(this->PlatformHandle);
        if (RestartResult == EOS_EResult::EOS_Success)
        {
            UE_LOG(
                LogEOS,
                Warning,
                TEXT("Game is restarting because it was not launched through the Epic Games Launcher."));
            FGenericPlatformMisc::RequestExit(false);
            return false;
        }
        else if (RestartResult == EOS_EResult::EOS_NoChange)
        {
            UE_LOG(LogEOS, Verbose, TEXT("Game was already launched through the Epic Games Launcher."));
        }
        else if (RestartResult == EOS_EResult::EOS_UnexpectedError)
        {
            UE_LOG(
                LogEOS,
                Warning,
                TEXT("Game is exiting because EOS_Platform_CheckForLauncherAndRestart failed to return an expected "
                     "result."));
            FMessageDialog::Open(
                EAppMsgType::Ok,
                LOCTEXT(
                    "OnlineSubsystemEOS_EGSRestartFailed",
                    "Unable to detect if the game was launched through the Epic Games Launcher. Please reinstall the "
                    "application."));
            FGenericPlatformMisc::RequestExit(false);
            return false;
        }
    }
#endif

    if (this->IsTrueDedicatedServer())
    {
        // Initialize the "act as" platform, which is used by trusted dedicated servers to operate on behalf of users.
        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("Initializing \"act as\" platform for EOS online subsystem: InstanceName=%s, bIsServer=%s, "
                 "bIsDedicated=%s, "
                 "bIsTrueDedicated=%s"),
            *this->InstanceName.ToString(),
            this->IsServer() ? TEXT("true") : TEXT("false"),
            this->IsDedicated() ? TEXT("true") : TEXT("false"),
            this->IsTrueDedicatedServer() ? TEXT("true") : TEXT("false"));

        auto ActAsClientId = StringCast<ANSICHAR>(*this->Config->GetClientId());
        auto ActAsClientSecret = StringCast<ANSICHAR>(*this->Config->GetClientSecret());

        EOS_Platform_Options ActAsPlatformOptions = {};
        ActAsPlatformOptions.ApiVersion = EOS_PLATFORM_OPTIONS_API_LATEST;
        ActAsPlatformOptions.bIsServer = EOS_TRUE;
        ActAsPlatformOptions.EncryptionKey = EncryptionKey.Get();
        if (CacheDirectoryStr.IsEmpty())
        {
            ActAsPlatformOptions.CacheDirectory = nullptr;
        }
        else
        {
            ActAsPlatformOptions.CacheDirectory = CacheDirectory.Get();
        }
        ActAsPlatformOptions.OverrideCountryCode = nullptr;
        ActAsPlatformOptions.OverrideLocaleCode = nullptr;
#if WITH_EDITOR
        // Disable the overlay in editor builds, if we're not running as a standalone process.
        if (FString(FCommandLine::Get()).ToUpper().Contains(TEXT("-GAME")))
        {
            ActAsPlatformOptions.Flags = 0;
        }
        else
        {
            ActAsPlatformOptions.Flags = EOS_PF_LOADING_IN_EDITOR;
        }
#else
        ActAsPlatformOptions.Flags = 0;
#endif
        ActAsPlatformOptions.ProductId = ProductId.Get();
        ActAsPlatformOptions.SandboxId = SandboxId.Get();
        ActAsPlatformOptions.DeploymentId = DeploymentId.Get();
        ActAsPlatformOptions.ClientCredentials.ClientId = ActAsClientId.Get();
        ActAsPlatformOptions.ClientCredentials.ClientSecret = ActAsClientSecret.Get();

        this->ActAsPlatformHandle = EOS_Platform_Create(&ActAsPlatformOptions);
        if (this->ActAsPlatformHandle == nullptr)
        {
            UE_LOG(LogEOS, Error, TEXT("Unable to initialize \"act as\" EOS platform."));
            if (FParse::Param(FCommandLine::Get(), TEXT("requireeos")))
            {
                checkf(
                    this->ActAsPlatformHandle != nullptr,
                    TEXT("Unable to initialize \"act as\" EOS platform and you have -REQUIREEOS on the command line."));
            }
            return false;
        }

        UE_LOG(LogEOS, Verbose, TEXT("EOS platform %p (act as) has been created."), this->ActAsPlatformHandle);
    }

    // Initalize the interfaces.

#if EOS_VERSION_AT_LEAST(1, 12, 0)
    if (this->Config->GetEnableAntiCheat())
    {
#if WITH_EDITOR
        this->AntiCheat = MakeShared<FEditorAntiCheat>();
#else
#if WITH_SERVER_CODE
        if (this->IsTrueDedicatedServer())
        {
            this->AntiCheat = MakeShared<FEOSDedicatedServerAntiCheat>(this->PlatformHandle);
        }
        else
        {
#endif // #if WITH_SERVER_CODE
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
            this->AntiCheat = MakeShared<FEOSGameAntiCheat>(this->PlatformHandle);
#else
        // Non-desktop platforms can not host Anti-Cheat sessions, so this ensures
        // they're still able to start listen servers even when Anti-Cheat is enabled.
        // Note that under this scenario, if a non-desktop platform hosts a game session,
        // then *none* of the clients that connect will have Anti-Cheat verified (so
        // desktop players will be able to cheat, etc.) If you need to have cross-platform
        // play with Anti-Cheat enforced for all desktop players, you'll currently need to
        // use dedicated servers.
        this->AntiCheat = MakeShared<FNullAntiCheat>();
#endif
#if WITH_SERVER_CODE
        }
#endif // #if WITH_SERVER_CODE
#endif // #if WITH_EDITOR
    }
    else
    {
        this->AntiCheat = MakeShared<FNullAntiCheat>();
    }
    if (!this->AntiCheat->Init())
    {
        UE_LOG(LogEOS, Error, TEXT("Anti-Cheat failed to initialize!"));
        FPlatformMisc::RequestExit(true);
        return false;
    }
#else
    this->AntiCheat = MakeShared<FNullAntiCheat>();
#endif

    this->UserFactory = MakeShared<FEOSUserFactory, ESPMode::ThreadSafe>(
        this->GetInstanceName(),
        this->PlatformHandle,
        this->RuntimePlatform.ToSharedRef());

#if EOS_HAS_AUTHENTICATION
    if (!this->IsTrueDedicatedServer())
    {
        this->SubsystemEAS =
            MakeShared<FOnlineSubsystemRedpointEAS, ESPMode::ThreadSafe>(this->InstanceName, this->AsShared());
        verifyf(this->SubsystemEAS->Init(), TEXT("EAS Subsystem did not Init successfully! This should not happen."));
    }
#endif

    TSharedPtr<FOnlineIdentityInterfaceEAS, ESPMode::ThreadSafe> IdentityEAS = nullptr;
#if EOS_HAS_AUTHENTICATION
    if (!this->IsTrueDedicatedServer())
    {
        IdentityEAS = StaticCastSharedPtr<FOnlineIdentityInterfaceEAS>(this->SubsystemEAS->GetIdentityInterface());
    }
#endif
    auto IdentityEOS = MakeShared<FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe>(
        this->AsShared(),
        this->PlatformHandle,
        this->GetInstanceName().ToString(),
        this->IsTrueDedicatedServer(),
        IdentityEAS,
        this->RuntimePlatform.ToSharedRef(),
        this->Config.ToSharedRef(),
        this->UserFactory.ToSharedRef(),
        this->AsShared());
    IdentityEOS->RegisterEvents();
    this->IdentityImpl = IdentityEOS;

    this->UserImpl = MakeShared<FOnlineUserInterfaceEOS, ESPMode::ThreadSafe>(
        this->PlatformHandle,
#if EOS_HAS_AUTHENTICATION
        this->IsTrueDedicatedServer() ? nullptr : this->SubsystemEAS,
#else
        nullptr,
#endif
        IdentityEOS,
        this->UserFactory.ToSharedRef());

#if defined(EOS_VOICE_CHAT_SUPPORTED)
    this->VoiceManager = MakeShared<FEOSVoiceManager>(
        this->PlatformHandle,
        this->Config.ToSharedRef(),
        this->IdentityImpl.ToSharedRef());
    this->IdentityImpl->VoiceManager = this->VoiceManager;
    EOS_HRTCAdmin EOSRTCAdmin = EOS_Platform_GetRTCAdminInterface(this->PlatformHandle);
    if (EOSRTCAdmin != nullptr)
    {
        this->VoiceAdminImpl = MakeShared<FOnlineVoiceAdminInterfaceEOS, ESPMode::ThreadSafe>(EOSRTCAdmin);
    }
    this->VoiceImpl = MakeShared<FOnlineVoiceInterfaceEOS, ESPMode::ThreadSafe>(
        this->VoiceManager.ToSharedRef(),
        this->IdentityImpl.ToSharedRef());
#endif

    this->SocketSubsystem = MakeShared<FSocketSubsystemEOSFull>(
        this->AsShared(),
        this->PlatformHandle,
        this->Config.ToSharedRef(),
        this->RegulatedTicker);

    // @todo: Handle errors more gracefully.
    FString SocketSubsystemError;
    this->SocketSubsystem->Init(SocketSubsystemError);

    this->MessagingHub =
        MakeShared<FEOSMessagingHub>(this->IdentityImpl.ToSharedRef(), this->SocketSubsystem.ToSharedRef());
    this->MessagingHub->RegisterEvents();

#if EOS_HAS_ORCHESTRATORS
    if (this->IsTrueDedicatedServer())
    {
        TSharedPtr<FAgonesServerOrchestrator> AgonesOrchestrator = MakeShared<FAgonesServerOrchestrator>();
        if (AgonesOrchestrator->IsEnabled())
        {
            this->ServerOrchestrator = AgonesOrchestrator;
            this->ServerOrchestrator->Init();
        }
    }
#endif // #if EOS_HAS_ORCHESTRATORS

#if EOS_HAS_AUTHENTICATION
    TSharedPtr<FOnlinePartySystemEOS, ESPMode::ThreadSafe> EOSPartyImpl;
    if (!this->IsTrueDedicatedServer())
    {
        this->UserCloudImpl = MakeShared<FOnlineUserCloudInterfaceEOS, ESPMode::ThreadSafe>(
            this->ActAsPlatformHandle != nullptr ? this->ActAsPlatformHandle : this->PlatformHandle);

        this->FriendsImpl = MakeShared<FOnlineFriendsInterfaceSynthetic, ESPMode::ThreadSafe>(
            this->GetInstanceName(),
            this->PlatformHandle,
            this->IdentityImpl.ToSharedRef(),
            this->UserCloudImpl.ToSharedRef(),
            this->MessagingHub.ToSharedRef(),
            this->SubsystemEAS,
            this->RuntimePlatform.ToSharedRef(),
            this->Config.ToSharedRef(),
            this->UserFactory.ToSharedRef());
        this->FriendsImpl->RegisterEvents();

        this->AvatarImpl = MakeShared<FOnlineAvatarInterfaceSynthetic, ESPMode::ThreadSafe>(
            this->GetInstanceName(),
            this->Config.ToSharedRef(),
            this->IdentityImpl,
            this->FriendsImpl,
            this->UserImpl);

        this->PresenceImpl = MakeShared<FOnlinePresenceInterfaceSynthetic, ESPMode::ThreadSafe>(
            this->GetInstanceName(),
            this->IdentityImpl.ToSharedRef(),
            this->FriendsImpl.ToSharedRef(),
            this->SubsystemEAS,
            this->Config.ToSharedRef());
        this->PresenceImpl->RegisterEvents();

        EOSPartyImpl = MakeShared<FOnlinePartySystemEOS, ESPMode::ThreadSafe>(
            this->PlatformHandle,
            this->Config.ToSharedRef(),
            this->IdentityImpl.ToSharedRef(),
            this->FriendsImpl.ToSharedRef(),
            this->UserFactory.ToSharedRef()
#if defined(EOS_VOICE_CHAT_SUPPORTED)
                ,
            this->VoiceManager.ToSharedRef()
#endif // #if defined(EOS_VOICE_CHAT_SUPPORTED)
        );
        EOSPartyImpl->RegisterEvents();
        this->PartyImpl = EOSPartyImpl;

        this->LobbyImpl = MakeShared<FOnlineLobbyInterfaceEOS, ESPMode::ThreadSafe>(
            this->PlatformHandle
#if defined(EOS_VOICE_CHAT_SUPPORTED)
            ,
            this->VoiceManager.ToSharedRef()
#endif // #if defined(EOS_VOICE_CHAT_SUPPORTED)
        );
        this->LobbyImpl->RegisterEvents();
    }
#endif // #if EOS_HAS_AUTHENTICATION

    auto EOSSessionImpl = MakeShared<FOnlineSessionInterfaceEOS, ESPMode::ThreadSafe>(
        this->PlatformHandle,
        IdentityEOS,
#if EOS_HAS_AUTHENTICATION
        this->IsTrueDedicatedServer() ? nullptr : FriendsImpl,
#else
        nullptr,
#endif
#if EOS_HAS_ORCHESTRATORS
        this->ServerOrchestrator,
#endif // #if EOS_HAS_ORCHESTRATORS
        this->Config.ToSharedRef());
    EOSSessionImpl->RegisterEvents();
    this->SessionImpl = EOSSessionImpl;
#if EOS_VERSION_AT_LEAST(1, 8, 0)
    this->TitleFileImpl =
        MakeShared<FOnlineTitleFileInterfaceEOS, ESPMode::ThreadSafe>(this->PlatformHandle, IdentityEOS);
#else
    this->TitleFileImpl.Reset();
#endif
    this->StatsImpl = MakeShared<FOnlineStatsInterfaceEOS, ESPMode::ThreadSafe>(
        this->PlatformHandle,
        this->Config.ToSharedRef(),
        this->GetInstanceName());
    auto EOSAchievementsImpl =
        MakeShared<FOnlineAchievementsInterfaceEOS, ESPMode::ThreadSafe>(this->PlatformHandle, this->StatsImpl);
    EOSAchievementsImpl->RegisterEvents();
    this->AchievementsImpl = EOSAchievementsImpl;
    this->LeaderboardsImpl = MakeShared<FOnlineLeaderboardsInterfaceEOS, ESPMode::ThreadSafe>(
        this->PlatformHandle,
        this->StatsImpl,
        this->IdentityImpl
#if EOS_HAS_AUTHENTICATION
        ,
        this->IsTrueDedicatedServer() ? nullptr : this->FriendsImpl
#endif // #if EOS_HAS_AUTHENTICATION
    );
    this->EntitlementsImpl =
        MakeShared<FOnlineEntitlementsInterfaceSynthetic, ESPMode::ThreadSafe>(this->IdentityImpl.ToSharedRef());
    this->StoreV2Impl =
        MakeShared<FOnlineStoreInterfaceV2Synthetic, ESPMode::ThreadSafe>(this->IdentityImpl.ToSharedRef());
    this->PurchaseImpl =
        MakeShared<FOnlinePurchaseInterfaceSynthetic, ESPMode::ThreadSafe>(this->IdentityImpl.ToSharedRef());

#if EOS_HAS_AUTHENTICATION
    if (!this->IsTrueDedicatedServer())
    {
        this->SyntheticPartySessionManager = MakeShared<FSyntheticPartySessionManager>(
            this->GetInstanceName(),
            EOSPartyImpl,
            EOSSessionImpl,
            this->IdentityImpl,
            this->RuntimePlatform.ToSharedRef(),
            this->Config.ToSharedRef());
        this->SyntheticPartySessionManager->RegisterEvents();
        EOSSessionImpl->SetSyntheticPartySessionManager(this->SyntheticPartySessionManager);
        EOSPartyImpl->SetSyntheticPartySessionManager(this->SyntheticPartySessionManager);

        this->ExternalUIImpl = MakeShared<FOnlineExternalUIInterfaceEOS, ESPMode::ThreadSafe>(
            this->PlatformHandle,
            this->IdentityImpl.ToSharedRef(),
            this->SessionImpl.ToSharedRef(),
            this->SyntheticPartySessionManager);
        this->ExternalUIImpl->RegisterEvents();
    }
#endif // #if EOS_HAS_AUTHENTICATION

    // If we are a dedicated server, call AutoLogin on the identity interface so that we have a valid
    // user ID in LocalUserNum 0 for all dedicated server operations.
    if (this->IsTrueDedicatedServer())
    {
        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("Automatically calling AutoLogin for LocalUserNum 0 because the game is running as a dedicated "
                 "server"));
        this->IdentityImpl->AutoLogin(0);
    }

    // We want to shutdown ahead of other subsystems, because we will have hooks and events registered
    // to them. If those events are still registered when other subsystems try to shutdown, we'll get
    // check() fails because the shared pointers will still have references.
    this->OnPreExitHandle = FCoreDelegates::OnPreExit.AddLambda([WeakThis = GetWeakThis(this)]() {
        if (auto This = PinWeakThis(WeakThis))
        {
            This->Shutdown();
        }
    });

#if WITH_EDITOR && defined(UE_4_27_OR_LATER)
    // We need to listen for FWorldDelegates::OnStartGameInstance. When initializing a play-in-editor window,
    // the UpdateUniqueNetIdForPlayerController logic from post-login runs before the UWorld* or ULocalPlayer*
    // is created, so we can't set the unique net ID that way. However, Unreal doesn't assign the unique net ID
    // it got from logging in to the new ULocalPlayer*, so it'll be stuck with an invalid net ID.
    //
    // We listen to FWorldDelegates::OnStartGameInstance so we can fix up any ULocalPlayer* instances to have
    // the unique net ID that was signed in from "login before PIE".
    FWorldDelegates::OnStartGameInstance.AddLambda([WeakThis = GetWeakThis(this)](UGameInstance *NewGameInstance) {
        if (auto This = PinWeakThis(WeakThis))
        {
            TSoftObjectPtr<UWorld> OurWorld = FWorldResolution::GetWorld(This->GetInstanceName(), true);
            checkf(
                IsValid(NewGameInstance->GetWorld()),
                TEXT("World must be valid for new game instance in OnStartGameInstance handler."));
            if (!OurWorld.IsValid() || OurWorld.Get() != NewGameInstance->GetWorld())
            {
                return;
            }

            for (ULocalPlayer *Player : NewGameInstance->GetLocalPlayers())
            {
                if (This->IdentityImpl->GetLoginStatus(Player->GetControllerId()) == ELoginStatus::LoggedIn)
                {
                    auto ThisUniqueId = This->IdentityImpl->GetUniquePlayerId(Player->GetControllerId());
#if defined(UE_5_0_OR_LATER)
                    Player->SetCachedUniqueNetId(FUniqueNetIdRepl(ThisUniqueId));
#else
                    Player->SetCachedUniqueNetId(ThisUniqueId);
#endif // #if defined(UE_5_0_OR_LATER)
                    UE_LOG(
                        LogEOS,
                        Verbose,
                        TEXT("Updated unique net ID of ULocalPlayer for local player num %d (via "
                             "OnStartGameInstance): %s"),
                        Player->GetControllerId(),
                        *Player->GetCachedUniqueNetId()->ToString());
                }
            }
        }
    });
#endif

    this->TickerHandle = this->RegulatedTicker->AddTicker(
        FTickerDelegate::CreateThreadSafeSP(this, &FOnlineSubsystemEOS::RegulatedTick));

    return true;
}

bool FOnlineSubsystemEOS::Tick(float DeltaTime)
{
    // We don't need an unregulated tick.
    return false;
}

bool FOnlineSubsystemEOS::RegulatedTick(float DeltaTime)
{
    EOS_TRACE_COUNTER_SET(CTR_EOSNetP2PReceivedLoopIters, 0);
    EOS_TRACE_COUNTER_SET(CTR_EOSNetP2PReceivedPackets, 0);
    EOS_TRACE_COUNTER_SET(CTR_EOSNetP2PReceivedBytes, 0);
    EOS_TRACE_COUNTER_SET(CTR_EOSNetP2PSentPackets, 0);
    EOS_TRACE_COUNTER_SET(CTR_EOSNetP2PSentBytes, 0);

    EOS_SCOPE_CYCLE_COUNTER(STAT_EOSOnlineSubsystemTick);

    if (this->PlatformHandle != nullptr)
    {
        EOS_Platform_Tick(this->PlatformHandle);
#if EOS_VERSION_AT_LEAST(1, 12, 0) && (!defined(UE_SERVER) || !UE_SERVER) &&                                           \
    (PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX)
        char PollMessage[256];
        EOS_EAntiCheatClientViolationType ViolationType;
        EOS_AntiCheatClient_PollStatusOptions Opts = {};
        Opts.ApiVersion = EOS_ANTICHEATCLIENT_POLLSTATUS_API_LATEST;
        Opts.OutMessageLength = 256;
        EOS_EResult PollResult =
            EOS_AntiCheatClient_PollStatus(this->EOSAntiCheatClient, &Opts, &ViolationType, &PollMessage[0]);
        if (PollResult == EOS_EResult::EOS_Success)
        {
            // There is an Anti-Cheat violation.
            // @todo: We need to propagate this to an event the developer can listen on, but there aren't any good
            // events in the standard IOnlineSubsystem API. We probably need to add a custom interface for this.
            FString ViolationTypeStr = TEXT("Unknown");
            if (ViolationType == EOS_EAntiCheatClientViolationType::EOS_ACCVT_Invalid)
            {
                ViolationTypeStr = TEXT("Invalid");
            }
            else if (ViolationType == EOS_EAntiCheatClientViolationType::EOS_ACCVT_IntegrityCatalogNotFound)
            {
                ViolationTypeStr = TEXT("IntegrityCatalogNotFound");
            }
            else if (ViolationType == EOS_EAntiCheatClientViolationType::EOS_ACCVT_IntegrityCatalogError)
            {
                ViolationTypeStr = TEXT("IntegrityCatalogError");
            }
            else if (ViolationType == EOS_EAntiCheatClientViolationType::EOS_ACCVT_IntegrityCatalogCertificateRevoked)
            {
                ViolationTypeStr = TEXT("IntegrityCatalogCertificateRevoked");
            }
            else if (
                ViolationType == EOS_EAntiCheatClientViolationType::EOS_ACCVT_IntegrityCatalogMissingMainExecutable)
            {
                ViolationTypeStr = TEXT("IntegrityCatalogMissingMainExecutable");
            }
            else if (ViolationType == EOS_EAntiCheatClientViolationType::EOS_ACCVT_GameFileMismatch)
            {
                ViolationTypeStr = TEXT("GameFileMismatch");
            }
            else if (ViolationType == EOS_EAntiCheatClientViolationType::EOS_ACCVT_RequiredGameFileNotFound)
            {
                ViolationTypeStr = TEXT("RequiredGameFileNotFound");
            }
            else if (ViolationType == EOS_EAntiCheatClientViolationType::EOS_ACCVT_UnknownGameFileForbidden)
            {
                ViolationTypeStr = TEXT("UnknownGameFileForbidden");
            }
            else if (ViolationType == EOS_EAntiCheatClientViolationType::EOS_ACCVT_SystemFileUntrusted)
            {
                ViolationTypeStr = TEXT("SystemFileUntrusted");
            }
            else if (ViolationType == EOS_EAntiCheatClientViolationType::EOS_ACCVT_ForbiddenModuleLoaded)
            {
                ViolationTypeStr = TEXT("ForbiddenModuleLoaded");
            }
            else if (ViolationType == EOS_EAntiCheatClientViolationType::EOS_ACCVT_CorruptedMemory)
            {
                ViolationTypeStr = TEXT("CorruptedMemory");
            }
            else if (ViolationType == EOS_EAntiCheatClientViolationType::EOS_ACCVT_ForbiddenToolDetected)
            {
                ViolationTypeStr = TEXT("ForbiddenToolDetected");
            }
            else if (ViolationType == EOS_EAntiCheatClientViolationType::EOS_ACCVT_InternalAntiCheatViolation)
            {
                ViolationTypeStr = TEXT("InternalAntiCheatViolation");
            }
            else if (ViolationType == EOS_EAntiCheatClientViolationType::EOS_ACCVT_CorruptedNetworkMessageFlow)
            {
                ViolationTypeStr = TEXT("CorruptedNetworkMessageFlow");
            }
            else if (ViolationType == EOS_EAntiCheatClientViolationType::EOS_ACCVT_VirtualMachineNotAllowed)
            {
                ViolationTypeStr = TEXT("VirtualMachineNotAllowed");
            }
            else if (ViolationType == EOS_EAntiCheatClientViolationType::EOS_ACCVT_ForbiddenSystemConfiguration)
            {
                ViolationTypeStr = TEXT("ForbiddenSystemConfiguration");
            }
            UE_LOG(
                LogEOS,
                Error,
                TEXT("Anti-Cheat Violation: Type: '%s'. Message: '%s'. You will not be able to connect to protected "
                     "servers."),
                *ViolationTypeStr,
                ANSI_TO_TCHAR(PollMessage));
        }

        Module->SubsystemInstancesTickedFlag.Add(this->InstanceName);
#endif
    }
    if (this->ActAsPlatformHandle != nullptr)
    {
        EOS_Platform_Tick(this->ActAsPlatformHandle);
    }

#if EOS_HAS_AUTHENTICATION
    if (this->PartyImpl.IsValid())
    {
        // Used for event deduplication.
        StaticCastSharedPtr<FOnlinePartySystemEOS>(this->PartyImpl)->Tick();
    }
#endif // #if EOS_HAS_AUTHENTICATION

    return FOnlineSubsystemImpl::Tick(DeltaTime);
}

bool FOnlineSubsystemEOS::Exec(UWorld *InWorld, const TCHAR *Cmd, FOutputDevice &Ar)
{
    bool bWasHandled = false;

    if (FParse::Command(&Cmd, TEXT("IDENTITY")))
    {
        bWasHandled = this->IdentityImpl->Exec(InWorld, Cmd, Ar);
    }

    if (FParse::Command(&Cmd, TEXT("ANTICHEAT")))
    {
        bWasHandled = this->AntiCheat->Exec(InWorld, Cmd, Ar);
    }

    if (!bWasHandled)
    {
        bWasHandled = FOnlineSubsystemImpl::Exec(InWorld, Cmd, Ar);
    }

    return bWasHandled;
}

template <typename T, ESPMode Mode> void DestructInterface(TSharedPtr<T, Mode> &Ref, const TCHAR *Name)
{
    if (Ref.IsValid())
    {
        ensureMsgf(
            Ref.IsUnique(),
            TEXT(
                "Interface is not unique during shutdown of EOS Online Subsystem: %s. "
                "This indicates you have a TSharedPtr<> or IOnline... in your code that is holding a reference open to "
                "the interface longer than the lifetime of the online subsystem. You should use TWeakPtr<> "
                "to hold references to interfaces in class fields to prevent lifetime issues"),
            Name);
        Ref = nullptr;
    }
}

class FCleanShutdown
{
private:
    TSharedPtr<FOnlineSubsystemEOS, ESPMode::ThreadSafe> EOS;
    TSharedPtr<FOnlineSessionInterfaceEOS, ESPMode::ThreadSafe> SessionImpl;

public:
    FCleanShutdown(
        TSharedPtr<FOnlineSubsystemEOS, ESPMode::ThreadSafe> InEOS,
        TSharedPtr<FOnlineSessionInterfaceEOS, ESPMode::ThreadSafe> InSession)
        : EOS(MoveTemp(InEOS))
        , SessionImpl(MoveTemp(InSession)){};

    void Shutdown();
};

void FCleanShutdown::Shutdown()
{
    // Set up a ticker so we can continue ticking (since the engine will no longer call the EOS's subsystems Tick
    // event).
    FUTickerDelegateHandle TickerHandle = FUTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateLambda([EOS = this->EOS](float DeltaSeconds) {
            EOS->Tick(DeltaSeconds);
            return true;
        }),
        0.0f);

    TArray<FName> SessionNames;
    for (const auto &Session : this->SessionImpl->Sessions)
    {
        SessionNames.Add(Session.SessionName);
    }
    this->SessionImpl.Reset();

    FMultiOperation<FName, bool>::Run(
        SessionNames,
        [this](const FName &SessionName, const std::function<void(bool)> &OnDone) {
            UE_LOG(
                LogEOS,
                Verbose,
                TEXT(
                    "Automatically destroying up session %s for you, since you're running in the editor. Sessions will "
                    "not be automatically destroyed if you're running in standalone."),
                *SessionName.ToString());
            auto SessionNameAnsi = EOSString_SessionModification_SessionName::ToAnsiString(SessionName.ToString());
            EOS_Sessions_DestroySessionOptions Opts = {};
            Opts.ApiVersion = EOS_SESSIONS_ENDSESSION_API_LATEST;
            Opts.SessionName = SessionNameAnsi.Ptr.Get();
            EOSRunOperation<EOS_HSessions, EOS_Sessions_DestroySessionOptions, EOS_Sessions_DestroySessionCallbackInfo>(
                EOS_Platform_GetSessionsInterface(this->EOS->PlatformHandle),
                &Opts,
                EOS_Sessions_DestroySession,
                [SessionName, OnDone](const EOS_Sessions_DestroySessionCallbackInfo *Info) {
                    if (Info->ResultCode == EOS_EResult::EOS_Success)
                    {
                        UE_LOG(
                            LogEOS,
                            Verbose,
                            TEXT("Session %s was automatically destroyed successfully."),
                            *SessionName.ToString());
                    }
                    else
                    {
                        UE_LOG(
                            LogEOS,
                            Error,
                            TEXT("Session %s could not be automatically destroyed. It may continue to exist on the "
                                 "EOS backend for a few minutes."),
                            *SessionName.ToString());
                    }
                    OnDone(true);
                });
            return true;
        },
        [this, TickerHandle](const TArray<bool> &Results) {
            // Do the real shutdown. We have to be super careful with pointers here for uniqueness checks!
            FUTicker::GetCoreTicker().AddTicker(
                FTickerDelegate::CreateLambda([this, TickerHandle](float DeltaSeconds) {
                    FUTicker::GetCoreTicker().RemoveTicker(TickerHandle);
                    FName ShutdownName =
                        FName(FString::Printf(TEXT("%s_ShuttingDown"), *this->EOS->InstanceName.ToString()));
                    this->EOS->RealShutdown();
                    this->EOS->Module->SubsystemInstances.Remove(ShutdownName);
                    this->EOS->Module->SubsystemInstancesTickedFlag.Remove(ShutdownName);
                    this->EOS.Reset();
                    delete this;
                    return false;
                }),
                0.0f);
        });
}

bool FOnlineSubsystemEOS::Shutdown()
{
#if WITH_EDITOR
    // bIsPerformingFullShutdown is true in *most* cases when the editor is exiting (from pressing X in the top-right
    // corner of the window). It's not perfect, but should eliminate some cases of delayed cleanup running on shutdown.
    bool bIsPerformingFullShutdown = IsEngineExitRequested();
    // WITH_EDITOR is true if you're playing "Standalone Game" from the editor, but bIsEditor will be false. We don't
    // want to do delayed cleanup for standalone games since the process will exit soon after calling ::Shutdown().
    bool bIsEditor = GIsEditor;
    if (this->PlatformHandle && this->SessionImpl.IsValid() && bIsEditor && !bIsPerformingFullShutdown)
    {
        // When we're running in the editor, try to destroy all sessions that we're a part of before we do the real
        // shutdown logic.
        FName ShutdownName = FName(FString::Printf(TEXT("%s_ShuttingDown"), *this->InstanceName.ToString()));
        if (Module->SubsystemInstances.Contains(ShutdownName) ||
            !Module->SubsystemInstances.Contains(this->InstanceName))
        {
            return true;
        }
        Module->SubsystemInstances.Add(ShutdownName, this);
        Module->SubsystemInstances.Remove(this->InstanceName);
        Module->SubsystemInstancesTickedFlag.Remove(this->InstanceName);
        this->bDidEarlyDestroyForEditor = true;
        auto Impl = new FCleanShutdown(this->AsShared(), this->SessionImpl);
        Impl->Shutdown();
        return true;
    }
    else
    {
        // EOS didn't initialize the editor, so we don't need to clean up sessions.
        this->RealShutdown();
        return true;
    }
#else
    this->RealShutdown();
    return true;
#endif
}

void FOnlineSubsystemEOS::RealShutdown()
{
    EOS_SCOPE_CYCLE_COUNTER(STAT_EOSOnlineSubsystemShutdown);

    FCoreDelegates::OnPreExit.Remove(this->OnPreExitHandle);

    if (this->AntiCheat.IsValid())
    {
        this->AntiCheat->Shutdown();
    }
    DestructInterface(this->AntiCheat, TEXT("AntiCheat"));
#if EOS_HAS_AUTHENTICATION
    DestructInterface(this->ExternalUIImpl, TEXT("IOnlineExternalUI"));
    DestructInterface(this->SyntheticPartySessionManager, TEXT("SyntheticPartySessionManager"));
#endif // #if EOS_HAS_AUTHENTICATION
    DestructInterface(this->PurchaseImpl, TEXT("IOnlinePurchase"));
    DestructInterface(this->StoreV2Impl, TEXT("IOnlineStoreV2"));
    DestructInterface(this->EntitlementsImpl, TEXT("IOnlineEntitlements"));
    DestructInterface(this->LeaderboardsImpl, TEXT("IOnlineLeaderboards"));
    DestructInterface(this->AchievementsImpl, TEXT("IOnlineAchievements"));
    DestructInterface(this->StatsImpl, TEXT("IOnlineStats"));
    DestructInterface(this->TitleFileImpl, TEXT("IOnlineTitleFile"));
    DestructInterface(this->SessionImpl, TEXT("IOnlineSession"));
#if EOS_HAS_AUTHENTICATION
    DestructInterface(this->PartyImpl, TEXT("IOnlinePartySystem"));
    DestructInterface(this->LobbyImpl, TEXT("IOnlineLobby"));
    DestructInterface(this->PresenceImpl, TEXT("IOnlinePresence"));
    DestructInterface(this->AvatarImpl, TEXT("IOnlineAvatar"));
    DestructInterface(this->FriendsImpl, TEXT("IOnlineFriends"));
    DestructInterface(this->UserCloudImpl, TEXT("IOnlineUserCloud"));
#endif // #if EOS_HAS_AUTHENTICATION
#if EOS_HAS_ORCHESTRATORS
    DestructInterface(this->ServerOrchestrator, TEXT("ServerOrchestrator"));
#endif // #if EOS_HAS_ORCHESTRATORS
    DestructInterface(this->MessagingHub, TEXT("MessagingHub"));
    if (this->SocketSubsystem.IsValid())
    {
        this->SocketSubsystem->Shutdown();
    }
    DestructInterface(this->SocketSubsystem, TEXT("SocketSubsystem"));
#if defined(EOS_VOICE_CHAT_SUPPORTED)
    DestructInterface(this->VoiceImpl, TEXT("IOnlineVoice"));
    DestructInterface(this->VoiceAdminImpl, TEXT("IOnlineVoiceAdmin"));
    if (this->IdentityImpl.IsValid())
    {
        this->IdentityImpl->VoiceManager.Reset();
    }
    if (this->VoiceManager.IsValid())
    {
        this->VoiceManager->RemoveAllLocalUsers();
    }
    DestructInterface(this->VoiceManager, TEXT("(internal) VoiceManager"));
#endif // #if defined(EOS_VOICE_CHAT_SUPPORTED)
    DestructInterface(this->UserImpl, TEXT("IOnlineUser"));
    DestructInterface(this->IdentityImpl, TEXT("IOnlineIdentity"));
#if EOS_HAS_AUTHENTICATION
    if (this->SubsystemEAS.IsValid())
    {
        verifyf(this->SubsystemEAS->Shutdown(), TEXT("EAS Subsystem did not shutdown successfully!"));
    }
    DestructInterface(this->SubsystemEAS, TEXT("(internal) SubsystemEAS"));
#endif // #if EOS_HAS_AUTHENTICATION
    DestructInterface(this->UserFactory, TEXT("(internal) UserFactory"));

    // Shutdown the platform.
    if (this->PlatformHandle)
    {
#if EOS_VERSION_AT_LEAST(1, 12, 0) && (!defined(UE_SERVER) || !UE_SERVER)
        this->EOSAntiCheatClient = nullptr;
#endif

        EOS_Platform_Release(this->PlatformHandle);

        UE_LOG(LogEOS, Verbose, TEXT("EOS platform %p has been released."), this->PlatformHandle);

        this->PlatformHandle = nullptr;
    }
    if (this->ActAsPlatformHandle)
    {
        EOS_Platform_Release(this->ActAsPlatformHandle);

        UE_LOG(LogEOS, Verbose, TEXT("EOS platform %p (act as) has been released."), this->ActAsPlatformHandle);

        this->ActAsPlatformHandle = nullptr;
    }

    this->RegulatedTicker->RemoveTicker(this->TickerHandle);
}

FString FOnlineSubsystemEOS::GetAppId() const
{
    return this->Config->GetClientId();
}

FText FOnlineSubsystemEOS::GetOnlineServiceName(void) const
{
    return NSLOCTEXT("EOS", "EOSPlatformName", "Epic Online Services");
}

bool FOnlineSubsystemEOS::IsTrueDedicatedServer() const
{
    // Neither IsServer nor IsDedicated work correctly in play-in-editor. Both listen servers
    // and dedicated servers return true from IsServer, but *neither* return true from IsDedicated
    // (in fact it looks like IsDedicated just doesn't do the right thing at all for single process).
    //
    // So instead implement our own version here which does the detection correctly.

#if WITH_EDITOR
    // Running multiple worlds in the editor; we need to see if this world is specifically a dedicated server.
    FName WorldContextHandle =
        (this->InstanceName != NAME_None && this->InstanceName != DefaultInstanceName) ? this->InstanceName : NAME_None;
    if (WorldContextHandle.IsNone())
    {
        // The default OSS instance is equal to IsRunningDedicatedServer(), in case the editor
        // is being run with -server for multi-process.
        return IsRunningDedicatedServer();
    }
    if (GEngine == nullptr)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("GEngine is not available, but EOS requires it in order to detect if it is running as a dedicated "
                 "server inside the editor! Please report this error in the EOS Online Subsystem Discord!"));
        return false;
    }
    FWorldContext *WorldContext = GEngine->GetWorldContextFromHandle(WorldContextHandle);
    if (WorldContext == nullptr)
    {
        // World context is invalid. This will occur during unit tests.
        return false;
    }
    return WorldContext->RunAsDedicated;
#else
    // Just use IsRunningDedicatedServer, since our process is either a server or it's not.
    return IsRunningDedicatedServer();
#endif
}

#undef LOCTEXT_NAMESPACE

EOS_DISABLE_STRICT_WARNINGS
