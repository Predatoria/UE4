// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "Authentication/AuthenticationGraph.h"
#include "Authentication/AuthenticationHelpers.h"
#include "Containers/Map.h"
#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "Delegates/DelegateCombinations.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Misc/Exec.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/CrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"

EOS_ENABLE_STRICT_WARNINGS

DECLARE_DELEGATE_TwoParams(FEOSLoginInternalComplete, bool /* bWasSuccessful */, FString /* ErrorMessage */);

DECLARE_DELEGATE_OneParam(FEOSConnect_DoRequestComplete, const EOS_Connect_LoginCallbackInfo * /* Data */);
DECLARE_DELEGATE_FourParams(
    FEOSIdentity_LoginComplete,
    bool /* bWasSuccessful */,
    TSharedPtr<FUniqueNetIdEOS> /* UserId */,
    EOS_ContinuanceToken /* ContinuanceToken */,
    FString /* ErrorMessage */);
DECLARE_DELEGATE(FEOSIdentity_GenericTask);
DECLARE_DELEGATE_OneParam(FEOSIdentity_AccountLoaded, TSharedPtr<FUserOnlineAccount> /* UserAccount */);

class ONLINESUBSYSTEMREDPOINTEOS_API FOnlineIdentityInterfaceEOS
    : public IOnlineIdentity,
      public TSharedFromThis<FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe>,
      public FExec
{
    friend class UEOSControlChannel;
    friend class FOnlineSubsystemEOS;
    friend class FUserOnlineAccountEOS;
    friend class FLegacyCredentialAuthPhase;

private:
    EOS_HPlatform EOSPlatform;
    EOS_HConnect EOSConnect;
    EOS_HAuth EOSAuth;
    bool IsDedicatedServer;
    FString InstanceName;
    TWeakPtr<class FOnlineSubsystemEOS, ESPMode::ThreadSafe> SubsystemEOS;
    TSharedPtr<class FOnlineIdentityInterfaceEAS, ESPMode::ThreadSafe> IdentityEAS;
    TSharedPtr<class IEOSRuntimePlatform> RuntimePlatform;
    TSharedPtr<class FEOSConfig> Config;
    TSharedPtr<class FEOSUserFactory, ESPMode::ThreadSafe> UserFactory;
    TSharedPtr<EOSEventHandle<EOS_Connect_AuthExpirationCallbackInfo>> Unregister_AuthExpiration;
    TSharedPtr<EOSEventHandle<EOS_Connect_LoginStatusChangedCallbackInfo>> Unregister_LoginStatusChanged;
    TWeakPtr<class FOnlineSubsystemEOS, ESPMode::ThreadSafe> OSSInstance;
#if defined(EOS_VOICE_CHAT_SUPPORTED)
    TSharedPtr<class FEOSVoiceManager> VoiceManager;
#endif

    /** Map of local user num to unique net IDs. */
    TMap<int32, TSharedPtr<const FUniqueNetId>> LoggedInUsers;

    /** Map of local user num to logged in accounts. */
    TMap<int32, TSharedPtr<FUserOnlineAccount>> LoggedInAccounts;

    /** Map of local user num to native subsystem names */
    TMap<int32, FName> LoggedInUserNativeSubsystemNames;

#if EOS_HAS_AUTHENTICATION
    /** Map of local user num to their refresh callbacks. */
    TMap<int32, FAuthenticationGraphRefreshEOSCredentials> RefreshCallbacks;

    /** Map of local user IDs to the external credentials used to sign them in. */
    TUserIdMap<TSharedRef<class IOnlineExternalCredentials>> ExternalCredentials;

    /**
     * A mapping from local user IDs to cross-platform account IDs. Used by some internal services to discover the Epic
     * account ID for a local user.
     */
    TUserIdMap<TSharedPtr<const FCrossPlatformAccountId>> ProductUserIdToCrossPlatformAccountId;

    /**
     * A mapping of user IDs to their platform IDs.
     */
    TUserIdMap<FPlatformUserId> PlatformIdMappings;
#endif // #if EOS_HAS_AUTHENTICATION

    /**
     * A list of users that are currently being authenticated, and are still waiting for the
     * login process to complete.
     *
     * NOTE: This map still exists in UE_SERVER, because the Free Edition must validate it's license. We just
     *       use a nullptr to fill this map while that's happening.
     */
    TMap<int32, TSharedPtr<class FAuthenticationGraph>> AuthenticationInProgress;

    void DumpLoggedInUsers();

    /**
     * Loads account information for the specified user ID.
     */
    void EOS_LoadAccountInformation(
        const TSharedPtr<const FUniqueNetIdEOS> &UserId,
        const TMap<FString, FString> &InAuthAttributes,
        const FEOSIdentity_AccountLoaded &OnComplete);

    /**
     * Updates the ULocalPlayer and APlayerState for a player controller, setting the cached UniqueNetId for the given
     * local user.
     */
    void UpdateUniqueNetIdForPlayerController(
        int InLocalPlayerNum,
        const TSharedPtr<const FUniqueNetId> &InNewUniqueId);

#if EOS_HAS_AUTHENTICATION
    void HandleLoginExpiry(int32 InLocalPlayerNum);
#endif // #if EOS_HAS_AUTHENTICATION

#if EOS_HAS_AUTHENTICATION
#if WITH_EDITOR
    bool CheckIfReadyToFinishEditorLogin(
        float DeltaSeconds,
        const TSharedPtr<class FAuthenticationGraphState> InState,
        const TSharedPtr<FUserOnlineAccount> InUserAccount);
#endif // #if WITH_EDITOR
    void FinishLogin(
        const TSharedPtr<class FAuthenticationGraphState> &InState,
        const TSharedPtr<FUserOnlineAccount> &InUserAccount);
#endif // #if EOS_HAS_AUTHENTICATION

public:
    FOnlineIdentityInterfaceEOS(
        const TSharedRef<class FOnlineSubsystemEOS, ESPMode::ThreadSafe> &InSubsystemEOS,
        EOS_HPlatform InPlatform,
        const FString &InInstanceName,
        bool InIsDedicatedServer,
        TSharedPtr<class FOnlineIdentityInterfaceEAS, ESPMode::ThreadSafe> InIdentityEAS,
        const TSharedRef<class IEOSRuntimePlatform> &InRuntimePlatform,
        const TSharedRef<class FEOSConfig> &InConfig,
        const TSharedRef<class FEOSUserFactory, ESPMode::ThreadSafe> &InUserFactory,
        const TSharedRef<class FOnlineSubsystemEOS, ESPMode::ThreadSafe> &InOSSInstance);
    UE_NONCOPYABLE(FOnlineIdentityInterfaceEOS);
    void RegisterEvents();

    virtual bool Exec(UWorld *InWorld, const TCHAR *Cmd, FOutputDevice &Ar) override;

    virtual bool Login(int32 LocalUserNum, const FOnlineAccountCredentials &AccountCredentials) override;
    virtual bool Logout(int32 LocalUserNum) override;
    virtual bool AutoLogin(int32 LocalUserNum) override;
    virtual TSharedPtr<FUserOnlineAccount> GetUserAccount(const FUniqueNetId &UserId) const override;
    virtual TArray<TSharedPtr<FUserOnlineAccount>> GetAllUserAccounts() const override;
    virtual TSharedPtr<const FUniqueNetId> GetUniquePlayerId(int32 LocalUserNum) const override;
    /** Returns the local user number of the given player. */
    virtual bool GetLocalUserNum(const FUniqueNetId &UniqueNetId, int32 &OutLocalUserNum) const;
    virtual TSharedPtr<const FUniqueNetId> CreateUniquePlayerId(uint8 *Bytes, int32 Size) override;
    virtual TSharedPtr<const FUniqueNetId> CreateUniquePlayerId(const FString &Str) override;
    virtual ELoginStatus::Type GetLoginStatus(int32 LocalUserNum) const override;
    virtual ELoginStatus::Type GetLoginStatus(const FUniqueNetId &UserId) const override;
    virtual FString GetPlayerNickname(int32 LocalUserNum) const override;
    virtual FString GetPlayerNickname(const FUniqueNetId &UserId) const override;
    virtual FString GetAuthToken(int32 LocalUserNum) const override;
    virtual void RevokeAuthToken(const FUniqueNetId &LocalUserId, const FOnRevokeAuthTokenCompleteDelegate &Delegate)
        override;
    virtual void GetUserPrivilege(
        const FUniqueNetId &LocalUserId,
        EUserPrivileges::Type Privilege,
        const FOnGetUserPrivilegeCompleteDelegate &Delegate) override;
    virtual FPlatformUserId GetPlatformUserIdFromUniqueNetId(const FUniqueNetId &UniqueNetId) const override;
    virtual FString GetAuthType() const override;
#if EOS_HAS_AUTHENTICATION
    /** Returns the cross-platform account ID of the given local player. */
    virtual TSharedPtr<const FCrossPlatformAccountId> GetCrossPlatformAccountId(const FUniqueNetId &UniqueNetId) const;
    /** Returns true if a cross-platform account provider is available. */
    virtual bool IsCrossPlatformAccountProviderAvailable() const;
#endif // #if EOS_HAS_AUTHENTICATION
    virtual IOnlineSubsystem *GetNativeSubsystem(int32 LocalUserNum, bool bObtainRedpointImplementation = false) const;
    virtual IOnlineSubsystem *GetNativeSubsystem(
        const FUniqueNetId &UserIdEOS,
        bool bObtainRedpointImplementation = false) const;
    template <typename TInterface>
    TSharedPtr<TInterface, ESPMode::ThreadSafe> GetNativeInterface(
        int32 LocalUserNum,
        TSharedPtr<const FUniqueNetId> &OutNativeUserId,
        std::function<TSharedPtr<TInterface, ESPMode::ThreadSafe>(IOnlineSubsystem *OSS)> Accessor)
    {
        // Get the native user IDs, which callers will need to use for passthrough.
        OutNativeUserId = nullptr;
        IOnlineSubsystem *NativeOSS = this->GetNativeSubsystem(LocalUserNum, false);
        if (NativeOSS == nullptr)
        {
            return nullptr;
        }
        TSharedPtr<IOnlineIdentity, ESPMode::ThreadSafe> NativeIdentity = NativeOSS->GetIdentityInterface();
        if (!NativeIdentity.IsValid())
        {
            return nullptr;
        }
        OutNativeUserId = NativeIdentity->GetUniquePlayerId(LocalUserNum);
        if (!OutNativeUserId.IsValid())
        {
            return nullptr;
        }

        // Prefer Redpoint implementation first.
        IOnlineSubsystem *RedpointOSS = this->GetNativeSubsystem(LocalUserNum, true);
        if (RedpointOSS != nullptr)
        {
            TSharedPtr<TInterface, ESPMode::ThreadSafe> RedpointInterface = Accessor(RedpointOSS);
            if (RedpointInterface.IsValid())
            {
                return RedpointInterface;
            }
        }

        // Then use native implementation if there's no Redpoint override.
        TSharedPtr<TInterface, ESPMode::ThreadSafe> NativeInterface = Accessor(NativeOSS);
        if (NativeInterface.IsValid())
        {
            return NativeInterface;
        }

        // Otherwise it's not available.
        return nullptr;
    }
    template <typename TInterface>
    TSharedPtr<TInterface, ESPMode::ThreadSafe> GetNativeInterface(
        const FUniqueNetId &UserIdEOS,
        TSharedPtr<const FUniqueNetId> &OutNativeUserId,
        std::function<TSharedPtr<TInterface, ESPMode::ThreadSafe>(IOnlineSubsystem *OSS)> Accessor)
    {
        int32 LocalUserNum;
        OutNativeUserId = nullptr;
        if (this->GetLocalUserNum(UserIdEOS, LocalUserNum))
        {
            return this->GetNativeInterface<TInterface>(LocalUserNum, OutNativeUserId, Accessor);
        }
        return nullptr;
    }
};

EOS_DISABLE_STRICT_WARNINGS
