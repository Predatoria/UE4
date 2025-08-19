// Copyright June Rhodes. All Rights Reserved.

#include "Containers/Ticker.h"
#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "Misc/Guid.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "RedpointEOSInterfaces/Private/Interfaces/OnlineLobbyInterface.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlineLobbyInterface_CanCreateSearchJoinAndDeleteLobby,
    "OnlineSubsystemEOS.OnlineLobbyInterface.CanCreateSearchJoinAndDeleteLobby",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

static void SearchUntilResultsAreFound(
    FOnlineSubsystemEOS_OnlineLobbyInterface_CanCreateSearchJoinAndDeleteLobby *Test,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FString GuidForSearch,
    const TWeakPtr<IOnlineLobby, ESPMode::ThreadSafe> &ClientLobbyInterface,
    const TSharedPtr<const FUniqueNetIdEOS> &ClientUserId,
    int AttemptCount,
    const FOnLobbySearchComplete &OnSearchComplete)
{
    FOnlineLobbySearchQuery Query;
    Query.Filters.Add(FOnlineLobbySearchQueryFilter(
        TEXT("Guid"),
        FVariantData(GuidForSearch),
        EOnlineLobbySearchQueryFilterComparator::Equal));
    Query.Limit = 1;

    if (auto ClientLobbyInterfacePinned = ClientLobbyInterface.Pin())
    {
        bool bSearchStarted = ClientLobbyInterfacePinned->Search(
            *ClientUserId,
            Query,
            FOnLobbySearchComplete::CreateLambda([=](const FOnlineError &Error,
                                                     const FUniqueNetId &UserId,
                                                     const TArray<TSharedRef<const FOnlineLobbyId>> &Lobbies) {
                if (Error.bSucceeded && Lobbies.Num() == 0 && AttemptCount < 10)
                {
                    FUTicker::GetCoreTicker().AddTicker(
                        FTickerDelegate::CreateLambda([=](float DeltaSeconds) {
                            SearchUntilResultsAreFound(
                                Test,
                                GuidForSearch,
                                ClientLobbyInterface,
                                ClientUserId,
                                AttemptCount + 1,
                                OnSearchComplete);
                            return false;
                        }),
                        5.0f);
                }
                else
                {
                    OnSearchComplete.ExecuteIfBound(Error, UserId, Lobbies);
                }
            }));
        if (!bSearchStarted)
        {
            OnSearchComplete.ExecuteIfBound(
                FOnlineError(false),
                *ClientUserId,
                TArray<TSharedRef<const FOnlineLobbyId>>());
        }
        return;
    }

    OnSearchComplete.ExecuteIfBound(FOnlineError(false), *ClientUserId, TArray<TSharedRef<const FOnlineLobbyId>>());
}

void FOnlineSubsystemEOS_OnlineLobbyInterface_CanCreateSearchJoinAndDeleteLobby::RunAsyncTest(
    const std::function<void()> &OnDone)
{
    CreateSubsystemsForTest_CreateOnDemand(
        this,
        2,
        OnDone,
        [this](const TArray<FMultiplayerScenarioInstance> &Instances, const FOnDone &OnDone) {
            auto Host = Instances[0];
            auto Client = Instances[1];

            auto HostLobbyInterface = Online::GetLobbyInterface(Host.Subsystem.Pin().Get());
            TestTrue("Host online subsystem provides IOnlineLobby interface", HostLobbyInterface.IsValid());

            auto ClientLobbyInterface = Online::GetLobbyInterface(Client.Subsystem.Pin().Get());
            TestTrue("Client online subsystem provides IOnlineLobby interface", ClientLobbyInterface.IsValid());

            TSharedPtr<FOnlineLobbyTransaction> Txn = HostLobbyInterface->MakeCreateLobbyTransaction(*Host.UserId);
            TestTrue("CreateLobbyTransaction should return new transaction", Txn.IsValid());

            FString Guid = FGuid::NewGuid().ToString();

            Txn->Capacity = 4;
            Txn->Locked = true;
            Txn->Public = true;
            Txn->SetMetadata.Add(TEXT("Guid"), FVariantData(Guid));

            bool bCreateStarted = HostLobbyInterface->CreateLobby(
                *Host.UserId,
                *Txn,
                FOnLobbyCreateOrConnectComplete::CreateLambda([=](const FOnlineError &Error,
                                                                  const FUniqueNetId &UserId,
                                                                  const TSharedPtr<class FOnlineLobby> &Lobby) {
                    TestTrue(
                        FString::Printf(TEXT("CreateLobby call should succeed: %s"), *Error.ErrorRaw),
                        Error.bSucceeded);
                    if (!Error.bSucceeded)
                    {
                        OnDone();
                        return;
                    }

                    FOnlineLobbySearchQuery Query;
                    Query.Filters.Add(FOnlineLobbySearchQueryFilter(
                        TEXT("Guid"),
                        FVariantData(Guid),
                        EOnlineLobbySearchQueryFilterComparator::Equal));
                    Query.Limit = 1;

                    SearchUntilResultsAreFound(
                        this,
                        Guid,
                        // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                        ClientLobbyInterface,
                        Client.UserId,
                        0,
                        FOnLobbySearchComplete::CreateLambda([=](const FOnlineError &Error,
                                                                 const FUniqueNetId &UserId,
                                                                 const TArray<TSharedRef<const FOnlineLobbyId>>
                                                                     &Lobbies) {
                            TestTrue(
                                FString::Printf(TEXT("Search call should succeed: %s"), *Error.ErrorRaw),
                                Error.bSucceeded);
                            if (!Error.bSucceeded)
                            {
                                OnDone();
                                return;
                            }

                            if (!TestEqual("Search should return exactly one result", Lobbies.Num(), 1))
                            {
                                OnDone();
                                return;
                            }

                            // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                            bool bConnectStarted = ClientLobbyInterface->ConnectLobby(
                                *Client.UserId,
                                *Lobbies[0],
                                FOnLobbyCreateOrConnectComplete::CreateLambda(
                                    [=](const FOnlineError &ClientError,
                                        const FUniqueNetId &ClientUserId,
                                        const TSharedPtr<class FOnlineLobby> &ClientLobby) {
                                        TestTrue(
                                            FString::Printf(
                                                TEXT("ConnectLobby call should succeed: %s"),
                                                *ClientError.ErrorRaw),
                                            ClientError.bSucceeded);
                                        if (!ClientError.bSucceeded)
                                        {
                                            OnDone();
                                            return;
                                        }

                                        FVariantData GuidValueFromLobby;
                                        // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                                        bool bGuidFound = ClientLobbyInterface->GetLobbyMetadataValue(
                                            *Client.UserId,
                                            *ClientLobby->Id,
                                            TEXT("Guid"),
                                            GuidValueFromLobby);
                                        TestTrue("Guid attribute was found on lobby", bGuidFound);
                                        TestTrue(
                                            "Guid attribute was string type",
                                            GuidValueFromLobby.GetType() == EOnlineKeyValuePairDataType::String);
                                        FString GuidValueFromLobbyStr;
                                        GuidValueFromLobby.GetValue(GuidValueFromLobbyStr);
                                        TestEqual("Guid attribute matches value", GuidValueFromLobbyStr, Guid);

                                        bool bDisconnectStarted = ClientLobbyInterface->DisconnectLobby(
                                            *Client.UserId,
                                            *ClientLobby->Id,
                                            FOnLobbyOperationComplete::CreateLambda(
                                                [=](const FOnlineError &ClientError, const FUniqueNetId &ClientUserId) {
                                                    TestTrue(
                                                        FString::Printf(
                                                            TEXT("ConnectLobby call should succeed: %s"),
                                                            *ClientError.ErrorRaw),
                                                        ClientError.bSucceeded);
                                                    if (!ClientError.bSucceeded)
                                                    {
                                                        OnDone();
                                                        return;
                                                    }

                                                    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                                                    bool bDeleteStarted = HostLobbyInterface->DeleteLobby(
                                                        *Host.UserId,
                                                        *ClientLobby->Id,
                                                        FOnLobbyOperationComplete::CreateLambda(
                                                            [=](const FOnlineError &Error, const FUniqueNetId &UserId) {
                                                                TestTrue(
                                                                    FString::Printf(
                                                                        TEXT("DeleteLobby call should succeed: %s"),
                                                                        *Error.ErrorRaw),
                                                                    Error.bSucceeded);

                                                                OnDone();
                                                            }));
                                                    if (!TestTrue("DeleteLobby should start", bDeleteStarted))
                                                    {
                                                        OnDone();
                                                    }
                                                }));
                                        if (!TestTrue("DisconnectLobby should start", bDisconnectStarted))
                                        {
                                            OnDone();
                                        }
                                    }));
                            if (!TestTrue("ConnectLobby should start", bConnectStarted))
                            {
                                OnDone();
                            }
                        }));
                }));
            TestTrue("CreateLobby should start", bCreateStarted);
            if (!bCreateStarted)
            {
                OnDone();
            }
        });
}