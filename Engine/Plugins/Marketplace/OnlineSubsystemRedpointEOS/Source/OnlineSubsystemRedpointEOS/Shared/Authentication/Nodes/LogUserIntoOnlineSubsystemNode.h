// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

#include "Interfaces/OnlineIdentityInterface.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FLogUserIntoOnlineSubsystemNode : public FAuthenticationGraphNode
{
private:
    FName SubsystemName;

    void OnOSSLoginUIClosed(
        TSharedPtr<const FUniqueNetId> UniqueId,
        const int ControllerIndex,
        const FOnlineError &Error,
        TSharedRef<FAuthenticationGraphState> InState,
        FAuthenticationGraphNodeOnDone InOnDone,
        TSharedPtr<IOnlineIdentity, ESPMode::ThreadSafe> OSSIdentity);

    void DoLogin(
        const TSharedRef<FAuthenticationGraphState> &InState,
        const FAuthenticationGraphNodeOnDone &InOnDone,
        const TSharedPtr<IOnlineIdentity, ESPMode::ThreadSafe> &OSSIdentity);
    FDelegateHandle OnOSSLoginDelegate;
    void OnOSSLoginComplete(
        int32 LocalUserNum,
        bool bWasSuccessful,
        const FUniqueNetId &UserId,
        const FString &Error,
        TSharedRef<FAuthenticationGraphState> InState,
        FAuthenticationGraphNodeOnDone InOnDone,
        TSharedPtr<IOnlineIdentity, ESPMode::ThreadSafe> OSSIdentity);

public:
    UE_NONCOPYABLE(FLogUserIntoOnlineSubsystemNode);
    virtual ~FLogUserIntoOnlineSubsystemNode() = default;

    FLogUserIntoOnlineSubsystemNode(FName InSubsystemName)
    {
        this->SubsystemName = InSubsystemName;
    }

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FLogUserIntoOnlineSubsystemNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
