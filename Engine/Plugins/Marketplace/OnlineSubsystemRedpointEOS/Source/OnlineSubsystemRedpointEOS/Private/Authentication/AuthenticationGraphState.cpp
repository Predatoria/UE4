// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraphState.h"

#include "Engine/Engine.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGamesCrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WorldResolution.h"

EOS_ENABLE_STRICT_WARNINGS

FAuthenticationGraphState::FAuthenticationGraphState(
    const TSharedRef<FOnlineSubsystemEOS, ESPMode::ThreadSafe> &InSubsystem,
    int32 InLocalUserNum,
    FName InWorldContextHandle,
    const TSharedRef<FEOSConfig> &InConfig)
    : SelectedEOSCandidate()
    , HasSelectedEOSCandidateFlag(false)
    , CurrentWidget(nullptr)
    , bWasMouseCursorShown(false)
    , LastCachedWorld(nullptr)
    , CleanupNodesOnError()
    , Subsystem(InSubsystem)
    , Config(InConfig)
    , EOSPlatform(InSubsystem->GetPlatformInstance())
    , EOSAuth(EOS_Platform_GetAuthInterface(this->EOSPlatform))
    , EOSConnect(EOS_Platform_GetConnectInterface(this->EOSPlatform))
    , EOSUserInfo(EOS_Platform_GetUserInfoInterface(this->EOSPlatform))
    , LocalUserNum(InLocalUserNum)
    , ExistingUserId()
    , ExistingExternalCredentials()
    , ExistingCrossPlatformAccountId(nullptr)
    , ProvidedCredentials()
    , ErrorMessages()
    , CrossPlatformAccountProvider()
    , AuthenticatedCrossPlatformAccountId(nullptr)
    , AvailableExternalCredentials()
    , AttemptedDeveloperAuthenticationCredentialNames()
    , WorldContextHandle(InWorldContextHandle)
    , EOSCandidates()
    , ResultUserId()
    , ResultExternalCredentials()
    , ResultCrossPlatformAccountId()
    , ResultRefreshCallback()
    , LastSwitchChoice()
    , LastSignInChoice()
#if !UE_BUILD_SHIPPING
    , AutomatedTestingEmailAddress()
    , AutomatedTestingPassword()
#endif // #if !UE_BUILD_SHIPPING
    , Metadata()
    , EASExternalContinuanceToken(nullptr)
    , ResultUserAuthAttributes()
{
    check(this->EOSPlatform != nullptr);
}

FAuthenticationGraphState::~FAuthenticationGraphState()
{
    // We can only clean up if we're not running in garbage collection, since calling
    // IsValid will fail in this scenario. I've only seen this failure when the game
    // is shutting down and the widget was still present on screen when the user
    // clicks the [X] close button on the window (not during a normal shutdown
    // from Exit Game).
    if (!(IsInGameThread() && IsGarbageCollecting()))
    {
        if (this->CurrentWidget.IsValid())
        {
            this->ClearCurrentUserInterfaceWidget();
        }
    }
}

UWorld *FAuthenticationGraphState::GetWorld()
{
    if (this->LastCachedWorld.IsValid())
    {
        return this->LastCachedWorld.Get();
    }

    this->LastCachedWorld = FWorldResolution::GetWorld(this->WorldContextHandle);
    if (!this->LastCachedWorld.IsValid())
    {
        return nullptr;
    }

    return this->LastCachedWorld.Get();
}

/**
 * Note: I don't super like having this inside AuthenticationGraphState, but it is the automatic implementation of the
 * refresh callback for external credentials.
 */
void RefreshFromExternalCredentials(
    const TSharedRef<FAuthenticationGraphRefreshEOSCredentialsInfo> &Info,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<IOnlineExternalCredentials> ExternalCredentials)
{
    ExternalCredentials->Refresh(
        Info->World,
        Info->LocalUserNum,
        FOnlineExternalCredentialsRefreshComplete::CreateLambda([Info, ExternalCredentials](bool bWasSuccessful) {
            if (!bWasSuccessful)
            {
                UE_LOG(LogEOS, Error, TEXT("Unable to refresh external credentials!"));
                Info->OnComplete.ExecuteIfBound(false);
                return;
            }

            FEOSAuthentication::DoRequest(
                Info->EOSConnect,
                ExternalCredentials->GetId(),
                ExternalCredentials->GetToken(),
                StrToExternalCredentialType(ExternalCredentials->GetType()),
                FEOSAuth_DoRequestComplete::CreateLambda([Info, ExternalCredentials](
                                                             const EOS_Connect_LoginCallbackInfo *Data) {
                    // FEOSAuthentication does EOS_Connect_Login, which is all we need to do.
                    if (Data->ResultCode == EOS_EResult::EOS_Success)
                    {
                        // Compute the difference between the existing auth attributes and what is currently in
                        // ExternalCredentials->GetAuthAttributes().
                        TMap<FString, FString> NewAuthAttributes = ExternalCredentials->GetAuthAttributes();
                        Info->SetUserAuthAttributes = NewAuthAttributes;
                        for (const auto &KV : Info->ExistingUserAuthAttributes)
                        {
                            if (!NewAuthAttributes.Contains(KV.Key))
                            {
                                Info->DeleteUserAuthAttributes.Add(KV.Key);
                            }
                        }
                        Info->OnComplete.ExecuteIfBound(true);
                        UE_LOG(LogEOS, Verbose, TEXT("Successfully refreshed EOS external credential login for user"))
                    }
                    else
                    {
                        UE_LOG(
                            LogEOS,
                            Error,
                            TEXT("Failed to refresh EOS external credential login for user, got result "
                                 "code: %s"),
                            ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
                        Info->OnComplete.ExecuteIfBound(false);
                    }
                }));
        }));
}

FAuthenticationGraphEOSCandidate FAuthenticationGraphState::AddEOSConnectCandidateFromExternalCredentials(
    const EOS_Connect_LoginCallbackInfo *Data,
    const TSharedRef<IOnlineExternalCredentials> &ExternalCredentials,
    EAuthenticationGraphEOSCandidateType InType,
    TSharedPtr<const FCrossPlatformAccountId> InCrossPlatformAccountId)
{
    FAuthenticationGraphEOSCandidate Candidate;
    Candidate.DisplayName = ExternalCredentials->GetProviderDisplayName();
    Candidate.UserAuthAttributes = ExternalCredentials->GetAuthAttributes();
    Candidate.ProductUserId = Data->LocalUserId;
    Candidate.ContinuanceToken = Data->ContinuanceToken;
    Candidate.Type = InType;
    Candidate.AssociatedCrossPlatformAccountId = MoveTemp(InCrossPlatformAccountId);
    Candidate.RefreshCallback =
        FAuthenticationGraphRefreshEOSCredentials::CreateStatic(&RefreshFromExternalCredentials, ExternalCredentials);
    Candidate.ExternalCredentials = ExternalCredentials;
    Candidate.NativeSubsystemName = ExternalCredentials->GetNativeSubsystemName();
    this->EOSCandidates.Add(Candidate);
    return Candidate;
}

FAuthenticationGraphEOSCandidate FAuthenticationGraphState::AddEOSConnectCandidate(
    FText DisplayName,
    TMap<FString, FString> UserAuthAttributes,
    const EOS_Connect_LoginCallbackInfo *Data,
    FAuthenticationGraphRefreshEOSCredentials RefreshCallback,
    const FName &InNativeSubsystemName,
    EAuthenticationGraphEOSCandidateType InType,
    TSharedPtr<const FCrossPlatformAccountId> InCrossPlatformAccountId)
{
    FAuthenticationGraphEOSCandidate Candidate;
    Candidate.DisplayName = MoveTemp(DisplayName);
    Candidate.UserAuthAttributes = MoveTemp(UserAuthAttributes);
    Candidate.ProductUserId = Data->LocalUserId;
    Candidate.ContinuanceToken = Data->ContinuanceToken;
    Candidate.Type = InType;
    Candidate.AssociatedCrossPlatformAccountId = MoveTemp(InCrossPlatformAccountId);
    Candidate.RefreshCallback = MoveTemp(RefreshCallback);
    Candidate.ExternalCredentials = nullptr;
    Candidate.NativeSubsystemName = InNativeSubsystemName;
    this->EOSCandidates.Add(Candidate);
    return Candidate;
}

void FAuthenticationGraphState::SelectEOSCandidate(const FAuthenticationGraphEOSCandidate &Candidate)
{
    this->SelectedEOSCandidate = Candidate;
    this->HasSelectedEOSCandidateFlag = true;
}

bool FAuthenticationGraphState::HasSelectedEOSCandidate()
{
    return this->HasSelectedEOSCandidateFlag;
}

bool FAuthenticationGraphState::HasCurrentUserInterfaceWidget()
{
    return this->CurrentWidget != nullptr;
}

void FAuthenticationGraphState::SetCurrentUserInterfaceWidget(UUserWidget *InWidget)
{
    check(this->CurrentWidget == nullptr);

    UEOSSubsystem *GlobalSubsystem = UEOSSubsystem::GetSubsystem(this->GetWorld());
    if (IsValid(GlobalSubsystem))
    {
        if (GlobalSubsystem->OnAddWidgetToViewport.IsBound())
        {
            // Defer to the event instead.
            this->CurrentWidget = InWidget;
            GlobalSubsystem->OnAddWidgetToViewport.Broadcast(InWidget);
            return;
        }
    }
    else
    {
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("Could not get reference to UEOSSubsystem. User widgets will be directly added to the "
                 "viewport."));
    }

    InWidget->AddToViewport(100000);

    this->CurrentWidget = InWidget;

    // Enable cursor.
    if (GEngine && GEngine->GameViewport)
    {
        auto WorldRef = GEngine->GameViewport->GetWorld();
        if (WorldRef != nullptr)
        {
            auto PC = WorldRef->GetFirstPlayerController();
            if (PC != nullptr)
            {
                this->bWasMouseCursorShown = PC->bShowMouseCursor;

                PC->SetInputMode(FInputModeUIOnly());
                PC->bShowMouseCursor = true;
            }
        }
    }
}

void FAuthenticationGraphState::ClearCurrentUserInterfaceWidget()
{
    if (this->CurrentWidget.IsValid())
    {
        UEOSSubsystem *GlobalSubsystem = UEOSSubsystem::GetSubsystem(this->GetWorld());
        if (IsValid(GlobalSubsystem))
        {
            if (GlobalSubsystem->OnRemoveWidgetFromViewport.IsBound())
            {
                // Defer to the event instead.
                GlobalSubsystem->OnRemoveWidgetFromViewport.Broadcast(this->CurrentWidget.Get());
                this->CurrentWidget = nullptr;
                return;
            }
        }
        else
        {
            UE_LOG(
                LogEOS,
                Warning,
                TEXT("Could not get reference to UEOSSubsystem. User widgets will be directly removed from the "
                     "viewport."));
        }

        this->CurrentWidget->RemoveFromParent();
    }

    this->CurrentWidget = nullptr;

    // Restore cursor.
    if (GEngine && GEngine->GameViewport)
    {
        auto WorldRef = GEngine->GameViewport->GetWorld();
        if (WorldRef != nullptr)
        {
            auto PC = WorldRef->GetFirstPlayerController();
            if (PC != nullptr)
            {
                // @todo: We can't restore the InputMode, since there's no GetInputMode function.
                PC->bShowMouseCursor = this->bWasMouseCursorShown;
            }
        }
    }
}

EOS_EpicAccountId FAuthenticationGraphState::GetAuthenticatedEpicAccountId() const
{
    TSharedPtr<const FEpicGamesCrossPlatformAccountId> EpicId =
        this->GetAuthenticatedCrossPlatformAccountId<FEpicGamesCrossPlatformAccountId>(EPIC_GAMES_ACCOUNT_ID);
    if (EpicId.IsValid() && EpicId->HasValidEpicAccountId())
    {
        return EpicId->GetEpicAccountId();
    }
    return nullptr;
}

FAuthenticationGraphEOSCandidate FAuthenticationGraphState::GetSelectedEOSCandidate()
{
    check(this->HasSelectedEOSCandidateFlag);
    return this->SelectedEOSCandidate;
}

void FAuthenticationGraphState::AddCleanupNode(const TSharedRef<class FAuthenticationGraphNode> &Node)
{
    this->CleanupNodesOnError.Add(Node);
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION