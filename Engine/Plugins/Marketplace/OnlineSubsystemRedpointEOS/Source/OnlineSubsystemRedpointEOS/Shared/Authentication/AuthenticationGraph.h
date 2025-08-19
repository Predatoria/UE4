// Copyright June Rhodes. All Rights Reserved.

#pragma once

#if EOS_HAS_AUTHENTICATION

#include "CoreMinimal.h"
#include "OnlineSubsystemRedpointEOS/Shared/Authentication/AuthenticationGraphNode.h"

EOS_ENABLE_STRICT_WARNINGS

#ifndef EOS_AUTH_ATTRIBUTE_AUTHENTICATEDWITH
#define EOS_AUTH_ATTRIBUTE_AUTHENTICATEDWITH TEXT("authenticatedWith")
#endif

class ONLINESUBSYSTEMREDPOINTEOS_API FAuthenticationGraph : public TSharedFromThis<FAuthenticationGraph>
{
    friend class FAuthenticationGraphNode;
    friend class FJumpToNamedNode;

private:
    TSharedPtr<FAuthenticationGraphNode> Start;
    FAuthenticationGraphNodeOnDone ForwardingOnDone;
    TMap<FName, TSharedPtr<FAuthenticationGraphNode>> RegisteredNodes;

    void __RegisterNode(const FName &InNodeName, const TSharedPtr<FAuthenticationGraphNode> &InNode);
    TSharedPtr<FAuthenticationGraphNode> __GetNode(const FName &InNodeName);

    void HandleOnDone(EAuthenticationGraphNodeResult Result, TSharedRef<FAuthenticationGraphState> ResultingState);
    void HandleCleanupOnDone(
        EAuthenticationGraphNodeResult Result,
        TSharedRef<FAuthenticationGraphState> ResultingState,
        EAuthenticationGraphNodeResult OriginalResult);

protected:
    virtual TSharedRef<FAuthenticationGraphNode> CreateGraph(
        const TSharedRef<FAuthenticationGraphState> &InitialState) = 0;

public:
    static bool Condition_Unauthenticated(const FAuthenticationGraphState &State);
    static bool Condition_HasCrossPlatformAccountProvider(const FAuthenticationGraphState &State);
    static bool Condition_RequireCrossPlatformAccount(const FAuthenticationGraphState &State);
    static bool Condition_CrossPlatformAccountIsValid(const FAuthenticationGraphState &State);
    static bool Condition_CrossPlatformAccountIsValidWithPUID(const FAuthenticationGraphState &State);
    static bool Condition_OneSuccessfulCandidate(const FAuthenticationGraphState &State);
    static bool Condition_MoreThanOneSuccessfulCandidate(const FAuthenticationGraphState &State);
    static bool Condition_NoSuccessfulCandidate_AtLeastOneContinuanceToken(const FAuthenticationGraphState &State);
    static bool Condition_NoSuccessfulCandidate_NoContinuanceToken(const FAuthenticationGraphState &State);
    static bool Condition_CrossPlatformProvidedContinuanceToken(const FAuthenticationGraphState &State);
    static bool Condition_IsSwitchToCrossPlatformAccount(const FAuthenticationGraphState &State);
    static bool Condition_IsLinkADifferentAccount(const FAuthenticationGraphState &State);

    void Execute(const TSharedRef<FAuthenticationGraphState> &State, const FAuthenticationGraphNodeOnDone &OnDone);

    FAuthenticationGraph() = default;
    virtual ~FAuthenticationGraph(){};
    UE_NONCOPYABLE(FAuthenticationGraph);
};

EOS_DISABLE_STRICT_WARNINGS

#endif // #if EOS_HAS_AUTHENTICATION