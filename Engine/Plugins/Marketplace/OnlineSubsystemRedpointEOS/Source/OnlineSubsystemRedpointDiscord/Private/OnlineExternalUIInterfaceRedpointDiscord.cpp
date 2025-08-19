// Copyright June Rhodes. All Rights Reserved.

#include "OnlineExternalUIInterfaceRedpointDiscord.h"
#include "LogRedpointDiscord.h"
#include "OnlineSubsystemRedpointEOS/Shared/WeakPtrHelpers.h"
#include "UniqueNetIdRedpointDiscord.h"

#if EOS_DISCORD_ENABLED

FOnlineExternalUIInterfaceRedpointDiscord::FOnlineExternalUIInterfaceRedpointDiscord(
    TSharedRef<discord::Core> InInstance)
    : Instance(MoveTemp(InInstance))
    , OnCurrentUserUpdateHandle(0)
    , CurrentUser(nullptr)
    , OAuth2Token(nullptr)
    , PendingLoginCallbacks()
    , bAuthenticated(false)
{
}

FOnlineExternalUIInterfaceRedpointDiscord::~FOnlineExternalUIInterfaceRedpointDiscord()
{
    if (this->OnCurrentUserUpdateHandle != 0)
    {
        this->Instance->UserManager().OnCurrentUserUpdate.Disconnect(this->OnCurrentUserUpdateHandle);
    }
}

void FOnlineExternalUIInterfaceRedpointDiscord::RegisterEvents()
{
    this->OnCurrentUserUpdateHandle =
        this->Instance->UserManager().OnCurrentUserUpdate.Connect([WeakThis = GetWeakThis(this)]() {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (!This->CurrentUser.IsValid())
                {
                    This->CurrentUser = MakeShared<discord::User>();
                }

                This->Instance->UserManager().GetCurrentUser(This->CurrentUser.Get());
                checkf(This->CurrentUser != nullptr, TEXT("Current user was set to nullptr by Discord!"));

                if (This->OAuth2Token.IsValid())
                {
                    This->bAuthenticated = true;

                    // Fire any login callbacks that were waiting on current user information.
                    TArray<FOnLoginUIClosedDelegate> LoginCallbacks = This->PendingLoginCallbacks;
                    This->PendingLoginCallbacks.Empty();
                    for (const auto &Callback : LoginCallbacks)
                    {
                        Callback.ExecuteIfBound(
                            MakeShared<FUniqueNetIdRedpointDiscord>(This->CurrentUser->GetId()),
                            0,
                            FOnlineError::Success());
                    }
                }
            }
        });
}

bool FOnlineExternalUIInterfaceRedpointDiscord::ShowLoginUI(
    const int ControllerIndex,
    bool bShowOnlineOnly,
    bool bShowSkipButton,
    const FOnLoginUIClosedDelegate &Delegate)
{
    if (ControllerIndex != 0)
    {
        UE_LOG(LogRedpointDiscord, Error, TEXT("Only local player 0 can sign into Discord."));
        return false;
    }

    this->Instance->ApplicationManager().GetOAuth2Token(
        [WeakThis = GetWeakThis(this), Delegate](discord::Result TokenResult, const discord::OAuth2Token &Token) {
            if (auto This = PinWeakThis(WeakThis))
            {
                if (TokenResult == discord::Result::Ok)
                {
                    // Store the token.
                    This->OAuth2Token = MakeShared<discord::OAuth2Token>(Token);

                    // If we already have the current user, we are done.
                    if (This->CurrentUser.IsValid())
                    {
                        This->bAuthenticated = true;
                        Delegate.ExecuteIfBound(
                            MakeShared<FUniqueNetIdRedpointDiscord>(This->CurrentUser->GetId()),
                            0,
                            FOnlineError::Success());
                    }
                    else
                    {
                        // Otherwise store our callback onto an array which OnCurrentUserUpdate will fire once the
                        // current user data is also available.
                        This->PendingLoginCallbacks.Add(Delegate);
                    }
                }
                else
                {
                    Delegate.ExecuteIfBound(nullptr, 0, FOnlineError(EOnlineErrorResult::AccessDenied));
                }
            }
        });
    return true;
}

bool FOnlineExternalUIInterfaceRedpointDiscord::ShowAccountCreationUI(
    const int ControllerIndex,
    const FOnAccountCreationUIClosedDelegate &Delegate)
{
    return false;
}

bool FOnlineExternalUIInterfaceRedpointDiscord::ShowFriendsUI(int32 LocalUserNum)
{
    return false;
}

bool FOnlineExternalUIInterfaceRedpointDiscord::ShowInviteUI(int32 LocalUserNum, FName SessionName)
{
    return false;
}

bool FOnlineExternalUIInterfaceRedpointDiscord::ShowAchievementsUI(int32 LocalUserNum)
{
    return false;
}

bool FOnlineExternalUIInterfaceRedpointDiscord::ShowLeaderboardUI(const FString &LeaderboardName)
{
    return false;
}

bool FOnlineExternalUIInterfaceRedpointDiscord::ShowWebURL(
    const FString &Url,
    const FShowWebUrlParams &ShowParams,
    const FOnShowWebUrlClosedDelegate &Delegate)
{
    return false;
}

bool FOnlineExternalUIInterfaceRedpointDiscord::CloseWebURL()
{
    return false;
}

bool FOnlineExternalUIInterfaceRedpointDiscord::ShowProfileUI(
    const FUniqueNetId &Requestor,
    const FUniqueNetId &Requestee,
    const FOnProfileUIClosedDelegate &Delegate)
{
    return false;
}

bool FOnlineExternalUIInterfaceRedpointDiscord::ShowAccountUpgradeUI(const FUniqueNetId &UniqueId)
{
    return false;
}

bool FOnlineExternalUIInterfaceRedpointDiscord::ShowStoreUI(
    int32 LocalUserNum,
    const FShowStoreParams &ShowParams,
    const FOnShowStoreUIClosedDelegate &Delegate)
{
    return false;
}

bool FOnlineExternalUIInterfaceRedpointDiscord::ShowSendMessageUI(
    int32 LocalUserNum,
    const FShowSendMessageParams &ShowParams,
    const FOnShowSendMessageUIClosedDelegate &Delegate)
{
    return false;
}

#endif