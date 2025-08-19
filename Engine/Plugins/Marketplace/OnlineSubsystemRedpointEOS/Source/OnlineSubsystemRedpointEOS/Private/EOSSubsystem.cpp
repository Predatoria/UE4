// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Public/EOSSubsystem.h"

#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

UEOSSubsystem *UEOSSubsystem::GetSubsystem(UWorld *InWorld)
{
    if (IsValid(InWorld))
    {
        UGameInstance *GameInstance = InWorld->GetGameInstance();
        if (IsValid(GameInstance))
        {
            UEOSSubsystem *GlobalSubsystem = GameInstance->GetSubsystem<UEOSSubsystem>();
            if (IsValid(GlobalSubsystem))
            {
                return GlobalSubsystem;
            }
            else
            {
                UE_LOG(
                    LogEOS,
                    Warning,
                    TEXT("UEOSSubsystem::GetSubsystem: Game instance does not have UEOSSubsystem."));
                return nullptr;
            }
        }
        else
        {
            UE_LOG(LogEOS, Warning, TEXT("UEOSSubsystem::GetSubsystem: Game instance is not valid."));
            return nullptr;
        }
    }
    else
    {
        UE_LOG(LogEOS, Warning, TEXT("UEOSSubsystem::GetSubsystem: Current world is not valid."));
        return nullptr;
    }
}