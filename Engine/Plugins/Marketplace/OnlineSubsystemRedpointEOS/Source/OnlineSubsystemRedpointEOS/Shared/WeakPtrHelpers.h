// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

EOS_ENABLE_STRICT_WARNINGS

template <typename T, ESPMode Mode> TWeakPtr<T, Mode> GetWeakThis(TSharedFromThis<T, Mode> *This)
{
    return TWeakPtr<T, Mode>(This->AsShared());
}

template <typename T, ESPMode Mode> TWeakPtr<T, Mode> GetWeakThis(TSharedPtr<T, Mode> This)
{
    return TWeakPtr<T, Mode>(This);
}

template <typename T, ESPMode Mode> TWeakPtr<T, Mode> GetWeakThis(TSharedRef<T, Mode> This)
{
    return TWeakPtr<T, Mode>(This);
}

template <typename T, ESPMode Mode> TSharedPtr<T, Mode> PinWeakThis(TWeakPtr<T, Mode> WeakThis)
{
    if (auto Pinned = WeakThis.Pin())
    {
        if (Pinned.IsValid())
        {
            return Pinned;
        }
    }

    return nullptr;
}

EOS_DISABLE_STRICT_WARNINGS