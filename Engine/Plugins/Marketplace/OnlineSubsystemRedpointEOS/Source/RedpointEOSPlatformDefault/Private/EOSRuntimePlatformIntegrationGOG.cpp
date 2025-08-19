// Copyright June Rhodes. All Rights Reserved.

#if EOS_GOG_ENABLED

#include "EOSRuntimePlatformIntegrationGOG.h"

#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemUtils.h"

#define GOG_SUBSYSTEM FName(TEXT("GOG"))

TSharedPtr<const class FUniqueNetId> FEOSRuntimePlatformIntegrationGOG::GetUserId(
    TSoftObjectPtr<class UWorld> InWorld,
    EOS_Connect_ExternalAccountInfo *InExternalInfo)
{
    if (!InWorld.IsValid() || InExternalInfo == nullptr)
    {
        return nullptr;
    }

    if (InExternalInfo->AccountIdType != EOS_EExternalAccountType::EOS_EAT_GOG)
    {
        return nullptr;
    }

    IOnlineSubsystem *OSS = Online::GetSubsystem(InWorld.Get(), GOG_SUBSYSTEM);
    if (OSS != nullptr)
    {
        IOnlineIdentityPtr Identity = OSS->GetIdentityInterface();
        if (Identity.IsValid())
        {
            FString SteamId = EOSString_Connect_ExternalAccountId::FromAnsiString(InExternalInfo->AccountId);
            return Identity->CreateUniquePlayerId(SteamId);
        }
    }

    return nullptr;
}

bool FEOSRuntimePlatformIntegrationGOG::CanProvideExternalAccountId(const FUniqueNetId &InUserId)
{
    return InUserId.GetType().IsEqual(GOG_SUBSYSTEM);
}

FExternalAccountIdInfo FEOSRuntimePlatformIntegrationGOG::GetExternalAccountId(const FUniqueNetId &InUserId)
{
    if (!InUserId.GetType().IsEqual(GOG_SUBSYSTEM))
    {
        return FExternalAccountIdInfo();
    }

    FExternalAccountIdInfo Info;
    Info.AccountIdType = EOS_EExternalAccountType::EOS_EAT_GOG;
    Info.AccountId = InUserId.ToString();
    return Info;
}

#endif