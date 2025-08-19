// Copyright June Rhodes. All Rights Reserved.

#include "EOSRuntimePlatformIntegrationOculus.h"

#if EOS_OCULUS_ENABLED && EOS_VERSION_AT_LEAST(1, 10, 3)

#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemUtils.h"

TSharedPtr<const class FUniqueNetId> FEOSRuntimePlatformIntegrationOculus::GetUserId(
    TSoftObjectPtr<class UWorld> InWorld,
    EOS_Connect_ExternalAccountInfo *InExternalInfo)
{
    if (!InWorld.IsValid() || InExternalInfo == nullptr)
    {
        return nullptr;
    }

    if (InExternalInfo->AccountIdType != EOS_EExternalAccountType::EOS_EAT_OCULUS)
    {
        return nullptr;
    }

    IOnlineSubsystem *OSS = Online::GetSubsystem(InWorld.Get(), OCULUS_SUBSYSTEM);
    if (OSS != nullptr)
    {
        IOnlineIdentityPtr Identity = OSS->GetIdentityInterface();
        if (Identity.IsValid())
        {
            FString OculusId = EOSString_Connect_ExternalAccountId::FromAnsiString(InExternalInfo->AccountId);
            return Identity->CreateUniquePlayerId(OculusId);
        }
    }

    return nullptr;
}

bool FEOSRuntimePlatformIntegrationOculus::CanProvideExternalAccountId(const FUniqueNetId &InUserId)
{
    return InUserId.GetType().IsEqual(OCULUS_SUBSYSTEM);
}

FExternalAccountIdInfo FEOSRuntimePlatformIntegrationOculus::GetExternalAccountId(const FUniqueNetId &InUserId)
{
    if (!InUserId.GetType().IsEqual(OCULUS_SUBSYSTEM))
    {
        return FExternalAccountIdInfo();
    }

    FExternalAccountIdInfo Info;
    Info.AccountIdType = EOS_EExternalAccountType::EOS_EAT_OCULUS;
    Info.AccountId = InUserId.ToString();
    return Info;
}

void FEOSRuntimePlatformIntegrationOculus::MutateFriendsListNameIfRequired(
    FName InSubsystemName,
    FString &InOutListName) const
{
    if (InSubsystemName.IsEqual(OCULUS_SUBSYSTEM))
    {
        InOutListName = TEXT("default");
    }
}

#endif