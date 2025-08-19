// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if WITH_GAMEPLAY_DEBUGGER

#include "CoreMinimal.h"
#include "GameplayDebuggerCategory.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

class APlayerController;
class AActor;

class FGameplayDebuggerCategory_P2PConnections : public FGameplayDebuggerCategory
{
private:
    bool bHasRegisteredPeriodicNATCheck = false;
    bool bIsQueryingNAT = false;
    FUTickerDelegateHandle NextNATCheck;
    EOS_ENATType QueriedNATType = EOS_ENATType::EOS_NAT_Unknown;

    bool PerformNATQuery(float DeltaSeconds, TWeakObjectPtr<UWorld> World);

public:
    FGameplayDebuggerCategory_P2PConnections();
    ~FGameplayDebuggerCategory_P2PConnections() = default;
    UE_NONCOPYABLE(FGameplayDebuggerCategory_P2PConnections);

    void CollectData(APlayerController *OwnerPC, AActor *DebugActor) override;
    void DrawData(APlayerController *OwnerPC, FGameplayDebuggerCanvasContext &CanvasContext) override;

    static TSharedRef<FGameplayDebuggerCategory> MakeInstance();

protected:
    struct FRepData
    {
        void Serialize(FArchive &Ar);
    };

    FRepData DataPack;
};

EOS_DISABLE_STRICT_WARNINGS

#endif // WITH_GAMEPLAY_DEBUGGER
