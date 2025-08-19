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

IMPLEMENT_MANAGED_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlineLobbyInterface_CanFindOwnedLobbyAfterJoiningNonExistantLobby,
    "OnlineSubsystemEOS.OnlineLobbyInterface.CanFindOwnedLobbyAfterJoiningNonExistantLobby",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter,
    FOnlineSubsystemEOS_OnlineLobbyInterface_CanFindOwnedLobbyAfterJoiningNonExistantLobby_Manager);

class FOnlineSubsystemEOS_OnlineLobbyInterface_CanFindOwnedLobbyAfterJoiningNonExistantLobby_Manager
    : public TSharedFromThis<
          FOnlineSubsystemEOS_OnlineLobbyInterface_CanFindOwnedLobbyAfterJoiningNonExistantLobby_Manager>
{
private:
    class FOnlineSubsystemEOS_OnlineLobbyInterface_CanFindOwnedLobbyAfterJoiningNonExistantLobby *T;
    FMultiplayerScenarioInstance Host;
    FMultiplayerScenarioInstance Client;
    FString HostGuid;
    FString ClientGuid;
    std::function<void()> OnDone;
    TSharedPtr<const FOnlineLobbyId> HostLobbyId;
    TSharedPtr<const FOnlineLobbyId> ClientLobbyId;
    TSharedPtr<const FOnlineLobbyId> LobbyIdClientWillTryToJoin;

    void SearchUntilResultsAreFound(
        FString GuidForSearch,
        const TWeakPtr<IOnlineLobby, ESPMode::ThreadSafe> &ClientLobbyInterface,
        const TSharedPtr<const FUniqueNetIdEOS> &ClientUserId,
        int AttemptCount,
        const FOnLobbySearchComplete &OnSearchComplete);

    void OnClientUpdatedLobby(const FOnlineError &Error, const FUniqueNetId &UserId);

    void HandleClientConnectComplete(
        const FOnlineError &Error,
        const FUniqueNetId &UserId,
        const TSharedPtr<class FOnlineLobby> &Lobby);

    void HostDeletedLobby(const FOnlineError &Error, const FUniqueNetId &UserId);

    void ClientHasFoundHostLobby(
        const FOnlineError &Error,
        const FUniqueNetId &UserId,
        const TArray<TSharedRef<const FOnlineLobbyId>> &Lobbies);

    void HandleClientCreateComplete(
        const FOnlineError &Error,
        const FUniqueNetId &UserId,
        const TSharedPtr<class FOnlineLobby> &Lobby);

    void HandleHostCreateComplete(
        const FOnlineError &Error,
        const FUniqueNetId &UserId,
        const TSharedPtr<class FOnlineLobby> &Lobby);

public:
    UE_NONCOPYABLE(FOnlineSubsystemEOS_OnlineLobbyInterface_CanFindOwnedLobbyAfterJoiningNonExistantLobby_Manager);
    virtual ~FOnlineSubsystemEOS_OnlineLobbyInterface_CanFindOwnedLobbyAfterJoiningNonExistantLobby_Manager() = default;

    typedef FOnlineSubsystemEOS_OnlineLobbyInterface_CanFindOwnedLobbyAfterJoiningNonExistantLobby_Manager TThisClass;

    void Start();

    // NOLINTNEXTLINE(unreal-field-not-initialized-in-constructor)
    FOnlineSubsystemEOS_OnlineLobbyInterface_CanFindOwnedLobbyAfterJoiningNonExistantLobby_Manager(
        class FOnlineSubsystemEOS_OnlineLobbyInterface_CanFindOwnedLobbyAfterJoiningNonExistantLobby *InT,
        FMultiplayerScenarioInstance InHost,
        FMultiplayerScenarioInstance InClient,
        std::function<void()> InOnDone)
    {
        this->T = InT;
        this->Host = MoveTemp(InHost);
        this->Client = MoveTemp(InClient);
        this->OnDone = MoveTemp(InOnDone);
    }
};

void FOnlineSubsystemEOS_OnlineLobbyInterface_CanFindOwnedLobbyAfterJoiningNonExistantLobby_Manager::
    SearchUntilResultsAreFound(
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
                            this->SearchUntilResultsAreFound(
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

void FOnlineSubsystemEOS_OnlineLobbyInterface_CanFindOwnedLobbyAfterJoiningNonExistantLobby_Manager::
    OnClientUpdatedLobby(const FOnlineError &Error, const FUniqueNetId &UserId)
{
    T->TestTrue(
        FString::Printf(TEXT("Client should be able to update own lobby: %s"), *Error.ErrorRaw),
        Error.bSucceeded);
    if (!Error.bSucceeded)
    {
        OnDone();
        return;
    }

    // We've finished the test.
    OnDone();
}

void FOnlineSubsystemEOS_OnlineLobbyInterface_CanFindOwnedLobbyAfterJoiningNonExistantLobby_Manager::
    HandleClientConnectComplete(
        const FOnlineError &Error,
        const FUniqueNetId &UserId,
        const TSharedPtr<class FOnlineLobby> &Lobby)
{
    if (!T->TestFalse(TEXT("Client should not be able to join deleted lobby"), Error.bSucceeded))
    {
        OnDone();
        return;
    }
    if (!T->TestTrue(
            TEXT("Client should fail join with EOS_NotFound"),
            Error.ErrorCode.EndsWith(".not_found") || Error.ErrorCode.EndsWith(".session_not_found")))
    {
        OnDone();
        return;
    }

    // Now try to update the lobby that the client owns. This should *not* fail.
    auto ClientLobbyInterface = Online::GetLobbyInterface(Client.Subsystem.Pin().Get());
    TSharedPtr<FOnlineLobbyTransaction> Txn =
        ClientLobbyInterface->MakeUpdateLobbyTransaction(*Client.UserId, *ClientLobbyId);
    Txn->SetMetadata.Add(TEXT("Test"), TEXT("Hello"));
    if (!T->TestTrue(
            TEXT("Client UpdateLobby operation should start"),
            ClientLobbyInterface->UpdateLobby(
                *Client.UserId,
                *ClientLobbyId,
                *Txn,
                FOnLobbyOperationComplete::CreateSP(this, &TThisClass::OnClientUpdatedLobby))))
    {
        OnDone();
    }
}

void FOnlineSubsystemEOS_OnlineLobbyInterface_CanFindOwnedLobbyAfterJoiningNonExistantLobby_Manager::HostDeletedLobby(
    const FOnlineError &Error,
    const FUniqueNetId &UserId)
{
    T->TestTrue(FString::Printf(TEXT("Host should be able to delete lobby: %s"), *Error.ErrorRaw), Error.bSucceeded);
    if (!Error.bSucceeded)
    {
        OnDone();
        return;
    }

    // Now try to join the lobby from the client. We expect this to fail.
    auto ClientLobbyInterface = Online::GetLobbyInterface(Client.Subsystem.Pin().Get());
    if (!T->TestTrue(
            TEXT("Client ConnectLobby operation should at least start..."),
            ClientLobbyInterface->ConnectLobby(
                *Client.UserId,
                *LobbyIdClientWillTryToJoin,
                FOnLobbyCreateOrConnectComplete::CreateSP(this, &TThisClass::HandleClientConnectComplete))))
    {
        OnDone();
    }
}

void FOnlineSubsystemEOS_OnlineLobbyInterface_CanFindOwnedLobbyAfterJoiningNonExistantLobby_Manager::
    ClientHasFoundHostLobby(
        const FOnlineError &Error,
        const FUniqueNetId &UserId,
        const TArray<TSharedRef<const FOnlineLobbyId>> &Lobbies)
{
    T->TestTrue(
        FString::Printf(TEXT("Client should be able to find host lobby: %s"), *Error.ErrorRaw),
        Error.bSucceeded);
    if (!Error.bSucceeded)
    {
        OnDone();
        return;
    }
    if (!T->TestFalse(TEXT("Client should be able to find at least one lobby"), Lobbies.Num() == 0))
    {
        OnDone();
        return;
    }

    LobbyIdClientWillTryToJoin = Lobbies[0];

    // Now delete the lobby on the host. The client will still have the handle.
    auto HostLobbyInterface = Online::GetLobbyInterface(Host.Subsystem.Pin().Get());
    if (!T->TestTrue(
            TEXT("Host DeleteLobby operation should start"),
            HostLobbyInterface->DeleteLobby(
                *Host.UserId,
                *HostLobbyId,
                FOnLobbyOperationComplete::CreateSP(this, &TThisClass::HostDeletedLobby))))
    {
        OnDone();
    }
}

void FOnlineSubsystemEOS_OnlineLobbyInterface_CanFindOwnedLobbyAfterJoiningNonExistantLobby_Manager::
    HandleClientCreateComplete(
        const FOnlineError &Error,
        const FUniqueNetId &UserId,
        const TSharedPtr<class FOnlineLobby> &Lobby)
{
    T->TestTrue(FString::Printf(TEXT("Client CreateLobby call should succeed: %s"), *Error.ErrorRaw), Error.bSucceeded);
    if (!Error.bSucceeded)
    {
        OnDone();
        return;
    }

    ClientLobbyId = Lobby->Id;

    auto ClientLobbyInterface = Online::GetLobbyInterface(Client.Subsystem.Pin().Get());
    this->SearchUntilResultsAreFound(
        HostGuid,
        ClientLobbyInterface,
        Client.UserId,
        0,
        FOnLobbySearchComplete::CreateSP(this, &TThisClass::ClientHasFoundHostLobby));
}

void FOnlineSubsystemEOS_OnlineLobbyInterface_CanFindOwnedLobbyAfterJoiningNonExistantLobby_Manager::
    HandleHostCreateComplete(
        const FOnlineError &Error,
        const FUniqueNetId &UserId,
        const TSharedPtr<class FOnlineLobby> &Lobby)
{
    T->TestTrue(FString::Printf(TEXT("Host CreateLobby call should succeed: %s"), *Error.ErrorRaw), Error.bSucceeded);
    if (!Error.bSucceeded)
    {
        OnDone();
        return;
    }

    HostLobbyId = Lobby->Id;

    auto ClientLobbyInterface = Online::GetLobbyInterface(Client.Subsystem.Pin().Get());
    T->TestTrue("Client online subsystem provides IOnlineLobby interface", ClientLobbyInterface.IsValid());

    // Create a lobby on the client.
    TSharedPtr<FOnlineLobbyTransaction> Txn = ClientLobbyInterface->MakeCreateLobbyTransaction(*Client.UserId);
    ClientGuid = FGuid::NewGuid().ToString();
    Txn->Capacity = 4;
    Txn->Locked = true;
    Txn->Public = true;
    Txn->SetMetadata.Add(TEXT("Guid"), FVariantData(ClientGuid));
    bool bCreateStarted = ClientLobbyInterface->CreateLobby(
        *Client.UserId,
        *Txn,
        FOnLobbyCreateOrConnectComplete::CreateSP(this, &TThisClass::HandleClientCreateComplete));
    T->TestTrue("Client CreateLobby should start", bCreateStarted);
    if (!bCreateStarted)
    {
        OnDone();
    }
}

void FOnlineSubsystemEOS_OnlineLobbyInterface_CanFindOwnedLobbyAfterJoiningNonExistantLobby_Manager::Start()
{
    auto HostLobbyInterface = Online::GetLobbyInterface(Host.Subsystem.Pin().Get());
    T->TestTrue("Host online subsystem provides IOnlineLobby interface", HostLobbyInterface.IsValid());

    // Create a lobby on the host.
    TSharedPtr<FOnlineLobbyTransaction> Txn = HostLobbyInterface->MakeCreateLobbyTransaction(*Host.UserId);
    HostGuid = FGuid::NewGuid().ToString();
    Txn->Capacity = 4;
    Txn->Locked = true;
    Txn->Public = true;
    Txn->SetMetadata.Add(TEXT("Guid"), FVariantData(HostGuid));
    bool bCreateStarted = HostLobbyInterface->CreateLobby(
        *Host.UserId,
        *Txn,
        FOnLobbyCreateOrConnectComplete::CreateSP(this, &TThisClass::HandleHostCreateComplete));
    T->TestTrue("Host CreateLobby should start", bCreateStarted);
    if (!bCreateStarted)
    {
        OnDone();
    }
}

void FOnlineSubsystemEOS_OnlineLobbyInterface_CanFindOwnedLobbyAfterJoiningNonExistantLobby::CreateManagingInstance(
    const FOnDone &OnDone,
    const std::function<void(
        const TSharedRef<FOnlineSubsystemEOS_OnlineLobbyInterface_CanFindOwnedLobbyAfterJoiningNonExistantLobby_Manager>
            &)> &OnInstanceCreated)
{
    CreateSubsystemsForTest_CreateOnDemand(
        this,
        2,
        OnDone,
        [this, OnInstanceCreated](TArray<FMultiplayerScenarioInstance> Instances, const FOnDone &OnDone) {
            auto Instance = MakeShared<
                FOnlineSubsystemEOS_OnlineLobbyInterface_CanFindOwnedLobbyAfterJoiningNonExistantLobby_Manager>(
                this,
                Instances[0],
                Instances[1],
                OnDone);
            OnInstanceCreated(Instance);
            Instance->Start();
        });
}
