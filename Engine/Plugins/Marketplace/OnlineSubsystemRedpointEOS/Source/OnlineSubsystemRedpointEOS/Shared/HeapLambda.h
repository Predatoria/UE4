// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include <functional>

EOS_ENABLE_STRICT_WARNINGS

enum class EHeapLambdaFlags
{
    // No additional behaviour.
    None = 0x0,

    // Always fire the lambda at least once (via the destructor) even if it's never explicitly invoked.
    AlwaysCleanup = 0x1,

    // Only fire the callback once, no matter how many time the () operator is used.
    OneShot = 0x2,

    // Always fire the callback only once.
    OneShotCleanup = OneShot | AlwaysCleanup,
};

namespace FHeapLambdaNS
{
template <EHeapLambdaFlags TFlags, typename... TArgs> struct FHeapLambdaBehaviour;
} // namespace FHeapLambdaNS

template <EHeapLambdaFlags TFlags, typename... TArgs> class FHeapLambda
{
private:
    class HeapLambdaState
    {
    public:
        std::function<void(TArgs...)> *CallbackOnHeap;
        int RefCount;
        int FireCount;

        HeapLambdaState()
            : CallbackOnHeap(nullptr)
            , RefCount(1)
            , FireCount(0)
        {
        }
        UE_NONCOPYABLE(HeapLambdaState);

        void ClearCallback()
        {
            if (this->CallbackOnHeap != nullptr)
            {
                delete this->CallbackOnHeap;
                this->CallbackOnHeap = nullptr;
            }
        }

        ~HeapLambdaState()
        {
            this->ClearCallback();
        }
    };
    HeapLambdaState *State;
    bool *DestructionTracker;

public:
    FHeapLambda()
        : State(new HeapLambdaState())
        , DestructionTracker(nullptr)
    {
        this->State = new HeapLambdaState();
        this->DestructionTracker = nullptr;
    }
    FHeapLambda(const FHeapLambda &Other)
        : State(Other.State)
        , DestructionTracker(nullptr)
    {
        this->State->RefCount++;
    };
    FHeapLambda(FHeapLambda &&Other)
        : State(Other.State)
        , DestructionTracker(nullptr)
    {
        this->State->RefCount++;
    }
    template <class F>
    FHeapLambda(F f)
        : FHeapLambda(std::function<void(TArgs...)>(f))
    {
    }
    FHeapLambda(const std::function<void(TArgs...)> &Callback)
        : State(new HeapLambdaState())
        , DestructionTracker(nullptr)
    {
        this->Assign(Callback);
    };
    ~FHeapLambda()
    {
        this->Reset();
        this->State = nullptr;

        // The destruction tracker lets the invocation code know if
        // the this pointer is no longer safe to use.
        if (this->DestructionTracker != nullptr)
        {
            *this->DestructionTracker = true;
        }
        this->DestructionTracker = nullptr;
    };
    // Do not implement this operator.
    FHeapLambda &operator=(const FHeapLambda &Other) = delete;
    FHeapLambda &operator=(FHeapLambda &&Other)
    {
        this->Reset();
        delete this->State;
        this->State = Other.State;
        this->State->RefCount++;
        return *this;
    }
    void Reset()
    {
        check(this->State->RefCount > 0);
        this->State->RefCount--;
        if (this->State->RefCount == 0)
        {
            if (this->State->FireCount == 0)
            {
                FHeapLambdaNS::FHeapLambdaBehaviour<TFlags, TArgs...>::DoCleanup(this);
            }

            delete this->State;
            this->State = new HeapLambdaState();
        }
    }
    template <class F> void Assign(F Callback)
    {
        check(this->State->CallbackOnHeap == nullptr /* Lambda must not already be assigned! */);
        this->State->CallbackOnHeap = new std::function<void(TArgs...)>(Callback);
    }
    void Assign(std::function<void(TArgs...)> Callback) const
    {
        check(this->State->CallbackOnHeap == nullptr /* Lambda must not already be assigned! */);
        this->State->CallbackOnHeap = new std::function<void(TArgs...)>(Callback);
    }
    template <class F> FHeapLambda &operator=(F Callback)
    {
        this->Assign(std::function<void(TArgs...)>(Callback));
        return *this;
    }
    FHeapLambda &operator=(std::function<void(TArgs...)> Callback)
    {
        this->Assign(Callback);
        return *this;
    }
    void operator()(TArgs... args) const
    {
        bool bSetupDestructionTracker = this->DestructionTracker == nullptr;

        if (this->State != nullptr && this->State->CallbackOnHeap != nullptr)
        {
            if (!FHeapLambdaNS::FHeapLambdaBehaviour<TFlags, TArgs...>::ShouldOnlyFireOnce() ||
                this->State->FireCount == 0)
            {
                // Keep a local for the state on the call stack, because once we invoke the
                // callback, our FHeapLambda this pointer might be freed, and we need to continue
                // to access the state to do cleanup.
                auto StateLocal = this->State;

                // Set up the destruction tracker so that we know after the callback
                // whether we should reset our state via the this pointer, or if we should
                // just clean up the state indirectly because we've already been freed.
                if (bSetupDestructionTracker)
                {
                    const_cast<FHeapLambda *>(this)->DestructionTracker = new bool(false);
                }
                auto DestructionTrackerLocal = const_cast<FHeapLambda *>(this)->DestructionTracker;

                // Obtain a reference while we run the callback, to prevent the state from being
                // freed while we still have a local reference to it.
                StateLocal->RefCount++;

                // Increment the fire count to track invocations.
                StateLocal->FireCount++;

                // Invoke the callback.
                (*StateLocal->CallbackOnHeap)(args...);

                if (FHeapLambdaNS::FHeapLambdaBehaviour<TFlags, TArgs...>::ShouldOnlyFireOnce())
                {
                    // Ensures that any references held by the lambda are immediately freed after the first
                    // fire.
                    StateLocal->ClearCallback();
                }

                // If we're still alive, use the normal reset path. Otherwise, manually release the
                // reference off StateLocal.
                if (!bSetupDestructionTracker || *DestructionTrackerLocal == false)
                {
                    const_cast<FHeapLambda *>(this)->Reset();
                    if (bSetupDestructionTracker)
                    {
                        delete this->DestructionTracker;
                        const_cast<FHeapLambda *>(this)->DestructionTracker = nullptr;
                    }
                }
                else
                {
                    check(StateLocal->RefCount > 0);
                    StateLocal->RefCount--;
                    if (StateLocal->RefCount == 0)
                    {
                        delete StateLocal;
                    }
                    delete DestructionTrackerLocal;
                }
            }
        }
    }
};

namespace FHeapLambdaNS
{
template <typename... TArgs> struct FHeapLambdaBehaviour<EHeapLambdaFlags::None, TArgs...>
{
public:
    static void DoCleanup(FHeapLambda<EHeapLambdaFlags::None, TArgs...> *Lambda)
    {
    }
    static bool ShouldOnlyFireOnce()
    {
        return false;
    }
};
template <typename... TArgs> struct FHeapLambdaBehaviour<EHeapLambdaFlags::AlwaysCleanup, TArgs...>
{
public:
    static void DoCleanup(FHeapLambda<EHeapLambdaFlags::AlwaysCleanup, TArgs...> *Lambda)
    {
        (*Lambda)();
    }
    static bool ShouldOnlyFireOnce()
    {
        return false;
    }
};
template <typename... TArgs> struct FHeapLambdaBehaviour<EHeapLambdaFlags::OneShot, TArgs...>
{
public:
    static void DoCleanup(FHeapLambda<EHeapLambdaFlags::OneShot, TArgs...> *Lambda)
    {
    }
    static bool ShouldOnlyFireOnce()
    {
        return true;
    }
};
template <typename... TArgs> struct FHeapLambdaBehaviour<EHeapLambdaFlags::OneShotCleanup, TArgs...>
{
public:
    static void DoCleanup(FHeapLambda<EHeapLambdaFlags::OneShotCleanup, TArgs...> *Lambda)
    {
        (*Lambda)();
    }
    static bool ShouldOnlyFireOnce()
    {
        return true;
    }
};
} // namespace FHeapLambdaNS

EOS_DISABLE_STRICT_WARNINGS