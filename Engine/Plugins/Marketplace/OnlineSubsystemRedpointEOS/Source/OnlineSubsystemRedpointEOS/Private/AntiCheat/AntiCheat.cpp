// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/AntiCheat/AntiCheat.h"

FString EOS_EAntiCheatCommonClientActionReason_ToString(const EOS_EAntiCheatCommonClientActionReason &ActionReasonCode)
{
    FString ReasonCode = TEXT("Unknown");
    if (ActionReasonCode == EOS_EAntiCheatCommonClientActionReason::EOS_ACCCAR_Invalid)
    {
        ReasonCode = TEXT("Invalid");
    }
    else if (ActionReasonCode == EOS_EAntiCheatCommonClientActionReason::EOS_ACCCAR_InternalError)
    {
        ReasonCode = TEXT("InternalError");
    }
    else if (ActionReasonCode == EOS_EAntiCheatCommonClientActionReason::EOS_ACCCAR_InvalidMessage)
    {
        ReasonCode = TEXT("InvalidMessage");
    }
    else if (ActionReasonCode == EOS_EAntiCheatCommonClientActionReason::EOS_ACCCAR_AuthenticationFailed)
    {
        ReasonCode = TEXT("AuthenticationFailed");
    }
    else if (ActionReasonCode == EOS_EAntiCheatCommonClientActionReason::EOS_ACCCAR_NullClient)
    {
        ReasonCode = TEXT("NullClient");
    }
    else if (ActionReasonCode == EOS_EAntiCheatCommonClientActionReason::EOS_ACCCAR_HeartbeatTimeout)
    {
        ReasonCode = TEXT("HeartbeatTimeout");
    }
    else if (ActionReasonCode == EOS_EAntiCheatCommonClientActionReason::EOS_ACCCAR_ClientViolation)
    {
        ReasonCode = TEXT("ClientViolation");
    }
    else if (ActionReasonCode == EOS_EAntiCheatCommonClientActionReason::EOS_ACCCAR_BackendViolation)
    {
        ReasonCode = TEXT("BackendViolation");
    }
    else if (ActionReasonCode == EOS_EAntiCheatCommonClientActionReason::EOS_ACCCAR_TemporaryCooldown)
    {
        ReasonCode = TEXT("TemporaryCooldown");
    }
    else if (ActionReasonCode == EOS_EAntiCheatCommonClientActionReason::EOS_ACCCAR_TemporaryBanned)
    {
        ReasonCode = TEXT("TemporaryBanned");
    }
    else if (ActionReasonCode == EOS_EAntiCheatCommonClientActionReason::EOS_ACCCAR_PermanentBanned)
    {
        ReasonCode = TEXT("PermanentBanned");
    }
    return ReasonCode;
}