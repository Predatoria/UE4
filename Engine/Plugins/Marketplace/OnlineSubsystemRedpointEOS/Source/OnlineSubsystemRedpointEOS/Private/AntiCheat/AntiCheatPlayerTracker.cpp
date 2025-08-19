// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Private/AntiCheat/AntiCheatPlayerTracker.h"

FAntiCheatPlayerTracker::FAntiCheatPlayerTracker()
    : NextHandle((EOS_AntiCheatCommon_ClientHandle)1000)
    , HandlesToPlayers()
    , PlayersToHandles()
    , PlayersToHandleCount()
{
}

EOS_AntiCheatCommon_ClientHandle FAntiCheatPlayerTracker::AddPlayer(
    const FUniqueNetIdEOS &UserId,
    bool &bOutShouldRegister)
{
    UE_LOG(LogEOSAntiCheat, Verbose, TEXT("Player tracking: %s: Requested to add player"), *UserId.ToString());

    if (this->PlayersToHandles.Contains(UserId))
    {
        this->PlayersToHandleCount[UserId]++;
        bOutShouldRegister = false;

        UE_LOG(
            LogEOSAntiCheat,
            Verbose,
            TEXT("Player tracking: %s: Incremented handle count as this player was already registered, handle count is "
                 "now at %d"),
            *UserId.ToString(),
            this->PlayersToHandleCount[UserId]);

        return this->PlayersToHandles[UserId];
    }
    else
    {
        EOS_AntiCheatCommon_ClientHandle ReturnHandle = this->NextHandle;
        this->NextHandle = (EOS_AntiCheatCommon_ClientHandle)((intptr_t)this->NextHandle + 1);
        this->PlayersToHandles.Add(UserId, ReturnHandle);
        this->PlayersToHandleCount.Add(UserId, 1);
        this->HandlesToPlayers.Add(ReturnHandle, StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared()));
        bOutShouldRegister = true;

        UE_LOG(
            LogEOSAntiCheat,
            Verbose,
            TEXT("Player tracking: %s: Added new player, handle count is now at %d"),
            *UserId.ToString(),
            this->PlayersToHandleCount[UserId]);

        return ReturnHandle;
    }
}

void FAntiCheatPlayerTracker::RemovePlayer(const FUniqueNetIdEOS &UserId)
{
    UE_LOG(LogEOSAntiCheat, Verbose, TEXT("Player tracking: %s: Requested to remove player"), *UserId.ToString());

    if (!this->PlayersToHandles.Contains(UserId))
    {
        // Player isn't currently registered.
        UE_LOG(
            LogEOSAntiCheat,
            Verbose,
            TEXT("Player tracking: %s: Player was not registered, ignoring remove request"),
            *UserId.ToString());
        return;
    }

    this->PlayersToHandleCount[UserId]--;

    if (this->PlayersToHandleCount[UserId] == 0)
    {
        // We're at zero, also deregister.
        EOS_AntiCheatCommon_ClientHandle Handle = this->PlayersToHandles[UserId];
        this->PlayersToHandles.Remove(UserId);
        this->PlayersToHandleCount.Remove(UserId);
        this->HandlesToPlayers.Remove(Handle);

        UE_LOG(
            LogEOSAntiCheat,
            Verbose,
            TEXT("Player tracking: %s: Removed player as handle count reached 0"),
            *UserId.ToString());
    }
    else
    {
        UE_LOG(
            LogEOSAntiCheat,
            Verbose,
            TEXT("Player tracking: %s: Decremented handle count, handle count is now at %d"),
            *UserId.ToString(),
            this->PlayersToHandleCount[UserId]);
    }
}

bool FAntiCheatPlayerTracker::ShouldDeregisterPlayerBeforeRemove(const FUniqueNetIdEOS &UserId) const
{
    if (!this->PlayersToHandles.Contains(UserId))
    {
        return false;
    }

    return this->PlayersToHandleCount[UserId] == 1;
}

TSharedPtr<const FUniqueNetIdEOS> FAntiCheatPlayerTracker::GetPlayer(EOS_AntiCheatCommon_ClientHandle Handle) const
{
    return this->HandlesToPlayers[Handle];
}

EOS_AntiCheatCommon_ClientHandle FAntiCheatPlayerTracker::GetHandle(const FUniqueNetIdEOS &UserId) const
{
    return this->PlayersToHandles[UserId];
}

bool FAntiCheatPlayerTracker::HasPlayer(const FUniqueNetIdEOS &UserId) const
{
    return this->PlayersToHandles.Contains(UserId);
}