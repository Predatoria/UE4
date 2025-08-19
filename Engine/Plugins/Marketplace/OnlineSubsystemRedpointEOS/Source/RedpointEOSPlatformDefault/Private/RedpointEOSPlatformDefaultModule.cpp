// Copyright June Rhodes. All Rights Reserved.

#include "Authentication/Nodes/GetExternalCredentialsFromAppleNode.h"
#include "Authentication/Nodes/GetExternalCredentialsFromGOGNode.h"
#include "Authentication/Nodes/GetExternalCredentialsFromGoogleNode.h"
#include "Authentication/Nodes/GetExternalCredentialsFromOculusNode.h"
#include "Authentication/Nodes/GetExternalCredentialsFromSteamNode.h"
#include "CoreMinimal.h"
#include "EOSRuntimePlatformAndroid.h"
#include "EOSRuntimePlatformIOS.h"
#include "EOSRuntimePlatformLinux.h"
#include "EOSRuntimePlatformMac.h"
#include "EOSRuntimePlatformWindows.h"
#include "Modules/ModuleManager.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraphRegistry.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Graphs/AuthenticationGraphOnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOSModule.h"

#if EOS_HAS_AUTHENTICATION && EOS_DISCORD_ENABLED
#include "OnlineSubsystemRedpointDiscordConstants.h"
#endif
#if EOS_HAS_AUTHENTICATION && EOS_ITCH_IO_ENABLED
#include "OnlineSubsystemRedpointItchIoConstants.h"
#endif

class FRedpointEOSPlatformDefaultModule : public IModuleInterface
{
    virtual void StartupModule() override
    {
        FModuleManager::GetModuleChecked<FOnlineSubsystemRedpointEOSModule>(FName("OnlineSubsystemRedpointEOS"))
#if PLATFORM_WINDOWS
            .SetRuntimePlatform(MakeShared<FEOSRuntimePlatformWindows>())
#elif PLATFORM_MAC
            .SetRuntimePlatform(MakeShared<FEOSRuntimePlatformMac>())
#elif PLATFORM_LINUX
            .SetRuntimePlatform(MakeShared<FEOSRuntimePlatformLinux>())
#elif PLATFORM_IOS
            .SetRuntimePlatform(MakeShared<FEOSRuntimePlatformIOS>())
#elif PLATFORM_ANDROID
            .SetRuntimePlatform(MakeShared<FEOSRuntimePlatformAndroid>())
#endif
            ;

#if EOS_HAS_AUTHENTICATION && EOS_STEAM_ENABLED
#if EOS_VERSION_AT_LEAST(1, 15, 1) ||                                                                                  \
    (EOS_VERSION_AT_LEAST(1, 14, 2) && EOS_VERSION_AT_MOST(1, 15, 0) && !EOS_VERSION_AT_LEAST(1, 15, 0) &&             \
     defined(EOS_HOTFIX_VERSION) && EOS_HOTFIX_VERSION >= 2)
        FAuthenticationGraphOnlineSubsystem::RegisterForCustomPlatform(
            FName(TEXT("Steam")),
            NSLOCTEXT("OnlineSubsystemRedpointEOS", "AuthGraph_Steam", "Steam Only"),
            STEAM_SUBSYSTEM,
            EOS_EExternalCredentialType::EOS_ECT_STEAM_SESSION_TICKET,
            TEXT("steam"),
            TEXT("steam.sessionTicket"),
            MakeShared<FGetExternalCredentialsFromSteamNode>());
#else
        FAuthenticationGraphOnlineSubsystem::RegisterForCustomPlatform(
            FName(TEXT("Steam")),
            NSLOCTEXT("OnlineSubsystemRedpointEOS", "AuthGraph_Steam", "Steam Only"),
            STEAM_SUBSYSTEM,
            EOS_EExternalCredentialType::EOS_ECT_STEAM_APP_TICKET,
            TEXT("steam"),
            TEXT("steam.encryptedAppTicket"),
            MakeShared<FGetExternalCredentialsFromSteamNode>());
#endif
#endif // #if EOS_HAS_AUTHENTICATION && EOS_STEAM_ENABLED

#if EOS_HAS_AUTHENTICATION && EOS_DISCORD_ENABLED
        FAuthenticationGraphOnlineSubsystem::RegisterForCustomPlatform(
            FName(TEXT("Discord")),
            NSLOCTEXT("OnlineSubsystemRedpointEOS", "AuthGraph_Discord", "Discord Only"),
            REDPOINT_DISCORD_SUBSYSTEM,
            EOS_EExternalCredentialType::EOS_ECT_DISCORD_ACCESS_TOKEN,
            TEXT("discord"),
            TEXT("discord.accessToken"));
#endif // #if EOS_HAS_AUTHENTICATION && EOS_DISCORD_ENABLED

#if EOS_HAS_AUTHENTICATION && EOS_GOG_ENABLED
        FAuthenticationGraphOnlineSubsystem::RegisterForCustomPlatform(
            FName(TEXT("GOG")),
            NSLOCTEXT("OnlineSubsystemRedpointEOS", "AuthGraph_GOG", "GOG Galaxy Only"),
            FName(TEXT("GOG")),
            EOS_EExternalCredentialType::EOS_ECT_GOG_SESSION_TICKET,
            TEXT("gog"),
            TEXT("gog.encryptedAppTicket"),
            MakeShared<FGetExternalCredentialsFromGOGNode>());
#endif // #if EOS_HAS_AUTHENTICATION && EOS_GOG_ENABLED

#if EOS_HAS_AUTHENTICATION && EOS_ITCH_IO_ENABLED && EOS_VERSION_AT_LEAST(1, 12, 0)
        FAuthenticationGraphOnlineSubsystem::RegisterForCustomPlatform(
            FName(TEXT("ItchIo")),
            NSLOCTEXT("OnlineSubsystemRedpointEOS", "AuthGraph_ItchIo", "itch.io Only"),
            REDPOINT_ITCH_IO_SUBSYSTEM,
            EOS_EExternalCredentialType::EOS_ECT_ITCHIO_JWT,
            TEXT("itchIo"),
            TEXT("itchIo.apiKey"));
#endif // #if EOS_HAS_AUTHENTICATION && EOS_ITCH_IO_ENABLED

#if EOS_HAS_AUTHENTICATION && EOS_OCULUS_ENABLED && EOS_VERSION_AT_LEAST(1, 10, 3)
        FAuthenticationGraphOnlineSubsystem::RegisterForCustomPlatform(
            FName(TEXT("Oculus")),
            NSLOCTEXT("OnlineSubsystemRedpointEOS", "AuthGraph_Oculus", "Oculus Only"),
            OCULUS_SUBSYSTEM,
            EOS_EExternalCredentialType::EOS_ECT_OCULUS_USERID_NONCE,
            TEXT("oculus"),
            TEXT("oculus.userIdNonce"),
            MakeShared<FGetExternalCredentialsFromOculusNode>());
#elif WITH_EDITOR
        FAuthenticationGraphRegistry::RegisterPlaceholder(
            FName(TEXT("Oculus")),
            NSLOCTEXT("OnlineSubsystemRedpointEOS", "AuthGraph_Oculus", "Oculus Only"));
#endif // #if EOS_HAS_AUTHENTICATION && EOS_ITCH_IO_ENABLED

#if EOS_HAS_AUTHENTICATION && EOS_GOOGLE_ENABLED
        FAuthenticationGraphOnlineSubsystem::RegisterForCustomPlatform(
            FName(TEXT("Google")),
            NSLOCTEXT("OnlineSubsystemRedpointEOS", "AuthGraph_Google", "Google Only"),
            GOOGLE_SUBSYSTEM,
            EOS_EExternalCredentialType::EOS_ECT_GOOGLE_ID_TOKEN,
            TEXT("google"),
            TEXT("google.idToken"),
            MakeShared<FGetExternalCredentialsFromGoogleNode>());
#elif WITH_EDITOR
        FAuthenticationGraphRegistry::RegisterPlaceholder(
            FName(TEXT("Google")),
            NSLOCTEXT("OnlineSubsystemRedpointEOS", "AuthGraph_Google", "Google Only"));
#endif // #if EOS_HAS_AUTHENTICATION && EOS_GOOGLE_ENABLED

#if EOS_HAS_AUTHENTICATION && EOS_APPLE_ENABLED && EOS_APPLE_HAS_RUNTIME_SUPPORT
        FAuthenticationGraphOnlineSubsystem::RegisterForCustomPlatform(
            FName(TEXT("Apple")),
            NSLOCTEXT("OnlineSubsystemRedpointEOS", "AuthGraph_Apple", "Apple Only"),
            APPLE_SUBSYSTEM,
            EOS_EExternalCredentialType::EOS_ECT_APPLE_ID_TOKEN,
            TEXT("apple"),
            TEXT("apple.idToken"),
            MakeShared<FGetExternalCredentialsFromAppleNode>());
#elif WITH_EDITOR
        FAuthenticationGraphRegistry::RegisterPlaceholder(
            FName(TEXT("Apple")),
            NSLOCTEXT("OnlineSubsystemRedpointEOS", "AuthGraph_Apple", "Apple Only"));
#endif // #if EOS_HAS_AUTHENTICATION && EOS_APPLE_ENABLED && EOS_APPLE_HAS_RUNTIME_SUPPORT
    }

    virtual void ShutdownModule() override
    {
    }
};

IMPLEMENT_MODULE(FRedpointEOSPlatformDefaultModule, RedpointEOSPlatformDefault)