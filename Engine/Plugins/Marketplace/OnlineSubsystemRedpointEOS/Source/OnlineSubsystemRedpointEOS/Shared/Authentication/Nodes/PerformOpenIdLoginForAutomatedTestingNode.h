// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#if !defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FPerformOpenIdLoginForAutomatedTestingNode : public FAuthenticationGraphNode
{
private:
    void HandleEOSAuthenticationCallback(
        const EOS_Connect_LoginCallbackInfo *Data,
        TSharedRef<FAuthenticationGraphState> State,
        FAuthenticationGraphNodeOnDone OnDone);

public:
    UE_NONCOPYABLE(FPerformOpenIdLoginForAutomatedTestingNode);
    FPerformOpenIdLoginForAutomatedTestingNode() = default;
    virtual ~FPerformOpenIdLoginForAutomatedTestingNode() = default;

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FPerformOpenIdLoginForAutomatedTestingNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if !UE_BUILD_SHIPPING

#endif // #if EOS_HAS_AUTHENTICATION
