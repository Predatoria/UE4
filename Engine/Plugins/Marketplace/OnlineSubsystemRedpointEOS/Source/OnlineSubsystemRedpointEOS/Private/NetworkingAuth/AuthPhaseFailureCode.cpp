// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthPhaseFailureCode.h"

FString GetAuthPhaseFailureCodeString(EAuthPhaseFailureCode Code)
{
    FString BaseMessage;
    switch (Code)
    {
        // Success
    case EAuthPhaseFailureCode::Success:
        return TEXT("");

        // Generic errors
    case EAuthPhaseFailureCode::All_CanNotAccessConfig:
    case EAuthPhaseFailureCode::All_CanNotAccessAntiCheat:
    case EAuthPhaseFailureCode::All_CanNotAccessOSS:
    case EAuthPhaseFailureCode::All_CanNotAccessSocketSubsystem:
        BaseMessage = TEXT("Invalid server state");
        break;
    case EAuthPhaseFailureCode::All_InvalidMessageType:
        BaseMessage = TEXT("Invalid message type");
        break;

        // Phase errors
    case EAuthPhaseFailureCode::Phase_AutomaticEncryption_AutomaticEncryptionNotCompiled:
        BaseMessage = TEXT("Automatic encryption support not compiled into this platform");
        break;
    case EAuthPhaseFailureCode::Phase_AutomaticEncryption_DedicatedServerMisconfigured:
        BaseMessage = TEXT("This dedicated server is misconfigured");
        break;
    case EAuthPhaseFailureCode::Phase_AutomaticEncryption_FailedToSignConnectionKeyPair:
        BaseMessage = TEXT("This dedicated server could not establish a secure connection");
        break;
    case EAuthPhaseFailureCode::Phase_AntiCheatProof_InvalidSignatureForUnprotectedClient:
        BaseMessage = TEXT("Invalid unprotected client proof");
        break;
    case EAuthPhaseFailureCode::Phase_AntiCheatProof_AntiCheatRegistrationFailed:
        BaseMessage = TEXT("Anti-Cheat registration failed");
        break;
    case EAuthPhaseFailureCode::Phase_AntiCheatIntegrity_KickedDueToEACFailure:
        BaseMessage = TEXT("Anti-Cheat integrity check failed");
        break;
    case EAuthPhaseFailureCode::Phase_LegacyCredentialAuth_ConnectionNotEncrypted:
        BaseMessage = TEXT("The connection is not encrypted");
        break;
    case EAuthPhaseFailureCode::Phase_LegacyIdentityCheck_CanNotCallUserInfo:
        BaseMessage = TEXT("Unable to look up connecting user information");
        break;
    case EAuthPhaseFailureCode::Phase_LegacyIdentityCheck_UserAccountNotFound:
    case EAuthPhaseFailureCode::Phase_LegacyIdentityCheck_UserAccountNotFoundAfterLoad:
        BaseMessage = TEXT("Unable to find connecting user account");
        break;
    case EAuthPhaseFailureCode::Phase_SanctionCheck_AccountBanned:
        BaseMessage = TEXT("Your account is banned");
        break;
    case EAuthPhaseFailureCode::Phase_SanctionCheck_FailedToCopySanctionResult:
    case EAuthPhaseFailureCode::Phase_SanctionCheck_FailedToRetrieveSanctions:
        BaseMessage = TEXT("Failed to retrieve sanction information");
        break;
    case EAuthPhaseFailureCode::Phase_P2PAddressCheck_InvalidSourceAddress:
    case EAuthPhaseFailureCode::Phase_P2PAddressCheck_UserIdDoesNotMatchSource:
        BaseMessage = TEXT("You must authenticate as the user ID you are connecting from");
        break;

        // Message errors
    case EAuthPhaseFailureCode::Msg_RequestClientEphemeralKey_UnexpectedAutomaticEncryptionNotEnabled:
    case EAuthPhaseFailureCode::Msg_RequestClientEphemeralKey_UnexpectedIncorrectRole:
    case EAuthPhaseFailureCode::Msg_RequestClientEphemeralKey_KeyNotLoaded:
    case EAuthPhaseFailureCode::Msg_RequestClientEphemeralKey_InvalidData:
        BaseMessage = TEXT("Invalid protocol message");
        break;
    case EAuthPhaseFailureCode::Msg_RequestClientEphemeralKey_UntrustedDedicatedServer:
        BaseMessage = TEXT("The client does not trust you as a dedicated server");
        break;
    case EAuthPhaseFailureCode::Msg_RequestClientEphemeralKey_ResponsePacketGenerationFailed:

    case EAuthPhaseFailureCode::Msg_DeliverClientEphemeralKey_UnexpectedAutomaticEncryptionNotEnabled:
    case EAuthPhaseFailureCode::Msg_DeliverClientEphemeralKey_UnexpectedIncorrectRole:
    case EAuthPhaseFailureCode::Msg_DeliverClientEphemeralKey_InvalidData:
        BaseMessage = TEXT("Invalid protocol message");
        break;
    case EAuthPhaseFailureCode::Msg_DeliverClientEphemeralKey_FailedToVerify:
    case EAuthPhaseFailureCode::Msg_DeliverClientEphemeralKey_FailedToEncrypt:
        BaseMessage = TEXT("An encrypted connection could not be established");
        break;

    case EAuthPhaseFailureCode::Msg_SymmetricKeyExchange_UnexpectedAutomaticEncryptionNotEnabled:
    case EAuthPhaseFailureCode::Msg_SymmetricKeyExchange_UnexpectedIncorrectRole:
    case EAuthPhaseFailureCode::Msg_SymmetricKeyExchange_InvalidData:
    case EAuthPhaseFailureCode::Msg_SymmetricKeyExchange_FailedToDecrypt:
        BaseMessage = TEXT("Invalid protocol message");
        break;

    case EAuthPhaseFailureCode::Msg_RequestClientToken_ConnectionNotEncrypted:
        BaseMessage = TEXT("The connection is not encrypted");
        break;
    case EAuthPhaseFailureCode::Msg_RequestClientToken_UnexpectedIncorrectRole:
    case EAuthPhaseFailureCode::Msg_RequestClientToken_UnexpectedTrustedServersNotEnabled:
        BaseMessage = TEXT("Invalid protocol message");
        break;
    case EAuthPhaseFailureCode::Msg_RequestClientToken_ConnectionNotTrusted:
        BaseMessage = TEXT("The connection is not trusted");
        break;
    case EAuthPhaseFailureCode::Msg_RequestClientToken_InvalidUserId:
    case EAuthPhaseFailureCode::Msg_RequestClientToken_MissingTransferrableUserCredentials:
        BaseMessage = TEXT("Unable to find local credentials to send to trusted server");
        break;

    case EAuthPhaseFailureCode::Msg_DeliverClientToken_ConnectionNotEncrypted:
        BaseMessage = TEXT("The connection is not encrypted");
        break;
    case EAuthPhaseFailureCode::Msg_DeliverClientToken_UnexpectedIncorrectRole:
    case EAuthPhaseFailureCode::Msg_DeliverClientToken_UnexpectedTrustedServersNotEnabled:
        BaseMessage = TEXT("Invalid protocol message");
        break;
    case EAuthPhaseFailureCode::Msg_DeliverClientToken_ConnectionNotTrusted:
        BaseMessage = TEXT("The connection is not trusted");
        break;
    case EAuthPhaseFailureCode::Msg_DeliverClientToken_InvalidUserId:
    case EAuthPhaseFailureCode::Msg_DeliverClientToken_TokenIsForADifferentAccount:
    case EAuthPhaseFailureCode::Msg_DeliverClientToken_AuthenticationFailed:
        BaseMessage = TEXT("Authentication failed");
        break;

    case EAuthPhaseFailureCode::Msg_RequestIdToken_ConnectionNotEncrypted:
        BaseMessage = TEXT("The connection is not encrypted");
        break;
    case EAuthPhaseFailureCode::Msg_RequestIdToken_UnexpectedIncorrectRole:
        BaseMessage = TEXT("Invalid protocol message");
        break;
    case EAuthPhaseFailureCode::Msg_RequestIdToken_ConnectionNotTrusted:
        BaseMessage = TEXT("The connection is not trusted");
        break;
    case EAuthPhaseFailureCode::Msg_RequestIdToken_InvalidUserId:
        BaseMessage = TEXT("The local user ID is invalid");
        break;
    case EAuthPhaseFailureCode::Msg_RequestIdToken_CanNotRetrieveIdToken:
        BaseMessage = TEXT("Could not retrieve ID token for authentication");
        break;

    case EAuthPhaseFailureCode::Msg_DeliverIdToken_ConnectionNotEncrypted:
        BaseMessage = TEXT("The connection is not encrypted");
        break;
    case EAuthPhaseFailureCode::Msg_DeliverIdToken_UnexpectedIncorrectRole:
        BaseMessage = TEXT("Invalid protocol message");
        break;
    case EAuthPhaseFailureCode::Msg_DeliverIdToken_ConnectionNotTrusted:
        BaseMessage = TEXT("The connection is not trusted");
        break;
    case EAuthPhaseFailureCode::Msg_DeliverIdToken_TokenIsForADifferentAccount:
        BaseMessage = TEXT("Provided ID token does not match claimed user ID");
        break;
    case EAuthPhaseFailureCode::Msg_DeliverIdToken_AuthenticationFailed:
        BaseMessage = TEXT("Authentication failed");
        break;

    case EAuthPhaseFailureCode::Msg_EnableEncryption_UnexpectedAutomaticEncryptionNotEnabled:
    case EAuthPhaseFailureCode::Msg_EnableEncryption_UnexpectedIncorrectRole:
        BaseMessage = TEXT("Invalid protocol message");
        break;

        // Fallback error
    default:
        BaseMessage = TEXT("Unknown error");
    }
    return FString::Printf(TEXT("%s (0x%04x)."), *BaseMessage, Code);
}