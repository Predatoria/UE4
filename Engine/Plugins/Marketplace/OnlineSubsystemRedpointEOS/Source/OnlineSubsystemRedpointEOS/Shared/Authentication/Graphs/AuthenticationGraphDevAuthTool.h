// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

EOS_ENABLE_STRICT_WARNINGS

#define EOS_AUTH_GRAPH_DEV_AUTH_TOOL FName(TEXT("DevAuthTool"))

/**
 * This authentication graph is used internally when "Login before play-in-editor" is enabled.
 */
class ONLINESUBSYSTEMREDPOINTEOS_API FAuthenticationGraphDevAuthTool : public FAuthenticationGraph
{
protected:
    virtual TSharedRef<FAuthenticationGraphNode> CreateGraph(
        const TSharedRef<FAuthenticationGraphState> &InitialState) override;

public:
    UE_NONCOPYABLE(FAuthenticationGraphDevAuthTool);
    FAuthenticationGraphDevAuthTool() = default;
    virtual ~FAuthenticationGraphDevAuthTool() = default;

    static void Register();
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
