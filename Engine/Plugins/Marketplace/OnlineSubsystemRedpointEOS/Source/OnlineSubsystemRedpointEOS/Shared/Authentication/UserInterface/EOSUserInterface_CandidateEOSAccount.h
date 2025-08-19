// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#if EOS_HAS_AUTHENTICATION
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"
#endif // #if EOS_HAS_AUTHENTICATION

#include "EOSUserInterface_CandidateEOSAccount.generated.h"

EOS_ENABLE_STRICT_WARNINGS

USTRUCT(BlueprintType)
struct ONLINESUBSYSTEMREDPOINTEOS_API FEOSUserInterface_CandidateEOSAccount
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EOS|User Interface")
    FText DisplayName;

#if EOS_HAS_AUTHENTICATION
    FAuthenticationGraphEOSCandidate Candidate;
#endif // #if EOS_HAS_AUTHENTICATION
};

EOS_DISABLE_STRICT_WARNINGS