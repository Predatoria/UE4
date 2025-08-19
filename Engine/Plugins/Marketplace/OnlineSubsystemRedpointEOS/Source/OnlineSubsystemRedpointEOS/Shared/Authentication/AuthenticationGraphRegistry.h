// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraph.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/CrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"

EOS_ENABLE_STRICT_WARNINGS

DECLARE_DELEGATE_RetVal_FourParams(
    FName /* AuthenticationGraphName */,
    FResolveAuthenticationGraphDelegate,
    const TSharedRef<class FAuthenticationGraphRegistry> & /* InRegistry */,
    const TSharedRef<class FEOSConfig> & /* InConfig */,
    const FOnlineAccountCredentials & /* InProvidedCredentials */,
    const TSoftObjectPtr<class UWorld> & /* InWorld */);

class ONLINESUBSYSTEMREDPOINTEOS_API FAuthenticationGraphRegistry : public TSharedFromThis<FAuthenticationGraphRegistry>
{
    friend class FOnlineSubsystemRedpointEOSModule;
    friend class FOnlineSubsystemEOSEditorConfigDetails;

private:
    static TSharedPtr<FAuthenticationGraphRegistry> Instance;

    TMap<FName, FResolveAuthenticationGraphDelegate> RegisteredResolvers;
    TMap<FName, TSharedPtr<FAuthenticationGraph>> RegisteredGraphs;
    TMap<FName, FText> RegisteredDescriptions;
    TMap<FName, TSharedPtr<ICrossPlatformAccountProvider>> RegisteredCrossPlatformAccountProviders;

    /**
     * Called by FOnlineSubsystemRedpointEOSModule when the plugin starts up to register all the built-in authentication
     * graphs.
     */
    static void RegisterDefaults();

public:
    FAuthenticationGraphRegistry() = default;
    UE_NONCOPYABLE(FAuthenticationGraphRegistry);
    virtual ~FAuthenticationGraphRegistry(){};

    /**
     * Registers an authentication graph with a given name. You can then choose the authentication graph appropriate for
     * your platform in Project Settings.
     */
    static void Register(
        FName InAuthenticationGraphName,
        const FText &InAuthenticationGraphDescription,
        const TSharedRef<FAuthenticationGraph> &InAuthenticationGraph);

    /**
     * Registers an authentication graph resolver with a given name. This allows the authentication graph to be
     * dynamically chosen based on platform, availability of graphs and the input credentials.
     */
    static void Register(
        FName InAuthenticationGraphName,
        const FText &InAuthenticationGraphDescription,
        const FResolveAuthenticationGraphDelegate &InAuthenticationGraphResolver);

    /**
     * Registers a placeholder authentication graph so it will appear in the Project Settings dropdown, where it does
     * not actually have an implementation available on Win64/Mac.
     */
    static void RegisterPlaceholder(FName InAuthenticationGraphName, const FText &InAuthenticationGraphDescription);

    /**
     * Registers a cross-platform account provider with a given name. You can then choose the cross-platform account
     * provider appropriate for your platform in Project Settings.
     */
    static void RegisterCrossPlatformAccountProvider(
        FName InAuthenticationGraphName,
        const TSharedRef<ICrossPlatformAccountProvider> &InCrossPlatformAccountProvider);

    /**
     * Returns whether or not a resolver or graph is registered with the given name.
     */
    static bool Has(FName InAuthenticationGraphName);

    /**
     * Returns whether or not a cross-platform account provider is registered with the given name.
     */
    static bool HasCrossPlatformAccountProvider(FName InAuthenticationGraphName);

    /**
     * Retrieves the authentication graph that was registered with the given name, or returns null if no such graph
     * exists.
     */
    static TSharedPtr<FAuthenticationGraph> Get(
        FName InAuthenticationGraphName,
        const TSharedRef<class FEOSConfig> &InConfig,
        const FOnlineAccountCredentials &InProvidedCredentials,
        const TSoftObjectPtr<class UWorld> &InWorld);

    /**
     * Retrieves the cross-platform account provider that was registered with the given name, or returns null if no such
     * cross-platform account provider exists.
     */
    static TSharedPtr<ICrossPlatformAccountProvider> GetCrossPlatformAccountProvider(
        FName InCrossPlatformAccountProviderName);

    /** Returns a list of registered authentication graph names and their descriptions. */
    static TMap<FName, FText> GetNames();

    /** Returns a list of cross-platform account providers. */
    static TArray<FName> GetCrossPlatformAccountProviderNames();
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION