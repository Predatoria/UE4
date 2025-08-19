// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/FailAuthenticationDueToConflictingAccountsNode.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraphState.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/CrossPlatformAccountId.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"

EOS_ENABLE_STRICT_WARNINGS

void FFailAuthenticationDueToConflictingAccountsNode::Execute(
    TSharedRef<FAuthenticationGraphState> State,
    FAuthenticationGraphNodeOnDone OnDone)
{
    FString ExistingLocalEOSAccountId = State->ExistingUserId->ToString();
    FString CrossPlatformAccountId = State->AuthenticatedCrossPlatformAccountId->ToString();
    FString CrossPlatformEOSAccountId = TEXT("Unavailable");
    for (const auto &Candidate : State->EOSCandidates)
    {
        if (Candidate.Type == EAuthenticationGraphEOSCandidateType::CrossPlatform)
        {
            if (EOSString_ProductUserId::IsValid(Candidate.ProductUserId))
            {
                verifyf(
                    EOSString_ProductUserId::ToString(Candidate.ProductUserId, CrossPlatformEOSAccountId) ==
                        EOS_EResult::EOS_Success,
                    TEXT("Unable to convert PUID for error message"));
                break;
            }
        }
    }

    State->ErrorMessages.Add(FString::Printf(
        TEXT("The specified cross-platform account has already played this title, so you can not "
             "link it against the game data of this local account. Please contact game support with the following "
             "information: \n"
             " - Local EOS account PUID: %s\n"
             " - Cross-platform account ID: %s\n"
             " - Cross-platform EOS account PUID: %s"),
        *ExistingLocalEOSAccountId,
        *CrossPlatformAccountId,
        *CrossPlatformEOSAccountId));
    OnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION