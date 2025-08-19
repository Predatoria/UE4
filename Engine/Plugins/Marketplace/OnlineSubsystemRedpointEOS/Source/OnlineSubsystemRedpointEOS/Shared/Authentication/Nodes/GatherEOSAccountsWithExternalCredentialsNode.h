// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FGatherEOSAccountsWithExternalCredentialsNode : public FAuthenticationGraphNode
{
private:
    void OnResultCallback(
        const EOS_Connect_LoginCallbackInfo *Data,
        TSharedRef<FAuthenticationGraphState> State,
        TSharedRef<IOnlineExternalCredentials> ExternalCredentials,
        std::function<void(bool)> OnPlatformDone);

public:
    UE_NONCOPYABLE(FGatherEOSAccountsWithExternalCredentialsNode);
    FGatherEOSAccountsWithExternalCredentialsNode() = default;
    virtual ~FGatherEOSAccountsWithExternalCredentialsNode() = default;

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FGatherEOSAccountsWithExternalCredentialsNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
