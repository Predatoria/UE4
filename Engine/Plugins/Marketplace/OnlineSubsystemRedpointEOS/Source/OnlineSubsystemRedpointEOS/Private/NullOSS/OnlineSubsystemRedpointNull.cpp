// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Private/NullOSS/OnlineSubsystemRedpointNull.h"

#include "OnlineSubsystemRedpointEOS/Private/NullOSS/OnlineIdentityInterfaceNull.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

EOS_ENABLE_STRICT_WARNINGS

FOnlineSubsystemRedpointNull::FOnlineSubsystemRedpointNull(FName InInstanceName)
    : FOnlineSubsystemImplBase(REDPOINT_EOS_SUBSYSTEM, InInstanceName)
    , IdentityImpl(MakeShared<FOnlineIdentityInterfaceNull, ESPMode::ThreadSafe>())
{
}

IOnlineIdentityPtr FOnlineSubsystemRedpointNull::GetIdentityInterface() const
{
    return this->IdentityImpl;
}

bool FOnlineSubsystemRedpointNull::Init()
{
    return true;
}

bool FOnlineSubsystemRedpointNull::Shutdown()
{
    return true;
}

FString FOnlineSubsystemRedpointNull::GetAppId() const
{
    return TEXT("");
}

FText FOnlineSubsystemRedpointNull::GetOnlineServiceName(void) const
{
    return NSLOCTEXT("EOS", "EOSNullPlatformName", "Epic Online Services (Null)");
}

EOS_DISABLE_STRICT_WARNINGS