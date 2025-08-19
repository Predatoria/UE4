// Copyright June Rhodes. All Rights Reserved.

#include "AuthorizeLauncher.h"

#include "DevAuthToolLauncher.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/FileHelper.h"
#include "Misc/MessageDialog.h"
#include "Misc/Paths.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "RedpointEOSEditorModule.h"

#define LOCTEXT_NAMESPACE "OnlineSubsystemEOSEditorModule"

#if PLATFORM_WINDOWS && EOS_VERSION_AT_LEAST(1, 15, 0)

bool FAuthorizeLauncher::bInProgress = false;

bool FAuthorizeLauncher::CanLaunch()
{
    FString ProductId;
    GConfig->GetString(TEXT("EpicOnlineServices"), TEXT("ProductId"), ProductId, GEngineIni);

    if (ProductId.IsEmpty())
    {
        // Can't do authorization flow if the developer hasn't set things up in Project Settings.
        return false;
    }

    return !FAuthorizeLauncher::bInProgress;
}

bool FAuthorizeLauncher::Launch(bool bInteractive)
{
    FAuthorizeLauncher::bInProgress = true;

    if (bInteractive)
    {
        FText AuthorizationTitle = LOCTEXT("AuthorizationTitle", "Authorizing Epic Games accounts");

        FMessageDialog::Open(
            EAppMsgType::Ok,
            LOCTEXT(
                "AuthorizationIntro",
                "Before you can use Epic Games accounts with the Developer Authentication Tool, you have to authorize "
                "them through this flow.\n\nThis flow will launch a standalone build of this project with the EOS "
                "overlay, which you can use to sign into the Epic Games account you want to use.\n\nThis flow only "
                "needs to be done ONCE PER ACCOUNT. After you've done this for an account, you can use the account in "
                "the Developer Authentication Tool without doing the authorization again.\n\nTo authorize multiple "
                "accounts, start the authorization again after you authorize each account.\n\nRefer to the "
                "documentation for more information."),
            &AuthorizationTitle);
    }

    FString WorkingDirectory = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(
        *((IPluginManager::Get().FindPlugin("OnlineSubsystemRedpointEOS")->GetBaseDir() / TEXT("Resources"))
              .Replace(TEXT("/"), TEXT("\\"))));

    FString AuthorizerBat = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(
        *((WorkingDirectory / TEXT("LaunchAccountAuthorizer.bat")).Replace(TEXT("/"), TEXT("\\"))));
    FString EditorBinary = FPlatformProcess::ExecutablePath();
    FString ProjectPath = FPaths::GetProjectFilePath();
    FString SdkLocation = FDevAuthToolLauncher::GetPathToToolsFolder() / TEXT("..");
    FString ProductId;
    GConfig->GetString(TEXT("EpicOnlineServices"), TEXT("ProductId"), ProductId, GEngineIni);
    FString LogFilePath = FPaths::ProjectSavedDir() / "EOSAuthorizationLog.txt";

    if (FPaths::FileExists(LogFilePath))
    {
        IFileManager &FileManager = IFileManager::Get();
        if (!FileManager.Delete(*LogFilePath, false, true, false))
        {
            FText AuthorizationTitle = LOCTEXT("AuthorizationTitle", "Authorizing Epic Games accounts");
            FFormatNamedArguments FormatArguments;
            FormatArguments.Add(TEXT("LogPath"), FText::FromString(LogFilePath));
            FMessageDialog::Open(
                EAppMsgType::Ok,
                FText::Format(
                    LOCTEXT(
                        "AuthorizationInProgress",
                        "Unable to delete the log file at {LogPath}. Make sure there isn't already another copy of the "
                        "authorization flow running."),
                    FormatArguments),
                &AuthorizationTitle);
            FAuthorizeLauncher::bInProgress = false;
            return false;
        }
    }

    FString FinalArguments = FString::Printf(
        TEXT("\"%s\" \"%s\" \"%s\" \"%s\" \"%s\""),
        *EditorBinary,
        *ProjectPath,
        *SdkLocation,
        *ProductId,
        *LogFilePath);

    UE_LOG(LogEOSEditor, Verbose, TEXT("Executing: %s"), *AuthorizerBat);
    UE_LOG(LogEOSEditor, Verbose, TEXT("With arguments: %s"), *FinalArguments);
    UE_LOG(LogEOSEditor, Verbose, TEXT("In working directory: %s"), *WorkingDirectory);

    int32 ReturnCode;
    FString Stdout;
    FString Stderr;
    FPlatformProcess::ExecProcess(*AuthorizerBat, *FinalArguments, &ReturnCode, &Stdout, &Stderr, nullptr);

    UE_LOG(LogEOSEditor, Verbose, TEXT("Authorizer return code: %d"), ReturnCode);
    UE_LOG(LogEOSEditor, Verbose, TEXT("Authorizer stdout: %s"), *Stdout);
    UE_LOG(LogEOSEditor, Verbose, TEXT("Authorizer stderr: %s"), *Stderr);

    if (bInteractive)
    {
        FText AuthorizationTitle = LOCTEXT("AuthorizationResult", "Authorization result");

        bool bSucceeded = false;
        TArray<FString> LogErrors;
        FString LogContents;
        if (FFileHelper::LoadFileToString(LogContents, *LogFilePath))
        {
            TArray<FString> LogLines;
            LogContents.ParseIntoArrayLines(LogLines, true);

            for (const auto &Line : LogLines)
            {
                if (Line.Contains(TEXT("LogEOS: Error: ")) || Line.Contains(TEXT("LogEOS: Warning: ")))
                {
                    LogErrors.Add(Line);
                }

                if (Line.Contains(TEXT("Authentication success: ")))
                {
                    bSucceeded = true;
                }
            }

            if (bSucceeded)
            {
                FMessageDialog::Open(
                    EAppMsgType::Ok,
                    LOCTEXT(
                        "AuthorizationSuccess",
                        "Success! The Epic Games account you signed into can now be used with the Developer "
                        "Authentication Tool."),
                    &AuthorizationTitle);
            }
            else
            {
                FFormatNamedArguments FormatArguments;
                FormatArguments.Add(TEXT("LogErrors"), FText::FromString(FString::Join(LogErrors, TEXT("\n"))));
                FMessageDialog::Open(
                    EAppMsgType::Ok,
                    FText::Format(
                        LOCTEXT(
                            "AuthorizationFailure",
                            "Oops! It looks like authorization ran into one or more errors:\n\n{LogErrors}\n\nIf you "
                            "contact support about this error, please attach the EOSAuthorizationLog.txt file "
                            "underneath the Saved/ directory in your project folder to your support request."),
                        FormatArguments),
                    &AuthorizationTitle);
            }
        }
        else
        {
            FMessageDialog::Open(
                EAppMsgType::Ok,
                LOCTEXT(
                    "AuthorizationNoLog",
                    "Oops! It looks like authorization didn't start up correctly as there is no log file present. "
                    "Please contact support."),
                &AuthorizationTitle);
        }
    }

    FAuthorizeLauncher::bInProgress = false;

    return true;
}

#endif