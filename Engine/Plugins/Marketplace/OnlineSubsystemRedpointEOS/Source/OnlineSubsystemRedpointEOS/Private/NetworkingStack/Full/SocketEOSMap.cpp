// Copyright June Rhodes. All Rights Reserved.

#include "SocketEOSMap.h"

EOS_ENABLE_STRICT_WARNINGS

FString FSocketEOSKey::ToString(bool bExcludeChannel) const
{
    FString LocalUserIdString, RemoteUserIdString;
    EOS_EResult LocalIdToStringResult = EOSString_ProductUserId::ToString(this->LocalUserId, LocalUserIdString);
    EOS_EResult RemoteIdToStringResult = EOSString_ProductUserId::ToString(this->RemoteUserId, RemoteUserIdString);
    if (LocalIdToStringResult != EOS_EResult::EOS_Success || RemoteIdToStringResult != EOS_EResult::EOS_Success)
    {
        return TEXT("");
    }

    if (bExcludeChannel)
    {
        return FString::Printf(
            TEXT("%s:%s:%s"),
            *LocalUserIdString,
            *RemoteUserIdString,
            *EOSString_P2P_SocketName::FromAnsiString(this->SymmetricSocketId.SocketName));
    }

    return FString::Printf(
        TEXT("%s:%s:%s:%u"),
        *LocalUserIdString,
        *RemoteUserIdString,
        *EOSString_P2P_SocketName::FromAnsiString(this->SymmetricSocketId.SocketName),
        this->SymmetricChannel);
}

bool FSocketEOSKey::IsIdenticalExcludingChannel(const FSocketEOSKey &Other) const
{
    return this->ToString(true) == Other.ToString(true);
}

EOS_DISABLE_STRICT_WARNINGS