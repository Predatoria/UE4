// Copyright June Rhodes. All Rights Reserved.

#include "RedpointEOSEditorModule.h"

#include "./Checks/EOSCheckEngine.h"
#include "./Launchers/AuthorizeLauncher.h"
#include "./Launchers/DevAuthToolLauncher.h"
#include "CoreGlobals.h"
#include "DesktopPlatform/Public/DesktopPlatformModule.h"
#include "DesktopPlatform/Public/IDesktopPlatform.h"
#include "DetailCustomizations.h"
#include "Editor.h"
#include "EditorDirectories.h"
#include "HAL/FileManager.h"
#include "ISettingsModule.h"
#include "Interfaces/IPluginManager.h"
#include "Interfaces/IProjectManager.h"
#include "LevelEditor.h"
#include "Misc/FeedbackContext.h"
#include "Misc/FileHelper.h"
#include "Misc/MessageDialog.h"
#include "Misc/ScopedSlowTask.h"
#include "ObjectEditorUtils.h"
#include "OnlineSubsystemEOSEditorCommands.h"
#include "OnlineSubsystemEOSEditorConfig.h"
#include "OnlineSubsystemEOSEditorConfigDetails.h"
#include "OnlineSubsystemEOSEditorPrefs.h"
#include "OnlineSubsystemEOSEditorStyle.h"
#include "OnlineSubsystemRedpointEOS/Public/OnlineSubsystemRedpointEOSModule.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystemUtils/Private/OnlinePIESettings.h"
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include <regex>
#include <string>
#if defined(EOS_IS_FREE_EDITION)
#include "./SlateWindows/LicenseAgreementWindow.h"
#include "./SlateWindows/UpgradeWindow.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IMainFrameModule.h"
#endif

DEFINE_LOG_CATEGORY(LogEOSEditor);

static const FName OnlineSubsystemEOSLicenseAgreementTabName("License Agreement");

#define LOCTEXT_NAMESPACE "OnlineSubsystemEOSEditorModule"

FRedpointEOSEditorModule::FRedpointEOSEditorModule()
    : SettingsInstance(nullptr)
    , CheckEngine(nullptr)
    , TickerHandle()
#if defined(EOS_IS_FREE_EDITION)
    , LicenseAgreementWindow(MakeShared<FLicenseAgreementWindow>())
    , UpgradePromptWindow(MakeShared<FUpgradeWindow>())
#endif
    , PluginCommands(nullptr)
{
}

void FRedpointEOSEditorModule::StartupModule()
{
    checkf(!this->CheckEngine.IsValid(), TEXT("FRedpointEOSEditorModule::StartupModule already called."));

    FModuleManager &ModuleManager = FModuleManager::Get();
    if (ModuleManager.GetModule("OnlineSubsystemEOS") != nullptr || ModuleManager.GetModule("EOSShared") != nullptr)
    {
        FText ConflictWarningTitle = LOCTEXT("ConflictTitle", "Error: Conflicting plugins detected");

        FMessageDialog::Open(
            EAppMsgType::Ok,
            LOCTEXT(
                "ConflictDescription",
                "This project has the Epic \"Online Subsystem EOS\" or \"EOS Shared\" plugin enabled. These plugins "
                "conflict with the Redpoint EOS Online Subsystem plugin, and they must be disabled from the Plugins "
                "window.\n\nThe Redpoint EOS Online Subsystem plugin will not work until you disable these plugins and "
                "restart the editor."),
            &ConflictWarningTitle);
    }

    // Create check engine.
    this->CheckEngine = MakeShared<FEOSCheckEngine>();
    auto RuntimeModule = (FOnlineSubsystemRedpointEOSModule *)ModuleManager.GetModule("OnlineSubsystemRedpointEOS");
    if (RuntimeModule != nullptr)
    {
        RuntimeModule->SetLogHandler([this](int32_t Level, const FString &Category, const FString &Message) {
            this->ProcessLogMessage((EOS_ELogLevel)Level, Category, Message);
        });
        RuntimeModule->SetCustomSignalHandler([this](const FString &Context, const FString &SignalId) {
            this->ProcessCustomSignal(Context, SignalId);
        });
    }

#if PLATFORM_WINDOWS || PLATFORM_MAC
    FDevAuthToolLauncher::bIsDevToolRunning = false;
    FDevAuthToolLauncher::bNeedsToCheckIfDevToolRunning = true;
    FDevAuthToolLauncher::ResetCheckHandle.Reset();
#endif

    // Set up editor styles & setup window.
    SettingsInstance = MakeShared<FOnlineSubsystemEOSEditorConfig>();
    SettingsInstance->RegisterSettings();
    FOnlineSubsystemEOSEditorStyle::Initialize();
    FOnlineSubsystemEOSEditorStyle::ReloadTextures();
    FOnlineSubsystemEOSEditorCommands::Register();

    // Map commands to actions.
    PluginCommands = MakeShareable(new FUICommandList);
    PluginCommands->MapAction(
        FOnlineSubsystemEOSEditorCommands::Get().ViewDocumentation,
        FExecuteAction::CreateRaw(this, &FRedpointEOSEditorModule::ViewDocumentation),
        FCanExecuteAction());
#if defined(EOS_IS_FREE_EDITION)
    PluginCommands->MapAction(
        FOnlineSubsystemEOSEditorCommands::Get().ViewLicenseAgreementInBrowser,
        FExecuteAction::CreateRaw(this, &FRedpointEOSEditorModule::ViewLicenseAgreementInBrowser),
        FCanExecuteAction());
#else
    PluginCommands->MapAction(
        FOnlineSubsystemEOSEditorCommands::Get().AccessSupport,
        FExecuteAction::CreateRaw(this, &FRedpointEOSEditorModule::AccessSupport),
        FCanExecuteAction());
#endif
    PluginCommands->MapAction(
        FOnlineSubsystemEOSEditorCommands::Get().ViewWebsite,
        FExecuteAction::CreateRaw(this, &FRedpointEOSEditorModule::ViewWebsite),
        FCanExecuteAction());

    // When the toolbar menus startup, let us register our menu items.
    UToolMenus::RegisterStartupCallback(
        FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FRedpointEOSEditorModule::RegisterMenus));

#if PLATFORM_WINDOWS || PLATFORM_MAC
    FEditorDelegates::PreBeginPIE.AddLambda([](const bool bIsSimulating) {
        FRedpointEOSEditorModule::ResyncLoginsIfEnabled();
    });
#endif

#if defined(EOS_IS_FREE_EDITION)
    // Show the license agreement.
    if (!HasAcceptedLicenseAgreement())
    {
        IMainFrameModule &MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
        if (MainFrame.IsWindowInitialized())
        {
            this->LicenseAgreementWindow->Open();
        }
        else
        {
            // NOLINTNEXTLINE(performance-unnecessary-value-param)
            MainFrame.OnMainFrameCreationFinished().AddLambda([this](TSharedPtr<SWindow>, bool) {
                this->LicenseAgreementWindow->Open();
            });
        }
    }

    // Fetch latest version number.
    auto HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(TEXT("https://licensing.redpoint.games/api/product-version/eos-online-subsystem-free"));
    HttpRequest->SetVerb(TEXT("GET"));
    HttpRequest->SetHeader(TEXT("Content-Length"), TEXT("0"));
    HttpRequest->OnProcessRequestComplete().BindLambda(
        // NOLINTNEXTLINE(performance-unnecessary-value-param)
        [this](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded) {
            if (HttpResponse.IsValid())
            {
                FString VersionNumberList = HttpResponse->GetContentAsString();

#if PLATFORM_WINDOWS
                FString VersionPath = FString(FPlatformMisc::GetEnvironmentVariable(TEXT("USERPROFILE"))) /
                                      ".eos-free-edition-latest-version";
#else
                FString VersionPath =
                    FString(FPlatformMisc::GetEnvironmentVariable(TEXT("HOME"))) / ".eos-free-edition-latest-version";
#endif
                FFileHelper::SaveStringToFile(VersionNumberList.TrimStartAndEnd(), *VersionPath);

#if defined(EOS_BUILD_VERSION_NAME)
                FString CurrentVersion = FString(EOS_BUILD_VERSION_NAME);
                TArray<FString> AllowedVersions;
                VersionNumberList.TrimStartAndEnd().ParseIntoArrayLines(AllowedVersions, true);
                if (AllowedVersions.Num() > 0)
                {
                    bool bAllowedVersionFound = false;
                    for (auto AllowedVersion : AllowedVersions)
                    {
                        if (AllowedVersion == CurrentVersion)
                        {
                            bAllowedVersionFound = true;
                            break;
                        }
                    }

                    if (!bAllowedVersionFound)
                    {
                        this->UpgradePromptWindow->Open();
                    }
                }
#endif
            }
        });
    HttpRequest->ProcessRequest();
#endif

    // Add custom detail for the EOS Online Subsystem project settings page.
    FPropertyEditorModule &PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomClassLayout(
        "OnlineSubsystemEOSEditorConfig",
        FOnGetDetailCustomizationInstance::CreateStatic(&FOnlineSubsystemEOSEditorConfigDetails::MakeInstance));

    // Register ticker.
    this->TickerHandle =
        FUTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FRedpointEOSEditorModule::Tick));
}

void FRedpointEOSEditorModule::ShutdownModule()
{
    FUTicker::GetCoreTicker().RemoveTicker(this->TickerHandle);
    this->TickerHandle.Reset();
    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);
    FOnlineSubsystemEOSEditorStyle::Shutdown();
    FOnlineSubsystemEOSEditorCommands::Unregister();
    SettingsInstance->UnregisterSettings();
}

bool FRedpointEOSEditorModule::Tick(float DeltaSeconds)
{
    this->CheckEngine->Tick(DeltaSeconds);
    return true;
}

void FRedpointEOSEditorModule::ProcessLogMessage(
    EOS_ELogLevel InLogLevel,
    const FString &Category,
    const FString &Message)
{
    this->CheckEngine->ProcessLogMessage(InLogLevel, Category, Message);
}

void FRedpointEOSEditorModule::ProcessCustomSignal(const FString &Context, const FString &SignalId)
{
    this->CheckEngine->ProcessCustomSignal(Context, SignalId);
}

void FRedpointEOSEditorModule::ViewDocumentation()
{
    FPlatformProcess::LaunchURL(TEXT("https://docs.redpoint.games/eos-online-subsystem/docs/"), nullptr, nullptr);
}

void FRedpointEOSEditorModule::ViewWebsite()
{
    FPlatformProcess::LaunchURL(TEXT("https://docs.redpoint.games/eos-online-subsystem/"), nullptr, nullptr);
}

#if defined(EOS_IS_FREE_EDITION)
void FRedpointEOSEditorModule::ViewLicenseAgreementInBrowser()
{
    if (!HasAcceptedLicenseAgreement())
    {
        // User hasn't accepted in editor yet, so they need to be able to click the I Accept button instead.
        this->LicenseAgreementWindow->Open();
        return;
    }

    FPlatformProcess::LaunchURL(TEXT("https://redpoint.games/eos-online-subsystem-free-eula/"), nullptr, nullptr);
}

bool FRedpointEOSEditorModule::HasAcceptedLicenseAgreement()
{
#if PLATFORM_WINDOWS
    FString FlagPath = FString(FPlatformMisc::GetEnvironmentVariable(TEXT("USERPROFILE"))) / ".eos-free-edition-agreed";
#else
    FString FlagPath = FString(FPlatformMisc::GetEnvironmentVariable(TEXT("HOME"))) / ".eos-free-edition-agreed";
#endif
    return FPaths::FileExists(FlagPath);
}
#else
void FRedpointEOSEditorModule::AccessSupport()
{
    FPlatformProcess::LaunchURL(
        TEXT("https://docs.redpoint.games/eos-online-subsystem/docs/accessing_support"),
        nullptr,
        nullptr);
}
#endif

TSharedRef<SWidget> FRedpointEOSEditorModule::GenerateOnlineSettingsMenu(
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<FUICommandList> InCommandList)
{
    FToolMenuContext MenuContext(InCommandList);

    return UToolMenus::Get()->GenerateWidget(
        "LevelEditor.LevelEditorToolBar.LevelToolbarOnlineSubsystemEOS",
        MenuContext);
}

void FRedpointEOSEditorModule::RegisterMenus()
{
    // Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
    FToolMenuOwnerScoped OwnerScoped(this);

    {
        UToolMenu *Menu =
            UToolMenus::Get()->RegisterMenu("LevelEditor.LevelEditorToolBar.LevelToolbarOnlineSubsystemEOS");

        struct Local
        {
            static void OpenSettings(FName ContainerName, FName CategoryName, FName SectionName)
            {
                FModuleManager::LoadModuleChecked<ISettingsModule>("Settings")
                    .ShowViewer(ContainerName, CategoryName, SectionName);
            }
        };

        {
            FToolMenuSection &Section =
                Menu->AddSection("OnlineSection", LOCTEXT("OnlineSection", "Epic Online Services"));

            Section.AddMenuEntry(
                "OnlineSettings",
                LOCTEXT("OnlineSettingsMenuLabel", "Settings..."),
                LOCTEXT("OnlineSettingsMenuToolTip", "Configure the EOS Online Subsystem plugin."),
                FSlateIcon(),
                FUIAction(FExecuteAction::CreateStatic(
                    &Local::OpenSettings,
                    FName("Project"),
                    FName("Game"),
                    FName("Epic Online Services"))));
        }

#if PLATFORM_WINDOWS || PLATFORM_MAC
        {
            FToolMenuSection &Section =
                Menu->AddSection("AuthenticationSection", LOCTEXT("AuthenticationSection", "Authentication"));
#if PLATFORM_WINDOWS && EOS_VERSION_AT_LEAST(1, 15, 0)
            Section.AddEntry(FToolMenuEntry::InitMenuEntry(
                "AuthorizeAccount",
                LOCTEXT("AuthorizeAccount", "Authorize Epic Games Account"),
                LOCTEXT(
                    "AuthorizeAccountTooltip",
                    "Launches the EOS account authorizer. You have to authorize each account with this tool before "
                    "the account can be used with the Developer Authentication Tool."),
                FSlateIcon(),
                FUIAction(
                    FExecuteAction::CreateLambda([]() {
                        FAuthorizeLauncher::Launch(true);
                    }),
                    FCanExecuteAction::CreateLambda([]() {
                        return !FDevAuthToolLauncher::GetPathToToolsFolder().IsEmpty() &&
                               FAuthorizeLauncher::CanLaunch();
                    }),
                    FGetActionCheckState(),
                    FIsActionButtonVisible()),
                EUserInterfaceActionType::Button,
                "StartAuthorizeAccountEAS"));
#endif
            Section.AddEntry(FToolMenuEntry::InitMenuEntry(
                "StartDevAuthTool",
                LOCTEXT("StartDevAuthTool", "Start Developer Authentication Tool"),
                LOCTEXT(
                    "StartDevAuthToolTooltip",
                    "Launches the Developer Authentication Tool, which can be used to automatically sign each "
                    "play-in-editor instance into a different Epic Games account."),
                FSlateIcon(),
                FUIAction(
                    FExecuteAction::CreateLambda([]() {
                        FDevAuthToolLauncher::Launch(true);
                    }),
                    FCanExecuteAction::CreateLambda([]() {
                        return !FDevAuthToolLauncher::IsDevAuthToolRunning() &&
                               !FDevAuthToolLauncher::GetPathToToolsFolder().IsEmpty();
                    }),
                    FGetActionCheckState(),
                    FIsActionButtonVisible()),
                EUserInterfaceActionType::Button,
                "StartDevAuthToolEAS"));
            Section.AddEntry(FToolMenuEntry::InitMenuEntry(
                "ToggleAutoLogin",
                LOCTEXT("AutoLogin", "Login before play-in-editor"),
                LOCTEXT(
                    "AutoLoginTooltip",
                    "If enabled, the editor will authenticate each play-in-editor instance against the Developer "
                    "Authentication Tool before play starts. This is recommended if you are testing anything other "
                    "than your login screen."),
                FSlateIcon(),
                FUIAction(
                    FExecuteAction::CreateLambda([]() {
                        IOnlineSubsystemUtils *Utils = Online::GetUtils();
                        FRedpointEOSEditorModule::SetLoginsEnabled(!Utils->IsOnlinePIEEnabled());
                        FRedpointEOSEditorModule::ResyncLoginsIfEnabled();
                    }),
                    FCanExecuteAction::CreateLambda([]() {
                        return FDevAuthToolLauncher::IsDevAuthToolRunning();
                    }),
                    FGetActionCheckState::CreateLambda([]() {
                        IOnlineSubsystemUtils *Utils = Online::GetUtils();
                        return (Utils->IsOnlinePIEEnabled() && FDevAuthToolLauncher::IsDevAuthToolRunning())
                                   ? ECheckBoxState::Checked
                                   : ECheckBoxState::Unchecked;
                    }),
                    FIsActionButtonVisible()),
                EUserInterfaceActionType::ToggleButton,
                "ToggleAutoLoginEAS"));
        }
#endif

        {
            FToolMenuSection &Section =
                Menu->AddSection("PluginSection", LOCTEXT("PluginSection", "EOS Online Subsystem Plugin"));

            Section.AddMenuEntry(FOnlineSubsystemEOSEditorCommands::Get().ViewDocumentation);
#if defined(EOS_IS_FREE_EDITION)
            Section.AddMenuEntry(FOnlineSubsystemEOSEditorCommands::Get().ViewLicenseAgreementInBrowser);
#else
            Section.AddMenuEntry(FOnlineSubsystemEOSEditorCommands::Get().AccessSupport);
#endif
            Section.AddMenuEntry(FOnlineSubsystemEOSEditorCommands::Get().ViewWebsite);
        }
    }

    {
#if defined(UE_5_0_OR_LATER)
        UToolMenu *Toolbar = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
#else
        UToolMenu *Toolbar = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
#endif
        {
#if defined(UE_5_0_OR_LATER)
            FToolMenuSection &SettingsSection = Toolbar->AddSection("EOSSettings");
#else
            FToolMenuSection &SettingsSection = Toolbar->AddSection("Settings");
#endif
            {
                FUIAction Act;
                Act.IsActionVisibleDelegate = FIsActionButtonVisible::CreateLambda([]() {
                    const UOnlineSubsystemEOSEditorPrefs *Prefs = GetDefault<UOnlineSubsystemEOSEditorPrefs>();
                    return !Prefs || !Prefs->bHideDropdownInEditorToolbar;
                });
                FToolMenuEntry ComboButton = FToolMenuEntry::InitComboButton(
                    "LevelToolbarOnlineSubsystemEOS",
                    Act,
                    FOnGetContent::CreateStatic(
                        &FRedpointEOSEditorModule::GenerateOnlineSettingsMenu,
                        PluginCommands.ToSharedRef()),
#if defined(UE_5_0_OR_LATER)
                    LOCTEXT("SettingsCombo", "Epic Online Services"),
#else
                    LOCTEXT("SettingsCombo", "Online (EOS)"),
#endif
                    LOCTEXT("SettingsCombo_ToolTip", "Manage Epic Online Services."),
                    FSlateIcon(FOnlineSubsystemEOSEditorStyle::GetStyleSetName(), "LevelEditor.OnlineSettings"),
                    false,
                    "LevelToolbarOnlineSubsystemEOS");
#if defined(UE_5_0_OR_LATER)
                ComboButton.StyleNameOverride = "CalloutToolbar";
#endif
                SettingsSection.AddEntry(ComboButton);
            }
        }
    }
}

#if PLATFORM_WINDOWS || PLATFORM_MAC
void FRedpointEOSEditorModule::SetLoginsEnabled(bool bEnabled)
{
    IOnlineSubsystemUtils *Utils = Online::GetUtils();
    Utils->SetShouldTryOnlinePIE(true);

    UE_LOG(LogEOSEditor, Verbose, TEXT("Setting PIE logins enabled to %s"), bEnabled ? TEXT("true") : TEXT("false"));

#if defined(UE_5_1_OR_LATER)
    UClass *PIESettingsClass =
        FindObject<UClass>(FTopLevelAssetPath(TEXT("/Script/OnlineSubsystemUtils"), TEXT("OnlinePIESettings")));
#else
    UClass *PIESettingsClass = FindObject<UClass>(ANY_PACKAGE, TEXT("OnlinePIESettings"));
#endif
    UObject *PIESettings = PIESettingsClass->GetDefaultObject(true);

    FProperty *Prop = PIESettingsClass->FindPropertyByName("bOnlinePIEEnabled");
    bool *SourceAddr = Prop->ContainerPtrToValuePtr<bool>(PIESettings);
    *SourceAddr = bEnabled;
}

void FRedpointEOSEditorModule::ResyncLoginsIfEnabled()
{
    const ULevelEditorPlaySettings *PlayInSettings = GetDefault<ULevelEditorPlaySettings>();
    int32 PlayNumberOfClients(0);
    if (!PlayInSettings->GetPlayNumberOfClients(PlayNumberOfClients))
    {
        return;
    }

    IOnlineSubsystemUtils *Utils = Online::GetUtils();
    if (Utils->IsOnlinePIEEnabled())
    {
        if (!FDevAuthToolLauncher::IsDevAuthToolRunning())
        {
            UE_LOG(
                LogEOSEditor,
                Verbose,
                TEXT("Turning off PIE automatic login because the Developer Authentication Tool is not running"));
            Utils->SetShouldTryOnlinePIE(false);
            return;
        }

        UE_LOG(
            LogEOSEditor,
            Verbose,
            TEXT("Synchronising automatic login credentials because PIE automatic login is enabled"));

#if defined(UE_5_1_OR_LATER)
        UClass *PIESettingsClass =
            FindObject<UClass>(FTopLevelAssetPath(TEXT("/Script/OnlineSubsystemUtils"), TEXT("OnlinePIESettings")));
#else
        UClass *PIESettingsClass = FindObject<UClass>(ANY_PACKAGE, TEXT("OnlinePIESettings"));
#endif
        UObject *PIESettings = PIESettingsClass->GetDefaultObject(true);

        FProperty *Prop = PIESettingsClass->FindPropertyByName("Logins");
        TArray<FPIELoginSettingsInternal> *Arr =
            Prop->ContainerPtrToValuePtr<TArray<FPIELoginSettingsInternal>>(PIESettings);

        while (Arr->Num() < PlayNumberOfClients)
        {
            FPIELoginSettingsInternal Settings;
            Settings.Id = TEXT("DEV_TOOL_AUTO_LOGIN");
            Settings.Token = TEXT("DEV_TOOL_AUTO_LOGIN");
            Settings.Type = TEXT("DEV_TOOL_AUTO_LOGIN");
            Arr->Add(Settings);
        }
        while (Arr->Num() > PlayNumberOfClients)
        {
            Arr->RemoveAt(Arr->Num() - 1);
        }
    }
}
#endif

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRedpointEOSEditorModule, RedpointEOSEditor)