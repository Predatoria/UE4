// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

EOS_ENABLE_STRICT_WARNINGS

#define EOS_METADATA_NO_EMPTY_EAS_REFRESH_TOKEN_ATTEMPT TEXT("NoEmptyEASRefreshTokenAttempt")
#define EOS_METADATA_EAS_REFRESH_TOKEN TEXT("EASRefreshToken")
#define EOS_METADATA_EAS_NATIVE_SUBSYSTEM TEXT("EASNativeSubsystem")

class ONLINESUBSYSTEMREDPOINTEOS_API FTryPersistentEASCredentialsNode : public FAuthenticationGraphNode
{
public:
    UE_NONCOPYABLE(FTryPersistentEASCredentialsNode);
    FTryPersistentEASCredentialsNode() = default;
    virtual ~FTryPersistentEASCredentialsNode() = default;

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FTryPersistentEASCredentialsNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
