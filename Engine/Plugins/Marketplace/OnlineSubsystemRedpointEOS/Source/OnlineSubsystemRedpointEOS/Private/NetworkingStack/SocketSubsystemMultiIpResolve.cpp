// Copyright June Rhodes. All Rights Reserved.

#include "SocketSubsystemMultiIpResolve.h"

EOS_ENABLE_STRICT_WARNINGS

#if !UE_BUILD_SHIPPING
PRAGMA_DISABLE_DEPRECATION_WARNINGS

/** Async task support for GetAddressInfo */
class FGetAddressInfoTask : public FNonAbandonableTask
{
public:
    friend class FAutoDeleteAsyncTask<FGetAddressInfoTask>;

    FGetAddressInfoTask(
        class ISocketSubsystem *InSocketSubsystem,
        const FString &InQueryHost,
        const FString &InQueryService,
        EAddressInfoFlags InQueryFlags,
        const FName &InQueryProtocol,
        ESocketType InQuerySocketType,
        FAsyncGetAddressInfoCallback InCallbackFunction)
        :

        SocketSubsystem(InSocketSubsystem)
        , QueryHost(InQueryHost)
        , QueryService(InQueryService)
        , QueryFlags(InQueryFlags)
        , QueryProtocol(InQueryProtocol)
        , QuerySocketType(InQuerySocketType)
        , CallbackFunction(MoveTemp(InCallbackFunction))
    {
    }

    void DoWork()
    {
        CallbackFunction(
            SocketSubsystem->GetAddressInfo(*QueryHost, *QueryService, QueryFlags, QueryProtocol, QuerySocketType));
    }

    FORCEINLINE TStatId GetStatId() const
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(FGetAddressInfoTask, STATGROUP_ThreadPoolAsyncTasks);
    }

private:
    ISocketSubsystem *SocketSubsystem;
    const FString QueryHost;
    const FString QueryService;
    EAddressInfoFlags QueryFlags;
    const FName QueryProtocol;
    ESocketType QuerySocketType;
    FAsyncGetAddressInfoCallback CallbackFunction;
};

bool FSocketSubsystemMultiIpResolve::Init(FString &Error)
{
    return ISocketSubsystem::Get()->Init(Error);
}

void FSocketSubsystemMultiIpResolve::Shutdown()
{
    return ISocketSubsystem::Get()->Shutdown();
}

FSocket *FSocketSubsystemMultiIpResolve::CreateSocket(
    const FName &SocketType,
    const FString &SocketDescription,
    bool bForceUDP)
{
    return ISocketSubsystem::Get()->CreateSocket(SocketType, SocketDescription, bForceUDP);
}

FSocket *FSocketSubsystemMultiIpResolve::CreateSocket(
    const FName &SocketType,
    const FString &SocketDescription,
    ESocketProtocolFamily ProtocolType)
{
    return ISocketSubsystem::Get()->CreateSocket(SocketType, SocketDescription, ProtocolType);
}

FSocket *FSocketSubsystemMultiIpResolve::CreateSocket(
    const FName &SocketType,
    const FString &SocketDescription,
    const FName &ProtocolName)
{
    return ISocketSubsystem::Get()->CreateSocket(SocketType, SocketDescription, ProtocolName);
}

class FResolveInfoCached *FSocketSubsystemMultiIpResolve::CreateResolveInfoCached(TSharedPtr<FInternetAddr> Addr) const
{
    return ISocketSubsystem::Get()->CreateResolveInfoCached(Addr);
}

void FSocketSubsystemMultiIpResolve::DestroySocket(FSocket *Socket)
{
    return ISocketSubsystem::Get()->DestroySocket(Socket);
}

FAddressInfoResult FSocketSubsystemMultiIpResolve::GetAddressInfo(
    const TCHAR *HostName,
    const TCHAR *ServiceName,
    EAddressInfoFlags QueryFlags,
    ESocketProtocolFamily ProtocolType,
    ESocketType SocketType)
{
    TArray<FAddressInfoResult> Results;
    TArray<FString> HostNames;
    FString(HostName).ParseIntoArray(HostNames, TEXT(","), true);

    // If we have no hostnames to query, just passthrough.
    if (HostNames.Num() == 0)
    {
        return ISocketSubsystem::Get()->GetAddressInfo(HostName, ServiceName, QueryFlags, ProtocolType, SocketType);
    }

    // Query each of the hostnames.
    for (const auto &HostNameEntry : HostNames)
    {
        Results.Add(
            ISocketSubsystem::Get()->GetAddressInfo(*HostNameEntry, ServiceName, QueryFlags, ProtocolType, SocketType));
    }

    // Merge all the results together.
    FAddressInfoResult FinalResult = Results[0];
    for (int i = 1; i < Results.Num(); i++)
    {
        for (const auto &Result : Results[i].Results)
        {
            FinalResult.Results.Add(Result);
        }
    }
    return FinalResult;
}

FAddressInfoResult FSocketSubsystemMultiIpResolve::GetAddressInfo(
    const TCHAR *HostName,
    const TCHAR *ServiceName,
    EAddressInfoFlags QueryFlags,
    const FName ProtocolTypeName,
    ESocketType SocketType)
{
    TArray<FAddressInfoResult> Results;
    TArray<FString> HostNames;
    FString(HostName).ParseIntoArray(HostNames, TEXT(","), true);

    // If we have no hostnames to query, just passthrough.
    if (HostNames.Num() == 0)
    {
        return ISocketSubsystem::Get()->GetAddressInfo(HostName, ServiceName, QueryFlags, ProtocolTypeName, SocketType);
    }

    // Query each of the hostnames.
    for (const auto &HostNameEntry : HostNames)
    {
        Results.Add(ISocketSubsystem::Get()
                        ->GetAddressInfo(*HostNameEntry, ServiceName, QueryFlags, ProtocolTypeName, SocketType));
    }

    // Merge all the results together.
    FAddressInfoResult FinalResult = Results[0];
    for (int i = 1; i < Results.Num(); i++)
    {
        for (const auto &Result : Results[i].Results)
        {
            FinalResult.Results.Add(Result);
        }
    }
    return FinalResult;
}

void FSocketSubsystemMultiIpResolve::GetAddressInfoAsync(
    FAsyncGetAddressInfoCallback Callback,
    const TCHAR *HostName,
    const TCHAR *ServiceName,
    EAddressInfoFlags QueryFlags,
    const FName ProtocolTypeName,
    ESocketType SocketType)
{
    (new FAutoDeleteAsyncTask<
         FGetAddressInfoTask>(this, HostName, ServiceName, QueryFlags, ProtocolTypeName, SocketType, Callback))
        ->StartBackgroundTask();
}

TSharedPtr<FInternetAddr> FSocketSubsystemMultiIpResolve::GetAddressFromString(const FString &InAddress)
{
    return ISocketSubsystem::Get()->GetAddressFromString(InAddress);
}

ESocketErrors FSocketSubsystemMultiIpResolve::GetHostByName(const ANSICHAR *HostName, FInternetAddr &OutAddr)
{
    return ISocketSubsystem::Get()->GetHostByName(HostName, OutAddr);
}

class FResolveInfo *FSocketSubsystemMultiIpResolve::GetHostByName(const ANSICHAR *HostName)
{
    return ISocketSubsystem::Get()->GetHostByName(HostName);
}

bool FSocketSubsystemMultiIpResolve::RequiresChatDataBeSeparate()
{
    return ISocketSubsystem::Get()->RequiresChatDataBeSeparate();
}

bool FSocketSubsystemMultiIpResolve::RequiresEncryptedPackets()
{
    return ISocketSubsystem::Get()->RequiresEncryptedPackets();
}

bool FSocketSubsystemMultiIpResolve::GetHostName(FString &HostName)
{
    return ISocketSubsystem::Get()->GetHostName(HostName);
}

TSharedRef<FInternetAddr> FSocketSubsystemMultiIpResolve::CreateInternetAddr(uint32 Address, uint32 Port)
{
    return ISocketSubsystem::Get()->CreateInternetAddr(Address, Port);
}

TSharedRef<FInternetAddr> FSocketSubsystemMultiIpResolve::CreateInternetAddr()
{
    return ISocketSubsystem::Get()->CreateInternetAddr();
}

TSharedRef<FInternetAddr> FSocketSubsystemMultiIpResolve::CreateInternetAddr(const FName ProtocolType)
{
    return ISocketSubsystem::Get()->CreateInternetAddr(ProtocolType);
}

TUniquePtr<FRecvMulti> FSocketSubsystemMultiIpResolve::CreateRecvMulti(
    int32 MaxNumPackets,
    int32 MaxPacketSize,
    ERecvMultiFlags Flags)
{
    return ISocketSubsystem::Get()->CreateRecvMulti(MaxNumPackets, MaxPacketSize, Flags);
}

bool FSocketSubsystemMultiIpResolve::HasNetworkDevice()
{
    return ISocketSubsystem::Get()->HasNetworkDevice();
}

const TCHAR *FSocketSubsystemMultiIpResolve::GetSocketAPIName() const
{
    return ISocketSubsystem::Get()->GetSocketAPIName();
}

ESocketErrors FSocketSubsystemMultiIpResolve::GetLastErrorCode()
{
    return ISocketSubsystem::Get()->GetLastErrorCode();
}

ESocketErrors FSocketSubsystemMultiIpResolve::TranslateErrorCode(int32 Code)
{
    return ISocketSubsystem::Get()->TranslateErrorCode(Code);
}

bool FSocketSubsystemMultiIpResolve::GetLocalAdapterAddresses(TArray<TSharedPtr<FInternetAddr>> &OutAddresses)
{
    return ISocketSubsystem::Get()->GetLocalAdapterAddresses(OutAddresses);
}

TSharedRef<FInternetAddr> FSocketSubsystemMultiIpResolve::GetLocalBindAddr(FOutputDevice &Out)
{
    return ISocketSubsystem::Get()->GetLocalBindAddr(Out);
}

TArray<TSharedRef<FInternetAddr>> FSocketSubsystemMultiIpResolve::GetLocalBindAddresses()
{
    return ISocketSubsystem::Get()->GetLocalBindAddresses();
}

TSharedRef<FInternetAddr> FSocketSubsystemMultiIpResolve::GetLocalHostAddr(FOutputDevice &Out, bool &bCanBindAll)
{
    return ISocketSubsystem::Get()->GetLocalHostAddr(Out, bCanBindAll);
}

bool FSocketSubsystemMultiIpResolve::GetMultihomeAddress(TSharedRef<FInternetAddr> &Addr)
{
    return ISocketSubsystem::Get()->GetMultihomeAddress(Addr);
}

bool FSocketSubsystemMultiIpResolve::IsSocketRecvMultiSupported() const
{
    return ISocketSubsystem::Get()->IsSocketRecvMultiSupported();
}

bool FSocketSubsystemMultiIpResolve::IsSocketWaitSupported() const
{
    return ISocketSubsystem::Get()->IsSocketWaitSupported();
}

double FSocketSubsystemMultiIpResolve::TranslatePacketTimestamp(
    const FPacketTimestamp &Timestamp,
    ETimestampTranslation Translation)
{
    return ISocketSubsystem::Get()->TranslatePacketTimestamp(Timestamp, Translation);
}

ESocketProtocolFamily FSocketSubsystemMultiIpResolve::GetProtocolFamilyFromName(const FName &InProtocolName) const
{
    if (InProtocolName == FNetworkProtocolTypes::IPv6)
    {
        return ESocketProtocolFamily::IPv6;
    }
    else if (InProtocolName == FNetworkProtocolTypes::IPv4)
    {
        return ESocketProtocolFamily::IPv4;
    }

    return ESocketProtocolFamily::None;
}

FName FSocketSubsystemMultiIpResolve::GetProtocolNameFromFamily(ESocketProtocolFamily InProtocolFamily) const
{
    switch (InProtocolFamily)
    {
    default:
        return NAME_None;
    case ESocketProtocolFamily::IPv4:
        return FNetworkProtocolTypes::IPv4;
    case ESocketProtocolFamily::IPv6:
        return FNetworkProtocolTypes::IPv6;
    }
}

PRAGMA_ENABLE_DEPRECATION_WARNINGS
#endif

EOS_DISABLE_STRICT_WARNINGS