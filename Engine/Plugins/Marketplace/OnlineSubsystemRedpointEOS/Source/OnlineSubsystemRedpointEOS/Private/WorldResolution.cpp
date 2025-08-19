// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/WorldResolution.h"

#include "Engine/Engine.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

TSoftObjectPtr<UWorld> FWorldResolution::GetWorld(const FName &InInstanceName, bool bAllowFailure)
{
    if (GEngine == nullptr)
    {
        if (!bAllowFailure)
        {
            UE_LOG(
                LogEOS,
                Error,
                TEXT("FWorldResolution::GetWorld: Called before the engine has initialized the first world."));
        }

        return nullptr;
    }

    TSoftObjectPtr<UWorld> WorldPtr = nullptr;
    if (InInstanceName.ToString() == TEXT("DefaultInstance"))
    {
        WorldPtr = GEngine->GetWorld();
        if (WorldPtr == nullptr)
        {
            // Okay, try this is a backup... seems to only be a thing outside of the editor.
            for (const auto &WorldContext : GEngine->GetWorldContexts())
            {
                WorldPtr = WorldContext.World();
                if (WorldPtr != nullptr)
                {
                    break;
                }
            }
        }
    }
    else
    {
        FWorldContext *WorldContext = GEngine->GetWorldContextFromHandle(InInstanceName);
        if (WorldContext == nullptr)
        {
            if (!bAllowFailure)
            {
                UE_LOG(LogEOS, Error, TEXT("FWorldResolution::GetWorld: The world context handle is invalid."));
            }
            return nullptr;
        }

        WorldPtr = WorldContext->World();
    }

    if (!WorldPtr.IsValid())
    {
        if (!bAllowFailure)
        {
            if (InInstanceName.ToString() == TEXT("DefaultInstance"))
            {
                UE_LOG(
                    LogEOS,
                    Error,
                    TEXT("FWorldResolution::GetWorld: There is no active world."),
                    *InInstanceName.ToString());
            }
            else
            {
                UE_LOG(
                    LogEOS,
                    Error,
                    TEXT("FWorldResolution::GetWorld: There is no active world for world context '%s'."),
                    *InInstanceName.ToString());
            }
        }

        return nullptr;
    }

    return WorldPtr;
}