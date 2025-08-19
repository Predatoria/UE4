// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#if !defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING

#include "CoreMinimal.h"

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"
#include "PlatformHttp.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FIssueJwtForAutomatedTestingNode : public FAuthenticationGraphNode
{
private:
    void OnHttpResponse(
        FHttpRequestPtr Request,
        FHttpResponsePtr Response,
        bool bConnectedSuccessfully,
        TSharedRef<FAuthenticationGraphState> State,
        FAuthenticationGraphNodeOnDone OnDone);

public:
    UE_NONCOPYABLE(FIssueJwtForAutomatedTestingNode);
    FIssueJwtForAutomatedTestingNode() = default;
    virtual ~FIssueJwtForAutomatedTestingNode() = default;

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FIssueJwtForAutomatedTestingNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if !UE_BUILD_SHIPPING

#endif // #if EOS_HAS_AUTHENTICATION
