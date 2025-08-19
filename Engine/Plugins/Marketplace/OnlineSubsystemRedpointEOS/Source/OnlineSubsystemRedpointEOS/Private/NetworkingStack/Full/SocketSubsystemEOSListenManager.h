// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "./InternetAddrEOSFull.h"
#include "./SocketEOSFull.h"
#include "CoreMinimal.h"
#include "SocketEOSMap.h"

EOS_ENABLE_STRICT_WARNINGS

class FSocketSubsystemEOSListenManager : public TSharedFromThis<FSocketSubsystemEOSListenManager>
{
private:
    struct FSocketEOSListeningKey
    {
    private:
        const EOS_ProductUserId LocalUserId;
        const EOS_P2P_SocketId SymmetricSocketId;

    public:
        FSocketEOSListeningKey(const EOS_ProductUserId InLocalUserId, const EOS_P2P_SocketId InSymmetricSocketId)
            : LocalUserId(InLocalUserId)
            , SymmetricSocketId(InSymmetricSocketId){};
        FSocketEOSListeningKey(const FSocketEOSListeningKey &Other)
            : LocalUserId(Other.LocalUserId)
            , SymmetricSocketId(Other.SymmetricSocketId){};
        ~FSocketEOSListeningKey(){};

        friend uint32 GetTypeHash(const FSocketEOSListeningKey &A)
        {
            return GetTypeHash(A.ToString());
        }

        FString ToString() const;
    };

    template <typename ValueType>
    struct TSocketEOSListeningMapHashableKeyFuncs
        : BaseKeyFuncs<TPair<FSocketEOSListeningKey, ValueType>, ValueType, false>
    {
        typedef typename TTypeTraits<FSocketEOSListeningKey>::ConstPointerType KeyInitType;
        typedef const TPairInitializer<
            typename TTypeTraits<FSocketEOSListeningKey>::ConstInitType,
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
    class TSocketEOSListeningMap : public TMap<
                                       FSocketEOSListeningKey,
                                       ValueType,
                                       FDefaultSetAllocator,
                                       TSocketEOSListeningMapHashableKeyFuncs<ValueType>>
    {
    };

    EOS_HP2P EOSP2P;

    TSocketEOSListeningMap<TArray<TSharedRef<FSocketEOSFull>>> ListeningSockets;
    TSocketEOSListeningMap<TSharedPtr<EOSEventHandle<EOS_P2P_OnIncomingConnectionRequestInfo>>> AcceptEvents;
    TSocketEOSListeningMap<TSharedPtr<EOSEventHandle<EOS_P2P_OnRemoteConnectionClosedInfo>>> ClosedEvents;

    void OnIncomingConnectionRequest(const EOS_P2P_OnIncomingConnectionRequestInfo *Info);
    void OnRemoteConnectionClosed(const EOS_P2P_OnRemoteConnectionClosedInfo *Info);

public:
    UE_NONCOPYABLE(FSocketSubsystemEOSListenManager);
    FSocketSubsystemEOSListenManager(EOS_HP2P InP2P);
    virtual ~FSocketSubsystemEOSListenManager() = default;

    bool Add(FSocketEOSFull &InSocket, const FInternetAddrEOSFull &InLocalAddr);
    bool Remove(FSocketEOSFull &InSocket, const FInternetAddrEOSFull &InLocalAddr);

    bool GetListeningSocketForNewInboundConnection(
        const FSocketEOSKey &InLookupKey,
        TSharedPtr<FSocketEOSFull> &OutSocket);
};

EOS_DISABLE_STRICT_WARNINGS
