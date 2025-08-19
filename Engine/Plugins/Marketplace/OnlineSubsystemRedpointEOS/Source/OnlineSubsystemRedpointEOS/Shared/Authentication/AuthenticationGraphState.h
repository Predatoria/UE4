// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Misc/Variant.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraphEOSCandidate.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/CrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/OnlineExternalCredentials.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/UserInterface/EOSUserInterface_SignInOrCreateAccount_Choice.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/UserInterface/EOSUserInterface_SwitchToCrossPlatformAccount_Choice.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSString.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

#ifndef EOS_AUTH_ATTRIBUTE_AUTHENTICATEDWITH
#define EOS_AUTH_ATTRIBUTE_AUTHENTICATEDWITH TEXT("authenticatedWith")
#endif

class ONLINESUBSYSTEMREDPOINTEOS_API FAuthenticationGraphState : public TSharedFromThis<FAuthenticationGraphState>
{
    friend class FAuthenticationGraph;

private:
    /**
     * The selected EOS candidate - FLoginWithSelectedEOSAccountNode will call CreateUser if the candidate only has a
     * continuation token.
     */
    FAuthenticationGraphEOSCandidate SelectedEOSCandidate;
    bool HasSelectedEOSCandidateFlag;

    /**
     * The current user interface widget displayed to the user.
     */
    TSoftObjectPtr<class UUserWidget> CurrentWidget;
    bool bWasMouseCursorShown;

    /** The last cached world instance. */
    TSoftObjectPtr<UWorld> LastCachedWorld;

    /** A list of nodes to run if the authentication graph fails. */
    TArray<TSharedPtr<class FAuthenticationGraphNode>> CleanupNodesOnError;

public:
    FAuthenticationGraphState(
        const TSharedRef<class FOnlineSubsystemEOS, ESPMode::ThreadSafe> &InSubsystem,
        int32 InLocalUserNum,
        FName InWorldContextHandle,
        const TSharedRef<FEOSConfig> &InConfig);
    ~FAuthenticationGraphState();
    UE_NONCOPYABLE(FAuthenticationGraphState);

    /** The current subsystem. This must be a weak pointer. */
    TWeakPtr<class FOnlineSubsystemEOS, ESPMode::ThreadSafe> Subsystem;

    /** The current configuration. */
    TSharedPtr<FEOSConfig> Config;

    /** The EOS SDK platform handle. */
    EOS_HPlatform EOSPlatform;

    /** The EOS Auth interface. */
    EOS_HAuth EOSAuth;

    /** The EOS Connect interface. */
    EOS_HConnect EOSConnect;

    /** The EOS User Info interface. */
    EOS_HUserInfo EOSUserInfo;

    /** The local user number of the user that is being authenticated. */
    int32 LocalUserNum;

    /**
     * If this authentication process is being run with an already authenticated user, this is their existing user ID.
     */
    TSharedPtr<const class FUniqueNetIdEOS> ExistingUserId;

    /**
     * If this authentication process is being run with an already authenticated user, these are the external
     * credentials that were originally used to authenticate them. This field can be used by cross-platform account
     * providers to link the local platform credentials against a cross-platform account.
     */
    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
    TSharedPtr<const IOnlineExternalCredentials> ExistingExternalCredentials;

    /**
     * If this authentication process is being run with an already authenticated user, this is their cross-platform
     * account ID.
     */
    TSharedPtr<const class FCrossPlatformAccountId> ExistingCrossPlatformAccountId;

    /**
     * These credentials are not used by any of the default authentication flows, but if you want to pass custom data
     * into a custom authentication graph, you can use the ::Login method on the identity interface and the credentials
     * you provide to that function will be available here.
     */
    FOnlineAccountCredentials ProvidedCredentials;

    /** A list of error messages that occurred during authentication. */
    TArray<FString> ErrorMessages;

    /**
     * The provider for cross-platform accounts.
     */
    TSharedPtr<ICrossPlatformAccountProvider> CrossPlatformAccountProvider;

    /**
     * The authenticated cross-platform account ID, present when a cross-platform provider is enabled and the user has
     * been signed into a cross-platform account.
     *
     * Note that this is NOT the field that is used to determine if the user has logged into the game with a
     * cross-platform account. This field is just used internally between cross-platform authentication graph nodes to
     * track the cross-platform account ID.
     *
     * The cross-platform account ID must still eventually end up associated with an EOS candidate and stored in the
     * AssociatedCrossPlatformAccountId field. Then, if the user selects this EOS candidate, it will be placed into the
     * ResultCrossPlatformAccountId field which is what FOnlineIdentityEOS reads to actually set the user's logged in
     * state.
     */
    TSharedPtr<const class FCrossPlatformAccountId> AuthenticatedCrossPlatformAccountId;

    /**
     * Casts and returns the account ID to the given type if the account ID belongs to the expected provider.
     */
    template <typename T> TSharedPtr<const T> GetAuthenticatedCrossPlatformAccountId(FName ProviderName) const
    {
        if (this->CrossPlatformAccountProvider.IsValid() &&
            this->CrossPlatformAccountProvider->GetName().IsEqual(ProviderName))
        {
            return StaticCastSharedPtr<const T>(this->AuthenticatedCrossPlatformAccountId);
        }
        return nullptr;
    }

    /**
     * DEPRECATED: Please do not use this function in new code.
     */
    EOS_EpicAccountId GetAuthenticatedEpicAccountId() const;

    /**
     * The available external credentials collected by the authentication graph. These credentials may be used by a
     * cross-platform account provider to try to implicitly sign the user into a cross-platform account.
     */
    TArray<TSharedRef<IOnlineExternalCredentials>> AvailableExternalCredentials;

    /**
     * Keeps track of the credential names we've already tried with the EAS Developer Authentication Tool so we don't
     * bother making duplicate requests.
     */
    TArray<FString> AttemptedDeveloperAuthenticationCredentialNames;

    /** The name of the current world context handle. */
    FName WorldContextHandle;

    /** A list of EOS candidates that the authentication process can select from. */
    TArray<FAuthenticationGraphEOSCandidate> EOSCandidates;

    /** The authenticated user ID from a successful login process. */
    TSharedPtr<const class FUniqueNetIdEOS> ResultUserId;

    /** The external credentials that were used to authenticate the user in ResultUserId. */
    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
    TSharedPtr<IOnlineExternalCredentials> ResultExternalCredentials;

    /** The authenticated cross-platform account ID from a successful login process. */
    TSharedPtr<const class FCrossPlatformAccountId> ResultCrossPlatformAccountId;

    /** The callback used to refresh the credentials and call EOS_Connect_Login for the authenticated user. */
    FAuthenticationGraphRefreshEOSCredentials ResultRefreshCallback;

    /** The native subsystem to use for external UI and e-commerce. */
    FName ResultNativeSubsystemName;

    /** The last choice made with regards to switching to a cross-platform account. */
    EEOSUserInterface_SwitchToCrossPlatformAccount_Choice LastSwitchChoice;

    /** The last choice made with regards to signing in or creating an account. */
    EEOSUserInterface_SignInOrCreateAccount_Choice LastSignInChoice;

#if !UE_BUILD_SHIPPING
    /** The email address to use for automated testing login. */
    FString AutomatedTestingEmailAddress;

    /** The password to use for automated testing login. */
    FString AutomatedTestingPassword;
#endif

    /** Storage of custom metadata between nodes. You can store whatever you like in this field. */
    TMap<FString, FVariant> Metadata;

    /** Storage of an EAS external continuance token, as it can not be stored portably inside Metadata. */
    EOS_ContinuanceToken EASExternalContinuanceToken;

    /** The attributes will be propagated to the FUserOnlineAccountEOS and available through GetAuthAttribute. */
    TMap<FString, FString> ResultUserAuthAttributes;

    /** Gets a reference to the current active world, or null if no world could be found. */
    UWorld *GetWorld();

    /**
     * Adds an EOS candidate based on IOnlineExternalCredentials. Automatically handles wiring up the refresh callback.
     *
     * @param Data                      The result of the EOS_Connect_Login call.
     * @param ExternalCredentials       The external credentials to use.
     * @param InType                    The type of credential that this is.
     * @param InCrossPlatformAccountId  If this is a cross-platform credential (via InType), this is the cross-platform
     *                                  account ID.
     */
    FAuthenticationGraphEOSCandidate AddEOSConnectCandidateFromExternalCredentials(
        const EOS_Connect_LoginCallbackInfo *Data,
        const TSharedRef<IOnlineExternalCredentials> &ExternalCredentials,
        EAuthenticationGraphEOSCandidateType InType = EAuthenticationGraphEOSCandidateType::Generic,
        TSharedPtr<const FCrossPlatformAccountId> InCrossPlatformAccountId = nullptr);

    /**
     * Adds an EOS candidate manually.
     *
     * @param ProviderDisplayName       The display name of the provider of this credential.
     * @param UserAuthAttributes        The user authentication attributes to provide if this credential is used to sign
     *                                  in.
     * @param Data                      The result of the EOS_Connect_Login call.
     * @param RefreshCallback           The callback used to refresh login credentials when needed.
     * @param InNativeSubsystemName     The native subsystem to use for external UI and e-commerce.
     * @param InType                    The type of credential that this is.
     * @param InCrossPlatformAccountId  If this is a cross-platform credential (via InType), this is the cross-platform
     *                                  account ID.
     */
    FAuthenticationGraphEOSCandidate AddEOSConnectCandidate(
        FText ProviderDisplayName,
        TMap<FString, FString> UserAuthAttributes,
        const EOS_Connect_LoginCallbackInfo *Data,
        FAuthenticationGraphRefreshEOSCredentials RefreshCallback,
        const FName &InNativeSubsystemName,
        EAuthenticationGraphEOSCandidateType InType = EAuthenticationGraphEOSCandidateType::Generic,
        TSharedPtr<const FCrossPlatformAccountId> InCrossPlatformAccountId = nullptr);

    /** Select the specified EOS candidate as the one to authenticate with. */
    void SelectEOSCandidate(const FAuthenticationGraphEOSCandidate &Candidate);

    /** Returns whether or not an EOS candidate has been selected. */
    bool HasSelectedEOSCandidate();

    /** Returns the currently selected EOS candidate. Only valid if HasSelectedEOSCandidate returns true. */
    FAuthenticationGraphEOSCandidate GetSelectedEOSCandidate();

    /** Internally used by UserInterfaceRef. Do not call this function directly. */
    bool HasCurrentUserInterfaceWidget();

    /** Internally used by UserInterfaceRef. Do not call this function directly. */
    void SetCurrentUserInterfaceWidget(class UUserWidget *InWidget);

    /** Internally used by UserInterfaceRef. Do not call this function directly. */
    void ClearCurrentUserInterfaceWidget();

    /** Add a cleanup node to the authentication graph. This node will be run if the authentication graph errors. */
    void AddCleanupNode(const TSharedRef<class FAuthenticationGraphNode> &Node);
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION