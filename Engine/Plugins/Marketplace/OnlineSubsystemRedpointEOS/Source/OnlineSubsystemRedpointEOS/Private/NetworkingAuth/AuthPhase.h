// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Net/DataBunch.h"

DECLARE_DELEGATE_RetVal_TwoParams(bool, FAuthPhaseRoute, class UEOSControlChannel *ControlChannel, FInBunch &);

class IAuthPhase
{
public:
    /**
     * The type of this beacon phase. Used by decoders to route calls.
     */
    virtual FName GetName() const = 0;
};