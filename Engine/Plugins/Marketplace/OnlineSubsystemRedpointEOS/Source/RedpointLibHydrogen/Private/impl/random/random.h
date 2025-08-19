// Copyright June Rhodes. All Rights Reserved.

#pragma once

#define PREPROCESSOR_TO_STRING(x) PREPROCESSOR_TO_STRING_INNER(x)
#define PREPROCESSOR_TO_STRING_INNER(x) #x

// clang-format off
#include PREPROCESSOR_TO_STRING(impl/random/RANDOM_HEADER.h)
// clang-format on