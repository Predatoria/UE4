// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraphRegistry.h"

#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/AutomatedTestingCrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/EpicGamesCrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/CrossPlatform/SimpleFirstPartyCrossPlatformAccountProvider.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Graphs/AuthenticationGraphAlwaysFail.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Graphs/AuthenticationGraphAnonymous.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Graphs/AuthenticationGraphAutomatedTesting.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Graphs/AuthenticationGraphCrossPlatformOnly.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Graphs/AuthenticationGraphDefault.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Graphs/AuthenticationGraphDevAuthTool.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/Graphs/AuthenticationGraphOnlineSubsystem.h"

EOS_ENABLE_STRICT_WARNINGS

TSharedPtr<FAuthenticationGraphRegistry> FAuthenticationGraphRegistry::Instance = nullptr;

void FAuthenticationGraphRegistry::RegisterDefaults()
{
    FAuthenticationGraphDefault::Register();
    FAuthenticationGraphAlwaysFail::Register();
    FAuthenticationGraphCrossPlatformOnly::Register();
    FAuthenticationGraphAnonymous::Register();
    FAuthenticationGraphDevAuthTool::Register();

#if !defined(UE_BUILD_SHIPPING) || !UE_BUILD_SHIPPING
    FAuthenticationGraphAutomatedTesting::Register();
    FAuthenticationGraphRegistry::RegisterCrossPlatformAccountProvider(
        FName(TEXT("AutomatedTesting")),
        MakeShared<FAutomatedTestingCrossPlatformAccountProvider>());
#endif
    FAuthenticationGraphRegistry::RegisterCrossPlatformAccountProvider(
        FName(TEXT("EpicGames")),
        MakeShared<FEpicGamesCrossPlatformAccountProvider>());
    FAuthenticationGraphRegistry::RegisterCrossPlatformAccountProvider(
        FName(TEXT("SimpleFirstParty")),
        MakeShared<FSimpleFirstPartyCrossPlatformAccountProvider>());
}

void FAuthenticationGraphRegistry::Register(
    FName InAuthenticationGraphName,
    const FText &InAuthenticationGraphDescription,
    const TSharedRef<FAuthenticationGraph> &InAuthenticationGraph)
{
    if (!FAuthenticationGraphRegistry::Instance.IsValid())
    {
        FAuthenticationGraphRegistry::Instance = MakeShared<FAuthenticationGraphRegistry>();
    }

    checkf(
        !FAuthenticationGraphRegistry::Instance->RegisteredResolvers.Contains(InAuthenticationGraphName),
        TEXT("Attempt to register authentication graph %s but an authentication graph resolver with that name is "
             "already registered."),
        *InAuthenticationGraphName.ToString());

    FAuthenticationGraphRegistry::Instance->RegisteredGraphs.Add(InAuthenticationGraphName, InAuthenticationGraph);
    FAuthenticationGraphRegistry::Instance->RegisteredDescriptions.Add(
        InAuthenticationGraphName,
        InAuthenticationGraphDescription);
}

void FAuthenticationGraphRegistry::Register(
    FName InAuthenticationGraphName,
    const FText &InAuthenticationGraphDescription,
    const FResolveAuthenticationGraphDelegate &InAuthenticationGraphResolver)
{
    if (!FAuthenticationGraphRegistry::Instance.IsValid())
    {
        FAuthenticationGraphRegistry::Instance = MakeShared<FAuthenticationGraphRegistry>();
    }

    checkf(
        !FAuthenticationGraphRegistry::Instance->RegisteredGraphs.Contains(InAuthenticationGraphName),
        TEXT("Attempt to register authentication graph resolver %s but an authentication graph with that name is "
             "already registered."),
        *InAuthenticationGraphName.ToString());

    FAuthenticationGraphRegistry::Instance->RegisteredResolvers.Add(
        InAuthenticationGraphName,
        InAuthenticationGraphResolver);
    FAuthenticationGraphRegistry::Instance->RegisteredDescriptions.Add(
        InAuthenticationGraphName,
        InAuthenticationGraphDescription);
}

void FAuthenticationGraphRegistry::RegisterPlaceholder(
    FName InAuthenticationGraphName,
    const FText &InAuthenticationGraphDescription)
{
    if (!FAuthenticationGraphRegistry::Instance.IsValid())
    {
        FAuthenticationGraphRegistry::Instance = MakeShared<FAuthenticationGraphRegistry>();
    }

    checkf(
        !FAuthenticationGraphRegistry::Instance->RegisteredResolvers.Contains(InAuthenticationGraphName),
        TEXT("Attempt to register placeholder authentication graph %s but an authentication graph resolver with that "
             "name is "
             "already registered."),
        *InAuthenticationGraphName.ToString());

    FAuthenticationGraphRegistry::Instance->RegisteredGraphs.Add(
        InAuthenticationGraphName,
        MakeShared<FAuthenticationGraphAlwaysFail>());
    FAuthenticationGraphRegistry::Instance->RegisteredDescriptions.Add(
        InAuthenticationGraphName,
        InAuthenticationGraphDescription);
}

void FAuthenticationGraphRegistry::RegisterCrossPlatformAccountProvider(
    FName InAuthenticationGraphName,
    const TSharedRef<ICrossPlatformAccountProvider> &InCrossPlatformAccountProvider)
{
    if (!FAuthenticationGraphRegistry::Instance.IsValid())
    {
        FAuthenticationGraphRegistry::Instance = MakeShared<FAuthenticationGraphRegistry>();
    }

    checkf(
        !FAuthenticationGraphRegistry::Instance->RegisteredGraphs.Contains(InAuthenticationGraphName),
        TEXT("Attempt to register cross-platform account provider %s but a cross-platform account provider with that "
             "name is "
             "already registered."),
        *InAuthenticationGraphName.ToString());

    FAuthenticationGraphRegistry::Instance->RegisteredCrossPlatformAccountProviders.Add(
        InAuthenticationGraphName,
        InCrossPlatformAccountProvider);
}

bool FAuthenticationGraphRegistry::Has(FName InAuthenticationGraphName)
{
    if (!FAuthenticationGraphRegistry::Instance.IsValid())
    {
        FAuthenticationGraphRegistry::Instance = MakeShared<FAuthenticationGraphRegistry>();
    }

    return FAuthenticationGraphRegistry::Instance->RegisteredResolvers.Contains(InAuthenticationGraphName) ||
           FAuthenticationGraphRegistry::Instance->RegisteredGraphs.Contains(InAuthenticationGraphName);
}

bool FAuthenticationGraphRegistry::HasCrossPlatformAccountProvider(FName InAuthenticationGraphName)
{
    if (!FAuthenticationGraphRegistry::Instance.IsValid())
    {
        FAuthenticationGraphRegistry::Instance = MakeShared<FAuthenticationGraphRegistry>();
    }

    return FAuthenticationGraphRegistry::Instance->RegisteredCrossPlatformAccountProviders.Contains(
        InAuthenticationGraphName);
}

TSharedPtr<FAuthenticationGraph> FAuthenticationGraphRegistry::Get(
    FName InAuthenticationGraphName,
    const TSharedRef<class FEOSConfig> &InConfig,
    const FOnlineAccountCredentials &InProvidedCredentials,
    const TSoftObjectPtr<class UWorld> &InWorld)
{
    if (!FAuthenticationGraphRegistry::Instance.IsValid())
    {
        FAuthenticationGraphRegistry::Instance = MakeShared<FAuthenticationGraphRegistry>();
    }

    if (FAuthenticationGraphRegistry::Instance->RegisteredResolvers.Contains(InAuthenticationGraphName))
    {
        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("Authentication graph registry is resolving dynamic graph name: %s"),
            *InAuthenticationGraphName.ToString());
        FResolveAuthenticationGraphDelegate Resolver =
            FAuthenticationGraphRegistry::Instance->RegisteredResolvers[InAuthenticationGraphName];
        checkf(Resolver.IsBound(), TEXT("Expected authentication graph resolver delegate to be bound to a function."));
        FName ResolvedName = Resolver.Execute(
            FAuthenticationGraphRegistry::Instance.ToSharedRef(),
            InConfig,
            InProvidedCredentials,
            InWorld);
        return FAuthenticationGraphRegistry::Get(ResolvedName, InConfig, InProvidedCredentials, InWorld);
    }

    if (FAuthenticationGraphRegistry::Instance->RegisteredGraphs.Contains(InAuthenticationGraphName))
    {
        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("Authentication graph registry is returning graph registered with name: %s"),
            *InAuthenticationGraphName.ToString());
        return FAuthenticationGraphRegistry::Instance->RegisteredGraphs[InAuthenticationGraphName];
    }

    UE_LOG(
        LogEOS,
        Error,
        TEXT("Authentication graph registry could not find a graph registered with name: %s"),
        *InAuthenticationGraphName.ToString());
    return nullptr;
}

TSharedPtr<ICrossPlatformAccountProvider> FAuthenticationGraphRegistry::GetCrossPlatformAccountProvider(
    FName InCrossPlatformAccountProviderName)
{
    if (!FAuthenticationGraphRegistry::Instance.IsValid())
    {
        FAuthenticationGraphRegistry::Instance = MakeShared<FAuthenticationGraphRegistry>();
    }

    if (FAuthenticationGraphRegistry::Instance->RegisteredCrossPlatformAccountProviders.Contains(
            InCrossPlatformAccountProviderName))
    {
        UE_LOG(
            LogEOS,
            Verbose,
            TEXT("Authentication graph registry is returning cross-platform account provider: %s"),
            *InCrossPlatformAccountProviderName.ToString());
        return FAuthenticationGraphRegistry::Instance
            ->RegisteredCrossPlatformAccountProviders[InCrossPlatformAccountProviderName];
    }

    UE_LOG(
        LogEOS,
        Error,
        TEXT("Authentication graph registry could not find a cross-platform account provider registered with name: %s"),
        *InCrossPlatformAccountProviderName.ToString());
    return nullptr;
}

TMap<FName, FText> FAuthenticationGraphRegistry::GetNames()
{
    if (!FAuthenticationGraphRegistry::Instance.IsValid())
    {
        FAuthenticationGraphRegistry::Instance = MakeShared<FAuthenticationGraphRegistry>();
    }

    TMap<FName, FText> Results;
    for (const auto &KV : FAuthenticationGraphRegistry::Instance->RegisteredResolvers)
    {
        Results.Add(KV.Key, FAuthenticationGraphRegistry::Instance->RegisteredDescriptions[KV.Key]);
    }
    for (const auto &KV : FAuthenticationGraphRegistry::Instance->RegisteredGraphs)
    {
        Results.Add(KV.Key, FAuthenticationGraphRegistry::Instance->RegisteredDescriptions[KV.Key]);
    }
    return Results;
}

TArray<FName> FAuthenticationGraphRegistry::GetCrossPlatformAccountProviderNames()
{
    if (!FAuthenticationGraphRegistry::Instance.IsValid())
    {
        FAuthenticationGraphRegistry::Instance = MakeShared<FAuthenticationGraphRegistry>();
    }

    TArray<FName> Results;
    for (const auto &KV : FAuthenticationGraphRegistry::Instance->RegisteredCrossPlatformAccountProviders)
    {
        Results.Add(KV.Key);
    }
    return Results;
}

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION