// Copyright June Rhodes. All Rights Reserved.

#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineStatsInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_Authentication_CrossPlatformRequiredFlowWorks,
    "OnlineSubsystemEOS.Authentication.CrossPlatformRequiredFlowWorks",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

class REDPOINTEOSTESTS_API FEOSConfigAuthTestRequiredLogin : public FEOSConfigEASLogin
{
public:
    FEOSConfigAuthTestRequiredLogin(){};
    UE_NONCOPYABLE(FEOSConfigAuthTestRequiredLogin);
    virtual ~FEOSConfigAuthTestRequiredLogin(){};

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
        return true;
    }
    virtual bool IsAutomatedTesting() const override
    {
        return true;
    }
};

void FOnlineSubsystemEOS_Authentication_CrossPlatformRequiredFlowWorks::RunAsyncTest(
    const std::function<void()> &OnDone)
{
    FOnlineSubsystemRedpointEOSModule &OSSModule =
        FModuleManager::GetModuleChecked<FOnlineSubsystemRedpointEOSModule>(FName(TEXT("OnlineSubsystemRedpointEOS")));

    auto Subsystem = OSSModule.CreateSubsystem(
        FName(*FString::Printf(TEXT("%s"), *this->GetTestName())),
        MakeShared<FEOSConfigAuthTestRequiredLogin>());
    check(Subsystem);

    this->RegisteredSubsystems.Add(Subsystem);

    auto Identity = Subsystem->GetIdentityInterface();
    check(Identity);

    this->AddExpectedError("CPAT-01");
    this->AddExpectedError("CPAT-04");

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

                OnDone();
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