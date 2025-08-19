// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Public/OnlineSubsystemRedpointEOSModule.h"

EOS_ENABLE_STRICT_WARNINGS

/**
 * This class does not actually create the FOnlineSubsystemRedpointEAS instances (that is done as part of
 * FOnlineSubsystemEOS construction). Instead this just makes those subsystems accessible via IOnlineFactory.
 */
class FOnlineSubsystemRedpointEASFactory : public IOnlineFactory,
                                           public TSharedFromThis<FOnlineSubsystemRedpointEASFactory>
{
private:
    FOnlineSubsystemRedpointEOSModule *Module;

public:
    FOnlineSubsystemRedpointEASFactory(FOnlineSubsystemRedpointEOSModule *InModule);
    UE_NONCOPYABLE(FOnlineSubsystemRedpointEASFactory);

    virtual IOnlineSubsystemPtr CreateSubsystem(FName InstanceName) override;
};

EOS_DISABLE_STRICT_WARNINGS