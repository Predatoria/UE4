// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/PhasesConnection/AutomaticEncryptionPhase.h"

#include "Misc/Base64.h"
#include "OnlineSubsystemRedpointEOS/Private/EOSControlChannel.h"

void IAutomaticEncryptionCommon::RegisterRoutes(UEOSControlChannel *ControlChannel)
{
    ControlChannel->AddRoute(
        NMT_EOS_RequestClientEphemeralKey,
        FAuthPhaseRoute::CreateLambda([](UEOSControlChannel *ControlChannel, FInBunch &Bunch) {
            TSharedPtr<FAuthConnectionPhaseContext> Context = ControlChannel->GetAuthConnectionPhaseContext();
            if (Context)
            {
                Context->Log(TEXT("Received NMT_EOS_RequestClientEphemeralKey."));
                auto Phase = Context->GetPhase<IAutomaticEncryptionCommon>(AUTH_PHASE_AUTOMATIC_ENCRYPTION);
                if (Phase.IsValid())
                {
                    Phase->On_NMT_EOS_RequestClientEphemeralKey(Context.ToSharedRef(), Bunch);
                    return true;
                }
            }
            return false;
        }));
    ControlChannel->AddRoute(
        NMT_EOS_DeliverClientEphemeralKey,
        FAuthPhaseRoute::CreateLambda([](UEOSControlChannel *ControlChannel, FInBunch &Bunch) {
            TSharedPtr<FAuthConnectionPhaseContext> Context = ControlChannel->GetAuthConnectionPhaseContext();
            if (Context)
            {
                Context->Log(TEXT("Received NMT_EOS_DeliverClientEphemeralKey."));
                auto Phase = Context->GetPhase<IAutomaticEncryptionCommon>(AUTH_PHASE_AUTOMATIC_ENCRYPTION);
                if (Phase.IsValid())
                {
                    Phase->On_NMT_EOS_DeliverClientEphemeralKey(Context.ToSharedRef(), Bunch);
                    return true;
                }
            }
            return false;
        }));
    ControlChannel->AddRoute(
        NMT_EOS_SymmetricKeyExchange,
        FAuthPhaseRoute::CreateLambda([](UEOSControlChannel *ControlChannel, FInBunch &Bunch) {
            TSharedPtr<FAuthConnectionPhaseContext> Context = ControlChannel->GetAuthConnectionPhaseContext();
            if (Context)
            {
                Context->Log(TEXT("Received NMT_EOS_SymmetricKeyExchange."));
                auto Phase = Context->GetPhase<IAutomaticEncryptionCommon>(AUTH_PHASE_AUTOMATIC_ENCRYPTION);
                if (Phase.IsValid())
                {
                    Phase->On_NMT_EOS_SymmetricKeyExchange(Context.ToSharedRef(), Bunch);
                    return true;
                }
            }
            return false;
        }));
    ControlChannel->AddRoute(
        NMT_EOS_EnableEncryption,
        FAuthPhaseRoute::CreateLambda([](UEOSControlChannel *ControlChannel, FInBunch &Bunch) {
            TSharedPtr<FAuthConnectionPhaseContext> Context = ControlChannel->GetAuthConnectionPhaseContext();
            if (Context)
            {
                Context->Log(TEXT("Received NMT_EOS_EnableEncryption."));
                auto Phase = Context->GetPhase<IAutomaticEncryptionCommon>(AUTH_PHASE_AUTOMATIC_ENCRYPTION);
                if (Phase.IsValid())
                {
                    Phase->On_NMT_EOS_EnableEncryption(Context.ToSharedRef(), Bunch);
                    return true;
                }
            }
            return false;
        }));
}

#if !defined(EOS_AUTOMATIC_ENCRYPTION_UNAVAILABLE)

FAutomaticEncryptionPhase::FAutomaticEncryptionPhase()
    : server_connection_unique_kp()
    , client_session_kp()
    , server_session_kp()
    , AESGCMKey()
{
    FMemory::Memzero(&this->server_connection_unique_kp, sizeof(this->server_connection_unique_kp));
    FMemory::Memzero(&this->client_session_kp, sizeof(this->client_session_kp));
    FMemory::Memzero(&this->server_session_kp, sizeof(this->server_session_kp));
    FMemory::Memzero(&this->AESGCMKey, sizeof(this->AESGCMKey));
}

void FAutomaticEncryptionPhase::Start(const TSharedRef<FAuthConnectionPhaseContext> &Context)
{
    // Load the public/private signing keypair from configuration.
    bool bLoadedKey = false;
    hydro_sign_keypair server_key_pair = {};
    const FEOSConfig *Config = Context->GetConfig();
    if (Config != nullptr)
    {
        FString PublicKey = Config->GetDedicatedServerPublicKey();
        FString PrivateKey = Config->GetDedicatedServerPrivateKey();
        if (!PublicKey.IsEmpty() && !PrivateKey.IsEmpty())
        {
            TArray<uint8> PublicKeyBytes, PrivateKeyBytes;
            if (FBase64::Decode(PublicKey, PublicKeyBytes) && FBase64::Decode(PrivateKey, PrivateKeyBytes))
            {
                if (PublicKeyBytes.Num() == sizeof(server_key_pair.pk) &&
                    PrivateKeyBytes.Num() == sizeof(server_key_pair.sk))
                {
                    FMemory::Memcpy(server_key_pair.pk, PublicKeyBytes.GetData(), sizeof(server_key_pair.pk));
                    FMemory::Memcpy(server_key_pair.sk, PrivateKeyBytes.GetData(), sizeof(server_key_pair.sk));
                    bLoadedKey = true;
                }
            }
        }
    }
    if (!bLoadedKey)
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Warning,
            TEXT("The dedicated server had no public/private signing keypair set, so the connection will not be "
                 "automatically encrypted."));
        Context->Finish(EAuthPhaseFailureCode::Success);
        return;
    }

    // Generate a unique public/private keypair so that the client can encrypt it's public key specifically for this
    // server (in a way that would not be interceptable by another dedicated server or even an attacker with the signing
    // private key).
    FMemory::Memzero(&this->server_connection_unique_kp, sizeof(this->server_connection_unique_kp));
    hydro_kx_keygen(&this->server_connection_unique_kp);

    // Sign the session public key with the signing key.
    uint8_t signature[hydro_sign_BYTES];
    if (hydro_sign_create(
            signature,
            this->server_connection_unique_kp.pk,
            sizeof(this->server_connection_unique_kp.pk),
            "SIGNING0",
            server_key_pair.sk) != 0)
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Rejecting player connection on dedicated server because the dedicated server could not sign the "
                 "connection keypair."));
        Context->Finish(EAuthPhaseFailureCode::Phase_AutomaticEncryption_FailedToSignConnectionKeyPair);
        return;
    }

    // Send the signed public key to the client for them to verify and use.
    Context->Log(TEXT("Sending NMT_EOS_RequestClientEphemeralKey to client..."));
    FString EncodedConnectionPublicKey = FBase64::Encode(
        TArray<uint8>(this->server_connection_unique_kp.pk, sizeof(this->server_connection_unique_kp.pk)));
    FString EncodedConnectionPublicKeySignature = FBase64::Encode(TArray<uint8>(signature, sizeof(signature)));
    FNetControlMessage<NMT_EOS_RequestClientEphemeralKey>::Send(
        Context->GetConnection(),
        EncodedConnectionPublicKey,
        EncodedConnectionPublicKeySignature);
    Context->GetConnection()->FlushNet(true);
}

void FAutomaticEncryptionPhase::On_NMT_EOS_RequestClientEphemeralKey(
    const TSharedRef<FAuthConnectionPhaseContext> &Context,
    FInBunch &Bunch)
{
    FString ServerConnectionPublicKey;
    FString ServerConnectionPublicKeySignature;
    if (!FNetControlMessage<NMT_EOS_RequestClientEphemeralKey>::Receive(
            Bunch,
            ServerConnectionPublicKey,
            ServerConnectionPublicKeySignature))
    {
        Context->Log(TEXT("Could not decode NMT_EOS_RequestClientEphemeralKey on client."));
        return;
    }

    if (!Context->GetConfig()->GetEnableAutomaticEncryptionOnTrustedDedicatedServers())
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT(
                "Disconnecting from remote host because NMT_EOS_RequestClientEphemeralKey was received when it was not "
                "expected (automatic encryption not enabled)."));
        Context->Finish(EAuthPhaseFailureCode::Msg_RequestClientEphemeralKey_UnexpectedAutomaticEncryptionNotEnabled);
        return;
    }

    if (Context->GetRole() != EEOSNetDriverRole::ClientConnectedToDedicatedServer)
    {
        // Unexpected packet for anything other than a client connected to a dedicated server. Disconnect in case the
        // server is malicious.
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT(
                "Disconnecting from remote host because NMT_EOS_RequestClientEphemeralKey was received when it was not "
                "expected."));
        Context->Finish(EAuthPhaseFailureCode::Msg_RequestClientEphemeralKey_UnexpectedIncorrectRole);
        return;
    }

    // Load the public signing key from configuration.
    bool bLoadedKey = false;
    hydro_sign_keypair server_key_pair = {};
    const FEOSConfig *Config = Context->GetConfig();
    if (Config != nullptr)
    {
        FString PublicKey = Config->GetDedicatedServerPublicKey();
        if (!PublicKey.IsEmpty())
        {
            TArray<uint8> PublicKeyBytes;
            if (FBase64::Decode(PublicKey, PublicKeyBytes))
            {
                if (PublicKeyBytes.Num() == sizeof(server_key_pair.pk))
                {
                    FMemory::Memcpy(server_key_pair.pk, PublicKeyBytes.GetData(), sizeof(server_key_pair.pk));
                    bLoadedKey = true;
                }
            }
        }
    }
    if (!bLoadedKey)
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting from remote host because we could not load the dedicated server signing public key on "
                 "the client when handling NMT_EOS_RequestClientEphemeralKey."));
        Context->Finish(EAuthPhaseFailureCode::Msg_RequestClientEphemeralKey_KeyNotLoaded);
        return;
    }

    // Read the public key and signature.
    uint8_t temp_connection_unique_pk[hydro_kx_PUBLICKEYBYTES] = {};
    uint8_t signature[hydro_sign_BYTES] = {};
    TArray<uint8> ServerConnectionPublicKeyBytes;
    TArray<uint8> ServerConnectionPublicKeySignatureBytes;
    if (!FBase64::Decode(ServerConnectionPublicKey, ServerConnectionPublicKeyBytes) ||
        !FBase64::Decode(ServerConnectionPublicKeySignature, ServerConnectionPublicKeySignatureBytes) ||
        ServerConnectionPublicKeyBytes.Num() != sizeof(temp_connection_unique_pk) ||
        ServerConnectionPublicKeySignatureBytes.Num() != hydro_sign_BYTES)
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT(
                "Disconnecting from remote host because NMT_EOS_RequestClientEphemeralKey was received but had invalid "
                "data."));
        Context->Finish(EAuthPhaseFailureCode::Msg_RequestClientEphemeralKey_InvalidData);
        return;
    }
    FMemory::Memcpy(
        &temp_connection_unique_pk,
        ServerConnectionPublicKeyBytes.GetData(),
        sizeof(temp_connection_unique_pk));
    FMemory::Memcpy(&signature, ServerConnectionPublicKeySignatureBytes.GetData(), sizeof(signature));

    // Verify that the public key is signed correctly.
    if (hydro_sign_verify(
            signature,
            temp_connection_unique_pk,
            sizeof(temp_connection_unique_pk),
            "SIGNING0",
            server_key_pair.pk) != 0)
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting from remote host because signature verification of provided public key in "
                 "NMT_EOS_RequestClientEphemeralKey failed (untrusted server)."));
        Context->Finish(EAuthPhaseFailureCode::Msg_RequestClientEphemeralKey_UntrustedDedicatedServer);
        return;
    }

    // The connection public key is valid at this point *and* we now trust the server. Mark this connection as trusted.
    // NOTE: This doesn't mean that communications are implicitly secure yet - we are still a way off from negotiating
    // the symmetric AES key. But it does mean that when encrypted communications have been established, we trust
    // the server such that we will provide them our external platform credentials to verify our account.
    Context->MarkConnectionAsTrustedOnClient();

    // Generate the client session keys and the packet to send the ephemeral public key back to the server.
    FMemory::Memzero(&this->client_session_kp, sizeof(this->client_session_kp));
    uint8_t client_packet[hydro_kx_N_PACKET1BYTES];
    if (hydro_kx_n_1(&this->client_session_kp, client_packet, NULL, temp_connection_unique_pk) != 0)
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting from remote host because generation of response packet failed from libhydrogen."));
        Context->Finish(EAuthPhaseFailureCode::Msg_RequestClientEphemeralKey_ResponsePacketGenerationFailed);
        return;
    }

    // Send the ephemeral key back to the server.
    Context->Log(TEXT("Sending NMT_EOS_DeliverClientEphemeralKey to server."));
    FString EphemeralKey = FBase64::Encode(TArray<uint8>(client_packet, sizeof(client_packet)));
    FNetControlMessage<NMT_EOS_DeliverClientEphemeralKey>::Send(Context->GetConnection(), EphemeralKey);
    Context->GetConnection()->FlushNet(true);
}

void FAutomaticEncryptionPhase::On_NMT_EOS_DeliverClientEphemeralKey(
    const TSharedRef<FAuthConnectionPhaseContext> &Context,
    FInBunch &Bunch)
{
    FString ClientEphemeralPacket;
    if (!FNetControlMessage<NMT_EOS_DeliverClientEphemeralKey>::Receive(Bunch, ClientEphemeralPacket))
    {
        return;
    }

    if (!Context->GetConfig()->GetEnableAutomaticEncryptionOnTrustedDedicatedServers())
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT(
                "Disconnecting from remote host because NMT_EOS_DeliverClientEphemeralKey was received when it was not "
                "expected (automatic encryption not enabled)."));
        Context->Finish(EAuthPhaseFailureCode::Msg_DeliverClientEphemeralKey_UnexpectedAutomaticEncryptionNotEnabled);
        return;
    }

    if (Context->GetRole() != EEOSNetDriverRole::DedicatedServer)
    {
        // Unexpected packet for anything other than a dedicated server. Disconnect the remote host.
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting the remote host because NMT_EOS_DeliverClientEphemeralKey was not an expected "
                 "packet."));
        Context->Finish(EAuthPhaseFailureCode::Msg_DeliverClientEphemeralKey_UnexpectedIncorrectRole);
        return;
    }

    // Read ephemeral key packet.
    uint8_t client_ephemeral_packet[hydro_kx_N_PACKET1BYTES] = {};
    TArray<uint8> ClientEphemeralPacketBytes;
    if (!FBase64::Decode(ClientEphemeralPacket, ClientEphemeralPacketBytes) ||
        ClientEphemeralPacketBytes.Num() != sizeof(client_ephemeral_packet))
    {
        // Unexpected packet for anything other than a dedicated server. Disconnect the remote host.
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting the remote host because NMT_EOS_DeliverClientEphemeralKey was not valid."));
        Context->Finish(EAuthPhaseFailureCode::Msg_DeliverClientEphemeralKey_InvalidData);
        return;
    }
    FMemory::Memcpy(&client_ephemeral_packet, ClientEphemeralPacketBytes.GetData(), sizeof(client_ephemeral_packet));

    // Verify and process ephemeral key.
    FMemory::Memzero(&this->server_session_kp, sizeof(this->server_session_kp));
    if (hydro_kx_n_2(&this->server_session_kp, client_ephemeral_packet, NULL, &this->server_connection_unique_kp) != 0)
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting the remote host because NMT_EOS_DeliverClientEphemeralKey could not be verified."));
        Context->Finish(EAuthPhaseFailureCode::Msg_DeliverClientEphemeralKey_FailedToVerify);
        return;
    }

    // Generate an AES GCM key and store it.
    FMemory::Memzero(&this->AESGCMKey, sizeof(this->AESGCMKey));
    hydro_random_buf(&this->AESGCMKey, sizeof(this->AESGCMKey));

    // Send the AES GCM key encrypted with session key down to the client.
    TArray<uint8> EncryptedAESKey;
    EncryptedAESKey.SetNumZeroed(hydro_secretbox_HEADERBYTES + sizeof(this->AESGCMKey));
    if (hydro_secretbox_encrypt(
            EncryptedAESKey.GetData(),
            this->AESGCMKey,
            sizeof(this->AESGCMKey),
            0,
            "AESEXCHG",
            this->server_session_kp.tx) != 0)
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting the remote host because we could not encrypt the shared key."));
        Context->Finish(EAuthPhaseFailureCode::Msg_DeliverClientEphemeralKey_FailedToEncrypt);
        return;
    }

    // Send the encrypted AES key down to the client.
    FString EncodedEncryptedAESKey = FBase64::Encode(EncryptedAESKey);
    FNetControlMessage<NMT_EOS_SymmetricKeyExchange>::Send(Context->GetConnection(), EncodedEncryptedAESKey);
    Context->GetConnection()->FlushNet(true);

    // Mark the client as trusting the server. This is necessary since phases like IdTokenAuth need to distinguish
    // between "client trusts the server" and "encryption was manually set up but client does not trust server".
    Context->MarkConnectionAsTrustedOnClient();

    // Set the encryption data on the server, without enabling encryption for outbound packets. This allows the
    // server to decrypt incoming packets that were encrypted by the client, without causing
    // NMT_EOS_SymmetricKeyExchange resends to be encrypted if the reliable transmission initially fails. Later on, once
    // the server has started receiving encrypted packets from the client, we then enable outbound encryption on the
    // server.
    //
    // Once the client enables encryption, it then sends NMT_EOS_EncryptionEnabled which then causes the server to
    // enable encryption and continue the connection process. Since NMT_EOS_EncryptionEnabled will only be sent
    // once the client has the encryption key, the server can safely turn encryption on at that point.
    FEncryptionData EncryptionData;
    EncryptionData.Fingerprint = TArray<uint8>();
    EncryptionData.Identifier = TEXT("");
    EncryptionData.Key = TArray<uint8>(this->AESGCMKey, sizeof(this->AESGCMKey));
    Context->GetConnection()->SetEncryptionData(EncryptionData);
}

void FAutomaticEncryptionPhase::On_NMT_EOS_EnableEncryption(
    const TSharedRef<FAuthConnectionPhaseContext> &Context,
    FInBunch &Bunch)
{
    if (!FNetControlMessage<NMT_EOS_EnableEncryption>::Receive(Bunch))
    {
        return;
    }

    if (!Context->GetConfig()->GetEnableAutomaticEncryptionOnTrustedDedicatedServers())
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT(
                "Disconnecting from remote host because NMT_EOS_EnableEncryption was received when it was not "
                "expected (automatic encryption not enabled)."));
        Context->Finish(EAuthPhaseFailureCode::Msg_EnableEncryption_UnexpectedAutomaticEncryptionNotEnabled);
        return;
    }

    if (Context->GetRole() != EEOSNetDriverRole::DedicatedServer)
    {
        // Unexpected packet for anything other than a dedicated server. Disconnect the remote host.
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting the remote host because NMT_EOS_EnableEncryption was not an expected "
                 "packet."));
        Context->Finish(EAuthPhaseFailureCode::Msg_EnableEncryption_UnexpectedIncorrectRole);
        return;
    }

    // Enable encryption on the server. We've already set the encryption data, so we just need
    // to enable it now. We don't pass an encryption token into NMT_Hello or try to register
    // FNetDelegates::OnReceivedNetworkEncryptionToken because we don't need to.
    Context->GetConnection()->EnableEncryption();

    UE_LOG(LogEOSNetworkAuth, Verbose, TEXT("Encrypted connection on server established."));

    Context->Finish(EAuthPhaseFailureCode::Success);
}

void FAutomaticEncryptionPhase::On_NMT_EOS_SymmetricKeyExchange(
    const TSharedRef<FAuthConnectionPhaseContext> &Context,
    FInBunch &Bunch)
{
    FString EncryptedSymmetricKey;
    if (!FNetControlMessage<NMT_EOS_SymmetricKeyExchange>::Receive(Bunch, EncryptedSymmetricKey))
    {
        return;
    }

    if (!Context->GetConfig()->GetEnableAutomaticEncryptionOnTrustedDedicatedServers())
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting from remote host because NMT_EOS_SymmetricKeyExchange was received when it was not "
                 "expected (automatic encryption not enabled)."));
        Context->Finish(EAuthPhaseFailureCode::Msg_SymmetricKeyExchange_UnexpectedAutomaticEncryptionNotEnabled);
        return;
    }

    if (Context->GetRole() != EEOSNetDriverRole::ClientConnectedToDedicatedServer)
    {
        // Unexpected packet for anything other than a client connected to a dedicated server. Disconnect.
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting from remote host because NMT_EOS_SymmetricKeyExchange was received when it was not "
                 "expected."));
        Context->Finish(EAuthPhaseFailureCode::Msg_SymmetricKeyExchange_UnexpectedIncorrectRole);
        return;
    }

    // Decode symmetric key.
    TArray<uint8> EncryptedSymmetricKeyBytes;
    if (!FBase64::Decode(EncryptedSymmetricKey, EncryptedSymmetricKeyBytes) ||
        EncryptedSymmetricKeyBytes.Num() != hydro_secretbox_HEADERBYTES + sizeof(this->AESGCMKey))
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting from remote host because NMT_EOS_SymmetricKeyExchange was not valid."));
        Context->Finish(EAuthPhaseFailureCode::Msg_SymmetricKeyExchange_InvalidData);
        return;
    }

    // Decrypt symmetric key.
    if (hydro_secretbox_decrypt(
            this->AESGCMKey,
            EncryptedSymmetricKeyBytes.GetData(),
            EncryptedSymmetricKeyBytes.Num(),
            0,
            "AESEXCHG",
            this->client_session_kp.rx) != 0)
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Disconnecting from remote host because we could not decrypt the symmetric key in "
                 "NMT_EOS_SymmetricKeyExchange."));
        Context->Finish(EAuthPhaseFailureCode::Msg_SymmetricKeyExchange_FailedToDecrypt);
        return;
    }

    // Enable encryption on the client.
    FEncryptionData EncryptionData;
    EncryptionData.Fingerprint = TArray<uint8>();
    EncryptionData.Identifier = TEXT("");
    EncryptionData.Key = TArray<uint8>(this->AESGCMKey, sizeof(this->AESGCMKey));
    Context->GetConnection()->EnableEncryption(EncryptionData);

    // Send the enable encryption message over the encrypted connection back to the server.
    FNetControlMessage<NMT_EOS_EnableEncryption>::Send(Context->GetConnection());
    Context->GetConnection()->FlushNet(true);

    UE_LOG(LogEOSNetworkAuth, Verbose, TEXT("Encrypted connection on client established."));
}

#else

void FUnavailableAutomaticEncryptionPhase::Start(const TSharedRef<FAuthConnectionPhaseContext> &Context)
{
    Context->Finish(EAuthPhaseFailureCode::Success);
}

void FUnavailableAutomaticEncryptionPhase::On_NMT_EOS_RequestClientEphemeralKey(
    const TSharedRef<FAuthConnectionPhaseContext> &Context,
    FInBunch &Bunch)
{
    UE_LOG(
        LogEOSNetworkAuth,
        Error,
        TEXT("Dedicated server requested automatic encryption negotiation, but this platform is not compiled with "
             "automatic encryption support. Refer to the OnlineSubsystemRedpointEOS_<Platform>.Build.cs file for "
             "instructions on how enable automatic encryption."));
    Context->Finish(EAuthPhaseFailureCode::Phase_AutomaticEncryption_AutomaticEncryptionNotCompiled);
}

void FUnavailableAutomaticEncryptionPhase::On_NMT_EOS_DeliverClientEphemeralKey(
    const TSharedRef<FAuthConnectionPhaseContext> &Context,
    FInBunch &Bunch)
{
    UE_LOG(
        LogEOSNetworkAuth,
        Error,
        TEXT("Dedicated server requested automatic encryption negotiation, but this platform is not compiled with "
             "automatic encryption support. Refer to the OnlineSubsystemRedpointEOS_<Platform>.Build.cs file for "
             "instructions on how enable automatic encryption."));
    Context->Finish(EAuthPhaseFailureCode::Phase_AutomaticEncryption_AutomaticEncryptionNotCompiled);
}

void FUnavailableAutomaticEncryptionPhase::On_NMT_EOS_SymmetricKeyExchange(
    const TSharedRef<FAuthConnectionPhaseContext> &Context,
    FInBunch &Bunch)
{
    UE_LOG(
        LogEOSNetworkAuth,
        Error,
        TEXT("Dedicated server requested automatic encryption negotiation, but this platform is not compiled with "
             "automatic encryption support. Refer to the OnlineSubsystemRedpointEOS_<Platform>.Build.cs file for "
             "instructions on how enable automatic encryption."));
    Context->Finish(EAuthPhaseFailureCode::Phase_AutomaticEncryption_AutomaticEncryptionNotCompiled);
}

void FUnavailableAutomaticEncryptionPhase::On_NMT_EOS_EnableEncryption(
    const TSharedRef<FAuthConnectionPhaseContext> &Context,
    FInBunch &Bunch)
{
    UE_LOG(
        LogEOSNetworkAuth,
        Error,
        TEXT("Dedicated server requested automatic encryption negotiation, but this platform is not compiled with "
             "automatic encryption support. Refer to the OnlineSubsystemRedpointEOS_<Platform>.Build.cs file for "
             "instructions on how enable automatic encryption."));
    Context->Finish(EAuthPhaseFailureCode::Phase_AutomaticEncryption_AutomaticEncryptionNotCompiled);
}

#endif // #if !defined(EOS_AUTOMATIC_ENCRYPTION_UNAVAILABLE)