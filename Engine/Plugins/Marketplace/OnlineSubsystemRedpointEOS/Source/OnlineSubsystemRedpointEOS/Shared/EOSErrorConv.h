// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "OnlineError.h"
#include "EOSCommon.h"
#include "Interfaces/OnlinePartyInterface.h"
#include "Interfaces/OnlineSessionInterface.h"

EOS_ENABLE_STRICT_WARNINGS

const FOnlineError &ConvertError(EOS_EResult Result);
const FOnlineError ConvertError(const TCHAR *InCall, const FString &InContextMsg, EOS_EResult Result);
const FOnlineError ConvertError(const TCHAR *InCall, const TCHAR* InContextMsg, EOS_EResult Result);
const FOnlineError ConvertError(const FUniqueNetId& InActorId, const TCHAR *InCall, const FString &InContextMsg, EOS_EResult Result);
const FOnlineError ConvertError(const FUniqueNetId& InActorId, EOS_EResult Result);
const FOnlineError ConvertError(const FUniqueNetId& InActorId, const FString& InObjectId, const TCHAR *InCall, const FString &InContextMsg, EOS_EResult Result);
ECreatePartyCompletionResult ConvertErrorTo_ECreatePartyCompletionResult(const FOnlineError& InError);
EJoinPartyCompletionResult ConvertErrorTo_EJoinPartyCompletionResult(const FOnlineError& InError);
ELeavePartyCompletionResult ConvertErrorTo_ELeavePartyCompletionResult(const FOnlineError& InError);
EOnJoinSessionCompleteResult::Type ConvertErrorTo_EOnJoinSessionCompleteResult(const FOnlineError& InError);
ESendPartyInvitationCompletionResult ConvertErrorTo_ESendPartyInvitationCompletionResult(const FOnlineError& InError);
EUpdateConfigCompletionResult ConvertErrorTo_EUpdateConfigCompletionResult(const FOnlineError& InError);

EOS_DISABLE_STRICT_WARNINGS
