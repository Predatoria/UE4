// Copyright June Rhodes. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#if EOS_OCULUS_ENABLED

#include "LogRedpointOculus.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointOculus.h"

class FOnlineSubsystemRedpointOculusModule : public IModuleInterface, public IOnlineFactory
{
private:
    bool IsRegisteredAsSubsystem;

public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    virtual IOnlineSubsystemPtr CreateSubsystem(FName InstanceName) override;
};

void FOnlineSubsystemRedpointOculusModule::StartupModule()
{
    if (IsRunningCommandlet())
    {
        return;
    }

    FModuleManager &ModuleManager = FModuleManager::Get();
    auto OSS = ModuleManager.GetModule("OnlineSubsystem");
    if (OSS != nullptr)
    {
        ((FOnlineSubsystemModule *)OSS)->RegisterPlatformService(REDPOINT_OCULUS_SUBSYSTEM, this);
        this->IsRegisteredAsSubsystem = true;
    }

    UE_LOG(LogRedpointOculus, Verbose, TEXT("Redpoint Oculus module has finished starting up."));
}

void FOnlineSubsystemRedpointOculusModule::ShutdownModule()
{
    if (this->IsRegisteredAsSubsystem)
    {
        FModuleManager &ModuleManager = FModuleManager::Get();
        auto OSS = ModuleManager.GetModule("OnlineSubsystem");
        if (OSS != nullptr)
        {
            ((FOnlineSubsystemModule *)OSS)->UnregisterPlatformService(REDPOINT_OCULUS_SUBSYSTEM);
            this->IsRegisteredAsSubsystem = false;
        }

        UE_LOG(LogRedpointOculus, Verbose, TEXT("Redpoint Oculus module shutdown complete."));
    }
}

IOnlineSubsystemPtr FOnlineSubsystemRedpointOculusModule::CreateSubsystem(FName InstanceName)
{
#if EOS_OCULUS_ENABLED
    auto Inst = MakeShared<FOnlineSubsystemRedpointOculus, ESPMode::ThreadSafe>(InstanceName);
    if (Inst->IsEnabled())
    {
        if (!Inst->Init())
        {
            UE_LOG(LogRedpointOculus, Warning, TEXT("Unable to init Redpoint Oculus online subsystem."));
            Inst->Shutdown();
            return nullptr;
        }
    }
    else
    {
        UE_LOG(LogRedpointOculus, Warning, TEXT("Redpoint Oculus online subsystem is not enabled."));
        Inst->Shutdown();
        return nullptr;
    }

    return Inst;
#else
    // Not compiled with Oculus support.
    return nullptr;
#endif
}

IMPLEMENT_MODULE(FOnlineSubsystemRedpointOculusModule, OnlineSubsystemRedpointOculus)
#else
IMPLEMENT_MODULE(FDefaultModuleImpl, OnlineSubsystemRedpointOculus);
#endif