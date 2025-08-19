// Copyright June Rhodes. All Rights Reserved.

#include "EOSRuntimePlatformAndroid.h"

#include "CoreMinimal.h"

#if PLATFORM_ANDROID

#include "Android/AndroidApplication.h"
#include "EOSRuntimePlatformIntegrationGoogle.h"
#include "EOSRuntimePlatformIntegrationOculus.h"
#include "GenericPlatform/GenericPlatformFile.h"
#if defined(UE_5_0_OR_LATER)
#include "HAL/PlatformFileManager.h"
#else
#include "HAL/PlatformFilemanager.h"
#endif // #if defined(UE_5_0_OR_LATER)
#include "HAL/PlatformProcess.h"
#include "Interfaces/IPluginManager.h"
#include "LogEOSPlatformDefault.h"
#include "Misc/App.h"
#include "Misc/MessageDialog.h"
#include "Misc/Paths.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"

#define LOCTEXT_NAMESPACE "FOnlineSubsystemRedpointEOSModule"

const char *FEOSRuntimePlatformAndroid::InternalPath = nullptr;
const char *FEOSRuntimePlatformAndroid::ExternalPath = nullptr;

void FEOSRuntimePlatformAndroid::Load()
{
#if EOS_VERSION_AT_LEAST(1, 13, 0)
    this->RTCOptions = EOS_Platform_RTCOptions{};
    this->RTCOptions.ApiVersion = EOS_PLATFORM_RTCOPTIONS_API_LATEST;
#endif

#if EOS_GOOGLE_ENABLED
    this->Integrations.Add(MakeShared<FEOSRuntimePlatformIntegrationGoogle>());
#endif
#if EOS_OCULUS_ENABLED
    this->Integrations.Add(MakeShared<FEOSRuntimePlatformIntegrationOculus>());
#endif
}

void FEOSRuntimePlatformAndroid::Unload()
{
    this->Integrations.Empty();
}

bool FEOSRuntimePlatformAndroid::IsValid()
{
    return true;
}

void *FEOSRuntimePlatformAndroid::GetSystemInitializeOptions()
{
    if (this->Opts.IsValid())
    {
        return this->Opts.Get();
    }

#if EOS_VERSION_AT_MOST(1, 10, 3)
    JNIEnv *JEnv = AndroidJavaEnv::GetJavaEnv();
    JavaVM *VM = nullptr;
    JEnv->GetJavaVM(&VM);
#endif

    this->Opts = MakeShared<EOS_Android_InitializeOptions>();
    this->Opts->ApiVersion = EOS_ANDROID_INITIALIZEOPTIONS_API_LATEST;
#if EOS_VERSION_AT_MOST(1, 10, 3)
    this->Opts->VM = VM;
#else
    this->Opts->Reserved = nullptr;
#endif
    checkf(
        FEOSRuntimePlatformAndroid::InternalPath != nullptr && FEOSRuntimePlatformAndroid::ExternalPath != nullptr,
        TEXT("InternalPath/ExternalPath has not been set on Android, this is a bug in the EOS Online Subsystem "
             "plugin!"));
    this->Opts->OptionalInternalDirectory = FEOSRuntimePlatformAndroid::InternalPath;
    this->Opts->OptionalExternalDirectory = FEOSRuntimePlatformAndroid::ExternalPath;
    return this->Opts.Get();
}

#if EOS_VERSION_AT_LEAST(1, 13, 0)
EOS_Platform_RTCOptions *FEOSRuntimePlatformAndroid::GetRTCOptions()
{
    return &this->RTCOptions;
}
#endif

FString FEOSRuntimePlatformAndroid::GetCacheDirectory()
{
    FString Path = FPaths::ProjectPersistentDownloadDir() / TEXT("EOSCache");

    UE_LOG(LogEOSPlatformDefault, Verbose, TEXT("Using the following path as the cache directory: %s"), *Path);
    IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (!PlatformFile.DirectoryExists(*Path))
    {
        PlatformFile.CreateDirectory(*Path);
    }
    return Path;
}

#if !UE_BUILD_SHIPPING
FString FEOSRuntimePlatformAndroid::GetPathToEASAutomatedTestingCredentials()
{
    return FString::Printf(TEXT("%s/Binaries/Android/Credentials.json"), FApp::GetProjectName());
}
#endif

#if EOS_HAS_UNREAL_ANDROID_NAMESPACE
JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeSetEOSCacheDirectories(
    JNIEnv *env,
    jobject /* this */,
    jstring InternalPath,
    jstring ExternalPath)
#else
JNI_METHOD void Java_com_epicgames_ue4_GameActivity_nativeSetEOSCacheDirectories(
    JNIEnv *env,
    jobject /* this */,
    jstring InternalPath,
    jstring ExternalPath)
#endif
{
    FString InternalPathStr = FJavaHelper::FStringFromParam(env, InternalPath);
    FString ExternalPathStr = FJavaHelper::FStringFromParam(env, ExternalPath);

    FPlatformMisc::LowLevelOutputDebugStringf(TEXT("Received internal directory path from JNI: %s"), *InternalPathStr);
    FPlatformMisc::LowLevelOutputDebugStringf(TEXT("Received external directory path from JNI: %s"), *ExternalPathStr);

    checkf(
        FEOSRuntimePlatformAndroid::InternalPath == nullptr && FEOSRuntimePlatformAndroid::ExternalPath == nullptr,
        TEXT("SetEOSCacheDirectories already called, this is a bug in EOS Online Subsystem plugin."));

    checkf(
        EOSString_Android_InitializeOptions_Directory::AllocateToCharBuffer(
            InternalPathStr,
            FEOSRuntimePlatformAndroid::InternalPath) == EOS_EResult::EOS_Success,
        TEXT("Unable to store path %s as internal directory on startup; this is a bug in the EOS Online Subsystem "
             "plugin."),
        *InternalPathStr);
    checkf(
        EOSString_Android_InitializeOptions_Directory::AllocateToCharBuffer(
            ExternalPathStr,
            FEOSRuntimePlatformAndroid::ExternalPath) == EOS_EResult::EOS_Success,
        TEXT("Unable to store path %s as external directory on startup; this is a bug in the EOS Online Subsystem "
             "plugin."),
        *ExternalPathStr);

    FPlatformMisc::LowLevelOutputDebugStringf(
        TEXT("Will use the following internal cache directory on Android: %s"),
        ANSI_TO_TCHAR(FEOSRuntimePlatformAndroid::InternalPath));
    FPlatformMisc::LowLevelOutputDebugStringf(
        TEXT("Will use the following external cache directory on Android: %s"),
        ANSI_TO_TCHAR(FEOSRuntimePlatformAndroid::ExternalPath));
}

const TArray<TSharedPtr<IEOSRuntimePlatformIntegration>> &FEOSRuntimePlatformAndroid::GetIntegrations() const
{
    return this->Integrations;
}

bool FEOSRuntimePlatformAndroid::UseCrossPlatformFriendStorage() const
{
    return true;
}

#undef LOCTEXT_NAMESPACE

#endif