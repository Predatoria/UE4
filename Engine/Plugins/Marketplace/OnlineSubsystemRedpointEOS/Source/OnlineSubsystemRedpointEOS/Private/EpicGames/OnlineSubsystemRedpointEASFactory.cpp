// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineSubsystemRedpointEASFactory.h"

#if EOS_HAS_AUTHENTICATION
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineSubsystemRedpointEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#endif // #if EOS_HAS_AUTHENTICATION

EOS_ENABLE_STRICT_WARNINGS

FOnlineSubsystemRedpointEASFactory::FOnlineSubsystemRedpointEASFactory(FOnlineSubsystemRedpointEOSModule *InModule)
    : Module(InModule)
{
}

IOnlineSubsystemPtr FOnlineSubsystemRedpointEASFactory::CreateSubsystem(FName InstanceName)
{
#if EOS_HAS_AUTHENTICATION
    if (Module->SubsystemInstances.Contains(InstanceName))
    {
        return Module->SubsystemInstances[InstanceName]->SubsystemEAS;
    }

    TSharedPtr<FOnlineSubsystemEOS, ESPMode::ThreadSafe> EOSSubsystem =
        StaticCastSharedPtr<FOnlineSubsystemEOS>(Module->CreateSubsystem(InstanceName));
    return StaticCastSharedPtr<IOnlineSubsystem, FOnlineSubsystemRedpointEAS, ESPMode::ThreadSafe>(
        EOSSubsystem->SubsystemEAS);
#else
    return nullptr;
#endif // #if EOS_HAS_AUTHENTICATION
}

EOS_DISABLE_STRICT_WARNINGS