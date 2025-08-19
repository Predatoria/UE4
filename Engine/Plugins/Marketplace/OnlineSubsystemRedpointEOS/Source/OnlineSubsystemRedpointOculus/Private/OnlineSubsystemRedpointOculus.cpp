// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointOculus.h"

#if EOS_OCULUS_ENABLED

#include "LogRedpointOculus.h"
#include "OnlineAvatarInterfaceRedpointOculus.h"

FOnlineSubsystemRedpointOculus::FOnlineSubsystemRedpointOculus(FName InInstanceName)
    : FOnlineSubsystemImplBase(REDPOINT_OCULUS_SUBSYSTEM, InInstanceName)
{
}

FOnlineSubsystemRedpointOculus::~FOnlineSubsystemRedpointOculus()
{
}

bool FOnlineSubsystemRedpointOculus::IsEnabled() const
{
    return true;
}

class UObject *FOnlineSubsystemRedpointOculus::GetNamedInterface(FName InterfaceName)
{
    if (InterfaceName.IsEqual(ONLINE_AVATAR_INTERFACE_NAME))
    {
        return (class UObject *)(void *)&this->AvatarImpl;
    }

    return nullptr;
}

bool FOnlineSubsystemRedpointOculus::Init()
{
    this->AvatarImpl = MakeShared<FOnlineAvatarInterfaceRedpointOculus, ESPMode::ThreadSafe>();

    return true;
}

template <typename T, ESPMode Mode> void DestructInterface(TSharedPtr<T, Mode> &Ref, const TCHAR *Name)
{
    if (Ref.IsValid())
    {
        ensureMsgf(
            Ref.IsUnique(),
            TEXT(
                "Interface is not unique during shutdown of Oculus subsystem: %s. "
                "This indicates you have a TSharedPtr<> or IOnline... in your code that is holding a reference open to "
                "the interface longer than the lifetime of the online subsystem. You should use TWeakPtr<> "
                "to hold references to interfaces in class fields to prevent lifetime issues"),
            Name);
        Ref = nullptr;
    }
}

bool FOnlineSubsystemRedpointOculus::Shutdown()
{
    DestructInterface(this->AvatarImpl, TEXT("IOnlineAvatar"));

    return true;
}

FString FOnlineSubsystemRedpointOculus::GetAppId() const
{
    return TEXT("");
}

FText FOnlineSubsystemRedpointOculus::GetOnlineServiceName() const
{
    return FText::GetEmpty();
}

#endif