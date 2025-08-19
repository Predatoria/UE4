// Copyright June Rhodes. All Rights Reserved.

#include "Containers/Ticker.h"
#include "HAL/MemoryMisc.h"
#include "Misc/AutomationTest.h"
#include "Misc/Guid.h"
#include "RedpointEOSInterfaces/Private/Interfaces/OnlineLobbyInterface.h"
#include "RedpointEOSTestsModule.h"
#include "TestHelpers.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_ASYNC_AUTOMATION_TEST(
    FOnlineSubsystemEOS_OnlineLobbyInterface_CanCreateUpdateAndDeleteLobby,
    "OnlineSubsystemEOS.OnlineLobbyInterface.CanCreateUpdateAndDeleteLobby",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FOnlineSubsystemEOS_OnlineLobbyInterface_CanCreateUpdateAndDeleteLobby::RunAsyncTest(
    const std::function<void()> &OnDone)
{
    CreateSingleSubsystemForTest_CreateOnDemand(
        this,
        OnDone,
        [this](
            const IOnlineSubsystemPtr &Subsystem,
            const TSharedPtr<const FUniqueNetIdEOS> &UserId,
            const FOnDone &OnDone) {
            if (!(Subsystem.IsValid() && UserId.IsValid()))
            {
                this->AddError(FString::Printf(TEXT("Unable to init subsystem / authenticate")));
                OnDone();
                return;
            }

            auto LobbyInterface = Online::GetLobbyInterface(Subsystem.Get());
            TestTrue("Online subsystem provides IOnlineLobby interface", LobbyInterface.IsValid());

            TSharedPtr<FOnlineLobbyTransaction> Txn = LobbyInterface->MakeCreateLobbyTransaction(*UserId);
            TestTrue("CreateLobbyTransaction should return new transaction", Txn.IsValid());

            Txn->Capacity = 4;
            Txn->Locked = true;
            Txn->Public = true;

            bool bCreateStarted = LobbyInterface->CreateLobby(
                *UserId,
                *Txn,
                FOnLobbyCreateOrConnectComplete::CreateLambda([=](const FOnlineError &Error,
                                                                  const FUniqueNetId &CreateUserId,
                                                                  const TSharedPtr<class FOnlineLobby> &Lobby) {
                    TestTrue(
                        FString::Printf(TEXT("CreateLobby call should succeed: %s"), *Error.ErrorRaw),
                        Error.bSucceeded);
                    if (Error.bSucceeded)
                    {
                        TSharedPtr<FOnlineLobbyTransaction> UpdateTxn =
                            // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                            LobbyInterface->MakeUpdateLobbyTransaction(CreateUserId, *Lobby->Id);

                        UpdateTxn->Capacity = 8;

                        TSharedPtr<bool> DidFireUpdate = MakeShared<bool>(false);
                        FDelegateHandle EventHandle =
                            LobbyInterface->AddOnLobbyUpdateDelegate_Handle(FOnLobbyUpdateDelegate::CreateLambda(
                                [=](const FUniqueNetId &UpdateUserId, const FOnlineLobbyId &LobbyId) {
                                    TestTrue("OnLobbyUpdate user ID matches", UpdateUserId == *UserId);
                                    TestTrue("OnLobbyUpdate lobby ID matches", LobbyId == *Lobby->Id);
                                    *DidFireUpdate = true;
                                }));

                        bool bUpdateStarted = LobbyInterface->UpdateLobby(
                            CreateUserId,
                            *Lobby->Id,
                            *UpdateTxn,
                            FOnLobbyOperationComplete::CreateLambda(
                                [=](const FOnlineError &Error, const FUniqueNetId &UserId) {
                                    TestTrue(
                                        FString::Printf(TEXT("UpdateLobby call should succeed: %s"), *Error.ErrorRaw),
                                        Error.bSucceeded);
                                    if (Error.bSucceeded)
                                    {
                                        // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                                        bool bDeleteStarted = LobbyInterface->DeleteLobby(
                                            UserId,
                                            *Lobby->Id,
                                            FOnLobbyOperationComplete::CreateLambda(
                                                [=](const FOnlineError &Error, const FUniqueNetId &UserId) {
                                                    FDelegateHandle EventHandleCopy = EventHandle;
                                                    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
                                                    LobbyInterface->ClearOnLobbyUpdateDelegate_Handle(EventHandleCopy);
                                                    TestTrue("OnLobbyUpdate event should have fired", *DidFireUpdate);
                                                    TestTrue(
                                                        FString::Printf(
                                                            TEXT("DeleteLobby call should succeed: %s"),
                                                            *Error.ErrorRaw),
                                                        Error.bSucceeded);
                                                    OnDone();
                                                }));
                                        TestTrue("DeleteLobby should start", bDeleteStarted);
                                        if (!bDeleteStarted)
                                        {
                                            FDelegateHandle EventHandleCopy = EventHandle;
                                            LobbyInterface->ClearOnLobbyUpdateDelegate_Handle(EventHandleCopy);
                                            OnDone();
                                        }
                                    }
                                    else
                                    {
                                        FDelegateHandle EventHandleCopy = EventHandle;
                                        LobbyInterface->ClearOnLobbyUpdateDelegate_Handle(EventHandleCopy);
                                        OnDone();
                                    }
                                }));
                        TestTrue("UpdateLobby should start", bUpdateStarted);
                        if (!bUpdateStarted)
                        {
                            FDelegateHandle EventHandleCopy = EventHandle;
                            LobbyInterface->ClearOnLobbyUpdateDelegate_Handle(EventHandleCopy);
                            OnDone();
                        }
                    }
                    else
                    {
                        OnDone();
                    }
                }));
            TestTrue("CreateLobby should start", bCreateStarted);
            if (!bCreateStarted)
            {
                OnDone();
            }
        });
}