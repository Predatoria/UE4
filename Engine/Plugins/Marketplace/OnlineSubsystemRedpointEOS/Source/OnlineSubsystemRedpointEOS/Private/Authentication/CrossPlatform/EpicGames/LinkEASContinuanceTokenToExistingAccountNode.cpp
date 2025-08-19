// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/LinkEASContinuanceTokenToExistingAccountNode.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"

EOS_ENABLE_STRICT_WARNINGS

void FLinkEASContinuanceTokenToExistingAccountNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
    check(State->ExistingUserId.IsValid());
    check(State->ExistingUserId->HasValidProductUserId());
    check(EOSString_EpicAccountId::IsValid(State->GetAuthenticatedEpicAccountId()));

    EOS_ProductUserId ExistingProductUserId = State->ExistingUserId->GetProductUserId();
    EOS_ContinuanceToken EASContinuanceToken = EOSString_ContinuanceToken::None;
    for (const auto &Candidate : State->EOSCandidates)
    {
        if (Candidate.Type == EAuthenticationGraphEOSCandidateType::CrossPlatform)
        {
            if (!EOSString_ProductUserId::IsValid(Candidate.ProductUserId) &&
                !EOSString_ContinuanceToken::IsNone(Candidate.ContinuanceToken))
            {
                EASContinuanceToken = Candidate.ContinuanceToken;
                break;
            }
        }
    }
    check(!EOSString_ContinuanceToken::IsNone(EASContinuanceToken));

    EOS_Connect_LinkAccountOptions Opts = {};
    Opts.ApiVersion = EOS_CONNECT_LINKACCOUNT_API_LATEST;
    Opts.ContinuanceToken = EASContinuanceToken;
    Opts.LocalUserId = ExistingProductUserId;

    EOSRunOperation<EOS_HConnect, EOS_Connect_LinkAccountOptions, EOS_Connect_LinkAccountCallbackInfo>(
        State->EOSConnect,
        &Opts,
        EOS_Connect_LinkAccount,
        [WeakThis = GetWeakThis(this), State, OnDone](const EOS_Connect_LinkAccountCallbackInfo *Info) {
            if (Info->ResultCode != EOS_EResult::EOS_Success)
            {
                State->ErrorMessages.Add(FString::Printf(
                    TEXT("Link account operation failed with result code %s"),
                    ANSI_TO_TCHAR(EOS_EResult_ToString(Info->ResultCode))));
                OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
                return;
            }

            OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        });
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION