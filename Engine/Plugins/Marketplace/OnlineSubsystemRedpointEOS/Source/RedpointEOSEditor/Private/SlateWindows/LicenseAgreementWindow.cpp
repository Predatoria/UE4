// Copyright June Rhodes. All Rights Reserved.

#include "./LicenseAgreementWindow.h"

#if defined(EOS_IS_FREE_EDITION)

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/FileHelper.h"
#include "Widgets/Layout/SScrollBox.h"

#define LOCTEXT_NAMESPACE "OnlineSubsystemEOSEditorModule"

bool FLicenseAgreementWindow::Open()
{
    if (this->LicenseAgreementWindow.IsValid())
    {
        // @todo: Figure out how to focus existing window.
        return false;
    }

    if (!FSlateApplicationBase::IsInitialized())
    {
        // UI not available.
        return false;
    }

    static FWindowStyle DialogStyle = FCoreStyle::Get().GetWidgetStyle<FWindowStyle>("Window");
    DialogStyle.SetBackgroundBrush(DialogStyle.ChildBackgroundBrush);

    this->LicenseAgreementLoaded = false;

    // Create the text box that we will load the license agreement text into.
    this->LicenseAgreementTextBox = SNew(STextBlock)
                                        .Text(LOCTEXT(
                                            "LicenseAgreementFetching",
                                            "Please wait while we fetch the license agreement from the server..."));

    const FVector2D WindowSize(800.f, 600.f);
    this->LicenseAgreementWindow =
        SNew(SWindow)
            .Title(LOCTEXT("EOSLicenseAgreement", "EOS Online Subsystem (Free Edition): License Agreement"))
            .ClientSize(WindowSize)
            .HasCloseButton(false)
            .SupportsMinimize(false)
            .SupportsMaximize(false)
            .FocusWhenFirstShown(true)
            .AutoCenter(EAutoCenter::PreferredWorkArea)
            .Style(&DialogStyle);

    this->LicenseAgreementWindow->SetContent(
        SNew(SVerticalBox) +
        SVerticalBox::Slot().AutoHeight().Padding(FMargin(
            10,
            10,
            10,
            10))[SNew(STextBlock)
                     .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 16))
                     .Text(LOCTEXT("LicenseAgreementTitle", "Please review the license agreement"))] +
        SVerticalBox::Slot().AutoHeight().Padding(FMargin(
            10,
            0,
            10,
            10))[SNew(STextBlock)
                     .AutoWrapText(true)
                     .Text(LOCTEXT(
                         "LicenseAgreementPreamble",
                         "Before you can use EOS Online Subsystem (Free Edition), you must agree to the license."))] +
        SVerticalBox::Slot().FillHeight(1).Padding(FMargin(10, 0, 10, 10))
            [SNew(SScrollBox) +
             SScrollBox::Slot().VAlign(EVerticalAlignment::VAlign_Top)[this->LicenseAgreementTextBox.ToSharedRef()]] +
        SVerticalBox::Slot().AutoHeight().Padding(FMargin(10, 0, 10, 10))
            [SNew(STextBlock)
                 .AutoWrapText(true)
                 .Text(LOCTEXT("LicenseAgreementEnd", "If you agree to the above terms, click 'I accept' below."))] +
        SVerticalBox::Slot()
            .AutoHeight()
            .Padding(FMargin(10, 0, 10, 10))
            .HAlign(EHorizontalAlignment::HAlign_Fill)
                [SNew(SHorizontalBox) +
                 SHorizontalBox::Slot().AutoWidth().Padding(FMargin(0, 0, 10, 0))
                     [SNew(SButton)
                          .ContentPadding(FMargin(10, 5))
                          .Text(LOCTEXT("LicenseAgreementAccept", "I accept"))
                          .IsEnabled_Lambda([this]() {
                              return this->LicenseAgreementLoaded;
                          })
                          .OnClicked(FOnClicked::CreateRaw(this, &FLicenseAgreementWindow::AcceptLicenseTerms))] +
                 SHorizontalBox::Slot().HAlign(EHorizontalAlignment::HAlign_Fill) +
                 SHorizontalBox::Slot().AutoWidth().HAlign(EHorizontalAlignment::HAlign_Right)
                     [SNew(SButton)
                          .ContentPadding(FMargin(10, 5))
                          .Text(LOCTEXT("LicenseAgreementDoNotAccept", "I do not accept (Quit UE4 Editor)"))
                          .OnClicked(FOnClicked::CreateRaw(this, &FLicenseAgreementWindow::RejectLicenseTerms))]]);

    this->LicenseAgreementWindow->SetOnWindowClosed(FOnWindowClosed::CreateLambda([this](const TSharedRef<SWindow> &) {
        this->LicenseAgreementWindow.Reset();
    }));

    FSlateApplication::Get().AddWindow(this->LicenseAgreementWindow.ToSharedRef(), true);

    auto HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(TEXT("https://redpoint.games/eos-online-subsystem-free-eula/eula.txt"));
    HttpRequest->SetVerb(TEXT("GET"));
    HttpRequest->SetHeader(TEXT("Content-Length"), TEXT("0"));
    HttpRequest->OnProcessRequestComplete().BindLambda(
        // NOLINTNEXTLINE(performance-unnecessary-value-param)
        [this](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded) {
            if (HttpResponse.IsValid())
            {
                FString LicenseAgreementText = HttpResponse->GetContentAsString().TrimStartAndEnd();
                if (LicenseAgreementText.IsEmpty())
                {
                    this->LicenseAgreementTextBox->SetText(FText::FromString(
                        TEXT("Unable to load the license agreement from the server. Please view the license agreement "
                             "online at: https://redpoint.games/eos-online-subsystem-free-eula/.\n\nBy clicking 'I "
                             "accept' you acknowledge you've read the terms on the website and agree to them.")));
                }
                else
                {
                    this->LicenseAgreementTextBox->SetText(FText::FromString(LicenseAgreementText));
                }
                this->LicenseAgreementLoaded = true;
            }
        });
    HttpRequest->ProcessRequest();

    return true;
}

FReply FLicenseAgreementWindow::ViewLicenseTermsOnWebsite()
{
    if (this->LicenseAgreementWindow.IsValid())
    {
        FPlatformProcess::LaunchURL(TEXT("https://redpoint.games/eos-online-subsystem-free-eula/"), nullptr, nullptr);
    }

    return FReply::Handled();
}

FReply FLicenseAgreementWindow::AcceptLicenseTerms()
{
    if (this->LicenseAgreementWindow.IsValid())
    {
#if PLATFORM_WINDOWS
        FString FlagPath =
            FString(FPlatformMisc::GetEnvironmentVariable(TEXT("USERPROFILE"))) / ".eos-free-edition-agreed";
#else
        FString FlagPath = FString(FPlatformMisc::GetEnvironmentVariable(TEXT("HOME"))) / ".eos-free-edition-agreed";
#endif
        FFileHelper::SaveStringToFile(TEXT("Accepted-v1"), *FlagPath);

        this->LicenseAgreementWindow->RequestDestroyWindow();
    }

    return FReply::Handled();
}

FReply FLicenseAgreementWindow::RejectLicenseTerms()
{
    // Close the editor, as the user is not agreeing to the license terms.
    FGenericPlatformMisc::RequestExit(false);

    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE

#endif