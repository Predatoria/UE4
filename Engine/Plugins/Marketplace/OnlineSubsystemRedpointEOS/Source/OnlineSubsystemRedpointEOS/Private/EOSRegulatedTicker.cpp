// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/EOSRegulatedTicker.h"
#include "HAL/IConsoleManager.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

EOS_ENABLE_STRICT_WARNINGS

#if !(defined(UE_BUILD_SHIPPING) && UE_BUILD_SHIPPING)
static TAutoConsoleVariable<bool> CVarEOSTickRegulation(
    TEXT("t.EOSTickRegulation"),
    true,
    TEXT("When on (default), EOS only ticks a maximum number of times a second, regardless of FPS. When off, EOS ticks "
         "as fast as the game renders."));
#endif

bool FEOSRegulatedTicker::Tick(float DeltaSeconds)
{
    this->CounterAccumulatedSeconds += DeltaSeconds;
    if (this->CounterAccumulatedSeconds > 1.0f)
    {
        this->InvocationCount = this->InvocationCountThisSecond;
        this->InvocationCountThisSecond = 0;
        this->CounterAccumulatedSeconds = 0.0f;
    }

    EOS_TRACE_COUNTER_SET(CTR_EOSRegulatedTicksInvocationsLastSecond, InvocationCount);

#if !(defined(UE_BUILD_SHIPPING) && UE_BUILD_SHIPPING)
    if (CVarEOSTickRegulation.GetValueOnGameThread())
    {
#endif
        this->AccumulatedDeltaSeconds += DeltaSeconds;
        if (this->AccumulatedDeltaSeconds > 1.0f / this->TicksPerSecond)
        {
            // Enough time has elapsed to fire our delegates.
            float DeltaSecondsSinceLastRegulatedTick = this->AccumulatedDeltaSeconds;
            this->AccumulatedDeltaSeconds -= DeltaSeconds;
            if (this->AccumulatedDeltaSeconds > DeltaSeconds)
            {
                // Game stalled and enough time elapsed to fire multiple times.
                // However, we don't want to run delegates multiple times because
                // that might exacerbate lag issues, and we don't need to ensure
                // a minimum tick rate for EOS operations, only a maximum.
                this->AccumulatedDeltaSeconds = 0.0f;
            }
            TArray<FDelegateHandle> TickerHandles;
            this->RegisteredTickers.GetKeys(TickerHandles);
            for (const auto &Key : TickerHandles)
            {
                if (this->RegisteredTickers.Contains(Key))
                {
                    auto &Ticker = this->RegisteredTickers[Key];
                    if (Ticker.IsBound())
                    {
                        if (!Ticker.Execute(DeltaSecondsSinceLastRegulatedTick))
                        {
                            this->RegisteredTickers.Remove(Key);
                        }
                    }
                }
            }
            InvocationCountThisSecond += 1;
        }
#if !(defined(UE_BUILD_SHIPPING) && UE_BUILD_SHIPPING)
    }
    else
    {
        TArray<FDelegateHandle> TickerHandles;
        this->RegisteredTickers.GetKeys(TickerHandles);
        for (const auto &Key : TickerHandles)
        {
            if (this->RegisteredTickers.Contains(Key))
            {
                auto &Ticker = this->RegisteredTickers[Key];
                if (Ticker.IsBound())
                {
                    if (!Ticker.Execute(DeltaSeconds))
                    {
                        this->RegisteredTickers.Remove(Key);
                    }
                }
            }
        }
        InvocationCountThisSecond += 1;
    }
#endif
    return true;
}

FEOSRegulatedTicker::FEOSRegulatedTicker(int32 InTicksPerSecond)
    : RegisteredTickers()
    , GlobalHandle()
    , TicksPerSecond(InTicksPerSecond)
    , AccumulatedDeltaSeconds(0.0f)
    , CounterAccumulatedSeconds(0.0f)
    , InvocationCount(0)
    , InvocationCountThisSecond(0)
{
}

FEOSRegulatedTicker::~FEOSRegulatedTicker()
{
    if (this->GlobalHandle.IsValid())
    {
        FUTicker::GetCoreTicker().RemoveTicker(this->GlobalHandle);
        this->GlobalHandle.Reset();
    }
}

FDelegateHandle FEOSRegulatedTicker::AddTicker(const FTickerDelegate &InDelegate)
{
    if (!this->GlobalHandle.IsValid())
    {
        this->GlobalHandle =
            FUTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateSP(this, &FEOSRegulatedTicker::Tick));
    }

    FDelegateHandle Handle(FDelegateHandle::EGenerateNewHandleType::GenerateNewHandle);
    this->RegisteredTickers.Add(Handle, InDelegate);
    return Handle;
}

void FEOSRegulatedTicker::RemoveTicker(FDelegateHandle Handle)
{
    if (this->RegisteredTickers.Contains(Handle))
    {
        this->RegisteredTickers.Remove(Handle);

        if (this->RegisteredTickers.Num() == 0 && this->GlobalHandle.IsValid())
        {
            FUTicker::GetCoreTicker().RemoveTicker(this->GlobalHandle);
            this->GlobalHandle.Reset();
        }
    }
}

EOS_DISABLE_STRICT_WARNINGS