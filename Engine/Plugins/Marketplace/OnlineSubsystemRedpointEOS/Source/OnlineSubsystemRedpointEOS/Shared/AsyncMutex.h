// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Containers/Array.h"
#include <functional>

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FAsyncMutex
{
private:
    bool Available;
    TArray<std::function<void(const std::function<void()> &MutexRelease)>> Pending;

public:
    FAsyncMutex()
        : Available(true){};
    void Run(const std::function<void(const std::function<void()> &MutexRelease)> &Callback);
};

EOS_DISABLE_STRICT_WARNINGS
