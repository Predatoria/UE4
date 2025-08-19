// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Graphs/AuthenticationGraphDefault.h"

#include "Engine/World.h"
#include "HAL/PlatformMisc.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraphRegistry.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/CrossPlatformAccountId.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Graphs/AuthenticationGraphAlwaysFail.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Graphs/AuthenticationGraphCrossPlatformOnly.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Graphs/AuthenticationGraphDevAuthTool.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemUtils.h"

EOS_ENABLE_STRICT_WARNINGS

FName FAuthenticationGraphDefault::Resolve(
    const TSharedRef<class FAuthenticationGraphRegistry> &InRegistry,
    const TSharedRef<class FEOSConfig> &InConfig,
    const FOnlineAccountCredentials &InProvidedCredentials,
    const TSoftObjectPtr<UWorld> &InWorld)
{
    if (InRegistry->Has(FName(PREPROCESSOR_TO_STRING(PLATFORM_HEADER_NAME))))
    {
        // Pick the platform specific subsystem if there is one.
        return FName(PREPROCESSOR_TO_STRING(PLATFORM_HEADER_NAME));
    }

    // Otherwise we need to choose the graph, based on what online subsystem is available.

#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
    {
        FString AuthType;
        FString ExchangeCode;
        if (InConfig->GetCrossPlatformAccountProvider().IsEqual(EPIC_GAMES_ACCOUNT_ID) &&
            InRegistry->Has(EOS_AUTH_GRAPH_CROSS_PLATFORM_ONLY))
        {
            if (FParse::Value(FCommandLine::Get(), TEXT("AUTH_TYPE="), AuthType, true) &&
                AuthType == TEXT("exchangecode") &&
                FParse::Value(FCommandLine::Get(), TEXT("AUTH_PASSWORD="), ExchangeCode, true))
            {
                // Use the cross-platform only authentication graph, which will authenticate
                // Epic Games via exchange code.
                return EOS_AUTH_GRAPH_CROSS_PLATFORM_ONLY;
            }
        }
    }
#endif

    if (InWorld.IsValid() && InRegistry->Has(FName(TEXT("Steam"))))
    {
        IOnlineSubsystem *SteamOSS = Online::GetSubsystem(InWorld.Get(), STEAM_SUBSYSTEM);
        if (SteamOSS != nullptr)
        {
            IOnlineIdentityPtr SteamIdentity = SteamOSS->GetIdentityInterface();
            if (SteamIdentity.IsValid() && SteamIdentity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
            {
                // Use the Steam authentication graph.
                return FName(TEXT("Steam"));
            }
        }
    }

    if (InWorld.IsValid() && InRegistry->Has(FName(TEXT("Discord"))))
    {
        FString DiscordAccessToken = FPlatformMisc::GetEnvironmentVariable(TEXT("DISCORD_ACCESS_TOKEN"));
        if (!DiscordAccessToken.IsEmpty())
        {
            IOnlineSubsystem *DiscordOSS = Online::GetSubsystem(InWorld.Get(), FName(TEXT("RedpointDiscord")));
            if (DiscordOSS != nullptr)
            {
                // Use the Discord authentication graph.
                return FName(TEXT("Discord"));
            }
        }
    }

    if (InWorld.IsValid() && InRegistry->Has(FName(TEXT("ItchIo"))))
    {
        FString ItchIoApiKey = FPlatformMisc::GetEnvironmentVariable(TEXT("ITCHIO_API_KEY"));
        if (!ItchIoApiKey.IsEmpty())
        {
            IOnlineSubsystem *ItchIoOSS = Online::GetSubsystem(InWorld.Get(), FName(TEXT("RedpointItchIo")));
            if (ItchIoOSS != nullptr)
            {
                // Use the itch.io authentication graph.
                return FName(TEXT("ItchIo"));
            }
        }
    }

    if (InWorld.IsValid() && InRegistry->Has(FName(TEXT("Oculus"))))
    {
        IOnlineSubsystem *OculusOSS = Online::GetSubsystem(InWorld.Get(), OCULUS_SUBSYSTEM);
        if (OculusOSS != nullptr)
        {
            IOnlineIdentityPtr OculusIdentity = OculusOSS->GetIdentityInterface();
            if (OculusIdentity.IsValid() && OculusIdentity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
            {
                if (
#if PLATFORM_WINDOWS
                    !GConfig->GetStr(TEXT("OnlineSubsystemOculus"), TEXT("RiftAppId"), GEngineIni).IsEmpty() ||
#elif PLATFORM_ANDROID
                    !GConfig->GetStr(TEXT("OnlineSubsystemOculus"), TEXT("MobileAppId"), GEngineIni).IsEmpty() ||
#endif
                    !GConfig->GetStr(TEXT("OnlineSubsystemOculus"), TEXT("OculusAppId"), GEngineIni).IsEmpty())
                {
                    // Use the Oculus authentication graph.
                    return FName(TEXT("Oculus"));
                }
            }
        }
    }

#if PLATFORM_ANDROID
    if (InWorld.IsValid() && InRegistry->Has(FName(TEXT("Google"))))
    {
        IOnlineSubsystem *GoogleOSS = Online::GetSubsystem(InWorld.Get(), GOOGLE_SUBSYSTEM);
        if (GoogleOSS != nullptr)
        {
            IOnlineIdentityPtr GoogleIdentity = GoogleOSS->GetIdentityInterface();
            if (GoogleIdentity.IsValid())
            {
                // Use the Google authentication graph.
                return FName(TEXT("Google"));
            }
        }
    }
#endif

#if PLATFORM_IOS
    if (InWorld.IsValid() && InRegistry->Has(FName(TEXT("Apple"))))
    {
        IOnlineSubsystem *AppleOSS = Online::GetSubsystem(InWorld.Get(), FName(TEXT("APPLE")));
        if (AppleOSS != nullptr)
        {
            IOnlineIdentityPtr AppleIdentity = AppleOSS->GetIdentityInterface();
            if (AppleIdentity.IsValid())
            {
                // Use the Apple authentication graph.
                return FName(TEXT("Apple"));
            }
        }
    }
#endif

#if WITH_EDITOR && (!defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING)
    if (InWorld.IsValid() && InRegistry->Has(EOS_AUTH_GRAPH_DEV_AUTH_TOOL) &&
        (InWorld->WorldType == EWorldType::Editor || InWorld->WorldType == EWorldType::PIE ||
         InWorld->WorldType == EWorldType::EditorPreview || InWorld->WorldType == EWorldType::Inactive ||
         /* Standalone game */
         InWorld->IsPlayInPreview()))
    {
        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("Choosing to authenticate via the Developer Authentication Tool because you are running in the editor "
                 "and there is no other way to authenticate."));
        return EOS_AUTH_GRAPH_DEV_AUTH_TOOL;
    }
#endif

    return EOS_AUTH_GRAPH_ALWAYS_FAIL;
}

FName FAuthenticationGraphDefault::ResolveWithCrossPlatform(
    const TSharedRef<class FAuthenticationGraphRegistry> &InRegistry,
    const TSharedRef<class FEOSConfig> &InConfig,
    const FOnlineAccountCredentials &InProvidedCredentials,
    const TSoftObjectPtr<UWorld> &InWorld)
{
    FName Result = FAuthenticationGraphDefault::Resolve(InRegistry, InConfig, InProvidedCredentials, InWorld);
    if (Result == EOS_AUTH_GRAPH_ALWAYS_FAIL)
    {
        if (!InConfig->GetCrossPlatformAccountProvider().IsNone())
        {
            return EOS_AUTH_GRAPH_CROSS_PLATFORM_ONLY;
        }
    }
    return Result;
}

void FAuthenticationGraphDefault::Register()
{
    FAuthenticationGraphRegistry::Register(
        EOS_AUTH_GRAPH_DEFAULT,
        NSLOCTEXT("OnlineSubsystemRedpointEOS", "AuthGraph_Default", "Default"),
        FResolveAuthenticationGraphDelegate::CreateStatic(&FAuthenticationGraphDefault::Resolve));
    FAuthenticationGraphRegistry::Register(
        EOS_AUTH_GRAPH_DEFAULT_WITH_CROSS_PLATFORM_FALLBACK,
        NSLOCTEXT(
            "OnlineSubsystemRedpointEOS",
            "AuthGraph_DefaultCrossPlatformFallback",
            "Default, with Cross-Platform Fallback"),
        FResolveAuthenticationGraphDelegate::CreateStatic(&FAuthenticationGraphDefault::ResolveWithCrossPlatform));
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION