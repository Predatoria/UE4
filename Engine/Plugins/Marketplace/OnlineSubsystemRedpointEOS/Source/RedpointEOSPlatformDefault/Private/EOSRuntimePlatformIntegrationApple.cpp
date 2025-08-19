// Copyright June Rhodes. All Rights Reserved.

#if EOS_APPLE_ENABLED

#include "EOSRuntimePlatformIntegrationApple.h"

#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemUtils.h"

TSharedPtr<const class FUniqueNetId> FEOSRuntimePlatformIntegrationApple::GetUserId(
    TSoftObjectPtr<class UWorld> InWorld,
    EOS_Connect_ExternalAccountInfo *InExternalInfo)
{
    if (!InWorld.IsValid() || InExternalInfo == nullptr)
    {
        return nullptr;
    }

    if (InExternalInfo->AccountIdType != EOS_EExternalAccountType::EOS_EAT_APPLE)
    {
        return nullptr;
    }

    IOnlineSubsystem *OSS = Online::GetSubsystem(InWorld.Get(), FName(TEXT("APPLE")));
    if (OSS != nullptr)
    {
        IOnlineIdentityPtr Identity = OSS->GetIdentityInterface();
        if (Identity.IsValid())
        {
            FString AppleId = EOSString_Connect_ExternalAccountId::FromAnsiString(InExternalInfo->AccountId);
            return Identity->CreateUniquePlayerId(AppleId);
        }
    }

    return nullptr;
}

bool FEOSRuntimePlatformIntegrationApple::CanProvideExternalAccountId(const FUniqueNetId &InUserId)
{
    return InUserId.GetType().IsEqual(FName(TEXT("APPLE")));
}

FExternalAccountIdInfo FEOSRuntimePlatformIntegrationApple::GetExternalAccountId(const FUniqueNetId &InUserId)
{
    if (!InUserId.GetType().IsEqual(FName(TEXT("APPLE"))))
    {
        return FExternalAccountIdInfo();
    }

    FExternalAccountIdInfo Info;
    Info.AccountIdType = EOS_EExternalAccountType::EOS_EAT_APPLE;
    Info.AccountId = InUserId.ToString();
    return Info;
}

#endif