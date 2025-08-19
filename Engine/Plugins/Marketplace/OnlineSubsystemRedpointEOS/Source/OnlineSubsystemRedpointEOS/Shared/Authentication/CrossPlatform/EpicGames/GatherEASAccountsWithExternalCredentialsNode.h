// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FGatherEASAccountsWithExternalCredentialsNode : public FAuthenticationGraphNode
{
    typedef TTuple<EOS_EpicAccountId, EOS_ContinuanceToken> FGatherEASAccountResponse;

private:
    void OnResultCallback(
        const EOS_Auth_LoginCallbackInfo *Data,
        TSharedRef<FAuthenticationGraphState> State,
        TSharedRef<IOnlineExternalCredentials> ExternalCredentials,
        std::function<void(FGatherEASAccountResponse)> OnPlatformDone);

public:
    UE_NONCOPYABLE(FGatherEASAccountsWithExternalCredentialsNode);
    FGatherEASAccountsWithExternalCredentialsNode() = default;
    virtual ~FGatherEASAccountsWithExternalCredentialsNode() = default;

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FGatherEASAccountsWithExternalCredentialsNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
