// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

#if EOS_OCULUS_ENABLED && EOS_VERSION_AT_LEAST(1, 10, 3)

#include "OnlineSubsystemRedpointEOS/Shared/EOSRuntimePlatform.h"

class FEOSRuntimePlatformIntegrationOculus : public IEOSRuntimePlatformIntegration
{
public:
    virtual TSharedPtr<const class FUniqueNetId> GetUserId(
        TSoftObjectPtr<class UWorld> InWorld,
        EOS_Connect_ExternalAccountInfo *InExternalInfo) override;
    virtual bool CanProvideExternalAccountId(const FUniqueNetId &InUserId) override;
    virtual FExternalAccountIdInfo GetExternalAccountId(const FUniqueNetId &InUserId) override;
    virtual void MutateFriendsListNameIfRequired(FName InSubsystemName, FString &InOutListName) const override;
};

#endif