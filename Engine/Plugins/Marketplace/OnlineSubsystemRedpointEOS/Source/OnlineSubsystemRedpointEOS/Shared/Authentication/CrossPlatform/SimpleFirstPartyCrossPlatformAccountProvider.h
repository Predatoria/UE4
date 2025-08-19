// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/CrossPlatformAccountId.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/CrossPlatformAccountProvider.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FSimpleFirstPartyCrossPlatformAccountId : public FCrossPlatformAccountId
{
private:
    uint8 *DataBytes;
    int32 DataBytesSize;
    int64 FirstPartyAccountId;

public:
    FSimpleFirstPartyCrossPlatformAccountId(int64 InFirstPartyAccountId);
    virtual ~FSimpleFirstPartyCrossPlatformAccountId();
    UE_NONCOPYABLE(FSimpleFirstPartyCrossPlatformAccountId);

    virtual bool Compare(const FCrossPlatformAccountId &Other) const override;
    virtual FName GetType() const override;
    virtual const uint8 *GetBytes() const override;
    virtual int32 GetSize() const override;
    virtual bool IsValid() const override;
    virtual FString ToString() const override;

    static TSharedPtr<const FCrossPlatformAccountId> ParseFromString(const FString &In);

    int64 GetFirstPartyAccountId() const;
};

class ONLINESUBSYSTEMREDPOINTEOS_API FSimpleFirstPartyCrossPlatformAccountProvider
    : public ICrossPlatformAccountProvider
{
public:
    FSimpleFirstPartyCrossPlatformAccountProvider(){};

    virtual FName GetName() override;
    virtual TSharedPtr<const FCrossPlatformAccountId> CreateCrossPlatformAccountId(
        const FString &InStringRepresentation) override;
    virtual TSharedPtr<const FCrossPlatformAccountId> CreateCrossPlatformAccountId(uint8 *InBytes, int32 InSize)
        override;
    virtual TSharedRef<FAuthenticationGraphNode> GetInteractiveAuthenticationSequence() override;
    virtual TSharedRef<FAuthenticationGraphNode> GetInteractiveOnlyAuthenticationSequence() override;
    virtual TSharedRef<FAuthenticationGraphNode> GetNonInteractiveAuthenticationSequence(
        bool bOnlyUseExternalCredentials) override;
    virtual TSharedRef<FAuthenticationGraphNode> GetUpgradeCurrentAccountToCrossPlatformAccountSequence() override;
    virtual TSharedRef<FAuthenticationGraphNode> GetLinkUnusedExternalCredentialsToCrossPlatformAccountSequence()
        override;
    virtual TSharedRef<FAuthenticationGraphNode> GetNonInteractiveDeauthenticationSequence() override;
    virtual TSharedRef<FAuthenticationGraphNode> GetAutomatedTestingAuthenticationSequence() override;
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION