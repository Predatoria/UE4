// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

ONLINESUBSYSTEMREDPOINTEOS_API EOS_EExternalCredentialType StrToExternalCredentialType(const FString &InStr);

EOS_DISABLE_STRICT_WARNINGS

#if EOS_HAS_AUTHENTICATION

#include "Containers/Ticker.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraphState.h"
#include "UObject/SoftObjectPtr.h"

EOS_ENABLE_STRICT_WARNINGS

DECLARE_DELEGATE_OneParam(FEASAuth_DoRequestComplete, const EOS_Auth_LoginCallbackInfo * /* Data */);
DECLARE_DELEGATE_OneParam(FEOSAuth_DoRequestComplete, const EOS_Connect_LoginCallbackInfo * /* Data */);
DECLARE_DELEGATE_OneParam(FEASAuth_DoRequestLinkComplete, const EOS_Auth_LinkAccountCallbackInfo * /* Data */);

ONLINESUBSYSTEMREDPOINTEOS_API FString ExternalCredentialTypeToStr(EOS_EExternalCredentialType InType);

class ONLINESUBSYSTEMREDPOINTEOS_API FEASAuthentication
{
public:
    static void DoRequest(
        EOS_HAuth EOSAuth,
        const FString &Id,
        const FString &Token,
        EOS_ELoginCredentialType Type,
        const FEASAuth_DoRequestComplete &OnComplete);
    static void DoRequestExternal(
        EOS_HAuth EOSAuth,
        const FString &Id,
        const FString &Token,
        EOS_EExternalCredentialType ExternalType,
        const FEASAuth_DoRequestComplete &OnComplete);
    static void DoRequestLink(
        EOS_HAuth EOSAuth,
        EOS_ContinuanceToken ContinuanceToken,
        EOS_ELinkAccountFlags LinkAccountFlags,
        EOS_EpicAccountId LocalUserId,
        const FEASAuth_DoRequestLinkComplete &OnComplete);
};

class ONLINESUBSYSTEMREDPOINTEOS_API FEOSAuthentication
{
public:
    static void DoRequest(
        EOS_HConnect EOSConnect,
        const FString &Id,
        const FString &Token,
        EOS_EExternalCredentialType Type,
        const FEOSAuth_DoRequestComplete &OnComplete);
};

/**
 * A helper class for implementing credential obtainment, where the implementation can be shared between the
 * authentication graph and IOnlineExternalCredentials::Refresh.
 */
template <typename TClass, typename TResult> class FAuthenticationCredentialObtainer : public TSharedFromThis<TClass>
{
public:
    DECLARE_DELEGATE_TwoParams(FOnCredentialObtained, bool /* bWasSuccessful*/, TResult /* Result */);

private:
    TSharedPtr<TClass> SelfReference;
    FOnCredentialObtained Callback;
    TSharedPtr<FAuthenticationGraphState> State;

protected:
    void Done(bool bWasSuccessful, TResult Result)
    {
        this->Callback.ExecuteIfBound(bWasSuccessful, Result);
        this->SelfReference = nullptr;
    }

    void EmitError(FString ErrorMessage, FString HumanSafeErrorMessage = TEXT(""))
    {
        UE_LOG(LogEOS, Error, TEXT("%s"), *ErrorMessage);
        if (State.IsValid())
        {
            State->ErrorMessages.Add(HumanSafeErrorMessage.IsEmpty() ? ErrorMessage : HumanSafeErrorMessage);
        }
    }

public:
    FAuthenticationCredentialObtainer(const FOnCredentialObtained &InCallback)
    {
        this->Callback = InCallback;
    }
    UE_NONCOPYABLE(FAuthenticationCredentialObtainer);
    virtual ~FAuthenticationCredentialObtainer(){};

    /** Start the asynchronous operation. Return true if Tick should run, or false on failure to start. */
    virtual bool Init(UWorld *World, int32 LocalUserNum) = 0;

    /** Tick the asynchronous operation. Optional. Return false when the task has completed (you should call Done here
     * or in an event handler you registered in Init). */
    virtual bool Tick(float DeltaSeconds)
    {
        return false;
    }

    template <typename... TArgs>
    static void StartFromAuthenticationGraph(
        TSharedRef<FAuthenticationGraphState> InState,
        FOnCredentialObtained OnDone,
        TArgs... Args)
    {
        UWorld *World = InState->GetWorld();

        TSharedPtr<TClass> Task = MakeShared<TClass>(OnDone, Args...);
        Task->State = InState;
        Task->SelfReference = Task;
        if (!Task->Init(!IsValid(World) ? nullptr : World, InState->LocalUserNum))
        {
            Task->SelfReference = nullptr;
            return;
        }

        FUTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateSP(Task.ToSharedRef(), &TClass::Tick), 0);
    }

    template <typename... TArgs>
    static void StartFromCredentialRefresh(
        TSoftObjectPtr<UWorld> InWorld,
        int32 LocalUserNum,
        FOnCredentialObtained OnDone,
        TArgs... Args)
    {
        TSharedPtr<TClass> Task = MakeShared<TClass>(OnDone, Args...);
        Task->SelfReference = Task;
        if (!Task->Init(InWorld.IsValid() ? InWorld.Get() : nullptr, LocalUserNum))
        {
            Task->SelfReference = nullptr;
            return;
        }

        FUTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateSP(Task.ToSharedRef(), &TClass::Tick), 0);
    }
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION