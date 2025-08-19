// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointDiscord.h"

#include "LogRedpointDiscord.h"
#include "Misc/ConfigCacheIni.h"
#include "OnlineAvatarInterfaceRedpointDiscord.h"
#include "OnlineExternalUIInterfaceRedpointDiscord.h"
#include "OnlineFriendsInterfaceRedpointDiscord.h"
#include "OnlineIdentityInterfaceRedpointDiscord.h"
#include "OnlinePresenceInterfaceRedpointDiscord.h"
#include "OnlineSessionInterfaceRedpointDiscord.h"

#define DISCORD_CONFIG_SECTION TEXT("OnlineSubsystemRedpointDiscord")

#if EOS_DISCORD_ENABLED

#define DESTRUCT_INTERFACE(Interface)                                                                                  \
    if (Interface.IsValid())                                                                                           \
    {                                                                                                                  \
        ensure(Interface.IsUnique());                                                                                  \
        Interface = nullptr;                                                                                           \
    }

FOnlineSubsystemRedpointDiscord::FOnlineSubsystemRedpointDiscord(FName InInstanceName)
    : FOnlineSubsystemImplBase(REDPOINT_DISCORD_SUBSYSTEM, InInstanceName)
    , bInitialized(false)
    , Instance(nullptr)
    , IdentityImpl(nullptr)
    , ExternalUIImpl(nullptr)
    , AvatarImpl(nullptr)
    , FriendsImpl(nullptr)
    , PresenceImpl(nullptr)
{
}

FOnlineSubsystemRedpointDiscord ::~FOnlineSubsystemRedpointDiscord()
{
}

bool FOnlineSubsystemRedpointDiscord::IsEnabled() const
{
    return true;
}

IOnlineIdentityPtr FOnlineSubsystemRedpointDiscord::GetIdentityInterface() const
{
    return this->IdentityImpl;
}

IOnlineFriendsPtr FOnlineSubsystemRedpointDiscord::GetFriendsInterface() const
{
    return this->FriendsImpl;
}

IOnlinePresencePtr FOnlineSubsystemRedpointDiscord::GetPresenceInterface() const
{
    return this->PresenceImpl;
}

IOnlineSessionPtr FOnlineSubsystemRedpointDiscord::GetSessionInterface() const
{
    return this->SessionImpl;
}

IOnlineExternalUIPtr FOnlineSubsystemRedpointDiscord::GetExternalUIInterface() const
{
    return this->ExternalUIImpl;
}

class UObject *FOnlineSubsystemRedpointDiscord::GetNamedInterface(FName InterfaceName)
{
    if (InterfaceName.IsEqual(ONLINE_AVATAR_INTERFACE_NAME))
    {
        return (class UObject *)(void *)&this->AvatarImpl;
    }

    return nullptr;
}

bool FOnlineSubsystemRedpointDiscord::Init()
{
    this->bInitialized = false;
    this->Instance = nullptr;

    FString ApplicationIdStr;
    if (!GConfig->GetString(DISCORD_CONFIG_SECTION, TEXT("ApplicationId"), ApplicationIdStr, GEngineIni))
    {
        return false;
    }
    int64 ApplicationId = FCString::Atoi64(*ApplicationIdStr);

    discord::Core *InstancePtr;
    discord::Result CreateResult =
        discord::Core::Create(ApplicationId, DiscordCreateFlags_NoRequireDiscord, &InstancePtr);
    if (CreateResult != discord::Result::Ok)
    {
        UE_LOG(
            LogRedpointDiscord,
            Warning,
            TEXT("Unable to initialize Discord Game SDK, got result code: %d"),
            CreateResult);
        return false;
    }

    this->Instance = MakeShareable(InstancePtr);
    this->bInitialized = true;

    UE_LOG(LogRedpointDiscord, Verbose, TEXT("Discord Game SDK has been initialized successfully."));
    this->Instance->SetLogHook(discord::LogLevel::Debug, [](discord::LogLevel level, const char *message) {
        FString Message(message);
        switch (level)
        {
        case discord::LogLevel::Error:
            UE_LOG(LogRedpointDiscord, Error, TEXT("Log(%d): %s"), level, *Message);
            break;
        case discord::LogLevel::Warn:
            UE_LOG(LogRedpointDiscord, Warning, TEXT("Log(%d): %s"), level, *Message);
            break;
        case discord::LogLevel::Info:
            UE_LOG(LogRedpointDiscord, Display, TEXT("Log(%d): %s"), level, *Message);
            break;
        case discord::LogLevel::Debug:
        default:
            UE_LOG(LogRedpointDiscord, Verbose, TEXT("Log(%d): %s"), level, *Message);
            break;
        }
    });

    this->ExternalUIImpl =
        MakeShared<FOnlineExternalUIInterfaceRedpointDiscord, ESPMode::ThreadSafe>(this->Instance.ToSharedRef());
    this->ExternalUIImpl->RegisterEvents();
    this->IdentityImpl = MakeShared<FOnlineIdentityInterfaceRedpointDiscord, ESPMode::ThreadSafe>(
        this->Instance.ToSharedRef(),
        this->ExternalUIImpl.ToSharedRef());
    this->AvatarImpl =
        MakeShared<FOnlineAvatarInterfaceRedpointDiscord, ESPMode::ThreadSafe>(this->Instance.ToSharedRef());
    this->FriendsImpl = MakeShared<FOnlineFriendsInterfaceRedpointDiscord, ESPMode::ThreadSafe>(
        this->Instance.ToSharedRef(),
        this->IdentityImpl.ToSharedRef());
    this->PresenceImpl = MakeShared<FOnlinePresenceInterfaceRedpointDiscord, ESPMode::ThreadSafe>(
        this->IdentityImpl.ToSharedRef(),
        this->FriendsImpl.ToSharedRef());
    this->PresenceImpl->ConnectFriendsToPresence();
    this->FriendsImpl->RegisterEvents();
    this->SessionImpl =
        MakeShared<FOnlineSessionInterfaceRedpointDiscord, ESPMode::ThreadSafe>(this->Instance.ToSharedRef());
    this->SessionImpl->RegisterEvents();

    return true;
}

bool FOnlineSubsystemRedpointDiscord::Tick(float DeltaTime)
{
    if (this->Instance.IsValid())
    {
        this->Instance->RunCallbacks();
    }

    return FOnlineSubsystemImpl::Tick(DeltaTime);
}

bool FOnlineSubsystemRedpointDiscord::Shutdown()
{
    if (this->bInitialized)
    {
        DESTRUCT_INTERFACE(this->SessionImpl);
        this->PresenceImpl->DisconnectFriendsFromPresence();
        DESTRUCT_INTERFACE(this->PresenceImpl);
        DESTRUCT_INTERFACE(this->FriendsImpl);
        DESTRUCT_INTERFACE(this->AvatarImpl);
        DESTRUCT_INTERFACE(this->IdentityImpl);
        DESTRUCT_INTERFACE(this->ExternalUIImpl);

        checkf(this->Instance.IsValid(), TEXT("Should not have bInitialized true and Instance null"));

        this->Instance = nullptr;
        this->bInitialized = false;
    }

    return true;
}

FString FOnlineSubsystemRedpointDiscord::GetAppId() const
{
    FString ApplicationIdStr;
    if (!GConfig->GetString(DISCORD_CONFIG_SECTION, TEXT("ApplicationId"), ApplicationIdStr, GEngineIni))
    {
        return ApplicationIdStr;
    }
    return TEXT("");
}

FText FOnlineSubsystemRedpointDiscord::GetOnlineServiceName() const
{
    return FText::GetEmpty();
}

#endif