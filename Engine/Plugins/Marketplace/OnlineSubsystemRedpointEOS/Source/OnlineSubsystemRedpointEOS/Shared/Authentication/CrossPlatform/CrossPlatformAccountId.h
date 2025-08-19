// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"

EOS_ENABLE_STRICT_WARNINGS

#define AUTOMATED_TESTING_ACCOUNT_ID FName(TEXT("AutomatedTesting"))
#define EPIC_GAMES_ACCOUNT_ID FName(TEXT("EpicGames"))
#define SIMPLE_FIRST_PARTY_ACCOUNT_ID FName(TEXT("SimpleFirstParty"))

/**
 * Abstraction of a cross-platform account ID
 */
class ONLINESUBSYSTEMREDPOINTEOS_API FCrossPlatformAccountId : public TSharedFromThis<FCrossPlatformAccountId>
{
protected:
    FCrossPlatformAccountId() = default;

    virtual bool Compare(const FCrossPlatformAccountId &Other) const
    {
        return (GetType().IsEqual(Other.GetType()) && GetSize() == Other.GetSize()) &&
               (FMemory::Memcmp(GetBytes(), Other.GetBytes(), GetSize()) == 0);
    }

public:
    UE_NONCOPYABLE(FCrossPlatformAccountId);
    virtual ~FCrossPlatformAccountId(){};

    /**
     *	Comparison operator
     */
    friend bool operator==(const FCrossPlatformAccountId &Lhs, const FCrossPlatformAccountId &Rhs)
    {
        return Lhs.Compare(Rhs);
    }
    friend bool operator!=(const FCrossPlatformAccountId &Lhs, const FCrossPlatformAccountId &Rhs)
    {
        return !Lhs.Compare(Rhs);
    }

    /**
     * Return the type of cross-platform account this is.
     */
    virtual FName GetType() const = 0;

    /**
     * Return the raw byte representation of this opaque data.
     */
    virtual const uint8 *GetBytes() const = 0;

    /**
     * Returns the size of the opaque data.
     */
    virtual int32 GetSize() const = 0;

    /**
     * Returns if this cross-platform account ID is valid.
     */
    virtual bool IsValid() const = 0;

    /**
     * Convert the cross-platform account ID to it's string representation.
     */
    virtual FString ToString() const = 0;
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION