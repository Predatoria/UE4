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
    FOnlineSubsystemEOS_OnlineSessionInterface_JoinSessionOnMissingSessionDoesNotCrash,
    "OnlineSubsystemEOS.OnlineSessionInterface.JoinSessionOnMissingSessionDoesNotCrash",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter,
    FOnlineSubsystemEOS_OnlineSessionInterface_JoinSessionOnMissingSessionDoesNotCrash_Manager);

class FOnlineSubsystemEOS_OnlineSessionInterface_JoinSessionOnMissingSessionDoesNotCrash_Manager
    : public TSharedFromThis<FOnlineSubsystemEOS_OnlineSessionInterface_JoinSessionOnMissingSessionDoesNotCrash_Manager>
{
public:
    UE_NONCOPYABLE(FOnlineSubsystemEOS_OnlineSessionInterface_JoinSessionOnMissingSessionDoesNotCrash_Manager);
    virtual ~FOnlineSubsystemEOS_OnlineSessionInterface_JoinSessionOnMissingSessionDoesNotCrash_Manager() = default;

    typedef FOnlineSubsystemEOS_OnlineSessionInterface_JoinSessionOnMissingSessionDoesNotCrash_Manager TThisClass;

    class FOnlineSubsystemEOS_OnlineSessionInterface_JoinSessionOnMissingSessionDoesNotCrash *T;
    FMultiplayerScenarioInstance Host;
    TWeakPtr<IOnlineSession, ESPMode::ThreadSafe> HostSessionWk;
    std::function<void()> OnDone;

    void Start_FindSessionById()
    {
        UE_LOG(
            LogEOSTests,
            Verbose,
            TEXT("JoinSessionOnMissingSessionDoesNotCrash: Starting Start_FindSessionById operation"));

        auto HostSession = this->HostSessionWk.Pin();
        if (!T->TestTrue("Host session interface is valid", HostSession.IsValid()))
        {
            this->Shutdown();
            return;
        }

        if (!T->TestTrue(
                "FindSessionById call succeeded",
                HostSession->FindSessionById(
                    *this->Host.UserId,
                    *HostSession->CreateSessionIdFromString("c7055aea5c644e7d8f5898464b7a5305"),
                    *this->Host.UserId,
                    FOnSingleSessionResultCompleteDelegate::CreateSP(this, &TThisClass::Handle_FindSessionById))))
        {
            this->Shutdown();
            return;
        }
    }
    void Handle_FindSessionById(int32 LocalUserNum, bool bWasSuccessful, const FOnlineSessionSearchResult &Result)
    {
        UE_LOG(
            LogEOSTests,
            Verbose,
            TEXT("JoinSessionOnMissingSessionDoesNotCrash: Handling FindSessionById response"));

        auto HostSession = this->HostSessionWk.Pin();
        if (!T->TestTrue("Host session interface is valid", HostSession.IsValid()))
        {
            this->Shutdown();
            return;
        }

        if (!T->TestFalse("FindSessionById should fail", bWasSuccessful))
        {
            this->Shutdown();
            return;
        }

        // We ignore the success result here and intentionally try to use a bad result with JoinSession.
        this->Start_JoinSession(Result);
    }

    FDelegateHandle JoinSessionHandle;
    void Start_JoinSession(const FOnlineSessionSearchResult &BadResult)
    {
        UE_LOG(
            LogEOSTests,
            Verbose,
            TEXT("JoinSessionOnMissingSessionDoesNotCrash: Starting Start_JoinSession operation"));

        auto HostSession = this->HostSessionWk.Pin();
        if (!T->TestTrue("Host session interface is valid", HostSession.IsValid()))
        {
            this->Shutdown();
            return;
        }

        this->JoinSessionHandle = HostSession->AddOnJoinSessionCompleteDelegate_Handle(
            FOnJoinSessionCompleteDelegate::CreateSP(this, &TThisClass::Handle_JoinSession));
        if (T->TestTrue(
                "Expected JoinSession call to start",
                HostSession->JoinSession(*this->Host.UserId, NAME_GameSession, BadResult)))
        {
            HostSession->ClearOnJoinSessionCompleteDelegate_Handle(this->JoinSessionHandle);
            this->Shutdown();
        }
    }
    void Handle_JoinSession(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult)
    {
        UE_LOG(LogEOSTests, Verbose, TEXT("CanFindSessionsByBooleanValue: Handling JoinSession response"));

        auto HostSession = this->HostSessionWk.Pin();
        if (!T->TestTrue("Host session interface is valid", HostSession.IsValid()))
        {
            this->Shutdown();
            return;
        }

        HostSession->ClearOnJoinSessionCompleteDelegate_Handle(this->JoinSessionHandle);

        if (!T->TestTrue("JoinSession is expected to fail", JoinResult != EOnJoinSessionCompleteResult::Success))
        {
            this->Shutdown();
            return;
        }

        // Test passed.
        this->Shutdown();
        return;
    }

    void Shutdown()
    {
        this->OnDone();
    }

    // NOLINTNEXTLINE(unreal-field-not-initialized-in-constructor)
    FOnlineSubsystemEOS_OnlineSessionInterface_JoinSessionOnMissingSessionDoesNotCrash_Manager(
        class FOnlineSubsystemEOS_OnlineSessionInterface_JoinSessionOnMissingSessionDoesNotCrash *InT,
        FMultiplayerScenarioInstance InHost,
        std::function<void()> InOnDone)
    {
        this->T = InT;
        this->Host = MoveTemp(InHost);
        this->OnDone = MoveTemp(InOnDone);

        this->HostSessionWk = this->Host.Subsystem.Pin()->GetSessionInterface();
    }
};

void FOnlineSubsystemEOS_OnlineSessionInterface_JoinSessionOnMissingSessionDoesNotCrash::CreateManagingInstance(
    const FOnDone &OnDone,
    const std::function<void(
        const TSharedRef<FOnlineSubsystemEOS_OnlineSessionInterface_JoinSessionOnMissingSessionDoesNotCrash_Manager> &)>
        &OnInstanceCreated)
{
    this->AddExpectedError(TEXT("sessions.invalid_session"), EAutomationExpectedErrorFlags::Contains);

    CreateSubsystemsForTest_CreateOnDemand(
        this,
        1,
        OnDone,
        [this, OnInstanceCreated](TArray<FMultiplayerScenarioInstance> Instances, const FOnDone &OnDone) {
            auto Instance =
                MakeShared<FOnlineSubsystemEOS_OnlineSessionInterface_JoinSessionOnMissingSessionDoesNotCrash_Manager>(
                    this,
                    Instances[0],
                    OnDone);
            OnInstanceCreated(Instance);
            Instance->Start_FindSessionById();
        });
}
