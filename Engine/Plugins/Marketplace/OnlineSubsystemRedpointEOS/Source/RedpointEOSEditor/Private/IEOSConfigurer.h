// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EOSConfigurationReader.h"
#include "EOSConfigurationWriter.h"
#include "OnlineSubsystemEOSEditorConfig.h"
#include "OnlineSubsystemEOSEditorConfigDetails.h"

class FEOSConfigurerContext
{
public:
    FEOSConfigurerContext();

    /**
     * Reflects the "automatically configure project for EOS" setting in the Editor Preferences.
     */
    bool bAutomaticallyConfigureEngineLevelSettings;
};

class IEOSConfigurer
{
public:
    /**
     * Initializes default settings for the instance.
     */
    virtual void InitDefaults(FEOSConfigurerContext &Context, UOnlineSubsystemEOSEditorConfig *Instance) = 0;

    /**
     * Loads the settings from the reader to the instance.
     */
    virtual void Load(
        FEOSConfigurerContext &Context,
        FEOSConfigurationReader &Reader,
        UOnlineSubsystemEOSEditorConfig *Instance) = 0;

    /**
     * Validates the instance's current settings, and updates
     * them inline if they're invalid.
     *
     * Return true if the instance needs to be saved (i.e. changes
     * were made that need to be persisted to disk).
     */
    virtual bool Validate(FEOSConfigurerContext &Context, UOnlineSubsystemEOSEditorConfig *Instance)
    {
        return false;
    }

    /**
     * Save the settings from the instance to the writer.
     */
    virtual void Save(
        FEOSConfigurerContext &Context,
        FEOSConfigurationWriter &Writer,
        UOnlineSubsystemEOSEditorConfig *Instance) = 0;

    /**
     * Customizes how configuration is rendered in the Project Settings page.
     */
    virtual void CustomizeProjectSettings(
        FEOSConfigurerContext &Context,
        FOnlineSubsystemEOSEditorConfigDetails &Details,
        IDetailLayoutBuilder &Builder){};
};