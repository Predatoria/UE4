// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

class FEOSRegulatedTicker : public TSharedFromThis<FEOSRegulatedTicker>
{
private:
    TMap<FDelegateHandle, FTickerDelegate> RegisteredTickers;
    FUTickerDelegateHandle GlobalHandle;
    int32 TicksPerSecond;
    float AccumulatedDeltaSeconds;
    float CounterAccumulatedSeconds;
    int32 InvocationCount;
    int32 InvocationCountThisSecond;

    bool Tick(float DeltaSeconds);

public:
    FEOSRegulatedTicker(int32 InTicksPerSecond = 60);
    UE_NONCOPYABLE(FEOSRegulatedTicker);
    ~FEOSRegulatedTicker();

    FDelegateHandle AddTicker(const FTickerDelegate &InDelegate);
    void RemoveTicker(FDelegateHandle Handle);
};

EOS_DISABLE_STRICT_WARNINGS