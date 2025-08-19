// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "./OnlineAsyncTaskSteamDelegates.h"
#include "OnlineAsyncTaskManager.h"

#if EOS_STEAM_ENABLED

THIRD_PARTY_INCLUDES_START
#include "steam/steam_api.h"
THIRD_PARTY_INCLUDES_END

EOS_ENABLE_STRICT_WARNINGS

class FOnlineAsyncTaskSteamGetOwnedDLC : public FOnlineAsyncTaskBasic<class FOnlineSubsystemSteam>
{
private:
    bool bInit;
    FSteamEntitlementsFetched Delegate;
    FString FailureContext;

public:
    FOnlineAsyncTaskSteamGetOwnedDLC(FSteamEntitlementsFetched InDelegate)
        : bInit(false)
        , Delegate(MoveTemp(InDelegate))
        , FailureContext()
    {
    }

    virtual ~FOnlineAsyncTaskSteamGetOwnedDLC()
    {
    }

    virtual FString ToString() const override
    {
        return TEXT("FOnlineAsyncTaskSteamGetOwnedDLC");
    }

    virtual void Tick() override;
    virtual void Finalize() override;
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_STEAM_ENABLED