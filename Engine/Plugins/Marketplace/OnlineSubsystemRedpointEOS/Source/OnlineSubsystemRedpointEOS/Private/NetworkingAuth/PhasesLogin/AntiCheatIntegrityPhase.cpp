// Copyright June Rhodes. All Rights Reserved.

#include "./AntiCheatIntegrityPhase.h"

#if EOS_VERSION_AT_LEAST(1, 12, 0)

#include "OnlineSubsystemRedpointEOS/Private/EOSControlChannel.h"

void FAntiCheatIntegrityPhase::RegisterRoutes(UEOSControlChannel *ControlChannel)
{
}

void FAntiCheatIntegrityPhase::Start(const TSharedRef<FAuthLoginPhaseContext> &Context)
{
    checkf(
        IsValid(Context->GetConnection()) && Context->GetConnection()->PlayerId.IsValid(),
        TEXT("Connection player ID must have been set by verification phase before Anti-Cheat phases can begin."));

    // NOTE: If this is a reconnection by the same player, where we skipped the registration
    // in AntiCheatProofPhase because of their existing registration, then we won't receive
    // the event and we need to proceed immediately.

    if (bIsVerified)
    {
        Context->Finish(EAuthPhaseFailureCode::Success);
    }
    else
    {
        bIsStarted = true;
    }
}

void FAntiCheatIntegrityPhase::OnAntiCheatPlayerAuthStatusChanged(
    const TSharedRef<FAuthLoginPhaseContext> &Context,
    EOS_EAntiCheatCommonClientAuthStatus NewAuthStatus)
{
    FString StatusStr = TEXT("Unknown");
    if (NewAuthStatus == EOS_EAntiCheatCommonClientAuthStatus::EOS_ACCCAS_Invalid)
    {
        StatusStr = TEXT("Invalid");
    }
    else if (NewAuthStatus == EOS_EAntiCheatCommonClientAuthStatus::EOS_ACCCAS_LocalAuthComplete)
    {
        StatusStr = TEXT("LocalAuthComplete");
    }
    else if (NewAuthStatus == EOS_EAntiCheatCommonClientAuthStatus::EOS_ACCCAS_RemoteAuthComplete)
    {
        StatusStr = TEXT("RemoteAuthComplete");
    }

    UE_LOG(
        LogEOSNetworkAuth,
        Verbose,
        TEXT("Server authentication: %s: Anti-Cheat verification status is now '%s'."),
        *Context->GetUserId()->ToString(),
        *StatusStr);

    if (NewAuthStatus == EOS_EAntiCheatCommonClientAuthStatus::EOS_ACCCAS_RemoteAuthComplete)
    {
        if (bIsStarted)
        {
            Context->Finish(EAuthPhaseFailureCode::Success);
        }
        else
        {
            bIsVerified = true;
        }
    }
}

void FAntiCheatIntegrityPhase::OnAntiCheatPlayerActionRequired(
    const TSharedRef<FAuthLoginPhaseContext> &Context,
    EOS_EAntiCheatCommonClientAction ClientAction,
    EOS_EAntiCheatCommonClientActionReason ActionReasonCode,
    const FString &ActionReasonDetailsString)
{
    FString DetailsString = FString::Printf(
        TEXT("Disconnected from server due to Anti-Cheat error. Reason: '%s'. Details: '%s'."),
        *EOS_EAntiCheatCommonClientActionReason_ToString(ActionReasonCode),
        *ActionReasonDetailsString);
    Context->Finish(EAuthPhaseFailureCode::Phase_AntiCheatIntegrity_KickedDueToEACFailure);
}

#endif // #if EOS_VERSION_AT_LEAST(1, 12, 0)