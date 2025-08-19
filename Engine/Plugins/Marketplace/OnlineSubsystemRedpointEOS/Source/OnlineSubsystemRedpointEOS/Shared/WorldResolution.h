// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FWorldResolution
{
public:
    /**
     * Attempts to resolve a pointer to the current world based on an online subsystem instance name. These differ
     * slightly from world context names.
     */
    static TSoftObjectPtr<UWorld> GetWorld(const FName &InInstanceName, bool bAllowFailure = false);
};

EOS_DISABLE_STRICT_WARNINGS