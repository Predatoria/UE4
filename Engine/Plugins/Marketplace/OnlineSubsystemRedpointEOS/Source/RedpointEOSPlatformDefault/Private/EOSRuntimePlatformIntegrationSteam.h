// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_STEAM_ENABLED

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSRuntimePlatform.h"

class FEOSRuntimePlatformIntegrationSteam : public IEOSRuntimePlatformIntegration
{
public:
    virtual TSharedPtr<const class FUniqueNetId> GetUserId(
        TSoftObjectPtr<class UWorld> InWorld,
        EOS_Connect_ExternalAccountInfo *InExternalInfo) override;
    virtual bool CanProvideExternalAccountId(const FUniqueNetId &InUserId) override;
    virtual FExternalAccountIdInfo GetExternalAccountId(const FUniqueNetId &InUserId) override;
};

#endif