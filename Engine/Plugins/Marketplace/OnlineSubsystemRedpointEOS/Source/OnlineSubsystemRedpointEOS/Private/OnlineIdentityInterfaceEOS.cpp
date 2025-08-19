// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/OnlineIdentityInterfaceEOS.h"

#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemImpl.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraphRegistry.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGamesCrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Graphs/AuthenticationGraphDevAuthTool.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSPlatformUserIdManager.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSRuntimePlatform.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineIdentityInterfaceEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/EpicGames/OnlineSubsystemRedpointEAS.h"
#include "OnlineSubsystemRedpointEOS/Shared/MultiOperation.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineSubsystemRedpointEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineUserEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/WorldResolution.h"
#include "OnlineSubsystemUtils.h"
#include "UObject/UObjectIterator.h"
#if !UE_BUILD_SHIPPING
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Graphs/AuthenticationGraphAutomatedTesting.h"
#endif
#if defined(EOS_IS_FREE_EDITION)
#include "OnlineSubsystemRedpointEOS/Shared/EOSLicenseValidator.h"
#endif
#if defined(EOS_VOICE_CHAT_SUPPORTED)
#include "OnlineSubsystemRedpointEOS/Shared/VoiceChat/VoiceManager.h"
#endif

EOS_ENABLE_STRICT_WARNINGS

#define ONLINE_ERROR_NAMESPACE "EOS"

FOnlineIdentityInterfaceEOS::FOnlineIdentityInterfaceEOS(
    const TSharedRef<FOnlineSubsystemEOS, ESPMode::ThreadSafe> &InSubsystemEOS,
    EOS_HPlatform InPlatform,
    const FString &InInstanceName,
    bool InIsDedicatedServer,
    TSharedPtr<class FOnlineIdentityInterfaceEAS, ESPMode::ThreadSafe> InIdentityEAS,
    const TSharedRef<class IEOSRuntimePlatform> &InRuntimePlatform,
    const TSharedRef<class FEOSConfig> &InConfig,
    const TSharedRef<class FEOSUserFactory, ESPMode::ThreadSafe> &InUserFactory,
    const TSharedRef<class FOnlineSubsystemEOS, ESPMode::ThreadSafe> &InOSSInstance)
    : EOSPlatform(InPlatform)
    , EOSConnect(EOS_Platform_GetConnectInterface(InPlatform))
    , EOSAuth(EOS_Platform_GetAuthInterface(InPlatform))
    , IsDedicatedServer(InIsDedicatedServer)
    , InstanceName(InInstanceName)
    , SubsystemEOS(InSubsystemEOS)
    , IdentityEAS(MoveTemp(InIdentityEAS))
    , RuntimePlatform(InRuntimePlatform)
    , Config(InConfig)
    , UserFactory(InUserFactory)
    , Unregister_AuthExpiration(nullptr)
    , Unregister_LoginStatusChanged(nullptr)
    , OSSInstance(InOSSInstance)
    ,
#if defined(EOS_VOICE_CHAT_SUPPORTED)
    VoiceManager(nullptr)
    ,
#endif // #if defined(EOS_VOICE_CHAT_SUPPORTED)
    LoggedInUsers()
    , LoggedInAccounts()
    ,
#if EOS_HAS_AUTHENTICATION
    RefreshCallbacks()
    , ExternalCredentials()
    , ProductUserIdToCrossPlatformAccountId()
    , PlatformIdMappings()
    ,
#endif // #if EOS_HAS_AUTHENTICATION
    AuthenticationInProgress()
{
    check(this->EOSPlatform != nullptr);
    check(this->EOSConnect != nullptr);
    check(this->EOSAuth != nullptr);
};

#if EOS_HAS_AUTHENTICATION
void FOnlineIdentityInterfaceEOS::HandleLoginExpiry(int32 LocalUserNum)
{
    if (this->GetLoginStatus(LocalUserNum) != ELoginStatus::LoggedIn)
    {
        UE_LOG(LogEOS, Error, TEXT("Can't refresh credentials for user %d, they are not signed in"), LocalUserNum);
        return;
    }

    TSharedPtr<const FUniqueNetId> Id = this->GetUniquePlayerId(LocalUserNum);
    TSharedPtr<FUserOnlineAccountEOS> AccountEOS =
        StaticCastSharedPtr<FUserOnlineAccountEOS>(this->GetUserAccount(*Id));

    UE_LOG(
        LogEOS,
        Verbose,
        TEXT("Authentication is about to expire for %s, invoking refresh callback"),
        *Id->ToString());

    TSoftObjectPtr<UWorld> WorldForRefresh = FWorldResolution::GetWorld(FName(*this->InstanceName));
    if (!WorldForRefresh.IsValid())
    {
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("FWorldResolution could not obtain world for instance name %s, refresh will probably fail!"),
            *this->InstanceName);
    }

    auto Info = MakeShared<FAuthenticationGraphRefreshEOSCredentialsInfo>(
        WorldForRefresh,
        LocalUserNum,
        this->EOSConnect,
        this->EOSAuth,
        AccountEOS->AuthAttributes);
    Info->OnComplete = FAuthenticationGraphRefreshEOSCredentialsComplete::CreateLambda(
        [WeakThis = GetWeakThis(this), Info, AccountEOS](bool bWasSuccessful) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (bWasSuccessful)
                {
                    for (const auto &Remove : Info->DeleteUserAuthAttributes)
                    {
                        AccountEOS->AuthAttributes.Remove(Remove);
                    }
                    for (const auto &Add : Info->SetUserAuthAttributes)
                    {
                        AccountEOS->AuthAttributes.Add(Add.Key, Add.Value);
                    }
                }

                TSoftObjectPtr<UWorld> World = FWorldResolution::GetWorld(FName(*This->InstanceName), true);
                if (World.IsValid())
                {
                    UEOSSubsystem *GlobalSubsystem = UEOSSubsystem::GetSubsystem(World.Get());
                    if (IsValid(GlobalSubsystem))
                    {
                        GlobalSubsystem->OnCredentialRefreshComplete.Broadcast(bWasSuccessful);
                    }
                }
            }
        });

    this->RefreshCallbacks[LocalUserNum].ExecuteIfBound(Info);
}
#endif // #if EOS_HAS_AUTHENTICATION

void FOnlineIdentityInterfaceEOS::RegisterEvents()
{
#if EOS_HAS_AUTHENTICATION
    EOS_Connect_AddNotifyAuthExpirationOptions ExpireOpts = {};
    ExpireOpts.ApiVersion = EOS_CONNECT_ADDNOTIFYAUTHEXPIRATION_API_LATEST;
    EOS_Connect_AddNotifyLoginStatusChangedOptions LoginChangedOpts = {};
    LoginChangedOpts.ApiVersion = EOS_CONNECT_ADDNOTIFYLOGINSTATUSCHANGED_API_LATEST;

    this->Unregister_AuthExpiration = EOSRegisterEvent<
        EOS_HConnect,
        EOS_Connect_AddNotifyAuthExpirationOptions,
        EOS_Connect_AuthExpirationCallbackInfo>(
        this->EOSConnect,
        &ExpireOpts,
        EOS_Connect_AddNotifyAuthExpiration,
        EOS_Connect_RemoveNotifyAuthExpiration,
        [WeakThis = GetWeakThis(this)](const EOS_Connect_AuthExpirationCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                auto TargetUserId = MakeShared<FUniqueNetIdEOS>(Data->LocalUserId);
                for (const auto &KV : This->LoggedInAccounts)
                {
                    auto AccountEOS = StaticCastSharedPtr<FUserOnlineAccountEOS>(KV.Value);
                    auto IdEOS = AccountEOS->GetUserIdEOS();
                    if (*IdEOS == *TargetUserId)
                    {
                        FString PUID;
                        EOSString_ProductUserId::ToString(Data->LocalUserId, PUID);

                        int32 LocalUserNum;
                        verifyf(
                            This->GetLocalUserNum(*TargetUserId, LocalUserNum),
                            TEXT("Can not determine LocalUserNum for logged in account on refresh"));

                        This->HandleLoginExpiry(LocalUserNum);
                    }
                }
            }
        });
    this->Unregister_LoginStatusChanged = EOSRegisterEvent<
        EOS_HConnect,
        EOS_Connect_AddNotifyLoginStatusChangedOptions,
        EOS_Connect_LoginStatusChangedCallbackInfo>(
        this->EOSConnect,
        &LoginChangedOpts,
        EOS_Connect_AddNotifyLoginStatusChanged,
        EOS_Connect_RemoveNotifyLoginStatusChanged,
        [WeakThis = GetWeakThis(this)](const EOS_Connect_LoginStatusChangedCallbackInfo *Data) {
            if (auto This = PinWeakThis(WeakThis))
            {
                EOS_ELoginStatus PreviousStatus = Data->PreviousStatus;
                if (PreviousStatus == EOS_ELoginStatus::EOS_LS_UsingLocalProfile)
                {
                    PreviousStatus = EOS_ELoginStatus::EOS_LS_NotLoggedIn;
                }

                EOS_ELoginStatus CurrentStatus = Data->CurrentStatus;
                if (CurrentStatus == EOS_ELoginStatus::EOS_LS_UsingLocalProfile)
                {
                    CurrentStatus = EOS_ELoginStatus::EOS_LS_NotLoggedIn;
                }

                if (CurrentStatus == EOS_ELoginStatus::EOS_LS_NotLoggedIn &&
                    PreviousStatus == EOS_ELoginStatus::EOS_LS_LoggedIn)
                {
                    // The user got signed out, fire the appropriate events.
                    FString PUID;
                    EOSString_ProductUserId::ToString(Data->LocalUserId, PUID);
                    auto TargetUserId = MakeShared<FUniqueNetIdEOS>(Data->LocalUserId);

                    for (const auto &KV : This->LoggedInUsers)
                    {
                        auto IdEOS = StaticCastSharedPtr<const FUniqueNetIdEOS>(KV.Value);
                        if (IdEOS && *IdEOS == *TargetUserId)
                        {
                            int LocalUserNum = KV.Key;
                            UE_LOG(
                                LogEOS,
                                Warning,
                                TEXT("User %s (local user num %d) was unexpectedly signed out, firing events"),
                                *PUID,
                                KV.Key);
                            This->Logout(LocalUserNum);
                            return;
                        }
                    }

                    UE_LOG(
                        LogEOS,
                        Verbose,
                        TEXT("User %s was signed out, but is not associated with any player; ignoring..."),
                        *PUID);
                }
            }
        });
#endif // #if EOS_HAS_AUTHENTICATION
}

bool FOnlineIdentityInterfaceEOS::Exec(UWorld *InWorld, const TCHAR *Cmd, FOutputDevice &Ar)
{
    TSoftObjectPtr<UWorld> OurWorld = FWorldResolution::GetWorld(FName(*this->InstanceName));
    if (!OurWorld.IsValid())
    {
        UE_LOG(
            LogEOS,
            Warning,
            TEXT("FWorldResolution failed to get world for instance %s when command was arriving in ::Exec"),
            *this->InstanceName);
        return false;
    }
    if (OurWorld.Get() != InWorld)
    {
        return false;
    }

#if EOS_HAS_AUTHENTICATION
    if (FParse::Command(&Cmd, TEXT("SIMULATEEXPIRY"), false))
    {
        int32 LocalUserNum = FCString::Atoi(*FParse::Token(Cmd, false));
        UE_LOG(LogEOS, Verbose, TEXT("Simulating login expiry for local user num %d..."), LocalUserNum);
        this->HandleLoginExpiry(LocalUserNum);
        return true;
    }
#endif // #if EOS_HAS_AUTHENTICATION

    return false;
}

void FOnlineIdentityInterfaceEOS::DumpLoggedInUsers()
{
    auto ConnectUsers = EOS_Connect_GetLoggedInUsersCount(this->EOSConnect);
    auto AuthAccounts = EOS_Auth_GetLoggedInAccountsCount(this->EOSAuth);

    UE_LOG(
        LogEOS,
        Verbose,
        TEXT("DumpLoggedInUsers: (%d connect users, %d auth accounts)"),
        ConnectUsers,
        AuthAccounts);
    for (int i = 0; i < ConnectUsers; i++)
    {
        auto ConnectUserId = EOS_Connect_GetLoggedInUserByIndex(this->EOSConnect, i);
        FString ConnectUserIdStr;
        if (EOSString_ProductUserId::ToString(ConnectUserId, ConnectUserIdStr) == EOS_EResult::EOS_Success)
        {
            UE_LOG(LogEOS, Verbose, TEXT("DumpLoggedInUsers: Connect user %d: %s"), i, *ConnectUserIdStr);
        }
    }
    for (int i = 0; i < AuthAccounts; i++)
    {
        auto EpicAccountId = EOS_Auth_GetLoggedInAccountByIndex(this->EOSAuth, i);
        FString EpicAccountIdStr;
        if (EOSString_EpicAccountId::ToString(EpicAccountId, EpicAccountIdStr) == EOS_EResult::EOS_Success)
        {
            UE_LOG(LogEOS, Verbose, TEXT("DumpLoggedInUsers: Epic account %d: %s"), i, *EpicAccountIdStr);
        }
    }
}

void FOnlineIdentityInterfaceEOS::EOS_LoadAccountInformation(
    const TSharedPtr<const FUniqueNetIdEOS> &UserId,
    const TMap<FString, FString> &InAuthAttributes,
    const FEOSIdentity_AccountLoaded &OnComplete)
{
    TArray<TSharedRef<const FUniqueNetIdEOS>> UserIds;
    UserIds.Add(UserId.ToSharedRef());
    FUserOnlineAccountEOS::Get(
        this->UserFactory.ToSharedRef(),
        UserId.ToSharedRef(),
        UserIds,
        [WeakThis = GetWeakThis(this), OnComplete, InAuthAttributes, UserId](
            const TUserIdMap<TSharedPtr<FUserOnlineAccountEOS>> &AccountMap) {
            const TSharedPtr<FUserOnlineAccountEOS> &Account = AccountMap[*UserId];

            Account->Identity = WeakThis;
            Account->AuthAttributes = InAuthAttributes;

            if (!Account->IsValid())
            {
                UE_LOG(LogEOS, Error, TEXT("EOS_LoadAccountInformation: Unable to load account information"));
                OnComplete.ExecuteIfBound(nullptr);
                return;
            }

            OnComplete.ExecuteIfBound(Account);
        });
}

void FOnlineIdentityInterfaceEOS::UpdateUniqueNetIdForPlayerController(
    int InLocalPlayerNum,
    const TSharedPtr<const FUniqueNetId> &InNewUniqueId)
{
    if (GEngine == nullptr)
    {
        if (GIsAutomationTesting)
        {
            return;
        }

        UE_LOG(
            LogEOS,
            Error,
            TEXT("GEngine is not available; the unique net ID stored in ULocalPlayer and APlayerState will "
                 "not be correct until the next map load."));
        return;
    }

    FName InstanceNameName = FName(*this->InstanceName);
    FName WorldContextHandle =
        (InstanceNameName != NAME_None && InstanceNameName != FOnlineSubsystemImpl::DefaultInstanceName)
            ? InstanceNameName
            : FName("Context_0");

    auto WorldContext = GEngine->GetWorldContextFromHandle(WorldContextHandle);
    if (WorldContext == nullptr)
    {
        if (GIsAutomationTesting)
        {
            return;
        }

        UE_LOG(
            LogEOS,
            Error,
            TEXT("Unable to obtain world context; the unique net ID stored in ULocalPlayer and APlayerState will "
                 "not be correct until the next map load."));
        return;
    }

    auto World = WorldContext->World();
    if (World == nullptr)
    {
        // This is probably a valid scenario, where the world hasn't been set up yet. Since the world hasn't been set
        // up, we don't need to worry about the ULocalPlayer or APlayerState not being in sync.
        return;
    }

    ULocalPlayer *LocalPlayer = GEngine->GetLocalPlayerFromControllerId(World, InLocalPlayerNum);
    if (LocalPlayer == nullptr)
    {
        if (GIsAutomationTesting)
        {
            return;
        }

        UE_LOG(
            LogEOS,
            Error,
            TEXT("Unable to get local player %d; the unique net ID stored in ULocalPlayer and APlayerState will "
                 "not be correct until the next map load."),
            InLocalPlayerNum);
        return;
    }

#if defined(UE_5_0_OR_LATER)
    LocalPlayer->SetCachedUniqueNetId(FUniqueNetIdRepl(InNewUniqueId));
#else
    LocalPlayer->SetCachedUniqueNetId(InNewUniqueId);
#endif // #if defined(UE_5_0_OR_LATER)
    UE_LOG(LogEOS, Verbose, TEXT("Updated unique net ID of ULocalPlayer for local player num %d."), InLocalPlayerNum);

    auto PlayerController = LocalPlayer->GetPlayerController(World);
    if (PlayerController != nullptr && PlayerController->PlayerState != nullptr)
    {
#if defined(UE_5_0_OR_LATER)
        PlayerController->PlayerState->SetUniqueId(FUniqueNetIdRepl(InNewUniqueId));
#else
        PlayerController->PlayerState->SetUniqueId(InNewUniqueId);
#endif // #if defined(UE_5_0_OR_LATER)
        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("Updated unique net ID of APlayerState for local player num %d."),
            InLocalPlayerNum);
    }
}

#if EOS_HAS_AUTHENTICATION
#if WITH_EDITOR
bool FOnlineIdentityInterfaceEOS::CheckIfReadyToFinishEditorLogin(
    float DeltaSeconds,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    const TSharedPtr<class FAuthenticationGraphState> InState,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    const TSharedPtr<FUserOnlineAccount> InUserAccount)
{
    if (GEngine == nullptr)
    {
        // Engine no longer valid, finish firing delegates for completeness.
        UE_LOG(LogEOS, Warning, TEXT("PIE client login completing early because engine is no longer available"));
        this->FinishLogin(InState, InUserAccount);
        return false;
    }

    for (const auto &WorldContext : GEngine->GetWorldContexts())
    {
        if (WorldContext.WorldType == EWorldType::PIE && WorldContext.PIEInstance == 0)
        {
            if (WorldContext.bWaitingOnOnlineSubsystem)
            {
                // We are still waiting for the host to be ready.
                return true;
            }

            // The host has finished being ready, complete our client login.
            UE_LOG(LogEOS, Verbose, TEXT("PIE client login completing because the PIE server is now ready"));
            this->FinishLogin(InState, InUserAccount);
            return false;
        }
    }

    // The host context has disappeared, finish firing delegates for completeness.
    UE_LOG(LogEOS, Warning, TEXT("PIE client login completing early because host context is no longer available"));
    this->FinishLogin(InState, InUserAccount);
    return false;
}
#endif // #if WITH_EDITOR

void FOnlineIdentityInterfaceEOS::FinishLogin(
    const TSharedPtr<class FAuthenticationGraphState> &State,
    const TSharedPtr<FUserOnlineAccount> &UserAccount)
{
    this->LoggedInUsers.Add(State->LocalUserNum, State->ResultUserId);
    this->LoggedInUserNativeSubsystemNames.Add(State->LocalUserNum, State->ResultNativeSubsystemName);
    this->LoggedInAccounts.Add(State->LocalUserNum, UserAccount);
    this->RefreshCallbacks.Add(State->LocalUserNum, State->ResultRefreshCallback);
    if (State->ResultExternalCredentials.IsValid())
    {
        this->ExternalCredentials.Add(*State->ResultUserId, State->ResultExternalCredentials.ToSharedRef());
    }
    if (State->ResultCrossPlatformAccountId.IsValid())
    {
        this->ProductUserIdToCrossPlatformAccountId.Add(
            *State->ResultUserId,
            State->ResultCrossPlatformAccountId.ToSharedRef());
    }
#if defined(EOS_VOICE_CHAT_SUPPORTED)
    this->VoiceManager->AddLocalUser(*State->ResultUserId);
#endif
    this->UpdateUniqueNetIdForPlayerController(State->LocalUserNum, State->ResultUserId);
    FPlatformUserId AllocatedPlatformId = FEOSPlatformUserIdManager::AllocatePlatformId(
        this->OSSInstance.Pin().ToSharedRef(),
        State->ResultUserId.ToSharedRef());
    this->PlatformIdMappings.Add(*State->ResultUserId, AllocatedPlatformId);

    if (State->ResultCrossPlatformAccountId.IsValid() &&
        State->ResultCrossPlatformAccountId->GetType() == EPIC_GAMES_ACCOUNT_ID)
    {
        this->IdentityEAS->UserSignedInWithEpicId(
            State->LocalUserNum,
            StaticCastSharedPtr<const FEpicGamesCrossPlatformAccountId>(State->ResultCrossPlatformAccountId));
    }

    // Remove AuthenticationInProgress before we fire the delegates; this is to allow a developer to call Login
    // again in response to a failure.
    this->AuthenticationInProgress.Remove(State->LocalUserNum);

    this->TriggerOnLoginCompleteDelegates(State->LocalUserNum, true, State->ResultUserId.ToSharedRef().Get(), TEXT(""));
    if (!State->ExistingUserId.IsValid())
    {
        this->TriggerOnLoginStatusChangedDelegates(
            State->LocalUserNum,
            ELoginStatus::NotLoggedIn,
            ELoginStatus::LoggedIn,
            *State->ResultUserId);
        this->TriggerOnLoginChangedDelegates(State->LocalUserNum);
    }

    UE_LOG(
        LogEOSIdentity,
        Verbose,
        TEXT("Local user %d is now signed in as %s"),
        State->LocalUserNum,
        *State->ResultUserId->ToString());
}
#endif // #if EOS_HAS_AUTHENTICATION

bool FOnlineIdentityInterfaceEOS::Login(int32 LocalUserNum, const FOnlineAccountCredentials &AccountCredentials)
{
    if (this->AuthenticationInProgress.Contains(LocalUserNum))
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Authentication is already in process for user %d, wait until it completes before calling "
                 "Login "
                 "again."),
            LocalUserNum);
        return false;
    }

#if !defined(PLATFORM_LINUX) || !PLATFORM_LINUX
    if (LocalUserNum >= MAX_LOCAL_PLAYERS)
    {
        UE_LOG(
            LogEOS,
            Error,
            TEXT("Specified local user num is greater than the maximum number of allowed local players."));
        return false;
    }
#endif

#if defined(EOS_IS_FREE_EDITION)
    // We have to guard here, even though we don't have the graph yet.
    this->AuthenticationInProgress.Add(LocalUserNum, nullptr);

    FEOSLicenseValidator::GetInstance()->ValidateLicense(
        this->Config.ToSharedRef(),
        [WeakThis = GetWeakThis(this), LocalUserNum]() {
            if (auto This = PinWeakThis(WeakThis))
            {
                This->AuthenticationInProgress.Remove(LocalUserNum);
                This->TriggerOnLoginCompleteDelegates(
                    LocalUserNum,
                    false,
                    *FUniqueNetIdEOS::InvalidId(),
                    TEXT("Invalid license key for EOS. Please report this issue to the game developer."));
            }
        },
        [WeakThis = GetWeakThis(this), LocalUserNum, AccountCredentials]() {
            if (auto This = PinWeakThis(WeakThis))
            {
                This->AuthenticationInProgress.Remove(LocalUserNum);
#else
    TSharedPtr<FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> This = this->AsShared();
#endif

#if EOS_HAS_AUTHENTICATION
                if (This->IsDedicatedServer)
#endif // #if EOS_HAS_AUTHENTICATION
                {
                    // If we are running as a dedicated server, we don't perform any kind of login. Instead
                    // the login succeeds automatically with an EOS ID built from MakeDedicatedServerId().
                    auto ServerUserId = FUniqueNetIdEOS::DedicatedServerId();
                    auto ServerAccount = FUserOnlineAccountEOS::NewInvalid(ServerUserId);
                    This->LoggedInUsers.Add(LocalUserNum, ServerUserId);
                    This->LoggedInUserNativeSubsystemNames.Add(LocalUserNum, NAME_None);
                    This->LoggedInAccounts.Add(LocalUserNum, ServerAccount);
#if EOS_HAS_AUTHENTICATION
                    This->RefreshCallbacks.Add(LocalUserNum, FAuthenticationGraphRefreshEOSCredentials());
#endif // #if EOS_HAS_AUTHENTICATION
                    This->UpdateUniqueNetIdForPlayerController(LocalUserNum, ServerUserId);
                    This->TriggerOnLoginCompleteDelegates(LocalUserNum, true, ServerUserId.Get(), TEXT(""));
                    This->TriggerOnLoginStatusChangedDelegates(
                        LocalUserNum,
                        ELoginStatus::NotLoggedIn,
                        ELoginStatus::LoggedIn,
                        *ServerUserId);
                    This->TriggerOnLoginChangedDelegates(LocalUserNum);
                    UE_LOG(LogEOS, Verbose, TEXT("DedicatedServer: Performed Login on dedicated server."));
                    UE_LOG(
                        LogEOSIdentity,
                        Verbose,
                        TEXT("Local user %d is now signed in as %s"),
                        LocalUserNum,
                        *ServerUserId->ToString());
#if defined(EOS_IS_FREE_EDITION)
                    return;
#else
        return true;
#endif
                }

#if EOS_HAS_AUTHENTICATION
                auto SubsystemEOSPinned = This->SubsystemEOS.Pin();
                checkf(
                    SubsystemEOSPinned.IsValid(),
                    TEXT("EOS subsystem should be valid when starting authentication!"));
                TSharedPtr<FAuthenticationGraphState> State = MakeShared<FAuthenticationGraphState>(
                    SubsystemEOSPinned.ToSharedRef(),
                    LocalUserNum,
                    FName(*This->InstanceName),
                    This->Config.ToSharedRef());
                if (AccountCredentials.Type == TEXT("AUTOMATED_TESTING"))
                {
#if !UE_BUILD_SHIPPING
                    State->AutomatedTestingEmailAddress = AccountCredentials.Id;
                    State->AutomatedTestingPassword = AccountCredentials.Token;
#endif
                    State->ProvidedCredentials = FOnlineAccountCredentials();
                }
                else
                {
                    State->ProvidedCredentials = AccountCredentials;
                }
                if (This->LoggedInUsers.Contains(LocalUserNum))
                {
                    // If already authenticated, set the existing user ID.
                    State->ExistingUserId =
                        StaticCastSharedPtr<const FUniqueNetIdEOS>(This->LoggedInUsers[LocalUserNum]);

                    if (State->ExistingUserId.IsValid() && This->ExternalCredentials.Contains(*State->ExistingUserId))
                    {
                        State->ExistingExternalCredentials = This->ExternalCredentials[*State->ExistingUserId];
                    }
                }

                FName CrossPlatformAccountProviderName = This->Config->GetCrossPlatformAccountProvider();
                if (!CrossPlatformAccountProviderName.IsNone())
                {
                    if (!FAuthenticationGraphRegistry::HasCrossPlatformAccountProvider(
                            CrossPlatformAccountProviderName))
                    {
                        UE_LOG(
                            LogEOS,
                            Error,
                            TEXT("Unable to instantiate cross platform account provider '%s'! Check your "
                                 "configuration!"),
                            *CrossPlatformAccountProviderName.ToString());
                        This->AuthenticationInProgress.Remove(LocalUserNum);
                        This->TriggerOnLoginCompleteDelegates(
                            LocalUserNum,
                            false,
                            *FUniqueNetIdEOS::InvalidId(),
                            FString::Printf(
                                TEXT("Your application is misconfigured. There is no cross platform account provider "
                                     "registered "
                                     "with name '%s'."),
                                *CrossPlatformAccountProviderName.ToString()));
#if defined(EOS_IS_FREE_EDITION)
                        return;
#else
                        return true;
#endif
                    }
                    else
                    {
                        State->CrossPlatformAccountProvider =
                            FAuthenticationGraphRegistry::GetCrossPlatformAccountProvider(
                                CrossPlatformAccountProviderName);
                    }
                }

                TSharedPtr<FAuthenticationGraph> Graph = nullptr;
#if !UE_BUILD_SHIPPING
                if (!State->AutomatedTestingEmailAddress.IsEmpty())
                {
                    Graph = MakeShared<FAuthenticationGraphAutomatedTesting>();
                }
                else
#endif
                    if (State->ProvidedCredentials.Id == TEXT("DEV_TOOL_AUTO_LOGIN"))
                {
                    Graph = MakeShared<FAuthenticationGraphDevAuthTool>();
                }
                else
                {
                    TSoftObjectPtr<UWorld> World = FWorldResolution::GetWorld(FName(*This->InstanceName), true);
                    FName GraphName = This->Config->GetAuthenticationGraph();
#if WITH_EDITOR && (!defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING)
                    if (World.IsValid() &&
                        (World->WorldType == EWorldType::Editor || World->WorldType == EWorldType::PIE ||
                         World->WorldType == EWorldType::EditorPreview || World->WorldType == EWorldType::Inactive))
                    {
                        GraphName = This->Config->GetEditorAuthenticationGraph();
                        UE_LOG(
                            LogEOS,
                            Verbose,
                            TEXT("Using authentication graph value '%s' because you are running in the editor."),
                            *GraphName.ToString());
                    }
#endif
                    Graph = FAuthenticationGraphRegistry::Get(
                        GraphName,
                        This->Config.ToSharedRef(),
                        State->ProvidedCredentials,
                        World);
                    if (!Graph.IsValid())
                    {
                        UE_LOG(
                            LogEOS,
                            Error,
                            TEXT("Unable to instantiate authentication graph! Check your configuration!"));

                        This->AuthenticationInProgress.Remove(LocalUserNum);
                        This->TriggerOnLoginCompleteDelegates(
                            LocalUserNum,
                            false,
                            *FUniqueNetIdEOS::InvalidId(),
                            FString::Printf(
                                TEXT("Your application is misconfigured. There is no authentication graph registered "
                                     "with name '%s'."),
                                *GraphName.ToString()));
#if defined(EOS_IS_FREE_EDITION)
                        return;
#else
                        return true;
#endif
                    }
                }

                This->AuthenticationInProgress.Add(LocalUserNum, Graph);

                Graph->Execute(
                    State.ToSharedRef(),
                    FAuthenticationGraphNodeOnDone::CreateLambda([WeakThis = GetWeakThis(This),
                                                                  State](EAuthenticationGraphNodeResult Result) {
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            if (Result == EAuthenticationGraphNodeResult::Continue)
                            {
                                check(State->ResultUserId.IsValid());

                                // Check to see if the user ID we authenticated as is already signed
                                // in under a different local user. If they are, this is an error.
                                for (const auto &ExistingUser : This->LoggedInUsers)
                                {
                                    if (ExistingUser.Value.IsValid() && State->ResultUserId.IsValid() &&
                                        *ExistingUser.Value == *State->ResultUserId)
                                    {
                                        if (ExistingUser.Key == State->LocalUserNum)
                                        {
                                            // This will happen if it's an account upgrade.
                                            continue;
                                        }

                                        UE_LOG(
                                            LogEOS,
                                            Error,
                                            TEXT("Authentication error: This user is already signed into as a "
                                                 "different local user. Each local user must sign into a unique "
                                                 "account."));
                                        This->AuthenticationInProgress.Remove(State->LocalUserNum);
                                        This->TriggerOnLoginCompleteDelegates(
                                            State->LocalUserNum,
                                            false,
                                            *FUniqueNetIdEOS::InvalidId(),
                                            TEXT("Authentication error: This user is already signed into as a "
                                                 "different local user. Each local user must sign into a unique "
                                                 "account."));
                                        return;
                                    }
                                }

                                UE_LOG(
                                    LogEOS,
                                    Verbose,
                                    TEXT("Authentication success: %s"),
                                    *(State->ResultUserId->ToString()));

                                This->EOS_LoadAccountInformation(
                                    State->ResultUserId,
                                    State->ResultUserAuthAttributes,
                                    FEOSIdentity_AccountLoaded::CreateLambda(
                                        [WeakThis = GetWeakThis(This),
                                         State](TSharedPtr<FUserOnlineAccount> UserAccount) {
                                            if (auto This = PinWeakThis(WeakThis))
                                            {
                                                if (!UserAccount.IsValid())
                                                {
                                                    // Blank account - we couldn't load their details.
                                                    UserAccount = FUserOnlineAccountEOS::NewInvalid(
                                                        State->ResultUserId.ToSharedRef());
                                                }

#if WITH_EDITOR
                                                // When we are automatically logging in in the editor, we need to
                                                // ensure that client PIE instances always finish their login
                                                // process *after* the server PIE instance is listening. This is
                                                // because UEOSNetDriver needs to fix up the default 127.0.0.1 URL
                                                // if the server is listening over EOS P2P.
                                                if ((State->ProvidedCredentials.Id == TEXT("DEV_TOOL_AUTO_LOGIN") ||
                                                     !State->AutomatedTestingEmailAddress.IsEmpty()) &&
                                                    GEngine != nullptr)
                                                {
                                                    FWorldContext *WorldContext =
                                                        GEngine->GetWorldContextFromHandle(FName(*This->InstanceName));
                                                    if (WorldContext != nullptr)
                                                    {
                                                        if (WorldContext->WorldType == EWorldType::PIE &&
                                                            WorldContext->PIEInstance != 0)
                                                        {
                                                            UE_LOG(
                                                                LogEOS,
                                                                Verbose,
                                                                TEXT("PIE client login is being delayed while waiting "
                                                                     "for the server to be ready."));
                                                            FUTicker::GetCoreTicker().AddTicker(
                                                                FTickerDelegate::CreateThreadSafeSP(
                                                                    This->AsShared(),
                                                                    &FOnlineIdentityInterfaceEOS::
                                                                        CheckIfReadyToFinishEditorLogin,
                                                                    State,
                                                                    UserAccount),
                                                                0);
                                                            return;
                                                        }
                                                    }
                                                }
#endif

                                                This->FinishLogin(State, UserAccount);
                                            }
                                        }));
                            }
                            else if (Result == EAuthenticationGraphNodeResult::Error)
                            {
                                for (int i = 0; i < State->ErrorMessages.Num(); i++)
                                {
                                    UE_LOG(
                                        LogEOS,
                                        Error,
                                        TEXT("Authentication error #%d: %s"),
                                        i,
                                        *State->ErrorMessages[i]);
                                }
                                This->AuthenticationInProgress.Remove(State->LocalUserNum);
                                This->TriggerOnLoginCompleteDelegates(
                                    State->LocalUserNum,
                                    false,
                                    *FUniqueNetIdEOS::InvalidId(),
                                    State->ErrorMessages.Num() == 0
                                        ? TEXT("AuthenticationGraph did not specify an error. Ensure you add an error "
                                               "message to State->ErrorMessages before returning with "
                                               "EAuthenticationGraphNodeResult::Error.")
                                        : State->ErrorMessages.Last());
                            }
                            else
                            {
                                check(false /* unknown result state */);
                                This->AuthenticationInProgress.Remove(State->LocalUserNum);
                            }
                        }
                    }));
#endif // #if EOS_HAS_AUTHENTICATION
#if defined(EOS_IS_FREE_EDITION)
            }
        });
#endif
    return true;
}

bool FOnlineIdentityInterfaceEOS::Logout(int32 LocalUserNum)
{
    if (this->LoggedInUsers.Contains(LocalUserNum))
    {
        UE_LOG(
            LogEOSIdentity,
            Verbose,
            TEXT("Local user %d is being signed out (was signed in as %s)"),
            LocalUserNum,
            *this->LoggedInUsers[LocalUserNum]->ToString());

#if EOS_HAS_AUTHENTICATION
        if (this->IsDedicatedServer)
#endif // #if EOS_HAS_AUTHENTICATION
        {
            // If we are running as a dedicated server, we don't perform any kind of logout. Instead,
            // just remove the account from the logged in user list.
            this->UpdateUniqueNetIdForPlayerController(LocalUserNum, nullptr);
#if EOS_HAS_AUTHENTICATION
            if (this->LoggedInUsers.Contains(LocalUserNum) &&
                this->ProductUserIdToCrossPlatformAccountId.Contains(*this->LoggedInUsers[LocalUserNum]))
            {
                this->ProductUserIdToCrossPlatformAccountId.Remove(*this->LoggedInUsers[LocalUserNum]);
            }
#endif // #if EOS_HAS_AUTHENTICATION
            this->LoggedInUsers.Remove(LocalUserNum);
            this->LoggedInUserNativeSubsystemNames.Remove(LocalUserNum);
            this->LoggedInAccounts.Remove(LocalUserNum);
#if EOS_HAS_AUTHENTICATION
            this->RefreshCallbacks.Remove(LocalUserNum);
#endif // #if EOS_HAS_AUTHENTICATION
            this->TriggerOnLogoutCompleteDelegates(LocalUserNum, true);
            this->TriggerOnLoginStatusChangedDelegates(
                LocalUserNum,
                ELoginStatus::LoggedIn,
                ELoginStatus::NotLoggedIn,
                *FUniqueNetIdEOS::InvalidId());
            this->TriggerOnLoginChangedDelegates(LocalUserNum);
            UE_LOG(LogEOS, Verbose, TEXT("DedicatedServer: Performed Logout on dedicated server."));
            UE_LOG(LogEOSIdentity, Verbose, TEXT("Local user %d is now signed out"), LocalUserNum);
            return true;
        }

#if EOS_HAS_AUTHENTICATION
        // On some platforms, we also need to clear the refresh token out of storage.
        this->RuntimePlatform->ClearStoredEASRefreshToken(LocalUserNum);

        EOS_Auth_DeletePersistentAuthOptions Opts = {};
        Opts.ApiVersion = EOS_AUTH_DELETEPERSISTENTAUTH_API_LATEST;
        Opts.RefreshToken = nullptr;

        EOSRunOperation<EOS_HAuth, EOS_Auth_DeletePersistentAuthOptions, EOS_Auth_DeletePersistentAuthCallbackInfo>(
            this->EOSAuth,
            &Opts,
            &EOS_Auth_DeletePersistentAuth,
            [WeakThis = GetWeakThis(this), LocalUserNum](const EOS_Auth_DeletePersistentAuthCallbackInfo *Data) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    if (Data->ResultCode != EOS_EResult::EOS_Success)
                    {
                        UE_LOG(
                            LogEOS,
                            Warning,
                            TEXT("Could not delete persistent authentication token on logout: %s"),
                            ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
                    }

                    This->UpdateUniqueNetIdForPlayerController(LocalUserNum, nullptr);
#if defined(EOS_VOICE_CHAT_SUPPORTED)
                    if (This->LoggedInUsers.Contains(LocalUserNum))
                    {
                        This->VoiceManager->RemoveLocalUser(*StaticCastSharedRef<const FUniqueNetIdEOS>(
                            This->LoggedInUsers[LocalUserNum].ToSharedRef()));
                    }
#endif
                    if (This->LoggedInUsers.Contains(LocalUserNum) &&
                        This->ProductUserIdToCrossPlatformAccountId.Contains(*This->LoggedInUsers[LocalUserNum]))
                    {
                        This->ProductUserIdToCrossPlatformAccountId.Remove(*This->LoggedInUsers[LocalUserNum]);
                    }
                    if (This->LoggedInUsers.Contains(LocalUserNum) &&
                        This->PlatformIdMappings.Contains(*This->LoggedInUsers[LocalUserNum]))
                    {
                        FEOSPlatformUserIdManager::DeallocatePlatformId(
                            This->PlatformIdMappings[*This->LoggedInUsers[LocalUserNum]]);
                        This->PlatformIdMappings.Remove(*This->LoggedInUsers[LocalUserNum]);
                    }
                    This->LoggedInUsers.Remove(LocalUserNum);
                    This->LoggedInUserNativeSubsystemNames.Remove(LocalUserNum);
                    This->LoggedInAccounts.Remove(LocalUserNum);
                    This->RefreshCallbacks.Remove(LocalUserNum);
                    This->IdentityEAS->UserSignedOut(LocalUserNum);
                    This->TriggerOnLogoutCompleteDelegates(LocalUserNum, true);
                    This->TriggerOnLoginStatusChangedDelegates(
                        LocalUserNum,
                        ELoginStatus::LoggedIn,
                        ELoginStatus::NotLoggedIn,
                        *FUniqueNetIdEOS::InvalidId());
                    This->TriggerOnLoginChangedDelegates(LocalUserNum);
                    UE_LOG(LogEOSIdentity, Verbose, TEXT("Local user %d is now signed out"), LocalUserNum);
                }
            });

        return true;
#endif // #if EOS_HAS_AUTHENTICATION
    }
    else
    {
        UE_LOG(LogEOS, Error, TEXT("Local user %d is not logged in, can not log them out"), LocalUserNum);
        this->TriggerOnLogoutCompleteDelegates(LocalUserNum, false);
        return true;
    }
}

bool FOnlineIdentityInterfaceEOS::AutoLogin(int32 LocalUserNum)
{
    return this->Login(LocalUserNum, FOnlineAccountCredentials());
}

TSharedPtr<FUserOnlineAccount> FOnlineIdentityInterfaceEOS::GetUserAccount(const FUniqueNetId &UserId) const
{
    for (const auto &UserAccount : this->LoggedInAccounts)
    {
        if (UserAccount.Value->GetUserId().Get() == UserId)
        {
            return UserAccount.Value;
        }
    }

    return nullptr;
}

TArray<TSharedPtr<FUserOnlineAccount>> FOnlineIdentityInterfaceEOS::GetAllUserAccounts() const
{
    TArray<TSharedPtr<FUserOnlineAccount>> Results;
    for (const auto &UserAccount : this->LoggedInAccounts)
    {
        Results.Add(UserAccount.Value);
    }
    return Results;
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityInterfaceEOS::GetUniquePlayerId(int32 LocalUserNum) const
{
#if !defined(PLATFORM_LINUX) || !PLATFORM_LINUX
    if (LocalUserNum < MAX_LOCAL_PLAYERS)
    {
#endif
        if (this->LoggedInUsers.Contains(LocalUserNum))
        {
            return *this->LoggedInUsers.Find(LocalUserNum);
        }
        else
        {
            return nullptr;
        }
#if !defined(PLATFORM_LINUX) || !PLATFORM_LINUX
    }
    else
    {
        UE_LOG(LogEOS, Error, TEXT("Invalid user %d passed to GetUniquePlayerId"), LocalUserNum);
        return nullptr;
    }
#endif
}

bool FOnlineIdentityInterfaceEOS::GetLocalUserNum(const FUniqueNetId &UniqueNetId, int32 &OutLocalUserNum) const
{
    for (const auto &KV : this->LoggedInUsers)
    {
        if (KV.Value.IsValid() && *KV.Value == UniqueNetId)
        {
            OutLocalUserNum = KV.Key;
            return true;
        }
    }

    OutLocalUserNum = 0;
    return false;
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityInterfaceEOS::CreateUniquePlayerId(uint8 *Bytes, int32 Size)
{
    FString Data;
    Data.Empty(Size);
    for (int i = 0; i < Size; i++)
    {
        if (Bytes[i] == 0)
        {
            break;
        }
        Data += (ANSICHAR)Bytes[i];
    }
    return FUniqueNetIdEOS::ParseFromString(Data);
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityInterfaceEOS::CreateUniquePlayerId(const FString &Str)
{
    return FUniqueNetIdEOS::ParseFromString(Str);
}

ELoginStatus::Type FOnlineIdentityInterfaceEOS::GetLoginStatus(int32 LocalUserNum) const
{
    if (this->LoggedInUsers.Contains(LocalUserNum))
    {
        return ELoginStatus::LoggedIn;
    }

    return ELoginStatus::NotLoggedIn;
}

ELoginStatus::Type FOnlineIdentityInterfaceEOS::GetLoginStatus(const FUniqueNetId &UserId) const
{
    // Reverse search in the logged in users list to find it.
    for (const auto &Kv : this->LoggedInUsers)
    {
        if (Kv.Value.ToSharedRef().Get() == UserId)
        {
            return ELoginStatus::LoggedIn;
        }
    }

    return ELoginStatus::NotLoggedIn;
}

FString FOnlineIdentityInterfaceEOS::GetPlayerNickname(int32 LocalUserNum) const
{
    if (this->LoggedInAccounts.Contains(LocalUserNum))
    {
        return this->LoggedInAccounts.Find(LocalUserNum)->Get()->GetDisplayName();
    }

    return TEXT("");
}

FString FOnlineIdentityInterfaceEOS::GetPlayerNickname(const FUniqueNetId &UserId) const
{
    for (const auto &Kv : this->LoggedInAccounts)
    {
        if (Kv.Value->GetUserId().Get() == UserId)
        {
            return Kv.Value->GetDisplayName();
        }
    }

    return TEXT("");
}

FString FOnlineIdentityInterfaceEOS::GetAuthToken(int32 LocalUserNum) const
{
    if (this->LoggedInAccounts.Contains(LocalUserNum))
    {
        return this->LoggedInAccounts.Find(LocalUserNum)->Get()->GetAccessToken();
    }

    return TEXT("");
}

void FOnlineIdentityInterfaceEOS::RevokeAuthToken(
    const FUniqueNetId &LocalUserId,
    const FOnRevokeAuthTokenCompleteDelegate &Delegate)
{
    Delegate.ExecuteIfBound(LocalUserId, ONLINE_ERROR(EOnlineErrorResult::NotImplemented));
}

void FOnlineIdentityInterfaceEOS::GetUserPrivilege(
    const FUniqueNetId &LocalUserId,
    EUserPrivileges::Type Privilege,
    const FOnGetUserPrivilegeCompleteDelegate &Delegate)
{
    // TODO: When the EOS SDK surfaces this information, use it instead of returning success for everything.

    switch (Privilege)
    {
    case EUserPrivileges::Type::CanPlay:
    case EUserPrivileges::Type::CanPlayOnline:
    case EUserPrivileges::Type::CanCommunicateOnline:
    case EUserPrivileges::Type::CanUseUserGeneratedContent:
    case EUserPrivileges::Type::CanUserCrossPlay:
        Delegate.ExecuteIfBound(LocalUserId, Privilege, (uint32)IOnlineIdentity::EPrivilegeResults::NoFailures);
        break;
    default:
        // Unknown privilege.
        Delegate.ExecuteIfBound(LocalUserId, Privilege, (uint32)IOnlineIdentity::EPrivilegeResults::GenericFailure);
        break;
    }
}

FPlatformUserId FOnlineIdentityInterfaceEOS::GetPlatformUserIdFromUniqueNetId(const FUniqueNetId &UniqueNetId) const
{
#if EOS_HAS_AUTHENTICATION
    if (this->PlatformIdMappings.Contains(UniqueNetId))
    {
        return this->PlatformIdMappings[UniqueNetId];
    }
#endif // #if EOS_HAS_AUTHENTICATION

#if defined(UE_5_0_OR_LATER)
    return PLATFORMUSERID_NONE;
#else
    return 0;
#endif // #if defined(UE_5_0_OR_LATER)
}

FString FOnlineIdentityInterfaceEOS::GetAuthType() const
{
    return TEXT("");
}

#if EOS_HAS_AUTHENTICATION

TSharedPtr<const FCrossPlatformAccountId> FOnlineIdentityInterfaceEOS::GetCrossPlatformAccountId(
    const FUniqueNetId &UniqueNetId) const
{
    if (this->ProductUserIdToCrossPlatformAccountId.Contains(UniqueNetId))
    {
        return this->ProductUserIdToCrossPlatformAccountId[UniqueNetId];
    }

    return nullptr;
}

bool FOnlineIdentityInterfaceEOS::IsCrossPlatformAccountProviderAvailable() const
{
    return !this->Config->GetCrossPlatformAccountProvider().IsEqual(NAME_None);
}

#endif // #if EOS_HAS_AUTHENTICATION

IOnlineSubsystem *FOnlineIdentityInterfaceEOS::GetNativeSubsystem(
    int32 LocalUserNum,
    bool bObtainRedpointImplementation) const
{
    if (!this->LoggedInUserNativeSubsystemNames.Contains(LocalUserNum))
    {
        if (bObtainRedpointImplementation)
        {
            return nullptr;
        }
        else
        {
            return IOnlineSubsystem::GetByPlatform();
        }
    }

#if EOS_HAS_AUTHENTICATION
    if (this->LoggedInUserNativeSubsystemNames[LocalUserNum].IsEqual(REDPOINT_EAS_SUBSYSTEM))
    {
        if (auto EAS = this->SubsystemEOS.Pin())
        {
            return EAS->SubsystemEAS.Get();
        }
        else
        {
            return nullptr;
        }
    }
#endif // #if EOS_HAS_AUTHENTICATION

    TSoftObjectPtr<UWorld> World = FWorldResolution::GetWorld(FName(*this->InstanceName), true);
    if (!World.IsValid())
    {
        if (bObtainRedpointImplementation)
        {
            return nullptr;
        }
        else
        {
            return IOnlineSubsystem::GetByPlatform();
        }
    }

    return Online::GetSubsystem(
        World.Get(),
        bObtainRedpointImplementation ? FName(*FString::Printf(
                                            TEXT("Redpoint%s"),
                                            *this->LoggedInUserNativeSubsystemNames[LocalUserNum].ToString()))
                                      : this->LoggedInUserNativeSubsystemNames[LocalUserNum]);
}

IOnlineSubsystem *FOnlineIdentityInterfaceEOS::GetNativeSubsystem(
    const FUniqueNetId &UserIdEOS,
    bool bObtainRedpointImplementation) const
{
    int32 LocalUserNum;
    if (this->GetLocalUserNum(UserIdEOS, LocalUserNum))
    {
        return this->GetNativeSubsystem(LocalUserNum, bObtainRedpointImplementation);
    }
    return nullptr;
}

EOS_DISABLE_STRICT_WARNINGS
