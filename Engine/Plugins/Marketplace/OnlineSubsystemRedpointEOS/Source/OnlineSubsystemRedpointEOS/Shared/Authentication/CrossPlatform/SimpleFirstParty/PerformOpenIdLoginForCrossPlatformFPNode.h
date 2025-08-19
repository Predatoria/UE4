// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FPerformOpenIdLoginForCrossPlatformFPNode : public FAuthenticationGraphNode
{
private:
    void HandleEOSAuthenticationCallback(
        const EOS_Connect_LoginCallbackInfo *Data,
        TSharedRef<FAuthenticationGraphState> State,
        FAuthenticationGraphNodeOnDone OnDone);

public:
    UE_NONCOPYABLE(FPerformOpenIdLoginForCrossPlatformFPNode);
    FPerformOpenIdLoginForCrossPlatformFPNode() = default;
    virtual ~FPerformOpenIdLoginForCrossPlatformFPNode() = default;

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FPerformOpenIdLoginForCrossPlatformFPNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
