// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointSteam.h"

#include "LogRedpointSteam.h"
#include "Misc/ConfigCacheIni.h"
#include "OnlineAvatarInterfaceRedpointSteam.h"
#include "OnlineEntitlementsInterfaceRedpointSteam.h"
#include "OnlinePurchaseInterfaceRedpointSteam.h"
#include "OnlineStoreInterfaceV2RedpointSteam.h"

#if EOS_STEAM_ENABLED

FOnlineAsyncTaskManager *FOnlineSubsystemSteamProtectedAccessor::GetAsyncTaskRunner(FOnlineSubsystemSteam *Steam)
{
    return (
        FOnlineAsyncTaskManager *)(((FOnlineSubsystemSteamProtectedAccessor *)Steam)->OnlineAsyncTaskThreadRunnable);
}

FOnlineSubsystemRedpointSteam::FOnlineSubsystemRedpointSteam(FName InInstanceName)
    : FOnlineSubsystemImplBase(REDPOINT_STEAM_SUBSYSTEM, InInstanceName)
{
}

FOnlineSubsystemRedpointSteam::~FOnlineSubsystemRedpointSteam()
{
}

bool FOnlineSubsystemRedpointSteam::IsEnabled() const
{
    return true;
}

IOnlineEntitlementsPtr FOnlineSubsystemRedpointSteam::GetEntitlementsInterface() const
{
    return this->EntitlementsImpl;
}

IOnlineStoreV2Ptr FOnlineSubsystemRedpointSteam::GetStoreV2Interface() const
{
    return this->StoreV2Impl;
}

IOnlinePurchasePtr FOnlineSubsystemRedpointSteam::GetPurchaseInterface() const
{
    return this->PurchaseImpl;
}

class UObject *FOnlineSubsystemRedpointSteam::GetNamedInterface(FName InterfaceName)
{
    if (InterfaceName.IsEqual(ONLINE_AVATAR_INTERFACE_NAME))
    {
        return (class UObject *)(void *)&this->AvatarImpl;
    }

    return nullptr;
}

bool FOnlineSubsystemRedpointSteam::Init()
{
    this->AvatarImpl = MakeShared<FOnlineAvatarInterfaceRedpointSteam, ESPMode::ThreadSafe>();

    this->EntitlementsImpl = MakeShared<FOnlineEntitlementsInterfaceRedpointSteam, ESPMode::ThreadSafe>();
    this->StoreV2Impl = MakeShared<FOnlineStoreInterfaceV2RedpointSteam, ESPMode::ThreadSafe>();
    this->PurchaseImpl = MakeShared<FOnlinePurchaseInterfaceRedpointSteam, ESPMode::ThreadSafe>();

    GConfig->GetString(TEXT("OnlineSubsystemSteam"), TEXT("WebApiKey"), WebApiKey, GEngineIni);

    return true;
}

template <typename T, ESPMode Mode> void DestructInterface(TSharedPtr<T, Mode> &Ref, const TCHAR *Name)
{
    if (Ref.IsValid())
    {
        ensureMsgf(
            Ref.IsUnique(),
            TEXT(
                "Interface is not unique during shutdown of Steam subsystem: %s. "
                "This indicates you have a TSharedPtr<> or IOnline... in your code that is holding a reference open to "
                "the interface longer than the lifetime of the online subsystem. You should use TWeakPtr<> "
                "to hold references to interfaces in class fields to prevent lifetime issues"),
            Name);
        Ref = nullptr;
    }
}

bool FOnlineSubsystemRedpointSteam::Shutdown()
{
    DestructInterface(this->PurchaseImpl, TEXT("IOnlinePurchase"));
    DestructInterface(this->StoreV2Impl, TEXT("IOnlineStoreV2"));
    DestructInterface(this->EntitlementsImpl, TEXT("IOnlineEntitlements"));
    DestructInterface(this->AvatarImpl, TEXT("IOnlineAvatar"));

    return true;
}

FString FOnlineSubsystemRedpointSteam::GetAppId() const
{
    return TEXT("");
}

FText FOnlineSubsystemRedpointSteam::GetOnlineServiceName() const
{
    return FText::GetEmpty();
}

FString FOnlineSubsystemRedpointSteam::GetWebApiKey() const
{
    return WebApiKey;
}

#endif
