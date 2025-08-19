// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "Containers/UnrealString.h"
#include "CoreMinimal.h"

enum EAuthPhaseFailureCode
{
    Success = 0x0000,

    All_CanNotAccessConfig = 0x1000,

    All_CanNotAccessAntiCheat = 0x1001,

    All_CanNotAccessOSS = 0x1002,

    All_CanNotAccessSocketSubsystem = 0x1003,

    All_InvalidMessageType = 0x1004,

    Phase_AutomaticEncryption_AutomaticEncryptionNotCompiled = 0x2000,

    Phase_AutomaticEncryption_DedicatedServerMisconfigured = 0x2001,

    Phase_AutomaticEncryption_FailedToSignConnectionKeyPair = 0x2002,

    Phase_AntiCheatProof_InvalidSignatureForUnprotectedClient = 0x2100,

    Phase_AntiCheatProof_AntiCheatRegistrationFailed = 0x2101,

    Phase_AntiCheatIntegrity_KickedDueToEACFailure = 0x2200,

    Phase_LegacyCredentialAuth_ConnectionNotEncrypted = 0x2300,

    Phase_LegacyIdentityCheck_CanNotCallUserInfo = 0x2400,

    Phase_LegacyIdentityCheck_UserAccountNotFound = 0x2401,

    Phase_LegacyIdentityCheck_UserAccountNotFoundAfterLoad = 0x2402,

    Phase_SanctionCheck_AccountBanned = 0x2500,

    Phase_SanctionCheck_FailedToCopySanctionResult = 0x2501,

    Phase_SanctionCheck_FailedToRetrieveSanctions = 0x2502,

    Phase_P2PAddressCheck_InvalidSourceAddress = 0x2600,

    Phase_P2PAddressCheck_UserIdDoesNotMatchSource = 0x2601,

    Msg_RequestClientEphemeralKey_UnexpectedAutomaticEncryptionNotEnabled = 0x3000,

    Msg_RequestClientEphemeralKey_UnexpectedIncorrectRole = 0x3001,

    Msg_RequestClientEphemeralKey_KeyNotLoaded = 0x3002,

    Msg_RequestClientEphemeralKey_InvalidData = 0x3003,

    Msg_RequestClientEphemeralKey_UntrustedDedicatedServer = 0x3004,

    Msg_RequestClientEphemeralKey_ResponsePacketGenerationFailed = 0x3005,

    Msg_DeliverClientEphemeralKey_UnexpectedAutomaticEncryptionNotEnabled = 0x3100,

    Msg_DeliverClientEphemeralKey_UnexpectedIncorrectRole = 0x3101,

    Msg_DeliverClientEphemeralKey_InvalidData = 0x3102,

    Msg_DeliverClientEphemeralKey_FailedToVerify = 0x3103,

    Msg_DeliverClientEphemeralKey_FailedToEncrypt = 0x3104,

    Msg_SymmetricKeyExchange_UnexpectedAutomaticEncryptionNotEnabled = 0x3200,

    Msg_SymmetricKeyExchange_UnexpectedIncorrectRole = 0x3201,

    Msg_SymmetricKeyExchange_InvalidData = 0x3202,

    Msg_SymmetricKeyExchange_FailedToDecrypt = 0x3203,

    Msg_RequestClientToken_ConnectionNotEncrypted = 0x3300,

    Msg_RequestClientToken_UnexpectedIncorrectRole = 0x3301,

    Msg_RequestClientToken_UnexpectedTrustedServersNotEnabled = 0x3302,

    Msg_RequestClientToken_ConnectionNotTrusted = 0x3303,

    Msg_RequestClientToken_InvalidUserId = 0x3304,

    Msg_RequestClientToken_MissingTransferrableUserCredentials = 0x3305,

    Msg_DeliverClientToken_ConnectionNotEncrypted = 0x3400,

    Msg_DeliverClientToken_UnexpectedIncorrectRole = 0x3401,

    Msg_DeliverClientToken_UnexpectedTrustedServersNotEnabled = 0x3402,

    Msg_DeliverClientToken_ConnectionNotTrusted = 0x3403,

    Msg_DeliverClientToken_InvalidUserId = 0x3404,

    Msg_DeliverClientToken_TokenIsForADifferentAccount = 0x3405,

    Msg_DeliverClientToken_AuthenticationFailed = 0x3406,

    Msg_RequestIdToken_ConnectionNotEncrypted = 0x3500,

    Msg_RequestIdToken_UnexpectedIncorrectRole = 0x3501,

    Msg_RequestIdToken_ConnectionNotTrusted = 0x3502,

    Msg_RequestIdToken_InvalidUserId = 0x3503,

    Msg_RequestIdToken_CanNotRetrieveIdToken = 0x3504,

    Msg_DeliverIdToken_ConnectionNotEncrypted = 0x3600,

    Msg_DeliverIdToken_UnexpectedIncorrectRole = 0x3601,

    Msg_DeliverIdToken_ConnectionNotTrusted = 0x3602,

    Msg_DeliverIdToken_TokenIsForADifferentAccount = 0x3603,

    Msg_DeliverIdToken_AuthenticationFailed = 0x3604,

    Msg_EnableEncryption_UnexpectedAutomaticEncryptionNotEnabled = 0x3700,

    Msg_EnableEncryption_UnexpectedIncorrectRole = 0x3701,
};

FString GetAuthPhaseFailureCodeString(EAuthPhaseFailureCode Code);