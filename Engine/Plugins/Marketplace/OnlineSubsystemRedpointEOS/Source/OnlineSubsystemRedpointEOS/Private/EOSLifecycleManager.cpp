// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/EOSLifecycleManager.h"

#include "OnlineSubsystemRedpointEOS/Shared/EOSRuntimePlatform.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"

EOS_ENABLE_STRICT_WARNINGS

#if EOS_VERSION_AT_LEAST(1, 15, 0)

// NOTE: We do not update lifecycle statuses for GetActAsPlatformInstance() because this is only
// used on dedicated servers and only used during the legacy credential authentication that will
// eventually be removed.

bool FEOSLifecycleManager::Tick(float DeltaSeconds)
{
    if (this->bShouldPollForApplicationStatus && this->bShouldPollForNetworkStatus)
    {
        EOS_EApplicationStatus NewApplicationStatus = EOS_EApplicationStatus::EOS_AS_Foreground;
        EOS_ENetworkStatus NewNetworkStatus = EOS_ENetworkStatus::EOS_NS_Online;
        this->RuntimePlatform->PollLifecycleApplicationStatus(NewApplicationStatus);
        this->RuntimePlatform->PollLifecycleNetworkStatus(NewNetworkStatus);
        for (const auto &Instance : this->Owner->SubsystemInstances)
        {
            EOS_HPlatform Platform = Instance.Value->GetPlatformInstance();
            EOS_EApplicationStatus OldApplicationStatus = EOS_Platform_GetApplicationStatus(Platform);
            if (OldApplicationStatus != NewApplicationStatus)
            {
                if (this->Owner->SubsystemInstancesTickedFlag.Contains(Instance.Key))
                {
                    UE_LOG(
                        LogEOS,
                        Verbose,
                        TEXT("FEOSLifecycleManager::Tick: Platform instance %p application status moving from [%d] to "
                             "[%d]"),
                        Platform,
                        OldApplicationStatus,
                        NewApplicationStatus);
                    EOS_Platform_SetApplicationStatus(Platform, NewApplicationStatus);
                }
                else
                {
                    // @todo: Do we need to queue these changes up and then apply them after the first tick? If we do
                    // we probably also need to change how platforms call EOS_Platform_SetApplicationStatus directly
                    // on startup so that those calls can also be queued until after the first tick.
                    UE_LOG(
                        LogEOS,
                        Warning,
                        TEXT("FEOSLifecycleManager::Tick: Platform instance %p is not updating application "
                             "status because "
                             "it has not ticked yet."),
                        Platform);
                }
            }
            if (EOS_Platform_GetNetworkStatus(Platform) != NewNetworkStatus)
            {
                EOS_Platform_SetNetworkStatus(Platform, NewNetworkStatus);
            }
        }
    }
    else if (this->bShouldPollForApplicationStatus)
    {
        EOS_EApplicationStatus NewApplicationStatus = EOS_EApplicationStatus::EOS_AS_Foreground;
        this->RuntimePlatform->PollLifecycleApplicationStatus(NewApplicationStatus);
        for (const auto &Instance : this->Owner->SubsystemInstances)
        {
            EOS_HPlatform Platform = Instance.Value->GetPlatformInstance();
            EOS_EApplicationStatus OldApplicationStatus = EOS_Platform_GetApplicationStatus(Platform);
            if (OldApplicationStatus != NewApplicationStatus)
            {
                if (this->Owner->SubsystemInstancesTickedFlag.Contains(Instance.Key))
                {
                    UE_LOG(
                        LogEOS,
                        Verbose,
                        TEXT("FEOSLifecycleManager::Tick: Platform instance %p application status moving from [%d] to "
                             "[%d]"),
                        Platform,
                        OldApplicationStatus,
                        NewApplicationStatus);
                    EOS_Platform_SetApplicationStatus(Platform, NewApplicationStatus);
                }
                else
                {
                    // @todo: Do we need to queue these changes up and then apply them after the first tick? If we do
                    // we probably also need to change how platforms call EOS_Platform_SetApplicationStatus directly
                    // on startup so that those calls can also be queued until after the first tick.
                    UE_LOG(
                        LogEOS,
                        Warning,
                        TEXT("FEOSLifecycleManager::Tick: Platform instance %p is not updating application "
                             "status because "
                             "it has not ticked yet."),
                        Platform);
                }
            }
        }
    }
    else if (this->bShouldPollForNetworkStatus)
    {
        EOS_ENetworkStatus NewNetworkStatus = EOS_ENetworkStatus::EOS_NS_Online;
        this->RuntimePlatform->PollLifecycleNetworkStatus(NewNetworkStatus);
        for (const auto &Instance : this->Owner->SubsystemInstances)
        {
            EOS_HPlatform Platform = Instance.Value->GetPlatformInstance();
            if (EOS_Platform_GetNetworkStatus(Platform) != NewNetworkStatus)
            {
                EOS_Platform_SetNetworkStatus(Platform, NewNetworkStatus);
            }
        }
    }
    return true;
}

FEOSLifecycleManager::FEOSLifecycleManager(
    FOnlineSubsystemRedpointEOSModule *InOwner,
    const TSharedRef<class IEOSRuntimePlatform> &InRuntimePlatform)
    : Owner(InOwner)
    , RuntimePlatform(InRuntimePlatform)
    , TickerHandle()
    , bShouldPollForApplicationStatus(InRuntimePlatform->ShouldPollLifecycleApplicationStatus())
    , bShouldPollForNetworkStatus(InRuntimePlatform->ShouldPollLifecycleNetworkStatus())
{
}

void FEOSLifecycleManager::Init()
{
    this->RuntimePlatform->RegisterLifecycleHandlers(this->AsShared());
    if (this->bShouldPollForApplicationStatus || this->bShouldPollForNetworkStatus)
    {
        this->TickerHandle =
            FUTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateSP(this, &FEOSLifecycleManager::Tick), 0.0f);
    }
}

void FEOSLifecycleManager::Shutdown()
{
    if (this->bShouldPollForApplicationStatus || this->bShouldPollForNetworkStatus)
    {
        FUTicker::GetCoreTicker().RemoveTicker(this->TickerHandle);
    }
}

void FEOSLifecycleManager::UpdateApplicationStatus(const EOS_EApplicationStatus &InNewStatus)
{
    for (const auto &Instance : this->Owner->SubsystemInstances)
    {
        EOS_HPlatform Platform = Instance.Value->GetPlatformInstance();
        EOS_EApplicationStatus OldApplicationStatus = EOS_Platform_GetApplicationStatus(Platform);
        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("FEOSLifecycleManager::UpdateApplicationStatus: Platform instance %p application status moving from [%d] to [%d]"),
            Platform,
            OldApplicationStatus,
            InNewStatus);
        EOS_Platform_SetApplicationStatus(Platform, InNewStatus);
    }
}

void FEOSLifecycleManager::UpdateNetworkStatus(const EOS_ENetworkStatus &InNewStatus)
{
    for (const auto &Instance : this->Owner->SubsystemInstances)
    {
        EOS_Platform_SetNetworkStatus(Instance.Value->GetPlatformInstance(), InNewStatus);
    }
}

#endif

EOS_DISABLE_STRICT_WARNINGS