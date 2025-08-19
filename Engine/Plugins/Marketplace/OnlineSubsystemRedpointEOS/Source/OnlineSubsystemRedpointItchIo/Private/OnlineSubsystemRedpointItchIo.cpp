// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointItchIo.h"

#include "LogRedpointItchIo.h"
#include "OnlineAvatarInterfaceRedpointItchIo.h"
#include "OnlineIdentityInterfaceRedpointItchIo.h"

#if EOS_ITCH_IO_ENABLED

FOnlineSubsystemRedpointItchIo::FOnlineSubsystemRedpointItchIo(FName InInstanceName)
    : FOnlineSubsystemImplBase(REDPOINT_ITCH_IO_SUBSYSTEM, InInstanceName)
    , IdentityImpl(nullptr)
    , AvatarImpl(nullptr)
{
}

FOnlineSubsystemRedpointItchIo::~FOnlineSubsystemRedpointItchIo()
{
}

bool FOnlineSubsystemRedpointItchIo::IsEnabled() const
{
    return true;
}

IOnlineIdentityPtr FOnlineSubsystemRedpointItchIo::GetIdentityInterface() const
{
    return this->IdentityImpl;
}

class UObject *FOnlineSubsystemRedpointItchIo::GetNamedInterface(FName InterfaceName)
{
    if (InterfaceName.IsEqual(ONLINE_AVATAR_INTERFACE_NAME))
    {
        return (class UObject *)(void *)&this->AvatarImpl;
    }

    return nullptr;
}

bool FOnlineSubsystemRedpointItchIo::Init()
{
    this->IdentityImpl = MakeShared<FOnlineIdentityInterfaceRedpointItchIo, ESPMode::ThreadSafe>();
    this->AvatarImpl =
        MakeShared<FOnlineAvatarInterfaceRedpointItchIo, ESPMode::ThreadSafe>(this->IdentityImpl.ToSharedRef());

    return true;
}

bool FOnlineSubsystemRedpointItchIo::Tick(float DeltaTime)
{
    return FOnlineSubsystemImpl::Tick(DeltaTime);
}

template <typename T, ESPMode Mode> void DestructInterface(TSharedPtr<T, Mode> &Ref, const TCHAR *Name)
{
    if (Ref.IsValid())
    {
        ensureMsgf(
            Ref.IsUnique(),
            TEXT(
                "Interface is not unique during shutdown of itch.io subsystem: %s. "
                "This indicates you have a TSharedPtr<> or IOnline... in your code that is holding a reference open to "
                "the interface longer than the lifetime of the online subsystem. You should use TWeakPtr<> "
                "to hold references to interfaces in class fields to prevent lifetime issues"),
            Name);
        Ref = nullptr;
    }
}

bool FOnlineSubsystemRedpointItchIo::Shutdown()
{
    DestructInterface(this->AvatarImpl, TEXT("IOnlineAvatar"));
    DestructInterface(this->IdentityImpl, TEXT("IOnlineIdentity"));

    return true;
}

FString FOnlineSubsystemRedpointItchIo::GetAppId() const
{
    return TEXT("");
}

FText FOnlineSubsystemRedpointItchIo::GetOnlineServiceName() const
{
    return FText::GetEmpty();
}

#endif