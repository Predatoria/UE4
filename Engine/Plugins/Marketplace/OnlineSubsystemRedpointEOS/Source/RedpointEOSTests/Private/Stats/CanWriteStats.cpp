// Copyright June Rhodes. All Rights Reserved.

#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineStatsInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlineStatsInterface_CanWriteStats,
    "OnlineSubsystemEOS.OnlineStatsInterface.CanWriteStats",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_OnlineStatsInterface_CanWriteStats::RunAsyncTest(const std::function<void()> &OnDone)
{
    CreateSingleSubsystemForTest_CreateOnDemand(
        this,
        OnDone,
        [this](
            const IOnlineSubsystemPtr &Subsystem,
            const TSharedPtr<const FUniqueNetIdEOS> &UserId,
            const FOnDone &OnDone) {
            if (!(Subsystem.IsValid() && UserId.IsValid()))
            {
                this->AddError(FString::Printf(TEXT("Unable to init subsystem / authenticate")));
                OnDone();
                return;
            }

            auto Stats = Subsystem->GetStatsInterface();
            TestTrue("Online subsystem provides IOnlineStats interface", Stats.IsValid());

            if (!Stats.IsValid())
            {
                OnDone();
                return;
            }

            FOnlineStatsUserUpdatedStats StatToSend = FOnlineStatsUserUpdatedStats(UserId.ToSharedRef());
            StatToSend.Stats.Add(
                TEXT("TestLatest"),
                FOnlineStatUpdate(20, FOnlineStatUpdate::EOnlineStatModificationType::Unknown));
            StatToSend.Stats.Add(
                TEXT("TestScore"),
                FOnlineStatUpdate(10, FOnlineStatUpdate::EOnlineStatModificationType::Unknown));

            TArray<FOnlineStatsUserUpdatedStats> StatsToSend;
            StatsToSend.Add(StatToSend);

            Stats->UpdateStats(
                UserId.ToSharedRef(),
                StatsToSend,
                FOnlineStatsUpdateStatsComplete::CreateLambda([this, OnDone](const FOnlineError &ResultState) {
                    TestTrue("Can write stats update", ResultState.bSucceeded);
                    OnDone();
                }));
        });
}

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlineStatsInterface_CanWriteStatsFloatTruncated,
    "OnlineSubsystemEOS.OnlineStatsInterface.CanWriteStatsFloatTruncated",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_OnlineStatsInterface_CanWriteStatsFloatTruncated::RunAsyncTest(
    const std::function<void()> &OnDone)
{
    CreateSingleSubsystemForTest_CreateOnDemand(
        this,
        OnDone,
        [this](
            const IOnlineSubsystemPtr &Subsystem,
            const TSharedPtr<const FUniqueNetIdEOS> &UserId,
            const FOnDone &OnDone) {
            if (!(Subsystem.IsValid() && UserId.IsValid()))
            {
                this->AddError(FString::Printf(TEXT("Unable to init subsystem / authenticate")));
                OnDone();
                return;
            }

            auto Stats = Subsystem->GetStatsInterface();
            TestTrue("Online subsystem provides IOnlineStats interface", Stats.IsValid());

            if (!Stats.IsValid())
            {
                OnDone();
                return;
            }

            FOnlineStatsUserUpdatedStats StatToSend = FOnlineStatsUserUpdatedStats(UserId.ToSharedRef());
            StatToSend.Stats.Add(
                TEXT("TestFloatTrunc"),
                FOnlineStatUpdate(20.0f, FOnlineStatUpdate::EOnlineStatModificationType::Unknown));

            TArray<FOnlineStatsUserUpdatedStats> StatsToSend;
            StatsToSend.Add(StatToSend);

            Stats->UpdateStats(
                UserId.ToSharedRef(),
                StatsToSend,
                FOnlineStatsUpdateStatsComplete::CreateLambda([this, OnDone](const FOnlineError &ResultState) {
                    TestTrue("Can write stats update", ResultState.bSucceeded);
                    OnDone();
                }));
        });
}

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlineStatsInterface_CanWriteStatsFloatEncoded,
    "OnlineSubsystemEOS.OnlineStatsInterface.CanWriteStatsFloatEncoded",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_OnlineStatsInterface_CanWriteStatsFloatEncoded::RunAsyncTest(
    const std::function<void()> &OnDone)
{
    CreateSingleSubsystemForTest_CreateOnDemand(
        this,
        OnDone,
        [this](
            const IOnlineSubsystemPtr &Subsystem,
            const TSharedPtr<const FUniqueNetIdEOS> &UserId,
            const FOnDone &OnDone) {
            if (!(Subsystem.IsValid() && UserId.IsValid()))
            {
                this->AddError(FString::Printf(TEXT("Unable to init subsystem / authenticate")));
                OnDone();
                return;
            }

            auto Stats = Subsystem->GetStatsInterface();
            TestTrue("Online subsystem provides IOnlineStats interface", Stats.IsValid());

            if (!Stats.IsValid())
            {
                OnDone();
                return;
            }

            FOnlineStatsUserUpdatedStats StatToSend = FOnlineStatsUserUpdatedStats(UserId.ToSharedRef());
            StatToSend.Stats.Add(
                TEXT("TestFloatEnc"),
                FOnlineStatUpdate(20.0f, FOnlineStatUpdate::EOnlineStatModificationType::Unknown));

            TArray<FOnlineStatsUserUpdatedStats> StatsToSend;
            StatsToSend.Add(StatToSend);

            Stats->UpdateStats(
                UserId.ToSharedRef(),
                StatsToSend,
                FOnlineStatsUpdateStatsComplete::CreateLambda([this, OnDone](const FOnlineError &ResultState) {
                    TestTrue("Can write stats update", ResultState.bSucceeded);
                    OnDone();
                }));
        });
}

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlineStatsInterface_CanWriteStatsDoubleEncoded,
    "OnlineSubsystemEOS.OnlineStatsInterface.CanWriteStatsDoubleEncoded",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_OnlineStatsInterface_CanWriteStatsDoubleEncoded::RunAsyncTest(
    const std::function<void()> &OnDone)
{
    CreateSingleSubsystemForTest_CreateOnDemand(
        this,
        OnDone,
        [this](
            const IOnlineSubsystemPtr &Subsystem,
            const TSharedPtr<const FUniqueNetIdEOS> &UserId,
            const FOnDone &OnDone) {
            if (!(Subsystem.IsValid() && UserId.IsValid()))
            {
                this->AddError(FString::Printf(TEXT("Unable to init subsystem / authenticate")));
                OnDone();
                return;
            }

            auto Stats = Subsystem->GetStatsInterface();
            TestTrue("Online subsystem provides IOnlineStats interface", Stats.IsValid());

            if (!Stats.IsValid())
            {
                OnDone();
                return;
            }

            FOnlineStatsUserUpdatedStats StatToSend = FOnlineStatsUserUpdatedStats(UserId.ToSharedRef());
            StatToSend.Stats.Add(
                TEXT("TestDoubleEnc"),
                FOnlineStatUpdate(20.0, FOnlineStatUpdate::EOnlineStatModificationType::Unknown));

            TArray<FOnlineStatsUserUpdatedStats> StatsToSend;
            StatsToSend.Add(StatToSend);

            Stats->UpdateStats(
                UserId.ToSharedRef(),
                StatsToSend,
                FOnlineStatsUpdateStatsComplete::CreateLambda([this, OnDone](const FOnlineError &ResultState) {
                    TestTrue("Can write stats update", ResultState.bSucceeded);
                    OnDone();
                }));
        });
}