// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/AntiCheat/AntiCheat.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"

class FAntiCheatPlayerTracker
{
private:
    EOS_AntiCheatCommon_ClientHandle NextHandle;
    TMap<EOS_AntiCheatCommon_ClientHandle, TSharedPtr<const FUniqueNetIdEOS>> HandlesToPlayers;
    TUserIdMap<EOS_AntiCheatCommon_ClientHandle> PlayersToHandles;
    TUserIdMap<int> PlayersToHandleCount;

public:
    FAntiCheatPlayerTracker();

    EOS_AntiCheatCommon_ClientHandle AddPlayer(const FUniqueNetIdEOS &UserId, bool &bOutShouldRegister);
    void RemovePlayer(const FUniqueNetIdEOS &UserId);
    bool ShouldDeregisterPlayerBeforeRemove(const FUniqueNetIdEOS &UserId) const;

    TSharedPtr<const FUniqueNetIdEOS> GetPlayer(EOS_AntiCheatCommon_ClientHandle Handle) const;
    EOS_AntiCheatCommon_ClientHandle GetHandle(const FUniqueNetIdEOS &UserId) const;
    bool HasPlayer(const FUniqueNetIdEOS &UserId) const;
};
