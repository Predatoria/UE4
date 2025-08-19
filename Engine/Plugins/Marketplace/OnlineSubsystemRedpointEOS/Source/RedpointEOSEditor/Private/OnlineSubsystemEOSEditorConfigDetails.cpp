// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemEOSEditorConfigDetails.h"

#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "IDocumentation.h"
#include "OnlineSubsystemEOSEditorConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraphRegistry.h"
#include "PlatformHelpers.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "OnlineSubsystemEOSEditor"

const FName Category_FreeEdition = TEXT("Free Edition");
const FName Category_CoreConfiguration = TEXT("Core Configuration");
const FName Category_Authentication = TEXT("Authentication");
const FName Category_CrossPlatformAccountProviderEpicGames = TEXT("Cross Platform Account Provider: Epic Games");
const FName Category_CrossPlatformAccountProviderSimpleFirstParty =
    TEXT("Cross Platform Account Provider: Simple First Party");
const FName Category_Storage = TEXT("Storage");
const FName Category_Presence = TEXT("Presence");
const FName Category_CrossPlatform = TEXT("Cross-Platform");
const FName Category_Compatibility = TEXT("Compatibility");
const FName Category_Networking = TEXT("Networking");
const FName Category_VoiceChat = TEXT("Voice Chat");
const FName Category_Stats = TEXT("Stats");

const FName Property_AuthenticationGraph =
    GET_MEMBER_NAME_CHECKED(UOnlineSubsystemEOSEditorConfig, AuthenticationGraph);
const FName Property_EditorAuthenticationGraph =
    GET_MEMBER_NAME_CHECKED(UOnlineSubsystemEOSEditorConfig, EditorAuthenticationGraph);
const FName Property_CrossPlatformAccountProvider =
    GET_MEMBER_NAME_CHECKED(UOnlineSubsystemEOSEditorConfig, CrossPlatformAccountProvider);
const FName Property_EnableSanctionChecks =
    GET_MEMBER_NAME_CHECKED(UOnlineSubsystemEOSEditorConfig, EnableSanctionChecks);
const FName Property_NetworkAuthenticationMode =
    GET_MEMBER_NAME_CHECKED(UOnlineSubsystemEOSEditorConfig, NetworkAuthenticationMode);
const FName Property_EnableAntiCheat = GET_MEMBER_NAME_CHECKED(UOnlineSubsystemEOSEditorConfig, EnableAntiCheat);
const FName Property_TrustedPlatforms = GET_MEMBER_NAME_CHECKED(UOnlineSubsystemEOSEditorConfig, TrustedPlatforms);
const FName Property_TrustedClientPublicKey =
    GET_MEMBER_NAME_CHECKED(UOnlineSubsystemEOSEditorConfig, TrustedClientPublicKey);
const FName Property_TrustedClientPrivateKey =
    GET_MEMBER_NAME_CHECKED(UOnlineSubsystemEOSEditorConfig, TrustedClientPrivateKey);
const FName Property_EnableIdentityChecksOnListenServers =
    GET_MEMBER_NAME_CHECKED(UOnlineSubsystemEOSEditorConfig, EnableIdentityChecksOnListenServers);
const FName Property_DedicatedServerDistributionMode =
    GET_MEMBER_NAME_CHECKED(UOnlineSubsystemEOSEditorConfig, DedicatedServerDistributionMode);
const FName Property_EnableTrustedDedicatedServers =
    GET_MEMBER_NAME_CHECKED(UOnlineSubsystemEOSEditorConfig, EnableTrustedDedicatedServers);
const FName Property_EnableAutomaticEncryptionOnTrustedDedicatedServers =
    GET_MEMBER_NAME_CHECKED(UOnlineSubsystemEOSEditorConfig, EnableAutomaticEncryptionOnTrustedDedicatedServers);
const FName Property_DedicatedServerClientId =
    GET_MEMBER_NAME_CHECKED(UOnlineSubsystemEOSEditorConfig, DedicatedServerClientId);
const FName Property_DedicatedServerClientSecret =
    GET_MEMBER_NAME_CHECKED(UOnlineSubsystemEOSEditorConfig, DedicatedServerClientSecret);
const FName Property_DedicatedServerPublicKey =
    GET_MEMBER_NAME_CHECKED(UOnlineSubsystemEOSEditorConfig, DedicatedServerPublicKey);
const FName Property_DedicatedServerPrivateKey =
    GET_MEMBER_NAME_CHECKED(UOnlineSubsystemEOSEditorConfig, DedicatedServerPrivateKey);
const FName Property_TypingRules = GET_MEMBER_NAME_CHECKED(UOnlineSubsystemEOSEditorConfig, TypingRules);
const FName Property_ApiVersion = GET_MEMBER_NAME_CHECKED(UOnlineSubsystemEOSEditorConfig, ApiVersion);

TSharedRef<IDetailCustomization> FOnlineSubsystemEOSEditorConfigDetails::MakeInstance()
{
    return MakeShared<FOnlineSubsystemEOSEditorConfigDetails>();
}

void FOnlineSubsystemEOSEditorConfigDetails::CustomizeDetails(class IDetailLayoutBuilder &DetailBuilder)
{
    // Build the map of categories so we can set things up easier.
    TArray<FName> CategoryNames = {
        Category_FreeEdition,
        Category_CoreConfiguration,
        Category_Authentication,
        Category_CrossPlatformAccountProviderEpicGames,
        Category_CrossPlatformAccountProviderSimpleFirstParty,
        Category_VoiceChat,
        Category_Networking,
        Category_Storage,
        Category_Presence,
        Category_Stats,
        Category_CrossPlatform,
        Category_Compatibility};
    for (int i = 0; i < CategoryNames.Num(); i++)
    {
        IDetailCategoryBuilder *CategoryBuilder = &DetailBuilder.EditCategory(CategoryNames[i]);
        CategoryBuilder->SetSortOrder(i + 1);
        this->Categories.Add(CategoryNames[i], CategoryBuilder);
    }

    const FMargin TextMargin = FMargin(1, 1, 1, 4);

    this->SetPropertyAsDynamicDropDown(
        DetailBuilder,
        Category_Authentication,
        Property_AuthenticationGraph,
        LOCTEXT(
            "Authentication_Description",
            "Built-in authentication graph options:\n"
            "\x95 Default: Sign in with the account associated with the store or platform you are "
            "shipping "
            "the game on (such as Steam). If you can't sign into the store or platform account, "
            "authentication fails.\n"
            "\x95 Default, with Cross-Platform Fallback: Prefers to sign in with the account associated "
            "with the store or platform you are shipping the game on (such as Steam). If the game is "
            "launched outside the store, or the store or platform account can't be used, falls back to "
            "performing interactive sign in through the cross-platform account provider.\n"
            "\x95 Anonymous: Signs in with using an anonymous account on the device. The account will be "
            "tied to the device. This is only recommended for mobile-only games.\n"
            "\x95 Cross-Platform Only: Only signs in with the cross-platform account provider, without "
            "using any store or platform credentials to sign in. Players will always get an interactive "
            "sign in prompt (at least initially).\n"
            "\x95 (Platform) Only: Only signs in with the this platform. If you can't sign in with "
            "this platform's account, authentication fails.\n"
            "It is highly recommended that you read the documentation to "
            "ensure that you have chosen the correct option for your game."),
        FOnGenerateDropDownMenu::CreateLambda([]() -> FDynamicOptionMap {
            FDynamicOptionMap Results;
            for (const auto &KV : FAuthenticationGraphRegistry::GetNames())
            {
                Results.Add(
                    KV.Key,
                    FDynamicDropDownOption(
                        KV.Value,
                        KV.Key.IsEqual(FName(TEXT("AlwaysFail"))) || KV.Key.IsEqual(FName(TEXT("DevAuthTool")))));
            }
            return Results;
        }),
        FDynamicOptionChanged::CreateLambda([this](FName NewValue) {
        }));

    this->SetPropertyAsDynamicDropDown(
        DetailBuilder,
        Category_Authentication,
        Property_EditorAuthenticationGraph,
        LOCTEXT(
            "Authentication_EditorDescription",
            "The authentication graph to use when running the game in the editor. This "
            "should almost always be left as \"Default\"."),
        FOnGenerateDropDownMenu::CreateLambda([]() -> FDynamicOptionMap {
            FDynamicOptionMap Results;
            for (const auto &KV : FAuthenticationGraphRegistry::GetNames())
            {
                Results.Add(KV.Key, FDynamicDropDownOption(KV.Value, KV.Key.IsEqual(FName(TEXT("AlwaysFail")))));
            }
            return Results;
        }));

    this->SetPropertyAsDynamicDropDown(
        DetailBuilder,
        Category_Authentication,
        Property_CrossPlatformAccountProvider,
        LOCTEXT(
            "CrossPlatformAccountProvider_Description",
            "To register your own cross-platform account provider, refer to the documentation."),
        FOnGenerateDropDownMenu::CreateLambda([]() -> FDynamicOptionMap {
            FDynamicOptionMap Results;
            Results.Add(NAME_None, FDynamicDropDownOption(FText::FromName(NAME_None)));
            for (const auto &Name : FAuthenticationGraphRegistry::GetCrossPlatformAccountProviderNames())
            {
                Results.Add(
                    Name,
                    FDynamicDropDownOption(
                        FText::FromName(Name),
                        Name.IsEqual(FName(TEXT("AutomatedTesting"))) ||
                            Name.IsEqual(FName(TEXT("AutomatedTestingOSS")))));
            }
            return Results;
        }),
        FDynamicOptionChanged::CreateLambda([this](FName NewValue) {
            this->Categories[Category_CrossPlatformAccountProviderEpicGames]->SetCategoryVisibility(
                NewValue.IsEqual(FName(TEXT("EpicGames"))));
            this->Categories[Category_CrossPlatformAccountProviderSimpleFirstParty]->SetCategoryVisibility(
                NewValue.IsEqual(FName(TEXT("SimpleFirstParty"))));
        }));

    this->DeclarePropertyOrder(
        DetailBuilder,
        Category_Networking,
        TArray<FName>({
            Property_NetworkAuthenticationMode,
            Property_EnableAntiCheat,
        }));

    auto GetEffectiveNetworkAuthenticationMode = [DB = &DetailBuilder]() {
        TSharedRef<IPropertyHandle> ApiVersionProperty =
            DB->GetProperty(Property_ApiVersion, UOnlineSubsystemEOSEditorConfig::StaticClass());
        TSharedRef<IPropertyHandle> NetworkAuthenticationProperty =
            DB->GetProperty(Property_NetworkAuthenticationMode, UOnlineSubsystemEOSEditorConfig::StaticClass());
        ENetworkAuthenticationMode NetworkAuth;
        EEOSApiVersion ApiVersion;
        ApiVersionProperty->GetValue((uint8 &)ApiVersion);
        NetworkAuthenticationProperty->GetValue((uint8 &)NetworkAuth);
        if (NetworkAuth == ENetworkAuthenticationMode::Default)
        {
            if (ApiVersion == EEOSApiVersion::v2022_02_11)
            {
                return ENetworkAuthenticationMode::UserCredentials;
            }
            else
            {
                return ENetworkAuthenticationMode::IDToken;
            }
        }
        return NetworkAuth;
    };

    this->SetPropertyVisibilityCondition(
        DetailBuilder,
        Category_Networking,
        Property_EnableAntiCheat,
        FComputePropertyVisibility::CreateLambda([GetEffectiveNetworkAuthenticationMode]() -> bool {
            return GetEffectiveNetworkAuthenticationMode() != ENetworkAuthenticationMode::Off;
        }));

    this->AddAntiCheatSigningRow(DetailBuilder, Category_Networking, GetEffectiveNetworkAuthenticationMode);
#if EAC_CLIENT_MODULE_TESTING_WORKS
    this->AddAntiCheatClientModuleRow(DetailBuilder, Category_Networking);
#endif

    this->SetPropertyAsTrustedPlatforms(
        DetailBuilder,
        Category_Networking,
        Property_TrustedPlatforms,
        GetEffectiveNetworkAuthenticationMode);

    this->DeclarePropertyOrder(
        DetailBuilder,
        Category_Networking,
        TArray<FName>({
            Property_TrustedClientPublicKey,
            Property_TrustedClientPrivateKey,
            Property_EnableSanctionChecks,
            Property_EnableIdentityChecksOnListenServers,
            Property_DedicatedServerDistributionMode,
        }));

    FComputePropertyVisibility AntiCheatSettingsVisibility = FComputePropertyVisibility::CreateLambda(
        [DB = &DetailBuilder, GetEffectiveNetworkAuthenticationMode]() -> bool {
            TSharedRef<IPropertyHandle> TargetPropertyEnabled =
                DB->GetProperty(Property_EnableAntiCheat, UOnlineSubsystemEOSEditorConfig::StaticClass());
            bool EnabledVal;
            TargetPropertyEnabled->GetValue(EnabledVal);
            return EnabledVal && GetEffectiveNetworkAuthenticationMode() != ENetworkAuthenticationMode::Off;
        });
    this->SetPropertyVisibilityCondition(
        DetailBuilder,
        Category_Networking,
        Property_TrustedClientPublicKey,
        AntiCheatSettingsVisibility);
    this->SetPropertyVisibilityCondition(
        DetailBuilder,
        Category_Networking,
        Property_TrustedClientPrivateKey,
        AntiCheatSettingsVisibility);

    FComputePropertyVisibility DefaultedNetworkSettingsVisibility = FComputePropertyVisibility::CreateLambda(
        [DB = &DetailBuilder, GetEffectiveNetworkAuthenticationMode]() -> bool {
            return GetEffectiveNetworkAuthenticationMode() == ENetworkAuthenticationMode::UserCredentials;
        });
    this->SetPropertyVisibilityCondition(
        DetailBuilder,
        Category_Networking,
        Property_EnableSanctionChecks,
        DefaultedNetworkSettingsVisibility);
    this->SetPropertyVisibilityCondition(
        DetailBuilder,
        Category_Networking,
        Property_EnableIdentityChecksOnListenServers,
        DefaultedNetworkSettingsVisibility);

    FDetailWidgetRow &SecretStorage =
        this->Categories[Category_Networking]->AddCustomRow(FText::FromString(TEXT("Dedicated Servers")));
    SecretStorage.ValueContent().MinDesiredWidth(
        800)[SNew(STextBlock)
                 .Font(IDetailLayoutBuilder::GetDetailFont())
                 .AutoWrapText(true)
                 .MinDesiredWidth(800)
                 .Margin(TextMargin)
                 .Text_Lambda([DB = &DetailBuilder]() -> FText {
                     TSharedRef<IPropertyHandle> PropertyDistribution = DB->GetProperty(
                         Property_DedicatedServerDistributionMode,
                         UOnlineSubsystemEOSEditorConfig::StaticClass());
                     uint8 EnumValue;
                     PropertyDistribution->GetValue(EnumValue);
                     if ((EDedicatedServersDistributionMode)EnumValue ==
                         EDedicatedServersDistributionMode::DevelopersOnly)
                     {
                         return LOCTEXT(
                             "DedicatedServerSecretStorage_Normal",
                             "Dedicated server secrets are being stored in Config/DedicatedServerEngine.ini "
                             "and will be embedded into the dedicated server binaries. Dedicated server "
                             "binaries are not safe to "
                             "distribute to end users.");
                     }
                     else if (
                         (EDedicatedServersDistributionMode)EnumValue ==
                         EDedicatedServersDistributionMode::DevelopersAndPlayers)
                     {
                         return LOCTEXT(
                             "DedicatedServerSecretStorage_NoRedist",
                             "Dedicated server secrets are being stored in "
                             "DefaultEditor.ini. You will need "
                             "to provide these values on the command-line when launching dedicated servers. "
                             "Dedicated server "
                             "binaries are SAFE to distribute to end users.");
                     }
                     else
                     {
                         return LOCTEXT(
                             "DedicatedServerSecretStorage_Off",
                             "Dedicated server secrets are being stored in "
                             "DefaultEditor.ini (in case you want to turn trusted "
                             "dedicated servers back on in the future). Trusted dedicated server "
                             "functionality is currently forced to off.");
                     }
                 })];

    this->DeclarePropertyOrder(
        DetailBuilder,
        Category_Networking,
        TArray<FName>({
            Property_EnableTrustedDedicatedServers,
            Property_EnableAutomaticEncryptionOnTrustedDedicatedServers,
            Property_DedicatedServerClientId,
            Property_DedicatedServerClientSecret,
            Property_DedicatedServerPublicKey,
            Property_DedicatedServerPrivateKey,
        }));

    this->SetPropertyVisibilityCondition(
        DetailBuilder,
        Category_Networking,
        Property_EnableTrustedDedicatedServers,
        FComputePropertyVisibility::CreateLambda(
            [DB = &DetailBuilder, GetEffectiveNetworkAuthenticationMode]() -> bool {
                EDedicatedServersDistributionMode DistributionMode;
                {
                    TSharedRef<IPropertyHandle> TargetProperty = DB->GetProperty(
                        Property_DedicatedServerDistributionMode,
                        UOnlineSubsystemEOSEditorConfig::StaticClass());
                    uint8 EnumVal;
                    TargetProperty->GetValue(EnumVal);
                    DistributionMode = (EDedicatedServersDistributionMode)EnumVal;
                }
                return DistributionMode != EDedicatedServersDistributionMode::PlayersOnly &&
                       GetEffectiveNetworkAuthenticationMode() == ENetworkAuthenticationMode::UserCredentials;
            }));
    FComputePropertyVisibility DedicatedServerSettingsVisibility = FComputePropertyVisibility::CreateLambda(
        [DB = &DetailBuilder, GetEffectiveNetworkAuthenticationMode]() -> bool {
            TSharedRef<IPropertyHandle> TargetPropertyDistribution = DB->GetProperty(
                Property_DedicatedServerDistributionMode,
                UOnlineSubsystemEOSEditorConfig::StaticClass());
            TSharedRef<IPropertyHandle> TargetPropertyEnabled =
                DB->GetProperty(Property_EnableTrustedDedicatedServers, UOnlineSubsystemEOSEditorConfig::StaticClass());
            uint8 EnumVal;
            ENetworkAuthenticationMode NetworkAuthVal = GetEffectiveNetworkAuthenticationMode();
            bool EnabledVal;
            TargetPropertyDistribution->GetValue(EnumVal);
            TargetPropertyEnabled->GetValue(EnabledVal);
            return (EDedicatedServersDistributionMode)EnumVal != EDedicatedServersDistributionMode::PlayersOnly &&
                   (NetworkAuthVal == ENetworkAuthenticationMode::IDToken ||
                    (NetworkAuthVal == ENetworkAuthenticationMode::UserCredentials && EnabledVal));
        });
    this->SetPropertyVisibilityCondition(
        DetailBuilder,
        Category_Networking,
        Property_EnableAutomaticEncryptionOnTrustedDedicatedServers,
        DedicatedServerSettingsVisibility);
    this->SetPropertyVisibilityCondition(
        DetailBuilder,
        Category_Networking,
        Property_DedicatedServerPublicKey,
        DedicatedServerSettingsVisibility);
    this->SetPropertyVisibilityCondition(
        DetailBuilder,
        Category_Networking,
        Property_DedicatedServerPrivateKey,
        DedicatedServerSettingsVisibility);
}

void FOnlineSubsystemEOSEditorConfigDetails::SetPropertyAsTrustedPlatforms(
    class IDetailLayoutBuilder &DetailBuilder,
    FName CategoryName,
    FName PropertyName,
    const std::function<ENetworkAuthenticationMode()> &GetEffectiveNetworkAuthenticationMode)
{
    const FMargin TextMargin = FMargin(1, 1, 1, 4);

    TSharedRef<IPropertyHandle> Property =
        DetailBuilder.GetProperty(PropertyName, UOnlineSubsystemEOSEditorConfig::StaticClass());
    IDetailPropertyRow &PropertyRow = this->Categories[CategoryName]->AddProperty(Property);
    FDetailWidgetRow &WidgetRow = PropertyRow.CustomWidget();
    PropertyRow.Visibility(TAttribute<EVisibility>::Create(TDelegate<EVisibility()>::CreateLambda(
        [DB = &DetailBuilder, GetEffectiveNetworkAuthenticationMode]() -> EVisibility {
            TSharedRef<IPropertyHandle> TargetPropertyEnabled =
                DB->GetProperty(Property_EnableAntiCheat, UOnlineSubsystemEOSEditorConfig::StaticClass());
            bool EnabledVal;
            TargetPropertyEnabled->GetValue(EnabledVal);
            return (GetEffectiveNetworkAuthenticationMode() != ENetworkAuthenticationMode::Off && EnabledVal)
                       ? EVisibility::Visible
                       : EVisibility::Collapsed;
        })));

    TSet<FName> UniquePlatformNames;
    for (const auto &PlatformName : GetConfidentialPlatformNames())
    {
        UniquePlatformNames.Add(PlatformName);
    }

    FText TrustedPlatformsTip = LOCTEXT(
        "TrustedPlatformsTip",
        "Trusted platforms are permitted to connect to Anti-Cheat protected game servers, "
        "without running Anti-Cheat. This is only available for console platforms, where "
        "clients can be trusted. EOS Online Subsystem uses a public/private keypair to ensure "
        "that the trusted platforms selected here can connect without Anti-Cheat protection.");

    auto PlatformList = SNew(SVerticalBox);
    PlatformList->AddSlot().AutoHeight()[SNew(STextBlock)
                                             .Font(IDetailLayoutBuilder::GetDetailFont())
                                             .Text(TrustedPlatformsTip)
                                             .Margin(TextMargin)
                                             .AutoWrapText(true)];
    for (const auto &PlatformName : UniquePlatformNames)
    {
        PlatformList->AddSlot().AutoHeight()
            [SNew(SCheckBox)
                 .IsChecked_Lambda([DB = &DetailBuilder, Property, PlatformName]() -> ECheckBoxState {
                     void *Value = nullptr;
                     Property->GetValueData(Value);
                     return ((TSet<FName> *)Value)->Contains(PlatformName) ? ECheckBoxState::Checked
                                                                           : ECheckBoxState::Unchecked;
                 })
                 .OnCheckStateChanged_Lambda([DB = &DetailBuilder, Property, PlatformName](ECheckBoxState InNewState) {
                     void *Value = nullptr;
                     Property->GetValueData(Value);
                     if (InNewState == ECheckBoxState::Checked)
                     {
                         ((TSet<FName> *)Value)->Add(PlatformName);
                     }
                     else
                     {
                         ((TSet<FName> *)Value)->Remove(PlatformName);
                     }
                     Property->NotifyPostChange(EPropertyChangeType::ValueSet);
                 })[SNew(SHorizontalBox) +
                    SHorizontalBox::Slot().AutoWidth()[SNew(STextBlock).Text(FText::FromName(PlatformName))]]];
    }

    WidgetRow.NameContent()[SNew(STextBlock)
                                .Font(IDetailLayoutBuilder::GetDetailFont())
                                .Text(LOCTEXT("TrustedPlatforms", "Trusted Platforms"))
                                .ToolTipText(TrustedPlatformsTip)];
    WidgetRow.ValueContent().MinDesiredWidth(800)[PlatformList];
}

static bool bKeyAndCerExists = false;

static const FText SigningKeyNotFound = LOCTEXT(
    "SigningKey_NotFound",
    "Build/NoRedist/base_private.key or Build/NoRedist/base_public.cer are missing. Please download the tooling "
    "configuration ZIP from the Epic Games Developer Portal under \"Player Moderation\". Extract the files from "
    "the downloaded ZIP and place them in the Build/NoRedist/ folder inside your project (create the folder if it "
    "doesn't exist).");
static const FText SigningKeyFound = LOCTEXT(
    "SigningKey_Found",
    "Build/NoRedist/base_private.key and Build/NoRedist/base_public.cer were both found successfully.");

static bool KeyAndCerExists()
{
    FString KeyPath = FPaths::ProjectDir() / TEXT("Build") / TEXT("NoRedist") / TEXT("base_private.key");
    FString CerPath = FPaths::ProjectDir() / TEXT("Build") / TEXT("NoRedist") / TEXT("base_public.cer");
    if (FPaths::FileExists(KeyPath) && FPaths::FileExists(CerPath))
    {
        return true;
    }
    return false;
}

static bool bSplashScreenExists = false;

static const FText SplashScreenNotFound =
    LOCTEXT("SplashScreen_NotFound", "Build/NoRedist/AntiCheatSplashScreen.png is missing. It should be an 800x450 PNG image.");

static const FText SplashScreenFound = LOCTEXT(
    "SplashScreen_Found",
    "Build/NoRedist/AntiCheatSplashScreen.png was found, and will be used as the Anti-Cheat splash screen.");

static bool SplashScreenExists()
{
    FString SplashPath = FPaths::ProjectDir() / TEXT("Build") / TEXT("NoRedist") / TEXT("AntiCheatSplashScreen.png");
    if (FPaths::FileExists(SplashPath))
    {
        return true;
    }
    return false;
}

#if EAC_CLIENT_MODULE_TESTING_WORKS
static bool bAntiCheatClientModuleExists = false;

static const FText AntiCheatClientModuleNotFound = LOCTEXT(
    "AntiCheatClientModule_NotFound",
    "Build/NoRedist/game.exe.eac is missing. Please download the details from the Epic Games Developer Portal under "
    "\"Player Moderation\" / \"Anti-Cheat\" for the Anti-Cheat version you have showing as Active (in green). You can "
    "download the details by clicking on the ... and then clicking \"Download Details\" in the menu that appears. "
    "Place the downloaded file under Build/NoRedist/.");
static const FText AntiCheatClientModuleFound =
    LOCTEXT("AntiCheatClientModule_Found", "Build/NoRedist/game.exe.eac was found successfully.");

static bool AntiCheatClientModuleExists()
{
    FString EACPath = FPaths::ProjectDir() / TEXT("Build") / TEXT("NoRedist") / TEXT("game.exe.eac");
    if (FPaths::FileExists(EACPath))
    {
        return true;
    }
    return false;
}
#endif

void FOnlineSubsystemEOSEditorConfigDetails::AddAntiCheatSigningRow(
    class IDetailLayoutBuilder &DetailBuilder,
    FName CategoryName,
    const std::function<ENetworkAuthenticationMode()> &GetEffectiveNetworkAuthenticationMode)
{
    const FMargin TextMargin = FMargin(1, 1, 1, 4);

    bKeyAndCerExists = KeyAndCerExists();
    bSplashScreenExists = SplashScreenExists();

    auto ValueContent = SNew(SVerticalBox);
    ValueContent->AddSlot().AutoHeight()[SNew(STextBlock)
                                             .Font(IDetailLayoutBuilder::GetDetailFont())
                                             .Text_Lambda([]() -> FText {
                                                 return bKeyAndCerExists ? SigningKeyFound : SigningKeyNotFound;
                                             })
                                             .ColorAndOpacity_Lambda([]() -> FSlateColor {
                                                 return bKeyAndCerExists
                                                            ? FSlateColor(FLinearColor(0.25f, 1.0f, 0.25f, 1.0f))
                                                            : FSlateColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
                                             })
                                             .Margin(TextMargin)
                                             .AutoWrapText(true)];
    ValueContent->AddSlot().AutoHeight()[SNew(STextBlock)
                                             .Font(IDetailLayoutBuilder::GetDetailFont())
                                             .Text_Lambda([]() -> FText {
                                                 return bSplashScreenExists ? SplashScreenFound : SplashScreenNotFound;
                                             })
                                             .ColorAndOpacity_Lambda([]() -> FSlateColor {
                                                 return bSplashScreenExists
                                                            ? FSlateColor(FLinearColor(0.25f, 1.0f, 0.25f, 1.0f))
                                                            : FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
                                             })
                                             .Margin(TextMargin)
                                             .AutoWrapText(true)];
    auto ButtonBox = SNew(SHorizontalBox);
    ButtonBox->AddSlot()
        .AutoWidth()[SNew(SButton).Text(LOCTEXT("SigningKeyCheck", "Check Now")).OnClicked_Lambda([]() -> FReply {
            bKeyAndCerExists = KeyAndCerExists();
            bSplashScreenExists = SplashScreenExists();
#if EAC_CLIENT_MODULE_TESTING_WORKS
            bAntiCheatClientModuleExists = AntiCheatClientModuleExists();
#endif
            return FReply::Handled();
        })];
    ValueContent->AddSlot().AutoHeight()[ButtonBox];

    FDetailWidgetRow &SigningKey =
        this->Categories[CategoryName]->AddCustomRow(FText::FromString(TEXT("Anti-Cheat Signing Key")));
    SigningKey.NameContent()[SNew(STextBlock)
                                 .Font(IDetailLayoutBuilder::GetDetailFont())
                                 .Text(LOCTEXT("AntiCheatSigningKey", "Anti-Cheat Signing Key"))];
    SigningKey.ValueContent().MinDesiredWidth(800)[ValueContent];
    SigningKey.Visibility(TAttribute<EVisibility>::Create(TDelegate<EVisibility()>::CreateLambda(
        [DB = &DetailBuilder, GetEffectiveNetworkAuthenticationMode]() -> EVisibility {
            TSharedRef<IPropertyHandle> TargetPropertyEnabled =
                DB->GetProperty(Property_EnableAntiCheat, UOnlineSubsystemEOSEditorConfig::StaticClass());
            bool EnabledVal;
            TargetPropertyEnabled->GetValue(EnabledVal);
            return (GetEffectiveNetworkAuthenticationMode() != ENetworkAuthenticationMode::Off && EnabledVal)
                       ? EVisibility::Visible
                       : EVisibility::Collapsed;
        })));
}

#if EAC_CLIENT_MODULE_TESTING_WORKS
void FOnlineSubsystemEOSEditorConfigDetails::AddAntiCheatClientModuleRow(
    class IDetailLayoutBuilder &DetailBuilder,
    FName CategoryName)
{
    const FMargin TextMargin = FMargin(1, 1, 1, 4);

    bAntiCheatClientModuleExists = AntiCheatClientModuleExists();

    auto ValueContent = SNew(SVerticalBox);
    ValueContent->AddSlot().AutoHeight()[SNew(STextBlock)
                                             .Font(IDetailLayoutBuilder::GetDetailFont())
                                             .Text_Lambda([]() -> FText {
                                                 return bAntiCheatClientModuleExists ? AntiCheatClientModuleFound
                                                                                     : AntiCheatClientModuleNotFound;
                                             })
                                             .ColorAndOpacity_Lambda([]() -> FSlateColor {
                                                 return bAntiCheatClientModuleExists
                                                            ? FSlateColor(FLinearColor(0.25f, 1.0f, 0.25f, 1.0f))
                                                            : FSlateColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
                                             })
                                             .Margin(TextMargin)
                                             .AutoWrapText(true)];
    auto ButtonBox = SNew(SHorizontalBox);
    ButtonBox->AddSlot()
        .AutoWidth()[SNew(SButton).Text(LOCTEXT("EACModuleCheck", "Check Now")).OnClicked_Lambda([]() -> FReply {
            bKeyAndCerExists = KeyAndCerExists();
            bAntiCheatClientModuleExists = AntiCheatClientModuleExists();
            return FReply::Handled();
        })];
    ValueContent->AddSlot().AutoHeight()[ButtonBox];

    FDetailWidgetRow &EACModule =
        this->Categories[CategoryName]->AddCustomRow(FText::FromString(TEXT("Anti-Cheat Client Module")));
    EACModule.NameContent()[SNew(STextBlock)
                                .Font(IDetailLayoutBuilder::GetDetailFont())
                                .Text(LOCTEXT("AntiCheatClientModule", "Anti-Cheat Client Module"))];
    EACModule.ValueContent().MinDesiredWidth(800)[ValueContent];
    EACModule.Visibility(
        TAttribute<EVisibility>::Create(TDelegate<EVisibility()>::CreateLambda([DB = &DetailBuilder]() -> EVisibility {
            TSharedRef<IPropertyHandle> TargetPropertyEnabled =
                DB->GetProperty(Property_EnableAntiCheat, UOnlineSubsystemEOSEditorConfig::StaticClass());
            bool EnabledVal;
            TargetPropertyEnabled->GetValue(EnabledVal);
            return EnabledVal ? EVisibility::Visible : EVisibility::Collapsed;
        })));
}
#endif

void FOnlineSubsystemEOSEditorConfigDetails::SetPropertyAsDynamicDropDown(
    class IDetailLayoutBuilder &DetailBuilder,
    FName CategoryName,
    FName PropertyName,
    const FText &Description,
    const FOnGenerateDropDownMenu &GenerateDropDownMenu,
    const FDynamicOptionChanged &OnOptionChanged)
{
    const FMargin TextMargin = FMargin(1, 1, 1, 4);
    const FMargin ComboMargin = FMargin(0, 0, 0, 2);

    TSharedRef<IPropertyHandle> Property =
        DetailBuilder.GetProperty(PropertyName, UOnlineSubsystemEOSEditorConfig::StaticClass());
    IDetailPropertyRow &PropertyRow = this->Categories[CategoryName]->AddProperty(Property);
    FDetailWidgetRow &WidgetRow = PropertyRow.CustomWidget();
    WidgetRow.NameContent()[Property->CreatePropertyNameWidget()];
    WidgetRow.ValueContent().MaxDesiredWidth(0)
        [SNew(SVerticalBox) +
         SVerticalBox::Slot()[SNew(STextBlock)
                                  .Font(IDetailLayoutBuilder::GetDetailFont())
                                  .AutoWrapText(true)
                                  .Margin(TextMargin)
                                  .Text(Description)] +
         SVerticalBox::Slot()
             .AutoHeight()
             .HAlign(EHorizontalAlignment::HAlign_Left)
             .Padding(ComboMargin)[SNew(SComboButton)
                                       .OnGetMenuContent(FOnGetContent::CreateLambda(
                                           [GenerateDropDownMenu, OnOptionChanged, Property]() -> TSharedRef<SWidget> {
                                               FMenuBuilder MenuBuilder(true, nullptr);

                                               FName CurrentValue;
                                               Property->GetValue(CurrentValue);

                                               TMap<FName, FDynamicDropDownOption> Mappings =
                                                   GenerateDropDownMenu.Execute();
                                               for (const auto &KV : Mappings)
                                               {
                                                   if (KV.Value.bHiddenByDefault && CurrentValue != KV.Key)
                                                   {
                                                       // Hide this option, unless it's currently selected.
                                                       continue;
                                                   }

                                                   FUIAction UIAction;
                                                   UIAction.ExecuteAction.BindLambda(
                                                       [Property, OnOptionChanged, NewValue = KV.Key]() {
                                                           Property->SetValue(NewValue);

                                                           OnOptionChanged.ExecuteIfBound(NewValue);
                                                       });
                                                   UIAction.GetActionCheckState.BindLambda(
                                                       [Property, CheckValue = KV.Key]() -> ECheckBoxState {
                                                           FName CurrentValueCheckState;
                                                           Property->GetValue(CurrentValueCheckState);
                                                           return CurrentValueCheckState == CheckValue
                                                                      ? ECheckBoxState::Checked
                                                                      : ECheckBoxState::Unchecked;
                                                       });

                                                   MenuBuilder.AddMenuEntry(
                                                       KV.Value.DisplayText,
                                                       FText::GetEmpty(),
                                                       FSlateIcon(),
                                                       UIAction,
                                                       NAME_None,
                                                       EUserInterfaceActionType::RadioButton);
                                               }

                                               return MenuBuilder.MakeWidget();
                                           }))
                                       .ContentPadding(FMargin(2.0f, 2.0f))
                                       .ButtonContent()[SNew(STextBlock)
                                                            .Font(IDetailLayoutBuilder::GetDetailFont())
                                                            .Text_Lambda([Property, GenerateDropDownMenu]() -> FText {
                                                                FName CurrentValue;
                                                                Property->GetValue(CurrentValue);

                                                                TMap<FName, FDynamicDropDownOption> Mappings =
                                                                    GenerateDropDownMenu.Execute();

                                                                if (Mappings.Contains(CurrentValue))
                                                                {
                                                                    return Mappings[CurrentValue].DisplayText;
                                                                }
                                                                else
                                                                {
                                                                    return LOCTEXT("UnknownOptionSelected", "Unknown");
                                                                }
                                                            })]]];

    if (OnOptionChanged.IsBound())
    {
        FName CurrentValueInit;
        Property->GetValue(CurrentValueInit);
        OnOptionChanged.Execute(CurrentValueInit);
    }
}

IDetailPropertyRow &FOnlineSubsystemEOSEditorConfigDetails::GetOrAddPropertyRow(
    class IDetailLayoutBuilder &DetailBuilder,
    FName CategoryName,
    FName PropertyName)
{
    FName CombinedName(*FString::Printf(TEXT("%s_%s"), *CategoryName.ToString(), *PropertyName.ToString()));
    if (this->Rows.Contains(CombinedName))
    {
        return *this->Rows[CombinedName];
    }
    TSharedRef<IPropertyHandle> TargetProperty =
        DetailBuilder.GetProperty(PropertyName, UOnlineSubsystemEOSEditorConfig::StaticClass());
    IDetailPropertyRow &PropertyRow = this->Categories[CategoryName]->AddProperty(TargetProperty);
    this->Rows.Add(CombinedName, &PropertyRow);
    return PropertyRow;
}

void FOnlineSubsystemEOSEditorConfigDetails::DeclarePropertyOrder(
    class IDetailLayoutBuilder &DetailBuilder,
    FName CategoryName,
    const TArray<FName> &PropertyNames)
{
    for (const auto &PropName : PropertyNames)
    {
        // Create the property row so that we get the right order.
        this->GetOrAddPropertyRow(DetailBuilder, CategoryName, PropName);
    }
}

void FOnlineSubsystemEOSEditorConfigDetails::SetPropertyVisibilityCondition(
    class IDetailLayoutBuilder &DetailBuilder,
    FName CategoryName,
    FName PropertyName,
    const FComputePropertyVisibility &VisibilityCallback)
{
    TSharedRef<IPropertyHandle> TargetProperty =
        DetailBuilder.GetProperty(PropertyName, UOnlineSubsystemEOSEditorConfig::StaticClass());
    IDetailPropertyRow &PropertyRow = this->GetOrAddPropertyRow(DetailBuilder, CategoryName, PropertyName);
    TDelegate<EVisibility()> VisibilityHandler =
        TDelegate<EVisibility()>::CreateLambda([VisibilityCallback]() -> EVisibility {
            if (VisibilityCallback.Execute())
            {
                return EVisibility::Visible;
            }
            return EVisibility::Collapsed;
        });
    PropertyRow.Visibility(TAttribute<EVisibility>::Create(VisibilityHandler));
}

#undef LOCTEXT_NAMESPACE