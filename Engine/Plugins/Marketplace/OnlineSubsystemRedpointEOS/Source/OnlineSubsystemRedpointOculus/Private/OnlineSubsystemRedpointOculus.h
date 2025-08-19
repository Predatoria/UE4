// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_OCULUS_ENABLED

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemImplBase.h"
#include "OnlineSubsystemRedpointOculusConstants.h"

class FOnlineSubsystemRedpointOculus : public FOnlineSubsystemImplBase,
                                       public TSharedFromThis<FOnlineSubsystemRedpointOculus, ESPMode::ThreadSafe>
{
private:
    TSharedPtr<class FOnlineAvatarInterfaceRedpointOculus, ESPMode::ThreadSafe> AvatarImpl;

public:
    UE_NONCOPYABLE(FOnlineSubsystemRedpointOculus);
    FOnlineSubsystemRedpointOculus() = delete;
    FOnlineSubsystemRedpointOculus(FName InInstanceName);
    ~FOnlineSubsystemRedpointOculus();

    virtual bool IsEnabled() const override;

    // Our custom interfaces. Note that even those these APIs return a UObject*, we return
    // non-UObjects that are TSharedFromThis.
    virtual class UObject *GetNamedInterface(FName InterfaceName) override;
    virtual void SetNamedInterface(FName InterfaceName, class UObject *NewInterface) override
    {
        checkf(false, TEXT("FOnlineSubsystemEOS::SetNamedInterface is not supported"));
    };

    virtual bool Init() override;
    virtual bool Shutdown() override;

    virtual FString GetAppId() const override;
    virtual FText GetOnlineServiceName() const override;
};

#endif