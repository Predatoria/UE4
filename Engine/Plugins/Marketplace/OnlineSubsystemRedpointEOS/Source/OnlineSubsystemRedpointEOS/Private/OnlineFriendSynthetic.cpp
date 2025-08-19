// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/OnlineFriendSynthetic.h"

#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/PDSFriends/FriendDatabase.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"

EOS_ENABLE_STRICT_WARNINGS

FOnlineFriendSynthetic::FOnlineFriendSynthetic(
    TSharedPtr<FOnlineUserEOS> InUserEOS,
    const TMap<FName, TSharedPtr<FOnlineFriend>> &InWrappedFriends,
    TWeakPtr<class FFriendDatabase> InFriendDatabase)
    : UserEOS(MoveTemp(InUserEOS))
    , WrappedFriends(InWrappedFriends)
    , FriendDatabase(MoveTemp(InFriendDatabase))
{
    checkf(
        InWrappedFriends.Num() > 0 || UserEOS.IsValid(),
        TEXT("FOnlineFriendSynthetic initialized with no friends to wrap and no EOS account!"));

    this->PreferredSubsystemName = NULL_SUBSYSTEM.ToString();
    this->NullPresence.Reset();

    for (const auto &KV : InWrappedFriends)
    {
        this->PreferredFriend = KV.Value;
        this->PreferredSubsystemName = KV.Key.ToString();
        break;
    }

    // Sometimes the primary friend (EOS) won't have a display name
    // or real name, so for those fields, try to find the friend data
    // with the most information.
    int32 CurrentScore = -1000;
    for (const auto &KV : InWrappedFriends)
    {
        int32 ThisFriendScore = 0;
        if (!KV.Value->GetDisplayName().IsEmpty())
        {
            ThisFriendScore += 100;
        }
        if (!KV.Value->GetRealName().IsEmpty())
        {
            ThisFriendScore += 50;
        }

        if (ThisFriendScore > CurrentScore)
        {
            this->PreferredFriend = KV.Value;
            this->PreferredSubsystemName = KV.Key.ToString();
            CurrentScore = ThisFriendScore;
        }
    }
}

TSharedRef<const FUniqueNetId> FOnlineFriendSynthetic::GetUserId() const
{
    if (this->UserEOS.IsValid())
    {
        return this->UserEOS->GetUserId();
    }
    else
    {
        return this->PreferredFriend->GetUserId();
    }
}

FString FOnlineFriendSynthetic::GetRealName() const
{
    if (this->PreferredFriend.IsValid())
    {
        return this->PreferredFriend->GetRealName();
    }
    else
    {
        return this->UserEOS->GetRealName();
    }
}

FString FOnlineFriendSynthetic::GetDisplayName(const FString &Platform) const
{
    auto Db = this->FriendDatabase.Pin();
    if (Db.IsValid())
    {
        const auto &FriendAliases = Db->GetFriendAliases();
        TSet<FString> IdsToCheck;
        if (this->UserEOS.IsValid())
        {
            IdsToCheck.Add(FString::Printf(
                TEXT("%s:%s"),
                *this->UserEOS->GetUserId()->GetType().ToString(),
                *this->UserEOS->GetUserId()->ToString()));
        }
        for (const auto &WrappedFriend : WrappedFriends)
        {
            IdsToCheck.Add(FString::Printf(
                TEXT("%s:%s"),
                *WrappedFriend.Value->GetUserId()->GetType().ToString(),
                *WrappedFriend.Value->GetUserId()->ToString()));
        }
        for (const auto &Id : IdsToCheck)
        {
            if (FriendAliases.Contains(Id))
            {
                return FriendAliases[Id];
            }
        }
    }

    if (this->PreferredFriend.IsValid())
    {
        return this->PreferredFriend->GetDisplayName(Platform);
    }
    else
    {
        return this->UserEOS->GetDisplayName(Platform);
    }
}

bool FOnlineFriendSynthetic::GetUserAttribute(const FString &AttrName, FString &OutAttrValue) const
{
    if (AttrName == TEXT("SubsystemName") || AttrName == TEXT("eosSynthetic.primaryFriend.subsystemName"))
    {
        if (this->UserEOS.IsValid())
        {
            OutAttrValue = TEXT("eos");
            return true;
        }

        OutAttrValue = this->PreferredSubsystemName;
        return true;
    }

    if (AttrName == TEXT("deletable"))
    {
        // The friend is deletable if they've come from the friends database
        // (and is not represented by any wrapped friends).
        OutAttrValue = this->WrappedFriends.Num() == 0 ? TEXT("true") : TEXT("false");
        return true;
    }

    if (AttrName == TEXT("eosSynthetic.preferredFriend.subsystemName"))
    {
        if (this->PreferredFriend.IsValid())
        {
            OutAttrValue = this->PreferredSubsystemName;
        }
        else
        {
            OutAttrValue = TEXT("eos");
        }
        return true;
    }

    if (AttrName == TEXT("eosSynthetic.subsystemNames"))
    {
        TArray<FString> SubsystemNames;
        if (this->UserEOS.IsValid())
        {
            SubsystemNames.Add(TEXT("eos"));
        }
        for (const auto &KV : this->WrappedFriends)
        {
            SubsystemNames.Add(KV.Key.ToString());
        }
        OutAttrValue = FString::Join(SubsystemNames, TEXT(","));
        return true;
    }

    if (AttrName.StartsWith("eosSynthetic.friend."))
    {
        FString CutString = AttrName.Mid(FString("eosSynthetic.friend.").Len());
        int IndexOfNextDot = CutString.Find(TEXT("."));
        FString SubsystemNameStr = CutString.Mid(0, IndexOfNextDot);
        FName SubsystemName = FName(*SubsystemNameStr);
        if (this->UserEOS.IsValid() && SubsystemName == TEXT("eos"))
        {
            FString SubAttrName = CutString.Mid(IndexOfNextDot + 1);
            if (SubAttrName == TEXT("id"))
            {
                OutAttrValue = this->UserEOS->GetUserId()->ToString();
                return true;
            }
            else if (SubAttrName == TEXT("realName"))
            {
                OutAttrValue = this->UserEOS->GetRealName();
                return true;
            }
            else if (SubAttrName == TEXT("displayName"))
            {
                OutAttrValue = this->UserEOS->GetDisplayName(SubsystemName.ToString());
                return true;
            }
            else if (SubAttrName.StartsWith("attr."))
            {
                FString SubSubAttrName = SubAttrName.Mid(5);
                return this->UserEOS->GetUserAttribute(SubSubAttrName, OutAttrValue);
            }
            else
            {
                // Unknown sub-attribute.
                return false;
            }
        }
        else if (this->WrappedFriends.Contains(SubsystemName))
        {
            const auto &WrappedFriend = this->WrappedFriends[SubsystemName];

            FString SubAttrName = CutString.Mid(IndexOfNextDot + 1);
            if (SubAttrName == TEXT("id"))
            {
                OutAttrValue = WrappedFriend->GetUserId()->ToString();
                return true;
            }
            else if (SubAttrName == TEXT("realName"))
            {
                OutAttrValue = WrappedFriend->GetRealName();
                return true;
            }
            else if (SubAttrName == TEXT("displayName"))
            {
                OutAttrValue = WrappedFriend->GetDisplayName(SubsystemName.ToString());
                return true;
            }
            else if (SubAttrName.StartsWith("attr."))
            {
                FString SubSubAttrName = SubAttrName.Mid(5);
                return WrappedFriend->GetUserAttribute(SubSubAttrName, OutAttrValue);
            }
            else
            {
                // Unknown sub-attribute.
                return false;
            }
        }
        else
        {
            // No such wrapped friend.
            return false;
        }
    }

    if (this->PreferredFriend.IsValid() && this->PreferredFriend->GetUserAttribute(AttrName, OutAttrValue))
    {
        // We fetched the attribute from the preferred friend first.
        return true;
    }

    if (this->UserEOS.IsValid())
    {
        // Return EOS user attribute if we have an EOS user attribute.
        return this->UserEOS->GetUserAttribute(AttrName, OutAttrValue);
    }

    return false;
}

EInviteStatus::Type FOnlineFriendSynthetic::GetInviteStatus() const
{
    if (this->UserEOS.IsValid())
    {
        auto UserIdEOS = this->UserEOS->GetUserId();
        if (auto Db = this->FriendDatabase.Pin())
        {
            if (Db->GetFriends().Contains(*UserIdEOS))
            {
                return EInviteStatus::Accepted;
            }
            else if (Db->GetPendingFriendRequests().Contains(*UserIdEOS))
            {
                switch (Db->GetPendingFriendRequests()[*UserIdEOS].Mode)
                {
                case ESerializedPendingFriendRequestMode::InboundPendingResponse:
                    return EInviteStatus::PendingInbound;
                case ESerializedPendingFriendRequestMode::OutboundPendingReceive:
                case ESerializedPendingFriendRequestMode::OutboundPendingResponse:
                case ESerializedPendingFriendRequestMode::OutboundPendingSend:
                    return EInviteStatus::PendingOutbound;
                }
            }
        }
    }

    if (this->PreferredFriend.IsValid())
    {
        return this->PreferredFriend->GetInviteStatus();
    }

    return EInviteStatus::Unknown;
}

const class FOnlineUserPresence &FOnlineFriendSynthetic::GetPresence() const
{
    if (this->PreferredFriend.IsValid())
    {
        return this->PreferredFriend->GetPresence();
    }
    else
    {
        return this->NullPresence;
    }
}

EOS_DISABLE_STRICT_WARNINGS