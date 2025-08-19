// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/OnlineReplStructs.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/UserIdMap.h"
#include "Subsystems/WorldSubsystem.h"

#include "RecentPlayersWorldSubsystem.generated.h"

EOS_ENABLE_STRICT_WARNINGS

UCLASS()
class ONLINESUBSYSTEMREDPOINTEOS_API URecentPlayersWorldSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

private:
    TUserIdMap<FReportPlayedWithUser> BufferedRecentPlayers;
    FUTickerDelegateHandle BufferFlushDelegate;
    void OnActorSpawned(AActor *NewActor);
    bool OnDelayedPlayerStartEvaluation(float DeltaSeconds, TSoftObjectPtr<APlayerState> InPlayerState);
    void DiscoveredPlayerId(const FUniqueNetIdRepl &InPlayerId);
    bool OnFlushBufferedRecentPlayers(float DeltaSeconds);
    void OnAddRecentPlayersComplete(const FUniqueNetId &UserId, const FOnlineError &Error, int32 ReportCount);

public:
    virtual void Initialize(FSubsystemCollectionBase &Collection) override;
};

EOS_DISABLE_STRICT_WARNINGS
