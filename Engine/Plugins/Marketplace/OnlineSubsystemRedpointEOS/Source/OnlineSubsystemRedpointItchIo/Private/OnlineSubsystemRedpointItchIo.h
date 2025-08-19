// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemImplBase.h"
#include "OnlineSubsystemRedpointItchIoConstants.h"

#if EOS_ITCH_IO_ENABLED

class FOnlineSubsystemRedpointItchIo : public FOnlineSubsystemImplBase,
                                       public TSharedFromThis<FOnlineSubsystemRedpointItchIo, ESPMode::ThreadSafe>
{
private:
    TSharedPtr<class FOnlineIdentityInterfaceRedpointItchIo, ESPMode::ThreadSafe> IdentityImpl;
    TSharedPtr<class FOnlineAvatarInterfaceRedpointItchIo, ESPMode::ThreadSafe> AvatarImpl;

public:
    UE_NONCOPYABLE(FOnlineSubsystemRedpointItchIo);
    FOnlineSubsystemRedpointItchIo() = delete;
    FOnlineSubsystemRedpointItchIo(FName InInstanceName);
    ~FOnlineSubsystemRedpointItchIo();

    virtual bool IsEnabled() const override;

    virtual IOnlineIdentityPtr GetIdentityInterface() const override;

    // Our custom interfaces. Note that even those these APIs return a UObject*, we return
    // non-UObjects that are TSharedFromThis.
    virtual class UObject *GetNamedInterface(FName InterfaceName) override;
    virtual void SetNamedInterface(FName InterfaceName, class UObject *NewInterface) override
    {
        checkf(false, TEXT("FOnlineSubsystemRedpointItchIo::SetNamedInterface is not supported"));
    };

    virtual bool Init() override;
    virtual bool Tick(float DeltaTime) override;
    virtual bool Shutdown() override;

    virtual FString GetAppId() const override;
    virtual FText GetOnlineServiceName() const override;
};

#endif