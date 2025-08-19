// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "OnlineSubsystemEOSEditorPrefs.generated.h"

/**
 * Editor preferences for Epic Online Services.
 */
UCLASS(Config = EditorPerProjectUserSettings, meta = (DisplayName = "Epic Online Services"))
class UOnlineSubsystemEOSEditorPrefs : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UOnlineSubsystemEOSEditorPrefs(const FObjectInitializer &ObjectInitializer);

    /**
     * If turned on, the EOS dropdown will not be visible in the editor toolbar.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Toolbar")
    bool bHideDropdownInEditorToolbar;

    /**
     * If turned on, the EOS plugin will automatically configure the project to use EOS. This changes the default
     * subsystem for the project, it's networking drivers, and various other engine configuration settings that must be
     * set in order for EOS to work.
     *
     * If you'd rather set these settings manually, you can turn this option off.
     */
    UPROPERTY(EditAnywhere, Config, Category = "Automation")
    bool bAutomaticallyConfigureProjectForEOS;
};