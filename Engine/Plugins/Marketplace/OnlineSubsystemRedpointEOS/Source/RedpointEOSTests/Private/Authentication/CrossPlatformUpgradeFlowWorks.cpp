// Copyright June Rhodes. All Rights Reserved.

#include "Containers/Ticker.h"
#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineStatsInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_Authentication_CrossPlatformUpgradeFlowWorks,
    "OnlineSubsystemEOS.Authentication.CrossPlatformUpgradeFlowWorks",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

class REDPOINTEOSTESTS_API FEOSConfigAuthTestUpgradeLogin : public FEOSConfigEASLogin
{
public:
    FEOSConfigAuthTestUpgradeLogin(){};
    UE_NONCOPYABLE(FEOSConfigAuthTestUpgradeLogin);
    virtual ~FEOSConfigAuthTestUpgradeLogin(){};

    virtual FName GetAuthenticationGraph() const override
    {
        return FName(TEXT("AutomatedTestingOSS"));
    }
    virtual FName GetCrossPlatformAccountProvider() const override
    {
        return FName(TEXT("AutomatedTesting"));
    }
    virtual bool GetRequireCrossPlatformAccount() const override
    {
        return false;
    }
    virtual bool IsAutomatedTesting() const override
    {
        return true;
    }
};

void FOnlineSubsystemEOS_Authentication_CrossPlatformUpgradeFlowWorks::RunAsyncTest(const std::function<void()> &OnDone)
{
    FOnlineSubsystemRedpointEOSModule &OSSModule =
        FModuleManager::GetModuleChecked<FOnlineSubsystemRedpointEOSModule>(FName(TEXT("OnlineSubsystemRedpointEOS")));

    auto Subsystem = OSSModule.CreateSubsystem(
        FName(*FString::Printf(TEXT("%s"), *this->GetTestName())),
        MakeShared<FEOSConfigAuthTestUpgradeLogin>());
    check(Subsystem);

    this->RegisteredSubsystems.Add(Subsystem);

    auto Identity = Subsystem->GetIdentityInterface();
    check(Identity);

    this->AddExpectedError("CPAT-03");
    this->AddExpectedError("CPAT-05");

    auto CancelLogin = RegisterOSSCallback(
        this,
        Identity,
        0,
        &IOnlineIdentity::AddOnLoginCompleteDelegate_Handle,
        &IOnlineIdentity::ClearOnLoginCompleteDelegate_Handle,
        std::function<void(int32, bool, const FUniqueNetId &, const FString &)>(
            [this,
             // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
             Subsystem,
             // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
             Identity,
             OnDone](int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId &UserId, const FString &Error) {
                if (!bWasSuccessful)
                {
                    this->AddError(TEXT("Failed to authenticate!"));
                    OnDone();
                    return;
                }

                // We must start the upgrade process on the next frame, because the login process is still considered
                // ongoing until the OnLoginCompleteDelegate finishes executing.
                FUTicker::GetCoreTicker().AddTicker(
                    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                    FTickerDelegate::CreateLambda([this, Subsystem, Identity, OnDone](float DeltaSeconds) {
                        // Now do a login again to trigger the upgrade process.
                        auto CancelUpgrade = RegisterOSSCallback(
                            this,
                            Identity,
                            0,
                            &IOnlineIdentity::AddOnLoginCompleteDelegate_Handle,
                            &IOnlineIdentity::ClearOnLoginCompleteDelegate_Handle,
                            std::function<void(int32, bool, const FUniqueNetId &, const FString &)>(
                                // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                                [this, Subsystem, Identity, OnDone](
                                    int32 LocalUserNum,
                                    bool bWasSuccessful,
                                    const FUniqueNetId &UserId,
                                    const FString &Error) {
                                    if (!bWasSuccessful)
                                    {
                                        this->AddError(TEXT("Failed to authenticate as part of upgrade!"));
                                        OnDone();
                                        return;
                                    }

                                    OnDone();
                                }));
                        FOnlineAccountCredentials Creds;
                        Creds.Type = TEXT("AUTOMATED_TESTING_OSS");
                        Creds.Id = FString::Printf(TEXT("CreateOnDemand:%s"), *this->GetTestName());
                        Creds.Token = FString::Printf(TEXT("%d"), TestHelpers::Port(0));
                        if (!Identity->Login(0, Creds))
                        {
                            CancelUpgrade();
                            this->AddError(TEXT("Login call failed to start for upgrade process!"));
                            OnDone();
                        }
                        return false;
                    }),
                    0.0f);
            }));

    FOnlineAccountCredentials Creds;
    Creds.Type = TEXT("AUTOMATED_TESTING_OSS");
    Creds.Id = FString::Printf(TEXT("CreateOnDemand:%s"), *this->GetTestName());
    Creds.Token = FString::Printf(TEXT("%d"), TestHelpers::Port(0));
    if (!Identity->Login(0, Creds))
    {
        CancelLogin();
        this->AddError(TEXT("Login call failed to start!"));
        OnDone();
    }
}