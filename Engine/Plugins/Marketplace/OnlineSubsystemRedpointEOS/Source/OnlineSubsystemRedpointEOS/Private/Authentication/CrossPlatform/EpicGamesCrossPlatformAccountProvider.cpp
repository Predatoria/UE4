// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGamesCrossPlatformAccountProvider.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/ChainEASResultToEOSNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/ClearEOSCandidatesNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/FailAuthenticationDueToConflictingAccountsNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/GatherEASAccountsWithExternalCredentialsNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/GetEASCTOAForExistingExternalCredentialsNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/LinkEASContinuanceTokenToExistingAccountNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/LinkUnconnectedEOSAccountToSignedInEASAccountNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/PerformAutomatedTestingEASLoginNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/PerformInteractiveEASLoginNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/PerformInteractiveLinkExternalCredentialsToEASAccountNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/SignOutEASCandidateNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/TryDeveloperAuthenticationEASCredentialsNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/TryExchangeCodeAuthenticationNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGames/TryPersistentEASCredentialsNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/AuthenticationGraphNodeConditional.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/AuthenticationGraphNodeUntil_CrossPlatformAccountPresent.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/AuthenticationGraphNodeUntil_Forever.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/FailAuthenticationNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/LoginWithSelectedEOSAccountNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/NoopAuthenticationGraphNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/SelectCrossPlatformAccountNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOS.h"

FEpicGamesCrossPlatformAccountId::FEpicGamesCrossPlatformAccountId(EOS_EpicAccountId InEpicAccountId)
    : DataBytes(nullptr)
    , DataBytesSize(0)
    , EpicAccountId(InEpicAccountId)
{
    check(!EOSString_EpicAccountId::IsNone(this->EpicAccountId));

    auto Str = StringCast<ANSICHAR>(*this->ToString());
    this->DataBytesSize = Str.Length() + 1;
    this->DataBytes = (uint8 *)FMemory::MallocZeroed(this->DataBytesSize);
    FMemory::Memcpy(this->DataBytes, Str.Get(), Str.Length());
}

bool FEpicGamesCrossPlatformAccountId::Compare(const FCrossPlatformAccountId &Other) const
{
    if (Other.GetType() != GetType())
    {
        return false;
    }

    if (Other.GetType() == EPIC_GAMES_ACCOUNT_ID)
    {
        const FEpicGamesCrossPlatformAccountId &OtherEOS = (const FEpicGamesCrossPlatformAccountId &)Other;
        if (EOSString_EpicAccountId::IsValid(OtherEOS.GetEpicAccountId()) &&
            EOSString_EpicAccountId::IsValid(this->GetEpicAccountId()))
        {
            return OtherEOS.GetEpicAccountIdString() == this->GetEpicAccountIdString();
        }
    }

    return (GetType() == Other.GetType() && GetSize() == Other.GetSize()) &&
           (FMemory::Memcmp(GetBytes(), Other.GetBytes(), GetSize()) == 0);
}

FEpicGamesCrossPlatformAccountId::~FEpicGamesCrossPlatformAccountId()
{
    FMemory::Free(this->DataBytes);
}

FName FEpicGamesCrossPlatformAccountId::GetType() const
{
    return EPIC_GAMES_ACCOUNT_ID;
}

const uint8 *FEpicGamesCrossPlatformAccountId::GetBytes() const
{
    return this->DataBytes;
}

int32 FEpicGamesCrossPlatformAccountId::GetSize() const
{
    return this->DataBytesSize;
}

bool FEpicGamesCrossPlatformAccountId::IsValid() const
{
    return EOSString_EpicAccountId::IsValid(this->EpicAccountId);
}

FString FEpicGamesCrossPlatformAccountId::ToString() const
{
    return this->GetEpicAccountIdString();
}

TSharedPtr<const FCrossPlatformAccountId> FEpicGamesCrossPlatformAccountId::ParseFromString(const FString &In)
{
    EOS_EpicAccountId EpicAccountId = nullptr;
    if (EOSString_EpicAccountId::FromString(In, EpicAccountId) != EOS_EResult::EOS_Success)
    {
        UE_LOG(LogEOS, Error, TEXT("Malformed Epic account ID component of unique net ID: %s"), *In);
        return nullptr;
    }
    return MakeShared<FEpicGamesCrossPlatformAccountId>(EpicAccountId);
}

bool FEpicGamesCrossPlatformAccountId::HasValidEpicAccountId() const
{
    return EOSString_EpicAccountId::IsValid(this->EpicAccountId);
}

EOS_EpicAccountId FEpicGamesCrossPlatformAccountId::GetEpicAccountId() const
{
    return this->EpicAccountId;
}

FString FEpicGamesCrossPlatformAccountId::GetEpicAccountIdString() const
{
    if (EOSString_EpicAccountId::IsNone(this->EpicAccountId))
    {
        return TEXT("");
    }

    FString Str;
    if (EOSString_EpicAccountId::ToString(this->EpicAccountId, Str) == EOS_EResult::EOS_Success)
    {
        return Str;
    }

    return TEXT("");
}

FName FEpicGamesCrossPlatformAccountProvider::GetName()
{
    return EPIC_GAMES_ACCOUNT_ID;
}

TSharedPtr<const FCrossPlatformAccountId> FEpicGamesCrossPlatformAccountProvider::CreateCrossPlatformAccountId(
    const FString &InStringRepresentation)
{
    return FEpicGamesCrossPlatformAccountId::ParseFromString(InStringRepresentation);
}

TSharedPtr<const FCrossPlatformAccountId> FEpicGamesCrossPlatformAccountProvider::CreateCrossPlatformAccountId(
    uint8 *InBytes,
    int32 InSize)
{
    FString Data = BytesToString(InBytes, InSize);
    return FEpicGamesCrossPlatformAccountId::ParseFromString(Data);
}

bool HasContinuanceToken(const FAuthenticationGraphState &State)
{
    return State.EASExternalContinuanceToken != nullptr;
}

bool HasEpicGamesAccount(const FAuthenticationGraphState &State)
{
    return State.AuthenticatedCrossPlatformAccountId.IsValid();
}

bool CrossPlatformEOSCandidateMatchesExistingAccount(const FAuthenticationGraphState &State)
{
    for (const auto &Candidate : State.EOSCandidates)
    {
        if (Candidate.Type == EAuthenticationGraphEOSCandidateType::CrossPlatform)
        {
            if (EOSString_ProductUserId::IsValid(Candidate.ProductUserId) &&
                *ConstCastSharedRef<const FUniqueNetIdEOS>(MakeShared<FUniqueNetIdEOS>(Candidate.ProductUserId)) ==
                    *State.ExistingUserId)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    return false;
}

bool CrossPlatformEOSCandidateIsContinuanceToken(const FAuthenticationGraphState &State)
{
    for (const auto &Candidate : State.EOSCandidates)
    {
        if (Candidate.Type == EAuthenticationGraphEOSCandidateType::CrossPlatform)
        {
            if (!EOSString_ContinuanceToken::IsNone(Candidate.ContinuanceToken))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    return false;
}

TSharedRef<FAuthenticationGraphNode> FEpicGamesCrossPlatformAccountProvider::GetInteractiveAuthenticationSequence()
{
    return MakeShared<FAuthenticationGraphNodeUntil_Forever>()
        ->Add(MakeShared<FAuthenticationGraphNodeUntil_CrossPlatformAccountPresent>(TEXT("Unable to sign in to Epic Games interactively. Check the logs for more information."))
                  ->Add(MakeShared<FTryExchangeCodeAuthenticationNode>())
                  ->Add(MakeShared<FTryPIEDeveloperAuthenticationEASCredentialsNode>())
                  ->Add(MakeShared<FTryDefaultDeveloperAuthenticationEASCredentialsNode>())
                  ->Add(MakeShared<FTryPersistentEASCredentialsNode>())
                  ->Add(MakeShared<FGatherEASAccountsWithExternalCredentialsNode>())
                  ->Add(MakeShared<FAuthenticationGraphNodeConditional>()
                            ->If(
                                FAuthenticationGraphCondition::CreateStatic(&HasContinuanceToken),
                                MakeShared<FPerformInteractiveLinkExternalCredentialsToEASAccountNode>())
                            ->Else(MakeShared<FPerformInteractiveEASLoginNode>())))
        ->Add(MakeShared<FChainEASResultToEOSNode>());
}

TSharedRef<FAuthenticationGraphNode> FEpicGamesCrossPlatformAccountProvider::GetInteractiveOnlyAuthenticationSequence()
{
    return MakeShared<FAuthenticationGraphNodeUntil_Forever>()
        ->Add(MakeShared<FAuthenticationGraphNodeUntil_CrossPlatformAccountPresent>()->Add(
            MakeShared<FPerformInteractiveEASLoginNode>()))
        ->Add(MakeShared<FChainEASResultToEOSNode>());
}

TSharedRef<FAuthenticationGraphNode> FEpicGamesCrossPlatformAccountProvider::GetNonInteractiveAuthenticationSequence(
    bool bOnlyUseExternalCredentials)
{
    auto Sequence = MakeShared<FAuthenticationGraphNodeUntil_CrossPlatformAccountPresent>()->AllowFailure(true);
    if (!bOnlyUseExternalCredentials)
    {
        Sequence->Add(MakeShared<FTryExchangeCodeAuthenticationNode>())
            ->Add(MakeShared<FTryPIEDeveloperAuthenticationEASCredentialsNode>())
            ->Add(MakeShared<FTryDefaultDeveloperAuthenticationEASCredentialsNode>())
            ->Add(MakeShared<FTryPersistentEASCredentialsNode>());
    }
    Sequence->Add(MakeShared<FGatherEASAccountsWithExternalCredentialsNode>());
    return MakeShared<FAuthenticationGraphNodeUntil_Forever>()->Add(Sequence)->Add(
        MakeShared<FAuthenticationGraphNodeConditional>()
            ->If(
                FAuthenticationGraphCondition::CreateStatic(
                    &FAuthenticationGraph::Condition_CrossPlatformAccountIsValid),
                MakeShared<FChainEASResultToEOSNode>())
            ->Else(MakeShared<FNoopAuthenticationGraphNode>()));
}

TSharedRef<FAuthenticationGraphNode> FEpicGamesCrossPlatformAccountProvider::
    GetUpgradeCurrentAccountToCrossPlatformAccountSequence()
{
    return MakeShared<FAuthenticationGraphNodeUntil_Forever>()
        ->Add(MakeShared<FGetEASContinuanceTokenOrAccountForExistingExternalCredentialsNode>())
        //
        // We will now either have:
        // * State->EASExternalContinuanceToken if the external credentials are not associated with an Epic account
        //   (which is what we expect in 99% of cases here; the exception is if the user has linked the platform
        //    with an Epic Games account *while* the game has been running)
        // * State->AuthenticatedCrossPlatformAccountId if the external credentials are associated with an Epic
        //   account.
        //
        // Regardless of which one we have, we'll eventually have to check if we can use
        // State->AuthenticatedCrossPlatformAccountId. It can only be used if it ends up resolving to either
        // no EOS account, or the same EOS account as the current user is signed in. Otherwise, the Epic account
        // points to a different user profile.
        //
        // Before we do any of that, we have to convert State->EASExternalContinuanceToken to
        // State->AuthenticatedCrossPlatformAccountId by going through EOS_Auth_LinkAccount.
        //
        ->Add(MakeShared<FAuthenticationGraphNodeConditional>()
                  ->If(
                      FAuthenticationGraphCondition::CreateStatic(&HasContinuanceToken),
                      MakeShared<FPerformInteractiveLinkExternalCredentialsToEASAccountNode>())
                  ->Else(MakeShared<FNoopAuthenticationGraphNode>()))
        // We should now have State->AuthenticatedCrossPlatformAccountId.
        ->Add(MakeShared<FAuthenticationGraphNodeConditional>()
                  ->If(
                      FAuthenticationGraphCondition::CreateStatic(&HasEpicGamesAccount),
                      MakeShared<FChainEASResultToEOSNode>())
                  ->Else(MakeShared<FFailAuthenticationNode>(
                      "Unable to complete linking process as no Epic Games account was available.")))
        // We'll now have an EOS candidate with a CrossPlatform type.
        ->Add(MakeShared<FAuthenticationGraphNodeConditional>()
                  ->If(
                      FAuthenticationGraphCondition::CreateStatic(&CrossPlatformEOSCandidateMatchesExistingAccount),
                      MakeShared<FAuthenticationGraphNodeUntil_Forever>()
                          ->Add(MakeShared<FSelectCrossPlatformAccountNode>())
                          ->Add(MakeShared<FLoginWithSelectedEOSAccountNode>()))
                  ->If(
                      FAuthenticationGraphCondition::CreateStatic(&CrossPlatformEOSCandidateIsContinuanceToken),
                      MakeShared<FAuthenticationGraphNodeUntil_Forever>()
                          ->Add(MakeShared<FLinkEASContinuanceTokenToExistingAccountNode>())
                          // Then re-authenticate with the external credentials so we get a full Epic login done.
                          ->Add(MakeShared<FClearEOSCandidatesNode>())
                          ->Add(MakeShared<FGetEASContinuanceTokenOrAccountForExistingExternalCredentialsNode>())
                          ->Add(MakeShared<FChainEASResultToEOSNode>())
                          ->Add(MakeShared<FSelectCrossPlatformAccountNode>())
                          ->Add(MakeShared<FLoginWithSelectedEOSAccountNode>()))
                  ->Else(MakeShared<FFailAuthenticationDueToConflictingAccountsNode>()));
}

TSharedRef<FAuthenticationGraphNode> FEpicGamesCrossPlatformAccountProvider::
    GetLinkUnusedExternalCredentialsToCrossPlatformAccountSequence()
{
    return MakeShared<FLinkUnconnectedEOSAccountToSignedInEASAccountNode>();
}

TSharedRef<FAuthenticationGraphNode> FEpicGamesCrossPlatformAccountProvider::GetNonInteractiveDeauthenticationSequence()
{
    return MakeShared<FSignOutEASCandidateNode>();
}

TSharedRef<FAuthenticationGraphNode> FEpicGamesCrossPlatformAccountProvider::GetAutomatedTestingAuthenticationSequence()
{
#if !defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING
    return MakeShared<FAuthenticationGraphNodeUntil_Forever>()
        ->Add(MakeShared<FAuthenticationGraphNodeUntil_CrossPlatformAccountPresent>()->Add(
            MakeShared<FPerformAutomatedTestingEASLoginNode>()))
        ->Add(MakeShared<FChainEASResultToEOSNode>());
#else
    return MakeShared<FFailAuthenticationNode>(
        TEXT("Automated testing authentication is not supported in shipping builds."));
#endif
}

FString CheckForDesktopCrossplayError(EOS_HPlatform InPlatform)
{
#if EOS_VERSION_AT_LEAST(1, 15, 0)
#if PLATFORM_WINDOWS
    EOS_Platform_GetDesktopCrossplayStatusOptions DesktopCrossplayOpts = {};
    DesktopCrossplayOpts.ApiVersion = EOS_PLATFORM_GETDESKTOPCROSSPLAYSTATUS_API_LATEST;

    EOS_Platform_GetDesktopCrossplayStatusInfo DesktopCrossplayInfo = {};
    EOS_EResult DesktopCrossplayResult =
        EOS_Platform_GetDesktopCrossplayStatus(InPlatform, &DesktopCrossplayOpts, &DesktopCrossplayInfo);

    FString DesktopCrossplayError = TEXT("");
    if (DesktopCrossplayResult != EOS_EResult::EOS_Success)
    {
        DesktopCrossplayError = TEXT("Unable to perform Epic Games authentication on this platform, as the desktop "
                                     "cross-play service is not available.");
    }
    else if (DesktopCrossplayInfo.Status != EOS_EDesktopCrossplayStatus::EOS_DCS_OK)
    {
        DesktopCrossplayError = FString::Printf(
            TEXT("Unable to perform Epic Games authentication on this platform, as the desktop "
                 "cross-play service is not available. The status of the EOS cross-play service is: %s"),
            *DesktopCrossplayStatusToString(DesktopCrossplayInfo.Status));
    }
    return DesktopCrossplayError;
#else
    // This platform does not require the cross-play service.
    return TEXT("");
#endif
#else
    // This SDK version does not have a cross-play service.
    return TEXT("");
#endif
}

#if EOS_VERSION_AT_LEAST(1, 15, 0)
FString DesktopCrossplayStatusToString(EOS_EDesktopCrossplayStatus InStatus)
{
    switch (InStatus)
    {
    case EOS_EDesktopCrossplayStatus::EOS_DCS_OK:
        return TEXT("OK");
    case EOS_EDesktopCrossplayStatus::EOS_DCS_ApplicationNotBootstrapped:
        return TEXT("EOS_DCS_ApplicationNotBootstrapped");
    case EOS_EDesktopCrossplayStatus::EOS_DCS_ServiceNotInstalled:
        return TEXT("EOS_DCS_ServiceNotInstalled");
    case EOS_EDesktopCrossplayStatus::EOS_DCS_ServiceStartFailed:
        return TEXT("EOS_DCS_ServiceStartFailed");
    case EOS_EDesktopCrossplayStatus::EOS_DCS_ServiceNotRunning:
        return TEXT("EOS_DCS_ServiceNotRunning");
    case EOS_EDesktopCrossplayStatus::EOS_DCS_OverlayDisabled:
        return TEXT("EOS_DCS_OverlayDisabled");
    case EOS_EDesktopCrossplayStatus::EOS_DCS_OverlayNotInstalled:
        return TEXT("EOS_DCS_OverlayNotInstalled");
    case EOS_EDesktopCrossplayStatus::EOS_DCS_OverlayTrustCheckFailed:
        return TEXT("EOS_DCS_OverlayTrustCheckFailed");
    case EOS_EDesktopCrossplayStatus::EOS_DCS_OverlayLoadFailed:
        return TEXT("EOS_DCS_OverlayLoadFailed");
    default:
        return TEXT("Unknown");
    }
}
#endif

#endif // #if EOS_HAS_AUTHENTICATION