// Copyright June Rhodes. All Rights Reserved.

#include "./OnlineAsyncTaskSteamLoadItemDefinitions.h"

#include "../LogRedpointSteam.h"
#include "../OnlineSubsystemRedpointSteam.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"

#if EOS_STEAM_ENABLED

EOS_ENABLE_STRICT_WARNINGS

void FOnlineAsyncTaskSteamLoadItemDefinitions::Tick()
{
    ISteamUtils *SteamUtilsPtr = SteamUtils();
    check(SteamUtilsPtr);

    if (!this->bInit)
    {
        this->LoadCallbackHandle = SteamInventory()->LoadItemDefinitions();
        this->bInit = true;
    }

    if (this->LoadCallbackHandle != k_uAPICallInvalid)
    {
        bool bFailedCall = false;
        bool bIsLoadCompleted =
            SteamUtilsPtr->IsAPICallCompleted(this->LoadCallbackHandle, &bFailedCall) ? true : false;
        if (bIsLoadCompleted)
        {
            bool bFailedResult;
            // Retrieve the callback data from the request
            bool bSuccessCallResult = SteamUtilsPtr->GetAPICallResult(
                this->LoadCallbackHandle,
                &this->LoadCallbackResults,
                sizeof(this->LoadCallbackResults),
                this->LoadCallbackResults.k_iCallback,
                &bFailedResult);

            // LoadItemDefinitions always seems to "fail" successfully, so ignore the failure result and just
            // proceed anyway.
            this->bIsComplete = true;
            this->bWasSuccessful = true;
        }
    }
    else
    {
        // Invalid API call
        this->FailureContext = TEXT("LoadItemDefinitions API call invalid");
        this->bIsComplete = true;
        this->bWasSuccessful = false;
    }
}

void FOnlineAsyncTaskSteamLoadItemDefinitions::Finalize()
{
    if (!this->bWasSuccessful)
    {
        this->Delegate.ExecuteIfBound(OnlineRedpointEOS::Errors::UnexpectedError(
            TEXT("FOnlineAsyncTaskSteamLoadItemDefinitions::Finalize"),
            FString::Printf(
                TEXT("Failed to retrieve item definitions from Steam for e-commerce (%s)."),
                *this->FailureContext)));
        return;
    }

    this->Delegate.ExecuteIfBound(OnlineRedpointEOS::Errors::Success());
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_STEAM_ENABLED