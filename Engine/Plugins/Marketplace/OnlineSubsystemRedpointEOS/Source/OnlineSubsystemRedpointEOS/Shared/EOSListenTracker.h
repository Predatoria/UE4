// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EOSCommon.h"
#include "IPAddress.h"

EOS_ENABLE_STRICT_WARNINGS

// Fired when a user has their listening address either initially opened
// or has changed.
DECLARE_MULTICAST_DELEGATE_ThreeParams(
    FEOSListenTrackerAddressChanged,
    EOS_ProductUserId /* ProductUserId */,
    const TSharedRef<const FInternetAddr> & /* InternetAddr */,
    const TArray<TSharedPtr<FInternetAddr>> & /* DeveloperInternetAddrs */);

// Fired when a user has their listening address closed.
DECLARE_MULTICAST_DELEGATE_OneParam(FEOSListenTrackerAddressClosed, EOS_ProductUserId /* ProductUserId */);

class ONLINESUBSYSTEMREDPOINTEOS_API FEOSListenTracker
{
private:
    TMap<FString, TArray<TSharedRef<class FEOSListenEntry>>> Entries;

    FString GetMapKey(EOS_ProductUserId InProductUserId);

public:
    FEOSListenTrackerAddressChanged OnChanged;
    FEOSListenTrackerAddressClosed OnClosed;

    void Register(
        EOS_ProductUserId InProductUserId,
        const TSharedRef<const FInternetAddr> &InInternetAddr,
        const TArray<TSharedPtr<FInternetAddr>> &InDeveloperInternetAddrs);

    void Deregister(EOS_ProductUserId InProductUserId, const TSharedRef<const FInternetAddr> &InInternetAddr);

    bool Get(
        EOS_ProductUserId InProductUserId,
        TSharedPtr<const FInternetAddr> &OutInternetAddr,
        TArray<TSharedPtr<FInternetAddr>> &OutDeveloperInternetAddrs);
};

EOS_DISABLE_STRICT_WARNINGS