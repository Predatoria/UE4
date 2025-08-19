// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/EOSListenTracker.h"

#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"

EOS_ENABLE_STRICT_WARNINGS

class FEOSListenEntry
{
public:
    TSharedRef<const FInternetAddr> ListeningAddress;
    TArray<TSharedPtr<FInternetAddr>> DeveloperAddresses;
    int ReferenceCount;

    FEOSListenEntry(
        TSharedRef<const FInternetAddr> InInternetAddr,
        TArray<TSharedPtr<FInternetAddr>> InDeveloperInternetAddrs)
        : ListeningAddress(MoveTemp(InInternetAddr))
        , DeveloperAddresses(MoveTemp(InDeveloperInternetAddrs))
        , ReferenceCount(1){};
};

FString FEOSListenTracker::GetMapKey(EOS_ProductUserId InProductUserId)
{
    FString ProductUserIdKey;
    if (InProductUserId == nullptr)
    {
        // Dedicated server.
        ProductUserIdKey = EOS_DEDICATED_SERVER_ID;
    }
    else
    {
        check(EOSString_ProductUserId::IsValid(InProductUserId));
        verify(EOSString_ProductUserId::ToString(InProductUserId, ProductUserIdKey) == EOS_EResult::EOS_Success);
    }
    return ProductUserIdKey;
}

void FEOSListenTracker::Register(
    EOS_ProductUserId InProductUserId,
    const TSharedRef<const FInternetAddr> &InInternetAddr,
    const TArray<TSharedPtr<FInternetAddr>> &InDeveloperInternetAddrs)
{
    FString MapKey = GetMapKey(InProductUserId);

    if (!this->Entries.Contains(MapKey))
    {
        this->Entries.Add(MapKey, TArray<TSharedRef<FEOSListenEntry>>());
    }
    TArray<TSharedRef<FEOSListenEntry>> &Arr = this->Entries[MapKey];

    for (const auto &Entry : Arr)
    {
        if (*Entry->ListeningAddress == *InInternetAddr)
        {
            Entry->ReferenceCount++;
            return;
        }
    }

    Arr.Add(MakeShared<FEOSListenEntry>(InInternetAddr, InDeveloperInternetAddrs));
    if (Arr.Num() == 1)
    {
        OnChanged.Broadcast(InProductUserId, InInternetAddr, InDeveloperInternetAddrs);
    }
}

void FEOSListenTracker::Deregister(
    EOS_ProductUserId InProductUserId,
    const TSharedRef<const FInternetAddr> &InInternetAddr)
{
    FString MapKey = GetMapKey(InProductUserId);

    if (!this->Entries.Contains(MapKey))
    {
        return;
    }
    TArray<TSharedRef<FEOSListenEntry>> &Arr = this->Entries[MapKey];

    bool bRemovedEntry = false;
    for (int i = 0; i < Arr.Num(); i++)
    {
        const auto &Entry = Arr[i];
        if (*Entry->ListeningAddress == *InInternetAddr)
        {
            Entry->ReferenceCount--;
            if (Entry->ReferenceCount == 0)
            {
                // This entry is gone, remove it.
                Arr.RemoveAt(i);
                bRemovedEntry = true;
                break;
            }
            else
            {
                // Not removed yet, but dereferenced so we're done.
                return;
            }
        }
    }

    if (bRemovedEntry)
    {
        // Notify listeners that the address for this user might have
        // changed (or entirely gone away).
        if (Arr.Num() == 0)
        {
            OnClosed.Broadcast(InProductUserId);
        }
        else
        {
            auto FirstEntry = Arr[0];
            OnChanged.Broadcast(InProductUserId, FirstEntry->ListeningAddress, FirstEntry->DeveloperAddresses);
        }
    }
}

bool FEOSListenTracker::Get(
    EOS_ProductUserId InProductUserId,
    TSharedPtr<const FInternetAddr> &OutInternetAddr,
    TArray<TSharedPtr<FInternetAddr>> &OutDeveloperInternetAddrs)
{
    FString MapKey = GetMapKey(InProductUserId);

    if (!this->Entries.Contains(MapKey))
    {
        return false;
    }
    TArray<TSharedRef<FEOSListenEntry>> &Arr = this->Entries[MapKey];

    if (Arr.Num() == 0)
    {
        return false;
    }

    OutInternetAddr = Arr[0]->ListeningAddress;
    OutDeveloperInternetAddrs = Arr[0]->DeveloperAddresses;
    return true;
}

EOS_DISABLE_STRICT_WARNINGS