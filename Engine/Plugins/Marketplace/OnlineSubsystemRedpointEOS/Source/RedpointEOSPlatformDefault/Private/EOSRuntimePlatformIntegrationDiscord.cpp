// Copyright June Rhodes. All Rights Reserved.

#if EOS_DISCORD_ENABLED

#include "EOSRuntimePlatformIntegrationDiscord.h"

#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointDiscordConstants.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemUtils.h"

TSharedPtr<const class FUniqueNetId> FEOSRuntimePlatformIntegrationDiscord::GetUserId(
    TSoftObjectPtr<class UWorld> InWorld,
    EOS_Connect_ExternalAccountInfo *InExternalInfo)
{
    if (!InWorld.IsValid() || InExternalInfo == nullptr)
    {
        return nullptr;
    }

    if (InExternalInfo->AccountIdType != EOS_EExternalAccountType::EOS_EAT_DISCORD)
    {
        return nullptr;
    }

    IOnlineSubsystem *OSS = Online::GetSubsystem(InWorld.Get(), REDPOINT_DISCORD_SUBSYSTEM);
    if (OSS != nullptr)
    {
        IOnlineIdentityPtr Identity = OSS->GetIdentityInterface();
        if (Identity.IsValid())
        {
            FString DiscordId = EOSString_Connect_ExternalAccountId::FromAnsiString(InExternalInfo->AccountId);
            return Identity->CreateUniquePlayerId(DiscordId);
        }
    }

    return nullptr;
}

bool FEOSRuntimePlatformIntegrationDiscord::CanProvideExternalAccountId(const FUniqueNetId &InUserId)
{
    return InUserId.GetType().IsEqual(REDPOINT_DISCORD_SUBSYSTEM);
}

FExternalAccountIdInfo FEOSRuntimePlatformIntegrationDiscord::GetExternalAccountId(const FUniqueNetId &InUserId)
{
    if (!InUserId.GetType().IsEqual(REDPOINT_DISCORD_SUBSYSTEM))
    {
        return FExternalAccountIdInfo();
    }

    FExternalAccountIdInfo Info;
    Info.AccountIdType = EOS_EExternalAccountType::EOS_EAT_DISCORD;
    Info.AccountId = InUserId.ToString();
    return Info;
}

#endif