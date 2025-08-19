// Copyright June Rhodes. All Rights Reserved.

#include "AuthenticationEOSConfigurer.h"

#define READ_WIDGET_CLASS(Class)                                                                                       \
    if (Reader.GetString(TEXT("WidgetClass_") TEXT(#Class), Value))                                                    \
    {                                                                                                                  \
        Instance->WidgetClass_##Class = FSoftClassPath(*Value);                                                        \
    }
#define READ_WIDGET_CLASS_LEGACY(Class, NewClass)                                                                      \
    if (Reader.GetString(TEXT("WidgetClass_") TEXT(#Class), Value))                                                    \
    {                                                                                                                  \
        Instance->WidgetClass_##NewClass = FSoftClassPath(*Value);                                                     \
    }
#define WRITE_WIDGET_CLASS(Class)                                                                                      \
    Writer.SetString(TEXT("WidgetClass_") TEXT(#Class), *Instance->WidgetClass_##Class.ToString());

void FAuthenticationEOSConfigurer::InitDefaults(
    FEOSConfigurerContext &Context,
    UOnlineSubsystemEOSEditorConfig *Instance)
{
    Instance->DevAuthToolAddress = TEXT("localhost:6300");
    Instance->DevAuthToolDefaultCredentialName = TEXT("Context_1");
    Instance->AuthenticationGraph = NAME_Default;
    Instance->EditorAuthenticationGraph = NAME_Default;
    Instance->CrossPlatformAccountProvider = NAME_None;
    Instance->RequireCrossPlatformAccount = false;
    Instance->SimpleFirstPartyLoginUrl = TEXT("");

    // NOTE: These defaults need to be kept in sync with the authentication graph.
    Instance->WidgetClass_EnterDevicePinCode = FSoftClassPath(
        FString(TEXT("/OnlineSubsystemRedpointEOS/"
                     "EOSDefaultUserInterface_EnterDevicePinCode.EOSDefaultUserInterface_EnterDevicePinCode_C")));
    Instance->WidgetClass_SignInOrCreateAccount =
        FSoftClassPath(FString(TEXT("/OnlineSubsystemRedpointEOS/"
                                    "EOSDefaultUserInterface_SignInOrCreateAccount.EOSDefaultUserInterface_"
                                    "SignInOrCreateAccount_C")));
}

void FAuthenticationEOSConfigurer::Load(
    FEOSConfigurerContext &Context,
    FEOSConfigurationReader &Reader,
    UOnlineSubsystemEOSEditorConfig *Instance)
{
    FString Value;
    if (Reader.GetString(TEXT("AuthenticationGraph"), Value))
    {
        Instance->AuthenticationGraph = FName(*Value);
    }
    else if (Reader.GetString(TEXT("AuthenticationBehaviour"), Value))
    {
        // We are migrating this project from the old AuthenticationBehaviour value to the new settings.
        if (Value == "NoEAS")
        {
            Instance->CrossPlatformAccountProvider = NAME_None;
            Instance->RequireCrossPlatformAccount = false;
        }
        else if (Value == "EASOptional")
        {
            Instance->CrossPlatformAccountProvider = FName(TEXT("EpicGames"));
            Instance->RequireCrossPlatformAccount = false;
        }
        else if (Value == "EASRequired")
        {
            Instance->CrossPlatformAccountProvider = FName(TEXT("EpicGames"));
            Instance->RequireCrossPlatformAccount = true;
        }
    }
    if (Reader.GetString(TEXT("EditorAuthenticationGraph"), Value))
    {
        Instance->EditorAuthenticationGraph = FName(*Value);
    }
    if (Reader.GetString(TEXT("CrossPlatformAccountProvider"), Value))
    {
        Instance->CrossPlatformAccountProvider = FName(*Value);
    }
    Reader.GetBool(TEXT("RequireCrossPlatformAccount"), Instance->RequireCrossPlatformAccount);
    Reader.GetBool(TEXT("DisablePersistentLogin"), Instance->DisablePersistentLogin);
    Reader.GetString(TEXT("DevAuthToolAddress"), Instance->DevAuthToolAddress);
    Reader.GetString(TEXT("DevAuthToolDefaultCredentialName"), Instance->DevAuthToolDefaultCredentialName);
    Reader.GetString(TEXT("SimpleFirstPartyLoginUrl"), Instance->SimpleFirstPartyLoginUrl);

    READ_WIDGET_CLASS(EnterDevicePinCode);
    READ_WIDGET_CLASS(SignInOrCreateAccount);
}

void FAuthenticationEOSConfigurer::Save(
    FEOSConfigurerContext &Context,
    FEOSConfigurationWriter &Writer,
    UOnlineSubsystemEOSEditorConfig *Instance)
{
    Writer.SetString(TEXT("AuthenticationGraph"), *Instance->AuthenticationGraph.ToString());
    Writer.SetString(TEXT("EditorAuthenticationGraph"), *Instance->EditorAuthenticationGraph.ToString());
    Writer.SetString(TEXT("CrossPlatformAccountProvider"), *Instance->CrossPlatformAccountProvider.ToString());
    Writer.SetBool(TEXT("RequireCrossPlatformAccount"), Instance->RequireCrossPlatformAccount);
    Writer.SetBool(TEXT("DisablePersistentLogin"), Instance->DisablePersistentLogin);
    Writer.SetString(TEXT("DevAuthToolAddress"), *Instance->DevAuthToolAddress);
    Writer.SetString(TEXT("DevAuthToolDefaultCredentialName"), *Instance->DevAuthToolDefaultCredentialName);
    Writer.SetString(TEXT("SimpleFirstPartyLoginUrl"), *Instance->SimpleFirstPartyLoginUrl);

    WRITE_WIDGET_CLASS(EnterDevicePinCode);
    WRITE_WIDGET_CLASS(SignInOrCreateAccount);
}