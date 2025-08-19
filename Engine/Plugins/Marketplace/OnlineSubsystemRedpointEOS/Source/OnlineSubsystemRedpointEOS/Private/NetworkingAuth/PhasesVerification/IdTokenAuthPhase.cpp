// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/PhasesVerification/IdTokenAuthPhase.h"

#include "OnlineSubsystemRedpointEOS/Private/EOSControlChannel.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSErrorConv.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineIdentityInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

void FIdTokenAuthPhase::RegisterRoutes(UEOSControlChannel *ControlChannel)
{
    ControlChannel->AddRoute(
        NMT_EOS_RequestIdToken,
        FAuthPhaseRoute::CreateLambda([](UEOSControlChannel *ControlChannel, FInBunch &Bunch) {
            FUniqueNetIdRepl TargetUserId;
            if (FNetControlMessage<NMT_EOS_RequestIdToken>::Receive(Bunch, TargetUserId))
            {
                TSharedPtr<FAuthVerificationPhaseContext> Context =
                    ControlChannel->GetAuthVerificationPhaseContext(TargetUserId);
                if (Context)
                {
                    TSharedPtr<FIdTokenAuthPhase> Phase =
                        Context->GetPhase<FIdTokenAuthPhase>(AUTH_PHASE_ID_TOKEN_AUTH);
                    if (Phase)
                    {
                        Phase->On_NMT_EOS_RequestIdToken(Context.ToSharedRef());
                        return true;
                    }
                }
            }
            return false;
        }));
    ControlChannel->AddRoute(
        NMT_EOS_DeliverIdToken,
        FAuthPhaseRoute::CreateLambda([](UEOSControlChannel *ControlChannel, FInBunch &Bunch) {
            FUniqueNetIdRepl TargetUserId;
            FString ClientToken;
            if (FNetControlMessage<NMT_EOS_DeliverIdToken>::Receive(Bunch, TargetUserId, ClientToken))
            {
                TSharedPtr<FAuthVerificationPhaseContext> Context =
                    ControlChannel->GetAuthVerificationPhaseContext(TargetUserId);
                if (Context)
                {
                    TSharedPtr<FIdTokenAuthPhase> Phase =
                        Context->GetPhase<FIdTokenAuthPhase>(AUTH_PHASE_ID_TOKEN_AUTH);
                    if (Phase)
                    {
                        Phase->On_NMT_EOS_DeliverIdToken(Context.ToSharedRef(), ClientToken);
                        return true;
                    }
                }
            }
            return false;
        }));
}

void FIdTokenAuthPhase::Start(const TSharedRef<FAuthVerificationPhaseContext> &Context)
{
    if (!Context->IsConnectionAsTrustedOnClient())
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Warning,
            TEXT("Skipping verification of connecting user %s because this connection is not trusted. To verify "
                 "users, turn on trusted dedicated servers."),
            *Context->GetUserId()->ToString());
        Context->GetConnection()->PlayerId = FUniqueNetIdRepl(Context->GetUserId()->AsShared());
        Context->Finish(EAuthPhaseFailureCode::Success);
        return;
    }

    // Ask the client to provide us the ID token for the given user.
    FUniqueNetIdRepl UserIdRepl(Context->GetUserId()->AsShared());
    FNetControlMessage<NMT_EOS_RequestIdToken>::Send(Context->GetConnection(), UserIdRepl);
}

void FIdTokenAuthPhase::On_NMT_EOS_RequestIdToken(const TSharedRef<FAuthVerificationPhaseContext> &Context)
{
#if EOS_HAS_AUTHENTICATION
    if (Context->GetRole() != EEOSNetDriverRole::ClientConnectedToDedicatedServer)
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting from remote host because NMT_EOS_RequestIdToken was received when it was not "
                 "expected."));
        Context->Finish(EAuthPhaseFailureCode::Msg_RequestIdToken_UnexpectedIncorrectRole);
        return;
    }
    if (!Context->GetConnection()->IsEncryptionEnabled())
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting from remote host; can not send the ID token for %s because this connection is not "
                 "encrypted."),
            *Context->GetUserId()->ToString());
        Context->Finish(EAuthPhaseFailureCode::Msg_RequestIdToken_ConnectionNotEncrypted);
        return;
    }
    if (!Context->IsConnectionAsTrustedOnClient())
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting from remote host; can not send the ID token for %s because this connection is not "
                 "trusted."),
            *Context->GetUserId()->ToString());
        Context->Finish(EAuthPhaseFailureCode::Msg_RequestIdToken_ConnectionNotTrusted);
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
        Context->Finish(EAuthPhaseFailureCode::Msg_RequestIdToken_InvalidUserId);
        return;
    }

    EOS_Connect_CopyIdTokenOptions CopyOpts = {};
    CopyOpts.ApiVersion = EOS_CONNECT_COPYIDTOKEN_API_LATEST;
    CopyOpts.LocalUserId = Context->GetUserId()->GetProductUserId();

    EOS_Connect_IdToken *IdToken = nullptr;

    EOS_EResult CopyResult =
        EOS_Connect_CopyIdToken(EOS_Platform_GetConnectInterface(OSS->GetPlatformInstance()), &CopyOpts, &IdToken);
    if (CopyResult != EOS_EResult::EOS_Success || IdToken == nullptr)
    {
        if (IdToken != nullptr)
        {
            EOS_Connect_IdToken_Release(IdToken);
        }

        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting from remote host because we could not obtain the ID token for %s, got error: %s"),
            *Context->GetUserId()->ToString(),
            *ConvertError(CopyResult).ToLogString());
        Context->Finish(EAuthPhaseFailureCode::Msg_RequestIdToken_CanNotRetrieveIdToken);
        return;
    }

    FString IdTokenStr = EOSString_AnsiUnlimited::FromAnsiString(IdToken->JsonWebToken);
    EOS_Connect_IdToken_Release(IdToken);

    FUniqueNetIdRepl TargetUserId = FUniqueNetIdRepl(Context->GetUserId()->AsShared());
    FString Token = IdTokenStr;
    FNetControlMessage<NMT_EOS_DeliverIdToken>::Send(Context->GetConnection(), TargetUserId, Token);
#else
    Context->Finish(EAuthPhaseFailureCode::All_InvalidMessageType);
#endif
}

void FIdTokenAuthPhase::On_NMT_EOS_DeliverIdToken(
    const TSharedRef<FAuthVerificationPhaseContext> &Context,
    const FString &ClientToken)
{
    if (Context->GetRole() != EEOSNetDriverRole::DedicatedServer)
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting remote host because NMT_EOS_DeliverIdToken was received when it was not "
                 "expected."));
        Context->Finish(EAuthPhaseFailureCode::Msg_DeliverIdToken_UnexpectedIncorrectRole);
        return;
    }
    if (!Context->GetConnection()->IsEncryptionEnabled())
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting client; can not receive the ID token for %s because this connection is not "
                 "encrypted."),
            *Context->GetUserId()->ToString());
        Context->Finish(EAuthPhaseFailureCode::Msg_DeliverIdToken_ConnectionNotEncrypted);
        return;
    }
    if (!Context->IsConnectionAsTrustedOnClient())
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting client; can not receive the ID token for %s because this connection is not "
                 "trusted."),
            *Context->GetUserId()->ToString());
        Context->Finish(EAuthPhaseFailureCode::Msg_DeliverIdToken_ConnectionNotTrusted);
        return;
    }

    FOnlineSubsystemEOS *OSS = Context->GetOSS();
    if (OSS == nullptr)
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting client because we could not access our online subsystem."));
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
            TEXT("Disconnecting client because we could not access the identity interface or the supplied "
                 "user ID was invalid."));
        Context->Finish(EAuthPhaseFailureCode::Msg_RequestIdToken_InvalidUserId);
        return;
    }

    auto IdTokenStr = EOSString_AnsiUnlimited::ToAnsiString(ClientToken);

    EOS_Connect_IdToken IdToken = {};
    IdToken.ApiVersion = EOS_CONNECT_IDTOKEN_API_LATEST;
    IdToken.JsonWebToken = IdTokenStr.Ptr.Get();
    IdToken.ProductUserId = Context->GetUserId()->GetProductUserId();

    EOS_Connect_VerifyIdTokenOptions VerifyOpts = {};
    VerifyOpts.ApiVersion = EOS_CONNECT_VERIFYIDTOKEN_API_LATEST;
    VerifyOpts.IdToken = &IdToken;

    EOS_HConnect EOSConnect = EOS_Platform_GetConnectInterface(OSS->GetPlatformInstance());

    EOSRunOperation<EOS_HConnect, EOS_Connect_VerifyIdTokenOptions, EOS_Connect_VerifyIdTokenCallbackInfo>(
        EOSConnect,
        &VerifyOpts,
        &EOS_Connect_VerifyIdToken,
        [WeakThis = GetWeakThis(this), Context, IdToken](const EOS_Connect_VerifyIdTokenCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Data->ResultCode == EOS_EResult::EOS_Success)
                {
                    if (*MakeShared<FUniqueNetIdEOS>(IdToken.ProductUserId) ==
                        *MakeShared<FUniqueNetIdEOS>(Data->ProductUserId))
                    {
                        // The token was valid and the user was successfully authenticated. Go to next step.
                        UE_LOG(
                            LogEOSNetworkAuth,
                            Verbose,
                            TEXT("Server authentication: %s: Successfully authenticated user with their ID token."),
                            *Context->GetUserId()->ToString());

                        Context->GetConnection()->PlayerId = FUniqueNetIdRepl(Context->GetUserId()->AsShared());
                        Context->Finish(EAuthPhaseFailureCode::Success);
                    }
                    else
                    {
                        UE_LOG(
                            LogEOSNetworkAuth,
                            Error,
                            TEXT("Server authentication: %s: ID token authentication was successful, but signed into a "
                                 "different account than the "
                                 "one the token is supposed to be for."),
                            *Context->GetUserId()->ToString());
                        Context->Finish(EAuthPhaseFailureCode::Msg_DeliverIdToken_TokenIsForADifferentAccount);
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
                    Context->Finish(EAuthPhaseFailureCode::Msg_DeliverIdToken_AuthenticationFailed);
                    return;
                }
            }
        });
}