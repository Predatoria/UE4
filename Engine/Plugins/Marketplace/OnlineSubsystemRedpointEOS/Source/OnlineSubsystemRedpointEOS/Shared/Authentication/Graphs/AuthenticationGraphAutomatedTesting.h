// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#if !defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

EOS_ENABLE_STRICT_WARNINGS

#define EOS_AUTH_GRAPH_AUTOMATED_TESTING FName(TEXT("AutomatedTesting"))
#define EOS_AUTH_GRAPH_AUTOMATED_TESTING_OSS FName(TEXT("AutomatedTestingOSS"))

/**
 * This authentication graph is used internally when the unit tests are running.
 */
class ONLINESUBSYSTEMREDPOINTEOS_API FAuthenticationGraphAutomatedTesting : public FAuthenticationGraph
{
protected:
    virtual TSharedRef<FAuthenticationGraphNode> CreateGraph(
        const TSharedRef<FAuthenticationGraphState> &InitialState) override;

public:
    UE_NONCOPYABLE(FAuthenticationGraphAutomatedTesting);
    FAuthenticationGraphAutomatedTesting() = default;
    virtual ~FAuthenticationGraphAutomatedTesting() = default;

    static void Register();
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if !UE_BUILD_SHIPPING

#endif // #if EOS_HAS_AUTHENTICATION
