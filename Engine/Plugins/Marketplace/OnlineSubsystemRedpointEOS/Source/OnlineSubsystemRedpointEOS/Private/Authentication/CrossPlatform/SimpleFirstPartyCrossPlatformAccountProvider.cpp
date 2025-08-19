// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/SimpleFirstPartyCrossPlatformAccountProvider.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/SimpleFirstParty/GetJwtForSimpleFirstPartyNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/SimpleFirstParty/PerformOpenIdLoginForCrossPlatformFPNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/AuthenticationGraphNodeUntil_Forever.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/FailAuthenticationNode.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Nodes/NoopAuthenticationGraphNode.h"

FSimpleFirstPartyCrossPlatformAccountId::FSimpleFirstPartyCrossPlatformAccountId(int64 InFirstPartyAccountId)
    : DataBytes(nullptr)
    , DataBytesSize(0)
    , FirstPartyAccountId(InFirstPartyAccountId)
{
    auto Str = StringCast<ANSICHAR>(*this->ToString());
    this->DataBytesSize = Str.Length() + 1;
    this->DataBytes = (uint8 *)FMemory::MallocZeroed(this->DataBytesSize);
    FMemory::Memcpy(this->DataBytes, Str.Get(), Str.Length());
}

bool FSimpleFirstPartyCrossPlatformAccountId::Compare(const FCrossPlatformAccountId &Other) const
{
    if (Other.GetType() != GetType())
    {
        return false;
    }

    if (Other.GetType() == SIMPLE_FIRST_PARTY_ACCOUNT_ID)
    {
        const FSimpleFirstPartyCrossPlatformAccountId &OtherId = (const FSimpleFirstPartyCrossPlatformAccountId &)Other;
        return OtherId.GetFirstPartyAccountId() == this->GetFirstPartyAccountId();
    }

    return (GetType() == Other.GetType() && GetSize() == Other.GetSize()) &&
           (FMemory::Memcmp(GetBytes(), Other.GetBytes(), GetSize()) == 0);
}

FSimpleFirstPartyCrossPlatformAccountId::~FSimpleFirstPartyCrossPlatformAccountId()
{
    FMemory::Free(this->DataBytes);
}

FName FSimpleFirstPartyCrossPlatformAccountId::GetType() const
{
    return SIMPLE_FIRST_PARTY_ACCOUNT_ID;
}

const uint8 *FSimpleFirstPartyCrossPlatformAccountId::GetBytes() const
{
    return this->DataBytes;
}

int32 FSimpleFirstPartyCrossPlatformAccountId::GetSize() const
{
    return this->DataBytesSize;
}

bool FSimpleFirstPartyCrossPlatformAccountId::IsValid() const
{
    return this->FirstPartyAccountId != 0;
}

FString FSimpleFirstPartyCrossPlatformAccountId::ToString() const
{
    return FString::Printf(TEXT("%lld"), this->FirstPartyAccountId);
}

int64 FSimpleFirstPartyCrossPlatformAccountId::GetFirstPartyAccountId() const
{
    return this->FirstPartyAccountId;
}

TSharedPtr<const FCrossPlatformAccountId> FSimpleFirstPartyCrossPlatformAccountId::ParseFromString(const FString &In)
{
    return MakeShared<FSimpleFirstPartyCrossPlatformAccountId>(FCString::Atoi64(*In));
}

FName FSimpleFirstPartyCrossPlatformAccountProvider::GetName()
{
    return SIMPLE_FIRST_PARTY_ACCOUNT_ID;
}

TSharedPtr<const FCrossPlatformAccountId> FSimpleFirstPartyCrossPlatformAccountProvider::CreateCrossPlatformAccountId(
    const FString &InStringRepresentation)
{
    return FSimpleFirstPartyCrossPlatformAccountId::ParseFromString(InStringRepresentation);
}

TSharedPtr<const FCrossPlatformAccountId> FSimpleFirstPartyCrossPlatformAccountProvider::CreateCrossPlatformAccountId(
    uint8 *InBytes,
    int32 InSize)
{
    FString Data = BytesToString(InBytes, InSize);
    return FSimpleFirstPartyCrossPlatformAccountId::ParseFromString(Data);
}

TSharedRef<FAuthenticationGraphNode> FSimpleFirstPartyCrossPlatformAccountProvider::
    GetInteractiveAuthenticationSequence()
{
    return MakeShared<FAuthenticationGraphNodeUntil_Forever>()
        ->Add(MakeShared<FGetJwtForSimpleFirstPartyNode>())
        ->Add(MakeShared<FPerformOpenIdLoginForCrossPlatformFPNode>());
}

TSharedRef<FAuthenticationGraphNode> FSimpleFirstPartyCrossPlatformAccountProvider::
    GetInteractiveOnlyAuthenticationSequence()
{
    return MakeShared<FAuthenticationGraphNodeUntil_Forever>()
        ->Add(MakeShared<FGetJwtForSimpleFirstPartyNode>())
        ->Add(MakeShared<FPerformOpenIdLoginForCrossPlatformFPNode>());
}

TSharedRef<FAuthenticationGraphNode> FSimpleFirstPartyCrossPlatformAccountProvider::
    GetNonInteractiveAuthenticationSequence(bool bOnlyUseExternalCredentials)
{
    return MakeShared<FNoopAuthenticationGraphNode>();
}

TSharedRef<FAuthenticationGraphNode> FSimpleFirstPartyCrossPlatformAccountProvider::
    GetUpgradeCurrentAccountToCrossPlatformAccountSequence()
{
    return MakeShared<FNoopAuthenticationGraphNode>();
}

TSharedRef<FAuthenticationGraphNode> FSimpleFirstPartyCrossPlatformAccountProvider::
    GetLinkUnusedExternalCredentialsToCrossPlatformAccountSequence()
{
    return MakeShared<FNoopAuthenticationGraphNode>();
}

TSharedRef<FAuthenticationGraphNode> FSimpleFirstPartyCrossPlatformAccountProvider::
    GetNonInteractiveDeauthenticationSequence()
{
    return MakeShared<FNoopAuthenticationGraphNode>();
}

TSharedRef<FAuthenticationGraphNode> FSimpleFirstPartyCrossPlatformAccountProvider::
    GetAutomatedTestingAuthenticationSequence()
{
#if !defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING
    return MakeShared<FNoopAuthenticationGraphNode>();
#else
    return MakeShared<FFailAuthenticationNode>(
        TEXT("Automated testing authentication is not supported in shipping builds."));
#endif
}

#endif // #if EOS_HAS_AUTHENTICATION
