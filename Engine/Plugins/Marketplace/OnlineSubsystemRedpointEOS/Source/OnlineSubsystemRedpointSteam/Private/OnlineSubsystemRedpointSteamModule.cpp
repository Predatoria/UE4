// Copyright June Rhodes. All Rights Reserved.

#include "CoreMinimal.h"
#include "LogRedpointSteam.h"
#include "Modules/ModuleManager.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointSteam.h"

class FOnlineSubsystemRedpointSteamModule : public IModuleInterface, public IOnlineFactory
{
private:
    bool IsRegisteredAsSubsystem;

public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    virtual IOnlineSubsystemPtr CreateSubsystem(FName InstanceName) override;
};

void FOnlineSubsystemRedpointSteamModule::StartupModule()
{
    if (IsRunningCommandlet())
    {
        return;
    }

    FModuleManager &ModuleManager = FModuleManager::Get();
    auto OSS = ModuleManager.GetModule("OnlineSubsystem");
    if (OSS != nullptr)
    {
        ((FOnlineSubsystemModule *)OSS)->RegisterPlatformService(REDPOINT_STEAM_SUBSYSTEM, this);
        this->IsRegisteredAsSubsystem = true;
    }

    UE_LOG(LogRedpointSteam, Verbose, TEXT("Redpoint Steam module has finished starting up."));
}

void FOnlineSubsystemRedpointSteamModule::ShutdownModule()
{
    if (this->IsRegisteredAsSubsystem)
    {
        FModuleManager &ModuleManager = FModuleManager::Get();
        auto OSS = ModuleManager.GetModule("OnlineSubsystem");
        if (OSS != nullptr)
        {
            ((FOnlineSubsystemModule *)OSS)->UnregisterPlatformService(REDPOINT_STEAM_SUBSYSTEM);
            this->IsRegisteredAsSubsystem = false;
        }

        UE_LOG(LogRedpointSteam, Verbose, TEXT("Redpoint Steam module shutdown complete."));
    }
}

IOnlineSubsystemPtr FOnlineSubsystemRedpointSteamModule::CreateSubsystem(FName InstanceName)
{
#if EOS_STEAM_ENABLED
    auto Inst = MakeShared<FOnlineSubsystemRedpointSteam, ESPMode::ThreadSafe>(InstanceName);
    if (Inst->IsEnabled())
    {
        if (!Inst->Init())
        {
            UE_LOG(LogRedpointSteam, Warning, TEXT("Unable to init Redpoint Steam online subsystem."));
            Inst->Shutdown();
            return nullptr;
        }
    }
    else
    {
        UE_LOG(LogRedpointSteam, Warning, TEXT("Redpoint Steam online subsystem is not enabled."));
        Inst->Shutdown();
        return nullptr;
    }

    return Inst;
#else
    // Not compiled with Steam support.
    return nullptr;
#endif
}

IMPLEMENT_MODULE(FOnlineSubsystemRedpointSteamModule, OnlineSubsystemRedpointSteam)