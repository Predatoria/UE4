// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationHelpers.h"

#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

EOS_ENABLE_STRICT_WARNINGS

EOS_EExternalCredentialType StrToExternalCredentialType(const FString &InStr)
{
    if (InStr.ToUpper() == TEXT("EPIC"))
    {
        return EOS_EExternalCredentialType::EOS_ECT_EPIC;
    }
    else if (InStr.ToUpper() == TEXT("STEAM_APP_TICKET"))
    {
        return EOS_EExternalCredentialType::EOS_ECT_STEAM_APP_TICKET;
    }
    else if (InStr.ToUpper() == TEXT("PSN_ID_TOKEN"))
    {
        return EOS_EExternalCredentialType::EOS_ECT_PSN_ID_TOKEN;
    }
    else if (InStr.ToUpper() == TEXT("XBL_XSTS_TOKEN"))
    {
        return EOS_EExternalCredentialType::EOS_ECT_XBL_XSTS_TOKEN;
    }
    else if (InStr.ToUpper() == TEXT("DISCORD_ACCESS_TOKEN"))
    {
        return EOS_EExternalCredentialType::EOS_ECT_DISCORD_ACCESS_TOKEN;
    }
    else if (InStr.ToUpper() == TEXT("GOG_SESSION_TICKET"))
    {
        return EOS_EExternalCredentialType::EOS_ECT_GOG_SESSION_TICKET;
    }
    else if (InStr.ToUpper() == TEXT("NINTENDO_ID_TOKEN"))
    {
        return EOS_EExternalCredentialType::EOS_ECT_NINTENDO_ID_TOKEN;
    }
    else if (InStr.ToUpper() == TEXT("NINTENDO_NSA_ID_TOKEN"))
    {
        return EOS_EExternalCredentialType::EOS_ECT_NINTENDO_NSA_ID_TOKEN;
    }
    else if (InStr.ToUpper() == TEXT("UPLAY_ACCESS_TOKEN"))
    {
        return EOS_EExternalCredentialType::EOS_ECT_UPLAY_ACCESS_TOKEN;
    }
    else if (InStr.ToUpper() == TEXT("OPENID_ACCESS_TOKEN"))
    {
        return EOS_EExternalCredentialType::EOS_ECT_OPENID_ACCESS_TOKEN;
    }
    else if (InStr.ToUpper() == TEXT("DEVICEID_ACCESS_TOKEN"))
    {
        return EOS_EExternalCredentialType::EOS_ECT_DEVICEID_ACCESS_TOKEN;
    }
    else if (InStr.ToUpper() == TEXT("APPLE_ID_TOKEN"))
    {
        return EOS_EExternalCredentialType::EOS_ECT_APPLE_ID_TOKEN;
    }
#if EOS_VERSION_AT_LEAST(1, 11, 0)
    else if (InStr.ToUpper() == TEXT("GOOGLE_ID_TOKEN"))
    {
        return EOS_EExternalCredentialType::EOS_ECT_GOOGLE_ID_TOKEN;
    }
#endif
#if EOS_VERSION_AT_LEAST(1, 10, 3)
    else if (InStr.ToUpper() == TEXT("OCULUS_USERID_NONCE"))
    {
        return EOS_EExternalCredentialType::EOS_ECT_OCULUS_USERID_NONCE;
    }
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    else if (InStr.ToUpper() == TEXT("ITCHIO_JWT"))
    {
        return EOS_EExternalCredentialType::EOS_ECT_ITCHIO_JWT;
    }
    else if (InStr.ToUpper() == TEXT("ITCHIO_KEY"))
    {
        return EOS_EExternalCredentialType::EOS_ECT_ITCHIO_KEY;
    }
#endif
#if EOS_VERSION_AT_LEAST(1, 15, 1) ||                                                                                  \
    (EOS_VERSION_AT_LEAST(1, 14, 2) && EOS_VERSION_AT_MOST(1, 15, 0) && !EOS_VERSION_AT_LEAST(1, 15, 0) &&             \
     defined(EOS_HOTFIX_VERSION) && EOS_HOTFIX_VERSION >= 2)
    else if (InStr.ToUpper() == TEXT("STEAM_SESSION_TICKET"))
    {
        return EOS_EExternalCredentialType::EOS_ECT_STEAM_SESSION_TICKET;
    }
#endif

    checkf(false, TEXT("Expected known credential type for StrToExternalCredentialType"));
    return EOS_EExternalCredentialType::EOS_ECT_EPIC;
}

EOS_DISABLE_STRICT_WARNINGS

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"

#ifdef PLATFORM_IOS
#if defined(EOS_IOS_REQUIRES_PRESENTATIONCONTEXT) && EOS_IOS_REQUIRES_PRESENTATIONCONTEXT
#include "IOS/IOSAppDelegate.h"
#import <AuthenticationServices/AuthenticationServices.h>
#endif
#endif

EOS_ENABLE_STRICT_WARNINGS

#ifdef PLATFORM_IOS
#if defined(EOS_IOS_REQUIRES_PRESENTATIONCONTEXT) && EOS_IOS_REQUIRES_PRESENTATIONCONTEXT

@interface PresentationContext : NSObject <ASWebAuthenticationPresentationContextProviding>
{
}
@end

@implementation PresentationContext

- (ASPresentationAnchor)presentationAnchorForWebAuthenticationSession:(ASWebAuthenticationSession *)session
{
    if ([IOSAppDelegate GetDelegate].Window == nullptr)
    {
        NSLog(@"authorizationController: presentationAnchorForAuthorizationController: error window is NULL");
    }
    return [IOSAppDelegate GetDelegate].Window;
}

@end

static PresentationContext *PresentationContextProvider = nullptr;

#endif
#endif

FString ExternalCredentialTypeToStr(EOS_EExternalCredentialType InType)
{
    if (InType == EOS_EExternalCredentialType::EOS_ECT_EPIC)
    {
        return TEXT("EPIC");
    }
    else if (InType == EOS_EExternalCredentialType::EOS_ECT_STEAM_APP_TICKET)
    {
        return TEXT("STEAM_APP_TICKET");
    }
    else if (InType == EOS_EExternalCredentialType::EOS_ECT_PSN_ID_TOKEN)
    {
        return TEXT("PSN_ID_TOKEN");
    }
    else if (InType == EOS_EExternalCredentialType::EOS_ECT_XBL_XSTS_TOKEN)
    {
        return TEXT("XBL_XSTS_TOKEN");
    }
    else if (InType == EOS_EExternalCredentialType::EOS_ECT_DISCORD_ACCESS_TOKEN)
    {
        return TEXT("DISCORD_ACCESS_TOKEN");
    }
    else if (InType == EOS_EExternalCredentialType::EOS_ECT_GOG_SESSION_TICKET)
    {
        return TEXT("GOG_SESSION_TICKET");
    }
    else if (InType == EOS_EExternalCredentialType::EOS_ECT_NINTENDO_ID_TOKEN)
    {
        return TEXT("NINTENDO_ID_TOKEN");
    }
    else if (InType == EOS_EExternalCredentialType::EOS_ECT_NINTENDO_NSA_ID_TOKEN)
    {
        return TEXT("NINTENDO_NSA_ID_TOKEN");
    }
    else if (InType == EOS_EExternalCredentialType::EOS_ECT_UPLAY_ACCESS_TOKEN)
    {
        return TEXT("UPLAY_ACCESS_TOKEN");
    }
    else if (InType == EOS_EExternalCredentialType::EOS_ECT_OPENID_ACCESS_TOKEN)
    {
        return TEXT("OPENID_ACCESS_TOKEN");
    }
    else if (InType == EOS_EExternalCredentialType::EOS_ECT_DEVICEID_ACCESS_TOKEN)
    {
        return TEXT("DEVICEID_ACCESS_TOKEN");
    }
    else if (InType == EOS_EExternalCredentialType::EOS_ECT_APPLE_ID_TOKEN)
    {
        return TEXT("APPLE_ID_TOKEN");
    }
#if EOS_VERSION_AT_LEAST(1, 11, 0)
    else if (InType == EOS_EExternalCredentialType::EOS_ECT_GOOGLE_ID_TOKEN)
    {
        return TEXT("GOOGLE_ID_TOKEN");
    }
#endif
#if EOS_VERSION_AT_LEAST(1, 10, 3)
    else if (InType == EOS_EExternalCredentialType::EOS_ECT_OCULUS_USERID_NONCE)
    {
        return TEXT("OCULUS_USERID_NONCE");
    }
#endif
#if EOS_VERSION_AT_LEAST(1, 12, 0)
    else if (InType == EOS_EExternalCredentialType::EOS_ECT_ITCHIO_JWT)
    {
        return TEXT("ITCHIO_JWT");
    }
    else if (InType == EOS_EExternalCredentialType::EOS_ECT_ITCHIO_KEY)
    {
        return TEXT("ITCHIO_KEY");
    }
#endif

    checkf(false, TEXT("Expected known credential type for ExternalCredentialTypeToStr"));
    return TEXT("");
}

void FEASAuthentication::DoRequest(
    EOS_HAuth EOSAuth,
    const FString &Id,
    const FString &Token,
    EOS_ELoginCredentialType Type,
    const FEASAuth_DoRequestComplete &OnComplete)
{
    auto TokenData = StringCast<ANSICHAR>(*Token);
    auto IdData = EOSString_Connect_UserLoginInfo_DisplayName::ToUtf8String(Id);

    EOS_Auth_Credentials Creds = {};
    Creds.ApiVersion = EOS_AUTH_CREDENTIALS_API_LATEST;
    Creds.Id = ((Id == TEXT("")) ? nullptr : IdData.GetAsChar());
    Creds.Token = ((Token == TEXT("")) ? nullptr : TokenData.Get());
    Creds.Type = Type;
#if PLATFORM_IOS
    EOS_IOS_Auth_CredentialsOptions IOSOpts = {};
    IOSOpts.ApiVersion = EOS_IOS_AUTH_CREDENTIALSOPTIONS_API_LATEST;
#if defined(EOS_IOS_REQUIRES_PRESENTATIONCONTEXT) && EOS_IOS_REQUIRES_PRESENTATIONCONTEXT
    if (@available(iOS 13.0, *))
    {
        if (PresentationContextProvider != nil)
        {
            [PresentationContextProvider release];
            PresentationContextProvider = nil;
        }
        PresentationContextProvider = [PresentationContext new];
        IOSOpts.PresentationContextProviding = (void *)CFBridgingRetain(PresentationContextProvider);
    }
    else
    {
        IOSOpts.PresentationContextProviding = nullptr;
    }
#else
    IOSOpts.PresentationContextProviding = nullptr;
#endif
    Creds.SystemAuthCredentialsOptions = &IOSOpts;
#endif

    EOS_Auth_LoginOptions Opts = {};
    Opts.ApiVersion = EOS_AUTH_LOGIN_API_LATEST;
    Opts.Credentials = &Creds;
    Opts.ScopeFlags = EOS_EAuthScopeFlags::EOS_AS_BasicProfile | EOS_EAuthScopeFlags::EOS_AS_FriendsList |
                      EOS_EAuthScopeFlags::EOS_AS_Presence;

    UE_LOG(LogEOS, Verbose, TEXT("EOSAuth_DoRequest: Request started"));

    EOSRunOperationKeepAlive<EOS_HAuth, EOS_Auth_LoginOptions, EOS_Auth_LoginCallbackInfo>(
        EOSAuth,
        &Opts,
        &EOS_Auth_Login,
        [OnComplete](const EOS_Auth_LoginCallbackInfo *Data, bool &KeepAlive) {
            UE_LOG(
                LogEOS,
                Verbose,
                TEXT("EOSAuth_DoRequest: Request finished (result code %s)"),
                ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));

            if (Data->ResultCode == EOS_EResult::EOS_Auth_PinGrantCode && Data->PinGrantInfo != nullptr)
            {
                // The SDK will run the callback again later, so keep the callback allocated.
                KeepAlive = true;
            }

            OnComplete.ExecuteIfBound(Data);
        });
}

void FEASAuthentication::DoRequestLink(
    EOS_HAuth EOSAuth,
    EOS_ContinuanceToken ContinuanceToken,
    EOS_ELinkAccountFlags LinkAccountFlags,
    EOS_EpicAccountId LocalUserId,
    const FEASAuth_DoRequestLinkComplete &OnComplete)
{
    EOS_Auth_LinkAccountOptions Opts = {};
    Opts.ApiVersion = EOS_AUTH_LINKACCOUNT_API_LATEST;
    Opts.ContinuanceToken = ContinuanceToken;
    Opts.LinkAccountFlags = LinkAccountFlags;
    Opts.LocalUserId = LocalUserId;

    UE_LOG(LogEOS, Verbose, TEXT("EOSAuth_DoRequestLink: Request started"));

    EOSRunOperationKeepAlive<EOS_HAuth, EOS_Auth_LinkAccountOptions, EOS_Auth_LinkAccountCallbackInfo>(
        EOSAuth,
        &Opts,
        &EOS_Auth_LinkAccount,
        [OnComplete](const EOS_Auth_LinkAccountCallbackInfo *Data, bool &KeepAlive) {
            UE_LOG(
                LogEOS,
                Verbose,
                TEXT("EOSAuth_DoRequestLink: Request finished (result code %s)"),
                ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));

            if (Data->ResultCode == EOS_EResult::EOS_Auth_PinGrantCode && Data->PinGrantInfo != nullptr)
            {
                // The SDK will run the callback again later, so keep the callback allocated.
                KeepAlive = true;
            }

            OnComplete.ExecuteIfBound(Data);
        });
}

void FEASAuthentication::DoRequestExternal(
    EOS_HAuth EOSAuth,
    const FString &Id,
    const FString &Token,
    EOS_EExternalCredentialType ExternalType,
    const FEASAuth_DoRequestComplete &OnComplete)
{
    auto TokenData = StringCast<ANSICHAR>(*Token);
    auto IdData = EOSString_Connect_UserLoginInfo_DisplayName::ToUtf8String(Id);

    EOS_Auth_Credentials Creds = {};
    Creds.ApiVersion = EOS_AUTH_CREDENTIALS_API_LATEST;
    Creds.Id = ((Id == TEXT("")) ? nullptr : IdData.GetAsChar());
    Creds.Token = ((Token == TEXT("")) ? nullptr : TokenData.Get());
    Creds.Type = EOS_ELoginCredentialType::EOS_LCT_ExternalAuth;
    Creds.ExternalType = ExternalType;
#if PLATFORM_IOS
    EOS_IOS_Auth_CredentialsOptions IOSOpts = {};
    IOSOpts.ApiVersion = EOS_IOS_AUTH_CREDENTIALSOPTIONS_API_LATEST;
#if defined(EOS_IOS_REQUIRES_PRESENTATIONCONTEXT) && EOS_IOS_REQUIRES_PRESENTATIONCONTEXT
    if (@available(iOS 13.0, *))
    {
        if (PresentationContextProvider != nil)
        {
            [PresentationContextProvider release];
            PresentationContextProvider = nil;
        }
        PresentationContextProvider = [PresentationContext new];
        IOSOpts.PresentationContextProviding = (void *)CFBridgingRetain(PresentationContextProvider);
    }
    else
    {
        IOSOpts.PresentationContextProviding = nullptr;
    }
#else
    IOSOpts.PresentationContextProviding = nullptr;
#endif
    Creds.SystemAuthCredentialsOptions = &IOSOpts;
#endif

    EOS_Auth_LoginOptions Opts = {};
    Opts.ApiVersion = EOS_AUTH_LOGIN_API_LATEST;
    Opts.Credentials = &Creds;
    Opts.ScopeFlags = EOS_EAuthScopeFlags::EOS_AS_BasicProfile | EOS_EAuthScopeFlags::EOS_AS_FriendsList |
                      EOS_EAuthScopeFlags::EOS_AS_Presence;

    UE_LOG(LogEOS, Verbose, TEXT("EOSAuth_DoRequestExternal: Request started"));

    EOSRunOperation<EOS_HAuth, EOS_Auth_LoginOptions, EOS_Auth_LoginCallbackInfo>(
        EOSAuth,
        &Opts,
        &EOS_Auth_Login,
        [OnComplete](const EOS_Auth_LoginCallbackInfo *Data) {
            UE_LOG(
                LogEOS,
                Verbose,
                TEXT("EOSAuth_DoRequestExternal: Request finished (result code %s)"),
                ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));

            OnComplete.ExecuteIfBound(Data);
        });
}

void FEOSAuthentication::DoRequest(
    EOS_HConnect EOSConnect,
    const FString &Id,
    const FString &Token,
    EOS_EExternalCredentialType Type,
    const FEOSAuth_DoRequestComplete &OnComplete)
{
    auto TokenData = StringCast<ANSICHAR>(*Token);
    auto IdData = EOSString_Connect_UserLoginInfo_DisplayName::ToUtf8String(Id);

    EOS_Connect_Credentials Creds = {};
    Creds.ApiVersion = EOS_CONNECT_CREDENTIALS_API_LATEST;
    Creds.Token = TokenData.Get();
    Creds.Type = Type;

    EOS_Connect_UserLoginInfo LoginInfo = {};
    LoginInfo.ApiVersion = EOS_CONNECT_USERLOGININFO_API_LATEST;
    LoginInfo.DisplayName = IdData.GetAsChar();

    EOS_Connect_LoginOptions Opts = {};
    Opts.ApiVersion = EOS_CONNECT_LOGIN_API_LATEST;
    Opts.Credentials = &Creds;
    if (Creds.Type == EOS_EExternalCredentialType::EOS_ECT_NINTENDO_ID_TOKEN ||
        Creds.Type == EOS_EExternalCredentialType::EOS_ECT_NINTENDO_NSA_ID_TOKEN ||
        Creds.Type == EOS_EExternalCredentialType::EOS_ECT_APPLE_ID_TOKEN ||
#if EOS_VERSION_AT_LEAST(1, 10, 3)
        Creds.Type == EOS_EExternalCredentialType::EOS_ECT_OCULUS_USERID_NONCE ||
#endif
#if EOS_VERSION_AT_LEAST(1, 11, 0)
        Creds.Type == EOS_EExternalCredentialType::EOS_ECT_GOOGLE_ID_TOKEN ||
#endif
        Creds.Type == EOS_EExternalCredentialType::EOS_ECT_DEVICEID_ACCESS_TOKEN)
    {
        Opts.UserLoginInfo = &LoginInfo;
    }
    else
    {
        Opts.UserLoginInfo = nullptr;
    }

    UE_LOG(LogEOS, Verbose, TEXT("EOSConnect_DoRequest: Request started"));

    EOSRunOperation<EOS_HConnect, EOS_Connect_LoginOptions, EOS_Connect_LoginCallbackInfo>(
        EOSConnect,
        &Opts,
        &EOS_Connect_Login,
        [OnComplete](const EOS_Connect_LoginCallbackInfo *Data) {
            UE_LOG(
                LogEOS,
                Verbose,
                TEXT("EOSConnect_DoRequest: Request finished (result code %s)"),
                ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));

            OnComplete.ExecuteIfBound(Data);
        });
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION