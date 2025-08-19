// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if defined(EOS_IS_FREE_EDITION)

class FLicenseAgreementWindow
{
private:
    TSharedPtr<class SWindow> LicenseAgreementWindow;
    TSharedPtr<class STextBlock> LicenseAgreementTextBox;
    bool LicenseAgreementLoaded;

    FReply ViewLicenseTermsOnWebsite();
    FReply AcceptLicenseTerms();
    FReply RejectLicenseTerms();

public:
    FLicenseAgreementWindow()
        : LicenseAgreementWindow(nullptr)
        , LicenseAgreementTextBox(nullptr)
        , LicenseAgreementLoaded(false){};
    bool Open();
};

#endif