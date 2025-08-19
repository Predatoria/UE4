// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/UserInterface/EOSUserInterface_SignInOrCreateAccount.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/UserInterface/UserInterfaceRef.h"

EOS_ENABLE_STRICT_WARNINGS

#if !defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING
class ONLINESUBSYSTEMREDPOINTEOS_API FPromptToSignInOrCreateAccountNode_AutomationSetting
{
public:
    static EEOSUserInterface_SignInOrCreateAccount_Choice UserChoice;
};
#endif

class ONLINESUBSYSTEMREDPOINTEOS_API FPromptToSignInOrCreateAccountNode : public FAuthenticationGraphNode
{
private:
    TSharedPtr<TUserInterfaceRef<
        IEOSUserInterface_SignInOrCreateAccount,
        UEOSUserInterface_SignInOrCreateAccount,
        UEOSUserInterface_SignInOrCreateAccount_Context>>
        Widget;

    virtual void SelectChoice(
        EEOSUserInterface_SignInOrCreateAccount_Choice SelectedChoice,
        TSharedRef<FAuthenticationGraphState> State,
        FAuthenticationGraphNodeOnDone OnDone);

public:
    UE_NONCOPYABLE(FPromptToSignInOrCreateAccountNode);
    FPromptToSignInOrCreateAccountNode() = default;
    virtual ~FPromptToSignInOrCreateAccountNode() = default;

    virtual void Execute(TSharedRef<FAuthenticationGraphState> State, FAuthenticationGraphNodeOnDone OnDone) override;

    virtual FString GetDebugName() const override
    {
        return TEXT("FPromptToSignInOrCreateAccountNode");
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION
