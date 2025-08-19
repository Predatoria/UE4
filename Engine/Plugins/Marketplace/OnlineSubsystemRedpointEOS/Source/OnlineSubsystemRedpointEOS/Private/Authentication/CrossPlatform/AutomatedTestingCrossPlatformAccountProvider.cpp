// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#if !defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/AutomatedTestingCrossPlatformAccountProvider.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/AutomatedTesting/EmitLogForAutomatedTestingNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/AutomatedTesting/IssueJwtForCrossPlatformAutomatedTestingNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/AutomatedTesting/PerformOpenIdLoginForCrossPlatformATNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/AuthenticationGraphNodeUntil_Forever.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/FailAuthenticationNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/IssueJwtForAutomatedTestingNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/LoginWithSelectedEOSAccountNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/NoopAuthenticationGraphNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/PerformOpenIdLoginForAutomatedTestingNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/SelectCrossPlatformAccountNode.h"

EOS_ENABLE_STRICT_WARNINGS

FAutomatedTestingCrossPlatformAccountId::FAutomatedTestingCrossPlatformAccountId(
    const FString &InAutomatedTestingAccountId)
    : DataBytes(nullptr)
    , DataBytesSize(0)
    , AutomatedTestingAccountId(InAutomatedTestingAccountId)
{
    auto Str = StringCast<ANSICHAR>(*this->ToString());
    this->DataBytesSize = Str.Length() + 1;
    this->DataBytes = (uint8 *)FMemory::MallocZeroed(this->DataBytesSize);
    FMemory::Memcpy(this->DataBytes, Str.Get(), Str.Length());
}

bool FAutomatedTestingCrossPlatformAccountId::Compare(const FCrossPlatformAccountId &Other) const
{
    if (Other.GetType() != GetType())
    {
        return false;
    }

    if (Other.GetType() == AUTOMATED_TESTING_ACCOUNT_ID)
    {
        const FAutomatedTestingCrossPlatformAccountId &OtherEOS =
            (const FAutomatedTestingCrossPlatformAccountId &)Other;
        return OtherEOS.AutomatedTestingAccountId == this->AutomatedTestingAccountId;
    }

    return (GetType() == Other.GetType() && GetSize() == Other.GetSize()) &&
           (FMemory::Memcmp(GetBytes(), Other.GetBytes(), GetSize()) == 0);
}

FAutomatedTestingCrossPlatformAccountId::~FAutomatedTestingCrossPlatformAccountId()
{
    FMemory::Free(this->DataBytes);
}

FName FAutomatedTestingCrossPlatformAccountId::GetType() const
{
    return AUTOMATED_TESTING_ACCOUNT_ID;
}

const uint8 *FAutomatedTestingCrossPlatformAccountId::GetBytes() const
{
    return this->DataBytes;
}

int32 FAutomatedTestingCrossPlatformAccountId::GetSize() const
{
    return this->DataBytesSize;
}

bool FAutomatedTestingCrossPlatformAccountId::IsValid() const
{
    return true;
}

FString FAutomatedTestingCrossPlatformAccountId::ToString() const
{
    return this->AutomatedTestingAccountId;
}

TSharedPtr<const FCrossPlatformAccountId> FAutomatedTestingCrossPlatformAccountId::ParseFromString(const FString &In)
{
    return MakeShared<FAutomatedTestingCrossPlatformAccountId>(In);
}

FName FAutomatedTestingCrossPlatformAccountProvider::GetName()
{
    return AUTOMATED_TESTING_ACCOUNT_ID;
}

TSharedPtr<const FCrossPlatformAccountId> FAutomatedTestingCrossPlatformAccountProvider::CreateCrossPlatformAccountId(
    const FString &InStringRepresentation)
{
    return FAutomatedTestingCrossPlatformAccountId::ParseFromString(InStringRepresentation);
}

TSharedPtr<const FCrossPlatformAccountId> FAutomatedTestingCrossPlatformAccountProvider::CreateCrossPlatformAccountId(
    uint8 *InBytes,
    int32 InSize)
{
    FString Data = BytesToString(InBytes, InSize);
    return FAutomatedTestingCrossPlatformAccountId::ParseFromString(Data);
}

TSharedRef<FAuthenticationGraphNode> FAutomatedTestingCrossPlatformAccountProvider::
    GetInteractiveAuthenticationSequence()
{
    // This will fire if the game requires a cross-platform account.
    // It needs to emit a log entry for the unit test to detect, and then do an automated sign-in.
    return MakeShared<FAuthenticationGraphNodeUntil_Forever>()
        ->Add(MakeShared<FIssueJwtForCrossPlatformAutomatedTestingNode>())
        ->Add(MakeShared<FEmitLogForAutomatedTestingNode>(
            TEXT("[CPAT-01] Emulating required sign-in for cross-platform automation testing")))
        ->Add(MakeShared<FPerformOpenIdLoginForCrossPlatformAutomatedTestingNode>());
}

TSharedRef<FAuthenticationGraphNode> FAutomatedTestingCrossPlatformAccountProvider::
    GetInteractiveOnlyAuthenticationSequence()
{
    // This will fire after the "user" picks "sign in" instead of "create an account" at the prompt.
    // It needs to emit a log entry for the unit test to detect, and then do an automated sign-in.
    return MakeShared<FAuthenticationGraphNodeUntil_Forever>()
        ->Add(MakeShared<FIssueJwtForCrossPlatformAutomatedTestingNode>())
        ->Add(MakeShared<FEmitLogForAutomatedTestingNode>(
            TEXT("[CPAT-02] Emulating interactive sign in because the automated testing emulated a click on 'sign in' "
                 "instead of 'create an account'")))
        ->Add(MakeShared<FPerformOpenIdLoginForCrossPlatformAutomatedTestingNode>());
}

TSharedRef<FAuthenticationGraphNode> FAutomatedTestingCrossPlatformAccountProvider::
    GetNonInteractiveAuthenticationSequence(bool bOnlyUseExternalCredentials)
{
    // This fires during an optional sign in process, and would be used to try persistent logins, dev tools and
    // already gathered external credentials. Because JWTs are always new accounts, this will always result in an EOS
    // candidate that has a continuance token (not a PUID) and thus cause the "sign-in / create account" prompt to show.
    // We do a normal (not cross-platform) JWT issuance here so that when FSelectSingleContinuanceTokenEOSCandidateNode
    // runs later in the graph if "create account" is selected, it actually has a valid continuance token with which to
    // create an EOS account.
    return MakeShared<FAuthenticationGraphNodeUntil_Forever>()
        ->Add(MakeShared<FEmitLogForAutomatedTestingNode>(
            TEXT("[CPAT-03] Emulating non-interactive sign-in for cross-platform automation testing")))
        ->Add(MakeShared<FIssueJwtForAutomatedTestingNode>())
        ->Add(MakeShared<FPerformOpenIdLoginForAutomatedTestingNode>());
}

TSharedRef<FAuthenticationGraphNode> FAutomatedTestingCrossPlatformAccountProvider::
    GetUpgradeCurrentAccountToCrossPlatformAccountSequence()
{
    // This will fire when the user is already signed into a platform-specific account and they want to upgrade to a
    // cross-platform account. Since this graph must complete with an authenticated cross-platform account, we just do a
    // normal login here and emit a different log code for the unit tests.
    return MakeShared<FAuthenticationGraphNodeUntil_Forever>()
        ->Add(MakeShared<FIssueJwtForCrossPlatformAutomatedTestingNode>())
        ->Add(MakeShared<FEmitLogForAutomatedTestingNode>(
            TEXT("[CPAT-05] Emulating interactive login as part of an upgrade process")))
        ->Add(MakeShared<FPerformOpenIdLoginForCrossPlatformAutomatedTestingNode>())
        ->Add(MakeShared<FSelectCrossPlatformAccountNode>())
        ->Add(MakeShared<FLoginWithSelectedEOSAccountNode>());
}

TSharedRef<FAuthenticationGraphNode> FAutomatedTestingCrossPlatformAccountProvider::
    GetLinkUnusedExternalCredentialsToCrossPlatformAccountSequence()
{
    // This sequence of nodes must always succeed, but is otherwise unused.
    return MakeShared<FEmitLogForAutomatedTestingNode>(
        TEXT("[CPAT-04] Requested linkage of platform credentials into cross-platform account"));
}

TSharedRef<FAuthenticationGraphNode> FAutomatedTestingCrossPlatformAccountProvider::
    GetNonInteractiveDeauthenticationSequence()
{
    return MakeShared<FNoopAuthenticationGraphNode>();
}

TSharedRef<FAuthenticationGraphNode> FAutomatedTestingCrossPlatformAccountProvider::
    GetAutomatedTestingAuthenticationSequence()
{
    return MakeShared<FFailAuthenticationNode>(
        TEXT("The automated testing cross-platform provider is not meant to be used with the AutomatedTesting graph, "
             "as this combination is redundant. The automated testing cross-platform provider is intended to be used "
             "with other graphs to test their interactive sign in and create account flows."));
}

EOS_DISABLE_STRICT_WARNINGS

#endif

#endif // #if EOS_HAS_AUTHENTICATION