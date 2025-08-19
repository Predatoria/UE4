// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/SignOutEASAccountNode.h"

#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraphState.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/CrossPlatformAccountId.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGamesCrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSErrorConv.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

void FSignOutEASAccountNode::Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone)
{
    // Remove any candidates related to this account.
    for (int i = State->EOSCandidates.Num() - 1; i >= 0; i--)
    {
        auto Candidate = State->EOSCandidates[i];
        if (Candidate.Type == EAuthenticationGraphEOSCandidateType::CrossPlatform &&
            Candidate.AssociatedCrossPlatformAccountId.IsValid() &&
            Candidate.AssociatedCrossPlatformAccountId->GetType().IsEqual(EPIC_GAMES_ACCOUNT_ID) &&
            this->AccountId->Compare(*Candidate.AssociatedCrossPlatformAccountId))
        {
            State->EOSCandidates.RemoveAt(i);
        }
    }

    // Clear the authenticated cross-platform account ID, if it's set to this account.
    if (State->AuthenticatedCrossPlatformAccountId.IsValid() &&
        this->AccountId->Compare(*State->AuthenticatedCrossPlatformAccountId))
    {
        State->AuthenticatedCrossPlatformAccountId.Reset();
    }

    // NOTE: EOS SDK has a bug since at least 1.14.2, where you can't call EOS_Auth_Logout if you haven't
    // successfully authenticated with EOS in at least some manner. If you haven't authenticated with EOS,
    // then EOS_Auth_Logout stalls forever and never completes.
    //
    // Make sure we're authenticated with at least one EOS account before proceeding.
    if (EOS_Connect_GetLoggedInUsersCount(State->EOSConnect) == 0)
    {
        FOnlineError Err = OnlineRedpointEOS::Errors::NotImplemented(
            TEXT("FSignOutEASAccountNode::Execute"),
            TEXT("Unable to sign out of the Epic Games account, due to a bug in the EOS SDK."));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    // Sign out of the EAS account.
    EOS_Auth_LogoutOptions Opts = {};
    Opts.ApiVersion = EOS_AUTH_LOGOUT_API_LATEST;
    Opts.LocalUserId = this->AccountId->GetEpicAccountId();
    EOSRunOperation<EOS_HAuth, EOS_Auth_LogoutOptions, EOS_Auth_LogoutCallbackInfo>(
        State->EOSAuth,
        &Opts,
        EOS_Auth_Logout,
        [WeakThis = GetWeakThis(this), State, OnDone](const EOS_Auth_LogoutCallbackInfo *Info) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Info->ResultCode != EOS_EResult::EOS_Success)
                {
                    FOnlineError Err = ConvertError(
                        TEXT("FSignOutEASAccountNode::Execute"),
                        TEXT("Unable to sign out Epic Games account when required."),
                        Info->ResultCode);
                    UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
                    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
                    return;
                }

                OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
            }
        });
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION