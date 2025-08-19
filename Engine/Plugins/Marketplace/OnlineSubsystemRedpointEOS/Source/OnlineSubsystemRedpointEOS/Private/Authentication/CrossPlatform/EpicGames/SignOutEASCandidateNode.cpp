// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/SignOutEASCandidateNode.h"

#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraphState.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/CrossPlatformAccountId.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGamesCrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSErrorConv.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

void FSignOutEASCandidateNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
    TSharedPtr<const FEpicGamesCrossPlatformAccountId> AccountToSignOut = nullptr;
    for (int i = State->EOSCandidates.Num() - 1; i >= 0; i--)
    {
        auto Candidate = State->EOSCandidates[i];
        if (Candidate.Type == EAuthenticationGraphEOSCandidateType::CrossPlatform)
        {
            checkf(!AccountToSignOut.IsValid(), TEXT("Multiple EAS candidates in authentication state!"));
            AccountToSignOut =
                StaticCastSharedPtr<const FEpicGamesCrossPlatformAccountId>(Candidate.AssociatedCrossPlatformAccountId);
            State->EOSCandidates.RemoveAt(i);
        }
    }

    if (AccountToSignOut == nullptr)
    {
        checkf(
            !State->AuthenticatedCrossPlatformAccountId.IsValid(),
            TEXT("Cross-platform account ID is set, but no EAS candidate was found."));
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        return;
    }

    checkf(
        AccountToSignOut->Compare(*State->AuthenticatedCrossPlatformAccountId),
        TEXT("Authenticated cross-platform account ID must match the candidate's ID"));

    EOS_Auth_LogoutOptions Opts = {};
    Opts.ApiVersion = EOS_AUTH_LOGOUT_API_LATEST;
    Opts.LocalUserId = AccountToSignOut->GetEpicAccountId();

    EOSRunOperation<EOS_HAuth, EOS_Auth_LogoutOptions, EOS_Auth_LogoutCallbackInfo>(
        State->EOSAuth,
        &Opts,
        EOS_Auth_Logout,
        [WeakThis = GetWeakThis(this), State, OnDone](const EOS_Auth_LogoutCallbackInfo *Info) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Info->ResultCode != EOS_EResult::EOS_Success)
                {
                    State->ErrorMessages.Add(
                        FString::Printf(TEXT("Unable to sign out Epic Games account when required")));
                    FOnlineError Err = ConvertError(
                        TEXT("FSignOutEASCandidateNode::Execute"),
                        TEXT("Unable to sign out Epic Games account when required."),
                        Info->ResultCode);
                    UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
                    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
                    return;
                }

                State->AuthenticatedCrossPlatformAccountId.Reset();
                OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
            }
        });
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION