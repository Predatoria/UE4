// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/PhasesVerification/LegacyCredentialAuthPhase.h"

#include "OnlineSubsystemRedpointEOS/Private/EOSControlChannel.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineIdentityInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

void FLegacyCredentialAuthPhase::RegisterRoutes(UEOSControlChannel *ControlChannel)
{
    ControlChannel->AddRoute(
        NMT_EOS_RequestClientToken,
        FAuthPhaseRoute::CreateLambda([](UEOSControlChannel *ControlChannel, FInBunch &Bunch) {
            FUniqueNetIdRepl TargetUserId;
            if (FNetControlMessage<NMT_EOS_RequestClientToken>::Receive(Bunch, TargetUserId))
            {
                TSharedPtr<FAuthVerificationPhaseContext> Context =
                    ControlChannel->GetAuthVerificationPhaseContext(TargetUserId);
                if (Context)
                {
                    TSharedPtr<FLegacyCredentialAuthPhase> Phase =
                        Context->GetPhase<FLegacyCredentialAuthPhase>(AUTH_PHASE_LEGACY_CREDENTIAL_AUTH);
                    if (Phase)
                    {
                        Phase->On_NMT_EOS_RequestClientToken(Context.ToSharedRef());
                        return true;
                    }
                }
            }
            return false;
        }));
    ControlChannel->AddRoute(
        NMT_EOS_DeliverClientToken,
        FAuthPhaseRoute::CreateLambda([](UEOSControlChannel *ControlChannel, FInBunch &Bunch) {
            FUniqueNetIdRepl TargetUserId;
            FString ClientTokenType;
            FString ClientDisplayName;
            FString ClientToken;
            if (FNetControlMessage<NMT_EOS_DeliverClientToken>::Receive(
                    Bunch,
                    TargetUserId,
                    ClientTokenType,
                    ClientDisplayName,
                    ClientToken))
            {
                TSharedPtr<FAuthVerificationPhaseContext> Context =
                    ControlChannel->GetAuthVerificationPhaseContext(TargetUserId);
                if (Context)
                {
                    TSharedPtr<FLegacyCredentialAuthPhase> Phase =
                        Context->GetPhase<FLegacyCredentialAuthPhase>(AUTH_PHASE_LEGACY_CREDENTIAL_AUTH);
                    if (Phase)
                    {
                        Phase->On_NMT_EOS_DeliverClientToken(
                            Context.ToSharedRef(),
                            ClientTokenType,
                            ClientDisplayName,
                            ClientToken);
                        return true;
                    }
                }
            }
            return false;
        }));
}

void FLegacyCredentialAuthPhase::Start(const TSharedRef<FAuthVerificationPhaseContext> &Context)
{
    Context->SetVerificationStatus(EUserVerificationStatus::CheckingAccountExistsFromDedicatedServer);

    if (!Context->GetConnection()->IsEncryptionEnabled())
    {
        Context->Finish(EAuthPhaseFailureCode::Phase_LegacyCredentialAuth_ConnectionNotEncrypted);
        return;
    }

    // Ask the client to provide us the token for the given user.
    FUniqueNetIdRepl UserIdRepl(Context->GetUserId()->AsShared());
    FNetControlMessage<NMT_EOS_RequestClientToken>::Send(Context->GetConnection(), UserIdRepl);
}

void FLegacyCredentialAuthPhase::On_NMT_EOS_RequestClientToken(const TSharedRef<FAuthVerificationPhaseContext> &Context)
{
#if EOS_HAS_AUTHENTICATION
    if (!Context->GetConnection()->IsEncryptionEnabled())
    {
        Context->Finish(EAuthPhaseFailureCode::Msg_RequestClientToken_ConnectionNotEncrypted);
        return;
    }

    if (!Context->GetConfig()->GetEnableTrustedDedicatedServers())
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting from remote host because NMT_EOS_RequestClientToken should not be received if trusted "
                 "dedicated servers are not enabled."));
        Context->Finish(EAuthPhaseFailureCode::Msg_RequestClientToken_UnexpectedTrustedServersNotEnabled);
        return;
    }

    if (!Context->IsConnectionAsTrustedOnClient())
    {
        // The connection is either not secure (missing encryption) or not trusted (couldn't verify server). Do not
        // provide our credentials on these connections.
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting from remote host because the server requested our credentials on an unencrypted or "
                 "untrusted connection."));
        Context->Finish(EAuthPhaseFailureCode::Msg_RequestClientToken_ConnectionNotTrusted);
        return;
    }

    FOnlineSubsystemEOS *OSS = Context->GetOSS();
    if (OSS == nullptr)
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting from remote host because we could not access our online subsystem."));
        Context->Finish(EAuthPhaseFailureCode::All_CanNotAccessOSS);
        return;
    }

    TSharedPtr<FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> Identity =
        StaticCastSharedPtr<FOnlineIdentityInterfaceEOS>(OSS->GetIdentityInterface());
    if (!Identity.IsValid() || !Context->GetUserId()->IsValid())
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting from remote host because we could not access the identity interface or the supplied "
                 "user ID was invalid."));
        Context->Finish(EAuthPhaseFailureCode::Msg_RequestClientToken_InvalidUserId);
        return;
    }

    if (!Identity->ExternalCredentials.Contains(*Context->GetUserId()))
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting from remote host because we do not have external credentials for the requested user. "
                 "This usually indicates that the method you are using to authenticate players in your game does not "
                 "supplied an IOnlineExternalCredentials instance (AddEOSConnectCandidateFromExternalCredentials). If "
                 "you are using your own cross-platform account provider, ensure you use "
                 "AddEOSConnectCandidateFromExternalCredentials instead of AddEOSConnectCandidate. Anonymous (device "
                 "ID) authentication is not supported for trusted dedicated servers, as there is no token to pass to "
                 "the server."));
        Context->Finish(EAuthPhaseFailureCode::Msg_RequestClientToken_MissingTransferrableUserCredentials);
        return;
    }

    const auto &Credentials = Identity->ExternalCredentials[*Context->GetUserId()];
    FUniqueNetIdRepl TargetUserId = FUniqueNetIdRepl(Context->GetUserId()->AsShared());
    FString TokenType = Credentials->GetType();
    FString DisplayName = Credentials->GetId();
    FString Token = Credentials->GetToken();
    FNetControlMessage<NMT_EOS_DeliverClientToken>::Send(
        Context->GetConnection(),
        TargetUserId,
        TokenType,
        DisplayName,
        Token);
#else
    Context->Finish(EAuthPhaseFailureCode::All_InvalidMessageType);
#endif // #if EOS_HAS_AUTHENTICATION
}

void FLegacyCredentialAuthPhase::On_NMT_EOS_DeliverClientToken(
    const TSharedRef<FAuthVerificationPhaseContext> &Context,
    const FString &ClientTokenType,
    const FString &ClientDisplayName,
    const FString &ClientToken)
{
    if (!Context->GetConnection()->IsEncryptionEnabled())
    {
        Context->Finish(EAuthPhaseFailureCode::Msg_DeliverClientToken_ConnectionNotEncrypted);
        return;
    }

    if (!Context->GetConfig()->GetEnableTrustedDedicatedServers())
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting from remote host because NMT_EOS_DeliverClientToken should not be received if trusted "
                 "dedicated servers are not enabled."));
        Context->Finish(EAuthPhaseFailureCode::Msg_DeliverClientToken_UnexpectedTrustedServersNotEnabled);
        return;
    }

    // We don't use any authentication helpers here, because the entire Authentication/ folder is not compiled into
    // dedicated servers. Instead, just call EOS_Connect directly.

    // @todo: Prevent this message from being handled if we're already handling it?

    FOnlineSubsystemEOS *OSS = Context->GetOSS();
    if (OSS == nullptr)
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Server authentication: Disconnecting client because we could not access our online subsystem."));
        Context->Finish(EAuthPhaseFailureCode::All_CanNotAccessOSS);
        return;
    }
    if (!Context->GetUserId()->IsValid())
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Server authentication: Disconnecting client because the user ID was invalid."));
        Context->Finish(EAuthPhaseFailureCode::Msg_DeliverClientToken_InvalidUserId);
        return;
    }

    auto Id = EOSString_Connect_UserLoginInfo_DisplayName::ToUtf8String(ClientDisplayName);
    auto Token = StringCast<ANSICHAR>(*ClientToken);

    EOS_Connect_Credentials Creds = {};
    Creds.ApiVersion = EOS_CONNECT_CREDENTIALS_API_LATEST;
    Creds.Token = Token.Get();
    Creds.Type = StrToExternalCredentialType(ClientTokenType);

    EOS_Connect_UserLoginInfo LoginInfo = {};
    LoginInfo.ApiVersion = EOS_CONNECT_USERLOGININFO_API_LATEST;
    LoginInfo.DisplayName = Id.GetAsChar();

    EOS_Connect_LoginOptions Opts = {};
    Opts.ApiVersion = EOS_CONNECT_LOGIN_API_LATEST;
    Opts.Credentials = &Creds;
    if (ClientDisplayName.IsEmpty())
    {
        Opts.UserLoginInfo = nullptr;
    }
    else
    {
        Opts.UserLoginInfo = &LoginInfo;
    }

    EOS_HConnect EOSConnect = EOS_Platform_GetConnectInterface(OSS->GetActAsPlatformInstance());

    EOSRunOperation<EOS_HConnect, EOS_Connect_LoginOptions, EOS_Connect_LoginCallbackInfo>(
        EOSConnect,
        &Opts,
        &EOS_Connect_Login,
        [WeakThis = GetWeakThis(this), Context](const EOS_Connect_LoginCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Data->ResultCode == EOS_EResult::EOS_Success)
                {
                    // Check that we were signed into the same account.
                    if (*Context->GetUserId() == *MakeShared<FUniqueNetIdEOS>(Data->LocalUserId))
                    {
                        // The token was valid and the user was successfully authenticated. Go to next step.
                        UE_LOG(
                            LogEOSNetworkAuth,
                            Verbose,
                            TEXT("Server authentication: %s: Successfully authenticated user with their EOS Connect "
                                 "token."),
                            *Context->GetUserId()->ToString());

                        Context->GetConnection()->PlayerId = FUniqueNetIdRepl(Context->GetUserId()->AsShared());
                        Context->Finish(EAuthPhaseFailureCode::Success);
                    }
                    else
                    {
                        UE_LOG(
                            LogEOSNetworkAuth,
                            Error,
                            TEXT("Server authentication: %s: Token authentication was successful, but signed into a "
                                 "different account than the "
                                 "one the token is supposed to be for."),
                            *Context->GetUserId()->ToString());
                        Context->Finish(EAuthPhaseFailureCode::Msg_DeliverClientToken_TokenIsForADifferentAccount);
                        return;
                    }
                }
                else
                {
                    UE_LOG(
                        LogEOSNetworkAuth,
                        Error,
                        TEXT("Server authentication: %s: Failed to verify user, got result code %s on server"),
                        *Context->GetUserId()->ToString(),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
                    Context->Finish(EAuthPhaseFailureCode::Msg_DeliverClientToken_AuthenticationFailed);
                    return;
                }
            }
        });
}