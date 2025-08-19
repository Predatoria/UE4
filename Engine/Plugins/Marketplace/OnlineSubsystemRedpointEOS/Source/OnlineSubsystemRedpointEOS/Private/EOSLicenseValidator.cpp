// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/EOSLicenseValidator.h"
#include "Misc/FileHelper.h"

#if defined(EOS_IS_FREE_EDITION)

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"

EOS_ENABLE_STRICT_WARNINGS

TSharedPtr<FEOSLicenseValidator> FEOSLicenseValidator::Instance = nullptr;

FEOSLicenseValidator::FEOSLicenseValidator()
    : CheckMutex()
    , bHasChecked(false)
    , bHasValidLicense(false)
    , LastCheckedLicenseKey()
{
}

TSharedRef<FEOSLicenseValidator> FEOSLicenseValidator::GetInstance()
{
    if (!FEOSLicenseValidator::Instance.IsValid())
    {
        FEOSLicenseValidator::Instance = MakeShareable(new FEOSLicenseValidator());
    }

    return FEOSLicenseValidator::Instance.ToSharedRef();
}

void FEOSLicenseValidator::ValidateLicenseKey(
    const FString &LicenseKey,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FLicenseValidatorCallback OnInvalidLicense,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FLicenseValidatorCallback OnValidLicense,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    std::function<void()> MutexRelease)
{
    // Otherwise, send a HTTP request to the endpoint.
    UE_LOG(LogEOSLicenseValidation, Verbose, TEXT("Sending request to validation server."));
    auto HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(TEXT("https://licensing.redpoint.games/api/validate"));
    HttpRequest->SetHeader("Authorization", FString::Printf(TEXT("Bearer %s"), *LicenseKey));
    HttpRequest->SetHeader("Audience", TEXT("epic-online-subsystem-free"));
    HttpRequest->SetVerb(TEXT("GET"));
    HttpRequest->SetHeader(TEXT("Content-Length"), TEXT("0"));
    HttpRequest->OnProcessRequestComplete().BindLambda(
        [This = this->AsShared(), OnValidLicense, OnInvalidLicense, LicenseKey, MutexRelease](
            // NOLINTNEXTLINE(performance-unnecessary-value-param)
            FHttpRequestPtr HttpRequest,
            // NOLINTNEXTLINE(performance-unnecessary-value-param)
            FHttpResponsePtr HttpResponse,
            bool bSucceeded) {
            // If we fail to get a response from the licensing server, assume the license key is valid. This
            // prevents outages from impacting games.
            if (!HttpResponse.IsValid() || HttpResponse->GetResponseCode() != 200)
            {
                UE_LOG(
                    LogEOSLicenseValidation,
                    Warning,
                    TEXT("Could not get valid response from licensing server. Assuming license is valid."));
                This->bHasValidLicense = true;
                This->bHasChecked = true;
                This->LastCheckedLicenseKey = LicenseKey;
                MutexRelease();
                OnValidLicense();
                return;
            }

            FString Result = HttpResponse->GetContentAsString();
            if (Result == TEXT("missing_authorization_header") || Result == TEXT("expected_bearer_token") ||
                Result == TEXT("missing_audience_header") || Result == TEXT("expected_audience") ||
                Result == TEXT("token_validation_failed"))
            {
                // Developer has not configured the product correctly (e.g. no license key provided or it wasn't
                // cryptographically valid).
                UE_LOG(
                    LogEOSLicenseValidation,
                    Error,
                    TEXT("License key validation failed with error code '%s'. Check that you have set the "
                         "license key in the configuration. You can get a license key from "
                         "https://licensing.redpoint.games/get/eos-online-subsystem-free."),
                    *Result);
                This->bHasValidLicense = false;
                This->bHasChecked = true;
                This->LastCheckedLicenseKey = LicenseKey;
                MutexRelease();
                OnInvalidLicense();
                return;
            }

            if (Result == TEXT("wrong_product_for_license_key"))
            {
                // Developer has used the wrong type of license key for this product.
                UE_LOG(
                    LogEOSLicenseValidation,
                    Error,
                    TEXT("License key validation failed because the license key is not for this type of "
                         "product. Go to https://licensing.redpoint.games/get/eos-online-subsystem-free to "
                         "obtain a license key for EOS Online Subsystem (Free Edition)."));
                This->bHasValidLicense = false;
                This->bHasChecked = true;
                This->LastCheckedLicenseKey = LicenseKey;
                MutexRelease();
                OnInvalidLicense();
                return;
            }

            if (Result == TEXT("license_key_revoked"))
            {
                // The license key has been revoked.
                UE_LOG(
                    LogEOSLicenseValidation,
                    Error,
                    TEXT("License key validation failed because the license key has been revoked. If you feel "
                         "this is in error, please contact support."));
                This->bHasValidLicense = false;
                This->bHasChecked = true;
                This->LastCheckedLicenseKey = LicenseKey;
                MutexRelease();
                OnInvalidLicense();
                return;
            }

            if (Result == TEXT("license_ok"))
            {
                // The license key is valid.
                UE_LOG(LogEOSLicenseValidation, Verbose, TEXT("License key validation succeeded."));
                This->bHasValidLicense = true;
                This->bHasChecked = true;
                This->LastCheckedLicenseKey = LicenseKey;
                MutexRelease();
                OnValidLicense();
                return;
            }

            // For everything else, including "internal_error", we assume the license key is valid. This
            // prevents validation server bugs from impacting games.
            UE_LOG(
                LogEOSLicenseValidation,
                Warning,
                TEXT("Could not get valid response from licensing server. Assuming license is valid."));
            This->bHasValidLicense = true;
            This->bHasChecked = true;
            This->LastCheckedLicenseKey = LicenseKey;
            MutexRelease();
            OnValidLicense();
        });
    HttpRequest->ProcessRequest();
}

void FEOSLicenseValidator::ValidateLicense(
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<FEOSConfig> Config,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FLicenseValidatorCallback OnInvalidLicense,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FLicenseValidatorCallback OnValidLicense)
{
#if WITH_EDITOR
#if PLATFORM_WINDOWS
    FString FlagPath = FString(FPlatformMisc::GetEnvironmentVariable(TEXT("USERPROFILE"))) / ".eos-free-edition-agreed";
    FString VersionPath =
        FString(FPlatformMisc::GetEnvironmentVariable(TEXT("USERPROFILE"))) / ".eos-free-edition-latest-version";
#else
    FString FlagPath = FString(FPlatformMisc::GetEnvironmentVariable(TEXT("HOME"))) / ".eos-free-edition-agreed";
    FString VersionPath =
        FString(FPlatformMisc::GetEnvironmentVariable(TEXT("HOME"))) / ".eos-free-edition-latest-version";
#endif

    if (!FPaths::FileExists(FlagPath))
    {
        // Have not yet accepted license agreement in editor.
        UE_LOG(
            LogEOSLicenseValidation,
            Error,
            TEXT("You need to accept the license agreement in the editor before this operation will succeed."))
        OnInvalidLicense();
        return;
    }

#if defined(EOS_BUILD_VERSION_NAME)
    if (FPaths::FileExists(VersionPath))
    {
        FString AllowedVersionString;
        if (FFileHelper::LoadFileToString(AllowedVersionString, *VersionPath))
        {
            TArray<FString> AllowedVersions;
            AllowedVersionString.ParseIntoArrayLines(AllowedVersions, true);
            if (AllowedVersions.Num() > 0)
            {
                bool bAllowedVersionFound = false;
                for (auto AllowedVersion : AllowedVersions)
                {
                    if (AllowedVersion == FString(EOS_BUILD_VERSION_NAME))
                    {
                        bAllowedVersionFound = true;
                        break;
                    }
                }

                if (!bAllowedVersionFound)
                {
                    // Version too old.
                    UE_LOG(
                        LogEOSLicenseValidation,
                        Error,
                        TEXT("Your version of EOS Online Subsystem Free Edition is too old. Please upgrade to the "
                             "latest version at https://licensing.redpoint.games/get/eos-online-subsystem-free. This "
                             "version check only applies in the editor, and does not apply to packaged games."))
                    OnInvalidLicense();
                    return;
                }
            }
        }
    }
#endif
#endif

    // If we have already checked in a previous request, don't do it again.
    FString LicenseKey = Config->GetFreeEditionLicenseKey().TrimStartAndEnd();
    if (this->bHasChecked && LastCheckedLicenseKey == LicenseKey)
    {
        if (this->bHasValidLicense)
        {
            OnValidLicense();
        }
        else
        {
            OnInvalidLicense();
        }
        return;
    }

    // Perform the validation check in a mutex so we don't send more than one license check if
    // we're performing concurrent logins.
    this->CheckMutex.Run([This = this->AsShared(), Config, LicenseKey, OnInvalidLicense, OnValidLicense](
                             // NOLINTNEXTLINE(performance-unnecessary-value-param)
                             std::function<void()> MutexRelease) {
        // Test again to see if we've checked, since we may get concurrent requests queued up
        // and still we only want to make one license check.
        if (This->bHasChecked && This->LastCheckedLicenseKey == LicenseKey)
        {
            // Release early; it doesn't need to be held while the valid/invalid callbacks run.
            MutexRelease();

            if (This->bHasValidLicense)
            {
                OnValidLicense();
            }
            else
            {
                OnInvalidLicense();
            }
            return;
        }

        // Get the license key from configuration.
        if (LicenseKey.IsEmpty())
        {
            // Automatic failure; the developer hasn't specified a license key.
            UE_LOG(
                LogEOSLicenseValidation,
                Error,
                TEXT("Missing license key for Online Subsystem EOS Free Edition. Set the FreeEditionLicenseKey= "
                     "value in DefaultEngine.ini. You can get a license key from "
                     "https://licensing.redpoint.games/get/eos-online-subsystem-free."))
            This->bHasChecked = true;
            This->bHasValidLicense = false;
            This->LastCheckedLicenseKey = LicenseKey;
            MutexRelease();
            OnInvalidLicense();
            return;
        }

        This->ValidateLicenseKey(LicenseKey, OnInvalidLicense, OnValidLicense, MutexRelease);
    });
}

EOS_DISABLE_STRICT_WARNINGS

#endif