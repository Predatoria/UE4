// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/LogUserIntoOnlineSubsystemNode.h"

#include "OnlineSubsystemUtils.h"

EOS_ENABLE_STRICT_WARNINGS

void FLogUserIntoOnlineSubsystemNode::OnOSSLoginUIClosed(
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedPtr<const FUniqueNetId> UniqueId,
    const int ControllerIndex,
    const FOnlineError &Error,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<FAuthenticationGraphState> InState,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FAuthenticationGraphNodeOnDone InOnDone,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedPtr<IOnlineIdentity, ESPMode::ThreadSafe> OSSIdentity)
{
    if (!Error.bSucceeded)
    {
        UE_LOG(LogEOS, Error, TEXT("Unable to sign user in through online subsystem prompt."));
        InState->ErrorMessages.Add(TEXT("Unable to sign user in through online subsystem prompt"));
        InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    this->DoLogin(InState, InOnDone, OSSIdentity);
}

void FLogUserIntoOnlineSubsystemNode::DoLogin(
    const TSharedRef<FAuthenticationGraphState> &InState,
    const FAuthenticationGraphNodeOnDone &InOnDone,
    const TSharedPtr<IOnlineIdentity, ESPMode::ThreadSafe> &OSSIdentity)
{
    // If the local user isn't signed into the online subsystem, we need them to sign in first before we can get the
    // auth token.
    if (OSSIdentity->GetLoginStatus(InState->LocalUserNum) != ELoginStatus::LoggedIn ||
        OSSIdentity->GetAuthToken(InState->LocalUserNum).IsEmpty())
    {
        // Some online subsystems are poorly implemented, and return false from Login even though they immediately
        // execute the OnLoginComplete callback. This tracks if OnLoginComplete has fired by the time we start handling
        // immediate failure, and lets us skip the failure branch if the subsystem started the callback.
        bool bDidExecuteCallback = false;
        FDelegateHandle BadOSSWorkaroundHandle = OSSIdentity->AddOnLoginCompleteDelegate_Handle(
            InState->LocalUserNum,
            FOnLoginCompleteDelegate::CreateLambda([&bDidExecuteCallback](
                                                       int32 LocalUserNum,
                                                       bool bWasSuccessful,
                                                       const FUniqueNetId &UserId,
                                                       const FString &Error) {
                bDidExecuteCallback = true;
            }));

        FOnlineAccountCredentials EmptyCreds;
        this->OnOSSLoginDelegate = OSSIdentity->AddOnLoginCompleteDelegate_Handle(
            InState->LocalUserNum,
            FOnLoginCompleteDelegate::CreateSP(
                StaticCastSharedRef<FLogUserIntoOnlineSubsystemNode>(this->AsShared()),
                &FLogUserIntoOnlineSubsystemNode::OnOSSLoginComplete,
                InState,
                InOnDone,
                OSSIdentity));
        if (!OSSIdentity->Login(InState->LocalUserNum, EmptyCreds))
        {
            if (!bDidExecuteCallback)
            {
                OSSIdentity->ClearOnLoginCompleteDelegate_Handle(InState->LocalUserNum, BadOSSWorkaroundHandle);
                OSSIdentity->ClearOnLoginCompleteDelegate_Handle(InState->LocalUserNum, this->OnOSSLoginDelegate);
                UE_LOG(
                    LogEOS,
                    Error,
                    TEXT("Could not authenticate with online subsystem, because the Login call failed to start."));
                InState->ErrorMessages.Add(
                    TEXT("Could not authenticate with online subsystem, because the Login call failed to start."));
                InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
                return;
            }
        }

        OSSIdentity->ClearOnLoginCompleteDelegate_Handle(InState->LocalUserNum, BadOSSWorkaroundHandle);
        return;
    }

    // User is now authenticated.
    InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
}

void FLogUserIntoOnlineSubsystemNode::OnOSSLoginComplete(
    int32 LocalUserNum,
    bool bWasSuccessful,
    const FUniqueNetId &UserId,
    const FString &Error,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<FAuthenticationGraphState> InState,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FAuthenticationGraphNodeOnDone InOnDone,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedPtr<IOnlineIdentity, ESPMode::ThreadSafe> OSSIdentity)
{
    OSSIdentity->ClearOnLoginCompleteDelegate_Handle(InState->LocalUserNum, this->OnOSSLoginDelegate);

    if (!bWasSuccessful)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Could not authenticate with online subsystem, subsystem Login() operation failed: %s"),
            *Error);
        InState->ErrorMessages.Add(FString::Printf(
            TEXT("Could not authenticate with online subsystem, subsystem Login() operation failed: %s."),
            *Error));
        InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    // Generate the candidate with the auth token token.
    InState->Metadata.Add("OSS_AUTH_TOKEN", OSSIdentity->GetAuthToken(InState->LocalUserNum));
    auto DisplayName = OSSIdentity->GetPlayerNickname(InState->LocalUserNum);
    if (DisplayName.IsEmpty())
    {
        DisplayName = TEXT("Anonymous");
    }
    InState->Metadata.Add("OSS_AUTH_DISPLAY_NAME", DisplayName);
    InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
}

void FLogUserIntoOnlineSubsystemNode::Execute(
    TSharedRef<FAuthenticationGraphState> InState,
    FAuthenticationGraphNodeOnDone InOnDone)
{
#if !defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING
    if (InState->Config->IsAutomatedTesting())
    {
        // We are running through authentication unit tests, which are designed to test the logic flow of the OSS
        // authentication graph without actually requiring an external service.
        InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Continue);
        return;
    }
#endif

    UWorld *World = InState->GetWorld();
    if (World == nullptr)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Could not authenticate with online subsystem, because the UWorld* pointer was null."));
        InState->ErrorMessages.Add(
            TEXT("Could not authenticate with online subsystem, because the UWorld* pointer was null."));
        InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    IOnlineSubsystem *OSSSubsystem = Online::GetSubsystem(World, this->SubsystemName);
    if (OSSSubsystem == nullptr)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Could not authenticate with online subsystem, because the subsystem '%s' was not available. Check "
                 "that it "
                 "is "
                 "enabled in your DefaultEngine.ini file."),
            *this->SubsystemName.ToString());
        InState->ErrorMessages.Add(TEXT(
            "Could not authenticate with online subsystem, because the subsystem was not available. Check that it is "
            "enabled in your DefaultEngine.ini file."));
        InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    TSharedPtr<IOnlineIdentity, ESPMode::ThreadSafe> OSSIdentity =
        StaticCastSharedPtr<IOnlineIdentity>(OSSSubsystem->GetIdentityInterface());
    if (!OSSIdentity.IsValid())
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Could not authenticate with online subsystem, because the identity interface was not available."));
        InState->ErrorMessages.Add(
            TEXT("Could not authenticate with online subsystem, because the identity interface was not available."));
        InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
        return;
    }

    // If the specified controller isn't signed in at all, we need to use the external UI interface to get
    // them to go through the login process.
    //
    // NOTE: Google implements external UI, but it always fails on Android because it's not necessary to call it.
    if (!OSSSubsystem->GetSubsystemName().IsEqual(GOOGLE_SUBSYSTEM) &&
        !OSSIdentity->GetUniquePlayerId(InState->LocalUserNum).IsValid())
    {
        UE_LOG(LogEOS, Verbose, TEXT("Local user ID is not valid, requesting sign in via external UI if possible..."));
        IOnlineExternalUIPtr OSSExternalUI = OSSSubsystem->GetExternalUIInterface();
        if (OSSExternalUI.IsValid())
        {
            if (!OSSExternalUI->ShowLoginUI(
                    InState->LocalUserNum,
                    true /* Only allow online profiles */,
                    false /* Do not show Skip button */,
                    FOnLoginUIClosedDelegate::CreateSP(
                        StaticCastSharedRef<FLogUserIntoOnlineSubsystemNode>(this->AsShared()),
                        &FLogUserIntoOnlineSubsystemNode::OnOSSLoginUIClosed,
                        InState,
                        InOnDone,
                        OSSIdentity)))
            {
                UE_LOG(LogEOS, Error, TEXT("User is not signed in, and could not show external UI sign-in prompt."));
                InState->ErrorMessages.Add(
                    TEXT("User is not signed in, and could not show external UI sign-in prompt."));
                InOnDone.ExecuteIfBound(EAuthenticationGraphNodeResult::Error);
            }
            return;
        }

        // If OSSExternalUI isn't valid, we assume that calling Login on IOnlineIdentity is enough.
        UE_LOG(LogEOS, Verbose, TEXT("External UI interface is invalid, calling DoLogin directly..."));
    }
    else if (OSSIdentity->GetUniquePlayerId(InState->LocalUserNum).IsValid())
    {
        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("Local user has a valid unique net ID, assuming that external UI doesn't need to be called..."));
    }

    // Otherwise if the user is already signed in externally, proceed with normal Login call immediately.
    this->DoLogin(InState, InOnDone, OSSIdentity);
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION