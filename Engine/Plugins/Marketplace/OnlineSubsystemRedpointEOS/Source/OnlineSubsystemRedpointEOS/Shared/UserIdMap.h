// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineIdentityInterface.h"

EOS_ENABLE_STRICT_WARNINGS

template <typename ValueType>
struct TUserIdMapHashableKeyFuncs : BaseKeyFuncs<TPair<TSharedRef<const FUniqueNetId>, ValueType>, ValueType, false>
{
    typedef typename TTypeTraits<TSharedRef<const FUniqueNetId>>::ConstPointerType KeyInitType;
    typedef const TPairInitializer<
        typename TTypeTraits<TSharedRef<const FUniqueNetId>>::ConstInitType,
        typename TTypeTraits<ValueType>::ConstInitType> &ElementInitType;

    static FORCEINLINE KeyInitType GetSetKey(ElementInitType Element)
    {
        return Element.Key;
    }

    static FORCEINLINE FString GetKeyValue(KeyInitType K)
    {
        return FString::Printf(TEXT("%s_%s"), *K->GetType().ToString(), *K->ToString());
    }

    static FORCEINLINE bool Matches(KeyInitType A, KeyInitType B)
    {
        return GetKeyValue(A) == GetKeyValue(B);
    }

    template <typename ComparableKey> static FORCEINLINE bool Matches(KeyInitType A, ComparableKey B)
    {
        return GetKeyValue(A) == GetKeyValue(B);
    }

    static FORCEINLINE uint32 GetKeyHash(KeyInitType Key)
    {
        return GetTypeHash(GetKeyValue(Key));
    }

    template <typename ComparableKey> static FORCEINLINE uint32 GetKeyHash(ComparableKey Key)
    {
        return GetTypeHash(GetKeyValue(Key));
    }
};

template <typename ValueType>
class TUserIdMap : public TMap<
                       TSharedRef<const FUniqueNetId>,
                       ValueType,
                       FDefaultSetAllocator,
                       TUserIdMapHashableKeyFuncs<ValueType>>
{
private:
    using TUserIdMapBase =
        TMap<TSharedRef<const FUniqueNetId>, ValueType, FDefaultSetAllocator, TUserIdMapHashableKeyFuncs<ValueType>>;
    using TUserIdMapBaseConst = const TMap<
        TSharedRef<const FUniqueNetId>,
        ValueType,
        FDefaultSetAllocator,
        TUserIdMapHashableKeyFuncs<ValueType>>;

public:
    FORCEINLINE ValueType &operator[](const FUniqueNetId &Key)
    {
        return TUserIdMapBase::operator[](Key.AsShared());
    }
    FORCEINLINE const ValueType &operator[](const FUniqueNetId &Key) const
    {
        return TUserIdMapBaseConst::operator[](Key.AsShared());
    }

    bool Contains(const FUniqueNetId &Key) const
    {
        return TUserIdMapBaseConst::Contains(Key.AsShared());
    }
    void Add(const FUniqueNetId &Key, ValueType Value)
    {
        TUserIdMapBase::Add(Key.AsShared(), Value);
    }
    void Remove(const FUniqueNetId &Key)
    {
        TUserIdMapBase::Remove(Key.AsShared());
    }
};

EOS_DISABLE_STRICT_WARNINGS