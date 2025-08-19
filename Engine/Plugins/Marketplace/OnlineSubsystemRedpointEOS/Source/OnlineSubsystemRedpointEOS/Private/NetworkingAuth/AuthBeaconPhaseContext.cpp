// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthBeaconPhaseContext.h"

FString FAuthBeaconPhaseContext::GetIdentifier() const
{
    return FString::Printf(TEXT("%s/%s"), *this->GetUserId()->ToString(), *this->BeaconName);
}

FString FAuthBeaconPhaseContext::GetPhaseGroup() const
{
    return TEXT("beacon");
}
