// Copyright June Rhodes. All Rights Reserved.

#if EOS_HAS_AUTHENTICATION

#include "OnlineAvatarInterfaceSynthetic.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSTextureLoader.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineFriendSynthetic.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineUserEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

FOnlineAvatarInterfaceSynthetic::FOnlineAvatarInterfaceSynthetic(
    FName InInstanceName,
    const TSharedRef<FEOSConfig> &InConfig,
    TSharedPtr<FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> InIdentity,
    IOnlineFriendsPtr InFriends,
    IOnlineUserPtr InUser)
    : DelegatedAvatar()
    , DelegatedIdentity()
    , InstanceName(InInstanceName)
    , Config(InConfig)
    , Identity(MoveTemp(InIdentity))
    , Friends(MoveTemp(InFriends))
    , User(MoveTemp(InUser))
    // It's not safe to initialize DelegatedIdentity and DelegatedAvatar here as most OSS implementations try to use the
    // UObject system when calling GetNamedInterface (which is what Online::GetAvatarInterface uses internally). So we
    // must defer until later to ensure that the UObject system is running before we try to get the delegated
    // subsystems.
    , bInitialized(false)
{
}

void FOnlineAvatarInterfaceSynthetic::OnDownloadAvatarComplete(
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FHttpRequestPtr Request,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FHttpResponsePtr Response,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    bool bConnectedSuccessfully,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSoftObjectPtr<UTexture> DefaultTexture,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FOnGetAvatarComplete OnComplete)
{
    if (!bConnectedSuccessfully || !Response.IsValid() || !EHttpResponseCodes::IsOk(Response->GetResponseCode()))
    {
        OnComplete.ExecuteIfBound(false, DefaultTexture);
        return;
    }

    UTexture *LoadedTexture = FEOSTextureLoader::LoadTextureFromHttpResponse(Response);
    if (IsValid(LoadedTexture))
    {
        OnComplete.ExecuteIfBound(true, LoadedTexture);
    }
    else
    {
        OnComplete.ExecuteIfBound(false, DefaultTexture);
    }
}

void FOnlineAvatarInterfaceSynthetic::OnQueryUserInfoComplete(
    int32 LocalUserNum,
    bool bWasSuccessful,
    const TArray<TSharedRef<const FUniqueNetId>> &TargetUserIds,
    const FString &ErrorMessage,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<FDelegateHandle> AllocatedHandle,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<const FUniqueNetId> IntendedUserId,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    FOnGetAvatarComplete OnComplete,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSoftObjectPtr<UTexture> DefaultTexture)
{
    // Do we want to handle this event?
    bool bWasIntendedEvent = false;
    for (const auto &TargetUserId : TargetUserIds)
    {
        if (*TargetUserId == *IntendedUserId)
        {
            bWasIntendedEvent = true;
            break;
        }
    }
    if (!bWasIntendedEvent)
    {
        return;
    }

    // Clear callback.
    this->User->ClearOnQueryUserInfoCompleteDelegate_Handle(LocalUserNum, *AllocatedHandle);

    // If we weren't successful, return the default texture.
    if (!bWasSuccessful)
    {
        OnComplete.ExecuteIfBound(false, DefaultTexture);
        return;
    }

    // Try to retrieve the (now cached) information about the user.
    TSharedPtr<FOnlineUser> CachedUser = this->User->GetUserInfo(LocalUserNum, *IntendedUserId);
    if (CachedUser.IsValid())
    {
        for (const auto &WrappedTargetUserId : StaticCastSharedPtr<FOnlineUserEOS>(CachedUser)->GetExternalUserIds())
        {
            if (this->DelegatedAvatar.Contains(WrappedTargetUserId->GetType()) &&
                this->DelegatedIdentity.Contains(WrappedTargetUserId->GetType()))
            {
                if (auto WrappedIdentity = this->DelegatedIdentity[WrappedTargetUserId->GetType()].Implementation.Pin())
                {
                    TSharedPtr<const FUniqueNetId> WrappedLocalUserId =
                        WrappedIdentity->GetUniquePlayerId(LocalUserNum);
                    if (WrappedLocalUserId.IsValid())
                    {
                        if (auto WrappedAvatar =
                                this->DelegatedAvatar[WrappedTargetUserId->GetType()].Implementation.Pin())
                        {
                            WrappedAvatar
                                ->GetAvatar(*WrappedLocalUserId, *WrappedTargetUserId, DefaultTexture, OnComplete);
                            return;
                        }
                    }
                }
            }
        }
    }

    // For some reason, the information wasn't in the cache.
    OnComplete.ExecuteIfBound(false, DefaultTexture);
    return;
}

bool FOnlineAvatarInterfaceSynthetic::GetAvatar(
    const FUniqueNetId &LocalUserId,
    const FUniqueNetId &TargetUserId,
    TSoftObjectPtr<UTexture> DefaultTexture,
    FOnGetAvatarComplete OnComplete)
{
    if (!this->bInitialized)
    {
        this->DelegatedIdentity = FDelegatedSubsystems::GetDelegatedInterfaces<IOnlineIdentity>(
            this->Config.ToSharedRef(),
            this->InstanceName,
            [](IOnlineSubsystem *OSS) {
                return OSS->GetIdentityInterface();
            });
        this->DelegatedAvatar = FDelegatedSubsystems::GetDelegatedInterfaces<IOnlineAvatar>(
            this->Config.ToSharedRef(),
            this->InstanceName,
            [](IOnlineSubsystem *OSS) {
                return Online::GetAvatarInterface(OSS);
            });
        this->bInitialized = true;
    }

    if (LocalUserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("LocalUserId is not an EOS user when calling GetAvatar."));
        OnComplete.ExecuteIfBound(false, DefaultTexture);
        return true;
    }

    int32 LocalUserNum;
    if (!this->Identity->GetLocalUserNum(LocalUserId, LocalUserNum))
    {
        UE_LOG(LogEOS, Error, TEXT("LocalUserId is not a locally signed in EOS user."));
        OnComplete.ExecuteIfBound(false, DefaultTexture);
        return true;
    }

    if (TargetUserId.GetType() == REDPOINT_EOS_SUBSYSTEM)
    {
        // Target user might be a local user.
        for (const auto &Account : this->Identity->GetAllUserAccounts())
        {
            if (*Account->GetUserId() == TargetUserId)
            {
                // Found the user as a local user.
                for (const auto &WrappedTargetUserId :
                     StaticCastSharedPtr<FUserOnlineAccountEOS>(Account)->GetExternalUserIds())
                {
                    if (this->DelegatedAvatar.Contains(WrappedTargetUserId->GetType()) &&
                        this->DelegatedIdentity.Contains(WrappedTargetUserId->GetType()))
                    {
                        if (auto WrappedIdentity =
                                this->DelegatedIdentity[WrappedTargetUserId->GetType()].Implementation.Pin())
                        {
                            TSharedPtr<const FUniqueNetId> WrappedLocalUserId =
                                WrappedIdentity->GetUniquePlayerId(LocalUserNum);
                            if (WrappedLocalUserId.IsValid())
                            {
                                if (auto WrappedAvatar =
                                        this->DelegatedAvatar[WrappedTargetUserId->GetType()].Implementation.Pin())
                                {
                                    WrappedAvatar->GetAvatar(
                                        *WrappedLocalUserId,
                                        *WrappedTargetUserId,
                                        DefaultTexture,
                                        OnComplete);
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }

        // Target user might be a friend.
        TSharedPtr<FOnlineFriend> Friend = this->Friends->GetFriend(LocalUserNum, TargetUserId, TEXT(""));
        if (Friend.IsValid() && Friend->GetUserId()->GetType() == REDPOINT_EOS_SUBSYSTEM)
        {
            // Found the user as a friend.
            for (const auto &WrappedFriend : StaticCastSharedPtr<FOnlineFriendSynthetic>(Friend)->GetWrappedFriends())
            {
                // The wrapped friend might be a *cached* friend, in which case we get the avatar URL from the
                // user attributes, rather than calling into a delegated subsystem which is not available.
                FString IsCached, CachedAvatarUrl;
                if (WrappedFriend.Value.IsValid() &&
                    WrappedFriend.Value->GetUserAttribute(TEXT("isCachedFriend"), IsCached) &&
                    IsCached == TEXT("true") &&
                    WrappedFriend.Value->GetUserAttribute(TEXT("cachedAvatarUrl"), CachedAvatarUrl))
                {
                    // Cached user; fetch by URL.
                    auto Request = FHttpModule::Get().CreateRequest();
                    Request->SetVerb("GET");
                    Request->SetURL(CachedAvatarUrl);
                    Request->OnProcessRequestComplete().BindThreadSafeSP(
                        this,
                        &FOnlineAvatarInterfaceSynthetic::OnDownloadAvatarComplete,
                        MoveTemp(DefaultTexture),
                        MoveTemp(OnComplete));
                    Request->ProcessRequest();
                    return true;
                }
                else if (
                    this->DelegatedAvatar.Contains(WrappedFriend.Key) &&
                    this->DelegatedIdentity.Contains(WrappedFriend.Key))
                {
                    if (auto WrappedIdentity = this->DelegatedIdentity[WrappedFriend.Key].Implementation.Pin())
                    {
                        TSharedPtr<const FUniqueNetId> WrappedLocalUserId =
                            WrappedIdentity->GetUniquePlayerId(LocalUserNum);
                        if (WrappedLocalUserId.IsValid())
                        {
                            if (auto WrappedAvatar = this->DelegatedAvatar[WrappedFriend.Key].Implementation.Pin())
                            {
                                WrappedAvatar->GetAvatar(
                                    *WrappedLocalUserId,
                                    *WrappedFriend.Value->GetUserId(),
                                    DefaultTexture,
                                    OnComplete);
                                return true;
                            }
                        }
                    }
                }
            }
        }

        // See if we already have cached information about this user.
        TSharedPtr<FOnlineUser> CachedUser = this->User->GetUserInfo(LocalUserNum, TargetUserId);
        if (CachedUser.IsValid())
        {
            for (const auto &WrappedTargetUserId :
                 StaticCastSharedPtr<FOnlineUserEOS>(CachedUser)->GetExternalUserIds())
            {
                if (this->DelegatedAvatar.Contains(WrappedTargetUserId->GetType()) &&
                    this->DelegatedIdentity.Contains(WrappedTargetUserId->GetType()))
                {
                    if (auto WrappedIdentity =
                            this->DelegatedIdentity[WrappedTargetUserId->GetType()].Implementation.Pin())
                    {
                        TSharedPtr<const FUniqueNetId> WrappedLocalUserId =
                            WrappedIdentity->GetUniquePlayerId(LocalUserNum);
                        if (WrappedLocalUserId.IsValid())
                        {
                            if (auto WrappedAvatar =
                                    this->DelegatedAvatar[WrappedTargetUserId->GetType()].Implementation.Pin())
                            {
                                WrappedAvatar
                                    ->GetAvatar(*WrappedLocalUserId, *WrappedTargetUserId, DefaultTexture, OnComplete);
                                return true;
                            }
                        }
                    }
                }
            }
        }

        // Query the target user ID on demand with IOnlineUser.
        TSharedRef<FDelegateHandle> AllocatedHandle = MakeShared<FDelegateHandle>();
        *AllocatedHandle = this->User->AddOnQueryUserInfoCompleteDelegate_Handle(
            LocalUserNum,
            FOnQueryUserInfoCompleteDelegate::CreateThreadSafeSP(
                this,
                &FOnlineAvatarInterfaceSynthetic::OnQueryUserInfoComplete,
                AllocatedHandle,
                TargetUserId.AsShared(),
                OnComplete,
                DefaultTexture));
        TArray<TSharedRef<const FUniqueNetId>> TargetUserIds;
        TargetUserIds.Add(TargetUserId.AsShared());
        if (!this->User->QueryUserInfo(LocalUserNum, TargetUserIds))
        {
            // Could not start query.
            this->User->ClearOnQueryUserInfoCompleteDelegate_Handle(LocalUserNum, *AllocatedHandle);
            OnComplete.ExecuteIfBound(false, DefaultTexture);
            return true;
        }

        // We are running the query in the background.
        return true;
    }
    else
    {
        // If the target user is a friend, they might be a *cached* friend, in which case we get the avatar URL
        // from the user attributes, rather than calling into a delegated subsystem which is not available.
        TSharedPtr<FOnlineFriend> Friend = this->Friends->GetFriend(LocalUserNum, TargetUserId, TEXT(""));
        FString IsCached, CachedAvatarUrl;
        if (Friend.IsValid() && Friend->GetUserAttribute(TEXT("isCachedFriend"), IsCached) &&
            IsCached == TEXT("true") && Friend->GetUserAttribute(TEXT("cachedAvatarUrl"), CachedAvatarUrl))
        {
            // Cached user; fetch by URL.
            auto Request = FHttpModule::Get().CreateRequest();
            Request->SetVerb("GET");
            Request->SetURL(CachedAvatarUrl);
            Request->OnProcessRequestComplete().BindThreadSafeSP(
                this,
                &FOnlineAvatarInterfaceSynthetic::OnDownloadAvatarComplete,
                MoveTemp(DefaultTexture),
                MoveTemp(OnComplete));
            Request->ProcessRequest();
            return true;
        }

        // Otherwise, targeting a native user ID. Just see if we have an IOnlineAvatar implementation for it.
        if (this->DelegatedAvatar.Contains(TargetUserId.GetType()) &&
            this->DelegatedIdentity.Contains(TargetUserId.GetType()))
        {
            if (auto WrappedIdentity = this->DelegatedIdentity[TargetUserId.GetType()].Implementation.Pin())
            {
                TSharedPtr<const FUniqueNetId> WrappedLocalUserId = WrappedIdentity->GetUniquePlayerId(LocalUserNum);
                if (WrappedLocalUserId.IsValid())
                {
                    if (auto WrappedAvatar = this->DelegatedAvatar[TargetUserId.GetType()].Implementation.Pin())
                    {
                        WrappedAvatar->GetAvatar(*WrappedLocalUserId, TargetUserId, DefaultTexture, OnComplete);
                        return true;
                    }
                }
            }
        }
    }

    OnComplete.ExecuteIfBound(false, DefaultTexture);
    return true;
}

bool FOnlineAvatarInterfaceSynthetic::GetAvatarUrl(
    const FUniqueNetId &LocalUserId,
    const FUniqueNetId &TargetUserId,
    FString DefaultAvatarUrl,
    FOnGetAvatarUrlComplete OnComplete)
{
    if (!this->bInitialized)
    {
        this->DelegatedIdentity = FDelegatedSubsystems::GetDelegatedInterfaces<IOnlineIdentity>(
            this->Config.ToSharedRef(),
            this->InstanceName,
            [](IOnlineSubsystem *OSS) {
                return OSS->GetIdentityInterface();
            });
        this->DelegatedAvatar = FDelegatedSubsystems::GetDelegatedInterfaces<IOnlineAvatar>(
            this->Config.ToSharedRef(),
            this->InstanceName,
            [](IOnlineSubsystem *OSS) {
                return Online::GetAvatarInterface(OSS);
            });
        this->bInitialized = true;
    }

    if (LocalUserId.GetType() != REDPOINT_EOS_SUBSYSTEM)
    {
        UE_LOG(LogEOS, Error, TEXT("LocalUserId is not an EOS user when calling GetAvatarUrl."));
        OnComplete.ExecuteIfBound(false, DefaultAvatarUrl);
        return true;
    }

    int32 LocalUserNum;
    if (!this->Identity->GetLocalUserNum(LocalUserId, LocalUserNum))
    {
        UE_LOG(LogEOS, Error, TEXT("LocalUserId is not a locally signed in EOS user."));
        OnComplete.ExecuteIfBound(false, DefaultAvatarUrl);
        return true;
    }

    if (TargetUserId.GetType() == REDPOINT_EOS_SUBSYSTEM)
    {
        // Target user might be a local user.
        for (const auto &Account : this->Identity->GetAllUserAccounts())
        {
            if (*Account->GetUserId() == TargetUserId)
            {
                // Found the user as a local user.
                for (const auto &WrappedTargetUserId :
                     StaticCastSharedPtr<FUserOnlineAccountEOS>(Account)->GetExternalUserIds())
                {
                    if (this->DelegatedAvatar.Contains(WrappedTargetUserId->GetType()) &&
                        this->DelegatedIdentity.Contains(WrappedTargetUserId->GetType()))
                    {
                        if (auto WrappedIdentity =
                                this->DelegatedIdentity[WrappedTargetUserId->GetType()].Implementation.Pin())
                        {
                            TSharedPtr<const FUniqueNetId> WrappedLocalUserId =
                                WrappedIdentity->GetUniquePlayerId(LocalUserNum);
                            if (WrappedLocalUserId.IsValid())
                            {
                                if (auto WrappedAvatar =
                                        this->DelegatedAvatar[WrappedTargetUserId->GetType()].Implementation.Pin())
                                {
                                    WrappedAvatar->GetAvatarUrl(
                                        *WrappedLocalUserId,
                                        *WrappedTargetUserId,
                                        DefaultAvatarUrl,
                                        OnComplete);
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }

        // Target user might be a friend.
        TSharedPtr<FOnlineFriend> Friend = this->Friends->GetFriend(LocalUserNum, TargetUserId, TEXT(""));
        if (Friend.IsValid() && Friend->GetUserId()->GetType() == REDPOINT_EOS_SUBSYSTEM)
        {
            // Found the user as a friend.
            for (const auto &WrappedFriend : StaticCastSharedPtr<FOnlineFriendSynthetic>(Friend)->GetWrappedFriends())
            {
                // The wrapped friend might be a *cached* friend, in which case we get the avatar URL from the
                // user attributes, rather than calling into a delegated subsystem which is not available.
                FString IsCached, CachedAvatarUrl;
                if (WrappedFriend.Value.IsValid() &&
                    WrappedFriend.Value->GetUserAttribute(TEXT("isCachedFriend"), IsCached) &&
                    IsCached == TEXT("true") &&
                    WrappedFriend.Value->GetUserAttribute(TEXT("cachedAvatarUrl"), CachedAvatarUrl))
                {
                    OnComplete.ExecuteIfBound(true, CachedAvatarUrl);
                    return true;
                }
                else if (
                    this->DelegatedAvatar.Contains(WrappedFriend.Key) &&
                    this->DelegatedIdentity.Contains(WrappedFriend.Key))
                {
                    if (auto WrappedIdentity = this->DelegatedIdentity[WrappedFriend.Key].Implementation.Pin())
                    {
                        TSharedPtr<const FUniqueNetId> WrappedLocalUserId =
                            WrappedIdentity->GetUniquePlayerId(LocalUserNum);
                        if (WrappedLocalUserId.IsValid())
                        {
                            if (auto WrappedAvatar = this->DelegatedAvatar[WrappedFriend.Key].Implementation.Pin())
                            {
                                WrappedAvatar->GetAvatarUrl(
                                    *WrappedLocalUserId,
                                    *WrappedFriend.Value->GetUserId(),
                                    DefaultAvatarUrl,
                                    OnComplete);
                                return true;
                            }
                        }
                    }
                }
            }
        }

        // @todo: Query on-demand with IOnlineUser.

        // No way to fetch the user's avatar.
        OnComplete.ExecuteIfBound(false, DefaultAvatarUrl);
        return true;
    }
    else
    {
        // If the target user is a friend, they might be a *cached* friend, in which case we get the avatar URL
        // from the user attributes, rather than calling into a delegated subsystem which is not available.
        TSharedPtr<FOnlineFriend> Friend = this->Friends->GetFriend(LocalUserNum, TargetUserId, TEXT(""));
        FString IsCached, CachedAvatarUrl;
        if (Friend.IsValid() && Friend->GetUserAttribute(TEXT("isCachedFriend"), IsCached) &&
            IsCached == TEXT("true") && Friend->GetUserAttribute(TEXT("cachedAvatarUrl"), CachedAvatarUrl))
        {
            OnComplete.ExecuteIfBound(true, CachedAvatarUrl);
            return true;
        }

        // Otherwise, targeting a native user ID. Just see if we have an IOnlineAvatar implementation for it.
        if (this->DelegatedAvatar.Contains(TargetUserId.GetType()) &&
            this->DelegatedIdentity.Contains(TargetUserId.GetType()))
        {
            if (auto WrappedIdentity = this->DelegatedIdentity[TargetUserId.GetType()].Implementation.Pin())
            {
                TSharedPtr<const FUniqueNetId> WrappedLocalUserId = WrappedIdentity->GetUniquePlayerId(LocalUserNum);
                if (WrappedLocalUserId.IsValid())
                {
                    if (auto WrappedAvatar = this->DelegatedAvatar[TargetUserId.GetType()].Implementation.Pin())
                    {
                        WrappedAvatar->GetAvatarUrl(*WrappedLocalUserId, TargetUserId, DefaultAvatarUrl, OnComplete);
                        return true;
                    }
                }
            }
        }
    }

    OnComplete.ExecuteIfBound(false, DefaultAvatarUrl);
    return true;
}

EOS_DISABLE_STRICT_WARNINGS

#endif