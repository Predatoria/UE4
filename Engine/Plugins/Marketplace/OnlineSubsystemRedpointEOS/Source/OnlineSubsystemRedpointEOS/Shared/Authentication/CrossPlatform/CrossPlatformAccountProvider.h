// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/CompatHelpers.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

EOS_ENABLE_STRICT_WARNINGS

/**
 * A cross-platform account provider, which can sign users in and link accounts across plaforms.
 */
class ONLINESUBSYSTEMREDPOINTEOS_API ICrossPlatformAccountProvider
{
public:
    ICrossPlatformAccountProvider(){};
    UE_NONCOPYABLE(ICrossPlatformAccountProvider);
    virtual ~ICrossPlatformAccountProvider(){};

    /**
     * The name that this cross-platform account provider is registered under. It's expected that the
     * FCrossPlatformAccountId instances that this provider returns return the same value from GetType.
     */
    virtual FName GetName() = 0;

    /**
     * Creates a cross-platform account ID from it's string representation.
     */
    virtual TSharedPtr<const class FCrossPlatformAccountId> CreateCrossPlatformAccountId(
        const FString &InStringRepresentation) = 0;

    /**
     * Creates a cross-platform account ID from opaque data.
     */
    virtual TSharedPtr<const class FCrossPlatformAccountId> CreateCrossPlatformAccountId(
        uint8 *InBytes,
        int32 InSize) = 0;

    /**
     * Return the authentication graph node sequence that is used for interactive login into this cross-platform account
     * provider. This should include non-interactive attempts if necessary.
     *
     * This sequence is expected to place a value into the AuthenticatedCrossPlatformAccountId field of the
     * authentication state and add a cross-platform EOS candidate if successful.
     *
     * If the user can't be signed in interactively, this sequence of nodes should fail.
     */
    virtual TSharedRef<class FAuthenticationGraphNode> GetInteractiveAuthenticationSequence() = 0;

    /**
     * Return the authentication graph node sequence that is used for performing *only* interactive login into this
     * cross-platform account provider. This should NOT include non-interactive login attempts, as they are likely to
     * already have been performed through the use of GetNonInteractiveAuthenticationSequence.
     *
     * This sequence is expected to place a value into the AuthenticatedCrossPlatformAccountId field of the
     * authentication state and add a cross-platform EOS candidate if successful.
     *
     * If the user can't be signed in interactively, this sequence of nodes should fail.
     */
    virtual TSharedRef<class FAuthenticationGraphNode> GetInteractiveOnlyAuthenticationSequence() = 0;

    /**
     * Return the authentication graph node sequence that is used for non-interactive login into this cross-platform
     * account provider. This must never prompt the user or require interaction.
     *
     * This sequence is expected to place a value into the AuthenticatedCrossPlatformAccountId field of the
     * authentication state and add a cross-platform EOS candidate if successful.
     *
     * If the user can't be signed in using any non-interactive means, this sequence should complete with no error,
     * leaving AuthenticatedCrossPlatformId null.
     *
     * @param bOnlyUseExternalCredentials   If true, this node sequence must only use the external credentials already
     *                                      provided. It must not attempt to authenticate non-interactively using other
     *                                      sources such as exchange codes, developer authentication tools or persistent
     *                                      authentication.
     */
    virtual TSharedRef<class FAuthenticationGraphNode> GetNonInteractiveAuthenticationSequence(
        bool bOnlyUseExternalCredentials = false) = 0;

    /**
     * Returns the authentication graph node sequence which links the current account (the state will have
     * ExistingUserId populated) into a cross-platform account.
     *
     * This is effectively the upgrade flow from a platform-specific account to a cross-platform account. It is expected
     * to be interactive on some level.
     *
     * This sequence is expected to place a cross-platform candidate into the state via AddEOSConnectCandidate if
     * successful.
     *
     * If the current account can not be linked to a cross-platform account (for example, if the cross-platform account
     * already has separate user data to the current account), this sequence of nodes should fail.
     */
    virtual TSharedRef<class FAuthenticationGraphNode> GetUpgradeCurrentAccountToCrossPlatformAccountSequence() = 0;

    /**
     * Returns the authentication graph node sequence which links unused external credentials against the just
     * authenticated cross-platform account.
     *
     * Unlike GetUpgradeCurrentAccountToCrossPlatformAccountSequence, this takes external credentials not currently
     * associated with an account, and associates them with a cross-platform account.
     *
     * Upgrade Flow:
     *   - The user is already signed into an EOS account with an external credential and is playing the game.
     *   - They want to upgrade to a cross-platform account so they can sign in elsewhere.
     *   - Usually initiated interactively by the user pressing a button in the game's UI.
     *
     * Link Flow:
     *   - The user has just signed into a cross-platform account through the initial login process (potentially
     *     interactively).
     *   - There are external credentials on the local device that are not yet associated with any account.
     *   - Those external credentials should be linked against the cross-platform account, so the user doesn't have to
     *     do an interactive sign in next time they want to use their cross-platform account on this machine.
     *   - Usually automatic at the end of a successful login.
     *
     * Regardless of whether an account could be linked or whether there are even external credentials available, this
     * sequence of nodes should always succeed.
     */
    virtual TSharedRef<class FAuthenticationGraphNode>
    GetLinkUnusedExternalCredentialsToCrossPlatformAccountSequence() = 0;

    /**
     * Return the authentication graph node sequence that is used to sign out of any cross-platform accounts that
     * will no longer be used in this authentication graph.
     *
     * You should deauthenticate (logout) any accounts that are described by cross-platform candidates in the
     * authentication graph state. This sequence should then remove them from the list of candidates.
     *
     * This sequence is also expected to clear out the value of the AuthenticatedCrossPlatformAccountId field
     * of the authentication state if it is set.
     *
     * If you can't deauthenticate (logout) all relevant accounts, this sequence should complete with an error,
     * as continuing with cross-platform accounts still authenticated may result in undefined behaviour.
     */
    virtual TSharedRef<class FAuthenticationGraphNode> GetNonInteractiveDeauthenticationSequence() = 0;

    /**
     * Returns the authentication graph node sequence for signing into this cross-platform provider during automated
     * testing.
     *
     * This sequence is expected to place a cross-platform candidate into the state via AddEOSConnectCandidate if
     * successful.
     *
     * If authentication with this cross-platform provider for automated testing is not possible, this sequence of nodes
     * should fail.
     */
    virtual TSharedRef<class FAuthenticationGraphNode> GetAutomatedTestingAuthenticationSequence() = 0;
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION