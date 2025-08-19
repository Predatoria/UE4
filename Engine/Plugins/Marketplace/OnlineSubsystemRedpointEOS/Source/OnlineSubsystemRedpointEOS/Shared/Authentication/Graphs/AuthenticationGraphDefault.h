// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

EOS_ENABLE_STRICT_WARNINGS

#define EOS_AUTH_GRAPH_DEFAULT NAME_Default
#define EOS_AUTH_GRAPH_DEFAULT_WITH_CROSS_PLATFORM_FALLBACK FName(TEXT("DefaultWithCrossPlatformFallback"))

/**
 * This class provides the default resolver for authentication graphs. If you don't explicitly set an authentication
 * graph for your project, this resolver will choose for you.
 */
class ONLINESUBSYSTEMREDPOINTEOS_API FAuthenticationGraphDefault
{
private:
    static FName Resolve(
        const TSharedRef<class FAuthenticationGraphRegistry> &InRegistry,
        const TSharedRef<class FEOSConfig> &InConfig,
        const FOnlineAccountCredentials &InProvidedCredentials,
        const TSoftObjectPtr<class UWorld> &InWorld);
    static FName ResolveWithCrossPlatform(
        const TSharedRef<class FAuthenticationGraphRegistry> &InRegistry,
        const TSharedRef<class FEOSConfig> &InConfig,
        const FOnlineAccountCredentials &InProvidedCredentials,
        const TSoftObjectPtr<class UWorld> &InWorld);

public:
    static void Register();
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION