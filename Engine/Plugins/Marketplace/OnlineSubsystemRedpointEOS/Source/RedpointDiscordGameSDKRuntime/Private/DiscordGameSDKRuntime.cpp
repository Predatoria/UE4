// Copyright June Rhodes. All Rights Reserved.

#include "DiscordGameSDK.h"

#include "CoreMinimal.h"
#include "GenericPlatform/GenericPlatformFile.h"
#if defined(UE_5_0_OR_LATER)
#include "HAL/PlatformFileManager.h"
#else
#include "HAL/PlatformFilemanager.h"
#endif // #if defined(UE_5_0_OR_LATER)
#include "HAL/PlatformProcess.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogDiscordRuntime);

void FRedpointDiscordGameSDKRuntimeModule::StartupModule()
{
#if PLATFORM_WINDOWS
    UE_LOG(
        LogDiscordRuntime,
        Verbose,
        TEXT("Discord Game SDK Dynamic Load - Starting dynamic load of Discord Game SDK..."));

    TArray<FString> SearchPaths;
    FString BaseDir = IPluginManager::Get().FindPlugin("OnlineSubsystemRedpointEOS")->GetBaseDir();

    FString CommonAppData = FPlatformMisc::GetEnvironmentVariable(TEXT("PROGRAMDATA"));
    FString UserProfile = FPlatformMisc::GetEnvironmentVariable(TEXT("USERPROFILE"));
    FString SystemDrive = FPlatformMisc::GetEnvironmentVariable(TEXT("SYSTEMDRIVE"));

    FString DllName = TEXT("discord_game_sdk.dll");

#if PLATFORM_64BITS
    FString Arch = TEXT("x86_64");
#else
    FString Arch = TEXT("x86");
#endif

    SearchPaths.Add(FPaths::ConvertRelativePathToFull(
        BaseDir / TEXT("Source") / TEXT("ThirdParty-DiscordGameSDK") / TEXT("lib") / Arch));
    if (!CommonAppData.IsEmpty())
    {
        SearchPaths.Add(CommonAppData / TEXT("DiscordGameSDK") / TEXT("lib") / Arch);
    }
    if (!UserProfile.IsEmpty())
    {
        SearchPaths.Add(UserProfile / TEXT("Downloads") / TEXT("discord_game_sdk") / TEXT("lib") / Arch);
    }
    if (!SystemDrive.IsEmpty())
    {
        SearchPaths.Add(SystemDrive / TEXT("DiscordGameSDK") / TEXT("lib") / Arch);
    }

    for (const auto &BaseSearchPath : SearchPaths)
    {
        FString SearchPath = BaseSearchPath / DllName;

        if (!FPaths::FileExists(SearchPath))
        {
            UE_LOG(
                LogDiscordRuntime,
                Verbose,
                TEXT("Discord Game SDK Dynamic Load - Could not locate the DLL at: %s"),
                *SearchPath);
            continue;
        }

        UE_LOG(LogDiscordRuntime, Verbose, TEXT("Discord Game SDK Dynamic Load - DLL found at: %s"), *SearchPath);
        FPlatformProcess::PushDllDirectory(*FPaths::GetPath(SearchPath));
        this->DynamicLibraryHandle = FPlatformProcess::GetDllHandle(*SearchPath);
        FPlatformProcess::PopDllDirectory(*FPaths::GetPath(SearchPath));

        if (this->DynamicLibraryHandle != nullptr)
        {
            UE_LOG(
                LogDiscordRuntime,
                Verbose,
                TEXT("Discord Game SDK Dynamic Load - Successfully loaded DLL at: %s"),
                *SearchPath);

            break;
        }
        else
        {
            UE_LOG(
                LogDiscordRuntime,
                Error,
                TEXT("Discord Game SDK Dynamic Load - Failed to load DLL at: %s"),
                *SearchPath);
        }
    }
#endif
}

void FRedpointDiscordGameSDKRuntimeModule::ShutdownModule()
{
#if PLATFORM_WINDOWS
    if (this->DynamicLibraryHandle != nullptr)
    {
        UE_LOG(
            LogDiscordRuntime,
            Verbose,
            TEXT("Discord Game SDK Dynamic Unload - Starting dynamic unload of Discord Game SDK..."));
        FPlatformProcess::FreeDllHandle(this->DynamicLibraryHandle);
        this->DynamicLibraryHandle = nullptr;
    }
#endif
}

bool FRedpointDiscordGameSDKRuntimeModule::IsAvailable() const
{
#if PLATFORM_WINDOWS
    return this->DynamicLibraryHandle != nullptr;
#else
    return false;
#endif
}

IMPLEMENT_MODULE(FRedpointDiscordGameSDKRuntimeModule, RedpointDiscordGameSDKRuntime)

#if EOS_DISCORD_ENABLED
THIRD_PARTY_INCLUDES_START
#include "cpp/RedpointPatched/achievement_manager.cpp"
#include "cpp/RedpointPatched/activity_manager.cpp"
#include "cpp/RedpointPatched/application_manager.cpp"
#include "cpp/RedpointPatched/core.cpp"
#include "cpp/RedpointPatched/image_manager.cpp"
#include "cpp/RedpointPatched/lobby_manager.cpp"
#include "cpp/RedpointPatched/network_manager.cpp"
#include "cpp/RedpointPatched/overlay_manager.cpp"
#include "cpp/RedpointPatched/relationship_manager.cpp"
#include "cpp/RedpointPatched/storage_manager.cpp"
#include "cpp/RedpointPatched/store_manager.cpp"
#include "cpp/RedpointPatched/types.cpp"
#include "cpp/RedpointPatched/user_manager.cpp"
#include "cpp/RedpointPatched/voice_manager.cpp"
THIRD_PARTY_INCLUDES_END
#endif