// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

EOS_ENABLE_STRICT_WARNINGS

#define EOS_AUTH_GRAPH_CROSS_PLATFORM_ONLY FName(TEXT("CrossPlatformOnly"))

/**
 * This authentication graph only uses the cross-platform account provider to authenticate a user, and never obtains
 * external credentials.
 */
class ONLINESUBSYSTEMREDPOINTEOS_API FAuthenticationGraphCrossPlatformOnly : public FAuthenticationGraph
{
protected:
    virtual TSharedRef<FAuthenticationGraphNode> CreateGraph(
        const TSharedRef<FAuthenticationGraphState> &InitialState) override;

public:
    UE_NONCOPYABLE(FAuthenticationGraphCrossPlatformOnly);
    FAuthenticationGraphCrossPlatformOnly() = default;
    virtual ~FAuthenticationGraphCrossPlatformOnly() = default;

    static void Register();
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
