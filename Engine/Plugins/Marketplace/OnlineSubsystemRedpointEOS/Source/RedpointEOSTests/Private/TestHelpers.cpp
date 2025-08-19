// Copyright June Rhodes. All Rights Reserved.

#include "TestHelpers.h"

#include "Dom/JsonObject.h"
#include "EngineUtils.h"
#include "GameFramework/GameStateBase.h"
#include "Misc/FileHelper.h"
#include "Modules/ModuleManager.h"
#include "Net/OnlineEngineInterface.h"
#include "Online.h"
#include "OnlineDelegates.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/HeapLambda.h"
#include "OnlineSubsystemRedpointEOS/Shared/MultiOperation.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"
#include "OnlineSubsystemRedpointEOSModule.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#if WITH_EDITOR
#include "Editor/EditorEngine.h"
#endif

DEFINE_LOG_CATEGORY(LogEOSTests);

FLambdaCommand::FLambdaCommand(FAsyncHotReloadableAutomationTestBase *InTestBase, FLambdaTest InTestCode)
    : TestBase(InTestBase)
    , TestCode(MoveTemp(InTestCode))
    , bIsStarted(false)
    , bIsDone(false)
{
}

bool FLambdaCommand::Update()
{
    if (!this->bIsStarted)
    {
        this->bIsStarted = true;
        this->TestCode([this]() {
            this->bIsDone = true;
        });
    }

    if (this->TestBase != nullptr)
    {
        TArray<int> IndexesToRemove;
        TArray<std::function<void()>> ContinuesToFire;
        int ConditionCount = this->TestBase->WaitingOnConditions.Num();
        for (int i = ConditionCount - 1; i >= 0; i--)
        {
            if (this->TestBase->WaitingOnConditions[i].Key())
            {
                check(
                    this->TestBase->WaitingOnConditions.Num() ==
                    ConditionCount /* Conditions must not call WaitOnCondition! */);
                ContinuesToFire.Add(this->TestBase->WaitingOnConditions[i].Value);
                this->TestBase->WaitingOnConditions.RemoveAt(i);
            }
        }
        for (const auto &FireFunc : ContinuesToFire)
        {
            FireFunc();
        }
    }

    return this->bIsDone;
}

FCleanupSubsystemsCommand::FCleanupSubsystemsCommand(FAsyncHotReloadableAutomationTestBase *InTestBase)
    : TestBase(InTestBase)
{
}

bool FCleanupSubsystemsCommand::Update()
{
    TArray<FName> InstanceNames;
    for (const auto &Subsystem : this->TestBase->RegisteredSubsystems)
    {
        InstanceNames.Add(Subsystem->GetInstanceName());
        Subsystem->Shutdown();
    }
    this->TestBase->RegisteredSubsystems.Empty();

    FOnlineSubsystemRedpointEOSModule &OSSModule =
        FModuleManager::GetModuleChecked<FOnlineSubsystemRedpointEOSModule>(FName(TEXT("OnlineSubsystemRedpointEOS")));
    for (auto InstanceName : InstanceNames)
    {
        check(!OSSModule.HasInstance(InstanceName));
    }

    return true;
}

FHotReloadableAutomationTestBase::FHotReloadableAutomationTestBase(const FString &InName, const bool bInComplexTask)
    : FAutomationTestBase(InName, bInComplexTask)
{
    FAutomationTestFramework::Get().UnregisterAutomationTest(TestName);
    FAutomationTestFramework::Get().RegisterAutomationTest(InName, this);
}

int16 TestHelpers::WorkerNum = -1;
int TestHelpers::Port(int BasePort)
{
    if (WorkerNum == -1)
    {
        if (!FParse::Value(FCommandLine::Get(), TEXT("-AutomationWorkerNum="), WorkerNum))
        {
            WorkerNum = 0;
        }
    }

    return BasePort + (WorkerNum * 25);
}

void FAsyncHotReloadableAutomationTestBase::WhenCondition(
    std::function<bool()> Condition,
    std::function<void()> OnContinue)
{
    this->WaitingOnConditions.Add(TTuple<std::function<bool()>, std::function<void()>>(Condition, OnContinue));
}

bool LoadEpicGamesAutomatedTestingCredentials(
    TArray<FEpicGamesAutomatedTestingCredential> &OutFAutomatedTestingCredentials)
{
    TArray<FString> SearchPaths;
    FString EASAutomatedTestCredentialPathAuthGraph =
        FPlatformMisc::GetEnvironmentVariable(TEXT("EAS_AUTOMATED_TESTING_CREDENTIAL_PATH_AUTH_GRAPH"));
    if (!EASAutomatedTestCredentialPathAuthGraph.IsEmpty())
    {
        SearchPaths.Add(EASAutomatedTestCredentialPathAuthGraph);
    }
    FString EASAutomatedTestCredentialPath =
        FPlatformMisc::GetEnvironmentVariable(TEXT("EAS_AUTOMATED_TESTING_CREDENTIAL_PATH"));
    if (!EASAutomatedTestCredentialPath.IsEmpty())
    {
        SearchPaths.Add(EASAutomatedTestCredentialPath);
    }
    FString DeviceCredentialsPath =
        FModuleManager::GetModuleChecked<FOnlineSubsystemRedpointEOSModule>(FName("OnlineSubsystemRedpointEOS"))
            .GetPathToEASAutomatedTestingCredentials();
    if (!DeviceCredentialsPath.IsEmpty())
    {
        SearchPaths.Add(DeviceCredentialsPath);
    }
#if PLATFORM_WINDOWS
    FString UserProfile = FPlatformMisc::GetEnvironmentVariable(TEXT("USERPROFILE"));
    SearchPaths.Add(
        UserProfile / TEXT("Projects") / TEXT("eas-automated-testing-credentials") / TEXT("Credentials.json"));
#elif PLATFORM_MAC
    FString Home = FPlatformMisc::GetEnvironmentVariable(TEXT("HOME"));
    SearchPaths.Add(Home / TEXT("Projects") / TEXT("eas-automated-testing-credentials") / TEXT("Credentials.json"));
#endif
    FString SelectedPath = TEXT("");
    for (const auto &SearchPath : SearchPaths)
    {
        UE_LOG(LogEOSTests, Verbose, TEXT("Searching for credentials JSON: %s"), *SearchPath);
        if (FPaths::FileExists(SearchPath))
        {
            UE_LOG(LogEOSTests, Verbose, TEXT("Located credentials JSON at: %s"), *SearchPath);
            SelectedPath = SearchPath;
            break;
        }
    }
    if (SelectedPath.IsEmpty())
    {
        UE_LOG(LogEOSTests, Error, TEXT("Credentials JSON could not be found"));
        return false;
    }

    FString JsonString;
    if (!FFileHelper::LoadFileToString(JsonString, *SelectedPath))
    {
        UE_LOG(LogEOSTests, Error, TEXT("Credentials JSON could not be loaded"));
        return false;
    }

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    if (!FJsonSerializer::Deserialize(Reader, JsonObject))
    {
        UE_LOG(LogEOSTests, Error, TEXT("Credentials JSON could not be parsed"));
        return false;
    }

    const TArray<TSharedPtr<FJsonValue>> *PlatformCredentials;
    if (JsonObject->TryGetArrayField(TEXT("Credentials"), PlatformCredentials))
    {
        for (const auto &PlatformCredential : *PlatformCredentials)
        {
            const TSharedPtr<FJsonObject> *Credential;
            if (PlatformCredential->TryGetObject(Credential))
            {
                FEpicGamesAutomatedTestingCredential CredentialLoaded;
                CredentialLoaded.Username = (*Credential)->GetStringField(TEXT("Username"));
                CredentialLoaded.Password = (*Credential)->GetStringField(TEXT("Password"));
                OutFAutomatedTestingCredentials.Add(CredentialLoaded);
            }
        }
        return true;
    }
    else
    {
        UE_LOG(LogEOSTests, Error, TEXT("Credentials JSON does not provide credentials"));
        return false;
    }
}

void CreateSubsystemsForTest_CreateOnDemand(
    FAsyncHotReloadableAutomationTestBase *Test,
    int SubsystemCount,
    const FOnDone &OnDoneFinal,
    const std::function<void(const TArray<FMultiplayerScenarioInstance> &Instances, const FOnDone &OnDone)> &Callback)
{
    TArray<FString> InstanceNames;
    for (int i = 0; i < SubsystemCount; i++)
    {
        InstanceNames.Add(FString::Printf(TEXT("%d"), TestHelpers::Port(i)));
    }
    check(InstanceNames.Num() > 0);

    FMultiOperation<FString, FMultiplayerScenarioInstance>::Run(
        InstanceNames,
        [Test](const FString &InstanceName, const std::function<void(FMultiplayerScenarioInstance OutValue)> &OnDone) {
            FOnlineSubsystemRedpointEOSModule &OSSModule =
                FModuleManager::GetModuleChecked<FOnlineSubsystemRedpointEOSModule>(
                    FName(TEXT("OnlineSubsystemRedpointEOS")));

            auto Subsystem = OSSModule.CreateSubsystem(
                FName(*FString::Printf(TEXT("%s_%s"), *Test->GetTestName(), *InstanceName)),
                MakeShared<FEOSConfigEASLogin>());
            check(Subsystem);

            Test->RegisteredSubsystems.Add(Subsystem);

            auto Identity = Subsystem->GetIdentityInterface();
            check(Identity);

            // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
            FOnDone Cleanup = [Subsystem, OnDone]() {
                OnDone(FMultiplayerScenarioInstance());
            };

            auto CancelLogin = RegisterOSSCallback(
                Test,
                Identity,
                0,
                &IOnlineIdentity::AddOnLoginCompleteDelegate_Handle,
                &IOnlineIdentity::ClearOnLoginCompleteDelegate_Handle,
                std::function<void(int32, bool, const FUniqueNetId &, const FString &)>(
                    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                    [Subsystem, Identity, Cleanup, OnDone](
                        int32 LocalUserNum,
                        bool bWasSuccessful,
                        const FUniqueNetId &UserId,
                        const FString &Error) {
                        if (!bWasSuccessful)
                        {
                            OnDone(FMultiplayerScenarioInstance());
                            return;
                        }

                        auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared());

                        FMultiplayerScenarioInstance Inst;
                        Inst.Subsystem = Subsystem;
                        Inst.UserId = EOSUser;
                        OnDone(Inst);
                    }));
            FOnlineAccountCredentials Creds;
            Creds.Type = TEXT("AUTOMATED_TESTING");
            Creds.Id = FString::Printf(TEXT("CreateOnDemand:%s"), *Test->GetTestName());
            Creds.Token = InstanceName;
            if (!Identity->Login(0, Creds))
            {
                CancelLogin();
                OnDone(FMultiplayerScenarioInstance());
            }

            return true;
        },
        [Test, Callback, OnDoneFinal](const TArray<FMultiplayerScenarioInstance> &Instances) {
            for (const auto &Instance : Instances)
            {
                Test->TestTrue("Subsystem is valid", Instance.Subsystem.IsValid());
                if (!Instance.Subsystem.IsValid())
                {
                    OnDoneFinal();
                    return;
                }
            }

            Callback(Instances, OnDoneFinal);
        });
}

void CreateUsersForTest_CreateOnDemand(
    FAsyncHotReloadableAutomationTestBase *Test,
    int UserCount,
    const FOnDone &OnDoneFinal,
    const std::function<void(
        const IOnlineSubsystemPtr &Subsystem,
        const TArray<TSharedPtr<const FUniqueNetIdEOS>> &UserIds,
        const FOnDone &OnDone)> &Callback)
{
    TArray<int> LocalUserNums;
    for (int i = 0; i < UserCount; i++)
    {
        LocalUserNums.Add(i);
    }
    check(LocalUserNums.Num() > 0);

    FOnlineSubsystemRedpointEOSModule &OSSModule =
        FModuleManager::GetModuleChecked<FOnlineSubsystemRedpointEOSModule>(FName(TEXT("OnlineSubsystemRedpointEOS")));

    auto Subsystem = OSSModule.CreateSubsystem(
        FName(*FString::Printf(TEXT("%s"), *Test->GetTestName())),
        MakeShared<FEOSConfigEASLogin>());

    Test->RegisteredSubsystems.Add(Subsystem);

    auto Identity = Subsystem->GetIdentityInterface();
    check(Identity);

    FMultiOperation<int, TSharedPtr<const FUniqueNetIdEOS>>::Run(
        LocalUserNums,
        [Test,
         // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
         Subsystem,
         // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
         Identity](int LocalUserNum, const std::function<void(TSharedPtr<const FUniqueNetIdEOS> OutValue)> &OnDone) {
            FString InstanceName = FString::Printf(TEXT("Test0%d"), LocalUserNum + 1);

            auto CancelLogin = RegisterOSSCallback(
                Test,
                Identity,
                LocalUserNum,
                &IOnlineIdentity::AddOnLoginCompleteDelegate_Handle,
                &IOnlineIdentity::ClearOnLoginCompleteDelegate_Handle,
                // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                std::function<void(int32, bool, const FUniqueNetId &, const FString &)>([Subsystem, Identity, OnDone](
                                                                                            int32 InLocalUserNum,
                                                                                            bool bWasSuccessful,
                                                                                            const FUniqueNetId &UserId,
                                                                                            const FString &Error) {
                    if (!bWasSuccessful)
                    {
                        OnDone(nullptr);
                        return;
                    }

                    auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared());

                    OnDone(EOSUser);
                }));
            FOnlineAccountCredentials Creds;
            Creds.Type = TEXT("AUTOMATED_TESTING");
            Creds.Id = FString::Printf(TEXT("CreateOnDemand:%s"), *Test->GetTestName());
            Creds.Token = FString::Printf(TEXT("%d"), TestHelpers::Port(LocalUserNum));
            if (!Identity->Login(LocalUserNum, Creds))
            {
                CancelLogin();
                OnDone(nullptr);
            }

            return true;
        },
        // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
        [Test, Subsystem, Callback, OnDoneFinal](const TArray<TSharedPtr<const FUniqueNetIdEOS>> &LocalUsers) {
            for (const auto &LocalUser : LocalUsers)
            {
                if (!LocalUser.IsValid())
                {
                    OnDoneFinal();
                    return;
                }
            }

            Callback(Subsystem, LocalUsers, OnDoneFinal);
        });
}

void CreateSubsystemsForTest_EpicGames(
    FAsyncHotReloadableAutomationTestBase *Test,
    EEpicGamesAccountCount SubsystemCount,
    const FOnDone &OnDoneFinal,
    const std::function<void(const TArray<FMultiplayerScenarioInstance> &Instances, const FOnDone &OnDone)> &Callback)
{
    TArray<FEpicGamesAutomatedTestingCredential> Credentials;
    if (!LoadEpicGamesAutomatedTestingCredentials(Credentials) || Credentials.Num() < 2)
    {
        Test->AddWarning(TEXT("This test is being skipped, because EAS testing credentials are not available."));
        OnDoneFinal();
        return;
    }

    TArray<FString> InstanceNames;
    if (SubsystemCount == EEpicGamesAccountCount::One)
    {
        InstanceNames.Add(TEXT("Test01"));
    }
    if (SubsystemCount == EEpicGamesAccountCount::Two)
    {
        InstanceNames.Add(TEXT("Test01"));
        InstanceNames.Add(TEXT("Test02"));
    }
    check(InstanceNames.Num() > 0);

    FMultiOperation<FString, FMultiplayerScenarioInstance>::Run(
        InstanceNames,
        [Test, Credentials](
            const FString &InstanceName,
            const std::function<void(FMultiplayerScenarioInstance OutValue)> &OnDone) {
            FOnlineSubsystemRedpointEOSModule &OSSModule =
                FModuleManager::GetModuleChecked<FOnlineSubsystemRedpointEOSModule>(
                    FName(TEXT("OnlineSubsystemRedpointEOS")));

            auto Subsystem = OSSModule.CreateSubsystem(
                FName(*FString::Printf(TEXT("%s_%s"), *Test->GetTestName(), *InstanceName)),
                MakeShared<FEOSConfigEASLogin>());
            check(Subsystem);

            Test->RegisteredSubsystems.Add(Subsystem);

            auto Identity = Subsystem->GetIdentityInterface();
            check(Identity);

            // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
            FOnDone Cleanup = [Subsystem, OnDone]() {
                OnDone(FMultiplayerScenarioInstance());
            };

            auto CancelLogin = RegisterOSSCallback(
                Test,
                Identity,
                0,
                &IOnlineIdentity::AddOnLoginCompleteDelegate_Handle,
                &IOnlineIdentity::ClearOnLoginCompleteDelegate_Handle,
                std::function<void(int32, bool, const FUniqueNetId &, const FString &)>(
                    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                    [Subsystem, Identity, Cleanup, OnDone](
                        int32 LocalUserNum,
                        bool bWasSuccessful,
                        const FUniqueNetId &UserId,
                        const FString &Error) {
                        if (!bWasSuccessful)
                        {
                            OnDone(FMultiplayerScenarioInstance());
                            return;
                        }

                        auto EOSUser = StaticCastSharedRef<const FUniqueNetIdEOS>(UserId.AsShared());

                        FMultiplayerScenarioInstance Inst;
                        Inst.Subsystem = Subsystem;
                        Inst.UserId = EOSUser;
                        OnDone(Inst);
                    }));
            FOnlineAccountCredentials Creds;
            Creds.Type = TEXT("AUTOMATED_TESTING");
            Creds.Id = Credentials[InstanceName == TEXT("Test01") ? 0 : 1].Username;
            Creds.Token = Credentials[InstanceName == TEXT("Test01") ? 0 : 1].Password;
            if (!Identity->Login(0, Creds))
            {
                CancelLogin();
                OnDone(FMultiplayerScenarioInstance());
            }

            return true;
        },
        [Test, Callback, OnDoneFinal](const TArray<FMultiplayerScenarioInstance> &Instances) {
            for (const auto &Instance : Instances)
            {
                Test->TestTrue("Subsystem is valid", Instance.Subsystem.IsValid());
                if (!Instance.Subsystem.IsValid())
                {
                    OnDoneFinal();
                    return;
                }
            }

            Callback(Instances, OnDoneFinal);
        });
}

void CreateSingleSubsystemForTest_CreateOnDemand(
    FAsyncHotReloadableAutomationTestBase *Test,
    const FOnDone &OnDone,
    const std::function<void(
        const IOnlineSubsystemPtr &Subsystem,
        const TSharedPtr<const FUniqueNetIdEOS> &UserId,
        const FOnDone &OnDone)> &Callback)
{
    CreateSubsystemsForTest_CreateOnDemand(
        Test,
        1,
        OnDone,
        [Callback](TArray<FMultiplayerScenarioInstance> Instances, const FOnDone &ChainedOnDone) {
            Callback(Instances[0].Subsystem.Pin(), Instances[0].UserId, ChainedOnDone);
        });
}

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_TestHelpers_MultipleSubsystems,
    "OnlineSubsystemEOS.TestHelpers.MultipleSubsystems",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_TestHelpers_MultipleSubsystems::RunAsyncTest(const std::function<void()> &OnDone)
{
    CreateSubsystemsForTest_CreateOnDemand(
        this,
        4,
        OnDone,
        [=](TArray<FMultiplayerScenarioInstance> Instances, const FOnDone &OnDone) {
            TestEqual("Instance count is as expected", Instances.Num(), 4);
            for (int i = 0; i < Instances.Num(); i++)
            {
                TestTrue(FString::Printf(TEXT("Subsystem %d is valid"), i), Instances[i].Subsystem.IsValid());
                TestTrue(FString::Printf(TEXT("User %d is valid"), i), Instances[i].UserId.IsValid());
            }

            OnDone();
        });
}

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_TestHelpers_HeapLambda,
    "OnlineSubsystemEOS.TestHelpers.HeapLambda",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_TestHelpers_HeapLambda::RunAsyncTest(const std::function<void()> &OnDone)
{
    {
        FHeapLambda<EHeapLambdaFlags::None> Lambda;

        bool bDidFire = false;
        Lambda = [&bDidFire]() {
            bDidFire = true;
        };

        Lambda();

        TestTrue("Lambda fired", bDidFire);
    }

    {
        FHeapLambda<EHeapLambdaFlags::None> Lambda;

        int FireCount = 0;
        Lambda = [&FireCount]() {
            FireCount++;
        };

        Lambda();

        FHeapLambda<EHeapLambdaFlags::None> LambdaSecondRef = Lambda;

        LambdaSecondRef();

        TestEqual("Lambda fired expected number of times", FireCount, 2);
    }

    {
        FHeapLambda<EHeapLambdaFlags::OneShot> Lambda;

        int FireCount = 0;
        Lambda = [&FireCount]() {
            FireCount++;
        };

        Lambda();

        FHeapLambda<EHeapLambdaFlags::None> LambdaSecondRef(Lambda);

        LambdaSecondRef();

        TestEqual("One-shot lambda fired expected number of times", FireCount, 1);
    }

    {
        FHeapLambda<EHeapLambdaFlags::None> Lambda;

        bool bDidFire = false;
        Lambda = [&bDidFire]() {
            bDidFire = true;
        };

        TestFalse("Lambda did not fire", bDidFire);
    }

    {
        bool bDidFireOnCleanup = false;

        {
            FHeapLambda<EHeapLambdaFlags::AlwaysCleanup> Lambda;
            Lambda = [&bDidFireOnCleanup]() {
                bDidFireOnCleanup = true;
            };
        }

        TestTrue("Lambda fired on cleanup", bDidFireOnCleanup);
    }

    {
        bool *bDidFireOnCleanup = new bool(false);

        FHeapLambda<EHeapLambdaFlags::OneShotCleanup> Cleanup = [bDidFireOnCleanup]() {
            delete bDidFireOnCleanup;
        };

        {
            FHeapLambda<EHeapLambdaFlags::None> Lambda;

            Lambda = [bDidFireOnCleanup]() {
                *bDidFireOnCleanup = true;
            };

            FUTicker::GetCoreTicker().AddTicker(
                FTickerDelegate::CreateLambda([this, bDidFireOnCleanup, Lambda, Cleanup, OnDone](float DeltaTime) {
                    TestFalse("Lambda hasn't fired yet", *bDidFireOnCleanup);

                    Lambda();

                    TestTrue("Lambda has fired", *bDidFireOnCleanup);

                    Cleanup();

                    OnDone();

                    return false;
                }),
                0);

            TestFalse("Lambda hasn't fired yet", *bDidFireOnCleanup);
        }
    }
}
