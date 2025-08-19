// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "Containers/Ticker.h"
#include "CoreMinimal.h"
#include "EOSDefines.h"

EOS_ENABLE_STRICT_WARNINGS

#if defined(UE_5_0_OR_LATER)
typedef FTSTicker FUTicker;
typedef FTSTicker::FDelegateHandle FUTickerDelegateHandle;
#else
typedef FTicker FUTicker;
typedef FDelegateHandle FUTickerDelegateHandle;
#endif // #if defined(UE_5_0_OR_LATER)

EOS_DISABLE_STRICT_WARNINGS