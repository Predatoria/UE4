// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Shared/OnlinePresenceInterfaceSynthetic.h"

#include "Interfaces/OnlineFriendsInterface.h"
#include "OnlineError.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSError.h"
#include "OnlineSubsystemRedpointEOS/Shared/DelegatedSubsystems.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSCommon.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSConfig.h"
#include "OnlineSubsystemRedpointEOS/Shared/EOSErrorConv.h"
#include "OnlineSubsystemRedpointEOS/Shared/MultiOperation.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineFriendSynthetic.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineFriendsInterfaceSynthetic.h"
#include "OnlineSubsystemRedpointEOS/Shared/OnlineIdentityInterfaceEOS.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

EOS_ENABLE_STRICT_WARNINGS

FOnlinePresenceInterfaceSynthetic::FOnlinePresenceInterfaceSynthetic(
    FName InInstanceName,
    TSharedPtr<FOnlineIdentityInterfaceEOS, ESPMode::ThreadSafe> InIdentity,
    const TSharedRef<FOnlineFriendsInterfaceSynthetic, ESPMode::ThreadSafe> &InFriends,
    const IOnlineSubsystemPtr &InSubsystemEAS,
    const TSharedRef<FEOSConfig> &InConfig)
{
    this->Config = InConfig;
    this->IdentityEOS = MoveTemp(InIdentity);
    this->FriendsSynthetic = InFriends;

    if (InSubsystemEAS != nullptr)
    {
        this->WrappedSubsystems.Add(
            {REDPOINT_EAS_SUBSYSTEM, InSubsystemEAS->GetIdentityInterface(), InSubsystemEAS->GetPresenceInterface()});
    }
    struct W
    {
        TWeakPtr<IOnlineIdentity, ESPMode::ThreadSafe> Identity;
        TWeakPtr<IOnlinePresence, ESPMode::ThreadSafe> Presence;
    };
    for (const auto &KV : FDelegatedSubsystems::GetDelegatedSubsystems<W>(
             InConfig,
             InInstanceName,
             [](FDelegatedSubsystems::ISubsystemContext &Ctx) -> TSharedPtr<W> {
                 auto Identity = Ctx.GetDelegatedInterface<IOnlineIdentity>([](IOnlineSubsystem *OSS) {
                     return OSS->GetIdentityInterface();
                 });
                 auto Presence = Ctx.GetDelegatedInterface<IOnlinePresence>([](IOnlineSubsystem *OSS) {
                     return OSS->GetPresenceInterface();
                 });
                 if (Identity.IsValid() && Presence.IsValid())
                 {
                     return MakeShared<W>(W{Identity, Presence});
                 }
                 return nullptr;
             }))
    {
        this->WrappedSubsystems.Add({KV.Key, KV.Value->Identity, KV.Value->Presence});
    }
}

void FOnlinePresenceInterfaceSynthetic::RegisterEvents()
{
    for (auto Wk : this->WrappedSubsystems)
    {
        if (auto Presence = Wk.Presence.Pin())
        {
            this->OnPresenceReceivedDelegates.Add(TTuple<FWrappedSubsystem, FDelegateHandle>(
                Wk,
                Presence->AddOnPresenceReceivedDelegate_Handle(FOnPresenceReceivedDelegate::CreateLambda(
                    [WeakThis = GetWeakThis(this),
                     Wk](const class FUniqueNetId &UserId, const TSharedRef<FOnlineUserPresence> &Presence) {
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            if (auto Identity = Wk.Identity.Pin())
                            {
                                UE_LOG(
                                    LogEOS,
                                    Verbose,
                                    TEXT("FOnlinePresenceInterfaceSynthetic: Forwarding OnPresenceReceived event"));
                                // The UserId is the user ID from the wrapped subsystem, not EOS. We need to try to map
                                // the native user ID back to an EOS ID if possible.
                                TSharedPtr<const FUniqueNetIdEOS> EOSId = nullptr;
                                for (int i = 0; i < MAX_LOCAL_PLAYERS; i++)
                                {
                                    TSharedPtr<const FUniqueNetId> WrappedId = Identity->GetUniquePlayerId(i);
                                    if (WrappedId.IsValid() && *WrappedId == UserId)
                                    {
                                        if (This->IdentityEOS->GetLoginStatus(i) == ELoginStatus::LoggedIn)
                                        {
                                            EOSId = StaticCastSharedPtr<const FUniqueNetIdEOS>(
                                                This->IdentityEOS->GetUniquePlayerId(i));
                                            if (EOSId.IsValid())
                                            {
                                                break;
                                            }
                                        }
                                    }
                                }
                                if (!EOSId.IsValid())
                                {
                                    EOSId = This->FriendsSynthetic->TryResolveNativeIdToEOS(UserId);
                                }
                                if (EOSId.IsValid())
                                {
                                    This->TriggerOnPresenceReceivedDelegates(*EOSId, Presence);
                                }
                                else
                                {
                                    This->TriggerOnPresenceReceivedDelegates(UserId, Presence);
                                }
                            }
                        }
                    }))));
            this->OnPresenceArrayUpdatedDelegates.Add(TTuple<FWrappedSubsystem, FDelegateHandle>(
                Wk,
                Presence->AddOnPresenceArrayUpdatedDelegate_Handle(FOnPresenceArrayUpdatedDelegate::CreateLambda(
                    [WeakThis = GetWeakThis(this), Wk](
                        const class FUniqueNetId &UserId,
                        const TArray<TSharedRef<FOnlineUserPresence>> &PresenceArray) {
                        if (auto This = PinWeakThis(WeakThis))
                        {
                            if (auto Identity = Wk.Identity.Pin())
                            {
                                UE_LOG(
                                    LogEOS,
                                    Verbose,
                                    TEXT("FOnlinePresenceInterfaceSynthetic: Forwarding OnPresenceArrayUpdated "
                                         "event"));
                                // The UserId is the user ID from the wrapped subsystem, not EOS. We need to try to map
                                // the native user ID back to an EOS ID if possible.
                                TSharedPtr<const FUniqueNetIdEOS> EOSId = nullptr;
                                for (int i = 0; i < MAX_LOCAL_PLAYERS; i++)
                                {
                                    TSharedPtr<const FUniqueNetId> WrappedId = Identity->GetUniquePlayerId(i);
                                    if (WrappedId.IsValid() && *WrappedId == UserId)
                                    {
                                        if (This->IdentityEOS->GetLoginStatus(i) == ELoginStatus::LoggedIn)
                                        {
                                            EOSId = StaticCastSharedPtr<const FUniqueNetIdEOS>(
                                                This->IdentityEOS->GetUniquePlayerId(i));
                                            if (EOSId.IsValid())
                                            {
                                                break;
                                            }
                                        }
                                    }
                                }
                                if (!EOSId.IsValid())
                                {
                                    EOSId = This->FriendsSynthetic->TryResolveNativeIdToEOS(UserId);
                                }
                                if (EOSId.IsValid())
                                {
                                    This->TriggerOnPresenceArrayUpdatedDelegates(*EOSId, PresenceArray);
                                }
                                else
                                {
                                    This->TriggerOnPresenceArrayUpdatedDelegates(UserId, PresenceArray);
                                }
                            }
                        }
                    }))));
        }
    }
}

FOnlinePresenceInterfaceSynthetic::~FOnlinePresenceInterfaceSynthetic()
{
    for (auto V : this->OnPresenceReceivedDelegates)
    {
        if (auto Ptr = V.Get<0>().Presence.Pin())
        {
            Ptr->ClearOnPresenceReceivedDelegate_Handle(V.Get<1>());
        }
    }
    this->OnPresenceReceivedDelegates.Empty();

    for (auto V : this->OnPresenceArrayUpdatedDelegates)
    {
        if (auto Ptr = V.Get<0>().Presence.Pin())
        {
            Ptr->ClearOnPresenceArrayUpdatedDelegate_Handle(V.Get<1>());
        }
    }
    this->OnPresenceArrayUpdatedDelegates.Empty();
}

void FOnlinePresenceInterfaceSynthetic::SetPresence(
    const FUniqueNetId &User,
    const FOnlineUserPresenceStatus &Status,
    const FOnPresenceTaskCompleteDelegate &Delegate)
{
    if (this->WrappedSubsystems.Num() == 0)
    {
        FOnlineError Err = OnlineRedpointEOS::Errors::NotConfigured(
            TEXT("FOnlinePresenceInterfaceSynthetic::SetPresence"),
            TEXT("There are no delegated subsystems to forward presence to."));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
        Delegate.ExecuteIfBound(User, false);
        return;
    }

    // Work out the local user num for the user, because this is what aligns local users in the other subsystems to EOS.
    int32 LocalUserNum;
    if (!this->IdentityEOS->GetLocalUserNum(User, LocalUserNum))
    {
        FOnlineError Err = OnlineRedpointEOS::Errors::InvalidUser(
            TEXT("FOnlinePresenceInterfaceSynthetic::SetPresence"),
            TEXT("The specified local user ID is not a locally signed in user."));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
        Delegate.ExecuteIfBound(User, false);
        return;
    }

    // For every subsystem that we are delegating to, set the presence for the same user based on the local user num.
    FMultiOperation<FWrappedSubsystem, bool>::Run(
        this->WrappedSubsystems,
        [LocalUserNum, Status](const FWrappedSubsystem &InSubsystem, const std::function<void(bool OutValue)> &OnDone) {
            if (auto Presence = InSubsystem.Presence.Pin())
            {
                if (auto Identity = InSubsystem.Identity.Pin())
                {
                    TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(LocalUserNum);
                    if (UserId.IsValid())
                    {
                        Presence->SetPresence(
                            *UserId,
                            Status,
                            FOnPresenceTaskCompleteDelegate::CreateLambda(
                                [OnDone](const class FUniqueNetId &UserId, const bool bWasSuccessful) {
                                    OnDone(bWasSuccessful);
                                }));
                        return true;
                    }
                }
            }

            return false;
        },
        [Delegate, UserRef = User.AsShared()](const TArray<bool> &OutValues) {
            for (auto Result : OutValues)
            {
                if (Result)
                {
                    Delegate.ExecuteIfBound(*UserRef, true);
                    return;
                }
            }

            FOnlineError Err = OnlineRedpointEOS::Errors::UnexpectedError(
                TEXT("FOnlinePresenceInterfaceSynthetic::SetPresence"),
                TEXT("All delegated subsystems returned an error while trying to set presence."));
            UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
            Delegate.ExecuteIfBound(*UserRef, false);
        });
    return;
}

void FOnlinePresenceInterfaceSynthetic::QueryPresence(
    const FUniqueNetId &User,
    const FOnPresenceTaskCompleteDelegate &Delegate)
{
    if (this->WrappedSubsystems.Num() == 0)
    {
        FOnlineError Err = OnlineRedpointEOS::Errors::NotConfigured(
            TEXT("FOnlinePresenceInterfaceSynthetic::QueryPresence"),
            TEXT("There are no delegated subsystems to query presence on."));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
        Delegate.ExecuteIfBound(User, false);
        return;
    }

    // Work out the local user num for the user, because this is what aligns local users in the other subsystems to EOS.
    int32 LocalUserNum;
    if (!this->IdentityEOS->GetLocalUserNum(User, LocalUserNum))
    {
        FOnlineError Err = OnlineRedpointEOS::Errors::InvalidUser(
            TEXT("FOnlinePresenceInterfaceSynthetic::QueryPresence"),
            TEXT("The specified user ID is not a locally signed in user. The 2-argument version of QueryPresence "
                 "queries presence of a user's friends, so the passed user ID must be a user that is locally signed "
                 "in."));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
        Delegate.ExecuteIfBound(User, false);
        return;
    }

    // Read the friends list for that local user, and then query presence of all
    // their friends.
    if (!this->FriendsSynthetic->ReadFriendsList(
            LocalUserNum,
            TEXT(""),
            FOnReadFriendsListComplete::CreateLambda([WeakThis = GetWeakThis(this), Delegate, User = User.AsShared()](
                                                         int32 LocalUserNum,
                                                         bool bWasSuccessful,
                                                         const FString &ListName,
                                                         const FString &ErrorStr) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    if (!bWasSuccessful)
                    {
                        FOnlineError Err = OnlineRedpointEOS::Errors::UnexpectedError(
                            TEXT("FOnlinePresenceInterfaceSynthetic::QueryPresence"),
                            TEXT("The underlying ReadFriendsList operation failed during the callback."));
                        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
                        Delegate.ExecuteIfBound(*User, false);
                        return;
                    }

                    TArray<TSharedRef<FOnlineFriend>> Friends;
                    if (!This->FriendsSynthetic->GetFriendsList(LocalUserNum, TEXT(""), Friends))
                    {
                        FOnlineError Err = OnlineRedpointEOS::Errors::UnexpectedError(
                            TEXT("FOnlinePresenceInterfaceSynthetic::QueryPresence"),
                            TEXT("The underlying GetFriendsList operation failed."));
                        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
                        Delegate.ExecuteIfBound(*User, false);
                        return;
                    }

                    TArray<TSharedRef<const FUniqueNetId>> FriendIds;
                    for (const auto &Friend : Friends)
                    {
                        FriendIds.Add(Friend->GetUserId());
                    }

                    // Forward to 3-argument version.
                    This->QueryPresence(*User, FriendIds, Delegate);
                }
            })))
    {
        FOnlineError Err = OnlineRedpointEOS::Errors::UnexpectedError(
            TEXT("FOnlinePresenceInterfaceSynthetic::QueryPresence"),
            TEXT("The underlying ReadFriendsList operation failed to start."));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
        Delegate.ExecuteIfBound(User, false);
        return;
    }
}

void FOnlinePresenceInterfaceSynthetic::QueryPresence(
    const FUniqueNetId &LocalUserId,
    const TArray<TSharedRef<const FUniqueNetId>> &UserIds,
    const FOnPresenceTaskCompleteDelegate &Delegate)
{
    if (this->WrappedSubsystems.Num() == 0)
    {
        FOnlineError Err = OnlineRedpointEOS::Errors::NotConfigured(
            TEXT("FOnlinePresenceInterfaceSynthetic::QueryPresence"),
            TEXT("There are no delegated subsystems to query presence on."));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
        Delegate.ExecuteIfBound(LocalUserId, false);
        return;
    }

    // Work out the local user num for the user, because this is what aligns local users in the other subsystems to EOS.
    int32 LocalUserNum;
    if (!this->IdentityEOS->GetLocalUserNum(LocalUserId, LocalUserNum))
    {
        FOnlineError Err = OnlineRedpointEOS::Errors::InvalidUser(
            TEXT("FOnlinePresenceInterfaceSynthetic::QueryPresence"),
            TEXT("The specified local user ID is not a locally signed in user."));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
        Delegate.ExecuteIfBound(LocalUserId, false);
        return;
    }

    // Get the friends list for the local user. We don't need to call ReadFriendsList, because
    // if we have EOS user IDs passed to us that are synthetic friends, then the caller must have
    // already called ReadFriendsList.
    TArray<TSharedRef<FOnlineFriend>> Friends;
    this->FriendsSynthetic->GetFriendsList(LocalUserNum, TEXT(""), Friends);
    TUserIdMap<TSharedRef<FOnlineFriend>> FriendsByUserId;
    for (const auto &Friend : Friends)
    {
        FriendsByUserId.Add(*Friend->GetUserId(), Friend);
    }

    // We have to filter the user IDs by their subsystem, so only user IDs belonging
    // to the given subsystem have presence queried by that subsystem.
    struct FEntry
    {
        FWrappedSubsystem WrappedSubsystemInt;
        TArray<TSharedRef<const FUniqueNetId>> UserIds;
    };
    TArray<FEntry> EntriesToProcess;
    for (int i = 0; i < this->WrappedSubsystems.Num(); i++)
    {
        TArray<TSharedRef<const FUniqueNetId>> OSSUserIds;

        for (const auto &UserId : UserIds)
        {
            // For any user IDs which are directly from the target subsystem, add them.
            // These will be user IDs of friends from the friend list, where those friends don't
            // have an EOS account.
            if (UserId->GetType().IsEqual(this->WrappedSubsystems[i].SubsystemName))
            {
                OSSUserIds.Add(UserId);
            }

            // If the user ID is an EOS user ID, and the friends list has this user ID, then
            // this is a synthetic friend, and we need to inspect all of the wrapped friends.
            if (UserId->GetType().IsEqual(REDPOINT_EOS_SUBSYSTEM))
            {
                if (FriendsByUserId.Contains(*UserId))
                {
                    auto SyntheticFriend = StaticCastSharedRef<const FOnlineFriendSynthetic>(FriendsByUserId[*UserId]);
                    for (const auto &KV : SyntheticFriend->GetWrappedFriends())
                    {
                        if (KV.Value->GetUserId()->GetType().IsEqual(this->WrappedSubsystems[i].SubsystemName))
                        {
                            OSSUserIds.Add(KV.Value->GetUserId());
                        }
                    }
                }
            }
        }

        FEntry Entry = {};
        Entry.WrappedSubsystemInt = this->WrappedSubsystems[i];
        Entry.UserIds = OSSUserIds;
        EntriesToProcess.Add(Entry);
    }

    FMultiOperation<FEntry, bool>::Run(
        EntriesToProcess,
        [LocalUserNum](const FEntry &InEntry, const std::function<void(bool OutValue)> &OnDone) {
            if (auto Presence = InEntry.WrappedSubsystemInt.Presence.Pin())
            {
                if (auto Identity = InEntry.WrappedSubsystemInt.Identity.Pin())
                {
                    TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(LocalUserNum);
                    if (UserId.IsValid())
                    {
#if defined(UE_4_26_OR_LATER)
                        Presence->QueryPresence(
                            *UserId,
                            InEntry.UserIds,
                            FOnPresenceTaskCompleteDelegate::CreateLambda(
                                [OnDone](const class FUniqueNetId &UserId, const bool bWasSuccessful) {
                                    OnDone(bWasSuccessful);
                                }));
#else
                        Presence->QueryPresence(
                            *UserId,
                            FOnPresenceTaskCompleteDelegate::CreateLambda(
                                [OnDone](const class FUniqueNetId &UserId, const bool bWasSuccessful) {
                                    OnDone(bWasSuccessful);
                                }));
#endif
                    }

                    return true;
                }
            }

            return false;
        },
        [Delegate, UserRef = LocalUserId.AsShared()](const TArray<bool> &OutValues) {
            for (auto Result : OutValues)
            {
                if (Result)
                {
                    Delegate.ExecuteIfBound(UserRef.Get(), true);
                    return;
                }
            }

            FOnlineError Err = OnlineRedpointEOS::Errors::UnexpectedError(
                TEXT("FOnlinePresenceInterfaceSynthetic::QueryPresence"),
                TEXT("All delegated subsystems returned an error while trying to query presence information."));
            UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
            Delegate.ExecuteIfBound(UserRef.Get(), false);
        });
}

EOnlineCachedResult::Type FOnlinePresenceInterfaceSynthetic::GetCachedPresence(
    const FUniqueNetId &User,
    TSharedPtr<FOnlineUserPresence> &OutPresence)
{
    if (User.GetType().IsEqual(REDPOINT_EOS_SUBSYSTEM))
    {
        // This is a synthetic friend. We don't know the local user in the GetCachedPresence call, so
        // we have to iterate through all locally signed in accounts, and scan their cached friends list
        // to find the synthetic friend object.
        for (const auto &Acc : this->IdentityEOS->GetAllUserAccounts())
        {
            int32 LocalUserNum;
            if (!this->IdentityEOS->GetLocalUserNum(*Acc->GetUserId(), LocalUserNum))
            {
                continue;
            }
            TArray<TSharedRef<FOnlineFriend>> Friends;
            if (!this->FriendsSynthetic->GetFriendsList(LocalUserNum, TEXT(""), Friends))
            {
                continue;
            }
            for (const auto &Friend : Friends)
            {
                if (*Friend->GetUserId() == User)
                {
                    // Make a copy of the presence object, so we can return the shared pointer.
                    OutPresence = MakeShared<FOnlineUserPresence>(Friend->GetPresence());
                    return EOnlineCachedResult::Success;
                }
            }
        }

        FOnlineError Err = OnlineRedpointEOS::Errors::InvalidRequest(
            TEXT("FOnlinePresenceInterfaceSynthetic::GetCachedPresence"),
            FString::Printf(
                TEXT("The passed EOS user ID %s was not found on any local user's friend list. For EOS IDs, you can "
                     "only query the presence of friends."),
                *User.ToString()));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
    }
    else
    {
        for (int i = 0; i < this->WrappedSubsystems.Num(); i++)
        {
            if (User.GetType().IsEqual(this->WrappedSubsystems[i].SubsystemName))
            {
                if (auto Presence = this->WrappedSubsystems[i].Presence.Pin())
                {
                    return Presence->GetCachedPresence(User, OutPresence);
                }
            }
        }
    }

    return EOnlineCachedResult::NotFound;
}

EOnlineCachedResult::Type FOnlinePresenceInterfaceSynthetic::GetCachedPresenceForApp(
    const FUniqueNetId &LocalUserId,
    const FUniqueNetId &User,
    const FString &AppId,
    TSharedPtr<FOnlineUserPresence> &OutPresence)
{
    if (User.GetType().IsEqual(REDPOINT_EOS_SUBSYSTEM))
    {
        // This is a synthetic friend. We have to scan their cached friends list
        // to find the synthetic friend object.
        int32 LocalUserNum;
        if (!this->IdentityEOS->GetLocalUserNum(LocalUserId, LocalUserNum))
        {
            FOnlineError Err = OnlineRedpointEOS::Errors::InvalidUser(
                TEXT("FOnlinePresenceInterfaceSynthetic::GetCachedPresenceForApp"),
                TEXT("The local user is not currently signed in."));
            UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
            return EOnlineCachedResult::NotFound;
        }
        TArray<TSharedRef<FOnlineFriend>> Friends;
        if (!this->FriendsSynthetic->GetFriendsList(LocalUserNum, TEXT(""), Friends))
        {
            FOnlineError Err = OnlineRedpointEOS::Errors::InvalidState(
                TEXT("FOnlinePresenceInterfaceSynthetic::GetCachedPresenceForApp"),
                TEXT("The GetFriendsList call failed for the local user. Make sure you've called ReadFriendsList at "
                     "least once before calling GetCachedPresenceForApp."));
            UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
            return EOnlineCachedResult::NotFound;
        }
        for (const auto &Friend : Friends)
        {
            if (*Friend->GetUserId() == User)
            {
                // Make a copy of the presence object, so we can return the shared pointer.
                OutPresence = MakeShared<FOnlineUserPresence>(Friend->GetPresence());
                return EOnlineCachedResult::Success;
            }
        }

        FOnlineError Err = OnlineRedpointEOS::Errors::InvalidRequest(
            TEXT("FOnlinePresenceInterfaceSynthetic::GetCachedPresenceForApp"),
            FString::Printf(
                TEXT("The passed EOS user ID %s was not found on the local user's friend list. For EOS IDs, you can "
                     "only query the presence of friends."),
                *User.ToString()));
        UE_LOG(LogEOS, Error, TEXT("%s"), *Err.ToLogString());
    }
    else
    {
        for (int i = 0; i < this->WrappedSubsystems.Num(); i++)
        {
            if (User.GetType().IsEqual(this->WrappedSubsystems[i].SubsystemName))
            {
                if (auto Presence = this->WrappedSubsystems[i].Presence.Pin())
                {
                    return Presence->GetCachedPresenceForApp(LocalUserId, User, AppId, OutPresence);
                }
            }
        }
    }

    return EOnlineCachedResult::NotFound;
}

EOS_DISABLE_STRICT_WARNINGS
