// Copyright June Rhodes. All Rights Reserved.

#include "VerifyFreeEditionLicenseCommandlet.h"

#include "../RedpointEOSEditorModule.h"
#include "Misc/Parse.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSLicenseValidator.h"

UVerifyFreeEditionLicenseCommandlet::UVerifyFreeEditionLicenseCommandlet(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;

    LogToConsole = true;
    ShowErrorCount = true;
    ShowProgress = true;
}

int32 UVerifyFreeEditionLicenseCommandlet::Main(const FString &Params)
{
#if !defined(EOS_IS_FREE_EDITION)
    UE_LOG(
        LogEOSEditor,
        Error,
        TEXT("This build of EOS Online Subsystem is not the Free Edition, so license keys can not be validated."));
    return 1;
#else
    FString LicenseKey;
    if (!FParse::Value(*Params, TEXT("-LICENSEKEY="), LicenseKey))
    {
        UE_LOG(LogEOSEditor, Error, TEXT("Missing -LICENSEKEY= parameter."));
        return 1;
    }

    bool bSuccessful = false;

    auto Validator = FEOSLicenseValidator::GetInstance();
    Validator->ValidateLicenseKey(
        LicenseKey,
        [&bSuccessful]() {
            bSuccessful = false;
            RequestEngineExit(FString::Printf(TEXT("License failed to verify!")));
        },
        [&bSuccessful]() {
            bSuccessful = true;
            RequestEngineExit(FString::Printf(TEXT("License verified successfully!")));
        },
        []() {
        });

    if (!IsEngineExitRequested())
    {
        GIsRunning = true;
        while (GIsRunning && !IsEngineExitRequested())
        {
            GEngine->UpdateTimeAndHandleMaxTickRate();
            GEngine->Tick(FApp::GetDeltaTime(), false);
            GFrameCounter++;
            FTSTicker::GetCoreTicker().Tick(FApp::GetDeltaTime());
            FPlatformProcess::Sleep(0);
        }
        GIsRunning = false;
    }

    return bSuccessful ? 0 : 1;
#endif
}