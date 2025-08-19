// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/PerformInteractiveEASLoginNode.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FPerformInteractiveLinkExternalCredentialsToEASAccountNode
    : public FAuthenticationGraphNode
{
private:
    TSharedPtr<TUserInterfaceRef<
        IEOSUserInterface_EnterDevicePinCode,
        UEOSUserInterface_EnterDevicePinCode,
        UEOSUserInterface_EnterDevicePinCode_Context>>
        Widget;

    void OnLinkCancel(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone);

    void HandleEASAuthenticationCallback(
        const FEOSAuthCallbackInfo &Data,
        const TSharedRef<FAuthenticationGraphState> &State,
        const FAuthenticationGraphNodeOnDone &OnDone);

public:
    UE_NONCOPYABLE(FPerformInteractiveLinkExternalCredentialsToEASAccountNode);
    FPerformInteractiveLinkExternalCredentialsToEASAccountNode() = default;
    virtual ~FPerformInteractiveLinkExternalCredentialsToEASAccountNode() = default;

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("PerformInteractiveLinkExternalCredentialsToEASAccountNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
