// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

EOS_ENABLE_STRICT_WARNINGS

UENUM(BlueprintType)
enum class EEOSUserInterface_SignInOrCreateAccount_Choice : uint8
{
    /** The user wants to sign in with an existing account. */
    SignIn,

    /** The user wants to create an account. */
    CreateAccount
};

EOS_DISABLE_STRICT_WARNINGS