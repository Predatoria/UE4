// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/CrossPlatformAccountId.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/CrossPlatformAccountProvider.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FEpicGamesCrossPlatformAccountId : public FCrossPlatformAccountId
{
private:
    uint8 *DataBytes;
    int32 DataBytesSize;
    EOS_EpicAccountId EpicAccountId;

public:
    FEpicGamesCrossPlatformAccountId(EOS_EpicAccountId InEpicAccountId);
    virtual ~FEpicGamesCrossPlatformAccountId();
    UE_NONCOPYABLE(FEpicGamesCrossPlatformAccountId);

    virtual bool Compare(const FCrossPlatformAccountId &Other) const override;
    virtual FName GetType() const override;
    virtual const uint8 *GetBytes() const override;
    virtual int32 GetSize() const override;
    virtual bool IsValid() const override;
    virtual FString ToString() const override;

    static TSharedPtr<const FCrossPlatformAccountId> ParseFromString(const FString &In);

    bool HasValidEpicAccountId() const;
    EOS_EpicAccountId GetEpicAccountId() const;
    FString GetEpicAccountIdString() const;
};

class ONLINESUBSYSTEMREDPOINTEOS_API FEpicGamesCrossPlatformAccountProvider : public ICrossPlatformAccountProvider
{
public:
    FEpicGamesCrossPlatformAccountProvider(){};

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

// Returns the error to propagate to the user if Epic Games sign in is not available due to the cross-play status.
ONLINESUBSYSTEMREDPOINTEOS_API FString CheckForDesktopCrossplayError(EOS_HPlatform InPlatform);
#if EOS_VERSION_AT_LEAST(1, 15, 0)
// Converts the desktop cross-play status to a string for error reporting.
ONLINESUBSYSTEMREDPOINTEOS_API FString DesktopCrossplayStatusToString(EOS_EDesktopCrossplayStatus InStatus);
#endif

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION