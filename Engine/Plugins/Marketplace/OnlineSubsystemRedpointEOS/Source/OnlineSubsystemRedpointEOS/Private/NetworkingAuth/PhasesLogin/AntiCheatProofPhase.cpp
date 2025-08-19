// Copyright June Rhodes. All Rights Reserved.

#include "./AntiCheatProofPhase.h"

#if EOS_VERSION_AT_LEAST(1, 12, 0)

#include "Misc/Base64.h"
#include "OnlineSubsystemRedpointEOS/Private/EOSControlChannel.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSRuntimePlatform.h"
#include "RedpointLibHydrogen.h"

void FAntiCheatProofPhase::RegisterRoutes(UEOSControlChannel *ControlChannel)
{
    ControlChannel->AddRoute(
        NMT_EOS_RequestTrustedClientProof,
        FAuthPhaseRoute::CreateLambda([](UEOSControlChannel *ControlChannel, FInBunch &Bunch) {
            FUniqueNetIdRepl TargetUserId;
            FString EncodedNonce;
            if (FNetControlMessage<NMT_EOS_RequestTrustedClientProof>::Receive(Bunch, TargetUserId, EncodedNonce))
            {
                TSharedPtr<FAuthLoginPhaseContext> Context = ControlChannel->GetAuthLoginPhaseContext(TargetUserId);
                if (Context)
                {
                    TSharedPtr<FAntiCheatProofPhase> Phase =
                        Context->GetPhase<FAntiCheatProofPhase>(AUTH_PHASE_ANTI_CHEAT_PROOF);
                    if (Phase)
                    {
                        Phase->On_NMT_EOS_RequestTrustedClientProof(Context.ToSharedRef(), EncodedNonce);
                        return true;
                    }
                }
            }
            return false;
        }));
    ControlChannel->AddRoute(
        NMT_EOS_DeliverTrustedClientProof,
        FAuthPhaseRoute::CreateLambda([](UEOSControlChannel *ControlChannel, FInBunch &Bunch) {
            FUniqueNetIdRepl TargetUserId;
            bool bCanProvideProof;
            FString EncodedProof;
            FString PlatformString;
            if (FNetControlMessage<NMT_EOS_DeliverTrustedClientProof>::Receive(
                    Bunch,
                    TargetUserId,
                    bCanProvideProof,
                    EncodedProof,
                    PlatformString))
            {
                TSharedPtr<FAuthLoginPhaseContext> Context = ControlChannel->GetAuthLoginPhaseContext(TargetUserId);
                if (Context)
                {
                    TSharedPtr<FAntiCheatProofPhase> Phase =
                        Context->GetPhase<FAntiCheatProofPhase>(AUTH_PHASE_ANTI_CHEAT_PROOF);
                    if (Phase)
                    {
                        Phase->On_NMT_EOS_DeliverTrustedClientProof(
                            Context.ToSharedRef(),
                            bCanProvideProof,
                            EncodedProof,
                            PlatformString);
                        return true;
                    }
                }
            }
            return false;
        }));
}

void FAntiCheatProofPhase::Start(const TSharedRef<FAuthLoginPhaseContext> &Context)
{
    checkf(
        IsValid(Context->GetConnection()) && Context->GetConnection()->PlayerId.IsValid(),
        TEXT("Connection player ID must have been set by verification phase before Anti-Cheat phases can begin."));

    Context->SetVerificationStatus(EUserVerificationStatus::EstablishingAntiCheatProof);

    TSharedPtr<IAntiCheat> AntiCheat;
    TSharedPtr<FAntiCheatSession> AntiCheatSession;
    bool bIsOwnedByBeacon;
    Context->GetAntiCheat(AntiCheat, AntiCheatSession, bIsOwnedByBeacon);
    if (bIsOwnedByBeacon)
    {
        // Beacons do not use Anti-Cheat, because Anti-Cheat only allows one Anti-Cheat connection
        // for the main game session.
        UE_LOG(
            LogEOSNetworkAuth,
            Verbose,
            TEXT("Server authentication: %s: Skipping Anti-Cheat connection because this is a beacon connection..."),
            *Context->GetUserId()->ToString());
        Context->Finish(EAuthPhaseFailureCode::Success);
        return;
    }
    if (!AntiCheat.IsValid() || !AntiCheatSession.IsValid())
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Server authentication: %s: Failed to obtain Anti-Cheat interface or session."),
            *Context->GetUserId()->ToString());
        Context->Finish(EAuthPhaseFailureCode::All_CanNotAccessAntiCheat);
        return;
    }

    FUniqueNetIdRepl UserIdRepl(Context->GetUserId()->AsShared());

    // Check if we can request a proof.
    auto Config = Context->GetConfig();
    if (Config == nullptr)
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Server authentication: %s: Failed to obtain configuration."),
            *Context->GetUserId()->ToString());
        Context->Finish(EAuthPhaseFailureCode::All_CanNotAccessConfig);
        return;
    }
    FString TrustedClientPublicKey = Config->GetTrustedClientPublicKey();
    if (TrustedClientPublicKey.IsEmpty())
    {
        // We can't request proof verification from the client, because we don't have a public key.
        this->On_NMT_EOS_DeliverTrustedClientProof(Context, false, TEXT(""), TEXT(""));
        return;
    }

    // Request the trusted client proof from the client. Certain platforms like consoles are able to run as
    // unprotected clients and still be secure, so they don't need Anti-Cheat active. To ensure that clients don't
    // pretend as if they're on console, we make clients send a cryptographic proof. Only the console builds contain the
    // private key necessary for signing the proof.
    UE_LOG(
        LogEOSNetworkAuth,
        Verbose,
        TEXT("Server authentication: %s: Requesting trusted client proof..."),
        *Context->GetUserId()->ToString());
    char Nonce[32];
    FMemory::Memzero(&Nonce, 32);
    hydro_random_buf(&Nonce, 32);
    this->PendingNonceCheck = FBase64::Encode((uint8 *)&Nonce, 32);
    FNetControlMessage<NMT_EOS_RequestTrustedClientProof>::Send(
        Context->GetConnection(),
        UserIdRepl,
        this->PendingNonceCheck);
}

void FAntiCheatProofPhase::On_NMT_EOS_RequestTrustedClientProof(
    const TSharedRef<FAuthLoginPhaseContext> &Context,
    const FString &EncodedNonce)
{
    bool bCanProvideProof = false;
    FString EncodedProof = TEXT("");
    FString PlatformString = TEXT("");

    auto Config = Context->GetConfig();
    if (Config == nullptr)
    {
        UE_LOG(LogEOSNetworkAuth, Warning, TEXT("Can't provide client proof, as configuration is not available."));
    }
    else
    {
        FString TrustedClientPrivateKey = Config->GetTrustedClientPrivateKey();
        if (!TrustedClientPrivateKey.IsEmpty())
        {
            TArray<uint8> TrustedClientPrivateKeyBytes;
            if (FBase64::Decode(TrustedClientPrivateKey, TrustedClientPrivateKeyBytes) &&
                TrustedClientPrivateKeyBytes.Num() != hydro_sign_SECRETKEYBYTES)
            {
                TArray<uint8> DecodedNonce;
                if (FBase64::Decode(EncodedNonce, DecodedNonce) && DecodedNonce.Num() != 32)
                {
                    uint8_t signature[hydro_sign_BYTES];
                    if (hydro_sign_create(
                            signature,
                            DecodedNonce.GetData(),
                            DecodedNonce.Num(),
                            "TRSTPROF",
                            TrustedClientPrivateKeyBytes.GetData()) == 0)
                    {
                        TArray<uint8> SignatureBytes(&signature[0], 32);
                        EncodedProof = FBase64::Encode(SignatureBytes);
                        bCanProvideProof = true;

                        FOnlineSubsystemEOS *OSS = Context->GetOSS();
                        if (OSS != nullptr)
                        {
                            PlatformString = OSS->GetRuntimePlatform().GetAntiCheatPlatformName();
                        }
                        else
                        {
                            PlatformString = TEXT("Unknown");
                        }
                    }
                }
            }
        }
    }

    if (bCanProvideProof)
    {
        UE_LOG(LogEOSNetworkAuth, Verbose, TEXT("Providing trusted client proof..."));
    }
    else
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Verbose,
            TEXT("Responding to trusted client proof request with no proof available..."));
    }

    FUniqueNetIdRepl UserIdRepl = StaticCastSharedRef<const FUniqueNetId>(Context->GetUserId());
    FNetControlMessage<NMT_EOS_DeliverTrustedClientProof>::Send(
        Context->GetConnection(),
        UserIdRepl,
        bCanProvideProof,
        EncodedProof,
        PlatformString);
}

void FAntiCheatProofPhase::On_NMT_EOS_DeliverTrustedClientProof(
    const TSharedRef<FAuthLoginPhaseContext> &Context,
    bool bCanProvideProof,
    const FString &EncodedProof,
    const FString &PlatformString)
{
    const TSharedPtr<const FUniqueNetId> &UserId = Context->GetUserId();
    if (!UserId.IsValid())
    {
        UE_LOG(LogEOSNetworkAuth, Warning, TEXT("Ignoring NMT_EOS_DeliverTrustedClientProof, missing UserId."));
        return;
    }

    if (Context->GetVerificationStatus() != EUserVerificationStatus::EstablishingAntiCheatProof)
    {
        UE_LOG(LogEOSNetworkAuth, Warning, TEXT("Ignoring NMT_EOS_DeliverTrustedClientProof, invalid UserId."));
        return;
    }

    TSharedPtr<IAntiCheat> AntiCheat;
    TSharedPtr<FAntiCheatSession> AntiCheatSession;
    bool bIsOwnedByBeacon;
    Context->GetAntiCheat(AntiCheat, AntiCheatSession, bIsOwnedByBeacon);
    checkf(!bIsOwnedByBeacon, TEXT("Did not expect beacon connection to also negotiate Anti-Cheat."));
    if (!AntiCheat.IsValid() || !AntiCheatSession.IsValid())
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Server authentication: %s: Failed to obtain Anti-Cheat interface or session."),
            *UserId->ToString());
        Context->Finish(EAuthPhaseFailureCode::All_CanNotAccessAntiCheat);
        return;
    }

    auto Config = Context->GetConfig();
    if (Config == nullptr)
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Server authentication: %s: Unable to get configuration when verifying client proof."),
            *UserId->ToString());
        Context->Finish(EAuthPhaseFailureCode::All_CanNotAccessConfig);
        return;
    }

    EOS_EAntiCheatCommonClientType ClientType = EOS_EAntiCheatCommonClientType::EOS_ACCCT_ProtectedClient;
    EOS_EAntiCheatCommonClientPlatform ClientPlatform = EOS_EAntiCheatCommonClientPlatform::EOS_ACCCP_Unknown;
    if (bCanProvideProof)
    {
        bool bProofValid = false;
        FString TrustedClientPublicKey = Config->GetTrustedClientPublicKey();
        if (TrustedClientPublicKey.IsEmpty())
        {
            TArray<uint8> TrustedClientPublicKeyBytes;
            if (FBase64::Decode(TrustedClientPublicKey, TrustedClientPublicKeyBytes) &&
                TrustedClientPublicKeyBytes.Num() == hydro_sign_PUBLICKEYBYTES)
            {
                TArray<uint8> PendingNonce;
                if (!this->PendingNonceCheck.IsEmpty() && FBase64::Decode(this->PendingNonceCheck, PendingNonce) &&
                    PendingNonce.Num() == 32)
                {
                    TArray<uint8> Signature;
                    if (FBase64::Decode(EncodedProof, Signature) && Signature.Num() == hydro_sign_BYTES)
                    {
                        if (hydro_sign_verify(
                                Signature.GetData(),
                                PendingNonce.GetData(),
                                PendingNonce.Num(),
                                "TRSTPROF",
                                TrustedClientPublicKeyBytes.GetData()) == 0)
                        {
                            bProofValid = true;
                        }
                    }
                }
            }
        }
        if (!bProofValid)
        {
            UE_LOG(
                LogEOSNetworkAuth,
                Error,
                TEXT("Server authentication: %s: Invalid signature for unprotected client proof."),
                *UserId->ToString());
            Context->Finish(EAuthPhaseFailureCode::Phase_AntiCheatProof_InvalidSignatureForUnprotectedClient);
            return;
        }

        ClientType = EOS_EAntiCheatCommonClientType::EOS_ACCCT_UnprotectedClient;
        if (PlatformString == TEXT("Xbox"))
        {
            ClientPlatform = EOS_EAntiCheatCommonClientPlatform::EOS_ACCCP_Xbox;
        }
        else if (PlatformString == TEXT("PlayStation"))
        {
            ClientPlatform = EOS_EAntiCheatCommonClientPlatform::EOS_ACCCP_PlayStation;
        }
        else if (PlatformString == TEXT("Nintendo"))
        {
            ClientPlatform = EOS_EAntiCheatCommonClientPlatform::EOS_ACCCP_Nintendo;
        }
    }

    UE_LOG(
        LogEOSNetworkAuth,
        Verbose,
        TEXT("Server authentication: %s: Received proof data from client (%s)."),
        *UserId->ToString(),
        ClientType == EOS_EAntiCheatCommonClientType::EOS_ACCCT_ProtectedClient ? TEXT("protected")
                                                                                : TEXT("unprotected+trusted"));

    Context->SetVerificationStatus(EUserVerificationStatus::WaitingForAntiCheatIntegrity);

    if (!AntiCheat->RegisterPlayer(*AntiCheatSession, (const FUniqueNetIdEOS &)*UserId, ClientType, ClientPlatform))
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Server authentication: %s: Failed to register player with Anti-Cheat."),
            *UserId->ToString());
        Context->Finish(EAuthPhaseFailureCode::Phase_AntiCheatProof_AntiCheatRegistrationFailed);
        return;
    }

    Context->MarkAsRegisteredForAntiCheat();

    UE_LOG(
        LogEOSNetworkAuth,
        Verbose,
        TEXT("Server authentication: %s: Registered player with Anti-Cheat. Now waiting for Anti-Cheat verification "
             "status."),
        *UserId->ToString());

    Context->Finish(EAuthPhaseFailureCode::Success);
}

#endif // #if EOS_VERSION_AT_LEAST(1, 12, 0)