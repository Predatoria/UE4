// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemImplBase.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FOnlineSubsystemRedpointNull
    : public FOnlineSubsystemImplBase,
      public TSharedFromThis<FOnlineSubsystemRedpointNull, ESPMode::ThreadSafe>
{
private:
    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
    IOnlineIdentityPtr IdentityImpl;

public:
    FOnlineSubsystemRedpointNull() = delete;
    FOnlineSubsystemRedpointNull(FName InInstanceName);
    UE_NONCOPYABLE(FOnlineSubsystemRedpointNull);
    virtual ~FOnlineSubsystemRedpointNull(){};

    virtual IOnlineIdentityPtr GetIdentityInterface() const override;

    virtual bool Init() override;
    virtual bool Shutdown() override;
    virtual FString GetAppId() const override;
    virtual FText GetOnlineServiceName(void) const override;
};

EOS_DISABLE_STRICT_WARNINGS