// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

EOS_ENABLE_STRICT_WARNINGS

UENUM(BlueprintType)
enum class EEOSUserInterface_SwitchToCrossPlatformAccount_Choice : uint8
{
    /** Switch to the account the user logged in with. */
    SwitchToThisAccount,

    /** Link a different Epic Games account (show the login prompt again). */
    LinkADifferentAccount,

    /** Cancel the linking operation entirely. */
    CancelLinking
};

EOS_DISABLE_STRICT_WARNINGS