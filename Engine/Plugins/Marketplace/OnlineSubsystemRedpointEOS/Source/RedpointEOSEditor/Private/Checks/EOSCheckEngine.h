// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IEOSCheck.h"
#include "Styling/SlateBrush.h"
#include "Styling/SlateColor.h"
#include "Styling/SlateTypes.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"

class FEOSCheckEngine : public TSharedFromThis<FEOSCheckEngine>
{
private:
    // These widgets are managed by the level editor, so must be TWeakPtr to allow level editor UI to be freed
    // correctly.
#if defined(UE_5_0_OR_LATER)
    TWeakPtr<class SLevelViewport> ActiveLevelViewport;
#else
    TWeakPtr<class IAssetViewport> ActiveLevelViewport;
#endif
    TWeakPtr<SVerticalBox> CurrentlyInjectedIntoWidget;

    TSharedPtr<SVerticalBox> InjectedNotificationBar;

    FButtonStyle NotificationButtonStyle;
    FSlateBrush NotificationBackgroundBrush;

    FSlateColor GetNotificationBackgroundColor() const;
    FSlateColor GetNotificationButtonOutlineColor() const;
    FSlateColor GetNotificationButtonBackgroundColor() const;
    FSlateColor GetNotificationButtonBackgroundHoveredColor() const;
    FSlateColor GetNotificationButtonBackgroundPressedColor() const;
    FSlateColor GetNotificationFontColor() const;
    const FButtonStyle *GetNotificationButtonStyle() const;
    const FSlateBrush *GetNotificationBackgroundBrush() const;

    TSharedRef<SBorder> CreateNewNotification(const FEOSCheckEntry &Entry);
    FReply OnActionClicked(FString CheckId, FString ActionId);
    FReply OnDismissClicked(FString CheckId);

    TMap<FString, TTuple<TSharedPtr<SBorder>, TSharedPtr<IEOSCheck>>> CurrentNotifications;

    TArray<TSharedRef<IEOSCheck>> RegisteredChecks;
    TSet<FString> DismissedCheckIds;

public:
    FEOSCheckEngine();
    UE_NONCOPYABLE(FEOSCheckEngine);
    virtual ~FEOSCheckEngine();

    void Tick(float DeltaSeconds);
    void ProcessLogMessage(EOS_ELogLevel InLogLevel, const FString &Category, const FString &Message);
    void ProcessCustomSignal(const FString &Context, const FString &SignalId);
};

// NOTE: ILevelEditor is the public interface that gives access to viewports (whose class is also public).
// Can get parent widget from GetParentWidget(). Should be able to traverse upwards to the SVerticalBox and then insert
// a slot.