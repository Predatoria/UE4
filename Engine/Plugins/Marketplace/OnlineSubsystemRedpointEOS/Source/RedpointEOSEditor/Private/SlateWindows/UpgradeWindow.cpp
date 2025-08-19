// Copyright June Rhodes. All Rights Reserved.

#include "./UpgradeWindow.h"

#if defined(EOS_IS_FREE_EDITION)

#define LOCTEXT_NAMESPACE "OnlineSubsystemEOSEditorModule"

bool FUpgradeWindow::Open()
{
    if (this->UpgradePromptWindow.IsValid())
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

    const FVector2D WindowSize(500.f, 105.f);
    this->UpgradePromptWindow = SNew(SWindow)
                                    .Title(LOCTEXT("EOSUpgradeRequired", "EOS Upgrade Required"))
                                    .ClientSize(WindowSize)
                                    .SupportsMaximize(false)
                                    .FocusWhenFirstShown(true)
                                    .AutoCenter(EAutoCenter::PreferredWorkArea)
                                    .Style(&DialogStyle);
    this->UpgradePromptWindow->SetContent(
        SNew(SVerticalBox) +
        SVerticalBox::Slot().AutoHeight().Padding(FMargin(10, 10, 10, 10))
            [SNew(STextBlock)
                 .LineHeightPercentage(1.1f)
                 .Text(LOCTEXT(
                     "UpgradeRequiredDescription",
                     "Your version of EOS Online Subsystem (Free Edition) is too old. Please upgrade to the\nlatest "
                     "version. This version check only applies in the editor, and does not apply to\npackaged "
                     "games."))] +
        SVerticalBox::Slot()
            .AutoHeight()
            .Padding(FMargin(10, 0, 10, 10))
            .HAlign(EHorizontalAlignment::HAlign_Fill)
                [SNew(SHorizontalBox) +
                 SHorizontalBox::Slot().AutoWidth().Padding(FMargin(0, 0, 10, 0))
                     [SNew(SButton)
                          .ContentPadding(FMargin(10, 5))
                          .Text(LOCTEXT("UpgradeRequiredDownload", "Download latest version"))
                          .OnClicked(FOnClicked::CreateRaw(this, &FUpgradeWindow::FollowUpgradePrompt))]]);

    this->UpgradePromptWindow->SetOnWindowClosed(FOnWindowClosed::CreateLambda([this](const TSharedRef<SWindow> &) {
        this->UpgradePromptWindow.Reset();
    }));

    FSlateApplication::Get().AddWindow(this->UpgradePromptWindow.ToSharedRef(), true);

    return true;
}

FReply FUpgradeWindow::FollowUpgradePrompt()
{
    FPlatformProcess::LaunchURL(
        TEXT("https://licensing.redpoint.games/get/eos-online-subsystem-free"),
        nullptr,
        nullptr);

    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE

#endif