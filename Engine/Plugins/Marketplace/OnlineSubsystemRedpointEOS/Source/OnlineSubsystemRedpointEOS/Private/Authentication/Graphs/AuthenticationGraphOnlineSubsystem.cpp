// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Graphs/AuthenticationGraphOnlineSubsystem.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraphRegistry.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/AuthenticationGraphNodeConditional.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/AuthenticationGraphNodeUntil_Forever.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/AuthenticationGraphNodeUntil_LoginComplete.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/BailIfAlreadyAuthenticatedNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/BailIfNotExactlyOneExternalCredentialNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/FailAuthenticationNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/GatherEOSAccountsWithExternalCredentialsNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/GetExternalCredentialsFromOnlineSubsystemNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/LogUserIntoOnlineSubsystemNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/LoginWithSelectedEOSAccountNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/PromptToSignInOrCreateAccountNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/SelectCrossPlatformAccountNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/SelectOnlyEOSCandidateNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/SelectSingleContinuanceTokenEOSCandidateNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/SelectSingleSuccessfulEOSAccountNode.h"

EOS_ENABLE_STRICT_WARNINGS

void FAuthenticationGraphOnlineSubsystem::RegisterForCustomPlatform(
    FName InAuthenticationGraphName,
    const FText &GeneralDescription,
    FName InSubsystemName,
    EOS_EExternalCredentialType InCredentialType,
    const FString &InAuthenticatedWithValue,
    const FString &InTokenAuthAttributeName,
    const TSharedPtr<class FAuthenticationGraphNode> &InOverrideGetCredentialsNode)
{
    FAuthenticationGraphRegistry::Register(
        InAuthenticationGraphName,
        GeneralDescription,
        MakeShared<FAuthenticationGraphOnlineSubsystem>(
            InSubsystemName,
            InCredentialType,
            InAuthenticatedWithValue,
            InTokenAuthAttributeName,
            InOverrideGetCredentialsNode));
}

bool Condition_CanUpgradeToCrossPlatformAccount(const FAuthenticationGraphState &State)
{
    return State.ExistingUserId.IsValid() && !State.ExistingCrossPlatformAccountId.IsValid() &&
           State.CrossPlatformAccountProvider.IsValid();
}

TSharedRef<FAuthenticationGraphNode> FAuthenticationGraphOnlineSubsystem::CreateGraph(
    const TSharedRef<FAuthenticationGraphState> &InitialState)
{
    if (InitialState->CrossPlatformAccountProvider.IsValid())
    {
        if (InitialState->Config->GetRequireCrossPlatformAccount())
        {
            return MakeShared<FAuthenticationGraphNodeUntil_Forever>()
                ->Add(MakeShared<FBailIfAlreadyAuthenticatedNode>())
                ->Add(MakeShared<FLogUserIntoOnlineSubsystemNode>(this->SubsystemName))
                ->Add(
                    this->OverrideGetCredentialsNode.IsValid()
                        ? this->OverrideGetCredentialsNode.ToSharedRef()
                        : MakeShared<FGetExternalCredentialsFromOnlineSubsystemNode>(
                              this->SubsystemName,
                              this->CredentialType,
                              this->AuthenticatedWithValue,
                              this->TokenAuthAttributeName))
                ->Add(InitialState->CrossPlatformAccountProvider->GetInteractiveAuthenticationSequence())
                ->Add(MakeShared<FSelectCrossPlatformAccountNode>())
                ->Add(MakeShared<FLoginWithSelectedEOSAccountNode>())
                // Also link the credential directly onto the EOS account, so that if the developer ever turns off Epic
                // Games, players can still access their accounts.
                ->Add(MakeShared<FGatherEOSAccountsWithExternalCredentialsNode>())
                ->Add(InitialState->CrossPlatformAccountProvider
                          ->GetLinkUnusedExternalCredentialsToCrossPlatformAccountSequence());
        }
        else
        {
            auto SignIn =
                MakeShared<FAuthenticationGraphNodeUntil_Forever>()
                    ->Add(MakeShared<FLogUserIntoOnlineSubsystemNode>(this->SubsystemName))
                    ->Add(
                        this->OverrideGetCredentialsNode.IsValid()
                            ? this->OverrideGetCredentialsNode.ToSharedRef()
                            : MakeShared<FGetExternalCredentialsFromOnlineSubsystemNode>(
                                  this->SubsystemName,
                                  this->CredentialType,
                                  this->AuthenticatedWithValue,
                                  this->TokenAuthAttributeName))
                    // We *must* have valid external credentials at this point; the default implementation of
                    // FGetExternalCredentialsFromOnlineSubsystemNode enforces this, but double check in case the
                    // OverrideGetCredentialsNode implementation does not.
                    ->Add(MakeShared<FBailIfNotExactlyOneExternalCredentialNode>())
                    // Try to sign into a cross-platform account using those credentials.
                    ->Add(InitialState->CrossPlatformAccountProvider->GetNonInteractiveAuthenticationSequence(true))
                    ->Add(
                        MakeShared<FAuthenticationGraphNodeConditional>()
                            // If we have a cross-platform account at this point, the cross-platform provider will have
                            // also already authenticated it against EOS, so we can select the single remaining node and
                            // finish login.
                            //
                            // NOTE: We do not implicitly sign in with Epic Games accounts that *do not* have an EOS
                            // account here. That's because you can run into this situation:
                            // - You play the game on a console, without signing into an Epic Games account. You
                            //   therefore have an EOS account without cross-platform linkage.
                            // - You then play the game on Steam. You have your Steam account linked to an Epic Games
                            //   account, so we're able to sign into the Epic Games account implicitly. But you *don't*
                            //   have an EOS account associated with this Epic Games account. Implicitly creating an EOS
                            //   account for the Epic Games account here would give you two EOS accounts, which we don't
                            //   want.
                            // It's also possible to run into this situation if you:
                            // - Play the game on Steam, without connecting an Epic Games account.
                            // - At some point, you link your Epic Games account to your Steam account outside of this
                            // game.
                            // - Next time you play the game, you would implicitly get a new EOS account with the Epic
                            //   Games account instead of being able to continue to use your Steam-only EOS account.
                            ->If(
                                FAuthenticationGraphCondition::CreateStatic(
                                    &FAuthenticationGraph::Condition_CrossPlatformAccountIsValidWithPUID),
                                MakeShared<FAuthenticationGraphNodeUntil_Forever>()
                                    ->Add(MakeShared<FSelectCrossPlatformAccountNode>())
                                    ->Add(MakeShared<FLoginWithSelectedEOSAccountNode>()))
                            // We could not implicitly sign into a cross-platform account. Try implicitly signing them
                            // into a platform-specific account first, as they might have created a non-cross-platform
                            // account and have not yet upgraded to a cross-platform account.
                            ->Else(
                                MakeShared<FAuthenticationGraphNodeUntil_Forever>()
                                    ->Add(InitialState->CrossPlatformAccountProvider
                                              ->GetNonInteractiveDeauthenticationSequence())
                                    ->Add(MakeShared<FGatherEOSAccountsWithExternalCredentialsNode>())
                                    ->Add(
                                        MakeShared<FAuthenticationGraphNodeConditional>()
                                            // A non-cross-platform account exists for this credential. Sign the user
                                            // into that account.
                                            ->If(
                                                FAuthenticationGraphCondition::CreateStatic(
                                                    &FAuthenticationGraph::Condition_OneSuccessfulCandidate),
                                                MakeShared<FAuthenticationGraphNodeUntil_Forever>()
                                                    ->Add(MakeShared<FSelectSingleSuccessfulEOSAccountNode>())
                                                    ->Add(MakeShared<FLoginWithSelectedEOSAccountNode>()))
                                            // A non-cross-platform account doesn't exist either. Ask the user
                                            // whether they want to sign into a cross-platform account interactively,
                                            // or create a new non-cross-platform account.
                                            ->Else(
                                                MakeShared<FAuthenticationGraphNodeUntil_Forever>()
                                                    ->Add(MakeShared<FPromptToSignInOrCreateAccountNode>())
                                                    ->Add(
                                                        MakeShared<FAuthenticationGraphNodeConditional>()
                                                            // The user wants to sign into an existing account, which
                                                            // means doing an interactive login with the cross-platform
                                                            // provider.
                                                            ->If(
                                                                FAuthenticationGraphCondition::CreateLambda(
                                                                    [](const FAuthenticationGraphState &State) {
                                                                        return State.LastSignInChoice ==
                                                                               EEOSUserInterface_SignInOrCreateAccount_Choice::
                                                                                   SignIn;
                                                                    }),
                                                                MakeShared<FAuthenticationGraphNodeUntil_Forever>()
                                                                    ->Add(
                                                                        InitialState->CrossPlatformAccountProvider
                                                                            ->GetInteractiveOnlyAuthenticationSequence())
                                                                    ->Add(MakeShared<FSelectCrossPlatformAccountNode>(
                                                                        ESelectCrossPlatformAccountMode::
                                                                            ExistingAccountOnly))
                                                                    ->Add(
                                                                        MakeShared<FLoginWithSelectedEOSAccountNode>())
                                                                    ->Add(
                                                                        InitialState->CrossPlatformAccountProvider
                                                                            ->GetLinkUnusedExternalCredentialsToCrossPlatformAccountSequence()))
                                                            // The user wants to create a new non-cross platform
                                                            // account. Select the EOS candidate from earlier that had a
                                                            // continuance token.
                                                            ->Else(
                                                                MakeShared<FAuthenticationGraphNodeUntil_Forever>()
                                                                    ->Add(MakeShared<
                                                                          FSelectSingleContinuanceTokenEOSCandidateNode>())
                                                                    ->Add(MakeShared<
                                                                          FLoginWithSelectedEOSAccountNode>())))))));

            return MakeShared<FAuthenticationGraphNodeConditional>()
                ->If(
                    FAuthenticationGraphCondition::CreateStatic(&FAuthenticationGraph::Condition_Unauthenticated),
                    SignIn)
                ->If(
                    FAuthenticationGraphCondition::CreateStatic(&Condition_CanUpgradeToCrossPlatformAccount),
                    InitialState->CrossPlatformAccountProvider
                        ->GetUpgradeCurrentAccountToCrossPlatformAccountSequence())
                ->Else(MakeShared<FBailIfAlreadyAuthenticatedNode>());
        }
    }
    else
    {
        return MakeShared<FAuthenticationGraphNodeUntil_Forever>()
            ->Add(MakeShared<FBailIfAlreadyAuthenticatedNode>())
            ->Add(MakeShared<FLogUserIntoOnlineSubsystemNode>(this->SubsystemName))
            ->Add(
                this->OverrideGetCredentialsNode.IsValid() ? this->OverrideGetCredentialsNode.ToSharedRef()
                                                           : MakeShared<FGetExternalCredentialsFromOnlineSubsystemNode>(
                                                                 this->SubsystemName,
                                                                 this->CredentialType,
                                                                 this->AuthenticatedWithValue,
                                                                 this->TokenAuthAttributeName))
            // We *must* have valid external credentials at this point; the default implementation of
            // FGetExternalCredentialsFromOnlineSubsystemNode enforces this, but double check in case the
            // OverrideGetCredentialsNode implementation does not.
            ->Add(MakeShared<FBailIfNotExactlyOneExternalCredentialNode>())
            ->Add(MakeShared<FGatherEOSAccountsWithExternalCredentialsNode>())
            ->Add(MakeShared<FSelectOnlyEOSCandidateNode>())
            ->Add(MakeShared<FLoginWithSelectedEOSAccountNode>());
    }
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION