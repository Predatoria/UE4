// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/AuthVerificationPhaseContext.h"

#include "OnlineSubsystemRedpointEOS/Private/EOSControlChannel.h"

FString FAuthVerificationPhaseContext::GetIdentifier() const
{
    return *this->GetUserId()->ToString();
}

FString FAuthVerificationPhaseContext::GetPhaseGroup() const
{
    return TEXT("verification");
}

void FAuthVerificationPhaseContext::SetVerificationStatus(EUserVerificationStatus InStatus)
{
    GetControlChannel()->VerificationDatabase.Add(*this->GetUserId(), InStatus);
}

EUserVerificationStatus FAuthVerificationPhaseContext::GetVerificationStatus() const
{
    if (!GetControlChannel()->VerificationDatabase.Contains(*this->GetUserId()))
    {
        return EUserVerificationStatus::NotStarted;
    }

    return GetControlChannel()->VerificationDatabase[*this->GetUserId()];
}

bool FAuthVerificationPhaseContext::IsConnectionAsTrustedOnClient() const
{
    return GetControlChannel()->bClientTrustsServer;
}
