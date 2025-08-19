// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION && EOS_STEAM_ENABLED

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

#include "Interfaces/OnlineIdentityInterface.h"

EOS_ENABLE_STRICT_WARNINGS

struct FSteamCredentialInfo
{
public:
    FSteamCredentialInfo()
        : UserId(TEXT(""))
        , EncryptedAppTicket(TEXT(""))
        , SessionTicket(TEXT("")){};

    FString UserId;
    FString EncryptedAppTicket;
    FString SessionTicket;
    TMap<FString, FString> AuthAttributes;
};

class FGetExternalCredentialsFromSteamNode : public FAuthenticationGraphNode
{
private:
    void OnCredentialsObtained(
        bool bWasSuccessful,
        struct FSteamCredentialInfo ObtainedCredentials,
        TSharedRef<FAuthenticationGraphState> InState,
        FAuthenticationGraphNodeOnDone InOnDone);

public:
    FGetExternalCredentialsFromSteamNode() = default;
    UE_NONCOPYABLE(FGetExternalCredentialsFromSteamNode);

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FGetExternalCredentialsFromSteamNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION && EOS_STEAM_ENABLED