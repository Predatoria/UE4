// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

EOS_ENABLE_STRICT_WARNINGS

#define EOS_AUTH_GRAPH_ALWAYS_FAIL FName(TEXT("AlwaysFail"))

/**
 * This authentication graph always fails, and is used when there is no viable authentication graph to use.
 */
class ONLINESUBSYSTEMREDPOINTEOS_API FAuthenticationGraphAlwaysFail : public FAuthenticationGraph
{
protected:
    virtual TSharedRef<FAuthenticationGraphNode> CreateGraph(
        const TSharedRef<FAuthenticationGraphState> &InitialState) override;

public:
    UE_NONCOPYABLE(FAuthenticationGraphAlwaysFail);
    FAuthenticationGraphAlwaysFail() = default;
    virtual ~FAuthenticationGraphAlwaysFail() = default;

    static void Register();
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
