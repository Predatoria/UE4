// Copyright June Rhodes. All Rights Reserved.

#include "EOSRuntimePlatformDesktopBase.h"

#include "CoreMinimal.h"

#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX

#include "EOSRuntimePlatformIntegrationDiscord.h"
#include "EOSRuntimePlatformIntegrationGOG.h"
#include "EOSRuntimePlatformIntegrationItchIo.h"
#include "EOSRuntimePlatformIntegrationOculus.h"
#include "EOSRuntimePlatformIntegrationSteam.h"
#include "GenericPlatform/GenericPlatformFile.h"
#if defined(UE_5_0_OR_LATER)
#include "HAL/PlatformFileManager.h"
#else
#include "HAL/PlatformFilemanager.h"
#endif // #if defined(UE_5_0_OR_LATER)
#include "HAL/PlatformProcess.h"
#include "Interfaces/IPluginManager.h"
#include "LogEOSPlatformDefault.h"
#include "Logging/LogMacros.h"
#include "Misc/App.h"
#include "Misc/Paths.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#if !WITH_EDITOR
#include "Misc/MessageDialog.h"
#endif

#define LOCTEXT_NAMESPACE "FOnlineSubsystemRedpointEOSModule"

void FEOSRuntimePlatformDesktopBase::Load()
{
    UE_LOG(LogEOSPlatformDefault, Verbose, TEXT("EOS SDK Dynamic Load - Starting dynamic load of EOS SDK..."));

    // Determine the library search paths for this platform.
    TArray<FString> SearchPaths;
    FString BaseDir = IPluginManager::Get().FindPlugin("OnlineSubsystemRedpointEOS")->GetBaseDir();
#if PLATFORM_WINDOWS
    FString XAudio29DllPath = TEXT("");
    FString CommonAppData = FPlatformMisc::GetEnvironmentVariable(TEXT("PROGRAMDATA"));
    FString UserProfile = FPlatformMisc::GetEnvironmentVariable(TEXT("USERPROFILE"));
    FString SystemDrive = FPlatformMisc::GetEnvironmentVariable(TEXT("SYSTEMDRIVE"));

#if PLATFORM_64BITS
    FString DllName = TEXT("EOSSDK-Win64-Shipping.dll");
    FString XAudio29DllName = TEXT("x64\\xaudio2_9redist.dll");
#else
    FString DllName = TEXT("EOSSDK-Win32-Shipping.dll");
    FString XAudio29DllName = TEXT("x86\\xaudio2_9redist.dll");
#endif

#if !WITH_EDITOR
#if PLATFORM_64BITS
    SearchPaths.Add(FPaths::ConvertRelativePathToFull(
        FPaths::ProjectDir() / TEXT("Binaries") / TEXT("Win64") / TEXT("RedpointEOS")));
#else
    SearchPaths.Add(FPaths::ConvertRelativePathToFull(
        FPaths::ProjectDir() / TEXT("Binaries") / TEXT("Win32") / TEXT("RedpointEOS")));
#endif
#else
    SearchPaths.Add(FPaths::ConvertRelativePathToFull(BaseDir / TEXT("Source") / TEXT("ThirdParty") / TEXT("Bin")));
    SearchPaths.Add(FPaths::ConvertRelativePathToFull(
        BaseDir / TEXT("..") / TEXT("..") / TEXT("EOS-SDK-" EOS_SDK_LINK_VERSION) / TEXT("SDK") / TEXT("Bin")));
    if (!CommonAppData.IsEmpty())
    {
        SearchPaths.Add(CommonAppData / TEXT("EOS-SDK-" EOS_SDK_LINK_VERSION) / TEXT("SDK") / TEXT("Bin"));
    }
    if (!UserProfile.IsEmpty())
    {
        SearchPaths.Add(
            UserProfile / TEXT("Downloads") / TEXT("EOS-SDK-" EOS_SDK_LINK_VERSION) / TEXT("SDK") / TEXT("Bin"));
    }
    if (!SystemDrive.IsEmpty())
    {
        SearchPaths.Add(SystemDrive / TEXT("EOS-SDK-" EOS_SDK_LINK_VERSION) / TEXT("SDK") / TEXT("Bin"));
    }
#endif
#elif PLATFORM_MAC
    FString Home = FPlatformMisc::GetEnvironmentVariable(TEXT("HOME"));

    FString DllName = TEXT("libEOSSDK-Mac-Shipping.dylib");

#if !WITH_EDITOR
    // Due to macOS RPATH shenanigans, packaged games need the library next to the main executable in /Contents/MacOS/,
    // not in the /Contents/UE4/<project>/Binaries/Mac/ inside the bundle.
    SearchPaths.Add(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("..") / TEXT("..") / TEXT("MacOS")));

    // When executing staged builds under Gauntlet, the game is not a packaged build, and the path to the SDK is
    // different.
    SearchPaths.Add(FPaths::ConvertRelativePathToFull(
        FPaths::ProjectDir() / TEXT("Binaries") / TEXT("Mac") /
        FString::Printf(TEXT("%s-Mac-Development.app"), FApp::GetProjectName()) / TEXT("Contents") / TEXT("MacOS")));
    SearchPaths.Add(FPaths::ConvertRelativePathToFull(
        FPaths::ProjectDir() / TEXT("Binaries") / TEXT("Mac") /
        FString::Printf(TEXT("%s-Mac-DebugGame.app"), FApp::GetProjectName()) / TEXT("Contents") / TEXT("MacOS")));
#else
    SearchPaths.Add(FPaths::ConvertRelativePathToFull(BaseDir / TEXT("Source") / TEXT("ThirdParty") / TEXT("Bin")));
    SearchPaths.Add(FPaths::ConvertRelativePathToFull(
        BaseDir / TEXT("..") / TEXT("..") / TEXT("EOS-SDK-" EOS_SDK_LINK_VERSION) / TEXT("SDK") / TEXT("Bin")));
    if (!Home.IsEmpty())
    {
        SearchPaths.Add(Home / TEXT("Downloads") / TEXT("EOS-SDK-" EOS_SDK_LINK_VERSION) / TEXT("SDK") / TEXT("Bin"));
    }
#endif
#elif PLATFORM_LINUX
    FString Home = FPlatformMisc::GetEnvironmentVariable(TEXT("HOME"));

    FString DllName = TEXT("libEOSSDK-Linux-Shipping.so");

#if !WITH_EDITOR
    SearchPaths.Add(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("Binaries") / TEXT("Linux")));
#else
    SearchPaths.Add(FPaths::ConvertRelativePathToFull(BaseDir / TEXT("Source") / TEXT("ThirdParty") / TEXT("Bin")));
    SearchPaths.Add(FPaths::ConvertRelativePathToFull(
        BaseDir / TEXT("..") / TEXT("..") / TEXT("EOS-SDK-" EOS_SDK_LINK_VERSION) / TEXT("SDK") / TEXT("Bin")));
    if (!Home.IsEmpty())
    {
        SearchPaths.Add(Home / TEXT("Downloads") / TEXT("EOS-SDK-" EOS_SDK_LINK_VERSION) / TEXT("SDK") / TEXT("Bin"));
    }
#endif
#else
#error Platform not supported by EOSDynamicLoaderDefault.
#endif

    bool bSimulateDLLNotFound =
        FString(FPlatformMisc::GetEnvironmentVariable(TEXT("EOS_SIMULATE_DLL_NOT_AVAILABLE"))) == TEXT("true");
    if (bSimulateDLLNotFound)
    {
        // We must delete the EOS SDK DLL if it resides in the default Windows search path, otherwise the DLL will
        // be implicitly loaded by the linker even if we fail to load it here.
        FString WindowsDllPath =
            FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("Binaries") / TEXT("Win64") / DllName);
        if (FPaths::FileExists(WindowsDllPath))
        {
            UE_LOG(
                LogEOSPlatformDefault,
                Warning,
                TEXT("EOS SDK Dynamic Load - Removing EOS SDK DLL at this path to ensure that "
                     "EOS_SIMULATE_DLL_NOT_AVAILABLE=true acts as intended: %s"),
                *WindowsDllPath);
            FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*WindowsDllPath);
        }
    }

    // Now try each of them in sequence.
    for (const auto &BaseSearchPath : SearchPaths)
    {
        FString SearchPath = BaseSearchPath / DllName;

        if (bSimulateDLLNotFound)
        {
            UE_LOG(
                LogEOSPlatformDefault,
                Warning,
                TEXT("EOS SDK Dynamic Load - Not checking path because environment variable "
                     "EOS_SIMULATE_DLL_NOT_AVAILABLE=true: %s"),
                *SearchPath);
            continue;
        }

        if (!FPaths::FileExists(SearchPath))
        {
            UE_LOG(
                LogEOSPlatformDefault,
                Verbose,
                TEXT("EOS SDK Dynamic Load - Could not locate the DLL at: %s"),
                *SearchPath);
            continue;
        }

        UE_LOG(LogEOSPlatformDefault, Verbose, TEXT("EOS SDK Dynamic Load - DLL found at: %s"), *SearchPath);
        FPlatformProcess::PushDllDirectory(*FPaths::GetPath(SearchPath));
        this->DynamicLibraryHandle = FPlatformProcess::GetDllHandle(*SearchPath);
        FPlatformProcess::PopDllDirectory(*FPaths::GetPath(SearchPath));

        if (this->DynamicLibraryHandle != nullptr)
        {
            UE_LOG(
                LogEOSPlatformDefault,
                Verbose,
                TEXT("EOS SDK Dynamic Load - Successfully loaded DLL at: %s"),
                *SearchPath);

#if EOS_VERSION_AT_LEAST(1, 13, 0) && PLATFORM_WINDOWS
            // Try to find XAudio2 DLL.
            FString XAudio29DllCandidatePath = BaseSearchPath / XAudio29DllName;
            if (FPaths::FileExists(XAudio29DllCandidatePath))
            {
                XAudio29DllPath = XAudio29DllCandidatePath;
                UE_LOG(
                    LogEOSPlatformDefault,
                    Verbose,
                    TEXT("EOS SDK Dynamic Load - Successfully found XAudio29 DLL at: %s"),
                    *XAudio29DllCandidatePath);
            }
            else
            {
                UE_LOG(
                    LogEOSPlatformDefault,
                    Verbose,
                    TEXT("EOS SDK Dynamic Load - Unable to locate XAudio29 DLL for Voice Chat. Expected to find in "
                         "location: %s"),
                    *XAudio29DllCandidatePath);
            }
#endif

            break;
        }
        else
        {
            UE_LOG(LogEOSPlatformDefault, Error, TEXT("EOS SDK Dynamic Load - Failed to load DLL at: %s"), *SearchPath);
        }
    }

    if (this->DynamicLibraryHandle == nullptr)
    {
        UE_LOG(
            LogEOSPlatformDefault,
            Error,
            TEXT("EOS SDK Dynamic Load - Unable to locate the EOS SDK DLL in any of the supported locations. Check the "
                 "documentation for instructions on how to install the SDK."));
#if !WITH_EDITOR
        FMessageDialog::Open(
            EAppMsgType::Ok,
            LOCTEXT(
                "OnlineSubsystemEOS_SDKNotFound",
                "The EOS SDK could not be found. Please reinstall the application."));
        FPlatformMisc::RequestExit(false);
#endif
        return;
    }

#if EOS_VERSION_AT_LEAST(1, 13, 0)
    this->RTCOptions = EOS_Platform_RTCOptions{};
    this->RTCOptions.ApiVersion = EOS_PLATFORM_RTCOPTIONS_API_LATEST;
#if PLATFORM_WINDOWS
    this->WindowsRTCOptions = EOS_Windows_RTCOptions{};
    this->WindowsRTCOptions.ApiVersion = EOS_WINDOWS_RTCOPTIONS_API_LATEST;
    checkf(this->XAudio29DllPathAllocated == nullptr, TEXT("Must not have already allocated XAudio29 path!"));
    EOSString_Utf8Unlimited::AllocateToCharBuffer(XAudio29DllPath, this->XAudio29DllPathAllocated);
    this->WindowsRTCOptions.XAudio29DllPath = this->XAudio29DllPathAllocated;
    this->RTCOptions.PlatformSpecificOptions = (void *)&this->WindowsRTCOptions;
#else
    this->RTCOptions.PlatformSpecificOptions = nullptr;
#endif
#endif

#if EOS_STEAM_ENABLED
    this->Integrations.Add(MakeShared<FEOSRuntimePlatformIntegrationSteam>());
#endif
#if EOS_DISCORD_ENABLED
    this->Integrations.Add(MakeShared<FEOSRuntimePlatformIntegrationDiscord>());
#endif
#if EOS_GOG_ENABLED
    this->Integrations.Add(MakeShared<FEOSRuntimePlatformIntegrationGOG>());
#endif
#if EOS_ITCH_IO_ENABLED && EOS_VERSION_AT_LEAST(1, 12, 0)
    this->Integrations.Add(MakeShared<FEOSRuntimePlatformIntegrationItchIo>());
#endif
#if EOS_OCULUS_ENABLED && EOS_VERSION_AT_LEAST(1, 10, 3)
    this->Integrations.Add(MakeShared<FEOSRuntimePlatformIntegrationOculus>());
#endif
}

void FEOSRuntimePlatformDesktopBase::Unload()
{
#if PLATFORM_WINDOWS && EOS_VERSION_AT_LEAST(1, 13, 0)
    if (this->XAudio29DllPathAllocated != nullptr)
    {
        EOSString_Utf8Unlimited::FreeFromCharBuffer(this->XAudio29DllPathAllocated);
    }
#endif
    this->Integrations.Empty();

    UE_LOG(LogEOSPlatformDefault, Verbose, TEXT("EOS SDK Dynamic Unload - Starting dynamic unload of EOS SDK..."));
    FPlatformProcess::FreeDllHandle(this->DynamicLibraryHandle);
    this->DynamicLibraryHandle = nullptr;
}

bool FEOSRuntimePlatformDesktopBase::IsValid()
{
    return this->DynamicLibraryHandle != nullptr;
}

void *FEOSRuntimePlatformDesktopBase::GetSystemInitializeOptions()
{
    return nullptr;
}

#if EOS_VERSION_AT_LEAST(1, 13, 0)
EOS_Platform_RTCOptions *FEOSRuntimePlatformDesktopBase::GetRTCOptions()
{
#if PLATFORM_WINDOWS
    if (this->XAudio29DllPathAllocated == nullptr)
    {
        return nullptr;
    }
#endif

    return &this->RTCOptions;
}
#endif

FString FEOSRuntimePlatformDesktopBase::GetCacheDirectory()
{
    FString Path = FPaths::ProjectPersistentDownloadDir() / TEXT("EOSCache");
    Path = FPaths::ConvertRelativePathToFull(Path);

    UE_LOG(LogEOSPlatformDefault, Verbose, TEXT("Using the following path as the cache directory: %s"), *Path);
    IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (!PlatformFile.DirectoryExists(*Path))
    {
        PlatformFile.CreateDirectory(*Path);
    }
    return Path;
}

#if !UE_BUILD_SHIPPING
FString FEOSRuntimePlatformDesktopBase::GetPathToEASAutomatedTestingCredentials()
{
    return TEXT("");
}
#endif

const TArray<TSharedPtr<IEOSRuntimePlatformIntegration>> &FEOSRuntimePlatformDesktopBase::GetIntegrations() const
{
    return this->Integrations;
}

bool FEOSRuntimePlatformDesktopBase::UseCrossPlatformFriendStorage() const
{
    return true;
}

#undef LOCTEXT_NAMESPACE

#endif