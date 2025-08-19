// Copyright June Rhodes. All Rights Reserved.

#include "./GameplayDebuggerCategory_P2PConnections.h"

#if WITH_GAMEPLAY_DEBUGGER

#include "Engine/Canvas.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/Full/EOSSocketRole.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/Full/SocketSubsystemEOSFull.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "OnlineSubsystemUtils.h"

EOS_ENABLE_STRICT_WARNINGS

FGameplayDebuggerCategory_P2PConnections::FGameplayDebuggerCategory_P2PConnections()
{
    this->bShowOnlyWithDebugActor = false;

    SetDataPackReplication<FRepData>(&DataPack);
}

void FGameplayDebuggerCategory_P2PConnections::CollectData(APlayerController *OwnerPC, AActor *DebugActor)
{
}

bool FGameplayDebuggerCategory_P2PConnections::PerformNATQuery(float DeltaSeconds, TWeakObjectPtr<UWorld> World)
{
    if (this->bIsQueryingNAT)
    {
        return false;
    }
    if (!World.Get())
    {
        this->bHasRegisteredPeriodicNATCheck = false;
        return false;
    }
    auto Subsystem = (FOnlineSubsystemEOS *)Online::GetSubsystem(World.Get(), REDPOINT_EOS_SUBSYSTEM);
    if (Subsystem == nullptr)
    {
        this->bHasRegisteredPeriodicNATCheck = false;
        return false;
    }
    auto SocketSubsystem = StaticCastSharedPtr<FSocketSubsystemEOSFull>(Subsystem->SocketSubsystem);
    if (!SocketSubsystem.IsValid())
    {
        this->bHasRegisteredPeriodicNATCheck = false;
        return false;
    }

    this->bIsQueryingNAT = true;
    EOS_HP2P P2P = EOS_Platform_GetP2PInterface(Subsystem->GetPlatformInstance());
    EOS_P2P_QueryNATTypeOptions Opts = {};
    Opts.ApiVersion = EOS_P2P_QUERYNATTYPE_API_LATEST;
    EOSRunOperation<EOS_HP2P, EOS_P2P_QueryNATTypeOptions, EOS_P2P_OnQueryNATTypeCompleteInfo>(
        P2P,
        &Opts,
        &EOS_P2P_QueryNATType,
        [World, P2P, WeakThis = GetWeakThis(this)](const EOS_P2P_OnQueryNATTypeCompleteInfo *Info) {
            if (auto This = StaticCastSharedPtr<FGameplayDebuggerCategory_P2PConnections>(PinWeakThis(WeakThis)))
            {
                This->bIsQueryingNAT = false;
                if (World.IsValid())
                {
                    EOS_P2P_GetNATTypeOptions GetOpts = {};
                    GetOpts.ApiVersion = EOS_P2P_GETNATTYPE_API_LATEST;
                    if (EOS_P2P_GetNATType(P2P, &GetOpts, &This->QueriedNATType) != EOS_EResult::EOS_Success)
                    {
                        This->QueriedNATType = EOS_ENATType::EOS_NAT_Unknown;
                    }

                    This->NextNATCheck = FUTicker::GetCoreTicker().AddTicker(
                        FTickerDelegate::CreateSP(
                            This.ToSharedRef(),
                            &FGameplayDebuggerCategory_P2PConnections::PerformNATQuery,
                            World),
                        60.0f);
                }
                else
                {
                    This->bHasRegisteredPeriodicNATCheck = false;
                }
            }
        });
    return false;
}

void FGameplayDebuggerCategory_P2PConnections::DrawData(
    APlayerController *OwnerPC,
    FGameplayDebuggerCanvasContext &CanvasContext)
{
    auto Actor = CanvasContext.Canvas->SceneView->ViewActor;
    if (!IsValid(Actor))
    {
        CanvasContext.Printf(TEXT("{red}No view actor\n"));
        return;
    }

    auto World = Actor->GetWorld();
    if (!IsValid(World))
    {
        CanvasContext.Printf(TEXT("{red}No world\n"));
        return;
    }

    auto Subsystem = (FOnlineSubsystemEOS *)Online::GetSubsystem(World, REDPOINT_EOS_SUBSYSTEM);
    if (Subsystem == nullptr)
    {
        CanvasContext.Printf(TEXT("{red}No EOS subsystem available\n"));
        return;
    }

    auto SocketSubsystem = StaticCastSharedPtr<FSocketSubsystemEOSFull>(Subsystem->SocketSubsystem);
    if (!SocketSubsystem.IsValid())
    {
        CanvasContext.Printf(TEXT("{red}No socket subsystem available\n"));
        return;
    }

    EOS_HP2P P2P = EOS_Platform_GetP2PInterface(Subsystem->GetPlatformInstance());

    if (!this->bHasRegisteredPeriodicNATCheck)
    {
        this->NextNATCheck = FUTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateSP(
                this,
                &FGameplayDebuggerCategory_P2PConnections::PerformNATQuery,
                TWeakObjectPtr<UWorld>(World)),
            60.0f);
        this->bHasRegisteredPeriodicNATCheck = true;

        // Immediately kick off a check.
        this->PerformNATQuery(0.0f, World);
    }

    CanvasContext.Printf(TEXT("{cyan}[NAT type in SDK]{white}"));
    if (this->bIsQueryingNAT)
    {
        CanvasContext.Printf(TEXT("NAT type: Querying..."));
    }
    else
    {
        switch (this->QueriedNATType)
        {
        case EOS_ENATType::EOS_NAT_Moderate:
            CanvasContext.Printf(TEXT("NAT type: {orange}Moderate{white}"));
            break;
        case EOS_ENATType::EOS_NAT_Open:
            CanvasContext.Printf(TEXT("NAT type: {green}Open{white}"));
            break;
        case EOS_ENATType::EOS_NAT_Strict:
            CanvasContext.Printf(TEXT("NAT type: {red}Strict{white}"));
            break;
        case EOS_ENATType::EOS_NAT_Unknown:
        default:
            CanvasContext.Printf(TEXT("NAT type: Unknown"));
            break;
        }
    }

    EOS_P2P_GetRelayControlOptions RelayOpts = {};
    RelayOpts.ApiVersion = EOS_P2P_GETRELAYCONTROL_API_LATEST;
    EOS_ERelayControl RelayControl;
    if (EOS_P2P_GetRelayControl(P2P, &RelayOpts, &RelayControl) == EOS_EResult::EOS_Success)
    {
        CanvasContext.Printf(TEXT("{cyan}[Relay control in SDK]{white}"));
        switch (RelayControl)
        {
        case EOS_ERelayControl::EOS_RC_AllowRelays:
            CanvasContext.Printf(TEXT("Status: Allow relays"));
            break;
        case EOS_ERelayControl::EOS_RC_ForceRelays:
            CanvasContext.Printf(TEXT("Status: {red}Force relays{white}"));
            break;
        case EOS_ERelayControl::EOS_RC_NoRelays:
            CanvasContext.Printf(TEXT("Status: {red}No relays{white}"));
            break;
        }
    }

    EOS_P2P_GetPacketQueueInfoOptions Opts = {};
    EOS_P2P_PacketQueueInfo Info = {};
    Opts.ApiVersion = EOS_P2P_GETPACKETQUEUEINFO_API_LATEST;
    if (EOS_P2P_GetPacketQueueInfo(P2P, &Opts, &Info) == EOS_EResult::EOS_Success)
    {
        CanvasContext.Printf(TEXT("{cyan}[Packet queue status in SDK]{white}"));
        CanvasContext.Printf(
            TEXT("Incoming packet queue: Packet count: %llu, Size in bytes: %llu/%llu"),
            Info.IncomingPacketQueueCurrentPacketCount,
            Info.IncomingPacketQueueCurrentSizeBytes,
            Info.IncomingPacketQueueMaxSizeBytes);
        CanvasContext.Printf(
            TEXT("Outgoing packet queue: Packet count: %llu, Size in bytes: %llu/%llu"),
            Info.OutgoingPacketQueueCurrentPacketCount,
            Info.OutgoingPacketQueueCurrentSizeBytes,
            Info.OutgoingPacketQueueMaxSizeBytes);
    }

    CanvasContext.Printf(TEXT("{cyan}[Plugin sockets (%d)]{white}"), SocketSubsystem->HeldSockets.Num());
    for (const auto &KV : SocketSubsystem->HeldSockets)
    {
        TArray<FString> Tags;

        Tags.Add(KV.Value->Role->GetRoleName().ToString());

        if (KV.Value->BindAddress.IsValid())
        {
            Tags.Add(FString::Printf(TEXT("BA: {orange}%s{white}"), *KV.Value->BindAddress->ToString(true)));
        }
        if (KV.Value->SocketKey.IsValid())
        {
            Tags.Add(FString::Printf(TEXT("K: {orange}%s{white}"), *KV.Value->SocketKey->ToString(false)));
        }
        {
            FString Threshold = "green";
            if (KV.Value->ReceivedPacketsQueueCount > 5)
            {
                Threshold = "orange";
            }
            if (KV.Value->ReceivedPacketsQueueCount > 20)
            {
                Threshold = "red";
            }
            Tags.Add(FString::Printf(TEXT("PPRQ: {%s}%d{white}"), *Threshold, KV.Value->ReceivedPacketsQueueCount));
        }

        CanvasContext.Printf(TEXT("{white}%d: %s"), KV.Key, *FString::Join(Tags, TEXT(", ")));
    }
}

TSharedRef<FGameplayDebuggerCategory> FGameplayDebuggerCategory_P2PConnections::MakeInstance()
{
    return MakeShareable(new FGameplayDebuggerCategory_P2PConnections());
}

void FGameplayDebuggerCategory_P2PConnections::FRepData::Serialize(FArchive &Ar)
{
}

EOS_DISABLE_STRICT_WARNINGS

#endif // WITH_GAMEPLAY_DEBUGGER
