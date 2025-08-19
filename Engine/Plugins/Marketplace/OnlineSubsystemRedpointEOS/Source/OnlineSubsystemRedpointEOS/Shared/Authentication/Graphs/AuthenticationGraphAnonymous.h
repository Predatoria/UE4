// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

EOS_ENABLE_STRICT_WARNINGS

#define EOS_AUTH_GRAPH_ANONYMOUS FName(TEXT("Anonymous"))

/**
 * This authentication graph performs anonymous authentication and nothing else. Suitable on mobile devices.
 */
class ONLINESUBSYSTEMREDPOINTEOS_API FAuthenticationGraphAnonymous : public FAuthenticationGraph
{
protected:
    virtual TSharedRef<FAuthenticationGraphNode> CreateGraph(
        const TSharedRef<FAuthenticationGraphState> &InitialState) override;

public:
    UE_NONCOPYABLE(FAuthenticationGraphAnonymous);
    FAuthenticationGraphAnonymous() = default;
    virtual ~FAuthenticationGraphAnonymous() = default;

    static void Register();
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
