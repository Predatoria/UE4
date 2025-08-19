// Copyright June Rhodes. All Rights Reserved.

#include "DevAuthToolLauncher.h"

#include "Interfaces/IPluginManager.h"
#include "Misc/FileHelper.h"
#include "Misc/MessageDialog.h"
#include "Misc/Paths.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "RedpointEOSEditorModule.h"

#define LOCTEXT_NAMESPACE "OnlineSubsystemEOSEditorModule"

#if PLATFORM_WINDOWS || PLATFORM_MAC

// These are set to their default values in FRedpointEOSEditorModule::StartupModule()
bool FDevAuthToolLauncher::bNeedsToCheckIfDevToolRunning;
bool FDevAuthToolLauncher::bIsDevToolRunning;
FUTickerDelegateHandle FDevAuthToolLauncher::ResetCheckHandle;

FString FDevAuthToolLauncher::GetPathToToolsFolder()
{
    TArray<FString> SearchPaths;
    FString BaseDir = IPluginManager::Get().FindPlugin("OnlineSubsystemRedpointEOS")->GetBaseDir();

    FString ToolsFolder = FString(TEXT("Tools"));

#if PLATFORM_WINDOWS
    FString CommonAppData = FPlatformMisc::GetEnvironmentVariable(TEXT("PROGRAMDATA"));
    FString UserProfile = FPlatformMisc::GetEnvironmentVariable(TEXT("USERPROFILE"));
    FString SystemDrive = FPlatformMisc::GetEnvironmentVariable(TEXT("SYSTEMDRIVE"));

    SearchPaths.Add(FPaths::ConvertRelativePathToFull(BaseDir / TEXT("Source") / TEXT("ThirdParty") / ToolsFolder));
    SearchPaths.Add(FPaths::ConvertRelativePathToFull(
        BaseDir / TEXT("..") / TEXT("..") / TEXT("EOS-SDK-" EOS_SDK_LINK_VERSION) / TEXT("SDK") / ToolsFolder));
    if (!CommonAppData.IsEmpty())
    {
        SearchPaths.Add(CommonAppData / TEXT("EOS-SDK-" EOS_SDK_LINK_VERSION) / TEXT("SDK") / ToolsFolder);
    }
    if (!UserProfile.IsEmpty())
    {
        SearchPaths.Add(
            UserProfile / TEXT("Downloads") / TEXT("EOS-SDK-" EOS_SDK_LINK_VERSION) / TEXT("SDK") / ToolsFolder);
    }
    if (!SystemDrive.IsEmpty())
    {
        SearchPaths.Add(SystemDrive / TEXT("EOS-SDK-" EOS_SDK_LINK_VERSION) / TEXT("SDK") / ToolsFolder);
    }
#else
    FString Home = FPlatformMisc::GetEnvironmentVariable(TEXT("HOME"));

    SearchPaths.Add(FPaths::ConvertRelativePathToFull(BaseDir / TEXT("Source") / TEXT("ThirdParty") / ToolsFolder));
    if (!Home.IsEmpty())
    {
        SearchPaths.Add(Home / TEXT("Downloads") / TEXT("EOS-SDK-" EOS_SDK_LINK_VERSION) / TEXT("SDK") / ToolsFolder);
    }
#endif

#if PLATFORM_WINDOWS
    TArray<FString> ZipNames;
    ZipNames.Add(TEXT("EOS_DevAuthTool-win32-x64-1.1.0.zip"));
    ZipNames.Add(TEXT("EOS_DevAuthTool-win32-x64-1.0.1.zip"));
#else
    TArray<FString> ZipNames;
    ZipNames.Add(TEXT("EOS_DevAuthTool-darwin-x64-1.1.0.zip"));
    ZipNames.Add(TEXT("EOS_DevAuthTool-darwin-x64-1.0.1.zip"));
#endif

    for (const auto &SearchPath : SearchPaths)
    {
#if PLATFORM_WINDOWS
        if (FPaths::FileExists(SearchPath / TEXT("DevAuthTool") / TEXT("EOS_DevAuthTool.exe")))
        {
            return SearchPath.Replace(TEXT("/"), TEXT("\\"));
        }
        for (const auto &ZipName : ZipNames)
        {
            if (FPaths::FileExists(SearchPath / ZipName))
            {
                return SearchPath.Replace(TEXT("/"), TEXT("\\"));
            }
        }
#else
        if (FPaths::FileExists(
                SearchPath / TEXT("EOS_DevAuthTool.app") / TEXT("Contents") / TEXT("MacOS") / TEXT("EOS_DevToolApp")))
        {
            return SearchPath;
        }
        for (const auto &ZipName : ZipNames)
        {
            if (FPaths::FileExists(SearchPath / ZipName))
            {
                return SearchPath;
            }
        }
#endif
    }

    return TEXT("");
}

bool FDevAuthToolLauncher::IsDevAuthToolRunning()
{
    // If the project is configured to use a remote host (not localhost) as the developer
    // tool address, then we assume it is running on the remote machine.
    // NOTE: We could try to do full host:port parsing here, but it's complicated
    // with IPv6 and Unreal doesn't have any good built-in utilities to do it.
    if (!FEOSConfigIni().GetDeveloperToolAddress().StartsWith("localhost:"))
    {
        return true;
    }

    if (!bNeedsToCheckIfDevToolRunning)
    {
        return bIsDevToolRunning;
    }

    ResetCheckHandle = FUTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateLambda([](float DeltaTime) {
            bNeedsToCheckIfDevToolRunning = true;
            return false;
        }),
        10.0f);

#if PLATFORM_WINDOWS
    UE_LOG(LogEOSEditor, Verbose, TEXT("Checking if EOS_DevAuthTool.exe process is running"));
    bIsDevToolRunning = FPlatformProcess::IsApplicationRunning(TEXT("EOS_DevAuthTool.exe"));
#else
    UE_LOG(LogEOSEditor, Verbose, TEXT("Checking if EOS_DevAuthTool process is running"));
    bIsDevToolRunning = FPlatformProcess::IsApplicationRunning(TEXT("EOS_DevAuthTool"));
#endif
    bNeedsToCheckIfDevToolRunning = false;

    return bIsDevToolRunning;
}

bool FDevAuthToolLauncher::Launch(bool bInteractive)
{
    FString Path = FDevAuthToolLauncher::GetPathToToolsFolder();
    if (!Path.IsEmpty())
    {
#if PLATFORM_WINDOWS
        FString ExePath = Path / TEXT("DevAuthTool") / TEXT("EOS_DevAuthTool.exe");
        FString ExeDirPath = Path / TEXT("DevAuthTool");
        TArray<FString> ZipPaths;
        ZipPaths.Add(Path / TEXT("EOS_DevAuthTool-win32-x64-1.1.0.zip"));
        ZipPaths.Add(Path / TEXT("EOS_DevAuthTool-win32-x64-1.0.1.zip"));
#else
        FString ExePath =
            Path / TEXT("EOS_DevAuthTool.app") / TEXT("Contents") / TEXT("MacOS") / TEXT("EOS_DevAuthTool");
        FString ExeDirPath = Path;
        TArray<FString> ZipPaths;
        ZipPaths.Add(Path / TEXT("EOS_DevAuthTool-darwin-x64-1.1.0.zip"));
        ZipPaths.Add(Path / TEXT("EOS_DevAuthTool-darwin-x64-1.0.1.zip"));
        FString LinkPath = Path / TEXT("EOS_DevAuthTool.app") / TEXT("Contents") / TEXT("Frameworks") /
                           TEXT("Electron Framework.framework") / TEXT("Electron Framework");
#endif
        if (!FPaths::FileExists(ExePath)
#if PLATFORM_MAC
            || !FPaths::FileExists(LinkPath)
#endif
        )
        {
            FString FoundZip = TEXT("");
            for (const auto &ZipPath : ZipPaths)
            {
                if (FPaths::FileExists(ZipPath))
                {
                    FoundZip = ZipPath;
                }
            }
            if (FoundZip.IsEmpty())
            {
                UE_LOG(LogEOSEditor, Error, TEXT("Have path to ZIP, but ZIP doesn't exist"));
                return false;
            }

            FText ExtractTitle = LOCTEXT("DevToolPromptTitle", "Extraction required");
            if (bInteractive)
            {
                FMessageDialog::Open(
                    EAppMsgType::Ok,
                    LOCTEXT(
                        "DevToolExtraction",
                        "The Developer Authentication Tool will now be extracted from the SDK."),
                    &ExtractTitle);
            }

#if PLATFORM_WINDOWS
            FString ExtractionDir = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(
                *((IPluginManager::Get().FindPlugin("OnlineSubsystemRedpointEOS")->GetBaseDir() / TEXT("Resources"))
                      .Replace(TEXT("/"), TEXT("\\"))));
            FString ExtractionBat = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(
                *((ExtractionDir / TEXT("ExtractDevAuthTool.bat")).Replace(TEXT("/"), TEXT("\\"))));
#else
            FString ExtractionDir = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(
                *((IPluginManager::Get().FindPlugin("OnlineSubsystemRedpointEOS")->GetBaseDir() / TEXT("Resources"))));
            FString ExtractionBat = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(
                *((ExtractionDir / TEXT("ExtractDevAuthTool.command"))));

            // Check that the script doesn't have Windows line-endings.
            FString ExtractScriptContents;
            if (FFileHelper::LoadFileToString(ExtractScriptContents, *ExtractionBat))
            {
                if (ExtractScriptContents.Find("\r") != -1)
                {
                    UE_LOG(
                        LogEOSEditor,
                        Verbose,
                        TEXT("Automatically fixed up extraction script to use UNIX line-endings."));
                    ExtractScriptContents = ExtractScriptContents.Replace(TEXT("\r"), TEXT(""));
                    FFileHelper::SaveStringToFile(ExtractScriptContents, *ExtractionBat);
                }
            }
#endif

            UE_LOG(LogEOSEditor, Verbose, TEXT("Executing: %s"), *ExtractionBat);
            UE_LOG(LogEOSEditor, Verbose, TEXT("With arguments: %s"), *Path);
            UE_LOG(LogEOSEditor, Verbose, TEXT("In working directory: %s"), *ExtractionDir);

            int32 ReturnCode;
            FString Stdout;
            FString Stderr;
#if PLATFORM_WINDOWS
            FPlatformProcess::ExecProcess(*ExtractionBat, *Path, &ReturnCode, &Stdout, &Stderr, *ExtractionDir);
#else
            FString BashArgs = FString::Printf(TEXT("%s \"%s\""), *ExtractionBat, *Path);
            FPlatformProcess::ExecProcess(TEXT("/bin/bash"), *BashArgs, &ReturnCode, &Stdout, &Stderr, *ExtractionDir);
#endif
            UE_LOG(LogEOSEditor, Verbose, TEXT("ZIP extraction return code: %d"), ReturnCode);
            UE_LOG(LogEOSEditor, Verbose, TEXT("ZIP extraction stdout: %s"), *Stdout);
            UE_LOG(LogEOSEditor, Verbose, TEXT("ZIP extraction stderr: %s"), *Stderr);

            if (!FPaths::FileExists(ExePath))
            {
                UE_LOG(LogEOSEditor, Error, TEXT("ZIP extraction failed to produce executable at path: %s"), *ExePath);
                return false;
            }

#if PLATFORM_MAC
            if (ReturnCode != 0)
            {
                UE_LOG(LogEOSEditor, Error, TEXT("ZIP extraction exited with non-zero exit code!"));
                return false;
            }
#endif
        }

        FText StartTitle = LOCTEXT("DevToolPromptTitle", "How to use the Developer Authentication Tool");
        if (bInteractive)
        {
            FMessageDialog::Open(
                EAppMsgType::Ok,
                LOCTEXT(
                    "DevToolPrompt",
                    "When prompted, use port 6300.\n\n"
                    "Name each credential Context_1, Context_2, etc.\n\n"
                    "Do not use the same Epic Games account "
                    "more than once.\n\n"
                    "For more information, check the documentation."),
                &StartTitle);
        }

        UE_LOG(LogEOSEditor, Verbose, TEXT("Launching Developer Authentication Tool located at: %s"), *ExePath);
        uint32 ProcID;
#if PLATFORM_WINDOWS
        FPlatformProcess::CreateProc(
            *ExePath,
            TEXT("-port 6300"),
            true,
            false,
            false,
            &ProcID,
            0,
            *ExeDirPath,
            nullptr,
            nullptr);
#else
        FPlatformProcess::CreateProc(
            TEXT("/usr/bin/open"),
            TEXT("-a EOS_DevAuthTool"),
            true,
            false,
            false,
            &ProcID,
            0,
            *ExeDirPath,
            nullptr,
            nullptr);
#endif
        ResetCheckHandle = FUTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateLambda([](float DeltaTime) {
                bNeedsToCheckIfDevToolRunning = true;
                return false;
            }),
            10.0f);
        bIsDevToolRunning = true;
        bNeedsToCheckIfDevToolRunning = false;
    }

    return true;
}

#endif

#undef LOCTEXT_NAMESPACE