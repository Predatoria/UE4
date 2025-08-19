// Copyright June Rhodes. All Rights Reserved.

#include "EOSCheckEngine.h"

#include "ILevelEditor.h"
#include "LevelEditor.h"
#include "Modules/ModuleManager.h"
#include "SLevelViewport.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SWidget.h"

#include "EOSDedicatedServerMissingSettingsCheck.h"
#include "EOSDeveloperAuthToolNotRunningButAttemptedCheck.h"
#include "EOSEditorRequiresRestartForOSSCheck.h"
#include "EOSEssentialSettingsCheck.h"
#include "EOSMisconfiguredClientAssociationCheck.h"
#include "EOSMisconfiguredClientIdOrSecretCheck.h"
#include "EOSMissingPermissionForActionCheck.h"
#include "EOSMissingScopeAuthorizationCheck.h"
#include "EOSNetDriverSettingsCheck.h"
#include "EOSNetworkingDuplicateAccountCheck.h"
#include "EOSRunUnderOneProcessCheck.h"
#include "EOSSeparateServerWithListenServerCheck.h"

FEOSCheckEngine::FEOSCheckEngine()
{
    this->InjectedNotificationBar = SNew(SVerticalBox);

    this->RegisteredChecks.Add(MakeShared<FEOSEssentialSettingsCheck>());
    this->RegisteredChecks.Add(MakeShared<FEOSNetDriverSettingsCheck>());
    this->RegisteredChecks.Add(MakeShared<FEOSEditorRequiresRestartForOSSCheck>());
    this->RegisteredChecks.Add(MakeShared<FEOSDeveloperAuthToolNotRunningButAttemptedCheck>());
    this->RegisteredChecks.Add(MakeShared<FEOSNetworkingDuplicateAccountCheck>());
    this->RegisteredChecks.Add(MakeShared<FEOSSeparateServerWithListenServerCheck>());
    this->RegisteredChecks.Add(MakeShared<FEOSRunUnderOneProcessCheck>());
    this->RegisteredChecks.Add(MakeShared<FEOSMisconfiguredClientAssociationCheck>());
    this->RegisteredChecks.Add(MakeShared<FEOSMissingScopeAuthorizationCheck>());
    this->RegisteredChecks.Add(MakeShared<FEOSMissingPermissionForActionCheck>());
    this->RegisteredChecks.Add(MakeShared<FEOSDedicatedServerMissingSettingsCheck>());
    this->RegisteredChecks.Add(MakeShared<FEOSMisconfiguredClientIdOrSecretCheck>());
}

FEOSCheckEngine::~FEOSCheckEngine()
{
    if (this->CurrentlyInjectedIntoWidget.IsValid())
    {
        this->CurrentlyInjectedIntoWidget.Pin()->RemoveSlot(this->InjectedNotificationBar.ToSharedRef());
        this->InjectedNotificationBar.Reset();
        this->CurrentlyInjectedIntoWidget.Reset();
    }
}

FSlateColor FEOSCheckEngine::GetNotificationBackgroundColor() const
{
    return FSlateColor(FLinearColor(0.89627, 0.799103, 0.219526, 1.0));
}

FSlateColor FEOSCheckEngine::GetNotificationButtonOutlineColor() const
{
    return FSlateColor(FLinearColor(202.f / 255.f, 187.f / 255.f, 80.f / 255.f, 1.0));
}

FSlateColor FEOSCheckEngine::GetNotificationButtonBackgroundColor() const
{
    return FSlateColor(FLinearColor(255.f / 255.f, 244.f / 255.f, 143.f / 255.f, 1.0));
}

FSlateColor FEOSCheckEngine::GetNotificationButtonBackgroundHoveredColor() const
{
    return FSlateColor(FLinearColor(1.0, 0.921569, 0.392157, 1.0));
}

FSlateColor FEOSCheckEngine::GetNotificationButtonBackgroundPressedColor() const
{
    return FSlateColor(FLinearColor(0.947307, 0.854993, 0.246201, 1.0));
}

FSlateColor FEOSCheckEngine::GetNotificationFontColor() const
{
    return FSlateColor(FLinearColor(0.0, 0.0, 0.0, 1.0));
}

const FButtonStyle *FEOSCheckEngine::GetNotificationButtonStyle() const
{
    auto Self = const_cast<FEOSCheckEngine *>(this);
    Self->NotificationButtonStyle = FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("Button");
#if defined(UE_5_0_OR_LATER)
    Self->NotificationButtonStyle.Normal.OutlineSettings.Color = this->GetNotificationButtonOutlineColor();
    Self->NotificationButtonStyle.Hovered.OutlineSettings.Color = this->GetNotificationButtonOutlineColor();
    Self->NotificationButtonStyle.Pressed.OutlineSettings.Color = this->GetNotificationButtonOutlineColor();
#endif
    Self->NotificationButtonStyle.Normal.TintColor = this->GetNotificationButtonBackgroundColor();
    Self->NotificationButtonStyle.Hovered.TintColor = this->GetNotificationButtonBackgroundHoveredColor();
    Self->NotificationButtonStyle.Pressed.TintColor = this->GetNotificationButtonBackgroundPressedColor();
    return &(Self->NotificationButtonStyle);
}

const FSlateBrush *FEOSCheckEngine::GetNotificationBackgroundBrush() const
{
    auto Self = const_cast<FEOSCheckEngine *>(this);
    Self->NotificationBackgroundBrush = FSlateBrush();
    Self->NotificationBackgroundBrush.ImageSize = FVector2D(32, 32);
    Self->NotificationBackgroundBrush.Margin = FMargin();
    Self->NotificationBackgroundBrush.TintColor = FLinearColor::White;
    Self->NotificationBackgroundBrush.DrawAs = ESlateBrushDrawType::Image;
    Self->NotificationBackgroundBrush.Tiling = ESlateBrushTileType::NoTile;
    Self->NotificationBackgroundBrush.Mirroring = ESlateBrushMirrorType::NoMirror;
    Self->NotificationBackgroundBrush.ImageType = ESlateBrushImageType::NoImage;
    return &(Self->NotificationBackgroundBrush);
}

TSharedRef<SBorder> FEOSCheckEngine::CreateNewNotification(const FEOSCheckEntry &Entry)
{
    auto HorizontalBox = SNew(SHorizontalBox);
    HorizontalBox->AddSlot().FillWidth(1).VAlign(
        EVerticalAlignment::VAlign_Center)[SNew(STextBlock)
                                               .Text(FText::FromString(Entry.GetCheckMessage()))
                                               .AutoWrapText(true)
                                               .ColorAndOpacity(this, &FEOSCheckEngine::GetNotificationFontColor)];
    for (const auto &Action : Entry.GetCheckActions())
    {
        HorizontalBox->AddSlot()
            .AutoWidth()
            .Padding(FMargin(10, 0, 0, 0))
            .VAlign(EVerticalAlignment::VAlign_Center)
                [SNew(SButton)
                     .ButtonStyle(this->FEOSCheckEngine::GetNotificationButtonStyle())
                     .OnClicked(FOnClicked::CreateSP(
                         this,
                         &FEOSCheckEngine::OnActionClicked,
                         Entry.GetCheckId(),
                         Action
                             .GetActionId()))[SNew(STextBlock)
                                                  .Text(FText::FromString(Action.GetActionDisplayName()))
                                                  .ColorAndOpacity(this, &FEOSCheckEngine::GetNotificationFontColor)]];
    }
    HorizontalBox->AddSlot()
        .AutoWidth()
        .Padding(FMargin(10, 0, 0, 0))
        .VAlign(EVerticalAlignment::VAlign_Center)
            [SNew(SButton)
                 .ButtonStyle(this->FEOSCheckEngine::GetNotificationButtonStyle())
                 .OnClicked(FOnClicked::CreateSP(this, &FEOSCheckEngine::OnDismissClicked, Entry.GetCheckId()))
                     [SNew(STextBlock)
                          .Text(FText::FromString("Dismiss"))
                          .ColorAndOpacity(this, &FEOSCheckEngine::GetNotificationFontColor)]];

    return SNew(SBorder)
        .BorderBackgroundColor(this, &FEOSCheckEngine::GetNotificationBackgroundColor)
        .BorderImage(this, &FEOSCheckEngine::GetNotificationBackgroundBrush)[HorizontalBox]
        .Padding(FMargin(10, 5));
}

FReply FEOSCheckEngine::OnActionClicked(
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FString CheckId,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FString ActionId)
{
    if (!this->CurrentNotifications.Contains(CheckId))
    {
        return FReply::Handled();
    }

    this->CurrentNotifications[CheckId].Value->HandleAction(CheckId, ActionId);

    this->InjectedNotificationBar->RemoveSlot(this->CurrentNotifications[CheckId].Key.ToSharedRef());
    this->CurrentNotifications.Remove(CheckId);

    return FReply::Handled();
}

FReply FEOSCheckEngine::OnDismissClicked(
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FString CheckId)
{
    if (!this->CurrentNotifications.Contains(CheckId))
    {
        return FReply::Handled();
    }

    this->InjectedNotificationBar->RemoveSlot(this->CurrentNotifications[CheckId].Key.ToSharedRef());
    this->CurrentNotifications.Remove(CheckId);

    this->DismissedCheckIds.Add(CheckId);

    return FReply::Handled();
}

void FEOSCheckEngine::Tick(float DeltaSeconds)
{
    TSet<FString> DismissedCheckIdsToForget = this->DismissedCheckIds;
    for (const auto &Check : this->RegisteredChecks)
    {
        if (Check->ShouldTick())
        {
            auto Results = Check->Tick(DeltaSeconds);
            if (Results.Num() > 0)
            {
                for (const auto &Entry : Results)
                {
                    if (!this->CurrentNotifications.Contains(Entry.GetCheckId()))
                    {
                        if (this->DismissedCheckIds.Contains(Entry.GetCheckId()))
                        {
                            DismissedCheckIdsToForget.Remove(Entry.GetCheckId());
                        }
                        else
                        {
                            TSharedRef<SBorder> Notification = this->CreateNewNotification(Entry);
                            this->CurrentNotifications.Add(
                                Entry.GetCheckId(),
                                TTuple<TSharedPtr<SBorder>, TSharedPtr<IEOSCheck>>(Notification, Check));
                            this->InjectedNotificationBar->AddSlot()
                                .Padding(FMargin(0, 0, 0, 1))
                                .AutoHeight()
                                .AttachWidget(Notification);
                        }
                    }
                }
            }
        }
    }
    for (const auto &CheckId : DismissedCheckIdsToForget)
    {
        this->DismissedCheckIds.Remove(CheckId);
    }

    if (FLevelEditorModule *LevelEditorModule = FModuleManager::GetModulePtr<FLevelEditorModule>(TEXT("LevelEditor")))
    {
        TWeakPtr<ILevelEditor> LevelEditor = LevelEditorModule->GetLevelEditorInstance();
        if (LevelEditor.IsValid())
        {
            TSharedPtr<ILevelEditor> PinnedLevelEditor = LevelEditor.Pin();
            if (PinnedLevelEditor.IsValid())
            {
#if defined(UE_5_0_OR_LATER)
                TSharedPtr<SLevelViewport> CurrentActiveLevelViewport = PinnedLevelEditor->GetActiveViewportInterface();
#else
                TSharedPtr<IAssetViewport> CurrentActiveLevelViewport = PinnedLevelEditor->GetActiveViewportInterface();
#endif
                if (!CurrentActiveLevelViewport.IsValid())
                {
                    if (PinnedLevelEditor->GetViewports().Num() > 0)
                    {
                        CurrentActiveLevelViewport = PinnedLevelEditor->GetViewports()[0];
                    }
                }

                bool bRebuildInjectedLocation = false;
                if ((CurrentActiveLevelViewport.IsValid() && !this->ActiveLevelViewport.IsValid()) ||
                    (CurrentActiveLevelViewport.Get() != ActiveLevelViewport.Pin().Get()))
                {
                    bRebuildInjectedLocation = true;
                    this->ActiveLevelViewport = CurrentActiveLevelViewport;
                }

                if (bRebuildInjectedLocation)
                {
#if defined(UE_5_0_OR_LATER)
                    TSharedPtr<SWidget> CurrentWidget = StaticCastSharedPtr<SWidget>(CurrentActiveLevelViewport);
#else
                    TSharedPtr<SWidget> CurrentWidget = CurrentActiveLevelViewport->AsWidget();
#endif
                    while (CurrentWidget.IsValid() && !CurrentWidget->GetType().IsEqual(FName(TEXT("SVerticalBox"))) &&
                           CurrentWidget->IsParentValid())
                    {
                        CurrentWidget = CurrentWidget->GetParentWidget();
                    }

                    if (CurrentWidget.IsValid() && CurrentWidget->GetType().IsEqual(FName(TEXT("SVerticalBox"))))
                    {
                        TSharedPtr<SVerticalBox> VerticalBox = StaticCastSharedPtr<SVerticalBox>(CurrentWidget);
                        if (VerticalBox.IsValid())
                        {
                            if (VerticalBox->GetChildren()->Num() == 2)
                            {
                                if (this->CurrentlyInjectedIntoWidget.IsValid())
                                {
                                    this->CurrentlyInjectedIntoWidget.Pin()->RemoveSlot(
                                        this->InjectedNotificationBar.ToSharedRef());
                                    this->CurrentlyInjectedIntoWidget.Reset();
                                }

                                VerticalBox->InsertSlot(1)
                                    .AutoHeight()
                                    .Padding(FMargin(0, 0, 0, 1))
                                    .AttachWidget(this->InjectedNotificationBar.ToSharedRef());
                                this->CurrentlyInjectedIntoWidget = VerticalBox;
                            }
                        }
                    }
                }
            }
        }
    }
}

void FEOSCheckEngine::ProcessLogMessage(EOS_ELogLevel InLogLevel, const FString &Category, const FString &Message)
{
    for (const auto &Check : this->RegisteredChecks)
    {
        auto Results = Check->ProcessLogMessage(InLogLevel, Category, Message);
        if (Results.Num() > 0)
        {
            for (const auto &Entry : Results)
            {
                if (!this->CurrentNotifications.Contains(Entry.GetCheckId()))
                {
                    // We don't persist dismissals for log-based checks, since they aren't persistently triggered every
                    // tick.
                    this->DismissedCheckIds.Remove(Entry.GetCheckId());

                    TSharedRef<SBorder> Notification = this->CreateNewNotification(Entry);
                    this->CurrentNotifications.Add(
                        Entry.GetCheckId(),
                        TTuple<TSharedPtr<SBorder>, TSharedPtr<IEOSCheck>>(Notification, Check));
                    this->InjectedNotificationBar->AddSlot()
                        .Padding(FMargin(0, 0, 0, 1))
                        .AutoHeight()
                        .AttachWidget(Notification);
                }
            }
        }
    }
}

void FEOSCheckEngine::ProcessCustomSignal(const FString &Context, const FString &SignalId)
{
    for (const auto &Check : this->RegisteredChecks)
    {
        auto Results = Check->ProcessCustomSignal(Context, SignalId);
        if (Results.Num() > 0)
        {
            for (const auto &Entry : Results)
            {
                if (!this->CurrentNotifications.Contains(Entry.GetCheckId()))
                {
                    // We don't persist dismissals for log-based checks, since they aren't persistently triggered every
                    // tick.
                    this->DismissedCheckIds.Remove(Entry.GetCheckId());

                    TSharedRef<SBorder> Notification = this->CreateNewNotification(Entry);
                    this->CurrentNotifications.Add(
                        Entry.GetCheckId(),
                        TTuple<TSharedPtr<SBorder>, TSharedPtr<IEOSCheck>>(Notification, Check));
                    this->InjectedNotificationBar->AddSlot()
                        .Padding(FMargin(0, 0, 0, 1))
                        .AutoHeight()
                        .AttachWidget(Notification);
                }
            }
        }
    }
}