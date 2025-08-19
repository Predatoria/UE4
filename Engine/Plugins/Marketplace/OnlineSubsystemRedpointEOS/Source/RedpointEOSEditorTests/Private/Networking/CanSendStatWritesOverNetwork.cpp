// Copyright June Rhodes. All Rights Reserved.

#include "EOSEditorCommands.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemUtils.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

struct FStatWriteOperationInfo
{
public:
    FStatWriteOperationInfo()
        : bOperationStarted(false)
        , bOperationComplete(false){};

    bool bOperationStarted;
    bool bOperationComplete;
};

// NOLINTNEXTLINE(performance-unnecessary-value-param)
DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FDoStatWriteOperation, TSharedPtr<FStatWriteOperationInfo>, Info);

bool FDoStatWriteOperation::Update()
{
    if (!this->Info->bOperationStarted)
    {
        APlayerController *HostController = nullptr;
        APlayerController *ClientController = nullptr;

        for (const FWorldContext &WorldContext : GEngine->GetWorldContexts())
        {
            if (WorldContext.WorldType == EWorldType::PIE && WorldContext.PIEInstance == 0 &&
                WorldContext.World() != nullptr)
            {
                for (TActorIterator<APlayerController> It(WorldContext.World()); It; ++It)
                {
                    if (WorldContext.PIEInstance == 0 && It->GetName() == TEXT("PlayerController_0") &&
                        It->GetLocalRole() == ENetRole::ROLE_Authority)
                    {
                        HostController = *It;
                    }

                    if (WorldContext.PIEInstance == 0 && It->GetName() == TEXT("PlayerController_1") &&
                        It->GetLocalRole() == ENetRole::ROLE_Authority)
                    {
                        ClientController = *It;
                    }

                    if (IsValid(HostController) && IsValid(ClientController))
                    {
                        break;
                    }
                }
            }

            if (IsValid(HostController) && IsValid(ClientController))
            {
                break;
            }
        }

        if (IsValid(HostController) && IsValid(ClientController))
        {
            this->Info->bOperationStarted = true;

            auto OSS = Online::GetSubsystem(HostController->GetWorld());

            TSharedPtr<const FUniqueNetId> HostId = OSS->GetIdentityInterface()->CreateUniquePlayerId(
                *HostController->GetPlayerState<APlayerState>()->GetUniqueId().ToString());
            TSharedPtr<const FUniqueNetId> ClientId = OSS->GetIdentityInterface()->CreateUniquePlayerId(
                *ClientController->GetPlayerState<APlayerState>()->GetUniqueId().ToString());

            TMap<FString, FOnlineStatUpdate> Stats;
            Stats.Add(
                TEXT("TestFloatEnc"),
                FOnlineStatUpdate(20.0f, FOnlineStatUpdate::EOnlineStatModificationType::Set));

            TArray<FOnlineStatsUserUpdatedStats> StatsList;
            StatsList.Add(FOnlineStatsUserUpdatedStats(ClientId.ToSharedRef(), Stats));

            OSS->GetStatsInterface()->UpdateStats(
                HostId.ToSharedRef(),
                StatsList,
                FOnlineStatsUpdateStatsComplete::CreateLambda([Info = this->Info](const FOnlineError &ResultState) {
                    // Give client time to perform operation.
                    FUTicker::GetCoreTicker().AddTicker(
                        FTickerDelegate::CreateLambda([Info](float DeltaSeconds) {
                            Info->bOperationComplete = true;
                            return false;
                        }),
                        10.0f);
                }));
        }
    }

    return this->Info->bOperationComplete;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FCanSendStatWritesOverNetwork,
    "OnlineSubsystemEOS.Networking.CanSendStatWritesOverNetwork",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);

bool FCanSendStatWritesOverNetwork::RunTest(const FString &InParam)
{
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FSetupAutomationConfig());
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FStartPlayingEmptyMap(
        this,
        EEditorAuthMode::CreateOnDemand,
        2,
        EPlayNetMode::PIE_ListenServer,
        TEXT("?NetMode=ForceP2P")));
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FWaitForConnectedPlayerController(this));

    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FDoStatWriteOperation(MakeShared<FStatWriteOperationInfo>()));

    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FEndPlayMapCommand());
    ADD_LATENT_AUTOMATION_COMMAND_PATCHED(FClearAutomationConfig());

    return true;
}
