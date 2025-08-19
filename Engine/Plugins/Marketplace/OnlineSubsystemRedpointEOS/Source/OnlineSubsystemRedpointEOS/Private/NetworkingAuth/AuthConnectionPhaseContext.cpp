// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthConnectionPhaseContext.h"

#include "OnlineSubsystemRedpointEOS/Private/EOSControlChannel.h"

FString FAuthConnectionPhaseContext::GetIdentifier() const
{
    return this->GetConnection()->LowLevelGetRemoteAddress();
}

FString FAuthConnectionPhaseContext::GetPhaseGroup() const
{
    return TEXT("connection");
}

void FAuthConnectionPhaseContext::MarkConnectionAsTrustedOnClient()
{
    GetControlChannel()->bClientTrustsServer = true;
}