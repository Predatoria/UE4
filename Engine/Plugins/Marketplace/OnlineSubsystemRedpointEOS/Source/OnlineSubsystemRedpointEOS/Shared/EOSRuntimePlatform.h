// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/UniqueNetIdEOSSession.h"
#include "UObject/SoftObjectPtr.h"

EOS_ENABLE_STRICT_WARNINGS

struct ONLINESUBSYSTEMREDPOINTEOS_API FExternalAccountIdInfo
{
public:
    EOS_EExternalAccountType AccountIdType;
    FString AccountId;
};

class ONLINESUBSYSTEMREDPOINTEOS_API IEOSRuntimePlatformIntegration
{
public:
    /**
     * Attempts to convert the provided external user information into an online subsystem user ID. The GetType()
     * function of the user ID should map to a loaded online subsystem, so that the caller can use the user ID with that
     * online subsystem's APIs.
     */
    virtual TSharedPtr<const class FUniqueNetId> GetUserId(
        TSoftObjectPtr<class UWorld> InWorld,
        EOS_Connect_ExternalAccountInfo *InExternalInfo) = 0;

    /**
     * Returns if the specified user ID can be converted into an FExternalAccountIdInfo for the friends system to look
     * up the EOS account by external info.
     */
    virtual bool CanProvideExternalAccountId(const FUniqueNetId &InUserId) = 0;

    /**
     * Returns the external account information for the given user ID.
     */
    virtual FExternalAccountIdInfo GetExternalAccountId(const FUniqueNetId &InUserId) = 0;

    /**
     * Called when the list name is being passed into a wrapped subsystem inside OnlineFriendsInterfaceSynthetic,
     * offering the integration the chance to update the list name (in case the platform restricts valid values).
     */
    virtual void MutateFriendsListNameIfRequired(FName InSubsystemName, FString &InOutListName) const {};

    /**
     * Called immediately after the session name is generated for a synthetic party, offering the integration a chance
     * to force a particular session name on the local subsystem (in case the platform requires specific values).
     */
    virtual void MutateSyntheticPartySessionNameIfRequired(
        FName InSubsystemName,
        const TSharedPtr<const FOnlinePartyId> &EOSPartyId,
        FString &InOutSessionName) const {};

    /**
     * Called immediately after the session name is generated for a synthetic session, offering the integration a chance
     * to force a particular session name on the local subsystem (in case the platform requires specific values).
     */
    virtual void MutateSyntheticSessionSessionNameIfRequired(
        FName InSubsystemName,
        const TSharedPtr<const FUniqueNetIdEOSSession> &EOSSessionId,
        FString &InOutSessionName) const {};
};

class ONLINESUBSYSTEMREDPOINTEOS_API IEOSRuntimePlatform
{
public:
    /**
     * Load the EOS SDK runtime library.
     */
    virtual void Load() = 0;

    /**
     * Unload the EOS SDK runtime library.
     */
    virtual void Unload() = 0;

    /**
     * Return true if the EOS SDK runtime library could be loaded, and EOS can be used.
     */
    virtual bool IsValid() = 0;

    /**
     * Returns the system initialization options that are passed to EOS_InitializeOptions.
     */
    virtual void *GetSystemInitializeOptions() = 0;

#if EOS_VERSION_AT_LEAST(1, 13, 0)
    /**
     * Returns the RTC options that are passed to EOS_Platform_Options.
     */
    virtual EOS_Platform_RTCOptions *GetRTCOptions()
    {
        return nullptr;
    }
#endif

    /**
     * Returns the path to cache directory that is passed to EOS_Platform_Options.
     */
    virtual FString GetCacheDirectory() = 0;

    /**
     * DEPRECATED.
     */
    virtual void ClearStoredEASRefreshToken(int32 LocalUserNum) = 0;

    /**
     * Returns the path to the Epic Games account credentials used for automated testing.
     */
#if !UE_BUILD_SHIPPING
    virtual FString GetPathToEASAutomatedTestingCredentials() = 0;
#endif

    /**
     * Returns all platform integrations available on this platform.
     */
    virtual const TArray<TSharedPtr<IEOSRuntimePlatformIntegration>> &GetIntegrations() const = 0;

    /**
     * Returns the name of the platform as a string (as per the EOS_EAntiCheatCommonClientPlatform enumeration). This
     * value is only used on trusted platforms like consoles.
     */
    virtual FString GetAntiCheatPlatformName() const
    {
        return TEXT("Unknown");
    };

    /**
     * Returns whether this platform is allowed to cache friends in
     * Player Data Storage and read friends from Player Data Storage.
     *
     * If this is false, friends are not synced to Player Data Storage,
     * and the synthetic friends interface will not return them as part
     * of ReadFriendsList.
     *
     * Note that sending friend invites for pure EOS friends is still
     * permitted; this just controls whether or not friends from other
     * platform's social graphs will be visible.
     */
    virtual bool UseCrossPlatformFriendStorage() const
    {
        return false;
    };

#if EOS_VERSION_AT_LEAST(1, 15, 0)
    /**
     * Called during startup to register code that will handle calling EOS_Platform_SetApplicationStatus and
     * EOS_Platform_SetNetworkStatus in response to operating system events. If you need to poll, this implementation
     * should be empty and you should implement ShouldPollLifecycleStatus and PollLifecycleStatus instead.
     */
    virtual void RegisterLifecycleHandlers(const TWeakPtr<class FEOSLifecycleManager> &InLifecycleManager){};

    /**
     * Called after a new platform instance is successfully created, and should be used to set the initial application
     * and network lifecycle of the instance.
     */
    virtual void SetLifecycleForNewPlatformInstance(EOS_HPlatform InPlatform){};

    /**
     * Called during startup to see if this platform should poll every tick for the application status. The
     * implementation of PollLifecycleApplicationStatus must be very quick as it will be called every frame.
     */
    virtual bool ShouldPollLifecycleApplicationStatus() const
    {
        return false;
    }

    /**
     * Called during startup to see if this platform should poll every tick for the application status. The
     * implementation of PollLifecycleNetworkStatus must be very quick as it will be called every frame.
     */
    virtual bool ShouldPollLifecycleNetworkStatus() const
    {
        return false;
    }

    /**
     * Polls the current application status and returns it. Only called if you override
     * ShouldPollLifecycleApplicationStatus and make it return true.
     */
    virtual void PollLifecycleApplicationStatus(EOS_EApplicationStatus &OutApplicationStatus) const
    {
        OutApplicationStatus = EOS_EApplicationStatus::EOS_AS_Foreground;
    };

    /**
     * Polls the current network status and returns it. Only called if you override
     * ShouldPollLifecycleApplicationStatus and make it return true.
     */
    virtual void PollLifecycleNetworkStatus(EOS_ENetworkStatus &OutNetworkStatus) const
    {
        OutNetworkStatus = EOS_ENetworkStatus::EOS_NS_Online;
    };
#endif
};

EOS_DISABLE_STRICT_WARNINGS