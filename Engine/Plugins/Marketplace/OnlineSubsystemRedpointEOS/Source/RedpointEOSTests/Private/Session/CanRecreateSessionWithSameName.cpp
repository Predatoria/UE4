// Copyright June Rhodes. All Rights Reserved.

#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/HeapLambda.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSessionInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_MANAGED_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlineSessionInterface_CanRecreateSessionWithSameName,
    "OnlineSubsystemEOS.OnlineSessionInterface.CanRecreateSessionWithSameName",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter,
    FOnlineSubsystemEOS_OnlineSessionInterface_CanRecreateSessionWithSameName_Manager);

class FOnlineSubsystemEOS_OnlineSessionInterface_CanRecreateSessionWithSameName_Manager
    : public TSharedFromThis<FOnlineSubsystemEOS_OnlineSessionInterface_CanRecreateSessionWithSameName_Manager>
{
public:
    UE_NONCOPYABLE(FOnlineSubsystemEOS_OnlineSessionInterface_CanRecreateSessionWithSameName_Manager);
    virtual ~FOnlineSubsystemEOS_OnlineSessionInterface_CanRecreateSessionWithSameName_Manager() = default;

    typedef FOnlineSubsystemEOS_OnlineSessionInterface_CanRecreateSessionWithSameName_Manager TThisClass;

    class FOnlineSubsystemEOS_OnlineSessionInterface_CanRecreateSessionWithSameName *T;
    FMultiplayerScenarioInstance Host;
    TWeakPtr<IOnlineSession, ESPMode::ThreadSafe> HostSessionWk;
    std::function<void()> OnDone;
    bool bShuttingDown;
    bool bDidInitialCreate;

    void Shutdown()
    {
        if (!this->bShuttingDown)
        {
            this->bShuttingDown = true;
            this->Start_DestroySession();
        }
    }

    FDelegateHandle SessionCreateHandle;
    void Start_CreateSession()
    {
        UE_LOG(LogEOSTests, Verbose, TEXT("CanRecreateSessionWithSameName: Starting CreateSession operation"));

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
        UE_LOG(LogEOSTests, Verbose, TEXT("CanRecreateSessionWithSameName: Handling CreateSession response"));

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

        if (this->bDidInitialCreate)
        {
            // Test finished.
            this->Shutdown();
            return;
        }

        this->bDidInitialCreate = true;
        this->Start_DestroySession();
    }

    FDelegateHandle DestroySessionHandle;
    void Start_DestroySession()
    {
        UE_LOG(LogEOSTests, Verbose, TEXT("CanRecreateSessionWithSameName: Starting DestroySession operation"));

        auto HostSession = this->HostSessionWk.Pin();
        if (!T->TestTrue("Host session interface is valid", HostSession.IsValid()))
        {
            this->OnDone();
            return;
        }

        this->DestroySessionHandle = HostSession->AddOnDestroySessionCompleteDelegate_Handle(
            FOnDestroySessionCompleteDelegate::CreateSP(this, &TThisClass::Handle_DestroySession));
        if (!T->TestTrue("Destroy session succeeded", HostSession->DestroySession(FName(TEXT("TestSession")))))
        {
            HostSession->ClearOnDestroySessionCompleteDelegate_Handle(this->DestroySessionHandle);
            this->OnDone();
        }
    }
    void Handle_DestroySession(FName SessionName, bool bWasSuccessful)
    {
        UE_LOG(LogEOSTests, Verbose, TEXT("CanRecreateSessionWithSameName: Handling DestroySession response"));

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

        if (this->bShuttingDown)
        {
            this->OnDone();
        }
        else
        {
            // Start recreate.
            this->Start_CreateSession();
        }
    }

    // NOLINTNEXTLINE(unreal-field-not-initialized-in-constructor)
    FOnlineSubsystemEOS_OnlineSessionInterface_CanRecreateSessionWithSameName_Manager(
        class FOnlineSubsystemEOS_OnlineSessionInterface_CanRecreateSessionWithSameName *InT,
        FMultiplayerScenarioInstance InHost,
        std::function<void()> InOnDone)
    {
        this->T = InT;
        this->Host = MoveTemp(InHost);
        this->OnDone = MoveTemp(InOnDone);

        this->HostSessionWk = this->Host.Subsystem.Pin()->GetSessionInterface();

        this->bShuttingDown = false;
    }
};

void FOnlineSubsystemEOS_OnlineSessionInterface_CanRecreateSessionWithSameName::CreateManagingInstance(
    const FOnDone &OnDone,
    const std::function<
        void(const TSharedRef<FOnlineSubsystemEOS_OnlineSessionInterface_CanRecreateSessionWithSameName_Manager> &)>
        &OnInstanceCreated)
{
    CreateSubsystemsForTest_CreateOnDemand(
        this,
        1,
        OnDone,
        [this, OnInstanceCreated](TArray<FMultiplayerScenarioInstance> Instances, const FOnDone &OnDone) {
            auto Instance =
                MakeShared<FOnlineSubsystemEOS_OnlineSessionInterface_CanRecreateSessionWithSameName_Manager>(
                    this,
                    Instances[0],
                    OnDone);
            OnInstanceCreated(Instance);
            Instance->Start_CreateSession();
        });
}
