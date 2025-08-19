// Copyright June Rhodes. All Rights Reserved.

#include "OnlineSubsystemRedpointEOS/Private/NetworkingAuth/PhasesVerification/LegacyIdentityCheckPhase.h"

#include "Interfaces/OnlineUserInterface.h"
#include "OnlineSubsystemRedpointEOS/Private/EOSControlChannel.h"

void FLegacyIdentityCheckPhase::RegisterRoutes(class UEOSControlChannel *ControlChannel)
{
}

void FLegacyIdentityCheckPhase::Start(const TSharedRef<FAuthVerificationPhaseContext> &Context)
{
    Context->SetVerificationStatus(EUserVerificationStatus::CheckingAccountExistsFromListenServer);

    FOnlineSubsystemEOS *OSS = Context->GetOSS();
    if (OSS == nullptr)
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Server authentication: %s: Disconnecting client because we could not access our online "
                 "subsystem."),
            *Context->GetUserId()->ToString());
        Context->Finish(EAuthPhaseFailureCode::All_CanNotAccessOSS);
        return;
    }

    this->UserInfoHandle = OSS->GetUserInterface()->AddOnQueryUserInfoCompleteDelegate_Handle(
        0,
        FOnQueryUserInfoCompleteDelegate::CreateSP(this, &FLegacyIdentityCheckPhase::OnUserInfoReceived, Context));
    TArray<TSharedRef<const FUniqueNetId>> UserIds;
    UserIds.Add(Context->GetUserId()->AsShared());
    if (!OSS->GetUserInterface()->QueryUserInfo(0, UserIds))
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Server authentication: %s: Disconnecting client because we could not call "
                 "IOnlineUser::QueryUserInfo."),
            *Context->GetUserId()->ToString());
        Context->Finish(EAuthPhaseFailureCode::Phase_LegacyIdentityCheck_CanNotCallUserInfo);
        return;
    }
}

void FLegacyIdentityCheckPhase::OnUserInfoReceived(
    int32 LocalUserNum,
    bool bWasSuccessful,
    const TArray<TSharedRef<const FUniqueNetId>> &UserIds,
    const FString &ErrorString,
    // NOLINTNEXTLINE(performance-unnecessary-value-param)
    TSharedRef<FAuthVerificationPhaseContext> Context)
{
    if (LocalUserNum != 0)
    {
        // Not the event we're interested in.
        return;
    }
    if (UserIds.Num() != 1 || *UserIds[0] != *Context->GetUserId())
    {
        // Not the event we're interested in.
        return;
    }

    FOnlineSubsystemEOS *OSS = Context->GetOSS();
    if (OSS == nullptr)
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Server authentication: %s: Disconnecting client because we could not access our online "
                 "subsystem."),
            *Context->GetUserId()->ToString());
        Context->Finish(EAuthPhaseFailureCode::All_CanNotAccessOSS);
        return;
    }

    OSS->GetUserInterface()->ClearOnQueryUserInfoCompleteDelegate_Handle(0, this->UserInfoHandle);

    if (!bWasSuccessful)
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Server authentication: %s: Disconnecting client because we could not obtain their user information: "
                 "%s."),
            *Context->GetUserId()->ToString(),
            *ErrorString);
        Context->Finish(EAuthPhaseFailureCode::Phase_LegacyIdentityCheck_UserAccountNotFound);
        return;
    }

    TSharedPtr<FOnlineUser> TargetUser = OSS->GetUserInterface()->GetUserInfo(0, *Context->GetUserId());
    if (!TargetUser.IsValid())
    {
        UE_LOG(
            LogEOSNetworkAuth,
            Error,
            TEXT("Server authentication: %s: Disconnecting client because the user account doesn't exist."),
            *Context->GetUserId()->ToString(),
            *ErrorString);
        Context->Finish(EAuthPhaseFailureCode::Phase_LegacyIdentityCheck_UserAccountNotFoundAfterLoad);
        return;
    }

    // We've done as much verification as we can on a listen server. Proceed to sanctions check or pass depending on EOS
    // SDK level.
    UE_LOG(
        LogEOSNetworkAuth,
        Verbose,
        TEXT("Server authentication: %s: Successfully found user with IOnlineUser::GetUserInfo."),
        *Context->GetUserId()->ToString());

    Context->GetConnection()->PlayerId = FUniqueNetIdRepl(Context->GetUserId()->AsShared());
    Context->Finish(EAuthPhaseFailureCode::Success);
}