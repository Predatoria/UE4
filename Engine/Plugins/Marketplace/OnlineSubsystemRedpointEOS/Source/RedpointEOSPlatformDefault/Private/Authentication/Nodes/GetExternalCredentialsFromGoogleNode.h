// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION && EOS_GOOGLE_ENABLED

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystemGoogle.h"

EOS_ENABLE_STRICT_WARNINGS

struct FNameAndToken
{
    FNameAndToken(){};
    FNameAndToken(const FString &InGivenName, const FString &InIdToken)
        : GivenName(InGivenName)
        , IdToken(InIdToken){};

    FString GivenName;
    FString IdToken;
};

class FGetExternalCredentialsFromGoogleNode : public FAuthenticationGraphNode
{
private:
    void OnIdTokenObtained(
        bool bWasSuccessful,
        FNameAndToken GivenNameAndIdToken,
        TSharedRef<FAuthenticationGraphState> InState,
        FAuthenticationGraphNodeOnDone InOnDone);

public:
    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FGetExternalCredentialsFromGoogleNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION && EOS_GOOGLE_ENABLED