// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthLoginPhaseContext.h"

#include "OnlineSubsystemRedpointEOS/Private/EOSControlChannel.h"

FString FAuthLoginPhaseContext::GetIdentifier() const
{
    return *this->GetUserId()->ToString();
}

FString FAuthLoginPhaseContext::GetPhaseGroup() const
{
    return TEXT("login");
}

void FAuthLoginPhaseContext::MarkAsRegisteredForAntiCheat()
{
    GetControlChannel()->bRegisteredForAntiCheat.Add(*this->GetUserId(), true);
}

void FAuthLoginPhaseContext::SetVerificationStatus(EUserVerificationStatus InStatus)
{
    GetControlChannel()->VerificationDatabase.Add(*this->GetUserId(), InStatus);
}

EUserVerificationStatus FAuthLoginPhaseContext::GetVerificationStatus() const
{
    if (!GetControlChannel()->VerificationDatabase.Contains(*this->GetUserId()))
    {
        return EUserVerificationStatus::NotStarted;
    }

    return GetControlChannel()->VerificationDatabase[*this->GetUserId()];
}