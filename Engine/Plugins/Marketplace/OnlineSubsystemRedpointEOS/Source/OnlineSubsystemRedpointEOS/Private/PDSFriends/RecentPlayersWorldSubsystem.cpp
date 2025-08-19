// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/PDSFriends/RecentPlayersWorldSubsystem.h"

#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"
#include "OnlineSubsystemUtils.h"

void URecentPlayersWorldSubsystem::OnActorSpawned(AActor *NewActor)
{
    APlayerState *NewPlayerState = Cast<APlayerState>(NewActor);
    if (IsValid(NewPlayerState))
    {
        FUniqueNetIdRepl PlayerStateId = NewPlayerState->GetUniqueId();
        if (PlayerStateId.IsValid())
        {
            this->DiscoveredPlayerId(PlayerStateId);
        }
        else
        {
            // Delay until the player state has a unique net ID set.
            FUTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(
                this,
                &URecentPlayersWorldSubsystem::OnDelayedPlayerStartEvaluation,
                TSoftObjectPtr<APlayerState>(NewActor)));
        }
    }
}

bool URecentPlayersWorldSubsystem::OnDelayedPlayerStartEvaluation(
    float DeltaSeconds,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSoftObjectPtr<APlayerState> InPlayerState)
{
    if (!InPlayerState.IsValid())
    {
        // Player state no longer valid.
        return false;
    }

    FUniqueNetIdRepl PlayerStateId = InPlayerState->GetUniqueId();
    if (PlayerStateId.IsValid())
    {
        this->DiscoveredPlayerId(PlayerStateId);
        return false;
    }

    // Need to try again later.
    return true;
}

void URecentPlayersWorldSubsystem::DiscoveredPlayerId(const FUniqueNetIdRepl &InPlayerId)
{
    // Figure out the online subsystem for the world.
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    if (Subsystem == nullptr)
    {
        UE_LOG(LogEOSFriends, Error, TEXT("No online subsystem is present, so recent players can not be tracked."));
        return;
    }
    if (Subsystem->GetSubsystemName() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(
            LogEOSFriends,
            Error,
            TEXT(
                "The current online subsystem is not the EOS online subsystem, so recent players can not be tracked."));
        return;
    }

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn &&
        StaticCastSharedPtr<const FUniqueNetIdEOS>(Identity->GetUniquePlayerId(0))->IsDedicatedServer())
    {
        // We don't track recent players on dedicated servers.
        return;
    }

    IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();
    if (!Friends.IsValid())
    {
        UE_LOG(
            LogEOSFriends,
            Error,
            TEXT("The friends interface is not available, so recent players can not be tracked."));
        return;
    }

    // Forcibly convert the player ID into an EOS player ID, in case it's a string version.
    auto UserId = StaticCastSharedPtr<const FUniqueNetIdEOS>(Identity->CreateUniquePlayerId(InPlayerId->ToString()));

    // Buffer the recent players, and start the buffer flush timer if it isn't
    // already counting down.
    this->BufferedRecentPlayers.Add(*UserId, FReportPlayedWithUser(UserId.ToSharedRef(), TEXT("")));
    if (!this->BufferFlushDelegate.IsValid())
    {
        this->BufferFlushDelegate = FUTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateUObject(this, &URecentPlayersWorldSubsystem::OnFlushBufferedRecentPlayers),
            10.0f);
    }
}

bool URecentPlayersWorldSubsystem::OnFlushBufferedRecentPlayers(float DeltaSeconds)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    if (Subsystem == nullptr)
    {
        UE_LOG(
            LogEOSFriends,
            Warning,
            TEXT("Required services were not available during OnFlushBufferedRecentPlayers."));
        this->BufferFlushDelegate.Reset();
        return false;
    }
    if (Subsystem->GetSubsystemName() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(
            LogEOSFriends,
            Warning,
            TEXT("Required services were not available during OnFlushBufferedRecentPlayers."));
        this->BufferFlushDelegate.Reset();
        return false;
    }

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();
    if (!Identity.IsValid() || !Friends.IsValid())
    {
        UE_LOG(
            LogEOSFriends,
            Warning,
            TEXT("Required services were not available during OnFlushBufferedRecentPlayers."));
        this->BufferFlushDelegate.Reset();
        return false;
    }

    for (const auto &Acc : Identity->GetAllUserAccounts())
    {
        TArray<FReportPlayedWithUser> Reports;
        for (const auto &KV : this->BufferedRecentPlayers)
        {
            if (*KV.Key != *Acc->GetUserId())
            {
                Reports.Add(KV.Value);
            }
        }
        if (Reports.Num() == 0)
        {
            continue;
        }

        Friends->AddRecentPlayers(
            *Acc->GetUserId(),
            Reports,
            TEXT(""),
            FOnAddRecentPlayersComplete::CreateUObject(
                this,
                &URecentPlayersWorldSubsystem::OnAddRecentPlayersComplete,
                Reports.Num()));
    }

    this->BufferedRecentPlayers.Reset();
    this->BufferFlushDelegate.Reset();
    return false;
}

void URecentPlayersWorldSubsystem::OnAddRecentPlayersComplete(
    const FUniqueNetId &UserId,
    const FOnlineError &Error,
    int32 ReportCount)
{
    if (Error.bSucceeded)
    {
        UE_LOG(
            LogEOSFriends,
            Verbose,
            TEXT("Successfully flushed %d recent players for %s."),
            ReportCount,
            *UserId.ToString());
    }
    else
    {
        UE_LOG(
            LogEOSFriends,
            Error,
            TEXT("Unable to flush %d recent players for %s."),
            ReportCount,
            *UserId.ToString());
    }
}

void URecentPlayersWorldSubsystem::Initialize(FSubsystemCollectionBase &Collection)
{
    this->GetWorld()->AddOnActorSpawnedHandler(
        FOnActorSpawned::FDelegate::CreateUObject(this, &URecentPlayersWorldSubsystem::OnActorSpawned));
}