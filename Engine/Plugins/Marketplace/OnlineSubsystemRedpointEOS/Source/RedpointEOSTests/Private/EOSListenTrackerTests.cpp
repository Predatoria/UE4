// Copyright June Rhodes. All Rights Reserved.

#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "OnlineSubsystemRedpointEOS/Private/NetworkingStack/Full/InternetAddrEOSFull.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSListenTracker.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(
    FOnlineSubsystemEOS_EOSListenTracker,
    FHotReloadableAutomationTestBase,
    "OnlineSubsystemEOS.EOSListenTracker",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

bool FOnlineSubsystemEOS_EOSListenTracker::RunTest(const FString &Parameters)
{
    TSharedRef<FEOSListenTracker> ListenTracker = MakeShared<FEOSListenTracker>();

    EOS_ProductUserId UserId = EOS_ProductUserId_FromString("TestAccountId");
    TSharedRef<const FInternetAddr> RegAddr1 = MakeShared<FInternetAddrEOSFull>(UserId, TEXT("SKT1"), 1);
    TSharedRef<const FInternetAddr> RegAddr2 = MakeShared<FInternetAddrEOSFull>(UserId, TEXT("SKT2"), 1);

    // Get returns false with no registration.
    {
        TSharedPtr<const FInternetAddr> InternetAddr;
        TArray<TSharedPtr<FInternetAddr>> DeveloperAddrs;
        TestFalse("No addresses for user", ListenTracker->Get(UserId, InternetAddr, DeveloperAddrs));
    }

    // OnChanged fires on registration (SKT1 #1).
    {
        bool bDidFire = false;
        FDelegateHandle Delegate =
            ListenTracker->OnChanged.AddLambda([=, &bDidFire](
                                                   EOS_ProductUserId EvProductUserId,
                                                   const TSharedRef<const FInternetAddr> &EvInternetAddr,
                                                   const TArray<TSharedPtr<FInternetAddr>> &EvDeveloperInternetAddrs) {
                this->TestEqual("Product user IDs equal OnChanged", UserId, EvProductUserId);
                this->TestTrue("Internet addrs equal OnChanged", *EvInternetAddr == *RegAddr1);
                bDidFire = true;
            });
        ListenTracker->Register(UserId, RegAddr1, TArray<TSharedPtr<FInternetAddr>>());
        TestTrue("OnChanged did fire", bDidFire);
        ListenTracker->OnChanged.Remove(Delegate);
    }

    // Get returns true after registration (SKT1 #1).
    {
        TSharedPtr<const FInternetAddr> InternetAddr;
        TArray<TSharedPtr<FInternetAddr>> DeveloperAddrs;
        TestTrue("Found address for user", ListenTracker->Get(UserId, InternetAddr, DeveloperAddrs));
        TestTrue("Internet addrs equal Get", *InternetAddr == *RegAddr1);
    }

    // OnChanged does *not* fire on registration (SKT1 #2).
    {
        bool bDidFire = false;
        FDelegateHandle Delegate =
            ListenTracker->OnChanged.AddLambda([=, &bDidFire](
                                                   EOS_ProductUserId EvProductUserId,
                                                   const TSharedRef<const FInternetAddr> &EvInternetAddr,
                                                   const TArray<TSharedPtr<FInternetAddr>> &EvDeveloperInternetAddrs) {
                bDidFire = true;
            });
        ListenTracker->Register(UserId, RegAddr1, TArray<TSharedPtr<FInternetAddr>>());
        TestFalse("OnChanged did fire", bDidFire);
        ListenTracker->OnChanged.Remove(Delegate);
    }

    // Get returns true after registration (SKT1 #2).
    {
        TSharedPtr<const FInternetAddr> InternetAddr;
        TArray<TSharedPtr<FInternetAddr>> DeveloperAddrs;
        TestTrue("Found address for user", ListenTracker->Get(UserId, InternetAddr, DeveloperAddrs));
        TestTrue("Internet addrs equal Get", *InternetAddr == *RegAddr1);
    }

    // OnChanged does *not* fire on registration (SKT2 #1).
    {
        bool bDidFire = false;
        FDelegateHandle Delegate =
            ListenTracker->OnChanged.AddLambda([=, &bDidFire](
                                                   EOS_ProductUserId EvProductUserId,
                                                   const TSharedRef<const FInternetAddr> &EvInternetAddr,
                                                   const TArray<TSharedPtr<FInternetAddr>> &EvDeveloperInternetAddrs) {
                bDidFire = true;
            });
        ListenTracker->Register(UserId, RegAddr2, TArray<TSharedPtr<FInternetAddr>>());
        TestFalse("OnChanged did not fire", bDidFire);
        ListenTracker->OnChanged.Remove(Delegate);
    }

    // Get returns true after registration (SKT2 #1) and returns (SKT1).
    {
        TSharedPtr<const FInternetAddr> InternetAddr;
        TArray<TSharedPtr<FInternetAddr>> DeveloperAddrs;
        TestTrue("Found address for user", ListenTracker->Get(UserId, InternetAddr, DeveloperAddrs));
        TestTrue("Internet addrs equal Get", *InternetAddr == *RegAddr1);
    }

    // OnChanged does *not* fire when deregistering (SKT1 #1), because SKT1 has two references.
    {
        bool bDidFire = false;
        FDelegateHandle Delegate =
            ListenTracker->OnChanged.AddLambda([=, &bDidFire](
                                                   EOS_ProductUserId EvProductUserId,
                                                   const TSharedRef<const FInternetAddr> &EvInternetAddr,
                                                   const TArray<TSharedPtr<FInternetAddr>> &EvDeveloperInternetAddrs) {
                bDidFire = true;
            });
        ListenTracker->Deregister(UserId, RegAddr1);
        TestFalse("OnChanged did fire", bDidFire);
        ListenTracker->OnChanged.Remove(Delegate);
    }

    // Get returns true after deregistration (SKT1 #1) and returns (SKT1).
    {
        TSharedPtr<const FInternetAddr> InternetAddr;
        TArray<TSharedPtr<FInternetAddr>> DeveloperAddrs;
        TestTrue("Found address for user", ListenTracker->Get(UserId, InternetAddr, DeveloperAddrs));
        TestTrue("Internet addrs equal Get", *InternetAddr == *RegAddr1);
    }

    // OnChanged fires when deregistering (SKT1 #2), and returns SKT2.
    {
        bool bDidFire = false;
        FDelegateHandle Delegate =
            ListenTracker->OnChanged.AddLambda([=, &bDidFire](
                                                   EOS_ProductUserId EvProductUserId,
                                                   const TSharedRef<const FInternetAddr> &EvInternetAddr,
                                                   const TArray<TSharedPtr<FInternetAddr>> &EvDeveloperInternetAddrs) {
                this->TestEqual("Product user IDs equal OnChanged", UserId, EvProductUserId);
                this->TestTrue("Internet addrs equal OnChanged", *EvInternetAddr == *RegAddr2);
                bDidFire = true;
            });
        ListenTracker->Deregister(UserId, RegAddr1);
        TestTrue("OnChanged did fire", bDidFire);
        ListenTracker->OnChanged.Remove(Delegate);
    }

    // Get returns true after deregistration (SKT1 #2) and returns (SKT2).
    {
        TSharedPtr<const FInternetAddr> InternetAddr;
        TArray<TSharedPtr<FInternetAddr>> DeveloperAddrs;
        TestTrue("Found address for user", ListenTracker->Get(UserId, InternetAddr, DeveloperAddrs));
        TestTrue("Internet addrs equal Get", *InternetAddr == *RegAddr2);
    }

    // OnClosed fires when deregistering (SKT2 #1).
    {
        bool bDidFire = false;
        FDelegateHandle Delegate = ListenTracker->OnClosed.AddLambda([=, &bDidFire](EOS_ProductUserId EvProductUserId) {
            this->TestEqual("Product user IDs equal OnChanged", UserId, EvProductUserId);
            bDidFire = true;
        });
        ListenTracker->Deregister(UserId, RegAddr2);
        TestTrue("OnClosed did fire", bDidFire);
        ListenTracker->OnClosed.Remove(Delegate);
    }

    return true;
}