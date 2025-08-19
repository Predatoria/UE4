// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSDefines.h"
#include "SocketSubsystem.h"

EOS_ENABLE_STRICT_WARNINGS

#if !UE_BUILD_SHIPPING
PRAGMA_DISABLE_DEPRECATION_WARNINGS

/**
 * To support connecting to dedicated servers in play-in-editor or across a local network, this socket
 * subsystem overrides the GetAddressInfo functions to support comma-separated IP address lists
 * such as "1.1.1.1:7777,192.168.0.1:7777,127.0.0.1:7777" to allow the UIpNetDriver implementation to try multiple
 * IP addresses.
 *
 * This implementation is not available in shipping builds, as it's only intended to be used in development.
 */
class FSocketSubsystemMultiIpResolve : public ISocketSubsystem, public TSharedFromThis<FSocketSubsystemMultiIpResolve>
{
public:
    UE_NONCOPYABLE(FSocketSubsystemMultiIpResolve);
    FSocketSubsystemMultiIpResolve() = default;
    virtual ~FSocketSubsystemMultiIpResolve() = default;

    virtual bool Init(FString &Error) override;
    virtual void Shutdown() override;
    virtual FSocket *CreateSocket(const FName &SocketType, const FString &SocketDescription, bool bForceUDP = false)
        override;
    virtual FSocket *CreateSocket(
        const FName &SocketType,
        const FString &SocketDescription,
        ESocketProtocolFamily ProtocolType) override;
    virtual FSocket *CreateSocket(const FName &SocketType, const FString &SocketDescription, const FName &ProtocolName)
        override;
    virtual class FResolveInfoCached *CreateResolveInfoCached(TSharedPtr<FInternetAddr> Addr) const override;
    virtual void DestroySocket(FSocket *Socket) override;
    virtual FAddressInfoResult GetAddressInfo(
        const TCHAR *HostName,
        const TCHAR *ServiceName = nullptr,
        EAddressInfoFlags QueryFlags = EAddressInfoFlags::Default,
        ESocketProtocolFamily ProtocolType = ESocketProtocolFamily::None,
        ESocketType SocketType = ESocketType::SOCKTYPE_Unknown) override;
    virtual FAddressInfoResult GetAddressInfo(
        const TCHAR *HostName,
        const TCHAR *ServiceName = nullptr,
        EAddressInfoFlags QueryFlags = EAddressInfoFlags::Default,
        const FName ProtocolTypeName = NAME_None,
        ESocketType SocketType = ESocketType::SOCKTYPE_Unknown) override;
    virtual void GetAddressInfoAsync(
        FAsyncGetAddressInfoCallback Callback,
        const TCHAR *HostName,
        const TCHAR *ServiceName = nullptr,
        EAddressInfoFlags QueryFlags = EAddressInfoFlags::Default,
        const FName ProtocolTypeName = NAME_None,
        ESocketType SocketType = ESocketType::SOCKTYPE_Unknown) override;
    virtual TSharedPtr<FInternetAddr> GetAddressFromString(const FString &InAddress) override;
    virtual ESocketErrors GetHostByName(const ANSICHAR *HostName, FInternetAddr &OutAddr) override;
    virtual class FResolveInfo *GetHostByName(const ANSICHAR *HostName) override;
    virtual bool RequiresChatDataBeSeparate() override;
    virtual bool RequiresEncryptedPackets() override;
    virtual bool GetHostName(FString &HostName) override;
    virtual TSharedRef<FInternetAddr> CreateInternetAddr(uint32 Address, uint32 Port = 0) override;
    virtual TSharedRef<FInternetAddr> CreateInternetAddr() override;
    virtual TSharedRef<FInternetAddr> CreateInternetAddr(const FName ProtocolType) override;
    virtual TUniquePtr<FRecvMulti> CreateRecvMulti(
        int32 MaxNumPackets,
        int32 MaxPacketSize,
        ERecvMultiFlags Flags = ERecvMultiFlags::None) override;
    virtual bool HasNetworkDevice() override;
    virtual const TCHAR *GetSocketAPIName() const override;
    virtual ESocketErrors GetLastErrorCode() override;
    virtual ESocketErrors TranslateErrorCode(int32 Code) override;
    virtual bool GetLocalAdapterAddresses(TArray<TSharedPtr<FInternetAddr>> &OutAddresses) override;
    virtual TSharedRef<FInternetAddr> GetLocalBindAddr(FOutputDevice &Out) override;
    virtual TArray<TSharedRef<FInternetAddr>> GetLocalBindAddresses() override;
    virtual TSharedRef<FInternetAddr> GetLocalHostAddr(FOutputDevice &Out, bool &bCanBindAll) override;
    virtual bool GetMultihomeAddress(TSharedRef<FInternetAddr> &Addr) override;
    virtual bool IsSocketRecvMultiSupported() const override;
    virtual bool IsSocketWaitSupported() const override;
    virtual double TranslatePacketTimestamp(
        const FPacketTimestamp &Timestamp,
        ETimestampTranslation Translation = ETimestampTranslation::LocalTimestamp) override;

protected:
    virtual ESocketProtocolFamily GetProtocolFamilyFromName(const FName &InProtocolName) const override;
    virtual FName GetProtocolNameFromFamily(ESocketProtocolFamily InProtocolFamily) const override;
};

PRAGMA_ENABLE_DEPRECATION_WARNINGS
#endif

EOS_DISABLE_STRICT_WARNINGS
