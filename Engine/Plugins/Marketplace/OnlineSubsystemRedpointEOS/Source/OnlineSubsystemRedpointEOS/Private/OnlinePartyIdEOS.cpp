// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/OnlinePartyIdEOS.h"

#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"

EOS_ENABLE_STRICT_WARNINGS

FOnlinePartyIdEOS::FOnlinePartyIdEOS(EOS_LobbyId InId)
    : AnsiData(nullptr)
    , AnsiDataLen(0)
{
    verify(
        EOSString_LobbyId::AllocateToCharBuffer(InId, this->AnsiData, this->AnsiDataLen) == EOS_EResult::EOS_Success);
}

FOnlinePartyIdEOS::~FOnlinePartyIdEOS()
{
    EOSString_LobbyId::FreeFromCharBuffer(this->AnsiData);
}

const uint8 *FOnlinePartyIdEOS::GetBytes() const
{
    return (const uint8 *)this->AnsiData;
}

int32 FOnlinePartyIdEOS::GetSize() const
{
    return this->AnsiDataLen;
}

bool FOnlinePartyIdEOS::IsValid() const
{
    return EOSString_LobbyId::IsValid(this->AnsiData);
}

FString FOnlinePartyIdEOS::ToString() const
{
    return EOSString_LobbyId::ToString(this->AnsiData);
}

FString FOnlinePartyIdEOS::ToDebugString() const
{
    return EOSString_LobbyId::ToString(this->AnsiData);
}

EOS_LobbyId FOnlinePartyIdEOS::GetLobbyId() const
{
    return (EOS_LobbyId)this->AnsiData;
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION