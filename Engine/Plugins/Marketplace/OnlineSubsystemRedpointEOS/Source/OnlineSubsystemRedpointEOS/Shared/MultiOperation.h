// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Containers/Array.h"
#include "Misc/ScopeLock.h"
#include "Templates/SharedPointer.h"
#include <functional>

EOS_ENABLE_STRICT_WARNINGS

template <typename TKey, typename TValue> TArray<TTuple<TKey, TValue>> UnpackMap(TMap<TKey, TValue> &Map)
{
    TArray<TTuple<TKey, TValue>> Arr;
    for (const auto &KV : Map)
    {
        Arr.Add(KV);
    }
    return Arr;
}

template <typename TKey, typename TIn>
TMap<TKey, TArray<TIn>> GroupBy(TArray<TIn> Array, std::function<TKey(const TIn &)> KeyMapper)
{
    TMap<TKey, TArray<TIn>> Result;
    for (const auto &Val : Array)
    {
        auto Key = KeyMapper(Val);
        if (!Result.Contains(Key))
        {
            Result.Add(Key, TArray<TIn>());
        }
        Result[Key].Add(Val);
    }
    return Result;
}

template <typename TIn, typename TOut> class FMultiOperation
{
private:
    FMultiOperation()
        : DoneMarkers()
        , OutValues()
        , bDoneCalled(false)
        , bDisallowFinalize(false){};

    TArray<bool> DoneMarkers;
    TArray<TOut> OutValues;
    bool bDoneCalled;
    bool bDisallowFinalize;

    void HandleDone(FMultiOperation *Op, int i, std::function<void(TArray<TOut> OutValues)> OnDone)
    {
        Op->DoneMarkers[i] = true;
        this->CheckDone(Op, OnDone);
    };

    void CheckDone(FMultiOperation *Op, std::function<void(TArray<TOut> OutValues)> OnDone)
    {
        if (Op->bDisallowFinalize)
        {
            // Some OSS implementations are very poor and return false even when they have fired the delegate, which
            // results in a double call to HandleDone. bDisallowFinalize prevents CheckDone from clearing memory while
            // the initial calls are being made which makes double invocation in the case of an immediate failure safe.
            return;
        }
        bool AllDone = true;
        for (auto Marker : Op->DoneMarkers)
        {
            if (!Marker)
            {
                AllDone = false;
                break;
            }
        }
        if (AllDone)
        {
            OnDone(Op->OutValues);
            delete Op;
        }
    };

public:
    static void Run(
        TArray<TIn> Inputs,
        std::function<bool(TIn Value, std::function<void(TOut OutValue)> OnDone)> Initiate,
        std::function<void(TArray<TOut> OutValues)> OnDone,
        TOut DefaultValue = TOut())
    {
        if (Inputs.Num() == 0)
        {
            // If there are no inputs, fire the OnDone immediately.
            OnDone(TArray<TOut>());
            return;
        }

        FMultiOperation *Op = new FMultiOperation();

        Op->DoneMarkers = TArray<bool>();
        Op->OutValues = TArray<TOut>();
        Op->bDisallowFinalize = true;
        for (auto i = 0; i < Inputs.Num(); i++)
        {
            Op->DoneMarkers.Add(false);
            Op->OutValues.Add(DefaultValue);
        }

        for (auto i = 0; i < Inputs.Num(); i++)
        {
            bool ShouldWait = Initiate(Inputs[i], [Op, i, OnDone](TOut OutValue) {
                Op->OutValues[i] = OutValue;
                Op->HandleDone(Op, i, OnDone);
            });
            if (!ShouldWait)
            {
                // Didn't start successfully.
                Op->HandleDone(Op, i, OnDone);
            }
        }
        Op->bDisallowFinalize = false;
        Op->CheckDone(Op, OnDone);
    }

    static void RunBatched(
        TArray<TIn> Inputs,
        int BatchMaxSize,
        std::function<bool(TArray<TIn> &InBatch, std::function<void(TArray<TOut> OutBatch)> OnDone)> Initiate,
        std::function<void(TArray<TOut> OutValues)> OnDone,
        TOut DefaultValue = TOut())
    {
        if (Inputs.Num() == 0)
        {
            // If there are no inputs, fire the OnDone immediately.
            OnDone(TArray<TOut>());
            return;
        }

        FMultiOperation *Op = new FMultiOperation();

        Op->DoneMarkers = TArray<bool>();
        Op->OutValues = TArray<TOut>();
        Op->bDisallowFinalize = true;
        for (auto i = 0; i < Inputs.Num(); i++)
        {
            Op->DoneMarkers.Add(false);
            Op->OutValues.Add(DefaultValue);
        }

        for (auto i = 0; i < Inputs.Num(); i += BatchMaxSize)
        {
            TArray<TIn> Batch;
            for (auto BIn = i; BIn < Inputs.Num() && BIn < i + BatchMaxSize; BIn++)
            {
                Batch.Add(Inputs[BIn]);
            }
            int ExpectedOutSize = Batch.Num();
            check(ExpectedOutSize > 0 && ExpectedOutSize <= BatchMaxSize);
            bool ShouldWait = Initiate(Batch, [Op, i, OnDone, ExpectedOutSize](TArray<TOut> OutBatch) {
                check(OutBatch.Num() == ExpectedOutSize /* result batch did not match expected size */);
                for (auto BOut = 0; BOut < ExpectedOutSize; BOut++)
                {
                    Op->OutValues[i + BOut] = OutBatch[BOut];
                    Op->DoneMarkers[i + BOut] = true;
                }
                Op->CheckDone(Op, OnDone);
            });
            if (!ShouldWait)
            {
                // Didn't start successfully.
                for (auto BOut = 0; BOut < ExpectedOutSize; BOut++)
                {
                    Op->DoneMarkers[i + BOut] = true;
                }
                Op->CheckDone(Op, OnDone);
            }
        }
        Op->bDisallowFinalize = false;
        Op->CheckDone(Op, OnDone);
    }
};

EOS_DISABLE_STRICT_WARNINGS
