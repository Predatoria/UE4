// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

EOS_ENABLE_STRICT_WARNINGS

/** Called by the implementor of the refresh callback to indicate that credentials have been refreshed. */
DECLARE_DELEGATE_OneParam(FAuthenticationGraphRefreshEOSCredentialsComplete, bool /* bWasSuccessful */);

/**
 * Information provided to the refresh callback such as whose credentials are being refreshed, and references to the EOS
 * APIs.
 *
 * It is NOT SAFE for refresh callbacks to capture references to the authentication graph or it's state, because the
 * graph and state will have been released by the time the refresh callback runs. Refresh callbacks are only permitted
 * to reference data that they were passed by FAuthenticationGraphRefreshEOSCredentialsInfo.
 */
struct ONLINESUBSYSTEMREDPOINTEOS_API FAuthenticationGraphRefreshEOSCredentialsInfo
    : public TSharedFromThis<FAuthenticationGraphRefreshEOSCredentialsInfo>
{
public:
    FAuthenticationGraphRefreshEOSCredentialsInfo(
        TSoftObjectPtr<UWorld> InWorld,
        int32 InLocalUserNum,
        EOS_HConnect InConnect,
        EOS_HAuth InAuth,
        const TMap<FString, FString> &InExistingUserAuthAttributes)
        : World(MoveTemp(InWorld))
        , LocalUserNum(InLocalUserNum)
        , EOSConnect(InConnect)
        , EOSAuth(InAuth)
        , ExistingUserAuthAttributes(InExistingUserAuthAttributes)
        , SetUserAuthAttributes()
        , DeleteUserAuthAttributes()
        , OnComplete(){};
    UE_NONCOPYABLE(FAuthenticationGraphRefreshEOSCredentialsInfo);

    /** The current world. */
    TSoftObjectPtr<UWorld> World;

    /** The local user whose credential is being refreshed. */
    int32 LocalUserNum;

    /** The EOS connect interface. */
    EOS_HConnect EOSConnect;

    /** The EOS auth interface. */
    EOS_HAuth EOSAuth;

    /** Entries in this map will be the existing auth attributes. If you e.g. need to refresh with a refresh token, you
     * can store the refresh token originally and then pull it out here (and then put the new refresh token in by
     * setting it into SetUserAuthAttributes. */
    TMap<FString, FString> ExistingUserAuthAttributes;

    /** Entries in this map will be added (or overwritten) in the auth attributes of the FOnlineUserAccountEOS. */
    TMap<FString, FString> SetUserAuthAttributes;

    /** Entries in this set will be deleted from the auth attributes of the FOnlineUserAccountEOS. */
    TSet<FString> DeleteUserAuthAttributes;

    /** After the refresh is successfully and the attribute maps have been filled, call this method to apply the changes
     * to the FOnlineUserAccountEOS. */
    FAuthenticationGraphRefreshEOSCredentialsComplete OnComplete;
};

/**
 * This type of delegate will be called by the identity subsystem when credentials need to be refreshed
 * for the associated EOS user. The implementation of this delegate must handle calling EOS_Connect_Login
 * for the EOS user. It's expected that the product user ID will be captured by the lambda that this event
 * is wired up to.
 *
 * It is NOT SAFE for the callback to capture any authentication graph pointers, as they will all have been
 * freed when this callback is invoked. Instead, use the information in the Info parameter.
 */
DECLARE_DELEGATE_OneParam(
    FAuthenticationGraphRefreshEOSCredentials,
    const TSharedRef<FAuthenticationGraphRefreshEOSCredentialsInfo> & /* Info */);

/**
 * The type of EOS candidate. This is used by authentication graph nodes to select particular candidates.
 */
enum class EAuthenticationGraphEOSCandidateType
{
    /** This is a generic EOS candidate. */
    Generic,

    /** This is a cross-platform candidate, such as Epic Games or another first party provider. */
    CrossPlatform,

    /** This is a device ID candidate. */
    DeviceId,

    /** This is a Nintendo NSA candidate. */
    NintendoNsa,

    /** This is a Nintendo ID candidate. */
    NintendoId,
};

/**
 * An EOS candidate capable of being used to authenticate the user.
 */
struct ONLINESUBSYSTEMREDPOINTEOS_API FAuthenticationGraphEOSCandidate
{
public:
    /** If this credential is displayed to the user as an option, what does it show to them? */
    FText DisplayName;

    /** The user auth attributes to set on FOnlineUserAccountEOS if this candidate is used. */
    TMap<FString, FString> UserAuthAttributes;

    /** The EOS product user ID; will only be present if this candidate has a valid EOS account. */
    EOS_ProductUserId ProductUserId;

    /** The continuance token if this account can be linked or created. */
    EOS_ContinuanceToken ContinuanceToken;

    /** The candidate type */
    EAuthenticationGraphEOSCandidateType Type;

    /** If this is a cross-platform candidate, this is the cross-platform account ID. */
    TSharedPtr<const class FCrossPlatformAccountId> AssociatedCrossPlatformAccountId;

    /**
     * If this candidate is selected, this is the delegate that will be used by the identity subsystem to refresh
     * credentials when needed. The implementation of this delegate *must* call EOS_Connect_Login.
     */
    FAuthenticationGraphRefreshEOSCredentials RefreshCallback;

    /**
     * The external credentials used to sign this candidate in.
     */
    // NOLINTNEXTLINE(unreal-unsafe-storage-of-oss-pointer)
    TSharedPtr<class IOnlineExternalCredentials> ExternalCredentials;

    /**
     * The name of the subsystem that should be used for external UI
     * and e-commerce if the user is authenticated with this candidate.
     */
    FName NativeSubsystemName;
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION