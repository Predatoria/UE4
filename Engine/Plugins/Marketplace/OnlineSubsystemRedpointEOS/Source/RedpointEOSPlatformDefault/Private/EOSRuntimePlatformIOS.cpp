// Copyright June Rhodes. All Rights Reserved.

#include "EOSRuntimePlatformIOS.h"

#include "CoreMinimal.h"

#if PLATFORM_IOS

#include "EOSRuntimePlatformIntegrationApple.h"
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

void FEOSRuntimePlatformIOS::Load()
{
#if EOS_VERSION_AT_LEAST(1, 13, 0)
    this->RTCOptions = EOS_Platform_RTCOptions{};
    this->RTCOptions.ApiVersion = EOS_PLATFORM_RTCOPTIONS_API_LATEST;
#endif

#if EOS_APPLE_ENABLED
    this->Integrations.Add(MakeShared<FEOSRuntimePlatformIntegrationApple>());
#endif
}

void FEOSRuntimePlatformIOS::Unload()
{
    this->Integrations.Empty();
}

bool FEOSRuntimePlatformIOS::IsValid()
{
    return true;
}

void *FEOSRuntimePlatformIOS::GetSystemInitializeOptions()
{
    return nullptr;
}

#if EOS_VERSION_AT_LEAST(1, 13, 0)
EOS_Platform_RTCOptions *FEOSRuntimePlatformIOS::GetRTCOptions()
{
    return &this->RTCOptions;
}
#endif

FString FEOSRuntimePlatformIOS::GetCacheDirectory()
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *cachePath = [paths objectAtIndex:0];
    BOOL isDir = NO;
    NSError *error;
    if (![[NSFileManager defaultManager] fileExistsAtPath:cachePath isDirectory:&isDir] && isDir == NO)
    {
        [[NSFileManager defaultManager] createDirectoryAtPath:cachePath
                                  withIntermediateDirectories:YES
                                                   attributes:nil
                                                        error:&error];
    }
    const char *URLStrUTF8 = [cachePath cStringUsingEncoding:NSASCIIStringEncoding];
    FString Path = FString(UTF8_TO_TCHAR(URLStrUTF8)) / TEXT("EOSCache");

    UE_LOG(LogEOSPlatformDefault, Verbose, TEXT("Using the following path as the cache directory: %s"), *Path);
    IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (!PlatformFile.DirectoryExists(*Path))
    {
        PlatformFile.CreateDirectory(*Path);
    }
    return Path;
}

#if !UE_BUILD_SHIPPING
FString FEOSRuntimePlatformIOS::GetPathToEASAutomatedTestingCredentials()
{
    return FString::Printf(TEXT("%s/Binaries/IOS/Credentials.json"), FApp::GetProjectName());
}
#endif

const TArray<TSharedPtr<IEOSRuntimePlatformIntegration>> &FEOSRuntimePlatformIOS::GetIntegrations() const
{
    return this->Integrations;
}

bool FEOSRuntimePlatformIOS::UseCrossPlatformFriendStorage() const
{
    return true;
}

#undef LOCTEXT_NAMESPACE

#endif