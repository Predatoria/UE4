// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION && EOS_APPLE_ENABLED

#if defined(__MAC_10_15) || (defined(__IPHONE_13_0) && __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_13_0) ||            \
    (defined(__TVOS_13_0) && __TV_OS_VERSION_MAX_ALLOWED >= __TVOS_13_0)
#define EOS_APPLE_HAS_RUNTIME_SUPPORT 1
#else
#define EOS_APPLE_HAS_RUNTIME_SUPPORT 0
#endif

#if EOS_APPLE_HAS_RUNTIME_SUPPORT

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

#include "Interfaces/OnlineIdentityInterface.h"

EOS_ENABLE_STRICT_WARNINGS

class FGetExternalCredentialsFromAppleNode : public FAuthenticationGraphNode
{
private:
    void OnIdTokenObtained(
        bool bWasSuccessful,
        FString IdToken,
        TSharedRef<FAuthenticationGraphState> InState,
        FAuthenticationGraphNodeOnDone InOnDone);

public:
    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FGetExternalCredentialsFromAppleNode");
    }
};

#endif // #if EOS_APPLE_HAS_RUNTIME_SUPPORT

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION && EOS_APPLE_ENABLED