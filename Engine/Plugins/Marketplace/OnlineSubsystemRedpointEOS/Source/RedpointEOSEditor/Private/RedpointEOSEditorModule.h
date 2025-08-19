// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "Containers/Ticker.h"
#include "CoreGlobals.h"
#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Modules/ModuleManager.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

DECLARE_LOG_CATEGORY_EXTERN(LogEOSEditor, All, All);

class FRedpointEOSEditorModule : public IModuleInterface
{
public:
    FRedpointEOSEditorModule();

    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    bool Tick(float DeltaSeconds);

    void ProcessLogMessage(EOS_ELogLevel InLogLevel, const FString &Category, const FString &Message);
    void ProcessCustomSignal(const FString &Context, const FString &SignalId);

private:
    TSharedPtr<class FOnlineSubsystemEOSEditorConfig> SettingsInstance;
    TSharedPtr<class FEOSCheckEngine> CheckEngine;
#if defined(UE_5_0_OR_LATER)
    FTSTicker::FDelegateHandle TickerHandle;
#else
    FDelegateHandle TickerHandle;
#endif

    void ViewDocumentation();
    void ViewWebsite();

#if defined(EOS_IS_FREE_EDITION)
    TSharedRef<class FLicenseAgreementWindow> LicenseAgreementWindow;
    TSharedRef<class FUpgradeWindow> UpgradePromptWindow;

    void ViewLicenseAgreementInBrowser();
    static bool HasAcceptedLicenseAgreement();
#else
    void AccessSupport();
#endif

    static TSharedRef<SWidget> GenerateOnlineSettingsMenu(TSharedRef<class FUICommandList> InCommandList);

    void RegisterMenus();

#if PLATFORM_WINDOWS || PLATFORM_MAC
    static void SetLoginsEnabled(bool bEnabled);
    static void ResyncLoginsIfEnabled();
#endif

    TSharedPtr<class FUICommandList> PluginCommands;
};