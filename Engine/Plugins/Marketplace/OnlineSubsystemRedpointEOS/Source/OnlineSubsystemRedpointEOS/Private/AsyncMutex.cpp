// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/AsyncMutex.h"

EOS_ENABLE_STRICT_WARNINGS

void FAsyncMutex::Run(const std::function<void(const std::function<void()> &MutexRelease)> &Callback)
{
    if (this->Available)
    {
        this->Available = false;
        Callback([this]() {
            this->Available = true;
            if (this->Pending.Num() != 0)
            {
                auto Next = this->Pending[0];
                this->Pending.RemoveAt(0);
                this->Run(Next);
            }
        });
    }
    else
    {
        this->Pending.Add(Callback);
    }
}

EOS_DISABLE_STRICT_WARNINGS
