// Copyright June Rhodes. All Rights Reserved.

/*
 * This initializes libhydrogen using OpenSSL's secure random number
 * generation, since Unreal Engine provides OpenSSL on all platforms
 * (including consoles).
 */

#pragma once

#include <openssl/rand.h>

static int hydro_random_init(void)
{
    bool bResult = !!RAND_bytes(hydro_random_context.state, sizeof(hydro_random_context.state));
    if (!bResult)
    {
        // Unable to seed PRNG.
        return -1;
    }

    hydro_random_context.counter = ~LOAD64_LE(hydro_random_context.state);

    return 0;
}