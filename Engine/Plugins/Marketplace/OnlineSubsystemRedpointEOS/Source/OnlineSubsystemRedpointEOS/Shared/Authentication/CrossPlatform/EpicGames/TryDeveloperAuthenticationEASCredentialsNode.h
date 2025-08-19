// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FTryDeveloperAuthenticationEASCredentialsNode : public FAuthenticationGraphNode
{
protected:
    virtual FString GetCredentialName(TSharedRef<FAuthenticationGraphState> State) = 0;

public:
    UE_NONCOPYABLE(FTryDeveloperAuthenticationEASCredentialsNode);
    FTryDeveloperAuthenticationEASCredentialsNode() = default;
    virtual ~FTryDeveloperAuthenticationEASCredentialsNode() = default;

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;
};

class ONLINESUBSYSTEMREDPOINTEOS_API FTryPIEDeveloperAuthenticationEASCredentialsNode
    : public FTryDeveloperAuthenticationEASCredentialsNode
{
public:
    UE_NONCOPYABLE(FTryPIEDeveloperAuthenticationEASCredentialsNode);
    FTryPIEDeveloperAuthenticationEASCredentialsNode() = default;
    virtual ~FTryPIEDeveloperAuthenticationEASCredentialsNode() = default;

protected:
    virtual FString GetCredentialName(TSharedRef<FAuthenticationGraphState> State) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FTryPIEDeveloperAuthenticationEASCredentialsNode");
    }
};

class ONLINESUBSYSTEMREDPOINTEOS_API FTryDefaultDeveloperAuthenticationEASCredentialsNode
    : public FTryDeveloperAuthenticationEASCredentialsNode
{
public:
    UE_NONCOPYABLE(FTryDefaultDeveloperAuthenticationEASCredentialsNode);
    FTryDefaultDeveloperAuthenticationEASCredentialsNode() = default;
    virtual ~FTryDefaultDeveloperAuthenticationEASCredentialsNode() = default;

protected:
    virtual FString GetCredentialName(TSharedRef<FAuthenticationGraphState> State) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FTryDefaultDeveloperAuthenticationEASCredentialsNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
