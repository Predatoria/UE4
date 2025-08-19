// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Serialization/Archive.h"

EOS_ENABLE_STRICT_WARNINGS

enum class ESerializedPendingFriendRequestMode : uint8
{
    /**
     * We couldn't reach the target player during the original SendInvite call, so
     * we've scheduled to try again later.
     */
    OutboundPendingSend,

    /**
     * We've made a send request (or are about to), and we're waiting for the target
     * user to confirm that they've received the request (by sending another message
     * back to us).
     */
    OutboundPendingReceive,

    /**
     * The target user has received the request (and has told us so). We're waiting
     * on the user at the other end to make an accept/reject decision.
     */
    OutboundPendingResponse,

    /**
     * We've had a friend request arrive, but the user is yet to pick whether to
     * accept or reject it.
     */
    InboundPendingResponse,

    /**
     * We've accepted a friend request, but we haven't been able to tell the sender
     * that we've accepted it yet (due to them being offline).
     */
    InboundPendingSendAcceptance,

    /**
     * We've rejected a friend request, but we haven't been able to tell the sender
     * that we've rejected it yet (due to them being offline).
     */
    InboundPendingSendRejection,

    /**
     * We've deleted a friend, but we haven't been able to tell them to remove us
     * from their friends list yet (due to them being offline).
     */
    OutboundPendingSendDeletion,
};

struct FSerializedPendingFriendRequest
{
public:
    FString ProductUserId;
    ESerializedPendingFriendRequestMode Mode;
    FDateTime NextAttempt;

    friend FArchive &operator<<(FArchive &Ar, FSerializedPendingFriendRequest &Obj)
    {
        int8 Version = -1;
        if (Ar.IsSaving())
        {
            Version = 2;
        }
        Ar << Version;
        Ar << Obj.ProductUserId;
        Ar << (uint8 &)Obj.Mode;
        if (Version >= 2)
        {
            Ar << Obj.NextAttempt;
        }
        return Ar;
    }
};

EOS_DISABLE_STRICT_WARNINGS