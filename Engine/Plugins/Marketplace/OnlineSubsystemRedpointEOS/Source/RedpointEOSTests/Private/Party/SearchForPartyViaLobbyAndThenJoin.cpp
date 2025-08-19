// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "./TestPartyManager.h"
#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/HeapLambda.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlinePartyInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "RedpointEOSInterfaces/Private/Interfaces/OnlineLobbyInterface.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_MANAGED_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlinePartyInterface_SearchForPartyViaLobbyAndThenJoin,
    "OnlineSubsystemEOS.OnlinePartyInterface.SearchForPartyViaLobbyAndThenJoin",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter,
    FOnlineSubsystemEOS_OnlinePartyInterface_SearchForPartyViaLobbyAndThenJoin_Manager);

class FOnlineSubsystemEOS_OnlinePartyInterface_SearchForPartyViaLobbyAndThenJoin_Manager
    : public TSharedFromThis<FOnlineSubsystemEOS_OnlinePartyInterface_SearchForPartyViaLobbyAndThenJoin_Manager>,
      public FSingleTestPartyManager
{
public:
    UE_NONCOPYABLE(FOnlineSubsystemEOS_OnlinePartyInterface_SearchForPartyViaLobbyAndThenJoin_Manager);
    virtual ~FOnlineSubsystemEOS_OnlinePartyInterface_SearchForPartyViaLobbyAndThenJoin_Manager() = default;

    typedef FOnlineSubsystemEOS_OnlinePartyInterface_SearchForPartyViaLobbyAndThenJoin_Manager TThisClass;

    class FOnlineSubsystemEOS_OnlinePartyInterface_SearchForPartyViaLobbyAndThenJoin *T;
    FMultiplayerScenarioInstance Host;
    FMultiplayerScenarioInstance Client;
    TWeakPtr<IOnlinePartySystem, ESPMode::ThreadSafe> HostPartySystemWk;
    TWeakPtr<IOnlineLobby, ESPMode::ThreadSafe> ClientLobbyWk;
    TWeakPtr<IOnlinePartySystem, ESPMode::ThreadSafe> ClientPartySystemWk;
    std::function<void()> OnDone;

    FDelegateHandle PartyDataReceivedHandle;
    FString HostGuid;

    void Start_CreatePartyOnHost();
    void Handle_CreatePartyOnHost(
        const FUniqueNetId &LocalUserId,
        const TSharedPtr<const FOnlinePartyId> &PartyId,
        const ECreatePartyCompletionResult Result);

    void Start_SetPartyDataOnHost();
    void Handle_SetPartyDataOnHost(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FName &Namespace,
        const FOnlinePartyData &PartyData);

    void Start_FindPartyOnClient();
    void Handle_FindPartyOnClient(
        const FOnlineError &Error,
        const FUniqueNetId &UserId,
        const TArray<TSharedRef<const FOnlineLobbyId>> &Lobbies);

    void Start_JoinPartyOnClient(const IOnlinePartyJoinInfoConstPtr &JoinInfo);
    void Handle_JoinPartyOnClient(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const EJoinPartyCompletionResult Result,
        const int32 NotApprovedReason);

    void Start();

    // NOLINTNEXTLINE(unreal-field-not-initialized-in-constructor)
    FOnlineSubsystemEOS_OnlinePartyInterface_SearchForPartyViaLobbyAndThenJoin_Manager(
        class FOnlineSubsystemEOS_OnlinePartyInterface_SearchForPartyViaLobbyAndThenJoin *InT,
        FMultiplayerScenarioInstance InHost,
        FMultiplayerScenarioInstance InClient,
        std::function<void()> InOnDone)
    {
        this->T = InT;
        this->Host = MoveTemp(InHost);
        this->Client = MoveTemp(InClient);
        this->OnDone = MoveTemp(InOnDone);

        this->HostPartySystemWk = this->Host.Subsystem.Pin()->GetPartyInterface();
        this->ClientPartySystemWk = this->Client.Subsystem.Pin()->GetPartyInterface();
        this->ClientLobbyWk = Online::GetLobbyInterface(this->Client.Subsystem.Pin().Get());
    }
};

void FOnlineSubsystemEOS_OnlinePartyInterface_SearchForPartyViaLobbyAndThenJoin_Manager::Start_CreatePartyOnHost()
{
    FOnlinePartyTypeId PartyTypeId = FOnlinePartyTypeId(1);

    FPartyConfiguration HostConfig;
    HostConfig.MaxMembers = 4;
    HostConfig.InvitePermissions = PartySystemPermissions::EPermissionType::Anyone;
    HostConfig.PresencePermissions = PartySystemPermissions::EPermissionType::Anyone;
    HostConfig.bIsAcceptingMembers = true;
    HostConfig.bChatEnabled = false;

    UE_LOG(LogEOSTests, Log, TEXT("Creating party"));
    if (!T->TestTrue(
            TEXT("CreateParty operation started"),
            this->HostPartySystemWk.Pin()->CreateParty(
                *this->Host.UserId,
                PartyTypeId,
                HostConfig,
                FOnCreatePartyComplete::CreateSP(this, &TThisClass::Handle_CreatePartyOnHost))))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_SearchForPartyViaLobbyAndThenJoin_Manager::Handle_CreatePartyOnHost(
    const FUniqueNetId &LocalUserId,
    const TSharedPtr<const FOnlinePartyId> &PartyId,
    const ECreatePartyCompletionResult Result)
{
    if (!T->TestEqual(TEXT("Party was successfully created"), Result, ECreatePartyCompletionResult::Succeeded))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    this->AddParty(this->Host, *PartyId);

    this->Start_SetPartyDataOnHost();
}

void FOnlineSubsystemEOS_OnlinePartyInterface_SearchForPartyViaLobbyAndThenJoin_Manager::Start_SetPartyDataOnHost()
{
    if (auto HostPartySystem = this->HostPartySystemWk.Pin())
    {
        check(!this->PartyDataReceivedHandle.IsValid());
        this->PartyDataReceivedHandle = HostPartySystem->AddOnPartyDataReceivedDelegate_Handle(
            FOnPartyDataReceivedDelegate::CreateSP(this, &TThisClass::Handle_SetPartyDataOnHost));

        this->HostGuid = FGuid::NewGuid().ToString();

        auto Data = HostPartySystem->GetPartyData(*this->Host.UserId, *this->Party(*this->Host.UserId), NAME_Default);
        auto NewData = MakeShared<FOnlinePartyData>(*Data);
        NewData->SetAttribute(TEXT("SearchAttr"), FVariantData(this->HostGuid));

        if (!T->TestTrue(
                TEXT("Party set data was successful"),
                HostPartySystem
                    ->UpdatePartyData(*this->Host.UserId, *this->Party(*this->Host.UserId), NAME_Default, *NewData)))
        {
            this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
            return;
        }
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_SearchForPartyViaLobbyAndThenJoin_Manager::Handle_SetPartyDataOnHost(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FName &Namespace,
    const FOnlinePartyData &PartyData)
{
    if (auto HostPartySystem = this->HostPartySystemWk.Pin())
    {
        HostPartySystem->ClearOnPartyDataReceivedDelegate_Handle(this->PartyDataReceivedHandle);
        this->PartyDataReceivedHandle.Reset();

        this->Start_FindPartyOnClient();
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_SearchForPartyViaLobbyAndThenJoin_Manager::Start_FindPartyOnClient()
{
    if (auto ClientLobby = this->ClientLobbyWk.Pin())
    {
        auto Query = MakeShared<FOnlineLobbySearchQuery>();
        Query->Limit = 1;
        Query->Filters.Add(FOnlineLobbySearchQueryFilter(
            TEXT("SearchAttr"),
            FVariantData(this->HostGuid),
            EOnlineLobbySearchQueryFilterComparator::Equal));

        if (!T->TestTrue(
                "Lobby search started",
                ClientLobby->Search(
                    *this->Client.UserId,
                    *Query,
                    FOnLobbySearchComplete::CreateSP(this, &TThisClass::Handle_FindPartyOnClient))))
        {
            this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
            return;
        }
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_SearchForPartyViaLobbyAndThenJoin_Manager::Handle_FindPartyOnClient(
    const FOnlineError &Error,
    const FUniqueNetId &UserId,
    const TArray<TSharedRef<const FOnlineLobbyId>> &Lobbies)
{
    if (auto ClientPartySystem = this->ClientPartySystemWk.Pin())
    {
        if (!T->TestTrue("Lobby search succeeded", Error.bSucceeded))
        {
            this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
            return;
        }

        if (Lobbies.Num() != 1)
        {
            // We have to retry.
            // @todo: Cap this with a timeout.
            this->Start_FindPartyOnClient();
            return;
        }

        auto JoinInfoJson = ClientPartySystem->MakeJoinInfoJson(UserId, Lobbies[0].Get());
        if (!T->TestFalse("Join info JSON is not empty", JoinInfoJson.IsEmpty()))
        {
            this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
            return;
        }

        auto JoinInfo = ClientPartySystem->MakeJoinInfoFromJson(JoinInfoJson);
        if (!T->TestTrue("Join info is valid", JoinInfo.IsValid()))
        {
            this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
            return;
        }

        this->Start_JoinPartyOnClient(JoinInfo);
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_SearchForPartyViaLobbyAndThenJoin_Manager::Start_JoinPartyOnClient(
    const IOnlinePartyJoinInfoConstPtr &JoinInfo)
{
    if (auto ClientPartySystem = this->ClientPartySystemWk.Pin())
    {
        if (!T->TestTrue(
                "Join party operation started",
                ClientPartySystem->JoinParty(
                    *this->Client.UserId,
                    *JoinInfo,
                    FOnJoinPartyComplete::CreateSP(this, &TThisClass::Handle_JoinPartyOnClient))))
        {
            this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
            return;
        }
    }
}

void FOnlineSubsystemEOS_OnlinePartyInterface_SearchForPartyViaLobbyAndThenJoin_Manager::Handle_JoinPartyOnClient(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const EJoinPartyCompletionResult Result,
    const int32 NotApprovedReason)
{
    if (!T->TestTrue("Party joined successfully", Result == EJoinPartyCompletionResult::Succeeded))
    {
        this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
        return;
    }

    // Test finished.
    this->CleanupPartiesAndThenCallOnDone(this->AsShared(), this->OnDone);
}

void FOnlineSubsystemEOS_OnlinePartyInterface_SearchForPartyViaLobbyAndThenJoin_Manager::Start()
{
    this->Start_CreatePartyOnHost();
}

void FOnlineSubsystemEOS_OnlinePartyInterface_SearchForPartyViaLobbyAndThenJoin::CreateManagingInstance(
    const FOnDone &OnDone,
    const std::function<
        void(const TSharedRef<FOnlineSubsystemEOS_OnlinePartyInterface_SearchForPartyViaLobbyAndThenJoin_Manager> &)>
        &OnInstanceCreated)
{
    CreateSubsystemsForTest_CreateOnDemand(
        this,
        2,
        OnDone,
        [this, OnInstanceCreated](TArray<FMultiplayerScenarioInstance> Instances, const FOnDone &OnDone) {
            auto Instance =
                MakeShared<FOnlineSubsystemEOS_OnlinePartyInterface_SearchForPartyViaLobbyAndThenJoin_Manager>(
                    this,
                    Instances[0],
                    Instances[1],
                    OnDone);
            OnInstanceCreated(Instance);
            Instance->Start();
        });
}

#endif // #if EOS_HAS_AUTHENTICATION
