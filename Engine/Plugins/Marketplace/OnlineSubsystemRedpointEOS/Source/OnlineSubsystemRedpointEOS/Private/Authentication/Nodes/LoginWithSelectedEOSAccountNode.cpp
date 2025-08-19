// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/LoginWithSelectedEOSAccountNode.h"

#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"

EOS_ENABLE_STRICT_WARNINGS

void FLoginWithSelectedEOSAccountNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
    check(State->HasSelectedEOSCandidate());

    FAuthenticationGraphEOSCandidate Candidate = State->GetSelectedEOSCandidate();
    if (EOSString_ProductUserId::IsValid(Candidate.ProductUserId))
    {
        // This account is already authenticated, we don't need to call EOS_CreateUser, so
        // just set the result user ID in the state and call it done.
        State->ResultUserId = MakeShared<FUniqueNetIdEOS>(Candidate.ProductUserId);
        State->ResultUserAuthAttributes = Candidate.UserAuthAttributes;
        State->ResultRefreshCallback = Candidate.RefreshCallback;
        State->ResultExternalCredentials = Candidate.ExternalCredentials;
        State->ResultCrossPlatformAccountId = Candidate.AssociatedCrossPlatformAccountId;
        State->ResultNativeSubsystemName = Candidate.NativeSubsystemName;
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        return;
    }

    if (EOSString_ContinuanceToken::IsNone(Candidate.ContinuanceToken))
    {
        // If we don't have a continuance token, then this isn't a valid candidate and
        // should not have been selected.
        State->ErrorMessages.Add(TEXT("LoginWithSelectedEOSAccountNode: Reached with unusable candidiate selected."));
        OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    // Otherwise, call EOS_CreateUser with the continuance token.
    EOS_Connect_CreateUserOptions CreateOpts = {};
    CreateOpts.ApiVersion = EOS_CONNECT_CREATEUSER_API_LATEST;
    CreateOpts.ContinuanceToken = Candidate.ContinuanceToken;
    EOSRunOperation<EOS_HConnect, EOS_Connect_CreateUserOptions, EOS_Connect_CreateUserCallbackInfo>(
        State->EOSConnect,
        &CreateOpts,
        EOS_Connect_CreateUser,
        [WeakThis = GetWeakThis(this), State, OnDone, Candidate](const EOS_Connect_CreateUserCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (Data->ResultCode != EOS_EResult::EOS_Success)
                {
                    State->ErrorMessages.Add(FString::Printf(
                        TEXT("LoginWithSelectedEOSAccountNode: Unable to create EOS user: %s"),
                        ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode))));
                    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
                    return;
                }

                // Success, we have created the EOS account.
                State->ResultUserId = MakeShared<FUniqueNetIdEOS>(Data->LocalUserId);
                for (const auto &KV : Candidate.UserAuthAttributes)
                {
                    State->ResultUserAuthAttributes.Add(KV.Key, KV.Value);
                }
                State->ResultRefreshCallback = Candidate.RefreshCallback;
                State->ResultCrossPlatformAccountId = Candidate.AssociatedCrossPlatformAccountId;
                State->ResultExternalCredentials = Candidate.ExternalCredentials;
                State->ResultNativeSubsystemName = Candidate.NativeSubsystemName;
                OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
                return;
            }
        });
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION