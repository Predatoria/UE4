// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/GatherEOSAccountsWithExternalCredentialsNode.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/MultiOperation.h"

EOS_ENABLE_STRICT_WARNINGS

void FGatherEOSAccountsWithExternalCredentialsNode::OnResultCallback(
    const EOS_Connect_LoginCallbackInfo *Data,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<FAuthenticationGraphState> State,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<IOnlineExternalCredentials> ExternalCredentials,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    std::function<void(bool)> OnPlatformDone)
{
    if (Data->ResultCode == EOS_EResult::EOS_Success || Data->ResultCode == EOS_EResult::EOS_InvalidUser)
    {
        State->AddEOSConnectCandidateFromExternalCredentials(Data, ExternalCredentials);
    }
    else
    {
        State->ErrorMessages.Add(FString::Printf(
            TEXT("GatherEOSAccountsWithNativePlatformCredentialsNode: External credential "
                 "'%s' failed to authenticate with EOS Connect: %s"),
            *ExternalCredentials->GetType(),
            ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode))));
    }

    OnPlatformDone(true);
}

void FGatherEOSAccountsWithExternalCredentialsNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnStepDone)
{
    FMultiOperation<TSharedRef<IOnlineExternalCredentials>, bool>::Run(
        State->AvailableExternalCredentials,
        [WeakThis = GetWeakThis(this), State](
            const TSharedRef<IOnlineExternalCredentials> &ExternalCredentials,
            std::function<void(bool)> OnPlatformDone) -> bool {
            if (auto This = StaticCastSharedPtr<FGatherEOSAccountsWithExternalCredentialsNode>(PinWeakThis(WeakThis)))
            {
                FEOSAuthentication::DoRequest(
                    State->EOSConnect,
                    ExternalCredentials->GetId(),
                    ExternalCredentials->GetToken(),
                    StrToExternalCredentialType(ExternalCredentials->GetType()),
                    FEOSAuth_DoRequestComplete::CreateSP(
                        This.ToSharedRef(),
                        &FGatherEOSAccountsWithExternalCredentialsNode::OnResultCallback,
                        State,
                        ExternalCredentials,
                        MoveTemp(OnPlatformDone)));
                return true;
            }
            return false;
        },
        [OnStepDone](const TArray<bool> &Results) {
            OnStepDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        });
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION