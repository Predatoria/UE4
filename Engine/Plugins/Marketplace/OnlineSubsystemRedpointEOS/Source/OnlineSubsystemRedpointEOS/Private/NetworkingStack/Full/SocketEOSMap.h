// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "Containers/Map.h"
#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"

EOS_ENABLE_STRICT_WARNINGS

struct FSocketEOSKey
{
    // So we can convert it to a listening key...
    friend class FSocketSubsystemEOSListenManager;

private:
    EOS_ProductUserId LocalUserId;
    EOS_ProductUserId RemoteUserId;
    EOS_P2P_SocketId SymmetricSocketId;
    uint8_t SymmetricChannel;

public:
    FSocketEOSKey(
        EOS_ProductUserId InLocalUserId,
        EOS_ProductUserId InRemoteUserId,
        EOS_P2P_SocketId InSymmetricSocketId,
        uint8_t InSymmetricChannel)
        : LocalUserId(InLocalUserId)
        , RemoteUserId(InRemoteUserId)
        , SymmetricSocketId(InSymmetricSocketId)
        , SymmetricChannel(InSymmetricChannel){};
    FSocketEOSKey(const FSocketEOSKey &Other)
        : LocalUserId(Other.LocalUserId)
        , RemoteUserId(Other.RemoteUserId)
        , SymmetricSocketId(Other.SymmetricSocketId)
        , SymmetricChannel(Other.SymmetricChannel){};
    ~FSocketEOSKey(){};

    friend uint32 GetTypeHash(const FSocketEOSKey &A)
    {
        return GetTypeHash(A.ToString());
    };

    FString ToString(bool bExcludeChannel = false) const;
    bool IsIdenticalExcludingChannel(const FSocketEOSKey &Other) const;

    const EOS_ProductUserId &GetLocalUserId() const
    {
        return this->LocalUserId;
    }
    const EOS_ProductUserId &GetRemoteUserId() const
    {
        return this->RemoteUserId;
    }
    const EOS_P2P_SocketId &GetSymmetricSocketId() const
    {
        return this->SymmetricSocketId;
    }
    const uint8_t &GetSymmetricChannel() const
    {
        return this->SymmetricChannel;
    }
};

template <typename ValueType>
struct TSocketEOSMapHashableKeyFuncs : BaseKeyFuncs<TPair<FSocketEOSKey, ValueType>, ValueType, false>
{
    typedef typename TTypeTraits<FSocketEOSKey>::ConstPointerType KeyInitType;
    typedef const TPairInitializer<
        typename TTypeTraits<FSocketEOSKey>::ConstInitType,
        typename TTypeTraits<ValueType>::ConstInitType> &ElementInitType;

    static FORCEINLINE KeyInitType GetSetKey(ElementInitType Element)
    {
        return Element.Key;
    }

    static FORCEINLINE bool Matches(KeyInitType A, KeyInitType B)
    {
        return GetTypeHash(A) == GetTypeHash(B);
    }

    template <typename ComparableKey> static FORCEINLINE bool Matches(KeyInitType A, ComparableKey B)
    {
        return GetTypeHash(A) == GetTypeHash(B);
    }

    static FORCEINLINE uint32 GetKeyHash(KeyInitType Key)
    {
        return GetTypeHash(Key);
    }

    template <typename ComparableKey> static FORCEINLINE uint32 GetKeyHash(ComparableKey Key)
    {
        return GetTypeHash(Key);
    }
};

template <typename ValueType>
class TSocketEOSMap
    : public TMap<FSocketEOSKey, ValueType, FDefaultSetAllocator, TSocketEOSMapHashableKeyFuncs<ValueType>>
{
private:
    using TSocketEOSMapBase =
        TMap<FSocketEOSKey, ValueType, FDefaultSetAllocator, TSocketEOSMapHashableKeyFuncs<ValueType>>;
    using TSocketEOSMapBaseConst =
        const TMap<FSocketEOSKey, ValueType, FDefaultSetAllocator, TSocketEOSMapHashableKeyFuncs<ValueType>>;
};

EOS_DISABLE_STRICT_WARNINGS