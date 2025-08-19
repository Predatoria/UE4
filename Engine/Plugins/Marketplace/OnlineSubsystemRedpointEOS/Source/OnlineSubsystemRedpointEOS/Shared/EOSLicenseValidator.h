// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if defined(EOS_IS_FREE_EDITION)

#include "AsyncMutex.h"
#include "CoreMinimal.h"
#include "EOSConfig.h"
#include <functional>

EOS_ENABLE_STRICT_WARNINGS

typedef std::function<void()> FLicenseValidatorCallback;

class ONLINESUBSYSTEMREDPOINTEOS_API FEOSLicenseValidator : public TSharedFromThis<FEOSLicenseValidator>
{
    friend class UVerifyFreeEditionLicenseCommandlet;

private:
    static TSharedPtr<FEOSLicenseValidator> Instance;

    FAsyncMutex CheckMutex;
    bool bHasChecked;
    bool bHasValidLicense;
    FString LastCheckedLicenseKey;

    FEOSLicenseValidator();

public:
    static TSharedRef<FEOSLicenseValidator> GetInstance();
    UE_NONCOPYABLE(FEOSLicenseValidator);

    void ValidateLicenseKey(
        const FString &LicenseKey,
        FLicenseValidatorCallback OnInvalidLicense,
        FLicenseValidatorCallback OnValidLicense,
        std::function<void()> MutexRelease);

    void ValidateLicense(
        TSharedRef<FEOSConfig> Config,
        FLicenseValidatorCallback OnInvalidLicense,
        FLicenseValidatorCallback OnValidLicense);
};

EOS_DISABLE_STRICT_WARNINGS

#endif