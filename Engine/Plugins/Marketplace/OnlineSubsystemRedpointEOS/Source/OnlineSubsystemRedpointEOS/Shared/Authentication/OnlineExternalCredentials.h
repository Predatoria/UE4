// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"

EOS_ENABLE_STRICT_WARNINGS

DECLARE_DELEGATE_OneParam(FOnlineExternalCredentialsRefreshComplete, bool /* bWasSuccessful */);

/**
 * Similar to FOnlineAccountCredentials, but also capable of storing a refresh callback that is used to be obtain a new
 * token value when the existing token expires.
 *
 * This is a heap-allocated class unlike FOnlineAccountCredentials; you should construct implementations with
 * MakeShared<>.
 */
class ONLINESUBSYSTEMREDPOINTEOS_API IOnlineExternalCredentials : public TSharedFromThis<IOnlineExternalCredentials>
{
public:
    IOnlineExternalCredentials() = default;
    virtual ~IOnlineExternalCredentials(){};
    UE_NONCOPYABLE(IOnlineExternalCredentials);

    /** Returns the display name of the credential provider (e.g. "Steam"). Shown to the end user. */
    virtual FText GetProviderDisplayName() const = 0;
    /** Returns the type of credential. */
    virtual FString GetType() const = 0;
    /** Returns the ID (username or display name) for the credential. */
    virtual FString GetId() const = 0;
    /** Returns the token for authentication.. */
    virtual FString GetToken() const = 0;
    /** Returns the authentication attributes for the user. */
    virtual TMap<FString, FString> GetAuthAttributes() const = 0;
    /** The native subsystem name to use for external UI and e-commerce. Return NAME_None if there is no native
     * subsystem for these interfaces with this credential type. */
    virtual FName GetNativeSubsystemName() const = 0;

    /**
     * Refresh the credentials. OnComplete will be called when you can call GetType/GetId/GetToken again to get the new
     * values.
     */
    virtual void Refresh(
        TSoftObjectPtr<UWorld> InWorld,
        int32 LocalUserNum,
        FOnlineExternalCredentialsRefreshComplete OnComplete) = 0;
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION