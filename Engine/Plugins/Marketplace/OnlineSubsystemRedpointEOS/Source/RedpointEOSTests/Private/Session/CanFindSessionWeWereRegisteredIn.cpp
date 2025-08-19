// Copyright June Rhodes. All Rights Reserved.

#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/HeapLambda.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSessionInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_MANAGED_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlineSessionInterface_CanFindSessionWeWereRegisteredIn,
    "OnlineSubsystemEOS.OnlineSessionInterface.CanFindSessionWeWereRegisteredIn",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter,
    FOnlineSubsystemEOS_OnlineSessionInterface_CanFindSessionWeWereRegisteredIn_Manager);

class FOnlineSubsystemEOS_OnlineSessionInterface_CanFindSessionWeWereRegisteredIn_Manager
    : public TSharedFromThis<FOnlineSubsystemEOS_OnlineSessionInterface_CanFindSessionWeWereRegisteredIn_Manager>
{
public:
    UE_NONCOPYABLE(FOnlineSubsystemEOS_OnlineSessionInterface_CanFindSessionWeWereRegisteredIn_Manager);
    virtual ~FOnlineSubsystemEOS_OnlineSessionInterface_CanFindSessionWeWereRegisteredIn_Manager() = default;

    typedef FOnlineSubsystemEOS_OnlineSessionInterface_CanFindSessionWeWereRegisteredIn_Manager TThisClass;

    class FOnlineSubsystemEOS_OnlineSessionInterface_CanFindSessionWeWereRegisteredIn *T;
    FMultiplayerScenarioInstance Host;
    FMultiplayerScenarioInstance Client;
    TWeakPtr<IOnlineSession, ESPMode::ThreadSafe> HostSessionWk;
    TWeakPtr<IOnlineSession, ESPMode::ThreadSafe> ClientSessionWk;
    std::function<void()> OnDone;
    FString TargetSessionId;
    int FindAttempts;
    bool bPlayerRegistered;
    bool bShuttingDown;

    FDelegateHandle SessionCreateHandle;
    void Start_CreateSession()
    {
        UE_LOG(LogEOSTests, Verbose, TEXT("CanFindSessionWeWereRegisteredIn: Starting CreateSession operation"));

        auto HostSession = this->HostSessionWk.Pin();
        if (!T->TestTrue("Host session interface is valid", HostSession.IsValid()))
        {
            this->Shutdown();
            return;
        }

        TSharedRef<FOnlineSessionSettings> SessionSettings = MakeShared<FOnlineSessionSettings>();
        SessionSettings->NumPublicConnections = 4;
        SessionSettings->bShouldAdvertise = true;
        SessionSettings->bUsesPresence = false;
        SessionSettings->Settings.Add(
            FName(TEXT("BooleanValue")),
            FOnlineSessionSetting(true, EOnlineDataAdvertisementType::ViaOnlineService));

        this->SessionCreateHandle = HostSession->AddOnCreateSessionCompleteDelegate_Handle(
            FOnCreateSessionCompleteDelegate::CreateSP(this, &TThisClass::Handle_CreateSession));
        if (!T->TestTrue(
                "CreateSession call succeeded",
                HostSession->CreateSession(*this->Host.UserId, FName(TEXT("TestSession")), *SessionSettings)))
        {
            HostSession->ClearOnCreateSessionCompleteDelegate_Handle(this->SessionCreateHandle);
            this->Shutdown();
        }
    }
    void Handle_CreateSession(FName SessionName, bool bWasSuccessful)
    {
        UE_LOG(LogEOSTests, Verbose, TEXT("CanFindSessionWeWereRegisteredIn: Handling CreateSession response"));

        auto HostSession = this->HostSessionWk.Pin();
        if (!T->TestTrue("Host session interface is valid", HostSession.IsValid()))
        {
            this->Shutdown();
            return;
        }

        HostSession->ClearOnCreateSessionCompleteDelegate_Handle(this->SessionCreateHandle);

        if (!T->TestTrue("Session was created", bWasSuccessful))
        {
            this->Shutdown();
            return;
        }

        this->TargetSessionId = HostSession->GetNamedSession(FName(TEXT("TestSession")))->GetSessionIdStr();

        this->Start_RegisterPlayer();
    }

    FDelegateHandle RegisterPlayerHandle;
    void Start_RegisterPlayer()
    {
        UE_LOG(LogEOSTests, Verbose, TEXT("CanFindSessionWeWereRegisteredIn: Starting RegisterPlayer operation"));

        auto HostSession = this->HostSessionWk.Pin();
        if (!T->TestTrue("Host session interface is valid", HostSession.IsValid()))
        {
            this->Shutdown();
            return;
        }

        this->RegisterPlayerHandle = HostSession->AddOnRegisterPlayersCompleteDelegate_Handle(
            FOnRegisterPlayersCompleteDelegate::CreateSP(this, &TThisClass::Handle_RegisterPlayer));
        if (!T->TestTrue(
                "Register player succeeded",
                HostSession->RegisterPlayer(FName(TEXT("TestSession")), *this->Client.UserId, false)))
        {
            HostSession->ClearOnRegisterPlayersCompleteDelegate_Handle(this->RegisterPlayerHandle);
            this->Shutdown();
        }
    }
    void Handle_RegisterPlayer(
        FName SessionName,
        const TArray<TSharedRef<const FUniqueNetId>> &RegisteredPlayers,
        bool bWasSuccessful)
    {
        UE_LOG(LogEOSTests, Verbose, TEXT("CanFindSessionWeWereRegisteredIn: Handling RegisterPlayers response"));

        auto HostSession = this->HostSessionWk.Pin();
        if (!T->TestTrue("Host session interface is valid", HostSession.IsValid()))
        {
            this->Shutdown();
            return;
        }

        HostSession->ClearOnRegisterPlayersCompleteDelegate_Handle(this->RegisterPlayerHandle);

        if (!T->TestTrue("Player was registered", bWasSuccessful))
        {
            this->Shutdown();
            return;
        }

        this->bPlayerRegistered = true;

        FUTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateLambda([this](float DeltaTime) {
                this->Start_FindFriendSession();
                return false;
            }),
            5.0f);
    }

    FDelegateHandle FindFriendSessionHandle;
    void Start_FindFriendSession()
    {
        UE_LOG(LogEOSTests, Verbose, TEXT("CanFindSessionWeWereRegisteredIn: Starting FindFriendSession operation"));

        auto ClientSession = this->ClientSessionWk.Pin();
        if (!T->TestTrue("Client session interface is valid", ClientSession.IsValid()))
        {
            this->Shutdown();
            return;
        }

        this->FindFriendSessionHandle = ClientSession->AddOnFindFriendSessionCompleteDelegate_Handle(
            0,
            FOnFindFriendSessionCompleteDelegate::CreateSP(this, &TThisClass::Handle_FindFriendSession));
        if (!T->TestTrue(
                "FindFriendSession call succeeded",
                ClientSession->FindFriendSession(*this->Client.UserId, *this->Client.UserId)))
        {
            ClientSession->ClearOnFindFriendSessionCompleteDelegate_Handle(0, this->FindFriendSessionHandle);
            this->Shutdown();
            return;
        }

        this->FindAttempts++;
    }
    void Handle_FindFriendSession(
        int32 LocalUserNum,
        bool bWasSuccessful,
        const TArray<FOnlineSessionSearchResult> &Results)
    {
        UE_LOG(
            LogEOSTests,
            Verbose,
            TEXT("CanFindSessionWeWereRegisteredIn: Handling FindFriendSession response (attempt %d)"),
            this->FindAttempts);

        auto ClientSession = this->ClientSessionWk.Pin();
        if (!T->TestTrue("Client session interface is valid", ClientSession.IsValid()))
        {
            this->Shutdown();
            return;
        }

        ClientSession->ClearOnFindFriendSessionCompleteDelegate_Handle(0, this->FindFriendSessionHandle);

        // bWasSuccessful is only true if "the session is found and joinable", so it's not strictly
        // an indicator of call success - therefore we expect it to be false if the session
        // wasn't found yet.

        TArray<FOnlineSessionSearchResult> MatchingResults;
        for (const auto &SearchResult : Results)
        {
            if (SearchResult.Session.GetSessionIdStr() == TargetSessionId)
            {
                MatchingResults.Add(SearchResult);
            }
        }

        if (MatchingResults.Num() == 0)
        {
            if (this->FindAttempts < 10)
            {
                UE_LOG(
                    LogEOSTests,
                    Verbose,
                    TEXT("CanFindSessionWeWereRegisteredIn: No matching session yet, rescheduling..."));
                FUTicker::GetCoreTicker().AddTicker(
                    FTickerDelegate::CreateLambda([this](float DeltaTime) {
                        this->Start_FindFriendSession();
                        return false;
                    }),
                    5.0f);
            }
            else
            {
                T->TestTrue("Failed to find session within 10 attempts", false);
                this->Shutdown();
            }
            return;
        }

        UE_LOG(LogEOSTests, Verbose, TEXT("CanFindSessionWeWereRegisteredIn: Found matching session!"));

        // Otherwise, we found our matching session.
        this->Shutdown();
    }

    void Shutdown()
    {
        if (!this->bShuttingDown)
        {
            this->bShuttingDown = true;
            if (this->bPlayerRegistered)
            {
                this->Start_UnregisterPlayer();
            }
            else
            {
                this->Start_DestroySession();
            }
        }
    }

    FDelegateHandle UnregisterPlayerHandle;
    void Start_UnregisterPlayer()
    {
        UE_LOG(LogEOSTests, Verbose, TEXT("CanFindSessionWeWereRegisteredIn: Starting UnregisterPlayer operation"));

        auto HostSession = this->HostSessionWk.Pin();
        if (!T->TestTrue("Host session interface is valid", HostSession.IsValid()))
        {
            this->OnDone();
            return;
        }

        this->UnregisterPlayerHandle = HostSession->AddOnUnregisterPlayersCompleteDelegate_Handle(
            FOnUnregisterPlayersCompleteDelegate::CreateSP(this, &TThisClass::Handle_UnregisterPlayer));
        if (!T->TestTrue(
                "Unregister player succeeded",
                HostSession->UnregisterPlayer(FName(TEXT("TestSession")), *this->Client.UserId)))
        {
            HostSession->ClearOnUnregisterPlayersCompleteDelegate_Handle(this->UnregisterPlayerHandle);
            this->OnDone();
        }
    }
    void Handle_UnregisterPlayer(
        FName SessionName,
        const TArray<TSharedRef<const FUniqueNetId>> &UnregisteredPlayers,
        bool bWasSuccessful)
    {
        UE_LOG(LogEOSTests, Verbose, TEXT("CanFindSessionWeWereRegisteredIn: Handling UnregisterPlayers response"));

        auto HostSession = this->HostSessionWk.Pin();
        if (!T->TestTrue("Host session interface is valid", HostSession.IsValid()))
        {
            this->OnDone();
            return;
        }

        HostSession->ClearOnUnregisterPlayersCompleteDelegate_Handle(this->UnregisterPlayerHandle);

        if (!T->TestTrue("Player was unregistered", bWasSuccessful))
        {
            this->OnDone();
            return;
        }

        this->Start_DestroySession();
    }

    FDelegateHandle DestroySessionHandle;
    void Start_DestroySession()
    {
        UE_LOG(LogEOSTests, Verbose, TEXT("CanFindSessionWeWereRegisteredIn: Starting DestroySession operation"));

        auto HostSession = this->HostSessionWk.Pin();
        if (!T->TestTrue("Host session interface is valid", HostSession.IsValid()))
        {
            this->OnDone();
            return;
        }

        this->DestroySessionHandle = HostSession->AddOnDestroySessionCompleteDelegate_Handle(
            FOnDestroySessionCompleteDelegate::CreateSP(this, &TThisClass::Handle_DestroySession));
        if (!T->TestTrue("Unregister player succeeded", HostSession->DestroySession(FName(TEXT("TestSession")))))
        {
            HostSession->ClearOnDestroySessionCompleteDelegate_Handle(this->DestroySessionHandle);
            this->OnDone();
        }
    }
    void Handle_DestroySession(FName SessionName, bool bWasSuccessful)
    {
        UE_LOG(LogEOSTests, Verbose, TEXT("CanFindSessionWeWereRegisteredIn: Handling DestroySession response"));

        auto HostSession = this->HostSessionWk.Pin();
        if (!T->TestTrue("Host session interface is valid", HostSession.IsValid()))
        {
            this->OnDone();
            return;
        }

        HostSession->ClearOnDestroySessionCompleteDelegate_Handle(this->DestroySessionHandle);

        if (!T->TestTrue("Session was destroyed", bWasSuccessful))
        {
            this->OnDone();
            return;
        }

        this->OnDone();
    }

    // NOLINTNEXTLINE(unreal-field-not-initialized-in-constructor)
    FOnlineSubsystemEOS_OnlineSessionInterface_CanFindSessionWeWereRegisteredIn_Manager(
        class FOnlineSubsystemEOS_OnlineSessionInterface_CanFindSessionWeWereRegisteredIn *InT,
        FMultiplayerScenarioInstance InHost,
        FMultiplayerScenarioInstance InClient,
        std::function<void()> InOnDone)
    {
        this->T = InT;
        this->Host = MoveTemp(InHost);
        this->Client = MoveTemp(InClient);
        this->OnDone = MoveTemp(InOnDone);

        this->HostSessionWk = this->Host.Subsystem.Pin()->GetSessionInterface();
        this->ClientSessionWk = this->Client.Subsystem.Pin()->GetSessionInterface();

        this->FindAttempts = 0;
        this->bPlayerRegistered = false;
        this->bShuttingDown = false;
    }
};

void FOnlineSubsystemEOS_OnlineSessionInterface_CanFindSessionWeWereRegisteredIn::CreateManagingInstance(
    const FOnDone &OnDone,
    const std::function<
        void(const TSharedRef<FOnlineSubsystemEOS_OnlineSessionInterface_CanFindSessionWeWereRegisteredIn_Manager> &)>
        &OnInstanceCreated)
{
    CreateSubsystemsForTest_CreateOnDemand(
        this,
        2,
        OnDone,
        [this, OnInstanceCreated](TArray<FMultiplayerScenarioInstance> Instances, const FOnDone &OnDone) {
            auto Instance =
                MakeShared<FOnlineSubsystemEOS_OnlineSessionInterface_CanFindSessionWeWereRegisteredIn_Manager>(
                    this,
                    Instances[0],
                    Instances[1],
                    OnDone);
            OnInstanceCreated(Instance);
            Instance->Start_CreateSession();
        });
}
