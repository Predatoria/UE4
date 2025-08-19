// Copyright June Rhodes. All Rights Reserved.

#include "./InternetAddrEOSFull.h"

#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"

EOS_ENABLE_STRICT_WARNINGS

FInternetAddrEOSFull::FInternetAddrEOSFull(
    EOS_ProductUserId InUserId,
    const FString &InSymmetricSocketName,
    EOS_CHANNEL_ID_TYPE InSymmetricChannel)
    : UserId(InUserId)
    , SymmetricSocketName(InSymmetricSocketName)
    , SymmetricChannel(InSymmetricChannel)
{
    check(EOSString_ProductUserId::IsValid(this->UserId));
    check(this->SymmetricSocketName.Len() > 0 && this->SymmetricSocketName.Len() <= EOS_P2P_SOCKET_NAME_MAX_LENGTH);
}

FInternetAddrEOSFull::FInternetAddrEOSFull()
    : UserId(nullptr)
    , SymmetricSocketName(TEXT(""))
    , SymmetricChannel(0)
{
}

void FInternetAddrEOSFull::SetFromParameters(
    EOS_ProductUserId InUserId,
    const FString &InSymmetricSocketName,
    EOS_CHANNEL_ID_TYPE InSymmetricChannel)
{
    check(EOSString_ProductUserId::IsValid(InUserId));
    check(InSymmetricSocketName.Len() > 0 && InSymmetricSocketName.Len() <= EOS_P2P_SOCKET_NAME_MAX_LENGTH);
    this->UserId = InUserId;
    this->SymmetricSocketName = InSymmetricSocketName;
    this->SymmetricChannel = InSymmetricChannel;
}

EOS_ProductUserId FInternetAddrEOSFull::GetUserId() const
{
    return this->UserId;
}

FString FInternetAddrEOSFull::GetSymmetricSocketName() const
{
    return this->SymmetricSocketName;
}

EOS_P2P_SocketId FInternetAddrEOSFull::GetSymmetricSocketId() const
{
    EOS_P2P_SocketId Socket = {};
    Socket.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;
    verify(
        EOSString_P2P_SocketName::CopyToExistingBuffer<33>(this->SymmetricSocketName, Socket.SocketName) ==
        EOS_EResult::EOS_Success);
    return Socket;
}

void FInternetAddrEOSFull::SetIp(uint32 InAddr)
{
    UE_LOG(LogEOS, Error, TEXT("FInternetAddrEOSFull::SetIp is not supported."));
}

void FInternetAddrEOSFull::SetIp(const TCHAR *InAddr, bool &bIsValid)
{
    FString AddrAndPort = InAddr;

    int32 PortIdx;
    EOS_CHANNEL_ID_TYPE ParsedChannel;
    FString Addr;
    if (AddrAndPort.FindLastChar(TEXT(':'), PortIdx))
    {
        Addr = AddrAndPort.Left(PortIdx);
        ParsedChannel = FCString::Atoi(*AddrAndPort.Mid(PortIdx + 1)) % EOS_CHANNEL_ID_MODULO;
    }
    else
    {
        Addr = AddrAndPort;
        ParsedChannel = 0;
    }

    if (!Addr.EndsWith(FString::Printf(TEXT(".%s"), INTERNET_ADDR_EOS_P2P_DOMAIN_SUFFIX)))
    {
        bIsValid = false;
        UE_LOG(LogEOS, Error, TEXT("FInternetAddrEOSFull::SetIp does not support plain IP addresses yet."));
        return;
    }

    TArray<FString> AddressComponents;
    Addr.ParseIntoArray(AddressComponents, TEXT("."), true);
    if (AddressComponents.Num() != 3)
    {
        bIsValid = false;
        UE_LOG(LogEOS, Error, TEXT("FInternetAddrEOSFull::SetIp received an incomplete address: '%s'"), *Addr);
        return;
    }

    EOS_ProductUserId ResultProductUserId = {};
    if (EOSString_ProductUserId::FromString(AddressComponents[0], ResultProductUserId) != EOS_EResult::EOS_Success)
    {
        bIsValid = false;
        UE_LOG(
            LogEOS,
            Error,
            TEXT("FInternetAddrEOSFull::SetIp did not have a valid product user ID. If you want to create an address "
                 "for binding locally, use the ID of the user that will be listening."));
        return;
    }

    FString ResultSocketName = AddressComponents[1];
    if (ResultSocketName.IsEmpty())
    {
        bIsValid = false;
        UE_LOG(LogEOS, Error, TEXT("FInternetAddrEOSFull::SetIp did not have a valid socket name."));
        return;
    }

    if (AddressComponents[2] != INTERNET_ADDR_EOS_P2P_DOMAIN_SUFFIX)
    {
        bIsValid = false;
        UE_LOG(
            LogEOS,
            Error,
            TEXT("FInternetAddrEOSFull::SetIp was missing '.%s' domain suffix."),
            INTERNET_ADDR_EOS_P2P_DOMAIN_SUFFIX);
        return;
    }

    this->UserId = ResultProductUserId;
    this->SymmetricSocketName = ResultSocketName;
    this->SymmetricChannel = ParsedChannel;
    bIsValid = true;
}

void FInternetAddrEOSFull::GetIp(uint32 &OutAddr) const
{
    UE_LOG(LogEOS, Error, TEXT("FInternetAddrEOSFull::GetIp is not supported and will set OutAddr to 0."));
    OutAddr = 0;
}

void FInternetAddrEOSFull::SetPort(int32 InPort)
{
    this->SymmetricChannel = InPort % EOS_CHANNEL_ID_MODULO;
}

int32 FInternetAddrEOSFull::GetPort() const
{
    return this->SymmetricChannel;
}

void FInternetAddrEOSFull::SetSymmetricChannel(EOS_CHANNEL_ID_TYPE InSymmetricChannel)
{
    this->SymmetricChannel = InSymmetricChannel;
}

EOS_CHANNEL_ID_TYPE FInternetAddrEOSFull::GetSymmetricChannel() const
{
    return this->SymmetricChannel;
}

void FInternetAddrEOSFull::SetRawIp(const TArray<uint8> &RawAddr)
{
    FMemoryReader Reader(RawAddr, true);

    FString ProductUserIdStr;
    FString SocketNameStr;
    EOS_CHANNEL_ID_TYPE ChannelRaw;

    Reader << ProductUserIdStr;
    Reader << SocketNameStr;
    Reader << ChannelRaw;

    EOS_ProductUserId ProductUserId = {};
    if (EOSString_ProductUserId::FromString(ProductUserIdStr, ProductUserId) != EOS_EResult::EOS_Success)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT(
                "FInternetAddrEOSFull::SetRawIp did not have a valid product user ID. If you want to create an address "
                "for binding locally, use the ID of the user that will be listening."));
        return;
    }

    if (SocketNameStr.IsEmpty())
    {
        UE_LOG(LogEOS, Error, TEXT("FInternetAddrEOSFull::SetRawIp did not have a valid socket name."));
        return;
    }

    this->UserId = ProductUserId;
    this->SymmetricSocketName = SocketNameStr;
    this->SymmetricChannel = ChannelRaw;
}

TArray<uint8> FInternetAddrEOSFull::GetRawIp() const
{
    TArray<uint8> Result;
    FMemoryWriter Writer(Result, true);

    FString ProductUserIdStr;
    if (EOSString_ProductUserId::ToString(this->UserId, ProductUserIdStr) != EOS_EResult::EOS_Success)
    {
        UE_LOG(LogEOS, Error, TEXT("FInternetAddrEOSFull::GetRawIp could not serialize the current user ID."));
        return TArray<uint8>();
    }

    FString SocketNameStr = FString(this->SymmetricSocketName);
    Writer << ProductUserIdStr;
    Writer << SocketNameStr;
    EOS_CHANNEL_ID_TYPE ChanLeft = (EOS_CHANNEL_ID_TYPE)this->SymmetricChannel;
    Writer << ChanLeft;

    Writer.Close();

    return Result;
}

void FInternetAddrEOSFull::SetAnyAddress()
{
    UE_LOG(LogEOS, Error, TEXT("FInternetAddrEOSFull::SetAnyAddress is not supported."));
}

void FInternetAddrEOSFull::SetBroadcastAddress()
{
    UE_LOG(LogEOS, Error, TEXT("FInternetAddrEOSFull::SetBroadcastAddress is not supported."));
}

void FInternetAddrEOSFull::SetLoopbackAddress()
{
    UE_LOG(LogEOS, Error, TEXT("FInternetAddrEOSFull::SetLoopbackAddress is not supported."));
}

FString FInternetAddrEOSFull::ToString(bool bAppendPort) const
{
    FString ProductUserIdStr;
    if (EOSString_ProductUserId::ToString(this->UserId, ProductUserIdStr) != EOS_EResult::EOS_Success)
    {
        UE_LOG(LogEOS, Error, TEXT("FInternetAddrEOSFull::ToString could not serialize the current user ID."));
        return TEXT("");
    }

    FString Result = FString::Printf(
        TEXT("%s.%s.%s"),
        *ProductUserIdStr,
        *this->SymmetricSocketName,
        INTERNET_ADDR_EOS_P2P_DOMAIN_SUFFIX);
    if (bAppendPort)
    {
        Result = FString::Printf(TEXT("%s:%u"), *Result, this->SymmetricChannel);
    }
    return Result;
}

uint32 FInternetAddrEOSFull::GetTypeHash() const
{
    return ::GetTypeHash(this->ToString(true));
}

bool FInternetAddrEOSFull::IsValid() const
{
    return EOSString_ProductUserId::IsValid(this->UserId) && this->SymmetricSocketName.Len() > 0 &&
           this->SymmetricSocketName.Len() <= EOS_P2P_SOCKET_NAME_MAX_LENGTH;
}

TSharedRef<FInternetAddr> FInternetAddrEOSFull::Clone() const
{
    auto Clone = MakeShared<FInternetAddrEOSFull>();
    Clone->UserId = this->UserId;
    Clone->SymmetricSocketName = this->SymmetricSocketName;
    Clone->SymmetricChannel = this->SymmetricChannel;
    return Clone;
}

bool FInternetAddrEOSFull::operator==(const FInternetAddr &Other) const
{
    if (!Other.GetProtocolType().IsEqual(REDPOINT_EOS_SUBSYSTEM))
    {
        return false;
    }

    return Other.ToString(true) == this->ToString(true);
}

EOS_DISABLE_STRICT_WARNINGS