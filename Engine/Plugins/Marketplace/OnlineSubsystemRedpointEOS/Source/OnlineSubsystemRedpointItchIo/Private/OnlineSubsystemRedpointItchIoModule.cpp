// Copyright June Rhodes. All Rights Reserved.

#include "CoreMinimal.h"
#include "LogRedpointItchIo.h"
#include "Modules/ModuleManager.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointItchIo.h"

class FOnlineSubsystemRedpointItchIoModule : public IModuleInterface, public IOnlineFactory
{
private:
    bool IsRegisteredAsSubsystem;

public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    virtual IOnlineSubsystemPtr CreateSubsystem(FName InstanceName) override;
};

void FOnlineSubsystemRedpointItchIoModule::StartupModule()
{
    if (IsRunningCommandlet())
    {
        return;
    }

    FModuleManager &ModuleManager = FModuleManager::Get();
    auto OSS = ModuleManager.GetModule("OnlineSubsystem");
    if (OSS != nullptr)
    {
        ((FOnlineSubsystemModule *)OSS)->RegisterPlatformService(REDPOINT_ITCH_IO_SUBSYSTEM, this);
        this->IsRegisteredAsSubsystem = true;
    }

    UE_LOG(LogRedpointItchIo, Verbose, TEXT("Redpoint itch.io module has finished starting up."));
}

void FOnlineSubsystemRedpointItchIoModule::ShutdownModule()
{
    if (this->IsRegisteredAsSubsystem)
    {
        FModuleManager &ModuleManager = FModuleManager::Get();
        auto OSS = ModuleManager.GetModule("OnlineSubsystem");
        if (OSS != nullptr)
        {
            ((FOnlineSubsystemModule *)OSS)->UnregisterPlatformService(REDPOINT_ITCH_IO_SUBSYSTEM);
            this->IsRegisteredAsSubsystem = false;
        }

        UE_LOG(LogRedpointItchIo, Verbose, TEXT("Redpoint itch.io module shutdown complete."));
    }
}

IOnlineSubsystemPtr FOnlineSubsystemRedpointItchIoModule::CreateSubsystem(FName InstanceName)
{
#if EOS_ITCH_IO_ENABLED
    auto Inst = MakeShared<FOnlineSubsystemRedpointItchIo, ESPMode::ThreadSafe>(InstanceName);
    if (Inst->IsEnabled())
    {
        if (!Inst->Init())
        {
            UE_LOG(LogRedpointItchIo, Warning, TEXT("Unable to init itch.io online subsystem."));
            Inst->Shutdown();
            return nullptr;
        }
    }
    else
    {
        UE_LOG(LogRedpointItchIo, Warning, TEXT("itch.io online subsystem is not enabled."));
        Inst->Shutdown();
        return nullptr;
    }

    return Inst;
#else
    // Not compiled with itch.io support.
    return nullptr;
#endif
}

IMPLEMENT_MODULE(FOnlineSubsystemRedpointItchIoModule, OnlineSubsystemRedpointItchIo)