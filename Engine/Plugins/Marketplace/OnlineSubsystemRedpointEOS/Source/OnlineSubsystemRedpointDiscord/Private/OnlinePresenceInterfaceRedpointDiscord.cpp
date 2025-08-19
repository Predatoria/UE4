// Copyright June Rhodes. All Rights Reserved.

#include "OnlinePresenceInterfaceRedpointDiscord.h"

#if EOS_DISCORD_ENABLED

#include "OnlineFriendsInterfaceRedpointDiscord.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"

void FOnlinePresenceInterfaceRedpointDiscord::ConnectFriendsToPresence()
{
    this->Friends->Presence = this->AsShared();
}

void FOnlinePresenceInterfaceRedpointDiscord::DisconnectFriendsFromPresence()
{
    this->Friends->Presence.Reset();
}

TSharedPtr<FOnlineUserPresenceRedpointDiscord> FOnlinePresenceInterfaceRedpointDiscord::
    GetOrCreatePresenceInfoForDiscordId(const TSharedRef<const FUniqueNetIdRedpointDiscord> &InDiscordId)
{
    if (this->PresenceByDiscordId.Contains(*InDiscordId))
    {
        return this->PresenceByDiscordId[*InDiscordId];
    }

    this->PresenceByDiscordId.Add(*InDiscordId, MakeShared<FOnlineUserPresenceRedpointDiscord>());
    return this->PresenceByDiscordId[*InDiscordId];
}

FOnlinePresenceInterfaceRedpointDiscord::FOnlinePresenceInterfaceRedpointDiscord(
    const TSharedRef<FOnlineIdentityInterfaceRedpointDiscord, ESPMode::ThreadSafe> &InIdentity,
    const TSharedRef<class FOnlineFriendsInterfaceRedpointDiscord, ESPMode::ThreadSafe> &InFriends)
    : Identity(InIdentity)
    , Friends(InFriends)
    , PresenceByDiscordId()
{
}

void FOnlinePresenceInterfaceRedpointDiscord::SetPresence(
    const FUniqueNetId &User,
    const FOnlineUserPresenceStatus &Status,
    const FOnPresenceTaskCompleteDelegate &Delegate)
{
    // Ignore requests to set custom presence on Discord right now.
    Delegate.ExecuteIfBound(User, true);
}

void FOnlinePresenceInterfaceRedpointDiscord::QueryPresence(
    const FUniqueNetId &User,
    const FOnPresenceTaskCompleteDelegate &Delegate)
{
    if (this->Friends->bFriendsLoaded)
    {
        Delegate.ExecuteIfBound(User, true);
        return;
    }

    this->Friends->ReadFriendsList(
        0,
        TEXT(""),
        FOnReadFriendsListComplete::CreateLambda(
            [WeakThis = GetWeakThis(this),
             UserShared = User.AsShared(),
             Delegate](int32 LocalUserNum, bool bWasSuccessful, const FString &ListName, const FString &ErrorStr) {
                if (auto This = PinWeakThis(WeakThis))
                {
                    Delegate.ExecuteIfBound(*UserShared, true);
                }
            }));
}

EOnlineCachedResult::Type FOnlinePresenceInterfaceRedpointDiscord::GetCachedPresence(
    const FUniqueNetId &User,
    TSharedPtr<FOnlineUserPresence> &OutPresence)
{
    if (this->PresenceByDiscordId.Contains(User))
    {
        OutPresence = this->PresenceByDiscordId[User];
        return EOnlineCachedResult::Success;
    }

    OutPresence = nullptr;
    return EOnlineCachedResult::NotFound;
}

EOnlineCachedResult::Type FOnlinePresenceInterfaceRedpointDiscord::GetCachedPresenceForApp(
    const FUniqueNetId &LocalUserId,
    const FUniqueNetId &User,
    const FString &AppId,
    TSharedPtr<FOnlineUserPresence> &OutPresence)
{
    return this->GetCachedPresence(User, OutPresence);
}

#endif