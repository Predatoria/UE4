// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemImpl.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSDefines.h"

EOS_ENABLE_STRICT_WARNINGS

/**
 * This is an extended version of FOnlineSubsystemImpl that provides default implementations for all of the
 * Get*Interface() methods, even on older engine versions.
 */
class ONLINESUBSYSTEMREDPOINTEOS_API FOnlineSubsystemImplBase : public FOnlineSubsystemImpl
{
public:
    FOnlineSubsystemImplBase(FName InSubsystemName, FName InInstanceName)
        : FOnlineSubsystemImpl(InSubsystemName, InInstanceName){};
    FOnlineSubsystemImplBase(FName InSubsystemName, FName InInstanceName, FUTicker &Ticker)
        : FOnlineSubsystemImpl(InSubsystemName, InInstanceName, Ticker){};
    virtual ~FOnlineSubsystemImplBase(){};

    virtual IOnlineIdentityPtr GetIdentityInterface() const override
    {
        return nullptr;
    }

    virtual IOnlineFriendsPtr GetFriendsInterface() const override
    {
        return nullptr;
    }

    virtual IOnlineUserPtr GetUserInterface() const override
    {
        return nullptr;
    }

    virtual IOnlineSessionPtr GetSessionInterface() const override
    {
        return nullptr;
    }

    virtual IOnlineTurnBasedPtr GetTurnBasedInterface() const override
    {
        return nullptr;
    }

    virtual IOnlineTournamentPtr GetTournamentInterface() const override
    {
        return nullptr;
    }

    virtual IOnlinePresencePtr GetPresenceInterface() const override
    {
        return nullptr;
    }

    virtual IOnlinePartyPtr GetPartyInterface() const override
    {
        return nullptr;
    }

    virtual IOnlineUserCloudPtr GetUserCloudInterface() const override
    {
        return nullptr;
    }

    virtual IOnlineTitleFilePtr GetTitleFileInterface() const override
    {
        return nullptr;
    }

    virtual IOnlineLeaderboardsPtr GetLeaderboardsInterface() const override
    {
        return nullptr;
    }

    virtual IOnlineAchievementsPtr GetAchievementsInterface() const override
    {
        return nullptr;
    }

    virtual IOnlineStatsPtr GetStatsInterface() const override
    {
        return nullptr;
    }

    virtual IOnlineGroupsPtr GetGroupsInterface() const override
    {
        return nullptr;
    }

    virtual IOnlineSharedCloudPtr GetSharedCloudInterface() const override
    {
        return nullptr;
    }

    virtual IOnlineEntitlementsPtr GetEntitlementsInterface() const override
    {
        return nullptr;
    }

    virtual IOnlineVoicePtr GetVoiceInterface() const override
    {
        return nullptr;
    }

    virtual IOnlineExternalUIPtr GetExternalUIInterface() const override
    {
        return nullptr;
    }

    virtual IOnlineTimePtr GetTimeInterface() const override
    {
        return nullptr;
    }

    virtual IOnlineStoreV2Ptr GetStoreV2Interface() const override
    {
        return nullptr;
    }

    virtual IOnlinePurchasePtr GetPurchaseInterface() const override
    {
        return nullptr;
    }

    virtual IOnlineEventsPtr GetEventsInterface() const override
    {
        return nullptr;
    }

    virtual IOnlineSharingPtr GetSharingInterface() const override
    {
        return nullptr;
    }

    virtual IOnlineMessagePtr GetMessageInterface() const override
    {
        return nullptr;
    }

    virtual IOnlineChatPtr GetChatInterface() const override
    {
        return nullptr;
    }
};

EOS_DISABLE_STRICT_WARNINGS