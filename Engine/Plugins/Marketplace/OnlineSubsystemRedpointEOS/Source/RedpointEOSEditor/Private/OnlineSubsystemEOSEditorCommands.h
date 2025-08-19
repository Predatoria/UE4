// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "OnlineSubsystemEOSEditorStyle.h"

class FOnlineSubsystemEOSEditorCommands : public TCommands<FOnlineSubsystemEOSEditorCommands>
{
public:
    FOnlineSubsystemEOSEditorCommands()
        : TCommands<FOnlineSubsystemEOSEditorCommands>(
              TEXT("OnlineSubsystemEOSEditor"),
              NSLOCTEXT("Contexts", "OnlineSubsystemEOSEditor", "Online Subsystem EOS"),
              NAME_None,
              FOnlineSubsystemEOSEditorStyle::GetStyleSetName())
    {
    }
    UE_NONCOPYABLE(FOnlineSubsystemEOSEditorCommands);

    // TCommands<> interface
    virtual void RegisterCommands() override;

public:
    TSharedPtr<FUICommandInfo> ViewDocumentation;
#if defined(EOS_IS_FREE_EDITION)
    TSharedPtr<FUICommandInfo> ViewLicenseAgreementInBrowser;
#else
    TSharedPtr<FUICommandInfo> AccessSupport;
#endif
    TSharedPtr<FUICommandInfo> ViewWebsite;
};