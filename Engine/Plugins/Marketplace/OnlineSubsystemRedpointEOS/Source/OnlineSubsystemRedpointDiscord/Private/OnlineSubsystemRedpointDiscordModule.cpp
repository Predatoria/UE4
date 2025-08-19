// Copyright June Rhodes. All Rights Reserved.

#include "CoreMinimal.h"
#include "LogRedpointDiscord.h"
#include "Modules/ModuleManager.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointDiscord.h"

class FOnlineSubsystemRedpointDiscordModule : public IModuleInterface, public IOnlineFactory
{
private:
    bool IsRegisteredAsSubsystem;

public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    virtual IOnlineSubsystemPtr CreateSubsystem(FName InstanceName) override;
};

void FOnlineSubsystemRedpointDiscordModule::StartupModule()
{
    if (IsRunningCommandlet())
    {
        return;
    }

    FModuleManager &ModuleManager = FModuleManager::Get();

    auto DiscordRuntime = ModuleManager.LoadModule("RedpointDiscordGameSDKRuntime");
    if (DiscordRuntime == nullptr)
    {
        UE_LOG(
            LogRedpointDiscord,
            Warning,
            TEXT("Discord runtime module is not available; skipping registration of Redpoint Discord OSS."));
        return;
    }

    if (!((FRedpointDiscordGameSDKRuntimeModule *)DiscordRuntime)->IsAvailable())
    {
        UE_LOG(
            LogRedpointDiscord,
            Warning,
            TEXT("Discord runtime is not available; skipping registration of Redpoint Discord OSS."));
        return;
    }

    auto OSS = ModuleManager.GetModule("OnlineSubsystem");
    if (OSS != nullptr)
    {
        ((FOnlineSubsystemModule *)OSS)->RegisterPlatformService(REDPOINT_DISCORD_SUBSYSTEM, this);
        this->IsRegisteredAsSubsystem = true;
    }

    UE_LOG(LogRedpointDiscord, Verbose, TEXT("Redpoint Discord module has finished starting up."));
}

void FOnlineSubsystemRedpointDiscordModule::ShutdownModule()
{
    if (this->IsRegisteredAsSubsystem)
    {
        FModuleManager &ModuleManager = FModuleManager::Get();
        auto OSS = ModuleManager.GetModule("OnlineSubsystem");
        if (OSS != nullptr)
        {
            ((FOnlineSubsystemModule *)OSS)->UnregisterPlatformService(REDPOINT_DISCORD_SUBSYSTEM);
            this->IsRegisteredAsSubsystem = false;
        }

        UE_LOG(LogRedpointDiscord, Verbose, TEXT("Redpoint Discord module shutdown complete."));
    }
}

IOnlineSubsystemPtr FOnlineSubsystemRedpointDiscordModule::CreateSubsystem(FName InstanceName)
{
#if EOS_DISCORD_ENABLED
    auto Inst = MakeShared<FOnlineSubsystemRedpointDiscord, ESPMode::ThreadSafe>(InstanceName);
    if (Inst->IsEnabled())
    {
        if (!Inst->Init())
        {
            UE_LOG(LogRedpointDiscord, Warning, TEXT("Unable to init Discord online subsystem."));
            Inst->Shutdown();
            return nullptr;
        }
    }
    else
    {
        UE_LOG(LogRedpointDiscord, Warning, TEXT("Discord online subsystem is not enabled."));
        Inst->Shutdown();
        return nullptr;
    }

    return Inst;
#else
    // Not compiled with Discord Game SDK support.
    return nullptr;
#endif
}

IMPLEMENT_MODULE(FOnlineSubsystemRedpointDiscordModule, OnlineSubsystemRedpointDiscord)